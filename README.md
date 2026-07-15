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
where $f = s / j$ if $0 = s\pmod j$, otherwise $f = -\infty$. Compared with the global WFA, lWFA computes $`\mathcal{X}_{s,d}`$ by including the possibility that $`\mathcal{X}_{s,d}`$ corresponds to the start cell of the aligned region. This is achieved by the term $f$. If $0 = s\pmod j$, then the unaligned region of size $f = s / j$ has the score exactly equal to $s$. Otherwise, just set $f = -\infty$.  

It can be prove that this recurrence format keeps the $O(sN)$ time complexity and the $O(s^2)$ space complexity. Furthermore, the bidirectional technique also works for the WFA in the local scope, which improve the space complexity from $O(s^2)$ to $O(s)$ while keep the $O(sN)$ time complexity.

# pybind11 python langauage binding

Install the pypi package.
```shell
$ pip install wavefront_local
```

There are four functions.

- `wavefront_local._core.cpplwfa`
- `wavefront_local._core.cpplbiwfa`
- `wavefront_local.pylwfa`
- `wavefront_local.pylbiwfa`

All functions have the same signature, see the [example](examples/example.py). To run the example,
```shell
$ examples/example.py
```
- `cpplwfa` implements the wavefront alignment in the local scope.
- `cpplbiwfa` implements the bidirectional wavefront alignment in the local scope. It use the least time and space (even faster than `cpplwfa`).
- `pylwfa` and `pylbiwfa` are python prototypes. These have higher readability than the c++ library. One may learn how lWFA and lBiWFA work by reading these python prototypes.

# c++ library

To use the c++ library, you need to clone the repository or download a release tarball.
```shell
$ git clone https://github.com/ljw20180420/wavefront_local.git
$ cd wavefront_local
```

## Local wavefront alignment (lWFA)

See this [example](examples/example_lwfa.cpp). To compile and run the example,
```shell
$ examples/lwfa.sh
```

## Local bidirectional wavefront alignment (lBiWFA)

See this [example](examples/example_lbiwfa.cpp). To compile and run the example,
```shell
$ examples/lbiwfa.sh
```

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
- [ ] manual simd
- [ ] webassembly
- [ ] python binding
```
