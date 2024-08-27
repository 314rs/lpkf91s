/**
 * @file gcode2hpgl_cli.cpp
 * @author Lars (lars.borntraeger@gmx.de)
 * @brief command line interface for gcode to hpgl conversion
 * @version 0.1
 * @date 2024-08-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "version.hpp"
#include "gcode2hpgl_parser.hpp"

using std::string;



int main(int argc, char *argv[])
{
	namespace po = boost::program_options;
	po::variables_map vm;
	try
	{
		po::positional_options_description p;
		p.add("input", 1);
		po::options_description desc{"Options"};
		desc.add_options()
		("help,h", "Help screen, show this message")
		("version" , "Show version number")
		("input,i", po::value<string>(), "input file name")
		("output,o", po::value<string>()->default_value("output.hpgl"), "output file name")
		("verbose,v", "Verbose output");

		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm); // important if this->notifier() is set on an option

		if (vm.count("version")) {
			std::cout << "Version: " << string(GIT_VERSION_STRING) << std::endl;
		}
		if (vm.count("help")) {
			std::cout << desc << '\n' << "developed by Lars Bornträger" << std::endl;
			return 0;
		}

	}
	catch (const po::error &ex)
	{
		std::cerr << ex.what() << '\n';
	}

	if (vm.count("input")) {
		std::ifstream in{vm["input"].as<string>()};
		std::ofstream out{vm["output"].as<string>()};

		/// @todo handle preamble 

		if (in.is_open()) {
			for(std::string line; std::getline(in, line);) {
				if (line.empty()) {continue;}
				if (line[0] == '(') {if (vm.count("verbose")){std::cout << "Ignoring comment: " << line << std::endl;} continue;}
				string command = line.substr(0, line.find(' '));
				auto pos = commands.find(command);
				if (pos == commands.end()) {
					std::cerr << "Unknown command in line: \"" << line << "\" (skipping this line)" << std::endl;
				} else {
					try {
						
						out << ((*pos->second)(line)) << std::endl;
						if (vm.count("verbose")) {
							std::cout << line << "\n->\t" << ((*pos->second)(line)) << std::endl;
						}
					} catch (const std::exception &ex) {
						std::cerr << "Exception while transpiling : " << line << "\n" << ex.what() << std::endl;
					}
				}
			}
		}

		/// @todo handle postamble

		std::cout << "Done. Check output file: " << vm["output"].as<string>() << std::endl;	
	}
	
	return 0;
}