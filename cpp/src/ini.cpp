#include "ini.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <utility>

std::string strip(const std::string &str)
{
    const std::string whitespace = " \t\r\n\v\f";

    // Find the first character that is not a whitespace
    size_t start = str.find_first_not_of(whitespace);
    if (std::cmp_equal(start, std::string::npos))
    {
        return ""; // The string is completely empty or all whitespace
    }

    // Find the last character that is not a whitespace
    size_t end = str.find_last_not_of(whitespace);

    // Extract the substring containing only the valid characters
    return str.substr(start, end - start + 1);
}

std::variant<int, double, std::string> safe_converter(std::string v)
{
    std::variant<int, double, std::string> value;
    v = strip(v);
    try
    {
        if (std::cmp_not_equal(v.find('.'), std::string::npos))
        {
            value = v;
        }
        else
        {
            value = std::stoi(v);
        }
    }
    catch (const std::exception &e)
    {
        value = v;
    }
    return value;
}

std::map<std::string, std::vector<std::variant<int, double, std::string>>> read_ini(std::string file_name)
{
    std::ifstream fin(file_name);
    if (!fin.is_open())
    {
        std::cerr << "file is not open\n";
        std::exit(1);
    }
    std::map<std::string, std::vector<std::variant<int, double, std::string>>> cfg;
    std::string line;
    while (std::getline(fin, line))
    {
        std::stringstream ss(line);
        std::string key, value_str;
        std::getline(ss, key, '=');
        std::getline(ss, value_str);
        key = strip(key);

        std::vector<std::variant<int, double, std::string>> values;
        std::stringstream ss_value(value_str);
        std::string v;
        while (std::getline(ss_value, v, ','))
        {
            values.push_back(safe_converter(v));
        }
        cfg[key] = values;
    }

    return cfg;
}
