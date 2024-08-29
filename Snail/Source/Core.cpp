#include "Core.h"
#include "Debug.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "Timer.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Editor.h"

#include <iostream> // for debugging
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_opengl3.h"
#include "External/ImGui/imgui_impl_glfw.h"

namespace Snail
{

	namespace Core
	{

		std::vector<std::unique_ptr<System>> systems;

		GLFWwindow *windowPtr;
		Window window = { { 640.f, 640.f }, "Snail Engine" };

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
			addSystem<Editor>(true);

			crashIf(!glfwInit(), "GLFW failed to initialise");

			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			windowPtr = glfwCreateWindow(static_cast<int>(window.size.x), static_cast<int>(window.size.y),
				window.name.c_str(), nullptr, nullptr);

			if (!windowPtr)
			{
				glfwTerminate();
				crashIf(true, "Window could not be created");
			}

			gs(Renderer)->setWindowPtr(windowPtr);
			gs(Renderer)->setWindow(window);

			glfwMakeContextCurrent(windowPtr);
			crashIf(glewInit() != GLEW_OK, "GLEW failed to initialise");
			std::cout << "OpenGL version: " << glGetString(GL_VERSION) << nl;

			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(handleOpenglError, nullptr);

			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();

			io.Fonts->AddFontFromFileTTF("Assets/Fonts/PoorStoryRegular.ttf", 24.f);
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
			
			ImGui_ImplGlfw_InitForOpenGL(windowPtr, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");

			// initialise systems
			for (auto &system : systems)
				system->init();
		}

		void update()
		{
			EntityId rect = gs(EntityManager)->addEntity();
			ShapeComponent shape;
			TransformComponent trans;

			shape.vertices = { Vertex({ -200, -100 }), Vertex({ 200, -100 }), Vertex({ 200, 100 }), 
				Vertex({ -200, 100 }), Vertex({ -100, -50 }), Vertex({ 100, -50 }), Vertex({ 100, 50 }), 
				Vertex({ -100, 50 }) }; // too lazy to write .f
			shape.edges = { { Edge(0, 1), Edge(1, 2), Edge(2, 3), Edge(3, 0), Edge(4, 5), Edge(5, 6), 
				Edge(6, 7), Edge(7, 4) } };

			trans.pos = { 200.f, 100.f };
			shape.translate({ 200.f, 100.f });
			shape.isDirty = true;
			shape.update();

			gs(ComponentManager)->addComponent<ShapeComponent>(rect, shape);
			gs(ComponentManager)->addComponent<TransformComponent>(rect, trans);

			gs(Renderer)->useVertFragShader("Default + Default");
			gs(Time)->setFps(60.f);

			while (!glfwWindowShouldClose(windowPtr))
			{
				gs(Time)->beginDt();
				glfwPollEvents();
				glClear(GL_COLOR_BUFFER_BIT);

				// Start the Dear ImGui frame
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				gs(Editor)->createDockspace();

				for (auto &system : systems)
					system->update();

				// More imgui stuff
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				// This is important if using multi-viewports 
				// (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(windowPtr);

				glfwSwapBuffers(windowPtr);
				gs(Time)->endDt();
			}
		}

		void free()
		{
			// Cleanup
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

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