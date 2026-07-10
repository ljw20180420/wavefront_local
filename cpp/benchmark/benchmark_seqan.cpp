#include <seqan3/alphabet/all.hpp>
#include <seqan3/alignment/all.hpp>
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

        seqan3::dna4_vector s1, s2;
        s1.resize(t.size());
        for (size_t i = 0; i < t.size(); ++i)
            s1[i].assign_char(t[i]);
        s2.resize(q.size());
        for (size_t i = 0; i < q.size(); ++i)
            s2[i].assign_char(q[i]);

        // Configure the alignment kernel.
        auto config = seqan3::align_cfg::method_local{} |
                      seqan3::align_cfg::scoring_scheme{
                          seqan3::nucleotide_scoring_scheme{
                              seqan3::match_score{l_c},
                              seqan3::mismatch_score{-x_c}}} |
                      seqan3::align_cfg::gap_cost_affine{
                          seqan3::align_cfg::open_score{-o_c},
                          seqan3::align_cfg::extension_score{-e_c}};

        auto start = std::chrono::steady_clock::now();

        // Invoke the pairwise alignment which returns a lazy range over alignment results.
        auto results = seqan3::align_pairwise(std::tie(s1, s2), config);
        auto &res = *results.begin();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        total_time += elapsed.count();
    };
    std::cout << length << ',' << err_num << ',' << end_up << ',' << total_time << ',' << peak_rss() << std::endl;
}
