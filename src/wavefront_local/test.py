from typing import Literal

import numpy as np


def random_DNA(length: int, rng: np.random.Generator) -> str:
    return "".join(rng.choice(["A", "C", "G", "T"], size=length))


def mutate(
    t: str,
    err: float,
    end_up: int,
    mode: Literal["local", "global"],
    rng: np.random.Generator,
) -> str:
    tup = 0
    qup = 0
    tdown = 0
    qdown = 0
    if mode == "local":
        tup, qup, tdown, qdown = rng.integers(low=0, high=end_up + 1, size=4)

    t = np.array(list(t[tup : len(t) - tdown]))
    ins_sizes = rng.negative_binomial(1, 1 - err / 3, size=len(t) + 1)
    all_ins = random_DNA(sum(ins_sizes), rng)
    segs = [random_DNA(qup, rng)]
    p = np.array([err / 3, err / 9, err / 9, err / 9, err / 9, 1 - 10 * err / 9])
    p = p / p.sum()
    muts = rng.choice(["", "A", "C", "G", "T", "N"], size=len(t), p=p)
    t[muts != "N"] = muts[muts != "N"]
    ins_offset = 0
    for i, base in enumerate(t):
        if ins_sizes[i] > 0:
            segs.append(all_ins[ins_offset : ins_offset + ins_sizes[i]])
            ins_offset += ins_sizes[i]
        segs.append(base)

    segs.append(all_ins[ins_offset:])
    segs.append(random_DNA(qdown, rng))

    return "".join(segs)


def generate_random_reads(
    size: int,
    length: int,
    err: float,
    end_up: int,
    mode: Literal["local", "global"],
    seed: int,
) -> tuple[list[str], list[str]]:
    rng = np.random.default_rng(seed)
    ts = [random_DNA(length, rng) for _ in range(size)]
    qs = [mutate(t, err, end_up, mode, rng) for t in ts]

    return qs, ts
