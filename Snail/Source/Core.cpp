#include "Core.h"
#include "Debug.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Timer.h"
#include "AssetManager.h"
#include "Renderer.h"

#include <iostream> // for debugging

namespace Snail
{

	namespace Core
	{

		std::vector<std::unique_ptr<System>> systems;

		GLFWwindow *window;
		unsigned shader;

		template <typename T>
		void addSystem(bool shdProfile)
		{
			systems.push_back(std::make_unique<T>());
			systems.back()->systemName = typeid(T).name();
			systems.back()->shdProfile = shdProfile;
		}

		void init()
		{
			// add systems
			addSystem<EntityManager>(false);
			addSystem<ComponentManager>(false);
			addSystem<Time>(true);
			addSystem<AssetManager>(false);
			addSystem<Renderer>(true);

			crashIf(!glfwInit(), "GLFW failed to initialise");

			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
			if (!window)
			{
				glfwTerminate();
				crashIf(true, "Window could not be created");
			}

			gs(Renderer)->setWindow(window);
			glfwMakeContextCurrent(window);
			crashIf(glewInit() != GLEW_OK, "GLEW failed to initialise");
			std::cout << "OpenGL version: " << glGetString(GL_VERSION) << nl;

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(handleOpenglError, nullptr);

			// initialise systems
			for (auto &system : systems)
				system->init();
		}

		void update()
		{
			float pos[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
			unsigned indices[] = { 0, 1, 2, 2, 3, 0 };
			unsigned vao;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			
			unsigned buffer, indexBuffer;
			glGenBuffers(1, &buffer); // create 1 vertex buffer and get id of buffer back
			glBindBuffer(GL_ARRAY_BUFFER, buffer); // select buffer using id
			glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), pos, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // first 0 is index of current attribute, last 0 is offset of current attribute in bytes from first attribute of current vertex (this line links vao to vertex buffer)
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned), indices, GL_STATIC_DRAW);

			//shader = createShader(gs(AssetManager)->getShaderSource(ShaderType::VERTEX, "Default"),
				//gs(AssetManager)->getShaderSource(ShaderType::FRAGMENT, "Default"));
			//glUseProgram(shader);
			gs(Renderer)->useVertFragShader("Default + Default");

			int location = glGetUniformLocation(gs(Renderer)->getVertFragShader("Default + Default"), "u_color");
			crashIf(location == -1, "Uniform not found");
			float r = 0.f, increment = 3.f;
			gs(Time)->setFps(60.f);
				
			/* Loop until the user closes the window */
			while (!glfwWindowShouldClose(window))
			{
				gs(Time)->beginDt();

				/* Render here */
				glClear(GL_COLOR_BUFFER_BIT);

				//glDrawArrays(GL_TRIANGLES, 0, 3);
				glUniform4f(location, r, 0.3f, 0.8f, 1.f);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // 6 = 6 vertices but 2 use same coords

				for (auto &system : systems)
					system->update();

				if (r > 1.f)
					increment = -3.f;
				else if (r < 0.f)
					increment = 3.f;
				r += increment * gs(Time)->getDt().actual;

				/* Swap front and back buffers */
				glfwSwapBuffers(window);

				/* Poll for and process events */
				glfwPollEvents();

				gs(Time)->endDt();
			}
		}

		void free()
		{
			glfwTerminate();
			glDeleteProgram(shader);

			// free systems in reverse order
			for (size_t i = systems.size(); i-- > 0; ) // --> is the "approach to" operator
				systems[i]->free();
			systems.clear();
		}

		const std::vector<std::unique_ptr<System>> &getSystems()
		{
			return systems;
		}

	}

}