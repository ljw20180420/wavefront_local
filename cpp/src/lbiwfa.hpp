#pragma once

#include "front.hpp"
#include "bifront.hpp"
#include "lbiwfa.hpp"
#include "lwfa.hpp"
#include <string>
#include <utility>
#include <cstring>
#include <ranges>
#include <limits>
#include <algorithm>
#include <tuple>
#include <functional>

template <int32_t gap, int32_t p>
int32_t loop_minus(int32_t idx, int32_t s)
{
    int32_t idx_ = idx - gap;
    if (std::cmp_less_equal(s, gap) || std::cmp_greater_equal(idx_, 0))
    {
        return idx_;
    }
    return idx_ + p + 1;
}

template <int32_t p>
int32_t loop_minus(int32_t idx, int32_t s, int32_t gap)
{
    int32_t idx_ = idx - gap;
    if (std::cmp_less_equal(s, gap) || std::cmp_greater_equal(idx_, 0))
    {
        return idx_;
    }
    return idx_ + p + 1;
}

template <int32_t p>
int32_t loop_next(int32_t idx)
{
    ++idx;
    if (std::cmp_greater(idx, p))
    {
        return 0;
    }
    return idx;
}

template <int32_t x, int32_t o, int32_t e, int32_t j, int32_t p, char orientation>
void next(int32_t N, int32_t M, BiFront<'M', p, orientation> &FrM, BiFront<'I', p, orientation> &FrI, BiFront<'D', p, orientation> &FrD, int32_t s, int32_t idx, char c)
{
    bool jump = std::cmp_equal(s % j, 0);
    int32_t idx_x = loop_minus<x, p>(idx, s);
    int32_t idx_e = loop_minus<e, p>(idx, s);
    int32_t idx_o = loop_minus<o + e, p>(idx, s);
    if (std::cmp_greater_equal(s, e))
    {
        int32_t Mdl, Mdu;
        if (std::cmp_greater_equal(s, o + e))
        {
            Mdl = FrM.dl(idx_o);
            Mdu = Mdl + FrM.wave_size(idx_o);
        }

        int32_t dl = std::numeric_limits<int32_t>::max();
        int32_t du = std::numeric_limits<int32_t>::min();
        if (std::cmp_greater_equal(s, o + e) && std::cmp_less(Mdl, Mdu))
        {
            dl = std::min(Mdl + 1, dl);
            du = std::max(Mdu + 1, du);
        }
        int32_t Idl = FrI.dl(idx_e);
        int32_t Idu = Idl + FrI.wave_size(idx_e);
        if (std::cmp_less(Idl, Idu))
        {
            dl = std::min(Idl + 1, dl);
            du = std::max(Idu + 1, du);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d)
        {
            int32_t f = std::max(FrI.f(idx_e, d - 1) + 1, FrM.f(idx_o, d - 1) + 1);
            FrI.push(idx, d, f);
        }

        dl = std::numeric_limits<int32_t>::max();
        du = std::numeric_limits<int32_t>::min();
        if (std::cmp_greater_equal(s, o + e) && std::cmp_less(Mdl, Mdu))
        {
            dl = std::min(Mdl - 1, dl);
            du = std::max(Mdu - 1, du);
        }
        int32_t Ddl = FrD.dl(idx_e);
        int32_t Ddu = Ddl + FrD.wave_size(idx_e);
        if (std::cmp_less(Ddl, Ddu))
        {
            dl = std::min(Ddl - 1, dl);
            du = std::max(Ddu - 1, du);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d)
        {
            int32_t f = std::max(FrD.f(idx_e, d + 1) + 1, FrM.f(idx_o, d + 1) + 1);
            FrD.push(idx, d, f);
        }
    }

    int32_t dl = std::numeric_limits<int32_t>::max();
    int32_t du = std::numeric_limits<int32_t>::min();
    int32_t Idl = FrI.dl(idx);
    int32_t Idu = Idl + FrI.wave_size(idx);
    if (std::cmp_less(Idl, Idu))
    {
        dl = std::min(Idl, dl);
        du = std::max(Idu, du);
    }
    int32_t Ddl = FrD.dl(idx);
    int32_t Ddu = Ddl + FrD.wave_size(idx);
    if (std::cmp_less(Ddl, Ddu))
    {
        dl = std::min(Ddl, dl);
        du = std::max(Ddu, du);
    }
    if (std::cmp_greater_equal(s, x))
    {
        int32_t Mdl = FrM.dl(idx_x);
        int32_t Mdu = Mdl + FrM.wave_size(idx_x);
        if (std::cmp_less(Mdl, Mdu))
        {
            dl = std::min(Mdl, dl);
            du = std::max(Mdu, du);
        }
    }
    int32_t Jdl, Jdu;
    if (c == 'L' && jump)
    {
        int32_t f = s / j;
        Jdl = std::max(-f, f - 2 * M);
        Jdu = std::min(f, 2 * N - f) + 1;
        if (std::cmp_less(Jdl, Jdu))
        {
            dl = std::min(Jdl, dl);
            du = std::max(Jdu, du);
        }
    }
    for (int32_t d = dl; std::cmp_less(d, du); ++d)
    {
        int32_t f = std::max({FrI.f(idx, d), FrD.f(idx, d), FrM.f(idx_x, d) + 2});
        if (c == 'L' && jump && std::cmp_less_equal(Jdl, d) && std::cmp_less(d, Jdu) && std::cmp_equal(d & 1, Jdl & 1))
        {
            f = std::max(s / j, f);
        }
        FrM.push(idx, d, f);
    }
}

