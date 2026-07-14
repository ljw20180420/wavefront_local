#include "src/lwfa.hpp"
#include "src/score.hpp"
#include "src/system.hpp"
#include <algorithm>
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
    FrontXM FrXM;
    Front FrI, FrD;

    std::string t = "CGGTGCTCGGAGGGCTAGGAGGGTACGTTCGTGTACGTTAAAAGGTTATCTGTGTCGACACCGAGATGAGGAACTTGACCGGGCCATTC";
    std::string q = "TACGTGGTGCTCGGAAGGCTAGGAGAGTACGTTCGTGTACGTCTAAAAGGTTATCTGTGTCGACACCGAGATTAGAAACTTGCCGGGCCATAAATATTTC";

    // get the optimal score
    auto [s_t, d_t, f_t] = lwfa(q, t, x, o, e, j, c_f, c_r, FrXM, FrI, FrD);

    // track the optimal alignment
    std::string aln = track(c_f, c_r, s_t, d_t, f_t, FrXM, FrI, FrD, t.size(), q.size(), x, o, e, j);
    std::reverse(aln.begin(), aln.end());

    std::cout << "score:\n"
              << s_t << "\nalign:\n"
              << aln << "\n";
}
