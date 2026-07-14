import sys
from collections import defaultdict
from enum import Enum, auto
from typing import Literal

from .utils import extend
from .wfa import track, wfa


def next(
    q: memoryview,
    t: memoryview,
    Fr: defaultdict,
    s: int,
    x: int,
    o: int,
    e: int,
    j: int,
    c: Literal["M", "I", "D", "L"],
) -> defaultdict:
    for d in Fr["M"][s - o - e].keys():
        f = Fr["M"][s - o - e][d]
        n = (f + d) // 2
        m = (f - d) // 2
        if n < len(q):
            Fr["I"][s][d + 1] = max(f + 1, Fr["I"][s][d + 1])
        if m < len(t):
            Fr["D"][s][d - 1] = max(f + 1, Fr["D"][s][d - 1])

    for d in Fr["I"][s - e].keys():
        f = Fr["I"][s - e][d]
        n = (f + d) // 2
        if n < len(q):
            Fr["I"][s][d + 1] = max(f + 1, Fr["I"][s][d + 1])

    for d in Fr["D"][s - e].keys():
        f = Fr["D"][s - e][d]
        m = (f - d) // 2
        if m < len(t):
            Fr["D"][s][d - 1] = max(f + 1, Fr["D"][s][d - 1])

    for d in Fr["M"][s - x].keys():
        f = Fr["M"][s - x][d]
        n = (f + d) // 2
        m = (f - d) // 2
        if n < len(q) and m < len(t):
            Fr["M"][s][d] = max(f + 2, Fr["M"][s][d])

    for d in Fr["I"][s].keys():
        Fr["M"][s][d] = max(Fr["I"][s][d], Fr["M"][s][d])

    for d in Fr["D"][s].keys():
        Fr["M"][s][d] = max(Fr["D"][s][d], Fr["M"][s][d])

    if c == "L" and s % j == 0:
        f = s // j
        for d in range(max(-f, f - 2 * len(t)), min(f, 2 * len(q) - f) + 1, 2):
            Fr["M"][s][d] = max(f, Fr["M"][s][d])
    for d in Fr["M"][s].keys():
        mu = extend(q, t, Fr["M"][s][d], d)
        Fr["M"][s][d] += 2 * mu

    p = max(x, o + e)
    for c in Fr.keys():
        if s - p - 1 in Fr[c]:
            del Fr[c][s - p - 1]

    return Fr


def try_jump(
    Fr: defaultdict, s: int, j: int, M: int, N: int
) -> tuple[int, int, int, int]:
    if not Fr["M"][s]:
        return sys.maxsize, None, None, None

    d_b = max(Fr["M"][s], key=Fr["M"][s].get)
    f_b = Fr["M"][s][d_b]
    c_b = "M"
    s_b = s + (M + N - f_b) * j

    return s_b, d_b, f_b, c_b


def overlap(
    Fr: defaultdict,
    Fr2: defaultdict,
    s: int,
    s2range: range,
    o: int,
    M: int,
    N: int,
) -> tuple[int, int, int, int, int, int]:
    s_b = sys.maxsize
    d_b = None
    f_b = None
    c_b = None
    score_upper = None
    score_lower = None
    for c in ["M", "I", "D"]:
        for s2 in s2range:
            for d in Fr[c][s].keys():
                d2 = N - M - d
                if d2 in Fr2[c][s2] and Fr[c][s][d] + Fr2[c][s2][d2] >= N + M:
                    new_score = s + s2
                    if c != "M":
                        new_score -= o

                    if new_score < s_b:
                        s_b = new_score
                        d_b = d
                        f_b = Fr[c][s][d]
                        c_b = c

    score_upper = s - o if c_b != "M" else s
    score_lower = s_b - s

    return s_b, d_b, f_b, c_b, score_upper, score_lower


class Split(Enum):
    FORWARD_JUMP = auto()
    REVERSE_JUMP = auto()
    PATH = auto()


