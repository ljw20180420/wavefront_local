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

    if (argc < 3)
    {
        std::cerr << "method is not provided\n";
        exit(1);
    }
    std::string method = argv[2];

    auto [l_c, x_c, o_c, e_c] = bowtie_score;

    for (const auto &[t, q] : bench)
    {
        parasail_matrix_t *matrix = parasail_matrix_create("ACGT", l_c, -x_c);
        parasail_result_t *result = nullptr;

        bench.tic();

        if (method == "striped")
        {
            result = parasail_sw_trace_striped_sat(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else if (method == "scan")
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
    std::cout << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
