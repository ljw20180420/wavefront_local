#include "lwfa.hpp"
#include "score.hpp"
#include "system.hpp"
#include <algorithm>
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
    FrontXM FrXM;
    Front FrI, FrD;

    std::string t = "CGGTGCTCGGAGGGCTAGGAGGGTACGTTCGTGTACGTTAAAAGGTTATCTGTGTCGACACCGAGATGAGGAACTTGACCGGGCCATTC";
    std::string q = "TACGTGGTGCTCGGAAGGCTAGGAGAGTACGTTCGTGTACGTCTAAAAGGTTATCTGTGTCGACACCGAGATTAGAAACTTGCCGGGCCATAAATATTTC";

    // get the optimal score
    auto [s_t, d_t, f_t] = lwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(q, t, c_f, c_r, FrXM, FrI, FrD);

    // track the optimal alignment
    std::string aln = track<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size());
    std::reverse(aln.begin(), aln.end());

    std::cout << "score:\n"
              << s_t << "\nalign:\n"
              << aln << "\n";
}
