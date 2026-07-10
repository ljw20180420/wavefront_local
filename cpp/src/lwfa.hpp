#pragma once

#include <string>
#include <tuple>
#include "front.hpp"

std::tuple<int32_t, int32_t, int32_t> lwfa(std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j, const char c_f, const char c_r, FrontXM &XM, Front &I, Front &D);

std::string track(const char c_f, const char c_r, const int32_t s_t, const int32_t d_t, const int32_t f_t, FrontXM &FrXM, Front &FrI, Front &FrD, const int32_t M, const int32_t N, const int32_t x, const int32_t o, const int32_t e, const int32_t j);
