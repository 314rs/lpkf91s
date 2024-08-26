/**
 * @file gcode2hpgl_parser.test.cpp
 * @author Lars (lars.borntraeger@gmx.de)
 * @brief test the _behavior_ of the parser
 * @version 0.1
 * @date 2024-08-25
 * 
 * @copyright Copyright (c) 2024
 */
#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "gcode2hpgl_parser.hpp"
#include "gcode2hpgl_parser.cpp"

using std::string;


TEST(gcode2hpgl_parser, G00_0_0) {
	auto fun = commands.at(string("G00"));
	string str("G00 X0 Y0");
	string ret = fun(str);
	EXPECT_EQ(ret, string("PA0,0;")); ///@todo whitespace and missing semicolon are allowed
}

TEST(gcode2hpgl_parser, G00_X_only) {
	auto fun = commands.at(string("G00"));
	string str("G00 X100");
	string ret = fun(str);
	EXPECT_EQ(ret, string("PA0,0;"));
}

TEST(gcode2hpgl_parser, G00_Y_only) {
	auto fun = commands.at(string("G00"));
	string str("G00 Y1337");
	string ret = fun(str);
	EXPECT_EQ(ret, string("PA0,0;"));
}

TEST(gcode2hpgl_parser, EQ_G00_G01) {
	auto parseG00 = commands.at(string("G00"));
	auto parseG01 = commands.at(string("G01"));
	const string strs[] = {
		"G00 X0 Y0",
		"G01 X1234 Y5678",
		"G00 X42",
		"G01 Y69",
	};	
	for (auto str : strs) {
		EXPECT_EQ(parseG00(str), parseG01(str));
	}
}

TEST(gcode2hpgl_parser, G21_no_return) {
	// G21: unit to millimeters
	auto fun = commands.at(string("G21"));
	string str("G21");
	string ret = fun(str);
	EXPECT_EQ(ret, string(""));
}

TEST(gcode2hpgl_parser, G21_behavior) {
	// G21: unit to millimeters
	auto fun = commands.at(string("G21"));
	string lines[] = {"G21"};
	string out;
	for(auto line : lines) {
		string command = line.substr(0, line.find(' '));
		auto pos = commands.find(command);
		if (pos == commands.end()) {
			std::cerr << "Unknown command in line: \"" << line << "\" (skipping this line)" << std::endl;
		} else {
			std::cout << ((*pos->second)(line)) << std::endl;
			out += ((*pos->second)(line)) + "\n";
		}
	}
	EXPECT_EQ(out, string(""));
}

TEST(gcode2hpgl_parser, G90) {
	// G90: absolute positioning
	auto fun = commands.at(string("G90"));
	fun(string("G90"));
}

TEST(gcode2hpgl_parser, G91) {
	// G91: relative positioning
	auto fun = commands.at(string("G91"));
	fun(string("G91"));
}

TEST(gcode2hpgl_parser, G92) {
	// G92: set position
	auto fun = commands.at(string("G92"));
	fun(string("G92 X0 Y0"));
}

TEST(gcode2hpgl_parser, M0) {
	// M0: stop
	auto fun = commands.at(string("M00"));
	fun(string("M0"));
}

TEST(gcode2hpgl_parser, M3) {
	// M3: spindle on clockwise
	auto fun = commands.at(string("M03"));
	fun(string("M3"));
}

TEST(gcode2hpgl_parser, M5) {
	// M5: spindle off
	auto fun = commands.at(string("M05"));
	fun(string("M5"));
}

TEST(gcode2hpgl_parser, M6) {
	// M6: toolchange
	auto fun = commands.at(string("M06"));
	fun(string("M6"));
}

TEST(gcode2hpgl_parser, T1) {
	// T1: toolchange
	auto fun = commands.at(string("T01"));
	fun(string("T1"));
}
