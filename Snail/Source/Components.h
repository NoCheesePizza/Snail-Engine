#pragma once

#include "Utility.h"
#include "Types.h"

#include <set>

namespace Snail
{

	struct TransformComponent
	{
		Vec2 pos;
		Vec2 scale;
		float rot;
	};

	struct ShapeComponent
	{
		std::vector<Vertex> vertices; // can modify
		std::vector<Edge> edges; // can modify
		std::vector<Triangle> triangles;
		std::vector<Line> lines;
		std::vector<Vec2> raycasts = { { -999.f, -999.f }, { 0.f, -999.f }, { 999.f, -999.f }, { -999.f, 0.f },
			{ 999.f, 0.f }, { -999.f, 999.f }, { 0.f, 999.f }, { 999.f, 999.f } }; // start of raycast

		std::vector<float> vbo;
		std::vector<unsigned> ebo;
		unsigned vboId = 0, vaoId = 0, eboId = 0;

		// please set to true if something was modified
		bool isDirty = false; // if anything in this struct is modified
		bool isTransformDirty = false; // if the transform component tied to this entity is modified

		float strokeWidth = 3.f;
		Color strokeColor;
		Color fillColor = Color(0.2f, 0.6f);
		bool shldUseFillColor = true; // use same colour for whole shape or use vertex's colour
		
		ShapeComponent() = default;
		~ShapeComponent();

		void update(); // call every frame before drawing

		void translate(Vec2 dir); // assume transform component's pos has been updated
		void scale(Vec2 dir); // assume transform component's pos and scale has been updated
		void rotate(float rad, Vec2 origin); // assume transform component's rot has been updated

	private:

		bool ifIsConvex(const Line &line) const;
		bool ifIsOutside(const Line &line, unsigned i, unsigned j);
		bool ifHasIntersection(const Line &line, unsigned i, unsigned j) const;
	};

}