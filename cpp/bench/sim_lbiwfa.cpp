#include "../src/lbiwfa.hpp"
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

    BiTable<bowtie_wfa.p> BiFr_f;
    BiTable<bowtie_wfa.p> BiFr_r;
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

    for (const auto &[t, q] : bench)
    {
        aln.resize(t.size() + q.size());

        bench.tic();

        char *aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln.data());
        aln.resize(aln_t - aln.data());
        int32_t s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
        bench.doNotOptimize(s_t);

        bench.toc();
    }
    std::cout << bench.length << ',' << bench.err_num << ',' << bench.end_up << ',' << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
