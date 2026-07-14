from typing import Literal

from .lbiwfa import lbiwfa
from .lwfa import lwfa, track
from .score import eval_score


def pylwfa(
    q: str,
    t: str,
    x: int,
    o: int,
    e: int,
    j: int,
    c_f: Literal["L", "M", "I", "D"],
    c_r: Literal["L", "M", "I", "D"],
) -> tuple[int, str]:

    t = memoryview(t.encode())
    q = memoryview(q.encode())

    s_t, d_t, Fr = lwfa(q, t, x, o, e, j, c_f, c_r)
    aln = track(c_r, s_t, d_t, Fr)

    return s_t, aln


def pylbiwfa(
    q: str,
    t: str,
    x: int,
    o: int,
    e: int,
    j: int,
    c_f: Literal["L", "M", "I", "D"],
    c_r: Literal["L", "M", "I", "D"],
) -> tuple[int, str]:
    t = memoryview(t.encode())
    q = memoryview(q.encode())

    aln = lbiwfa(q, t, x, o, e, j, c_f, c_r)
    s_t = eval_score(aln, q, t, x, o, e, j)

    return s_t, aln
