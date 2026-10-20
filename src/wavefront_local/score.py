def classical2wfa_sys(
    l_c: int,
    x_c: int,
    o_c: int,
    e_c: int,
) -> tuple[int, int, int, int]:
    x = 2 * l_c + 2 * x_c
    o = 2 * o_c
    e = 2 * e_c + l_c
    j = l_c

    return x, o, e, j


def wfa2classical_val(s_t: int, l_c: int, len_t: int, len_q: int) -> int:
    assert (l_c * (len_t + len_q) - s_t) % 2 == 0, "classical score is not integer"

    return (l_c * (len_t + len_q) - s_t) // 2


def classical2wfa_val(score: int, l_c: int, len_t: int, len_q: int) -> int:
    return l_c * (len_t + len_q) - 2 * score


def wfa2classical_sys(
    x: int,
    o: int,
    e: int,
    j: int,
) -> tuple[int, int, int, int]:
    l_c = j
    assert (x - 2 * j) % 2 == 0, "x_c is not integer"
    x_c = (x - 2 * j) // 2
    assert o % 2 == 0, "o_c is not integer"
    o_c = o // 2
    assert (e - j) % 2 == 0, "e_c is not integer"
    e_c = (e - j) // 2

    return l_c, x_c, o_c, e_c


def bowtie_score() -> tuple[int, int, int, int]:
    l_c = 2
    x_c = 6
    o_c = 5
    e_c = 3

    return l_c, x_c, o_c, e_c


def bwa_score() -> tuple[int, int, int, int]:
    l_c = 1
    x_c = 4
    o_c = 6
    e_c = 1

    return l_c, x_c, o_c, e_c


def eval_score(
    aln: str, q: memoryview, t: memoryview, x: int, o: int, e: int, j: int
) -> int:
    n = 0
    m = 0
    s = 0
    for i, a in enumerate(aln):
        if a == "N":
            s += j
            m += 1
        elif a == "S":
            s += j
            n += 1
        elif a == "M":
            if q[n] != t[m]:
                s += x
            n += 1
            m += 1
        elif a == "I":
            if i > 0 and aln[i - 1] == "I":
                s += e
            else:
                s += o + e
            n += 1
        else:
            assert a == "D", f"unknown align type {a}"
            if i > 0 and aln[i - 1] == "D":
                s += e
            else:
                s += o + e
            m += 1

    return s
