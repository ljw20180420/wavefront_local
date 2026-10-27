#include "../src/lwfa.hpp"
#include "../src/score.hpp"
#include "bench.hpp"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>

int main(int argc, char **argv)
{
    Bench bench = Bench(argc, argv);

    char c_f = 'L';
    char c_r = 'L';

    FrontXM FrXM;
    Front FrI, FrD;

    for (const auto &[t, q] : bench)
    {
        bench.tic();

        auto [s_t, d_t, f_t] = lwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);
        std::string aln = track<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
        bench.doNotOptimize(aln);

        // std::reverse(aln.begin(), aln.end());
        // int32_t s_t_aln = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
        // if (std::cmp_not_equal(s_t, s_t_aln))
        // {
        //     std::cerr << "inconsistant score\n";
        //     std::exit(1);
        // }

        bench.toc();
    }
    std::cout << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
