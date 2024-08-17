#pragma once

#include "Vec2.h"

#include <bitset>
#include <optional>
#include <array>

namespace Snail
{
	/*! ------------ ECS ------------ */

	using EntityId = size_t;
	using CompId = size_t;

	enum class Component : CompId
	{
		SIMPLE,
		SHAPE,
		MAX_COMPONENTS
	};

	using Signature = std::bitset<static_cast<size_t>(Component::MAX_COMPONENTS)>;

	/*! ------------ Graphics ------------ */

	enum class ShaderType : unsigned
	{
		VERTEX,
		FRAGMENT,
		MAX_SHADER_TYPES
	};

	enum class VertexType
	{
		NONE,
		ADDED,
		REMOVED,
		MAX_VERTEX_TYPES
	};

	enum class EdgeType
	{
		NONE,
		ADDED,
		REMOVED,
		MAX_EDGE_TYPES
	};

	struct Color : public Debugger::Printable
	{
		float r, g, b, a;

		explicit Color(float _r = 1.f, float _g = 1.f, float _b = 1.f, float _a = 1.f);

		std::string stringify() const override;
	};

	struct Vertex : public Debugger::Printable
	{
		Vec2 pos;
		VertexType type = VertexType::NONE;
		Color color;

		explicit Vertex(Vec2 _pos = Vec2(), VertexType _type = VertexType::NONE);

		bool operator<(const Vertex &that) const;
		bool operator==(const Vertex &that) const;

		std::string stringify() const override;
	};

	struct Edge : public Debugger::Printable
	{
		unsigned p1, p2;
		EdgeType type = EdgeType::NONE;
		bool isOutside; // whether is inside or outside the shape

		explicit Edge(unsigned _p1, unsigned _p2, EdgeType _type = EdgeType::NONE, bool _isOutside = true);

		bool operator<(const Edge &that) const;
		bool operator==(const Edge &that) const;

		std::string stringify() const override;
	};

	struct Triangle : public Debugger::Printable
	{
		unsigned p1, p2, p3;

		explicit Triangle(unsigned _p1, unsigned _p2, unsigned _p3);

		bool operator<(const Triangle &that) const;
		bool operator==(const Triangle &that) const;

		std::string stringify() const override;
	};

	struct Line : public Debugger::Printable
	{
		Vec2 p1, p2;
		Vec2 dir, norm;
		Vec2 dirHat, normHat; // normalised
		std::optional<unsigned> edge;

		std::array<float, 8> vbo; // rectangle (x, y for each point)
		std::array<unsigned, 6> ebo = { 0, 1, 3, 0, 2, 3 }; // 2 triangles
		unsigned vaoId = 0, vboId = 0, eboId = 0;
		bool shldUpdatePos = false, shldUpdateBuffers = false;

		explicit Line(Vec2 _p1 = Vec2(), Vec2 _p2 = Vec2(), std::optional<unsigned> _edge = std::nullopt);

		std::string stringify() const override;

		bool ifIsOnOneSide(const std::vector<Vec2> &positions) const;
		bool ifIsIntersecting(const Line &that) const;
		std::optional<Vec2> findIntersection(const Line &that) const;

		void recompute();
		void update(float stroke);
	};

}