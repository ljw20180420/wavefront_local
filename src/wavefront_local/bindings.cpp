#include <pybind11/pybind11.h>
#include "lwfa.hpp"
#include "lbiwfa.hpp"
#include "score.hpp"

namespace py = pybind11;

std::tuple<int32_t, std::string> cpplwfa(std::string_view q, std::string_view t, std::string_view score_schema, const char c_f, const char c_r)
{
    FrontXM FrXM;
    Front FrI, FrD;

    int32_t s_t, d_t, f_t;
    std::string aln;
    if (score_schema == "bowtie")
    {
        std::tie(s_t, d_t, f_t) = lwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
        aln = track<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
    }
    else if (score_schema == "bwa")
    {
        std::tie(s_t, d_t, f_t) = lwfa<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
        aln = track<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
    }
    else
    {
        throw std::invalid_argument("score_schema must be bowtie or bwa");
    }
    std::reverse(aln.begin(), aln.end());

    return std::make_tuple(s_t, aln);
}

std::tuple<int32_t, std::string> cpplbiwfa(std::string_view q, std::string_view t, std::string_view score_schema, const char c_f, const char c_r)
{
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

    // resize aln to hold the full alignment
    aln.resize(t.size() + q.size());

    int32_t s_t;
    if (score_schema == "bowtie")
    {
        BiTable<bowtie_wfa.p> BiFr_f;
        BiTable<bowtie_wfa.p> BiFr_r;
        char *aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln.data());
        aln.resize(aln_t - aln.data());
        s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
    }
    else if (score_schema == "bwa")
    {
        BiTable<bwa_wfa.p> BiFr_f;
        BiTable<bwa_wfa.p> BiFr_r;
        char *aln_t = lbiwfa<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j, bwa_wfa.p>(q, t, c_f, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln.data());
        aln.resize(aln_t - aln.data());
        s_t = eval_score<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(aln, q, t);
    }
    else
    {
        throw std::invalid_argument("score_schema must be bowtie or bwa");
    }

    return std::make_tuple(s_t, aln);
}

PYBIND11_MODULE(_core, m, py::mod_gil_not_used())
{
    m.doc() = "wavefront algorithm for local alignment scope"; // optional module docstring

    m.def("cpplwfa", &cpplwfa, py::arg("q"), py::arg("t"), py::arg("score_schema"), py::arg("c_f"), py::arg("c_r"), "Get the optimal score, as well as the diagonal and offset of the end cell of the aligned region for tracing the alignment.");

    m.def("cpplbiwfa", &cpplbiwfa, py::arg("q"), py::arg("t"), py::arg("score_schema"), py::arg("c_f"), py::arg("c_r"), "Bidirectional version of lwfa.");
}
