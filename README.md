# Introduction

`wavefront_local` is a library implement the wavefront alignment in the local alignment scope. Current implementation like [`WFA2-lib`](https://github.com/smarco/WFA2-lib) only support semi-global alignment. `wavefront_local` fill the gap to the local alignment.

# Usage

## Local wavefront alignment (lWFA)

See this [example](examples/example_lwfa.cpp). To compile and run,
```shell
$ examples/lwfa.sh
```

## Local bidirectional wavefront alignment (lBiWFA)

See this [example](examples/example_lbiwfa.cpp). To compile and run,
```shell
$ examples/lbiwfa.sh
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
