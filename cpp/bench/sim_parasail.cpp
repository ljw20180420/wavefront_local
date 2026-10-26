#include "../parasail/parasail.h"
#include "../src/score.hpp"
#include "bench.hpp"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>

int main(int argc, char **argv)
{
    Bench bench = Bench(argc, argv);

    auto [l_c, x_c, o_c, e_c] = bowtie_score;

    for (const auto &[t, q] : bench)
    {
        parasail_matrix_t *matrix = parasail_matrix_create("ACGT", l_c, -x_c);
        parasail_result_t *result = nullptr;

        bench.tic();

        if (bench.method == "striped")
        {
            result = parasail_sw_trace_striped_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else if (bench.method == "scan")
        {
            result = parasail_sw_trace_scan_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else
        {
            // bench.method == "diag"
            result = parasail_sw_trace_diag_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }

        bench.doNotOptimize(result);

        bench.toc();
    };
    std::cout << bench.length << ',' << bench.err_num << ',' << bench.end_up << ',' << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
