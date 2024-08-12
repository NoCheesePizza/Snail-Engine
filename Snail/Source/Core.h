#pragma once

#include "Utility.h"
#include "System.h"
#include "Debug.h"

namespace Snail
{

	namespace Core
	{

		void init();
		void update();
		void free();
		const std::vector<std::unique_ptr<System>> &getSystems();

		template <typename T>
		T *getSystem()
		{
			for (auto &system : getSystems())
				if (T *ptr = dynamic_cast<T *>(system.get()))
					return ptr;

			crashIf(true, "System " + Util::quote(typeid(T).name()) + " does not exist"s);
			return nullptr;
		}

	}
}