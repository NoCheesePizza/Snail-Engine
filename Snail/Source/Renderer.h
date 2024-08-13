#pragma once

#include "System.h"
#include "Utility.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <unordered_map>

namespace Snail
{

	void GLAPIENTRY handleOpenglError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const char *message, const void *param);

	class Renderer : public System
	{
		GLFWwindow *window;

		std::unordered_map<std::string, unsigned> vertShaders;
		std::unordered_map<std::string, unsigned> fragShaders;
		std::unordered_map<std::string, unsigned> vertFragShaders;

	public:

		void init() override;
		void update() override;
		void free() override;

		void setWindow(GLFWwindow *_window);

		unsigned compileShader(GLenum type, const std::string &source);
		unsigned createVertFragShader(GLuint vertId, GLuint fragId);
		void useVertFragShader(const std::string &name);
		unsigned getVertFragShader(const std::string &name);
	};

}