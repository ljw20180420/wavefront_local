#include "score.hpp"
#include <utility>

std::tuple<int32_t, int32_t, int32_t, int32_t> score_system_canonical2wavefront(int32_t l_c, int32_t x_c, int32_t o_c, int32_t e_c)
{
    int32_t x = 2 * l_c + 2 * x_c;
    int32_t o = 2 * o_c;
    int32_t e = 2 * e_c + l_c;
    int32_t j = l_c;

    return std::make_tuple(x, o, e, j);
}

int32_t score_wavefront2canonical(int32_t penalty, int32_t l_c, int32_t M, int32_t N)
{
    return (l_c * (M + N) - penalty) / 2;
}

int32_t eval_score(std::string_view aln, std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j)
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
