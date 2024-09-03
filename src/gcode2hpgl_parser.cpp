/**
 * @file gcode2hpgl_parser.cpp
 * @author Lars (lars.borntraeger@gmx.de)
 * @brief 
 * @version 0.1
 * @date 2024-08-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "gcode2hpgl_parser.hpp"

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>
#include <valarray>

using std::string;

/**
 * \f$ \mathrm{factor} = \frac{\text{hpgl steps}}{\text{gcode mm}} \f$
 * from lpkf91s manual: The step size is 7.9375 µm/step (6.35 mm/800 steps).
 */
static const double factor_mm = 0.0079375; 
static const double factor_inch = 0.0003125;
static const double* factor = &factor_mm;

static std::valarray<double> gcode_current_xy = {0.0, 0.0};
static std::valarray<double> gcode_current_origin = {3.125, 3.125};
static bool gcode_absolute_positioning = true; /// default: `true`, `true` after G90, `false` after G91

// `OH;` output : W42267,25088,0


/**
 * @brief split a string at space(= ' ')
 * 
 * @param in string to split
 * @return std::vector<string> vector of splitted strings
 */
static std::vector<string> split(const string in) {
	size_t last = 0, next = 0;
	std::vector<string> out;
	while((next = in.find(' ', last)) != string::npos) {
		out.push_back(in.substr(last, next-last));
		last = next + 1;
	}
	out.push_back(in.substr(last));
	return out;
};

/**
 * @brief move conversion
 * 
 * @param in string (full line) with `G0` or `G1` command
 * @return converted hpgl command 
 */
static string parse_g0_g1(const string in) {
	/// @todo check if absolute or relative
	string out = "";
	std::vector<string> splitted = split(in);

	// tool up/down
	auto z_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Z'; });
	if (z_ind != splitted.end()) {
		string z = splitted[z_ind - splitted.begin()];
		z.erase(0,1);
		if (std::stof(z) <= 0.0) /// @todo check for tool down
		{
			out += "PD;"; // pen down
		} 
		else if (std::stof(z) > 0.0) /// @todo check for tool up
		{
			out += "PU;"; // pen up
		}
	}

	// speed
	// auto f_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'F'; });
	// if (f_ind != splitted.end()) {
	// 	string f = splitted[f_ind - splitted.begin()];
	// 	f.erase(0,1);

	// 	/// @todo check for speed units hpgl is mm/s;  gcode is ???/min
	// 	out += "VS" + std::to_string(int(std::stod(f)/(*factor))) + ";"; 
	// }

	// xy coordinates
	auto x_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'X'; });
	auto y_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Y'; });
	if (x_ind != splitted.end() && y_ind != splitted.end()) {
		string x = splitted[x_ind - splitted.begin()];
		x.erase(0,1);

		string y = splitted[y_ind - splitted.begin()];
		y.erase(0,1);
		gcode_current_xy = gcode_current_origin + std::valarray<double>{std::stod(x), std::stod(y)};
		out += (gcode_absolute_positioning ? "PA" : "PR") +  std::to_string(int(gcode_current_xy[0]/(*factor))) + "," + std::to_string(int(gcode_current_xy[1]/(*factor))) + ";";	
	}
	return out;
};

/**
 * @brief arc conversion
 * 
 * @param in string (full line) with `G2` or `G3` command
 * @return converted hpgl command 
 */
static string parse_g2_g3(const string in) {
	// throw std::exception("gcode g2, g3 is not implemented correctly");
	/// @todo check if absolute or relative
	string out = "";
	std::vector<string> splitted = split(in);

		// speed
	// auto f_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'F'; });
	// if (f_ind != splitted.end()) {
	// 	string f = splitted[f_ind - splitted.begin()];
	// 	f.erase(0,1);
		
	// 	/// @todo check for speed units hpgl is mm/s;  gcode is ???/min
	// 	out += "VS" + std::to_string(int(std::stod(f)/(*factor))) + ";"; 
	// }

	// center coordinate
	auto i_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'I'; });
	string i = splitted[i_ind - splitted.begin()];
	i.erase(0,1);
	auto j_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'J'; });
	string j = splitted[j_ind - splitted.begin()];
	j.erase(0,1);
	auto gcode_center_coordinate = gcode_current_origin + std::valarray<double>{std::stod(i), std::stod(j)};

	// destination coordinate
	auto x_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'X'; });
	auto y_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Y'; });
	assert (x_ind != splitted.end() && y_ind != splitted.end()); // G02 or G03 requires X and Y coordinates
	string x = splitted[x_ind - splitted.begin()];
	x.erase(0,1);
	string y = splitted[y_ind - splitted.begin()];
	y.erase(0,1);
	auto gcode_destination_coordinate = gcode_current_origin + std::valarray<double>{std::stod(x), std::stod(y)};



	// angle
	double angle1 = atan2((gcode_center_coordinate[0] - gcode_current_xy[0]), (gcode_center_coordinate[1] - gcode_current_xy[1]));
	double angle2 = atan2((gcode_destination_coordinate[0] - gcode_current_xy[0]), (gcode_destination_coordinate[1] - gcode_current_xy[1]));
	double angle = angle2 - angle1;
	string a = std::to_string(int(std::round(angle)));	/// @todo have a clever idea on how to compute the angle. perhaps store the current position. if thats not possible, the hpgl cant be generated beforehand. :(
	if (a == "0") {
		std::cerr << "angle is 0: " << a << angle << std::endl;
		throw std::exception("angle is 0");
	}
	
	out += (gcode_absolute_positioning ? "AA" : "AR"); // absolute or relative
	out += std::to_string(int(gcode_center_coordinate[0]/(*factor))) + "," + std::to_string(int(gcode_center_coordinate[1]/(*factor))) + "," + a + ";";	
	return out;

};

