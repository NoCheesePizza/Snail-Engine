#include "Components.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <iterator>
#include <array>
#include <optional>
#include <stack>
#include <iostream> // debugging

namespace Snail
{

	bool ShapeComponent::ifIsConvex(const Line &line) const
	{
		std::vector<Vec2> positions;
		for (unsigned i = 0; i < static_cast<unsigned>(vertices.size()); ++i)
			if (i != edges[*line.edge].p1 && i != edges[*line.edge].p2
				&& vertices[i].type != VertexType::REMOVED)
				positions.push_back(vertices[i].pos);
		return line.ifIsOnOneSide(positions);
	}

	bool ShapeComponent::ifIsOutside(const Line &line) const
	{
		crashIf(raycasts.empty(), "Raycasts vector is empty");
		std::pair<unsigned, unsigned> distribution; // of outside (true) and inside (false) respectively

		for (Vec2 raycast : raycasts)
		{
			Line ray = Line(raycast, line.p1.midpoint(line.p2));
			unsigned intersections = 0;
			
			// only check with edges that are not removed and
			// are outside (all edges are defaulted to outside, user has to manually set to inside for this to take effect)
			for (const Line &obstacle : lines)
				if (&obstacle != &line && edges[*obstacle.edge].type != EdgeType::REMOVED &&
					edges[*obstacle.edge].isOutside && obstacle.findIntersection(ray))
					++intersections;

			if (!(intersections % 2))
				++distribution.first;
			else
				++distribution.second;
		}

		return distribution.first > distribution.second;
	}

	bool ShapeComponent::ifHasIntersection(const Line &line, unsigned i, unsigned j) const
	{
		bool hasIntersection = false;

		for (const Line &otherLine : lines)
		{
			// don't check intersection with adjacent or removed lines
			const Edge &otherEdge = edges[*otherLine.edge];
			if (otherEdge.p1 == i || otherEdge.p2 == i || otherEdge.p1 == j || otherEdge.p2 == j
				|| otherEdge.type == EdgeType::REMOVED) // ADDED or REMOVED?
				continue;

			if (otherLine.findIntersection(line))
			{
				hasIntersection = true;
				break;
			}
		}

		return hasIntersection;
	}

	void ShapeComponent::update()
	{
		if (isTransformDirty)
		{
			isTransformDirty = false;
			crashIf(vbo.size() != vertices.size() * 2, "Set isBufferDirty to true instead of isTransformDirty");

			// also add removed vertices to maintain indices
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				vbo[i * 2] = vertices[i].pos.x;
				vbo[i * 2 + 1] = vertices[i].pos.y;
			}

			glBindBuffer(GL_ARRAY_BUFFER, vboId);
			glBufferData(GL_ARRAY_BUFFER, vbo.size() * sizeof(float), vbo.data(), GL_DYNAMIC_DRAW);
		}

