#include "../parasail/parasail.h"
#include "../src/score.hpp"
#include "../src/system.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <utility>

int main(int argc, char **argv)
{
    limit_memory(30UL * 1024UL * 1024UL * 1024UL);
    if (argc < 5)
    {
        std::exit(1);
    }
    std::string length = argv[1];
    std::string err_num = argv[2];
    std::string end_up = argv[3];
    std::string method = argv[4];

    auto [l_c, x_c, o_c, e_c] = bowtie_score;

    double total_time = 0.0;

    std::string benchmark_file = "paper/benchmark/data/" + length + '_' + err_num + '_' + end_up + ".csv";
    std::cerr << benchmark_file << '\n';

    std::ifstream fin(benchmark_file);
    if (!fin.is_open())
    {
        std::cerr << "Error: Could not open file.\n";
        std::exit(1);
    }

    std::string line;
    std::getline(fin, line);
    while (std::getline(fin, line))
    {
        std::string t, q;
        std::stringstream ss(line);
        std::getline(ss, t, ',');
        std::getline(ss, q);

        parasail_matrix_t *matrix = parasail_matrix_create("ACGT", l_c, -x_c);

        auto start = std::chrono::steady_clock::now();

        if (method == "striped")
        {
            parasail_result_t *result = parasail_sw_trace_striped_32(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else if (method == "scan")
        {
            parasail_result_t *result = parasail_sw_trace_scan_32(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }
        else
        {
            // method == "diag"
            parasail_result_t *result = parasail_sw_trace_diag_32(t.c_str(), t.size(), q.c_str(), q.size(), o_c, e_c, matrix);
        }

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        total_time += elapsed.count();
    };
    std::cout << length << ',' << err_num << ',' << end_up << ',' << total_time << ',' << peak_rss() << std::endl;
}
