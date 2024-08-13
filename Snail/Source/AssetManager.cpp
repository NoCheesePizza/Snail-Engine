#include "AssetManager.h"
#include "Utility.h"

#include <filesystem>
#include <fstream>

namespace Snail
{

	void AssetManager::init()
	{
		loadShaders();
	}

	void AssetManager::update()
	{

	}

	void AssetManager::free()
	{

	}

	void AssetManager::loadShaders()
	{
		for (const auto &entry : std::filesystem::directory_iterator("Assets/Shaders"))
		{
			std::string line, source;
			std::ifstream ifs(entry.path());
			while (std::getline(ifs, line))
				source += line + nl;

			if (entry.path().extension() == ".vert")
				vertSources[entry.path().stem().string()] = source;
			else if (entry.path().extension() == ".frag")
				fragSources[entry.path().stem().string()] = source;
			else
				crashIf(true, "Invalid file type in \"Shaders\" folder");
		}
	}

	const std::string &AssetManager::getShaderSource(ShaderType type, const std::string &name)
	{
		switch (type)
		{
		case ShaderType::VERTEX:
			crashIf(!vertSources.count(name), "Vertex shader " + Util::quote(name) + " was not found");
			return vertSources.at(name);
			break;

		case ShaderType::FRAGMENT:
			crashIf(!fragSources.count(name), "Fragment shader " + Util::quote(name) + " was not found");
			return fragSources.at(name);
			break;

		default:
			crashIf(true, "Invalid shader type given");
			break;
		}

		return systemName; // lol won't reach here just returning a string that won't evaporate
	}

	const std::unordered_map<std::string, std::string> &AssetManager::getVertSources()
	{
		return vertSources;
	}

	const std::unordered_map<std::string, std::string> &AssetManager::getFragSources()
	{
		return fragSources;
	}

}