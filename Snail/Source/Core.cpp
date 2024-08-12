#include "Core.h"
#include "Debug.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Timer.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream> // for debugging

namespace Snail
{

	namespace Core
	{

		std::vector<std::unique_ptr<System>> systems;

		GLFWwindow *window;

		template <typename T>
		void addSystem(bool shdProfile)
		{
			systems.push_back(std::make_unique<T>());
			systems.back()->name = typeid(T).name();
			systems.back()->shdProfile = shdProfile;
		}

		void init()
		{
			// add systems
			addSystem<EntityManager>(false);
			addSystem<ComponentManager>(false);
			addSystem<Time>(true);

			for (auto &system : systems)
				system->init();

			/* Initialize the library */
			crashIf(!glfwInit(), "GLFW failed to initialise");

			/* Create a windowed mode window and its OpenGL context */
			window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
			if (!window)
			{
				glfwTerminate();
				crashIf(true, "Window could not be created");
			}

			/* Make the window's context current */
			glfwMakeContextCurrent(window);
			crashIf(glewInit() != GLEW_OK, "GLEW failed to initialise");
		}

		void update()
		{
			/* Loop until the user closes the window */
			while (!glfwWindowShouldClose(window))
			{
				/* Render here */
				glClear(GL_COLOR_BUFFER_BIT);

				for (auto &system : systems)
					system->update();

				/* Swap front and back buffers */
				glfwSwapBuffers(window);

				/* Poll for and process events */
				glfwPollEvents();
			}
		}

		void free()
		{
			glfwTerminate();

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