		if (isBufferDirty)
		{
			isBufferDirty = false;

			/*! ------------ Clear old buffers ------------ */

			if (!vaoId || !vboId || !eboId)
			{
				crashIf(vaoId || vboId || eboId, "One or more of vaoId/vboId/eboId doesn't match");

				glDeleteBuffers(1, &eboId);
				glDeleteBuffers(1, &vboId);
				glDeleteVertexArrays(1, &vaoId);
			}

			/*! ------------ Initialise lines from edges ------------ */

			lines.clear();
			for (unsigned i = 0; i < static_cast<unsigned>(edges.size()); ++i)
				lines.emplace_back(vertices[edges[i].p1].pos, vertices[edges[i].p2].pos, i);

			/*! ------------ Check for intersections ------------ */

			for (unsigned i = 0; i < static_cast<unsigned>(lines.size()); ++i) // lines[i] == line1
				for (unsigned j = 0; j < static_cast<unsigned>(lines.size()); ++j) // lines[j] == line2
					if (i != j && edges[*lines[i].edge].type != EdgeType::REMOVED 
						&& edges[*lines[j].edge].type != EdgeType::REMOVED) // can't put together with next line
						if (std::optional<Vec2> intersection = lines[i].findIntersection(lines[j]))
						{
							// add new vertex at the intersection if it doesn't exist already
							unsigned newVertexIdx = static_cast<unsigned>(std::find(vertices.begin(),
								vertices.end(), Vertex(*intersection)) - vertices.begin()); // largest index
							unsigned lastIdx = static_cast<unsigned>(edges.size()); // bind to new edge
							if (newVertexIdx == static_cast<unsigned>(vertices.size()))
								vertices.emplace_back(*intersection, VertexType::ADDED);

							// add 4 edges to connect old points to new vertex
							lines.emplace_back(lines[i].p1, *intersection, lastIdx);
							lines.emplace_back(lines[i].p2, *intersection, lastIdx + 1);
							lines.emplace_back(lines[j].p1, *intersection, lastIdx + 2);
							lines.emplace_back(lines[j].p2, *intersection, lastIdx + 3);

							// same as above
							edges.emplace_back(edges[*lines[i].edge].p1, newVertexIdx, EdgeType::ADDED,
								edges[*lines[i].edge].isOutside);
							edges.emplace_back(edges[*lines[i].edge].p2, newVertexIdx, EdgeType::ADDED,
								edges[*lines[i].edge].isOutside);
							edges.emplace_back(edges[*lines[j].edge].p1, newVertexIdx, EdgeType::ADDED,
								edges[*lines[j].edge].isOutside);
							edges.emplace_back(edges[*lines[j].edge].p2, newVertexIdx, EdgeType::ADDED,
								edges[*lines[j].edge].isOutside);

							// remove the 2 intersecting edges
							auto toRemove = std::find(edges.begin(), edges.end(),
								Edge(edges[*lines[i].edge].p1, edges[*lines[i].edge].p2));
							crashIf(toRemove == edges.end(), "Could not find edge");
							toRemove->type = EdgeType::REMOVED;

							// same as above
							toRemove = std::find(edges.begin(), edges.end(),
								Edge(edges[*lines[j].edge].p1, edges[*lines[j].edge].p2));
							crashIf(toRemove == edges.end(), "Could not find edge");
							toRemove->type = EdgeType::REMOVED;
						}

			/*! ------------ Remove dangling edges and vertices ------------ */

			bool didVertexGetRemoved = true;

			// dangling edges == those with vertices that appear fewer than twice
			// repeat unti no more needs to be removed
			while (didVertexGetRemoved)
			{
				didVertexGetRemoved = false;

				for (unsigned i = 0; i < static_cast<unsigned>(vertices.size()); ++i)
				{
					if (vertices[i].type == VertexType::REMOVED)
						continue;

					int count = 0;
					std::vector<Edge *> edgesWithCurrVertex;

					for (Edge &edge : edges)
						if (edge.type != EdgeType::REMOVED && (edge.p1 == i || edge.p2 == i))
						{
							++count;
							edgesWithCurrVertex.push_back(&edge);
						}

					if (count < 2)
					{
						vertices[i].type = VertexType::REMOVED;
						std::for_each(edgesWithCurrVertex.begin(), edgesWithCurrVertex.end(),
							[](auto &elem) { elem->type = EdgeType::REMOVED; });
						didVertexGetRemoved = true;
						break; // next loop will check the rest
					}
				}
			}

			/*! ------------ Initialise VBO ------------ */

			vbo.clear();

			// also add removed vertices to maintain indices
			for (const Vertex &vertex : vertices)
			{
				vbo.push_back(vertex.pos.x);
				vbo.push_back(vertex.pos.y);
			}

			/*! ------------ Triangulation ------------ */

			// add edges to triangulate shape, but only if it doesn't change the shape
			// uses raycasting to determine if an edge is inside or outside the shape, and thus whether to add it
			for (unsigned i = 0; i < static_cast<unsigned>(vertices.size()); ++i)
			{
				if (vertices[i].type == VertexType::REMOVED)
					continue;
				
				for (unsigned j = i + 1; j < static_cast<unsigned>(vertices.size()); ++j)
				{
					if (vertices[j].type == VertexType::REMOVED)
						continue;

					// check if line already exists
					// cannot std::sort edges then std::binary_search because lines have indices into edges
					// and cannot use edge * instead of indices because std::vector may allocate new memory blocks
					if (std::find(edges.begin(), edges.end(), Edge(i, j)) != edges.end())
						continue;

					Line currLine = Line(vertices[i].pos, vertices[j].pos);

					// check if line intersects any other lines or if it is outside the shape
					if (!ifHasIntersection(currLine, i, j) && !ifIsOutside(currLine))
					{
						currLine.edge = static_cast<unsigned>(edges.size());
						lines.push_back(currLine);
						edges.emplace_back(i, j, EdgeType::ADDED, false);
					}
				}
			}

			/*! ------------ Initialise triangles ------------ */

			triangles.clear();

			for (size_t i = 0; i < edges.size(); ++i) // edges[i] == edge1
			{
				if (edges[i].type == EdgeType::REMOVED)
					continue;

				for (size_t j = i + 1; j < edges.size(); ++j) // edges[j] == edge2
				{
					if (edges[j].type == EdgeType::REMOVED)
						continue;

					// find the repeat vertex if any (i.e. the joint of 2 connected edges)
					std::array<unsigned, 4> edgeVertices = { edges[i].p1, edges[i].p2, edges[j].p1, edges[j].p2 };
					std::sort(edgeVertices.begin(), edgeVertices.end());
					auto duplicate = std::adjacent_find(edgeVertices.begin(), edgeVertices.end());
					if (duplicate == edgeVertices.end())
						continue;

					// craft the new edge to find based on the non-repeated vertex
					unsigned newP1 = edges[i].p1 == *duplicate ? edges[i].p2 : edges[i].p1;
					unsigned newP2 = edges[j].p1 == *duplicate ? edges[j].p2 : edges[j].p1;
					if (newP1 > newP2)
						std::swap(newP1, newP2); // make newP1 < newP2

					auto edge3 = std::find_if(edges.begin(), edges.end(),
						[newP1, newP2](const auto &elem) { return elem.p1 == newP1 && elem.p2 == newP2
						&& elem.type != EdgeType::REMOVED; });

					// if such an edge exists, a valid triangle can be formed (also arrange its vertices in ascending order)
					if (edge3 != edges.end())
					{
						std::array<unsigned, 3> points{ newP1, newP2, *duplicate };
						std::sort(points.begin(), points.end());
						Triangle toAdd = Triangle(points[0], points[1], points[2]);
						if (std::find(triangles.begin(), triangles.end(), toAdd) == triangles.end())
							triangles.emplace_back(toAdd);
					}
				}
			}

			/*! ------------ Initialise EBO ------------ */

			for (const Triangle &triangle : triangles)
			{
				ebo.push_back(triangle.p1);
				ebo.push_back(triangle.p2);
				ebo.push_back(triangle.p3);
			}

			/*! ------------ Initialise OpenGL stuff ------------ */

			// init vaoId
			glGenVertexArrays(1, &vaoId);
			glBindVertexArray(vaoId);

			// init vboId
			glGenBuffers(1, &vboId);
			glBindBuffer(GL_ARRAY_BUFFER, vboId);

			// init attributes
			glBufferData(GL_ARRAY_BUFFER, vbo.size() * sizeof(float), vbo.data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, static_cast<void *>(0));
			glEnableVertexAttribArray(0);

			// init eboId
			glGenBuffers(1, &eboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo.size() * sizeof(unsigned), ebo.data(), GL_DYNAMIC_DRAW);
		}

		if (isDirty)
		{
			isDirty = false;

			std::for_each(lines.begin(), lines.end(), [this](auto &elem) {
				elem.shldUpdateBuffers = true;
				elem.update(strokeWidth);
				});
		}
	}

}