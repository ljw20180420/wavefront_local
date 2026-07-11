# Introduction

`wavefront_local` is a library implement the wavefront alignment in the local alignment scope. Current implementation like [`WFA2-lib`](https://github.com/smarco/WFA2-lib) only support semi-global alignment. `wavefront_local` fill the gap to the local alignment.

# Scoring system

The difficulty of applying WFA to the local alignment is that the WFA scoring system does not award matches. Thus, the best possible score is zero. However, in the local alignment scope, this can be easily achieved by aligning nothing at all. Eizenga and Paten show that the WFA scoring system with mismatch penalty $x$, gap opening penalty $o$, and gap extension penalty $e$, can be related to the classical scoring system with match score $l_c$, mismatch penalty $x_c$, gap opening penalty $o_c$, and gap extension penalty $e_c$ by
```math
\begin{aligned}
&x = 2l_c + 2x_c \\
&o = 2o_c \\
&e = 2e_c + l_c
\end{aligned}
```
By the identiy
```math
2L + 2X + E = M + N
```
with $L$ the total matches, $X$ the total mismatches, $E$ the total gap size, $M$ and $N$ the size of text and qury, respectively, Eizenga and Paten prove that the classical score $s_c$ and the WFA score $s_w$ are related by
```math
2s_c + s_w = l_c(M + N)
```
We extend their observation by penalize the unaligned region in the local scope. In detail, let $J$ be the total size of unaligned region. Then there is the identity
```math
2L + 2X + E + J = M + N
```
for the local scope. We penalty each step in the unaligned region by $j$. Observe that if set the local-scope WFA scoring system by 
```math
\begin{aligned}
&x = 2l_c + 2x_c \\
&o = 2o_c \\
&e = 2e_c + l_c \\
&j = l_c
\end{aligned}
```
then we recover the relation between the global-scope WFA scoring system and the classical one as
```math
2s_c + s_w = l_c(M + N)
```

# Recurrence of wavefront components in the local alignemnt scope

The recurrence of wavefront components for the wavefront alignment in the local scope (lWFA) is
```math
\begin{aligned}
&\mathcal{I}_{s,d} = \max(\mathcal{M}_{s-o-e,d-1} + 1, \mathcal{I}_{s-e,d-1} + 1) \\
&\mathcal{D}_{s,d} = \max(\mathcal{M}_{s-o-e,d+1} + 1, \mathcal{D}_{s-e,d+1} +1) \\
&\mathcal{X} = \max(f, \mathcal{M}_{s-x,d} + 2, \mathcal{I}_{s,d}, \mathcal{D}_{s,d}) \\
&\mathcal{M}_{s,d} = \mathcal{X}_{s,d} + 2\mu(m,n)
\end{aligned}
```
where $f = s / j$ if $0 = s\pmod j$, otherwise $f = -\infty$. Compared with the global WFA, lWFA computes $`\mathcal{X}_{s,d}`$ by including the possibility that $\mathcal{X}_{s,d}$ corresponds to the start cell of the aligned region. This is achieved by the term $f$. If $0 = s\pmod j$, then the unaligned region of size $f = s / j$ has the score exactly equal to $s$. Otherwise, just set $f = -\infty$.  

It can be prove that this recurrence format keeps the $O(sN)$ time complexity and the $O(s^2)$ space complexity. Furthermore, the bidirectional technique also works for the WFA in the local scope, which improve the space complexity from $O(s^2)$ to $O(s)$ while keep the $O(sN)$ time complexity.

# Usage

## Local wavefront alignment (lWFA)

```cpp
#include "wavefront_local/cpp/src/lwfa.hpp"
#include "wavefront_local/cpp/src/score.hpp"
#include "wavefront_local/cpp/src/system.hpp"

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
}

```

## Local bidirectional wavefront alignment (lBiWFA)

```cpp
#include "wavefront_local/cpp/src/lbiwfa.hpp"
#include "wavefront_local/cpp/src/score.hpp"
#include "wavefront_local/cpp/src/system.hpp"

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
}
```

# Python prototypes

Python prototypes of the local [`WFA`](src/wavefront_local/wfa.py) and [`BiWFA`](src/wavefront_local/wfa.py) are available. These python prototypes have higher readability than the c++ library. One may learn how lWFA and lBiWFA work by reading these python prototypes.

# Citation

In preparation.

# TODO

```
- [ ] Skip useless jumping at diagonals which have been passed by previous low-score waves. This prevent redundant extension, thereby the time complexity no longer depends on x.
  - Prove the correctness of skipping useless jumping as follows.
    - The optimal score keep.
    - The iteration get Fr (wavefront).
    - The optimal score is achieved at Fr.
    - The iteration skipping useless jumping achieves optimal score.
- [ ] Implement the technique in the paper: Improving the time and space complexity of the WFA algorithm and generalizing its scoring.
- [ ] find sota library for efficient implementaion
  - [ ] bit vector for recording alignment
  - [ ] efficient sparse matrix
- [ ] auto simd
- [ ] manual simd
- [ ] webassembly
- [ ] python binding
```
