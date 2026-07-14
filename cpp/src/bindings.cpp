#include "pybind11/pybind11.h"
#include "lwfa.hpp"
#include "lbiwfa.hpp"
#include "score.hpp"

namespace py = pybind11;

std::tuple<int32_t, std::string> cpplwfa(std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j, const char c_f, const char c_r)
{
    FrontXM FrXM;
    Front FrI, FrD;

    // get the optimal score
    auto [s_t, d_t, f_t] = lwfa(q, t, x, o, e, j, c_f, c_r, FrXM, FrI, FrD);

    // track the optimal alignment
    std::string aln = track(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size(), x, o, e, j);
    // Alignment string returned by track is reversed initially. Reverse it again to get the forward alignment string.
    std::reverse(aln.begin(), aln.end());

    return std::make_tuple(s_t, aln);
}

std::tuple<int32_t, std::string> cpplbiwfa(std::string_view q, std::string_view t, const int32_t x, const int32_t o, const int32_t e, const int32_t j, const char c_f, const char c_r)
{
    // preallocate containers
    int32_t p = std::max(x, o + e);
    BiFront FrM_f('M', p + 1, 'F');
    BiFront FrI_f('I', p + 1, 'F');
    BiFront FrD_f('D', p + 1, 'F');
    BiFront FrM_r('M', p + 1, 'R');
    BiFront FrI_r('I', p + 1, 'R');
    BiFront FrD_r('D', p + 1, 'R');
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

    // resize aln to hold the full alignment
    aln.resize(t.size() + q.size());

    // get the optimal alignment
    char *aln_t = lbiwfa(q, t, x, o, e, j, c_f, c_r, FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln.data());

    // evaluate the score of the optimal alignment
    aln.resize(aln_t - aln.data());
    int32_t s_t = eval_score(aln, q, t, x, o, e, j);

    return std::make_tuple(s_t, aln);
}

PYBIND11_MODULE(_core, m, py::mod_gil_not_used())
{
    m.doc() = "wavefront algorithm for local alignment scope"; // optional module docstring

    m.def("cpplwfa", &cpplwfa, "Get the optimal score, as well as the diagonal and offset of the end cell of the aligned region for tracing the alignment.");

    m.def("cpplbiwfa", &cpplbiwfa, "Bidirectional version of lwfa.");
}
