#pragma once

#include <tuple>
#include <string>

struct ScoreCanonical
{
    int32_t l_c;
    int32_t x_c;
    int32_t o_c;
    int32_t e_c;
};

const ScoreCanonical bowtie_score = {2, 6, 5, 3};
const ScoreCanonical bwa_score = {1, 4, 6, 1};
const ScoreCanonical blast_score = {1, 3, 5, 2};

std::tuple<int32_t, int32_t, int32_t, int32_t> score_system_canonical2wavefront(int32_t l_c, int32_t x_c, int32_t o_c, int32_t e_c);

int32_t score_wavefront2canonical(int32_t penalty, int32_t l_c, int32_t M, int32_t N);

int32_t eval_score(std::string_view aln, std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j);
