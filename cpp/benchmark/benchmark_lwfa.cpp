#include "../src/lwfa.hpp"
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
    if (argc < 4)
    {
        std::exit(1);
    }
    std::string length = argv[1];
    std::string err_num = argv[2];
    std::string end_up = argv[3];

    auto [l_c, x_c, o_c, e_c] = bowtie_score;
    auto [x, o, e, j] = score_system_canonical2wavefront(l_c, x_c, o_c, e_c);
    char c_f = 'L';
    char c_r = 'L';

    double total_time = 0.0;

    FrontXM FrXM;
    Front FrI, FrD;

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

        auto start = std::chrono::steady_clock::now();

        auto [s_t, d_t, f_t] = lwfa(q, t, x, o, e, j, c_f, c_r, FrXM, FrI, FrD);
        std::string aln = track(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size(), x, o, e, j);

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        // std::reverse(aln.begin(), aln.end());
        // int32_t s_t_aln = eval_score(aln, q, t, x, o, e, j);
        // if (std::cmp_not_equal(s_t, s_t_aln))
        // {
        //     std::cerr << "inconsistant score\n";
        //     std::exit(1);
        // }

        total_time += elapsed.count();
    }
    std::cout << length << ',' << err_num << ',' << end_up << ',' << total_time << ',' << peak_rss() << std::endl;
}
