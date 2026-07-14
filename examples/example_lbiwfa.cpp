#include "../cpp/src/lbiwfa.hpp"
#include "../cpp/src/score.hpp"
#include "../cpp/src/system.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // load classical scoring system like bowtie
    auto [l_c, x_c, o_c, e_c] = bowtie_score;
    // transform the classical scoring system to the equivalent wavefront scoring system
    auto [x, o, e, j] = score_system_canonical2wavefront(l_c, x_c, o_c, e_c);
    // set c_f (forward) and c_r (reverse) to one of:
    // L (local)
    // M (global)
    // I (global but with insertion end)
    // D (global but with deletion end)
    char c_f = 'L';
    char c_r = 'L';

    // preallocate containers
    int32_t p = std::max(x, o + e);
    BiFront FrM_f('M', p + 1, 'F');
    BiFront FrI_f('I', p + 1, 'F');
    BiFront FrD_f('D', p + 1, 'F');
    BiFront FrM_r('M', p + 1, 'R');
    BiFront FrI_r('I', p + 1, 'R');
    BiFront FrD_r('D', p + 1, 'R');
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

    std::string t = "CGGTGCTCGGAGGGCTAGGAGGGTACGTTCGTGTACGTTAAAAGGTTATCTGTGTCGACACCGAGATGAGGAACTTGACCGGGCCATTC";
    std::string q = "TACGTGGTGCTCGGAAGGCTAGGAGAGTACGTTCGTGTACGTCTAAAAGGTTATCTGTGTCGACACCGAGATTAGAAACTTGCCGGGCCATAAATATTTC";

    // resize aln to hold the full alignment
    aln.resize(t.size() + q.size());

    // get the optimal alignment
    char *aln_t = lbiwfa(q, t, x, o, e, j, c_f, c_r, FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln.data());

    // evaluate the score of the optimal alignment
    aln.resize(aln_t - aln.data());
    int32_t s_t = eval_score(aln, q, t, x, o, e, j);

    std::cout << "score:\n"
              << s_t << "\nalign:\n"
              << aln << "\n";
}
