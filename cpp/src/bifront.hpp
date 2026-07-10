#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>

struct BiRow
{
    std::vector<int32_t> fs;
    int32_t dl; // dl > N means empty row
};

class BiFront
{
private:
    std::string_view t;        // text
    std::string_view q;        // query
    char c;                    // M or I or D
    std::vector<BiRow> birows; // row information
    char orientation;          // F (forward) or R (reverse)
    int32_t invalid_num;       // the number of accumulated invalid offset, either out of bound or offset is -1 (does not exist)

public:
    BiFront(const char c_, const int32_t pp1, const char orientation_);
    int32_t extend(const int32_t n, const int32_t m);
    void push(int32_t idx, int32_t d, int32_t f);
    int32_t f(int32_t idx, int32_t d);
    char get_c();
    int32_t dl(int32_t idx);
    int32_t wave_size(int32_t idx);
    void reset(std::string_view t_, std::string_view q_);
    void reset_row(int32_t idx);
};
