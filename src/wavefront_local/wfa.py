import sys
from collections import defaultdict
from typing import Literal

import numpy as np

from . import extend


class Cell:
    def __init__(
        self,
        f: int,
        source_c: Literal["S", "M", "X", "I", "D"] | None,
        source_s: int | None,
        source_d: int | None,
    ) -> None:
        self.f = f
        self.source_c = source_c
        self.source_s = source_s
        self.source_d = source_d

    def update(
        self,
        f: int,
        source_c: Literal["S", "M", "X", "I", "D"],
        source_s,
        source_d,
    ) -> None:
        if f > self.f:
            self.f = f
            self.source_c = source_c
            self.source_s = source_s
            self.source_d = source_d


def wfa(
    q: memoryview,
    t: memoryview,
    x: int,
    o: int,
    e: int,
    j: int,
    c_f: Literal["L", "M", "I", "D"],
    c_r: Literal["L", "M", "I", "D"],
) -> tuple:
    if c_f == "M":
        c_f = "X"

    s = 0
    s_t = sys.maxsize
    Fr = defaultdict(
        lambda: defaultdict(lambda: defaultdict(lambda: Cell(-1, None, None, None)))
    )
    Fr[c_f][0][0] = Cell(0, None, None, None)
    while s_t > s:
        for d in Fr["M"][s - o - e].keys():
            f = Fr["M"][s - o - e][d].f
            n = (f + d) // 2
            m = (f - d) // 2
            if n < len(q):
                Fr["I"][s][d + 1].update(f + 1, "M", s - o - e, d)
            if m < len(t):
                Fr["D"][s][d - 1].update(f + 1, "M", s - o - e, d)

        for d in Fr["I"][s - e].keys():
            f = Fr["I"][s - e][d].f
            n = (f + d) // 2
            if n < len(q):
                Fr["I"][s][d + 1].update(f + 1, "I", s - e, d)

        for d in Fr["D"][s - e].keys():
            f = Fr["D"][s - e][d].f
            m = (f - d) // 2
            if m < len(t):
                Fr["D"][s][d - 1].update(f + 1, "D", s - e, d)

        for d in Fr["M"][s - x].keys():
            f = Fr["M"][s - x][d].f
            n = (f + d) // 2
            m = (f - d) // 2
            if n < len(q) and m < len(t):
                Fr["X"][s][d].update(f + 2, "M", s - x, d)

        for d in Fr["I"][s].keys():
            Fr["X"][s][d].update(Fr["I"][s][d].f, "I", s, d)

        for d in Fr["D"][s].keys():
            Fr["X"][s][d].update(Fr["D"][s][d].f, "D", s, d)

        if c_f == "L" and s % j == 0:
            f = s // j
            for d in range(
                max(-f, f - 2 * len(t)),
                min(f, 2 * len(q) - f) + 1,
                2,
            ):
                Fr["X"][s][d].update(f, "L", 0, 0)

        for d in Fr["X"][s].keys():
            mu = extend(q, t, Fr["X"][s][d].f, d)
            Fr["M"][s][d].update(Fr["X"][s][d].f + 2 * mu, "X", s, d)

        if c_r == "L":
            for d in Fr["M"][s].keys():
                new_score = s + (len(q) + len(t) - Fr["M"][s][d].f) * j
                if new_score < s_t:
                    s_t = new_score
                    L = Cell(len(q) + len(t), "M", s, d)
        else:
            d = len(q) - len(t)
            if d in Fr[c_r][s] and Fr[c_r][s][d].f == len(q) + len(t):
                return s, d, Fr

        s += 1

    Fr["L"][s_t][len(q) - len(t)] = L
    return s_t, len(q) - len(t), Fr


def track(c_r: Literal["M", "I", "D", "L"], s_t: int, d_t: int, Fr: defaultdict) -> str:
    aln = []
    c_c = c_r
    s_c = s_t
    d_c = d_t
    current = Fr[c_c][s_c][d_c]
    while current.source_c is not None:
        c_p = current.source_c
        s_p = current.source_s
        d_p = current.source_d
        source = Fr[c_p][s_p][d_p]

        n_c = (current.f + d_c) // 2
        m_c = (current.f - d_c) // 2

        n_p = (source.f + d_p) // 2
        m_p = (source.f - d_p) // 2

        if c_c == "L" or c_p == "L":
            aln.insert(0, "S" * (n_c - n_p) + "N" * (m_c - m_p))
        elif n_c == n_p:
            aln.insert(0, "D" * (m_c - m_p))
        elif m_c == m_p:
            aln.insert(0, "I" * (n_c - n_p))
        else:
            assert n_c - n_p == m_c - m_p, "inconsistent size in query and text"
            aln.insert(0, "M" * (n_c - n_p))

        c_c = c_p
        s_c = s_p
        d_c = d_p
        current = source

    return "".join(aln)


def countM(Fr: defaultdict, M: int, N: int) -> tuple[int, int]:
    M_count = 0
    dp_table = np.zeros((M + 1, N + 1), dtype=np.int8)
    for s in Fr["M"].keys():
        for d in Fr["M"][s].keys():
            nX = (Fr["X"][s][d].f + d) // 2
            mX = (Fr["X"][s][d].f - d) // 2
            nM = (Fr["M"][s][d].f + d) // 2
            mM = (Fr["M"][s][d].f - d) // 2
            M_count += mM - mX + 1
            dp_table[np.arange(mX, mM + 1), np.arange(nX, nM + 1)] = 1

    return M_count, dp_table.sum().item()
