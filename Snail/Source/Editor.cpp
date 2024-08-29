#include "Editor.h"
#include "Core.h"
#include "Timer.h"
#include "EntityManager.h"
#include "ComponentManager.h"

namespace Snail
{
	EditorWindow::EditorWindow(const std::string &_name, bool _shldBeOpened) 
		: name(_name), shldBeOpened(_shldBeOpened) 
	{

	}

	EditorWindow::~EditorWindow()
	{

	}

	void EditorWindow::onEnter()
	{
		// add general code for all windows
	}

	void EditorWindow::onUpdate()
	{
		if (!isOpen)
			return;

		// add general code for all windows
	}

	void EditorWindow::onExit()
	{
		// add general code for all windows
	}

	void EditorWindow::open()
	{
		isOpen = true;
		onEnter();
	}

	void EditorWindow::close()
	{
		isOpen = false;
		onExit();
	}

	bool EditorWindow::isWindowOpen()
	{
		return isOpen;
	}

	bool EditorWindow::shldWindowBeOpened()
	{
		return shldBeOpened;
	}

	MainMenu::MainMenu(const std::string &_name, bool _shldBeOpened) 
		: EditorWindow(_name, _shldBeOpened)
	{

	}

	void MainMenu::onEnter()
	{
		EditorWindow::onEnter();
	}

	void MainMenu::onUpdate()
	{
		if (!isOpen)
			return;
		EditorWindow::onUpdate();

		ImGui::Begin(name.c_str());

		ImGui::Text("FPS: %.2f", 1.f / gs(Time)->getDt().actual);
		gs(Editor)->addSpace(3);

		for (const auto &[name, window] : gs(Editor)->getWindows())
			if (window->shldWindowBeOpened() && ImGui::Button(("Toggle "s + name).c_str()))
				gs(Editor)->toggleWindow(name);

		ImGui::End();
	}

	void MainMenu::onExit()
	{
		EditorWindow::onExit();
	}

	Inspector::Inspector(const std::string &_name, bool _shldBeOpened)
		: EditorWindow(_name, _shldBeOpened)
	{

	}

	void Inspector::onEnter()
	{
		EditorWindow::onEnter();
	}

	void Inspector::onUpdate()
	{
		if (!isOpen)
			return;
		EditorWindow::onUpdate();

		ImGui::Begin(name.c_str());

		for (EntityId entity : gs(EntityManager)->getEntityIds())
		{
			ImGui::Text("Entity %llu", entity);
			for (CompId id = 0; id < MAX_COMPONENTS; ++id)
				if (gs(EntityManager)->hasComponent(entity, id))
				{
					ImGui::Text("%s", gs(ComponentManager)->getCompName(id).c_str());
					//TransformComponent &tc = gs(ComponentManager)->getComponent(entity, id);
				}
			gs(Editor)->addSpace(3);
		}

		ImGui::End();
	}

	void Inspector::onExit()
	{
		EditorWindow::onExit();
	}

	void Editor::init()
	{
		addWindow<MainMenu>(true, false);
		addWindow<Inspector>(true, true);
	}

	void Editor::update()
	{
		for (auto &[key, value] : windows)
			value->onUpdate();
	}

	void Editor::free()
	{
		for (auto &[key, value] : windows)
			delete value;
	}

	void Editor::openWindow(const std::string &windowName)
	{
		crashIf(!windows.count(windowName), "Could not find window with name " + Util::quote(windowName));
		windows.at(windowName)->open();
	}

	void Editor::closeWindow(const std::string &windowName)
	{
		crashIf(!windows.count(windowName), "Could not find window with name " + Util::quote(windowName));
		windows.at(windowName)->close();
	}

	void Editor::toggleWindow(const std::string &windowName)
	{
		crashIf(!windows.count(windowName), "Could not find window with name " + Util::quote(windowName));
		if (windows.at(windowName)->isWindowOpen())
			windows.at(windowName)->close();
		else
			windows.at(windowName)->open();
	}

	void Editor::createDockspace()
	{
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// Set window background color to be transparent
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

		ImGui::Begin("DockSpace Demo", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();  // Pop the color style

		ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();
	}

	const std::unordered_map<std::string, EditorWindow *> &Editor::getWindows()
	{
		return windows;
	}

	void Editor::addSpace(int spaceCount)
	{
		for (int i = 0; i < spaceCount; ++i)
			ImGui::Spacing();
	}

}