#pragma once

#include <map>
#include <vector>
#include <string>
#include <variant>

std::map<std::string, std::vector<std::variant<int, double, std::string>>> read_ini(std::string file_name);
