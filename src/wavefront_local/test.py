import os
import pathlib
from typing import Literal

import numpy as np
import pandas as pd
from Bio.Align import PairwiseAligner
from tqdm import tqdm

from wavefront_local import pylbiwfa, pylwfa
from wavefront_local._core import cpplbiwfa, cpplwfa
from wavefront_local.score import (
    bowtie_score,
    bwa_score,
    classical2wfa_sys,
    eval_score,
    wfa2classical_val,
)


def _random_DNA(length: str, rng: np.random.Generator) -> str:
    return "".join(rng.choice(["A", "C", "G", "T"], size=length))


def _mutate_DNA(DNA: str, err: float, rng: np.random.Generator) -> str:
    DNA = np.array(list(DNA))
    ins_sizes = rng.negative_binomial(1, 1 - err / 3, size=len(DNA) + 1)
    all_ins = _random_DNA(sum(ins_sizes), rng)
    p = np.array([
        err / 3,
        err / 9,
        err / 9,
        err / 9,
        err / 9,
        1 - 10 * err / 9,
    ])
    p = p / p.sum()
    muts = rng.choice(["", "A", "C", "G", "T", "N"], size=len(DNA), p=p)
    DNA[muts != "N"] = muts[muts != "N"]
    segs = []
    ins_offset = 0
    for i, base in enumerate(DNA):
        if ins_sizes[i] > 0:
            segs.append(all_ins[ins_offset : ins_offset + ins_sizes[i]])
            ins_offset += ins_sizes[i]
        segs.append(base)

    segs.append(all_ins[ins_offset:])

    return "".join(segs)


def _local_mutate_DNA(
    t: str,
    err: float,
    end_up: int,
    align_mode: str,
    rng: np.random.Generator,
) -> str:
    tup = 0
    qup = 0
    tdown = 0
    qdown = 0
    if align_mode == "local":
        tup, qup, tdown, qdown = rng.integers(low=0, high=end_up + 1, size=4)

    return "".join([
        _random_DNA(qup, rng),
        _mutate_DNA(t[tup : len(t) - tdown], err, rng),
        _random_DNA(qdown, rng),
    ])


def generate_sim_data(
    size: int,
    length: int,
    err: float,
    end_up: int,
    align_mode: Literal["local", "global"],
    seed: int,
    data_file: os.PathLike,
) -> None:
    rng = np.random.default_rng(seed=seed)
    ts = [_random_DNA(length, rng) for _ in range(size)]
    qs = [_local_mutate_DNA(t, err, end_up, align_mode, rng) for t in ts]

    data_file.parent.mkdir(exist_ok=True, parents=True)
    pd.DataFrame({
        "text": ts,
        "query": qs,
    }).to_csv(
        data_file,
        index=False,
    )


def align(
    align_mode: Literal["local", "global"],
    data_file: os.PathLike,
    score_schema: Literal["bowtie", "bwa"],
    aligner: Literal["cpplbiwfa", "cpplwfa", "pylbiwfa", "pylwfa"],
    align_file: os.PathLike,
):
    df = pd.read_csv(data_file, header=0)
    if align_mode == "local":
        c_f = "L"
        c_r = "L"
    else:
        # align_mode == "global"
        c_f = "M"
        c_r = "M"

    aligner = eval(aligner)
    l_c, x_c, o_c, e_c = eval(f"{score_schema}_score()")
    x, o, e, j = classical2wfa_sys(l_c, x_c, o_c, e_c)

    s_ts = []
    alns = []
    scores = []
    for t, q in tqdm(zip(df["text"], df["query"])):
        if aligner.__name__.startswith("cpp"):
            s_t, aln = aligner(
                q=q,
                t=t,
                score_schema=score_schema,
                c_f=c_f,
                c_r=c_r,
            )
        else:
            # aligner.__name__.startswith("py"):
            s_t, aln = aligner(
                q=q,
                t=t,
                x=x,
                o=o,
                e=e,
                j=j,
                c_f=c_f,
                c_r=c_r,
            )

        s_ts.append(s_t)
        alns.append(aln)
        assert s_t == eval_score(aln=aln, q=q, t=t, x=x, o=o, e=e, j=j), (
            "inconsistent penalty"
        )

        # check score
        score = wfa2classical_val(s_t=s_t, l_c=l_c, len_t=len(t), len_q=len(q))
        scores.append(score)

        yield s_t, score, aln, t, q

    align_file.parent.mkdir(exist_ok=True, parents=True)
    pd.DataFrame({
        "s_t": s_ts,
        "score": scores,
        "aln": alns,
    }).to_csv(align_file, index=False)


def test(
    align_mode: Literal["local", "global"],
    data_file: os.PathLike,
    score_schema: Literal["bowtie", "bwa"],
    aligner: Literal["cpplbiwfa", "cpplwfa", "pylbiwfa", "pylwfa"],
    align_file: os.PathLike,
) -> None:
    l_c, x_c, o_c, e_c = eval(f"{score_schema}_score()")
    std_aligner = PairwiseAligner(
        mode=align_mode,
        match_score=l_c,
        mismatch_score=-x_c,
        open_gap_score=-o_c - e_c,
        extend_gap_score=-e_c,
    )

    for _, score, _, t, q in align(
        align_mode,
        data_file,
        score_schema,
        aligner,
        align_file,
    ):
        assert score == std_aligner.score(t, q), "inconsistent score"
