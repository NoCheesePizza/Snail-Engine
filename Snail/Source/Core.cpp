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
		void addSystem(bool shldProfile)
		{
			systems.push_back(std::make_unique<T>());
			systems.back()->systemName = typeid(T).name();
			systems.back()->shldProfile = shldProfile;
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
			EntityId rect = gs(EntityManager)->addEntity();
			ShapeComponent shape;

			shape.vertices = std::vector<Vertex>{ Vertex({ -200, -100 }), Vertex({ 200, -100 }), 
				Vertex({ 200, 100 }), Vertex({ -200, 100 }), Vertex({ -100, -50 }), Vertex({ 100, -50 }), 
				Vertex({ 100, 50 }), Vertex({ -100, 50 }) }; // too lazy to write .f
			shape.edges = std::vector<Edge>{ { Edge(0, 1), Edge(1, 2), Edge(2, 3), Edge(3, 0), Edge(4, 5), 
				Edge(5, 6), Edge(6, 7), Edge(7, 4) } };

			shape.isDirty = true;
			shape.update();

			gs(ComponentManager)->addComponent<ShapeComponent>(rect, shape);
			gs(ComponentManager)->addComponent<TransformComponent>(rect);
			
			gs(Renderer)->useVertFragShader("Default + Default");
			gs(Time)->setFps(60.f);

			while (!glfwWindowShouldClose(window))
			{
				gs(Time)->beginDt();
				glClear(GL_COLOR_BUFFER_BIT);

				for (auto &system : systems)
					system->update();

				glfwSwapBuffers(window);
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