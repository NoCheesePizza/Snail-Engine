#include "Debug.h"
#include "Core.h"

#include <csignal>
#include <fstream>
#include <iostream>

namespace Snail
{

	namespace Debugger
	{

		std::vector<std::string> signals; // interrupt vector table (literally just copied from OS slides)

		void handleSignal(int signal)
		{
			Debugger::log(signal);
		}

		void initSignalHandler()
		{
			// load signal names from file
			std::ifstream ifs("Assets/Data/signals.txt");
			crashIf(!ifs, "Unable to open signal.txt for reading");
			std::string temp;

			while (std::getline(ifs, temp))
				signals.emplace_back(temp);

			for (int i = 0; i < NSIG; ++i)
				signal(i, handleSignal);
		}

		void log(int signal, const std::string &reason, const std::string &fileName, int line)
		{
			// get local time
			std::time_t now = std::time(nullptr);
			std::tm date;
			localtime_s(&date, &now);

			// format file name, date, and time
			char format[SMALL] = { 0 };
			snprintf(format, SMALL, "[%02d/%02d/%d %02d:%02d:%02d]",
				date.tm_mday, date.tm_mon + 1, date.tm_year + 1900, date.tm_hour, date.tm_min, date.tm_sec);
			std::string toLog = format;

			if (signal == -1) // user-induced crash
				toLog += " Program crashed at line " + std::to_string(line) + " in " +
				fileName.substr(fileName.find_last_of('\\') + 1) + ", reason: " + reason + nl + nl;
			else // OS-induced crash
				toLog += " Program crashed due to " + signals[signal] + " (" + std::to_string(signal) + ")" + nl + nl;

			// open log file for logging
			std::ofstream ofs("Assets/Data/log.txt", std::ios_base::app);
			std::cout << toLog;
			Core::free();

			if (ofs)
			{
				ofs << toLog;
				exit(signal);
			}

			std::cout << "Unable to open log.txt for appending\n";
			exit(signal);
		}

		std::ostream &operator<<(std::ostream &os, const Printable &that)
		{
			os << that.stringify();
			return os;
		}

	}

}