#include "Renderer.h"
#include "Utility.h"
#include "AssetManager.h"
#include "Core.h"

#include <iostream> // for debugging

namespace Snail
{

	void GLAPIENTRY handleOpenglError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
		const char *message, const void *param)
	{
		unref(param);
		unref(length);

		if (severity != 0x826b)
		{
			std::ostringstream oss;
			oss << "OpenGL error!\nSource: 0x" << std::hex << source << "; Type: 0x" << type
				<< "; ID: 0x" << id << "; Severity: 0x" << severity << "; Message: " << std::string(message);
			crashIf(true, oss.str());
		}
	}

	void Renderer::init()
	{
		for (const auto &[file, source] : gs(AssetManager)->getVertSources())
			vertShaders[file] = compileShader(GL_VERTEX_SHADER, source);
		for (const auto &[file, source] : gs(AssetManager)->getFragSources())
			fragShaders[file] = compileShader(GL_FRAGMENT_SHADER, source);

		// all possible combinations of vert and frag shaders
		for (const auto &[vName, vId] : vertShaders)
			for (const auto &[fName, fId] : fragShaders)
				vertFragShaders[vName + " + " + fName] = createVertFragShader(vId, fId);
	}

	void Renderer::update()
	{

	}

	void Renderer::free()
	{
		for (const auto &[name, id] : vertShaders)
			glDeleteShader(id);
		for (const auto &[name, id] : fragShaders)
			glDeleteShader(id);
		for (const auto &[name, id] : vertFragShaders)
			glDeleteProgram(id);
	}

	void Renderer::setWindow(GLFWwindow *_window)
	{
		window = _window;
	}

	unsigned Renderer::compileShader(GLenum type, const std::string &source)
	{
		GLuint id = glCreateShader(type);
		const GLchar *src = source.c_str();
		glShaderSource(id, 1, &src, nullptr); // count = number of code files, 
		// length = length of code (nullptr for everything or until \0
		glCompileShader(id);

		GLint result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result); // i = int, v = vector

		if (!result)
		{
			GLint length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			GLchar *message = static_cast<char *>(_malloca(length * sizeof(char)));
			glGetShaderInfoLog(id, length, &length, message);
			crashIf(true, "Failed to compile shader!\n"s + message);
		}

		return id;
	}

	unsigned Renderer::createVertFragShader(GLuint vertId, GLuint fragId)
	{
		unsigned program = glCreateProgram();

		glAttachShader(program, vertId);
		glAttachShader(program, fragId);
		glLinkProgram(program);
		glValidateProgram(program);

		return program;
	}

	void Renderer::useVertFragShader(const std::string &name)
	{
		crashIf(!vertFragShaders.count(name), "Vertex + fragment shader " +
			Util::quote(name) + " does not exist");
		glUseProgram(vertFragShaders.at(name));
	}

	unsigned Renderer::getVertFragShader(const std::string &name)
	{
		crashIf(!vertFragShaders.count(name), "Vertex + fragment shader " +
			Util::quote(name) + " does not exist");
		return vertFragShaders.at(name);
	}

}