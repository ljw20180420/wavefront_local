#include "../src/lbiwfa.hpp"
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

    char c_f = 'L';
    char c_r = 'L';

    double total_time = 0.0;

    BiFront<'M', bowtie_wfa.p, 'F'> FrM_f;
    BiFront<'I', bowtie_wfa.p, 'F'> FrI_f;
    BiFront<'D', bowtie_wfa.p, 'F'> FrD_f;
    BiFront<'M', bowtie_wfa.p, 'R'> FrM_r;
    BiFront<'I', bowtie_wfa.p, 'R'> FrI_r;
    BiFront<'D', bowtie_wfa.p, 'R'> FrD_r;
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

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

        aln.resize(t.size() + q.size());

        auto start = std::chrono::high_resolution_clock::now();

        char *aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln.data());
        aln.resize(aln_t - aln.data());
        int32_t s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
        doNotOptimize(s_t);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        total_time += elapsed.count();
    }
    std::cout << length << ',' << err_num << ',' << end_up << ',' << total_time << ',' << peak_rss() << std::endl;
}