def biwfa(
    q: memoryview,
    t: memoryview,
    x: int,
    o: int,
    e: int,
    j: int,
    c_f: Literal["M", "I", "D", "L"],
    c_r: Literal["M", "I", "D", "L"],
) -> str:
    Fr_f = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: -1)))
    if c_f != "L":
        Fr_f[c_f][0][0] = 0
    Fr_r = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: -1)))
    if c_r != "L":
        Fr_r[c_r][0][0] = 0

    p = max(x, o + e)
    s = 0
    s_t = sys.maxsize
    anti_diag_upper = -1
    anti_diag_lower = -1
    while 2 * s - o - p < s_t:
        Fr_f = next(q, t, Fr_f, s, x, o, e, j, c_f)
        if c_r == "L":
            s_b, d_b, f_b, c_b = try_jump(Fr_f, s, j, len(t), len(q))
            if s_b < s_t:
                s_t = s_b
                d_t = d_b
                f_t = f_b
                c_t = c_b
                score_upper_t = s
                split = Split.FORWARD_JUMP

        Fr_r = next(q[::-1], t[::-1], Fr_r, s, x, o, e, j, c_r)
        if c_f == "L":
            s_b, d_b, f_b, c_b = try_jump(Fr_r, s, j, len(t), len(q))
            if s_b < s_t:
                d_b = len(q) - len(t) - d_b
                f_b = len(q) + len(t) - f_b
                s_t = s_b
                d_t = d_b
                f_t = f_b
                c_t = c_b
                score_lower_t = s
                split = Split.REVERSE_JUMP

        anti_diag_upper = max(max(Fr_f["M"][s].values(), default=-1), anti_diag_upper)
        anti_diag_lower = max(max(Fr_r["M"][s].values(), default=-1), anti_diag_lower)
        if anti_diag_upper + anti_diag_lower >= len(t) + len(q):
            s_b, d_b, f_b, c_b, score_upper, score_lower = overlap(
                Fr_f, Fr_r, s, range(s - p, s), o, len(t), len(q)
            )
            if s_b < s_t:
                s_t = s_b
                d_t = d_b
                f_t = f_b
                c_t = c_b
                score_upper_t = score_upper
                score_lower_t = score_lower
                split = Split.PATH

            s_b, d_b, f_b, c_b, score_lower, score_upper = overlap(
                Fr_r, Fr_f, s, range(s - p, s + 1), o, len(t), len(q)
            )
            if s_b < s_t:
                d_b = len(q) - len(t) - d_b
                f_b = len(q) + len(t) - f_b
                s_t = s_b
                d_t = d_b
                f_t = f_b
                c_t = c_b
                score_upper_t = score_upper
                score_lower_t = score_lower
                split = Split.PATH

        s += 1

    n = (f_t + d_t) // 2
    m = (f_t - d_t) // 2
    if split == Split.FORWARD_JUMP:
        if score_upper_t <= p:
            s_w, d_w, Fr = wfa(q[:n], t[:m], x, o, e, j, c_f, c_t)
            aln_upper = track("M", s_w, d_w, Fr)
        else:
            aln_upper = biwfa(q[:n], t[:m], x, o, e, j, c_f, c_t)
        aln_lower = "S" * (len(q) - n) + "N" * (len(t) - m)
    elif split == Split.REVERSE_JUMP:
        if score_lower_t <= p:
            s_w, d_w, Fr = wfa(q[n:], t[m:], x, o, e, j, c_t, c_r)
            aln_lower = track(c_r, s_w, d_w, Fr)
        else:
            aln_lower = biwfa(q[n:], t[m:], x, o, e, j, c_t, c_r)
        aln_upper = "S" * n + "N" * m
    else:
        if score_upper_t <= p:
            s_w, d_w, Fr = wfa(q[:n], t[:m], x, o, e, j, c_f, c_t)
            aln_upper = track(c_t, s_w, d_w, Fr)
        else:
            aln_upper = biwfa(q[:n], t[:m], x, o, e, j, c_f, c_t)
        if score_lower_t <= p:
            s_w, d_w, Fr = wfa(q[n:], t[m:], x, o, e, j, c_t, c_r)
            aln_lower = track(c_r, s_w, d_w, Fr)
        else:
            aln_lower = biwfa(q[n:], t[m:], x, o, e, j, c_t, c_r)

    return aln_upper + aln_lower
