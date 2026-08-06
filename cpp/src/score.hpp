#pragma once

#include <tuple>
#include <string>
#include <cstdint>
#include <utility>

struct ScoreCanonical
{
    int32_t l_c;
    int32_t x_c;
    int32_t o_c;
    int32_t e_c;
};

struct ScoreWFA
{
    int32_t x;
    int32_t o;
    int32_t e;
    int32_t j;
    int32_t p;
};

constexpr ScoreCanonical bowtie_score = {2, 6, 5, 3};
constexpr ScoreCanonical bwa_score = {1, 4, 6, 1};
constexpr ScoreCanonical blast_score = {1, 3, 5, 2};

constexpr ScoreWFA score_system_canonical2wavefront(const ScoreCanonical score_canonical)
{
    ScoreWFA score_wfa;
    score_wfa.x = 2 * score_canonical.l_c + 2 * score_canonical.x_c;
    score_wfa.o = 2 * score_canonical.o_c;
    score_wfa.e = 2 * score_canonical.e_c + score_canonical.l_c;
    score_wfa.j = score_canonical.l_c;
    score_wfa.p = std::max(score_wfa.x, score_wfa.o + score_wfa.e);

    return score_wfa;
}

constexpr ScoreWFA bowtie_wfa = score_system_canonical2wavefront(bowtie_score);
constexpr ScoreWFA bwa_wfa = score_system_canonical2wavefront(bwa_score);
constexpr ScoreWFA blast_wfa = score_system_canonical2wavefront(blast_score);

int32_t score_wavefront2canonical(int32_t penalty, int32_t l_c, int32_t M, int32_t N)
{
    return (l_c * (M + N) - penalty) / 2;
}

template <int32_t x, int32_t o, int32_t e, int32_t j>
int32_t eval_score(std::string_view aln, std::string_view q, std::string_view t)
{
    int32_t n = 0;
    int32_t m = 0;
    int32_t s = 0;
    for (int32_t i = 0; std::cmp_less(i, aln.size()); ++i)
    {
        char a = aln[i];
        if (a == 'N')
        {
            s += j;
            ++m;
        }
        else if (a == 'S')
        {
            s += j;
            ++n;
        }
        else if (a == 'M')
        {
            if (q[n] != t[m])
            {
                s += x;
            }
            ++n;
            ++m;
        }
        else if (a == 'I')
        {
            if (std::cmp_greater(i, 0) && aln[i - 1] == 'I')
            {
                s += e;
            }
            else
            {
                s += o + e;
            }
            ++n;
        }
        else
        {
            // a == "D"
            if (std::cmp_greater(i, 0) && aln[i - 1] == 'D')
            {
                s += e;
            }
            else
            {
                s += o + e;
            }
            ++m;
        }
    }
    return s;
}
