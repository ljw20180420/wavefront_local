#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>
#include <utility>
#include <limits>

struct BiRow
{
    std::vector<int32_t> fs;
    int32_t dl; // dl > N means empty row
};

template <char c, int32_t p, char orientation>
class BiFront
{
private:
    std::string_view t;              // text
    std::string_view q;              // query
    std::array<BiRow, p + 1> birows; // row information

public:
    const int32_t extend(int32_t n, int32_t m)
    {
        const int32_t M = t.size();
        const int32_t N = q.size();
        if (orientation == 'F')
        {
            const int32_t n_o = n;
            for (; std::cmp_less(n, N) && std::cmp_less(m, M) && q[n] == t[m]; ++n, ++m)
            {
            }
            return n - n_o;
        }
        else
        {
            // orientation == 'R'
            n = N - n - 1;
            m = M - m - 1;
            const int32_t n_o = n;
            for (; std::cmp_greater_equal(n, 0) && std::cmp_greater_equal(m, 0) && q[n] == t[m]; --n, --m)
            {
            }
            return n_o - n;
        }
    }

    void push(const int32_t idx, const int32_t d, const int32_t f)
    {
        BiRow &birow = birows[idx];
        const int32_t n = (f + d) / 2;
        const int32_t m = (f - d) / 2;
        int32_t ff = f;
        if (c == 'M')
        {
            int32_t mu = extend(n, m);
            ff += 2 * mu;
        }
        if (birow.fs.empty())
        {
            birow.dl = d;
        }
        else
        {
            birow.fs.resize(d - birow.dl, std::numeric_limits<int32_t>::min());
        }
        birow.fs.push_back(ff);
    }

    int32_t f(int32_t idx, int32_t d)
    {
        if (std::cmp_less(idx, 0))
        {
            return std::numeric_limits<int32_t>::min();
        }
        BiRow &birow = birows[idx];
        if (std::cmp_less(d, birow.dl) || std::cmp_greater_equal(d - birow.dl, birow.fs.size()))
        {
            return std::numeric_limits<int32_t>::min();
        }
        return birow.fs[d - birow.dl];
    }

    int32_t dl(int32_t idx)
    {
        return birows[idx].dl;
    }

    int32_t wave_size(int32_t idx)
    {
        return birows[idx].fs.size();
    }

    void reset(std::string_view t_, std::string_view q_)
    {
        t = t_;
        q = q_;
        for (BiRow &birow : birows)
        {
            birow.fs.clear();
            birow.dl = q.size() + 1;
        }
    }

    void reset_row(int32_t idx)
    {
        birows[idx].fs.clear();
        birows[idx].dl = q.size() + 1;
    }
};
