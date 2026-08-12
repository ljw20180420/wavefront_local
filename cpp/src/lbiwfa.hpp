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
int32_t loop_minus(int32_t idx)
{
    int32_t idx_ = idx - gap;
    if (std::cmp_greater_equal(idx_, 0))
    {
        return idx_;
    }
    return idx_ + p + 1;
}

template <int32_t p>
int32_t loop_minus(int32_t idx, int32_t gap)
{
    int32_t idx_ = idx - gap;
    if (std::cmp_greater_equal(idx_, 0))
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
void next(const std::string_view q, const std::string_view t, BiTable<p> &BiFr, const int32_t s, const int32_t idx, const char c)
{
    const int32_t N = q.size();
    const int32_t M = t.size();
    const int32_t idx_x = loop_minus<x, p>(idx);
    const int32_t idx_e = loop_minus<e, p>(idx);
    const int32_t idx_o = loop_minus<o + e, p>(idx);
    {
        int32_t dl = BiFr.dl(IMD::I, idx_e);
        int32_t du = BiFr.du(IMD::I, idx_e);
        BiFr.dl(IMD::I, idx) = std::min(dl + 1, BiFr.dl(IMD::I, idx));
        BiFr.du(IMD::I, idx) = std::max(du + 1, BiFr.du(IMD::I, idx));
        int32_t ii = static_cast<int32_t>(IMD::I) + idx * 3 + (dl + 1 + BiFr.mid) * 3 * (p + 1);
        int32_t jj = ii - 3 * (p + 1) - 3 * e;
        if (std::cmp_greater_equal(idx_e, idx))
        {
            jj += 3 * (p + 1);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d, ii += 3 * (p + 1), jj += 3 * (p + 1))
        {
            BiFr.fs[ii] = std::max(BiFr.fs[jj] + 1, BiFr.fs[ii]);
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::D, idx_e);
        int32_t du = BiFr.du(IMD::D, idx_e);
        BiFr.dl(IMD::D, idx) = std::min(dl - 1, BiFr.dl(IMD::D, idx));
        BiFr.du(IMD::D, idx) = std::max(du - 1, BiFr.du(IMD::D, idx));
        int32_t ii = static_cast<int32_t>(IMD::D) + idx * 3 + (dl - 1 + BiFr.mid) * 3 * (p + 1);
        int32_t jj = ii + 3 * (p + 1) - 3 * e;
        if (std::cmp_greater_equal(idx_e, idx))
        {
            jj += 3 * (p + 1);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d, ii += 3 * (p + 1), jj += 3 * (p + 1))
        {
            BiFr.fs[ii] = std::max(BiFr.fs[jj] + 1, BiFr.fs[ii]);
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::M, idx_o);
        int32_t du = BiFr.du(IMD::M, idx_o);
        BiFr.dl(IMD::I, idx) = std::min(dl + 1, BiFr.dl(IMD::I, idx));
        BiFr.du(IMD::I, idx) = std::max(du + 1, BiFr.du(IMD::I, idx));
        BiFr.dl(IMD::D, idx) = std::min(dl - 1, BiFr.dl(IMD::D, idx));
        BiFr.du(IMD::D, idx) = std::max(du - 1, BiFr.du(IMD::D, idx));
        int32_t jj = static_cast<int32_t>(IMD::M) + idx_o * 3 + (dl + BiFr.mid) * 3 * (p + 1);
        int32_t iiI = jj + (static_cast<int32_t>(IMD::I) - static_cast<int32_t>(IMD::M)) + 3 * (p + 1) + 3 * (o + e);
        int32_t iiD = jj + (static_cast<int32_t>(IMD::D) - static_cast<int32_t>(IMD::M)) - 3 * (p + 1) + 3 * (o + e);
        if (std::cmp_greater_equal(idx_o, idx))
        {
            iiI -= 3 * (p + 1);
            iiD -= 3 * (p + 1);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d, jj += 3 * (p + 1), iiI += 3 * (p + 1), iiD += 3 * (p + 1))
        {
            BiFr.fs[iiI] = std::max(BiFr.fs[jj] + 1, BiFr.fs[iiI]);
            BiFr.fs[iiD] = std::max(BiFr.fs[jj] + 1, BiFr.fs[iiD]);
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::M, idx_x);
        int32_t du = BiFr.du(IMD::M, idx_x);
        BiFr.dl(IMD::M, idx) = std::min(dl, BiFr.dl(IMD::M, idx));
        BiFr.du(IMD::M, idx) = std::max(du, BiFr.du(IMD::M, idx));
        int32_t ii = static_cast<int32_t>(IMD::M) + idx * 3 + (dl + BiFr.mid) * 3 * (p + 1);
        int32_t jj = ii - 3 * x;
        if (std::cmp_greater_equal(idx_x, idx))
        {
            jj += 3 * (p + 1);
        }
        for (int32_t d = dl; std::cmp_less(d, du); ++d, ii += 3 * (p + 1), jj += 3 * (p + 1))
        {
            BiFr.fs[ii] = std::max(BiFr.fs[jj] + 2, BiFr.fs[ii]);
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::I, idx);
        int32_t du = BiFr.du(IMD::I, idx);
        BiFr.dl(IMD::M, idx) = std::min(dl, BiFr.dl(IMD::M, idx));
        BiFr.du(IMD::M, idx) = std::max(du, BiFr.du(IMD::M, idx));
        int32_t ii = static_cast<int32_t>(IMD::M) + idx * 3 + (dl + BiFr.mid) * 3 * (p + 1);
        for (int32_t d = dl; d < du; ++d, ii += 3 * (p + 1))
        {
            BiFr.fs[ii] = std::max(BiFr.fs[ii + (static_cast<int32_t>(IMD::I) - static_cast<int32_t>(IMD::M))], BiFr.fs[ii]);
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::D, idx);
        int32_t du = BiFr.du(IMD::D, idx);
        BiFr.dl(IMD::M, idx) = std::min(dl, BiFr.dl(IMD::M, idx));
        BiFr.du(IMD::M, idx) = std::max(du, BiFr.du(IMD::M, idx));
        int32_t ii = static_cast<int32_t>(IMD::M) + idx * 3 + (dl + BiFr.mid) * 3 * (p + 1);
        for (int32_t d = dl; d < du; ++d, ii += 3 * (p + 1))
        {
            BiFr.fs[ii] = std::max(BiFr.fs[ii + (static_cast<int32_t>(IMD::D) - static_cast<int32_t>(IMD::M))], BiFr.fs[ii]);
        }
    }

    if (c == 'L' && std::cmp_equal(s % j, 0))
    {
        int32_t f = s / j;
        int32_t dl = std::max(-f, f - 2 * M);
        int32_t du = std::min(f, 2 * N - f) + 1;
        BiFr.dl(IMD::M, idx) = std::min(dl, BiFr.dl(IMD::M, idx));
        BiFr.du(IMD::M, idx) = std::max(du, BiFr.du(IMD::M, idx));
        for (int32_t d = dl; d < du; d += 2)
        {
            BiFr.f(IMD::M, idx, d) = std::max(f, BiFr.f(IMD::M, idx, d));
        }
    }

    {
        int32_t dl = BiFr.dl(IMD::M, idx);
        int32_t du = BiFr.du(IMD::M, idx);
        for (int32_t d = dl; d < du; ++d)
        {
            int32_t f = BiFr.f(IMD::M, idx, d);
            if (std::cmp_less(f, 0))
            {
                continue;
            }
            int32_t n = (f + d) / 2;
            int32_t m = (f - d) / 2;
            if (std::cmp_greater_equal(n, N) || std::cmp_greater_equal(m, M))
            {
                continue;
            }
            int32_t mu = extend<orientation>(q, t, n, m);
            BiFr.f(IMD::M, idx, d) += 2 * mu;
        }
    }
}

template <int32_t j, int32_t p>
std::tuple<int32_t, int32_t, int32_t> try_jump(int32_t N, int32_t M, BiTable<p> &BiFr, int32_t s, int32_t idx)
{
    int32_t s_b = std::numeric_limits<int32_t>::max() / 2;
    int32_t d_b;
    int32_t f_b = std::numeric_limits<int32_t>::min() / 2;
    for (int32_t d = BiFr.dl(IMD::M, idx); std::cmp_less(d, BiFr.du(IMD::M, idx)); ++d)
    {
        int32_t f = BiFr.f(IMD::M, idx, d);
        if (std::cmp_greater(f, f_b))
        {
            f_b = f;
            d_b = d;
            s_b = s + (M + N - f_b) * j;
        }
    }

    return std::make_tuple(s_b, d_b, f_b);
}

template <IMD imd, int32_t o, int32_t p>
std::tuple<int32_t, int32_t, int32_t> overlap_c(int32_t N, int32_t M, BiTable<p> &BiFr, BiTable<p> &BiFr2, int32_t s, int32_t idx, int32_t s2l, int32_t s2u)
{
    int32_t s_b = std::numeric_limits<int32_t>::max() / 2;
    int32_t d_b, f_b;
    int32_t dl = BiFr.dl(imd, idx);
    int32_t du = BiFr.du(imd, idx);
    for (int32_t s2 = s2l; s2 < s2u; ++s2)
    {
        int32_t idx2 = loop_minus<p>(idx, s - s2);
        int32_t d2l = BiFr2.dl(imd, idx2);
        int32_t d2u = BiFr2.du(imd, idx2);
        for (int32_t d = std::max(dl, N - M - d2u + 1); std::cmp_less(d, std::min(du, N - M - d2l + 1)); ++d)
        {
            int32_t d2 = N - M - d;
            int32_t f = BiFr.f(imd, idx, d);
            int32_t f2 = BiFr2.f(imd, idx2, d2);
            if (f + f2 >= N + M)
            {
                int32_t new_score = s + s2;
                if (imd != IMD::M)
                {
                    new_score -= o;
                }
                if (new_score < s_b)
                {
                    s_b = new_score;
                    d_b = d;
                    f_b = f;
                }
            }
        }
    }

    return std::make_tuple(s_b, d_b, f_b);
}

template <int32_t o, int32_t p>
std::tuple<int32_t, int32_t, int32_t, char, int32_t, int32_t> overlap(int32_t N, int32_t M, BiTable<p> &BiFr, BiTable<p> &BiFr2, int32_t s, int32_t idx, int32_t s2l, int32_t s2u)
{
    int32_t s_b = std::numeric_limits<int32_t>::max() / 2;
    int32_t d_b, f_b;
    char c_b = '\0';
    {
        auto [s_c, d_c, f_c] = overlap_c<IMD::M, o, p>(N, M, BiFr, BiFr2, s, idx, s2l, s2u);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = 'M';
        }
    }
    {
        auto [s_c, d_c, f_c] = overlap_c<IMD::I, o, p>(N, M, BiFr, BiFr2, s, idx, s2l, s2u);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = 'I';
        }
    }
    {
        auto [s_c, d_c, f_c] = overlap_c<IMD::D, o, p>(N, M, BiFr, BiFr2, s, idx, s2l, s2u);
        if (s_c < s_b)
        {
            s_b = s_c;
            d_b = d_c;
            f_b = f_c;
            c_b = 'D';
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

template <int32_t x, int32_t o, int32_t e, int32_t j, int32_t p>
char *lbiwfa(std::string_view q, std::string_view t, char c_f, char c_r, BiTable<p> &BiFr_f, BiTable<p> &BiFr_r, FrontXM &FrXM, Front &FrI, Front &FrD, char *aln_t)
{
    int32_t M = t.size();
    int32_t N = q.size();
    BiFr_f.reset(N, M);
    BiFr_r.reset(N, M);

    int32_t idx = 0;
    if (c_f == 'M')
    {
        BiFr_f.f(IMD::M, idx, 0) = 0;
        BiFr_f.dl(IMD::M, idx) = 0;
        BiFr_f.du(IMD::M, idx) = 1;
    }
    else if (c_f == 'I')
    {
        BiFr_f.f(IMD::I, idx, 0) = 0;
        BiFr_f.dl(IMD::I, idx) = 0;
        BiFr_f.du(IMD::I, idx) = 1;
    }
    else if (c_f == 'D')
    {
        BiFr_f.f(IMD::D, idx, 0) = 0;
        BiFr_f.dl(IMD::D, idx) = 0;
        BiFr_f.du(IMD::D, idx) = 1;
    }

    if (c_r == 'M')
    {
        BiFr_r.f(IMD::M, idx, 0) = 0;
        BiFr_r.dl(IMD::M, idx) = 0;
        BiFr_r.du(IMD::M, idx) = 1;
    }
    else if (c_r == 'I')
    {
        BiFr_r.f(IMD::I, idx, 0) = 0;
        BiFr_r.dl(IMD::I, idx) = 0;
        BiFr_r.du(IMD::I, idx) = 1;
    }
    else if (c_r == 'D')
    {
        BiFr_r.f(IMD::D, idx, 0) = 0;
        BiFr_r.dl(IMD::D, idx) = 0;
        BiFr_r.du(IMD::D, idx) = 1;
    }

    int32_t s = 0;
    int32_t s_t = std::numeric_limits<int32_t>::max() / 2;
    int32_t d_t, f_t;
    char c_t = '\0';
    int32_t anti_diag_upper = -1;
    int32_t anti_diag_lower = -1;
    bool anti_diag_cross = false;
    int32_t score_upper_t, score_lower_t;
    char split; // F (forward jump) or R (reverse jump) or P (path)
    while (std::cmp_less(2 * s - o - p, s_t))
    {
        next<x, o, e, j, p, 'F'>(q, t, BiFr_f, s, idx, c_f);
        if (c_r == 'L')
        {
            auto [s_b, d_b, f_b] = try_jump<j, p>(N, M, BiFr_f, s, idx);
            if (std::cmp_less(s_b, s_t))
            {
                s_t = s_b;
                d_t = d_b;
                f_t = f_b;
                score_upper_t = s;
                split = 'F';
            }
        }
        next<x, o, e, j, p, 'R'>(q, t, BiFr_r, s, idx, c_r);
        if (c_f == 'L')
        {
            auto [s_b, d_b, f_b] = try_jump<j, p>(N, M, BiFr_r, s, idx);
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

        if (!anti_diag_cross)
        {
            for (int32_t d = BiFr_f.dl(IMD::M, idx); std::cmp_less(d, BiFr_f.du(IMD::M, idx)); ++d)
            {
                anti_diag_upper = std::max(BiFr_f.f(IMD::M, idx, d), anti_diag_upper);
            }
            for (int32_t d = BiFr_r.dl(IMD::M, idx); std::cmp_less(d, BiFr_r.du(IMD::M, idx)); ++d)
            {
                anti_diag_lower = std::max(BiFr_r.f(IMD::M, idx, d), anti_diag_lower);
            }
            anti_diag_cross = std::cmp_greater_equal(anti_diag_upper + anti_diag_lower, M + N);
        }

        if (anti_diag_cross)
        {
            {
                auto [s_b, d_b, f_b, c_b, score_upper, score_lower] = overlap<o, p>(N, M, BiFr_f, BiFr_r, s, idx, s - p, s);
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
                auto [s_b, d_b, f_b, c_b, score_lower, score_upper] = overlap<o, p>(N, M, BiFr_r, BiFr_f, s, idx, s - p, s + 1);
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
        BiFr_f.reset_row(idx);
        BiFr_r.reset_row(idx);
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
            aln_t = lbiwfa<x, o, e, j, p>(q.substr(0, n), t.substr(0, m), c_f, 'M', BiFr_f, BiFr_r, FrXM, FrI, FrD, aln_t);
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
            aln_t = lbiwfa<x, o, e, j, p>(q.substr(n), t.substr(m), 'M', c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln_t);
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
            aln_t = lbiwfa<x, o, e, j, p>(q.substr(0, n), t.substr(0, m), c_f, c_t, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln_t);
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
            aln_t = lbiwfa<x, o, e, j, p>(q.substr(n), t.substr(m), c_t, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln_t);
        }
    }
    return aln_t;
}
