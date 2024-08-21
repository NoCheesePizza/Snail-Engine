#include "Types.h"
#include "Utility.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace Snail
{

	Color::Color(float _r, float _g, float _b, float _a)
		: r(_r), g(_g), b(_b), a(_a)
	{

	}

	std::string Color::stringify() const
	{
		return "{ r: " + toStr(r) + "; g: " + toStr(g) + "; b: " + toStr(b) + "; a: " + toStr(a) + " }";
	}

	Vertex::Vertex(Vec2 _pos, VertexType _type)
		: pos(_pos), type(_type)
	{

	}

	bool Vertex::operator<(const Vertex &that) const
	{
		return pos < that.pos;
	}

	bool Vertex::operator==(const Vertex &that) const
	{
		return pos == that.pos;
	}

	std::string Vertex::stringify() const
	{
		return "{ pos: " + pos.stringify() + "; type: " + toStr(static_cast<int>(type)) + " }";
	}

	Edge::Edge(unsigned _p1, unsigned _p2, EdgeType _type, bool _isOutside)
		: type(_type), isOutside(_isOutside)
	{
		p1 = std::min(_p1, _p2);
		p2 = std::max(_p1, _p2);
	}

	bool Edge::operator<(const Edge &that) const
	{
		if (p1 == that.p1)
			return p2 < that.p2;
		return p1 < that.p1;
	}

	bool Edge::operator==(const Edge &that) const
	{
		return p1 == that.p1 && p2 == that.p2;
	}

	std::string Edge::stringify() const
	{
		return "{ p1: " + toStr(p1) + "; p2: " + toStr(p2) + "; type: " + toStr(static_cast<int>(type)) 
			+ "; isOutside: " + Util::toString(isOutside) + " }";
	}

	Triangle::Triangle(unsigned _p1, unsigned _p2, unsigned _p3)
		: p1(_p1), p2(_p2), p3(_p3)
	{

	}

	bool Triangle::operator<(const Triangle &that) const
	{
		if (p1 == that.p1)
		{
			if (p2 == that.p2)
				return p3 < that.p3;
			return p2 < that.p2;
		}
		return p1 < that.p1;
	}

	bool Triangle::operator==(const Triangle &that) const
	{
		return p1 == that.p1 && p2 == that.p2 && p3 == that.p3;
	}

	std::string Triangle::stringify() const
	{
		return "{ p1: " + toStr(p1) + "; p2: " + toStr(p2) + "; p3: " + toStr(p3) + " }";
	}

	Line::Line(Vec2 _p1, Vec2 _p2, std::optional<unsigned> _edge)
		: p1(_p1), p2(_p2), edge(_edge)
	{
		update();

		glGenVertexArrays(1, &vaoId);
		glBindVertexArray(vaoId);

		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vbo.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, static_cast<void *>(0));
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &eboId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * 6, ebo.data(), GL_DYNAMIC_DRAW);
	}

	Line::~Line()
	{
		if (!vaoId || !vboId || !eboId)
		{
			crashIf(vaoId || vboId || eboId, "One or more of vaoId/vboId/eboId doesn't match");

			glDeleteBuffers(1, &eboId);
			glDeleteBuffers(1, &vboId);
			glDeleteVertexArrays(1, &vaoId);
		}
	}

	std::string Line::stringify() const 
	{
		return "{ p1: " + p1.stringify() + "; p2: " + p2.stringify() + "; dir: " + dir.stringify() + "; norm: "
			+ norm.stringify() + "; edge: " + (edge ? toStr(*edge) : "-") + " }";
	}
	
	bool Line::ifIsOnOneSide(const std::vector<Vec2> &positions) const
	{
		std::optional<bool> isAllPositive = std::nullopt;

		for (Vec2 pos : positions)
		{
			Vec2 p1ToNewPoint = pos - p1; // don't need to normalise

			if (!isAllPositive)
			{
				isAllPositive = norm.dot(p1ToNewPoint) > 0.f; // dot of 0.f (straight line) is taken to be behind
				continue;
			}

			// straight lines will not be considered (ie assume they lie on the same side as the others)
			float dotProduct = norm.dot(p1ToNewPoint);
			if ((dotProduct > 0.f) != *isAllPositive && !(Util::isEqual(dotProduct, 0.f)))
				return false;
		}

		return true;
	}

	bool Line::ifIsIntersecting(const Line &that) const
	{
		return !ifIsOnOneSide({ that.p1, that.p2 });
	}

	IntersectData Line::findIntersection(const Line &that, float error) const
	{
		IntersectData ret;
		if (Util::isEqual(dir.y * that.dir.x - dir.x * that.dir.y, 0.f) || 
			(Util::isEqual(dir.x, 0.f) && Util::isEqual(dir.y, 0.f)))
			return ret;

		ret.s = (dir.x * that.p1.y - dir.x * p1.y - dir.y * that.p1.x + dir.y * p1.x) /
			(dir.y * that.dir.x - dir.x * that.dir.y);
		ret.t = dir.x ? (that.p1.x + ret.s * that.dir.x - p1.x) / dir.x : 
			(that.p1.y + ret.s * that.dir.y - p1.y) / dir.y;

		if (ret.s >= 0.f + error && ret.s <= 1.f - error && ret.t >= 0.f + error && ret.t <= 1.f + error)
		{
			ret.isIntersecting = true;
			ret.intersection = that.p1 + that.dir * ret.s;
		}
		return ret;
	}

	void Line::update()
	{
		if (!isDirty)
			return;
		isDirty = false;

		dir = (p2 - p1);
		dirHat = dir.normalize();
		norm = (p2 - p1).inwardNormal();
		normHat = norm.normalize();

		float halfStroke = stroke / 2.f;
		Vec2 curr = p1 + normHat * halfStroke;
		vbo[0] = curr.x;
		vbo[1] = curr.y;

		curr = p1 - normHat * halfStroke;
		vbo[2] = curr.x;
		vbo[3] = curr.y;

		curr = p2 + normHat * halfStroke;
		vbo[4] = curr.x;
		vbo[5] = curr.y;

		curr = p2 - normHat * halfStroke;
		vbo[6] = curr.x;
		vbo[7] = curr.y;

		if (vboId)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vboId);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vbo.data(), GL_DYNAMIC_DRAW);
		}
	}

}