/**
 * @brief wait
 * 
 * @param in string (full line) with `G4` command
 * @retval `!TW (t);`
 */
static string parse_g4(const string in) {
	assert (in.find("G04") != string::npos);
	std::vector<string> splitted = split(in);

	auto p_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'P'; });
	auto s_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'S'; });
	if (p_ind != splitted.end() && s_ind != splitted.end()) {
		throw std::exception("gcode g4: found p and s arguments. dont know how what to do with that.");
	} else if (p_ind != splitted.end()) {
		// P argument is in milliseconds
		string p = splitted[p_ind - splitted.begin()];
		p.erase(0,1);
		return "!TW" + std::to_string(std::stoi(p)) + ";";
	} else if (s_ind != splitted.end()) {
		// S argument is in seconds
		string s = splitted[s_ind - splitted.begin()];
		s.erase(0,1);
		return "!TW" + std::to_string(std::stoi(s)*1000) + ";";
	} 

	return "";
}

/**
 * @brief set XY plane
 * this is just here to make `Unknown command G17` dissapear
 * @param in 
 * @return empty string 
 * @retval ""
 */
static string parse_g17(const string in) {
	// set XY plane as the current plane for G2/G3 arc moves. We do nothing here, because thats the only option anyways
	assert (in.find("G17") != string::npos);
	return "";
}

/**
 * @brief set gcode units to inches
 * 
 * @param in 
 * @return string 
 */
static string parse_g20(const string in) {
	assert(in.find("G20") != string::npos);
	factor = &factor_inch;
	return "";
}

/**
 * @brief set gcode units to millimeters
 * 
 * @param in 
 * @return string 
 */
static string parse_g21(const string in) {
	assert(in.find("G21") != string::npos);
	factor = &factor_mm;
	return "";
}

/**
 * @brief set absolute positioning
 * 
 * @param in 
 * @retval "" 
 * @return empty string
 */
static string parse_g90(const string in) {
	assert (in.find("G90") != string::npos);
	gcode_absolute_positioning = true;
	return "";
};

/**
 * @brief set relative positioning
 * 
 * @param in 
 * @retval "" 
 * @return empty string
 */
static string parse_g91(const string in) {
	assert (in.find("G91") != string::npos);
	gcode_absolute_positioning = false;
	return "";
};

/**
 * @brief set position
 * i.e. give the current position a new name
 * @param in string (full line) with `G92` command
 * @return string 
 */
static string parse_g92(const string in) {
	// throw std::exception("G92 is not implemented yet");
	assert (in.find("G92") != string::npos);

	// xy coordinates
	std::vector<string> splitted = split(in);
	auto x_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'X'; });
	auto y_ind = std::find_if(splitted.begin(), splitted.end(), [](string s) { return s[0] == 'Y'; });
	if (x_ind != splitted.end() && y_ind != splitted.end()) {
		string x = splitted[x_ind - splitted.begin()];
		x.erase(0,1);

		string y = splitted[y_ind - splitted.begin()];
		y.erase(0,1);
		auto gcode_next_xy = std::valarray<double>{std::stod(x), std::stod(y)};
		gcode_current_origin = gcode_current_xy - gcode_next_xy;
		gcode_current_xy = gcode_next_xy;
	}

	return "";
};

/**
 * @brief set XY plane
 * this is just here to make `Unknown command M02` dissapear
 * @param in 
 * @return empty string 
 * @retval ""
 */
static string parse_m2(const string in) {
	// set XY plane as the current plane for G2/G3 arc moves. We do nothing here, because thats the only option anyways
	assert (in.find("M02") != string::npos);
	return "";
}

static string parse_m3(const string in) {
	return string("!EM1;"); // Spindle on, clockwise
};

static string parse_m5(const string in) {
	return string("!EM0;"); // Spindle off
};

static string parse_m6(const string in) {
	return string("!CM0;"); // change mode @todo what does flatcam actually produce here and is this needed
};


/// @todo allow non padded gcode
const std::map<std::string, std::string (*)(const std::string)> commands = {
	{"G00", parse_g0_g1},
	{"G0", parse_g0_g1},
	{"G01", parse_g0_g1},
	{"G1", parse_g0_g1},
	{"G02", parse_g2_g3},
	{"G2", parse_g2_g3},
	{"G03", parse_g2_g3},
	{"G3", parse_g2_g3},
	{"G04", parse_g4},
	{"G17", parse_g17},
	{"G20", parse_g20},
	{"G21", parse_g21},
	{"G90", parse_g90},
	{"G91", parse_g91},
	{"G92", parse_g92},
	{"M2", parse_m2},
	{"M02", parse_m2},
	{"M3", parse_m3},
	{"M03", parse_m3},
	{"M5", parse_m5},
	{"M05", parse_m5},
	{"M6", parse_m6},
	{"M06", parse_m6},
};
