#pragma once

#include "System.h"
#include "Utility.h"

#include "External/ImGui/imgui.h"

namespace Snail
{

	class EditorWindow
	{
	protected:

		std::string name;
		bool isOpen = false;
		bool shldBeOpened = true;

	public:

		EditorWindow(const std::string &_name = "", bool _shldBeOpened = true);
		virtual ~EditorWindow();

		virtual void onEnter();
		virtual void onUpdate();
		virtual void onExit();

		void open();
		void close();
		bool isWindowOpen();
		bool shldWindowBeOpened();
	};

	class MainMenu : public EditorWindow
	{
	public:

		MainMenu(const std::string &_name = "", bool _shldBeOpened = true);

		void onEnter() override;
		void onUpdate() override;
		void onExit() override;
	};

	class Inspector : public EditorWindow
	{
	public:

		Inspector(const std::string &_name = "", bool _shldBeOpened = true);

		void onEnter() override;
		void onUpdate() override;
		void onExit() override;
	};

	class Editor : public System
	{
		std::unordered_map<std::string, EditorWindow *> windows;

	public:

		void init();
		void update();
		void free();
		void openWindow(const std::string &windowName);
		void closeWindow(const std::string &windowName);
		void toggleWindow(const std::string &windowName);
		void createDockspace();
		const std::unordered_map<std::string, EditorWindow *> &getWindows();
		void addSpace(int spaceCount);

		template <typename T>
		void addWindow(bool shldOpen, bool shldBeOpened)
		{
			if constexpr (std::is_base_of_v<EditorWindow, std::decay_t<T>>)
			{
				std::string name = typeid(T).name();
				Util::trimString(name, "class Snail::");
				crashIf(windows.count(name), "Window name " + Util::quote(name) + " already exists");

				windows[name] = new T(name, shldBeOpened);
				if (shldOpen)
					openWindow(name);
			}
		}

		template <typename T>
		T *getWindow()
		{
			std::string name = typeid(T).name();
			Util::trimString(name, "class Snail::");
			crashIf(!windows.count(name), "Window name " + Util::quote(name) + " does not exist");
			return dynamic_cast<T *>(windows.at(name));
		}
	};

}