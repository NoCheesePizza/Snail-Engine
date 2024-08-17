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

		unsigned currShader = 0;

		std::unordered_map<std::string, unsigned> vertShaders;
		std::unordered_map<std::string, unsigned> fragShaders;
		std::unordered_map<std::string, unsigned> vertFragShaders;
		std::unordered_map<unsigned, std::unordered_map<std::string, unsigned>> uniforms; // program : name : location

		unsigned compileShader(GLenum type, const std::string &source);
		unsigned createVertFragShader(GLuint vertId, GLuint fragId);

	public:

		void init() override;
		void update() override;
		void free() override;

		void setWindow(GLFWwindow *_window);
		
		void useVertFragShader(const std::string &name);
		unsigned getCurrShader();
		void setUniform(const std::string &name);
		unsigned getUniform(const std::string &name);
	};

}