#pragma once

#include <vector>
#include <limits>
#include <string>
#include "utils.hpp"

struct Row
{
    int32_t dl; // dl > N means empty row
    int32_t wave;
};

class Front
{
private:
    int32_t M;               // text length
    int32_t N;               // query length
    std::vector<int32_t> fs; // offsets
    std::vector<Row> rows;   // row information
    int32_t invalid_num;     // the number of accumulated invalid offset, either out of bound or offset is -1 (does not exist)

public:
    void push(int32_t d, int32_t f);

    int32_t f(int32_t s, int32_t d);

    int32_t &operator()(int32_t s, int32_t d);

    int32_t dl(int32_t s);

    int32_t wave_size(int32_t s);

    void new_wave();

    void reset(int32_t M_, int32_t N_);
};

struct XMF
{
    int32_t Xf;
    int32_t Mf;
};

class FrontXM
{
private:
    std::string_view t;    // text
    std::string_view q;    // query
    std::vector<XMF> XMfs; // offsets
    std::vector<Row> rows; // row information
    int32_t invalid_num;   // the number of accumulated invalid offset, either out of bound or offset is -1 (does not exist)

public:
    void push(int32_t d, int32_t Xf);

    int32_t f(int32_t s, int32_t d);

    XMF &operator()(int32_t s, int32_t d);

    int32_t dl(int32_t s);

    int32_t wave_size(int32_t s);

    void new_wave();

    void reset(std::string_view t_, std::string_view q_);
};
