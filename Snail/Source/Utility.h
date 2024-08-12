#pragma once

#include "Debug.h"
#include "Vec2.h"

#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <bitset>

// called by the user when a logic or runtime error occurs (user must implement) to crash the program gracefully
#define crashIf(condition, reason) do { if (condition) Debugger::log(-1, reason, __FILE__, __LINE__); } while (0)

#define gs(system) Core::getSystem<system>()

#define unref(param) param

/*! ------------ Constants ------------ */

// buffer sizes
#define LEN 16 // maximum length of some string (for printing to look neat)
#define LONG_LEN 32
#define SMALL 64
#define MEDIUM 256
#define BIG 1024

// other sizes
#define MAX_ENTITIES 2048

// printing
#define nl "\n"
#define toStr(val) std::to_string(val)

// time
#define NOW std::chrono::steady_clock::now()
#define EPSILON 0.000001f

namespace Snail
{

	using namespace std::string_literals;

	/*! ------------ Type Definitions ------------ */

	using EntityId = size_t;
	using CompId = size_t;
	using TimerId = size_t;

	enum class Component : CompId
	{
		SIMPLE,
		MAX_COMPONENTS
	};

	using Signature = std::bitset<static_cast<size_t>(Component::MAX_COMPONENTS)>;

	namespace Util
	{

		inline bool isEqual(float flt1, float flt2)
		{
			return (flt1 - flt2 > 0 ? flt1 - flt2 : flt2 - flt1) <= EPSILON;
		}

		inline std::string quote(std::string const &str, char delim = '"')
		{
			std::ostringstream oss;
			oss << std::quoted(str, delim);
			return oss.str();
		}

		// @brief retrieves the function signature of the given function
		// @return a string representing the function signature
		template <typename Func>
		std::string getFunctionSignature(Func const &func) noexcept
		{
			unref(func);
			return typeid(typename std::remove_pointer<typename std::decay<Func>::type>::type).name();
		}

	}

}