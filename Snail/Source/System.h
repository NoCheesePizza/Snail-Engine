#pragma once

#include <string>

namespace Snail
{

	class System
	{
	public:

		std::string name;
		bool shdProfile = true;

		virtual void init() = 0;
		virtual void update() = 0;
		virtual void free() = 0;

		virtual ~System();
	};

}