#include "Core.h"
#include "Utility.h"
#include "System.h"

#include <iostream>

int main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	{
		Snail::Debugger::initSignalHandler();
		Snail::Core::init();
		Snail::Core::update();
		Snail::Core::free();
	}

	return 0;
}