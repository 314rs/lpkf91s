/**
 * @file gcode2hpgl_parser.hpp
 * @author Lars (lars.borntraeger@gmx.de)
 * @brief 
 * @version 0.1
 * @date 2024-08-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <map>
#include <string>

extern const std::map<std::string, std::string (*)(const std::string)> commands;