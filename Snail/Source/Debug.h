#pragma once

#include "System.h"

#include <vector>

namespace Snail
{

	namespace Debugger
	{

		void initSignalHandler();
		void log(int signal, const std::string &reason = "", const std::string &fileName = "", int line = 0);

		class Printable
		{
		public:

			virtual std::string stringify() const = 0;
			friend std::ostream &operator<<(std::ostream &os, const Printable &that);
		};

	}

}