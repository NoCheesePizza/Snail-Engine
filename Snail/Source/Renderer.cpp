#include "Renderer.h"
#include "Utility.h"
#include "AssetManager.h"
#include "Core.h"
#include "Components.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Timer.h"

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
			oss << "OpenGL error!u\nSource: 0x" << std::hex << source << "; Type: 0x" << type
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

		for (const auto &[name, id] : vertShaders)
			glDeleteShader(id);
		for (const auto &[name, id] : fragShaders)
			glDeleteShader(id);

		// set uniform locations
		useVertFragShader("Default + Default");
		setUniform("fillColor");
		setUniform("lineColor");
		setUniform("shldUseFillColor");
		setUniform("screenSize");

		// set static uniforms
		glUniform2f(gs(Renderer)->getUniform("screenSize"), 640.f, 280.f);
	}

	void Renderer::update()
	{
		for (EntityId entity : gs(EntityManager)->getEntityIds())
		{
			ShapeComponent &shape = gs(ComponentManager)->getComponent<ShapeComponent>(entity);
			shape.update();

			glUniform4f(getUniform("fillColor"), shape.fillColor.r, shape.fillColor.g, shape.fillColor.b,
				shape.fillColor.a);
			glUniform1i(getUniform("shldUseFillColor"), shape.shldUseFillColor);
			glBindVertexArray(shape.vaoId);
			glDrawElements(GL_TRIANGLES, static_cast<GLuint>(shape.ebo.size()), GL_UNSIGNED_INT, nullptr);

			glUniform4f(getUniform("lineColor"), shape.strokeColor.r, shape.strokeColor.g, shape.strokeColor.b,
				shape.strokeColor.a); // set line colour

			for (const Line &line : shape.lines)
				if (shape.edges[*line.edge].type != EdgeType::ADDED)
				{
					glBindVertexArray(line.vaoId);
					glDrawElements(GL_TRIANGLES, static_cast<GLuint>(line.ebo.size()), GL_UNSIGNED_INT, nullptr);
				}
		}
	}

	void Renderer::free()
	{
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
		currShader = vertFragShaders.at(name);
		glUseProgram(currShader);
	}

	unsigned Renderer::getCurrShader()
	{
		return currShader;
	}

	void Renderer::setUniform(const std::string &name)
	{
		unsigned location = glGetUniformLocation(currShader, name.c_str());
		crashIf(location == -1, "Uniform " + Util::quote(name) + " could not be found");
		uniforms.at(currShader)[name] = location;
	}

	unsigned Renderer::getUniform(const std::string &name)
	{
		crashIf(!uniforms.at(currShader).count(name), "Uniform " + Util::quote(name) + " could not be found");
		return uniforms.at(currShader).at(name);
	}

}