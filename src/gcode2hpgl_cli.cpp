#include <iostream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <boost/program_options.hpp>
#include "project_version.h"

using std::string;

const double factor = 0.0079375;

std::vector<string> split(string in) {
	size_t last = 0, next = 0;
	std::vector<string> out;
	while((next = in.find(' ', last)) != string::npos) {
		out.push_back(in.substr(last, next-last));
		last = next + 1;
	}
	out.push_back(in.substr(last));
	return out;
}

string parse_g0_g1(string in) {
	/// @todo check if absolute or relative
	std::ostringstream out;
	std::vector<string> splitted = split(in);

	// tool up/down
	auto z_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Z'; });
	if (z_ind != splitted.end()) {
		string z = splitted[z_ind - splitted.begin()];
		z.erase(0,1);
		if (std::stof(z) <= 0.0) /// @todo check for tool down
		{
			out << "PD;"; // pen down
		} 
		else if (std::stof(z) > 0.0) /// @todo check for tool up
		{
			out << "PU;"; // pen up
		}
	}

	// speed
	// auto f_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'F'; });
	// if (f_ind != splitted.end()) {
	// 	string f = splitted[f_ind - splitted.begin()];
	// 	f.erase(0,1);
	// 	out << "VS" << int(std::stod(f)/factor) << ";"; /// @todo check for speed units hpgl is mm/s;  gcode is ???/min
	// }
	// xy coordinates
	auto x_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'X'; });
	auto y_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Y'; });
	if (x_ind != splitted.end() && y_ind != splitted.end()) {
		string x = splitted[x_ind - splitted.begin()];
		x.erase(0,1);

		string y = splitted[y_ind - splitted.begin()];
		y.erase(0,1);
		out << "PA" << int(std::stod(x)/factor) << "," << int(std::stod(y)/factor) << ";";	
	}
	return out.str();
};

string parse_g2_g3(string in) {
	throw "gcode g2, g3 is not implemented correctly";
	/// @todo check if absolute or relative
	string out = "";
	std::vector<string> splitted = split(in);

	// speed
	auto f_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'F'; });
	string f = splitted[f_ind - splitted.begin()];
	f.erase(0,1);
	out += "VS" + f + ";"; /// @todo check for speed units hpgl is mm/s;  gcode is ???/min

	// xy coordinates
	auto i_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'I'; });
	string i = splitted[i_ind - splitted.begin()];
	i.erase(0,1);
	auto j_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'J'; });
	string j = splitted[j_ind - splitted.begin()];
	j.erase(0,1);

	// angle
	string a = "0";	/// @todo have a clever idea on how to compute the angle. perhaps store the current position. if thats not possible, the hpgl cant be generated beforehand. :(
	out += "AA" + i + "," + j + "," + a + ";";	

}

string parse_m3(string in) {
	return string("!EM1;"); // Spindle on, clockwise
};

string parse_m5(string in) {
	return string("!EM0;"); // Spindle off
}

string parse_m6(string in) {
	return string("!CM0;"); // change mode @todo what does flatcam actually produce here and is this needed
}

std::map<string, string (*)(string)> commands = {
	{"G00", parse_g0_g1}, 
	{"G01", parse_g0_g1},
	// {"G02", parse_g2_g3},
	// {"G03", parse_g2_g3},
	{"M03", parse_m3},
	{"M5", parse_m5}
};


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
		("output,o", po::value<string>()->default_value("output.hpgl"), "output file name");

		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm); // important if this->notifier() is set on an option

		if (vm.count("version")) {
			std::cout << "Version: " << string(PROJECT_VERSION) << std::endl;
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
				string command = line.substr(0, line.find(' '));
				auto pos = commands.find(command);
				if (pos == commands.end()) {
					std::cerr << "Unknown command in line: \"" << line << "\" (skipping this line)" << std::endl;
				} else {
					std::cout << ((*pos->second)(line)) << std::endl;
					out << ((*pos->second)(line)) << std::endl;
				}
			}
		}

		/// @todo handle postamble

		std::cout << "Done. Check output file: " << vm["output"].as<string>() << std::endl;	
	}
	
	return 0;
}