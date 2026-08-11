#include "lbiwfa.hpp"
#include "score.hpp"
#include "system.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    // set c_f (forward) and c_r (reverse) to one of:
    // L (local)
    // M (global)
    // I (global but with insertion end)
    // D (global but with deletion end)
    char c_f = 'L';
    char c_r = 'L';

    // preallocate containers
    BiTable<bowtie_wfa.p> BiFr_f;
    BiTable<bowtie_wfa.p> BiFr_r;
    FrontXM FrXM;
    Front FrI, FrD;
    std::string aln;

    std::string t = "CGGTGCTCGGAGGGCTAGGAGGGTACGTTCGTGTACGTTAAAAGGTTATCTGTGTCGACACCGAGATGAGGAACTTGACCGGGCCATTC";
    std::string q = "TACGTGGTGCTCGGAAGGCTAGGAGAGTACGTTCGTGTACGTCTAAAAGGTTATCTGTGTCGACACCGAGATTAGAAACTTGCCGGGCCATAAATATTTC";

    // resize aln to hold the full alignment
    aln.resize(t.size() + q.size());

    // get the optimal alignment
    char *aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, BiFr_f, BiFr_r, FrXM, FrI, FrD, aln.data());

    // evaluate the score of the optimal alignment
    aln.resize(aln_t - aln.data());
    int32_t s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);

    std::cout << "score:\n"
              << s_t << "\nalign:\n"
              << aln << "\n";
}
