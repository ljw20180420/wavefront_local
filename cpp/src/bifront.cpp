#include "bifront.hpp"
#include <utility>
#include <limits>

BiFront::BiFront(const char c_, const int32_t pp1, const char orientation_)
{
    c = c_;
    birows.resize(pp1);
    birows.shrink_to_fit();
    orientation = orientation_;
}

int32_t BiFront::extend(const int32_t n, const int32_t m)
{
    int32_t M = t.size();
    int32_t N = q.size();
    int32_t mu = 0;
    while (std::cmp_less(n + mu, N) && std::cmp_less(m + mu, M))
    {
        int32_t n_, m_;
        if (orientation == 'F')
        {
            n_ = n + mu;
            m_ = m + mu;
        }
        else
        {
            // orientation == 'R'
            n_ = N - n - mu - 1;
            m_ = M - m - mu - 1;
        }
        if (q[n_] != t[m_])
        {
            break;
        }
        ++mu;
    }
    return mu;
}

void BiFront::push(int32_t idx, int32_t d, int32_t f)
{
    int32_t M = t.size();
    int32_t N = q.size();
    BiRow &birow = birows[idx];
    if (f < 0)
    {
        if (std::cmp_less_equal(birow.dl, N))
        {
            ++invalid_num;
        }
    }
    else
    {
        int32_t n = (f + d) / 2;
        int32_t m = (f - d) / 2;
        if (std::cmp_greater(birow.dl, N))
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                birow.dl = d;
                int32_t ff = f;
                if (c == 'M')
                {
                    int32_t mu = extend(n, m);
                    ff += 2 * mu;
                }
                birow.fs.push_back(ff);
            }
        }
        else
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                birow.fs.resize(birow.fs.size() + invalid_num, std::numeric_limits<int32_t>::min());
                invalid_num = 0;
                int32_t ff = f;
                if (c == 'M')
                {
                    int32_t mu = extend(n, m);
                    ff += 2 * mu;
                }
                birow.fs.push_back(ff);
            }
            else
            {
                ++invalid_num;
            }
        }
    }
}

int32_t BiFront::f(int32_t idx, int32_t d)
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

int32_t BiFront::dl(int32_t idx)
{
    return birows[idx].dl;
}

int32_t BiFront::wave_size(int32_t idx)
{
    return birows[idx].fs.size();
}

char BiFront::get_c()
{
    return c;
}

void BiFront::reset(std::string_view t_, std::string_view q_)
{
    t = t_;
    q = q_;
    for (BiRow &birow : birows)
    {
        birow.fs.clear();
        birow.dl = q.size() + 1;
    }
    invalid_num = 0;
}

void BiFront::reset_row(int32_t idx)
{
    birows[idx].fs.clear();
    birows[idx].dl = q.size() + 1;
    invalid_num = 0;
}
