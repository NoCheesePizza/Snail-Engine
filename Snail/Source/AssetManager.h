#pragma once

#include "System.h"
#include "Utility.h"
#include "Types.h"

#include <unordered_map>

namespace Snail
{

	class AssetManager : public System
	{
		std::unordered_map<std::string, std::string> vertSources;
		std::unordered_map<std::string, std::string> fragSources;

	public:

		void init() override;
		void update() override;
		void free() override;
		
		void loadShaders();

		const std::string &getShaderSource(ShaderType type, const std::string &name);
		const std::unordered_map<std::string, std::string> &getVertSources();
		const std::unordered_map<std::string, std::string> &getFragSources();
	};

}