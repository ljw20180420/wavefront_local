#include "utils.hpp"
#include <utility>

int32_t extend(std::string_view q, std::string_view t, const int32_t n, const int32_t m)
{
    int32_t mu = 0;
    while (std::cmp_less(n + mu, q.size()) && std::cmp_less(m + mu, t.size()) && q[n + mu] == t[m + mu])
    {
        ++mu;
    }
    return mu;
}
