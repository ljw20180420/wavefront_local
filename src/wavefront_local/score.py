def bowtie_score():
    return 2, 6, 5, 3


def bwa_score():
    return 1, 4, 6, 1


def transform_score(l_c: int, x_c: int, o_c: int, e_c: int) -> tuple[int]:
    x = 2 * l_c + 2 * x_c
    o = 2 * o_c
    e = 2 * e_c + l_c
    j = l_c
    return (x, o, e, j)


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
