#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>
#include <utility>
#include <limits>
#include <algorithm>

enum class IMD
{
    I, // Defaults to 0
    M, // Defaults to 1
    D  // Defaults to 2
};

template <int32_t p>
class BiTable
{
public:
    int32_t mid;
    std::vector<int32_t> fs;
    std::array<int32_t, 3 * (p + 1)> dls;
    std::array<int32_t, 3 * (p + 1)> dus;

    BiTable()
    {
        mid = 0;
        dls.fill(std::numeric_limits<int32_t>::max() / 2);
        dus.fill(std::numeric_limits<int32_t>::min() / 2);
    }

    void reset(const int32_t N, const int32_t M)
    {
        for (int32_t idx = 0; std::cmp_less_equal(idx, p); ++idx)
        {
            reset_row(idx);
        }
        int32_t mid_ = ((N + M) & 1) ? ((N + M + 1) / 2) : (N + M) / 2;
        if (mid_ > mid)
        {
            mid = mid_;
            fs.resize(3 * (p + 1) * (2 * mid_ + 1), std::numeric_limits<int32_t>::min() / 2);
        }
    }

    void reset_row(const int32_t idx)
    {
        for (IMD imd : {IMD::I, IMD::M, IMD::D})
        {
            for (int32_t d = dl(imd, idx); std::cmp_less(d, du(imd, idx)); ++d)
            {
                f(imd, idx, d) = std::numeric_limits<int32_t>::min() / 2;
            }
            dl(imd, idx) = std::numeric_limits<int32_t>::max() / 2;
            du(imd, idx) = std::numeric_limits<int32_t>::min() / 2;
        }
    }

    int32_t &f(IMD imd, int32_t idx, int32_t d)
    {
        return fs[static_cast<int32_t>(imd) + idx * 3 + (d + mid) * 3 * (p + 1)];
    }

    int32_t &dl(IMD imd, int32_t idx)
    {
        return dls[static_cast<int32_t>(imd) + idx * 3];
    }

    int32_t &du(IMD imd, int32_t idx)
    {
        return dus[static_cast<int32_t>(imd) + idx * 3];
    }
};

template <char orientation>
const int32_t extend(std::string_view q, std::string_view t, int32_t n, int32_t m)
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
