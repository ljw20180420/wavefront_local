#!/usr/bin/env python

from wavefront_local._core import cpplbiwfa, cpplwfa

from wavefront_local import pylbiwfa, pylwfa
from wavefront_local.score import bowtie_score, transform_score

l_c, x_c, o_c, e_c = bowtie_score()  # classical scoring system
x, o, e, j = transform_score(l_c, x_c, o_c, e_c)  # transform to wfa scoring system
c_f = "L"  # for local scope at start
c_r = "L"  # for local scope at end

t = "CGGTGCTCGGAGGGCTAGGAGGGTACGTTCGTGTACGTTAAAAGGTTATCTGTGTCGACACCGAGATGAGGAACTTGACCGGGCCATTC"
q = "TACGTGGTGCTCGGAAGGCTAGGAGAGTACGTTCGTGTACGTCTAAAAGGTTATCTGTGTCGACACCGAGATTAGAAACTTGCCGGGCCATAAATATTTC"


s_t, aln = cpplwfa(q, t, "bowtie", c_f, c_r)
print(f"score:\n{s_t}\nalign\n{aln}")
s_t, aln = cpplbiwfa(q, t, "bwa", c_f, c_r)
print(f"score:\n{s_t}\nalign\n{aln}")
s_t, aln = pylwfa(q, t, x, o, e, j, c_f, c_r)
print(f"score:\n{s_t}\nalign\n{aln}")
s_t, aln = pylbiwfa(q, t, x, o, e, j, c_f, c_r)
print(f"score:\n{s_t}\nalign\n{aln}")