template <int32_t j, int32_t p, char orientation>
std::tuple<int32_t, int32_t, int32_t> try_jump(BiFront<'M', p, orientation> &FrM, int32_t s, int32_t idx, int32_t M, int32_t N)
{
    if (FrM.wave_size(idx) == 0)
    {
        return std::make_tuple(std::numeric_limits<int32_t>::max(), -1, -1);
    }

    int32_t d_b, s_b;
    int32_t f_b = std::numeric_limits<int32_t>::min();
    int32_t dl = FrM.dl(idx);
    int32_t du = dl + FrM.wave_size(idx);
    for (int32_t d = dl; std::cmp_less(d, du); ++d)
    {
        int32_t f = FrM.f(idx, d);
        if (std::cmp_greater(f, f_b))
        {
            f_b = f;
            d_b = d;
            s_b = s + (M + N - f_b) * j;
        }
    }

    return std::make_tuple(s_b, d_b, f_b);
}

template <char c, int32_t o, int32_t p, char orientation1, char orientation2>
std::tuple<int32_t, int32_t, int32_t, char> overlap_c(BiFront<c, p, orientation1> &Fr, BiFront<c, p, orientation2> &Fr2, int32_t s,
                                                      int32_t idx,
                                                      int32_t s2l,
                                                      int32_t s2u,
                                                      int32_t M,
                                                      int32_t N)
{
    int32_t s_b = std::numeric_limits<int32_t>::max();
    int32_t d_b, f_b;
    char c_b;
    int32_t dl = Fr.dl(idx);
    int32_t du = dl + Fr.wave_size(idx);
    for (int32_t s2 = s2l; s2 < s2u; ++s2)
    {
        int32_t idx2 = loop_minus<p>(idx, s, s - s2);
        int32_t d2l = Fr2.dl(idx2);
        int32_t d2u = d2l + Fr2.wave_size(idx2);
        for (int32_t d = std::max(dl, N - M - d2u + 1); std::cmp_less(d, std::min(du, N - M - d2l + 1)); ++d)
        {
            int32_t d2 = N - M - d;
            int32_t f = Fr.f(idx, d);
            int32_t f2 = Fr2.f(idx2, d2);
            if (f + f2 >= N + M)
            {
                int32_t new_score = s + s2;
                if (c != 'M')
                {
                    new_score -= o;
                }
                if (new_score < s_b)
                {
                    s_b = new_score;
                    d_b = d;
                    f_b = f;
                    c_b = c;
                }
            }
        }
    }

    return std::make_tuple(s_b, d_b, f_b, c_b);
}

template <int32_t o, int32_t p, char orientation1, char orientation2>
std::tuple<int32_t, int32_t, int32_t, char, int32_t, int32_t> overlap(
    BiFront<'M', p, orientation1> &FrM,
    BiFront<'I', p, orientation1> &FrI,
    BiFront<'D', p, orientation1> &FrD,
    BiFront<'M', p, orientation2> &FrM2,
    BiFront<'I', p, orientation2> &FrI2,
    BiFront<'D', p, orientation2> &FrD2,
    int32_t s,
    int32_t idx,
    int32_t s2l,
    int32_t s2u,
    int32_t M,
    int32_t N)
{
    int32_t s_b = std::numeric_limits<int32_t>::max();
    int32_t d_b, f_b;
    char c_b;
    {
        auto [s_c, d_c, f_c, c_c] = overlap_c<'M', o, p, orientation1, orientation2>(FrM, FrM2, s, idx, s2l, s2u, M, N);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = c_c;
        }
    }
    {
        auto [s_c, d_c, f_c, c_c] = overlap_c<'I', o, p, orientation1, orientation2>(FrI, FrI2, s, idx, s2l, s2u, M, N);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = c_c;
        }
    }
    {
        auto [s_c, d_c, f_c, c_c] = overlap_c<'D', o, p, orientation1, orientation2>(FrD, FrD2, s, idx, s2l, s2u, M, N);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = c_c;
        }
    }
    int32_t score_upper = s;
    if (c_b != 'M')
    {
        score_upper -= o;
    }
    int32_t score_lower = s_b - s;

    return std::make_tuple(s_b, d_b, f_b, c_b, score_upper, score_lower);
}

