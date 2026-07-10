#pragma once

#include "front.hpp"
#include "bifront.hpp"
#include <string>
#include <cstdint>

void next(int32_t N, int32_t M, BiFront &FrM, BiFront &FrI, BiFront &FrD, int32_t s, int32_t x, int32_t o, int32_t e, int32_t j, char c);

std::tuple<int32_t, int32_t, int32_t, char> try_jump(BiFront &FrM, int32_t s, int32_t j, int32_t M, int32_t N);

std::tuple<int32_t, int32_t, int32_t, char, int32_t, int32_t> overlap(BiFront &FrM, BiFront &FrI, BiFront &FrD, BiFront &FrM2, BiFront &FrI2, BiFront &FrD2, int32_t s, int32_t s2l, int32_t s2u, int32_t o, int32_t M, int32_t N);

char *lbiwfa(std::string_view q, std::string_view t, int32_t x, int32_t o, int32_t e, int32_t j, char c_f, char c_r, BiFront &FrM_f, BiFront &FrI_f, BiFront &FrD_f, BiFront &FrM_r, BiFront &FrI_r, BiFront &FrD_r, FrontXM &FrXM, Front &FrI, Front &FrD, char *aln_t);
