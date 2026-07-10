#include "lwfa.hpp"
#include <limits>
#include <algorithm>
#include <utility>

std::tuple<int32_t, int32_t, int32_t> lwfa(std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j, const char c_f, const char c_r, FrontXM &FrXM, Front &FrI, Front &FrD)
{
    int32_t N = q.size();
    int32_t M = t.size();
    FrXM.reset(t, q);
    FrI.reset(M, N);
    FrD.reset(M, N);
    FrXM.new_wave();
    FrI.new_wave();
    FrD.new_wave();

    if (c_f == 'M')
    {
        FrXM.push(0, 0);
    }
    else if (c_f == 'I')
    {
        FrI.push(0, 0);
    }
    else if (c_f == 'D')
    {
        FrD.push(0, 0);
    }

    int32_t s = 0;
    int32_t s_t = std::numeric_limits<int32_t>::max();
    int32_t d_t = N - M;
    int32_t f_t = N + M;
    while (std::cmp_greater(s_t, s))
    {
        if (std::cmp_greater_equal(s, e))
        {
            int32_t Mdl, Mdu;
            if (std::cmp_greater_equal(s, o + e))
            {
                Mdl = FrXM.dl(s - o - e);
                Mdu = Mdl + FrXM.wave_size(s - o - e);
            }

            int32_t dl = std::numeric_limits<int32_t>::max();
            int32_t du = std::numeric_limits<int32_t>::min();
            if (std::cmp_greater_equal(s, o + e) && std::cmp_less(Mdl, Mdu))
            {
                dl = std::min(Mdl + 1, dl);
                du = std::max(Mdu + 1, du);
            }
            int32_t Idl = FrI.dl(s - e);
            int32_t Idu = Idl + FrI.wave_size(s - e);
            if (std::cmp_less(Idl, Idu))
            {
                dl = std::min(Idl + 1, dl);
                du = std::max(Idu + 1, du);
            }
            for (int32_t d = dl; std::cmp_less(d, du); ++d)
            {
                int32_t f = std::max(FrI.f(s - e, d - 1) + 1, FrXM.f(s - o - e, d - 1) + 1);
                FrI.push(d, f);
            }

            dl = std::numeric_limits<int32_t>::max();
            du = std::numeric_limits<int32_t>::min();
            if (std::cmp_greater_equal(s, o + e) && std::cmp_less(Mdl, Mdu))
            {
                dl = std::min(Mdl - 1, dl);
                du = std::max(Mdu - 1, du);
            }
            int32_t Ddl = FrD.dl(s - e);
            int32_t Ddu = Ddl + FrD.wave_size(s - e);
            if (std::cmp_less(Ddl, Ddu))
            {
                dl = std::min(Ddl - 1, dl);
                du = std::max(Ddu - 1, du);
            }
            for (int32_t d = dl; std::cmp_less(d, du); ++d)
            {
                int32_t f = std::max(FrD.f(s - e, d + 1) + 1, FrXM.f(s - o - e, d + 1) + 1);
                FrD.push(d, f);
            }
        }
        FrI.new_wave(); // wave_size for s is not accessible before new_wave call
        FrD.new_wave();

        int32_t dl = std::numeric_limits<int32_t>::max();
        int32_t du = std::numeric_limits<int32_t>::min();
        int32_t Idl = FrI.dl(s);
        int32_t Idu = Idl + FrI.wave_size(s);
        if (std::cmp_less(Idl, Idu))
        {
            dl = std::min(Idl, dl);
            du = std::max(Idu, du);
        }
        int32_t Ddl = FrD.dl(s);
        int32_t Ddu = Ddl + FrD.wave_size(s);
        if (std::cmp_less(Ddl, Ddu))
        {
            dl = std::min(Ddl, dl);
            du = std::max(Ddu, du);
        }
        if (std::cmp_greater_equal(s, x))
        {
            int32_t Mdl = FrXM.dl(s - x);
            int32_t Mdu = Mdl + FrXM.wave_size(s - x);
            if (std::cmp_less(Mdl, Mdu))
            {
                dl = std::min(Mdl, dl);
                du = std::max(Mdu, du);
            }
        }
        int32_t Jdl, Jdu;
        if (c_f == 'L' && std::cmp_equal(s % j, 0))
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
            int32_t f = std::max({FrI.f(s, d), FrD.f(s, d), FrXM.f(s - x, d) + 2});
            if (c_f == 'L' && std::cmp_equal(s % j, 0) && std::cmp_less_equal(Jdl, d) && std::cmp_less(d, Jdu) && std::cmp_equal((d - Jdl) % 2, 0))
            {
                f = std::max(s / j, f);
            }
            FrXM.push(d, f);
        }
        FrXM.new_wave();

        if (c_r == 'L')
        {
            int32_t Mdl = FrXM.dl(s);
            int32_t Mdu = Mdl + FrXM.wave_size(s);
            for (int32_t d = Mdl; std::cmp_less(d, Mdu); ++d)
            {
                int32_t f = FrXM.f(s, d);
                if (std::cmp_less(f, 0))
                {
                    continue;
                }
                int32_t new_score = s + (N + M - f) * j;
                if (std::cmp_less(new_score, s_t))
                {
                    s_t = new_score;
                    d_t = d;
                    f_t = f;
                }
            }
        }
        else
        {
            if (c_r == 'D' && std::cmp_equal(FrD.f(s, d_t), M + N) || c_r == 'I' && std::cmp_equal(FrI.f(s, d_t), M + N) || c_r == 'M' && std::cmp_equal(FrXM.f(s, d_t), M + N))
            {
                return std::make_tuple(s, d_t, f_t);
            }
        }

        ++s;
    }

    return std::make_tuple(s_t, d_t, f_t);
}

