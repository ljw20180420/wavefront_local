#include "front.hpp"
#include <utility>

void Front::push(int32_t d, int32_t f)
{
    if (std::cmp_less(f, 0))
    {
        if (std::cmp_less_equal(rows.back().dl, N))
        {
            ++invalid_num;
        }
    }
    else
    {
        int32_t n = (f + d) / 2;
        int32_t m = (f - d) / 2;
        if (std::cmp_greater(rows.back().dl, N))
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                rows.back().dl = d;
                fs.push_back(f);
            }
        }
        else
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                fs.resize(fs.size() + invalid_num, std::numeric_limits<int32_t>::min());
                invalid_num = 0;
                fs.push_back(f);
            }
            else
            {
                ++invalid_num;
            }
        }
    }
}

int32_t Front::f(int32_t s, int32_t d)
{
    if (std::cmp_less(s, 0) || std::cmp_greater_equal(s, rows.size()))
    {
        return std::numeric_limits<int32_t>::min();
    }
    Row row = rows[s];
    if (std::cmp_less(d, row.dl) || std::cmp_greater_equal(d - row.dl, rows[s + 1].wave - row.wave))
    {
        return std::numeric_limits<int32_t>::min();
    }
    return fs[row.wave + d - row.dl];
}

int32_t &Front::operator()(int32_t s, int32_t d)
{
    Row row = rows[s];
    return fs[row.wave + d - row.dl];
}

int32_t Front::dl(int32_t s)
{
    return rows[s].dl;
}

int32_t Front::wave_size(int32_t s)
{
    return rows[s + 1].wave - rows[s].wave;
}

void Front::new_wave()
{
    rows.emplace_back(N + 1, fs.size());
    invalid_num = 0;
}

void Front::reset(int32_t M_, int32_t N_)
{
    M = M_;
    N = N_;
    fs.clear();
    rows.clear();
};

void FrontXM::push(int32_t d, int32_t Xf)
{
    int32_t N = q.size();
    int32_t M = t.size();

    if (std::cmp_less(Xf, 0))
    {
        if (std::cmp_less_equal(rows.back().dl, N))
        {
            ++invalid_num;
        }
    }
    else
    {
        int32_t n = (Xf + d) / 2;
        int32_t m = (Xf - d) / 2;
        if (std::cmp_greater(rows.back().dl, N))
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                rows.back().dl = d;
                int32_t mu = extend(q, t, n, m);
                int32_t Mf = Xf + 2 * mu;
                XMfs.emplace_back(Xf, Mf);
            }
        }
        else
        {
            if (std::cmp_less_equal(n, N) && std::cmp_less_equal(m, M))
            {
                XMfs.resize(XMfs.size() + invalid_num, {std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()});
                invalid_num = 0;
                int32_t mu = extend(q, t, n, m);
                int32_t Mf = Xf + 2 * mu;
                XMfs.emplace_back(Xf, Mf);
            }
            else
            {
                ++invalid_num;
            }
        }
    }
}

int32_t FrontXM::f(int32_t s, int32_t d)
{
    if (std::cmp_less(s, 0) || std::cmp_greater_equal(s, rows.size()))
    {
        return std::numeric_limits<int32_t>::min();
    }
    Row row = rows[s];
    if (std::cmp_less(d, row.dl) || std::cmp_greater_equal(d - row.dl, rows[s + 1].wave - row.wave))
    {
        return std::numeric_limits<int32_t>::min();
    }
    return XMfs[row.wave + d - row.dl].Mf;
}

XMF &FrontXM::operator()(int32_t s, int32_t d)
{
    Row row = rows[s];
    return XMfs[row.wave + d - row.dl];
}

int32_t FrontXM::dl(int32_t s)
{
    return rows[s].dl;
}

int32_t FrontXM::wave_size(int32_t s)
{
    return rows[s + 1].wave - rows[s].wave;
}

void FrontXM::new_wave()
{
    rows.emplace_back(q.size() + 1, XMfs.size());
    invalid_num = 0;
}

void FrontXM::reset(std::string_view t_, std::string_view q_)
{
    t = t_;
    q = q_;
    XMfs.clear();
    rows.clear();
}
