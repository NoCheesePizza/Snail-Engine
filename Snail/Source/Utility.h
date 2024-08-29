#pragma once

#include "Debug.h"
#include "Vec2.h"

#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>

/*! ------------ Macros ------------ */

// debugging
#define crashIf(condition, reason) do { if (condition) Debugger::log(-1, reason, __FILE__, __LINE__); } while (0)

// printing
#define nl "\n"
#define toStr(val) std::to_string(val)
#define printv(...) Util::print(#__VA_ARGS__, __VA_ARGS__)
#define whitespace " \n\t\r\v\f"

// convenience
#define gs(system) Core::getSystem<system>()
#define unref(param) param
#define rightNow std::chrono::steady_clock::now()

/*! ------------ Constants ------------ */

// buffer sizes
constexpr unsigned LEN = 16; // maximum length of some string (for printing to look neat)
constexpr unsigned LONG_LEN = 32;
constexpr unsigned SMALL = 64;
constexpr unsigned MEDIUM = 256;
constexpr unsigned BIG = 1024;
constexpr unsigned MAX_ENTITIES = 2048;
constexpr unsigned MAX_COMPONENTS = 2; // change if components are added/removed

// math
constexpr float EPSILON = 0.000001f;
constexpr float PI = 3.141592f;

/*! ------------ Functions ------------ */

namespace Snail
{

	using namespace std::string_literals;

	namespace Util
	{

		/*! ------------ Math ------------ */

		inline float radToDeg(float rad)
		{
			return rad * 180.f / static_cast<float>(PI);
		}

		inline float degToRad(float deg)
		{
			return deg * static_cast<float>(PI) / 180.f;
		}

		inline Vec2 rotate(Vec2 pos, float rad, Vec2 origin = Vec2())
		{
			Vec2 dir = pos - origin, temp;
			float length = dir.length();
			dir = dir.normalize();

			temp.x = cosf(rad) * dir.x - sinf(rad) * dir.y;
			temp.y = sinf(rad) * dir.x + cosf(rad) * dir.y;

			temp *= length;
			temp += origin;
			return temp;
		}

		inline float calcRot(Vec2 dir)
		{
			return std::atan2f(dir.y, dir.x);
		}

		inline int round(float val)
		{
			return ((static_cast<int>(val * 10.f) % 10 < 5) ? static_cast<int>(val) : static_cast<int>(val) + 1);
		}

		inline float clamp(float value, float lower, float upper)
		{
			return value > upper ? upper : value < lower ? lower : value;
		}

		inline bool isEqual(float flt1, float flt2)
		{
			return (flt1 - flt2 > 0.f ? flt1 - flt2 : flt2 - flt1) <= EPSILON;
		}

		template <typename T>
		T normalize(T val)
		{
			return val ? val / std::abs(val) : T();
		}

		/*! ------------ Strings ------------ */

		inline std::string toString(bool val)
		{
			return val ? "true" : "false";
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

		inline std::string trimString(std::string &toTrim)
		{
			size_t pos = toTrim.find_first_not_of(whitespace);
			if (pos != std::string::npos)
				toTrim = toTrim.substr(pos);
			pos = toTrim.find_last_not_of(whitespace);
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
			std::ostringstream oss;
			oss << static_cast<const void *>(ptr);
			return oss.str();
		}

		/*! ------------ Printing ------------ */

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
			const T &value, const Args &...args)
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
		void print(const std::string &namesStr, const Args &...args)
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

		/*! ------------ Random ------------ */

		inline int randInt(int min, int max)
		{
			return (max - min + 1) ? rand() % (max - min + 1) + min : min; // in case division by 0
		}

		inline float randFloat(float min, float max)
		{
			return static_cast<float>(rand()) / RAND_MAX * (max - min) + min;
		}

		/*! ------------ Lerps ------------ */

		inline float getPercent(float start, float end, float curr)
		{
			return (curr - start) / (end - start);
		}

		inline float lerp(float start, float end, float percentage)
		{
			return start + (end - start) * percentage;
		}

		inline float easeIn(float curr, float deg = 2.f)
		{
			return powf(curr, deg);
		}

		inline float flip(float curr)
		{
			return 1.f - curr;
		}

		inline float easeOut(float curr, float deg = 2.f)
		{
			return flip(powf(flip(curr), deg));
		}

		inline float easeInOut(float curr, float degIn = 2.f, float degOut = 2.f)
		{
			return lerp(easeIn(curr, degIn), easeOut(curr, degOut), curr);
		}

	}

}