std::string track(const char c_f, const char c_r, const int32_t s_t, const int32_t d_t, const int32_t f_t, FrontXM &FrXM, Front &FrI, Front &FrD, const int32_t M, const int32_t N, const int32_t x, const int32_t o, const int32_t e, const int32_t j)
{
    std::string rev_aln;
    int32_t s_c;
    if (c_r == 'L')
    {
        s_c = s_t - (M + N - f_t) * j;
        int32_t n = (f_t + d_t) / 2;
        int32_t m = (f_t - d_t) / 2;
        rev_aln.append(N - n, 'S');
        rev_aln.append(M - m, 'N');
    }
    else
    {
        s_c = s_t;
    }

    int32_t d_c = d_t;
    char c_c = c_r;
    if (c_c == 'L')
    {
        c_c = 'M';
    }
    int32_t f_c = f_t;

    while (std::cmp_greater(f_c, 0))
    {
        if (c_c == 'M')
        {
            XMF &XMf = FrXM(s_c, d_c);
            rev_aln.append((XMf.Mf - XMf.Xf) / 2, 'M');
            if (std::cmp_equal(XMf.Xf, 0))
            {
                break;
            }
            if (c_f == 'L' && std::cmp_equal(s_c, XMf.Xf * j))
            {
                int32_t n = (XMf.Xf + d_c) / 2;
                int32_t m = (XMf.Xf - d_c) / 2;
                rev_aln.append(n, 'S');
                rev_aln.append(m, 'N');
                return rev_aln;
            }
            if (std::cmp_equal(FrXM.f(s_c - x, d_c) + 2, XMf.Xf))
            {
                rev_aln.append(1, 'M');
                s_c -= x;
                f_c = XMf.Xf - 2;
                c_c = 'M';
                continue;
            }
            if (std::cmp_equal(FrI.f(s_c, d_c), XMf.Xf))
            {
                f_c = XMf.Xf;
                c_c = 'I';
                continue;
            }
            if (std::cmp_equal(FrD.f(s_c, d_c), XMf.Xf))
            {
                f_c = XMf.Xf;
                c_c = 'D';
                continue;
            }
        }
        if (c_c == 'I')
        {
            if (std::cmp_equal(FrI.f(s_c - e, d_c - 1) + 1, f_c))
            {
                rev_aln.append(1, 'I');
                s_c -= e;
                --d_c;
                --f_c;
                continue;
            }
            if (std::cmp_equal(FrXM.f(s_c - o - e, d_c - 1) + 1, f_c))
            {
                rev_aln.append(1, 'I');
                s_c -= o + e;
                --d_c;
                --f_c;
                c_c = 'M';
                continue;
            }
        }
        if (c_c == 'D')
        {
            if (std::cmp_equal(FrD.f(s_c - e, d_c + 1) + 1, f_c))
            {
                rev_aln.append(1, 'D');
                s_c -= e;
                ++d_c;
                --f_c;
                continue;
            }
            if (std::cmp_equal(FrXM.f(s_c - o - e, d_c + 1) + 1, f_c))
            {
                rev_aln.append(1, 'D');
                s_c -= o + e;
                ++d_c;
                --f_c;
                c_c = 'M';
                continue;
            }
        }
    }

    return rev_aln;
}
