#include <seqan3/alphabet/all.hpp>
#include <seqan3/alignment/all.hpp>
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

    // Configure the alignment kernel.
    auto config = seqan3::align_cfg::method_local{} |
                  seqan3::align_cfg::scoring_scheme{
                      seqan3::nucleotide_scoring_scheme{
                          seqan3::match_score{l_c},
                          seqan3::mismatch_score{-x_c}}} |
                  seqan3::align_cfg::gap_cost_affine{
                      seqan3::align_cfg::open_score{-o_c},
                      seqan3::align_cfg::extension_score{-e_c}};

    for (const auto &[t, q] : bench)
    {
        bench.tic();

        seqan3::dna4_vector s1, s2;
        s1.resize(t.size());
        for (size_t i = 0; i < t.size(); ++i)
            s1[i].assign_char(t[i]);
        s2.resize(q.size());
        for (size_t i = 0; i < q.size(); ++i)
            s2[i].assign_char(q[i]);

        // Invoke the pairwise alignment which returns a lazy range over alignment results.
        auto results = seqan3::align_pairwise(std::tie(s1, s2), config);
        auto &res = *results.begin();
        auto const &[gapped_s1, gapped_s2] = res.alignment();
        // Include this because seqan3 lazily skip the backtrack if the actually alignment is not accessed.
        auto view1 = gapped_s1 | seqan3::views::to_char;
        auto view2 = gapped_s2 | seqan3::views::to_char;
        std::string str1{std::ranges::begin(view1), std::ranges::end(view1)};
        std::string str2{std::ranges::begin(view2), std::ranges::end(view2)};

        bench.doNotOptimize(str1);
        bench.doNotOptimize(str2);

        bench.toc();
    };
    std::cout << bench.length << ',' << bench.err_num << ',' << bench.end_up << ',' << bench.total_time << ',' << bench.peak_rss() << std::endl;
}
