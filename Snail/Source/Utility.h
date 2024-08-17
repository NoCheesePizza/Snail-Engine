#pragma once

#include "Debug.h"
#include "Vec2.h"
#include "Types.h"

#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <iostream>

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
#define PRINT(...) Util::print(#__VA_ARGS__, __VA_ARGS__)
#define WHITESPACE " \n\t\r\v\f"

// time
#define NOW std::chrono::steady_clock::now()
#define EPSILON 0.000001f

namespace Snail
{

	using namespace std::string_literals;

	namespace Util
	{

		inline std::string toString(bool val)
		{
			return val ? "true" : "false";
		}

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

		// Helper function to split variable names
		inline std::vector<std::string> splitString(const std::string &toSplit, char delim = ' ')
		{
			std::string word;
			std::istringstream line(toSplit);
			std::vector<std::string> words;

			while (std::getline(line, word, delim))
				words.push_back(word);
			return words;
		}

		// @brief removes the leading and trailing whitespaces of the given string
		// @brief toTrim: the string to trim (this value is modified to be the same as the return value too)
		// @returns the trimmed string
		inline std::string trimString(std::string &toTrim)
		{
			size_t pos = toTrim.find_first_not_of(WHITESPACE);
			if (pos != std::string::npos)
				toTrim = toTrim.substr(pos);
			pos = toTrim.find_last_not_of(WHITESPACE);
			if (pos != std::string::npos)
				toTrim = toTrim.substr(0, pos + 1);
			return toTrim;
		}

		// warning: no error checking
		inline std::string trimString(std::string &toTrim, const std::string &toFind, bool isAtFront = true)
		{
			if (isAtFront)
				toTrim = toTrim.substr(toTrim.find(toFind) + toFind.length());
			else
				toTrim = toTrim.substr(0, toTrim.rfind(toFind));
			return toTrim;
		}

		template <typename T>
		std::string ptrToStr(T *ptr)
		{
			std::stringstream ss;
			ss << static_cast<const void *>(ptr);
			return ss.str();
		}

		// @brief retrieves the function signature of the given function
		// @return a string representing the function signature
		template <typename Func>
		std::string getFunctionSignature(Func const &func) noexcept
		{
			unref(func);
			return typeid(typename std::remove_pointer<typename std::decay<Func>::type>::type).name();
		}

		// Function to print variable names and values
		template<typename T>
		void printVariable(const std::string &name, const T &value)
		{
			std::cout << name << ": " << value;
		}

		// Base case for recursion
		inline void printVariables(const std::vector<std::string> &, size_t)
		{
			std::cout << nl;
		}

		// Function to print each variable with its name and value
		template<typename T, typename... Args>
		void printVariables(const std::vector<std::string> &names, size_t index,
			const T &value, const Args&... args)
		{
			if (index < names.size())
			{
				Util::printVariable(names[index], value);
				if (index < names.size() - 1)
					std::cout << ',';
				Util::printVariables(names, index + 1, args...);  // Recursive call to print next variable
			}
		}

		// Main function to print the variables
		template<typename... Args>
		void print(const std::string &namesStr, const Args&... args)
		{
			std::vector<std::string> names = Util::splitString(namesStr, ',');
			Util::printVariables(names, 0, args...);
		}

		template <typename T, size_t S>
		std::ostream &operator<<(std::ostream &os, const std::array<T, S> &container)
		{
			os << "[ ";
			for (auto curr = container.begin(); curr != container.end(); ++curr)
				os << *curr << (std::next(curr) == container.end() ? "" : ", ");
			os << " ]";
			return os;
		}

		template <typename T>
		std::ostream &operator<<(std::ostream &os, const std::vector<T> &container)
		{
			os << "[ ";
			for (auto curr = container.begin(); curr != container.end(); ++curr)
				os << *curr << (std::next(curr) == container.end() ? "" : nl);
			os << " ]";
			return os;
		}

	}

}