template <int32_t x, int32_t o, int32_t e, int32_t j, int32_t p, char c_f, char c_r>
char *lbiwfa(std::string_view q, std::string_view t, BiFront<'M', p, 'F'> &FrM_f, BiFront<'I', p, 'F'> &FrI_f, BiFront<'D', p, 'F'> &FrD_f, BiFront<'M', p, 'R'> &FrM_r, BiFront<'I', p, 'R'> &FrI_r, BiFront<'D', p, 'R'> &FrD_r, FrontXM &FrXM, Front &FrI, Front &FrD, char *aln_t)
{
    int32_t M = t.size();
    int32_t N = q.size();
    FrM_f.reset(t, q);
    FrI_f.reset(t, q);
    FrD_f.reset(t, q);
    FrM_r.reset(t, q);
    FrI_r.reset(t, q);
    FrD_r.reset(t, q);

    if (c_f == 'M')
    {
        FrM_f.push(0, 0, 0);
    }
    else if (c_f == 'I')
    {
        FrI_f.push(0, 0, 0);
    }
    else if (c_f == 'D')
    {
        FrD_f.push(0, 0, 0);
    }

    if (c_r == 'M')
    {
        FrM_r.push(0, 0, 0);
    }
    else if (c_r == 'I')
    {
        FrI_r.push(0, 0, 0);
    }
    else if (c_r == 'D')
    {
        FrD_r.push(0, 0, 0);
    }

    int32_t s = 0;
    int32_t idx = 0;
    int32_t s_t = std::numeric_limits<int32_t>::max();
    int32_t d_t, f_t;
    char c_t;
    int32_t anti_diag_upper = -1;
    int32_t anti_diag_lower = -1;
    int32_t score_upper_t, score_lower_t;
    char split; // F (forward jump) or R (reverse jump) or P (path)
    while (std::cmp_less(2 * s - o - p, s_t))
    {
        next<x, o, e, j, p, 'F'>(N, M, FrM_f, FrI_f, FrD_f, s, idx, c_f);
        if (c_r == 'L')
        {
            auto [s_b, d_b, f_b] = try_jump<j, p, 'F'>(FrM_f, s, idx, M, N);
            if (std::cmp_less(s_b, s_t))
            {
                s_t = s_b;
                d_t = d_b;
                f_t = f_b;
                score_upper_t = s;
                split = 'F';
            }
        }
        next<x, o, e, j, p, 'R'>(N, M, FrM_r, FrI_r, FrD_r, s, idx, c_r);
        if (c_f == 'L')
        {
            auto [s_b, d_b, f_b] = try_jump<j, p, 'R'>(FrM_r, s, idx, M, N);
            if (std::cmp_less(s_b, s_t))
            {
                d_b = N - M - d_b;
                f_b = N + M - f_b;
                s_t = s_b;
                d_t = d_b;
                f_t = f_b;
                score_lower_t = s;
                split = 'R';
            }
        }

        int32_t dl = FrM_f.dl(idx);
        int32_t du = dl + FrM_f.wave_size(idx);
        for (int32_t d = dl; std::cmp_less(d, du); ++d)
        {
            anti_diag_upper = std::max(FrM_f.f(idx, d), anti_diag_upper);
        }
        dl = FrM_r.dl(idx);
        du = dl + FrM_r.wave_size(idx);
        for (int32_t d = dl; std::cmp_less(d, du); ++d)
        {
            anti_diag_lower = std::max(FrM_r.f(idx, d), anti_diag_lower);
        }

        if (anti_diag_upper + anti_diag_lower >= M + N)
        {
            {
                auto [s_b, d_b, f_b, c_b, score_upper, score_lower] = overlap<o, p, 'F', 'R'>(FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, s, idx, s - p, s, M, N);
                if (s_b < s_t)
                {
                    s_t = s_b;
                    d_t = d_b;
                    f_t = f_b;
                    c_t = c_b;
                    score_upper_t = score_upper;
                    score_lower_t = score_lower;
                    split = 'P';
                }
            }
            {
                auto [s_b, d_b, f_b, c_b, score_lower, score_upper] = overlap<o, p, 'R', 'F'>(FrM_r, FrI_r, FrD_r, FrM_f, FrI_f, FrD_f, s, idx, s - p, s + 1, M, N);
                if (s_b < s_t)
                {
                    d_b = N - M - d_b;
                    f_b = N + M - f_b;
                    s_t = s_b;
                    d_t = d_b;
                    f_t = f_b;
                    c_t = c_b;
                    score_upper_t = score_upper;
                    score_lower_t = score_lower;
                    split = 'P';
                }
            }
        }

        ++s;
        idx = loop_next<p>(idx);
        FrM_f.reset_row(idx);
        FrI_f.reset_row(idx);
        FrD_f.reset_row(idx);
        FrM_r.reset_row(idx);
        FrI_r.reset_row(idx);
        FrD_r.reset_row(idx);
    }

    int32_t n = (f_t + d_t) / 2;
    int32_t m = (f_t - d_t) / 2;
    if (split == 'F')
    {
        if (score_upper_t <= p)
        {
            auto [s_t, d_t, f_t] = lwfa<x, o, e, j>(q.substr(0, n), t.substr(0, m), c_f, 'M', FrXM, FrI, FrD);
            std::string aln = track<x, o, e, j>(c_f, 'M', s_t, d_t, f_t, FrXM, FrI, FrD, m, n);
            for (auto it = aln.rbegin(); it != aln.rend(); ++it, ++aln_t)
            {
                *aln_t = *it;
            }
        }
        else
        {
            aln_t = lbiwfa<x, o, e, j, p, c_f, 'M'>(q.substr(0, n), t.substr(0, m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
        }
        std::memset(aln_t, 'S', N - n);
        aln_t += N - n;
        std::memset(aln_t, 'N', M - m);
        aln_t += M - m;
    }
    else if (split == 'R')
    {
        std::memset(aln_t, 'S', n);
        aln_t += n;
        std::memset(aln_t, 'N', m);
        aln_t += m;
        if (score_lower_t <= p)
        {
            auto [s_t, d_t, f_t] = lwfa<x, o, e, j>(q.substr(n), t.substr(m), 'M', c_r, FrXM, FrI, FrD);
            std::string aln = track<x, o, e, j>('M', c_r, s_t, d_t, f_t, FrXM, FrI, FrD, M - m, N - n);
            for (auto it = aln.rbegin(); it != aln.rend(); ++it, ++aln_t)
            {
                *aln_t = *it;
            }
        }
        else
        {
            aln_t = lbiwfa<x, o, e, j, p, 'M', c_r>(q.substr(n), t.substr(m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
        }
    }
    else
    {
        if (score_upper_t <= p)
        {
            auto [s_t, d_t, f_t] = lwfa<x, o, e, j>(q.substr(0, n), t.substr(0, m), c_f, c_t, FrXM, FrI, FrD);
            std::string aln = track<x, o, e, j>(c_f, c_t, s_t, d_t, f_t, FrXM, FrI, FrD, m, n);
            for (auto it = aln.rbegin(); it != aln.rend(); ++it, ++aln_t)
            {
                *aln_t = *it;
            }
        }
        else
        {
            if (c_t == 'M')
            {
                aln_t = lbiwfa<x, o, e, j, p, c_f, 'M'>(q.substr(0, n), t.substr(0, m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
            else if (c_t == 'I')
            {
                aln_t = lbiwfa<x, o, e, j, p, c_f, 'I'>(q.substr(0, n), t.substr(0, m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
            else
            {
                aln_t = lbiwfa<x, o, e, j, p, c_f, 'D'>(q.substr(0, n), t.substr(0, m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
        }
        if (score_lower_t <= p)
        {
            auto [s_t, d_t, f_t] = lwfa<x, o, e, j>(q.substr(n), t.substr(m), c_t, c_r, FrXM, FrI, FrD);
            std::string aln = track<x, o, e, j>(c_t, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, M - m, N - n);
            for (auto it = aln.rbegin(); it != aln.rend(); ++it, ++aln_t)
            {
                *aln_t = *it;
            }
        }
        else
        {
            if (c_t == 'M')
            {
                aln_t = lbiwfa<x, o, e, j, p, 'M', c_r>(q.substr(n), t.substr(m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
            else if (c_t == 'I')
            {
                aln_t = lbiwfa<x, o, e, j, p, 'I', c_r>(q.substr(n), t.substr(m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
            else
            {
                aln_t = lbiwfa<x, o, e, j, p, 'D', c_r>(q.substr(n), t.substr(m), FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln_t);
            }
        }
    }
    return aln_t;
}
