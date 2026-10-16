#!/usr/bin/env python

import os
import pathlib
from dataclasses import dataclass, field

import pandas as pd
import tqdm
from Bio.Align import PairwiseAligner
from wavefront_local._core import cpplbiwfa, cpplwfa

from wavefront_local import pylbiwfa, pylwfa
from wavefront_local.config import SimDataConfig
from wavefront_local.ini import read_ini
from wavefront_local.score import (
    bowtie_score,
    bwa_score,
    eval_score,
    transform_score,
)
from wavefront_local.test import generate_random_reads


@dataclass(frozen=True)
class TestConfig:
    sim_data_config: SimDataConfig

    def __post_init__(self):
        object.__setattr__(
            self, "data_dir", pathlib.Path("data") / self.sim_data_config.align_mode
        )
        object.__setattr__(
            self, "align_dir", pathlib.Path("data") / self.sim_data_config.align_mode
        )


def get_test_configs():
    test_configs = []
    for align_mode in ["local", "global"]:
        for length in [100, 200]:
            for err in [0.01, 0.05, 0.1]:
                TestConfig(
                    sim_data_config=SimDataConfig(
                        align_mode=align_mode,
                        size=100,
                        length=length,
                        err=err,
                        end_up=10,
                    )
                )


def generate_test_data(cfg: dict) -> None:
    for mode in cfg["mode"]:
        (data_dir / mode).mkdir(parents=True, exist_ok=True)
        for length in cfg["length"]:
            for err in cfg["err"]:
                for end_up in cfg["end_up"]:
                    ts, qs = generate_random_reads(
                        size=size,
                        length=length,
                        err=err,
                        end_up=end_up,
                        mode=mode,
                        seed=seed,
                    )
                    pd.DataFrame({
                        "text": ts,
                        "query": qs,
                    }).to_csv(
                        data_dir / mode / f"{length}_{err}_{end_up}.csv",
                        index=False,
                    )


def run_test(data_dir: os.PathLike, align_dir: os.PathLike, cfg: dict) -> None:
    data_dir = pathlib.Path(os.fspath(data_dir))
    align_dir = pathlib.Path(os.fspath(align_dir))
    for align_func in [cpplbiwfa, cpplwfa, pylbiwfa, pylwfa]:
        for score_schema in cfg["score_schema"]:
            if score_schema == "bowtie":
                l_c, x_c, o_c, e_c = bowtie_score()
            elif score_schema == "bwa":
                l_c, x_c, o_c, e_c = bwa_score()
            else:
                raise ValueError("score schema must be bowtie or bwa")
            aligners = {
                mode: PairwiseAligner(
                    mode=mode,
                    match_score=l_c,
                    mismatch_score=-x_c,
                    open_gap_score=-o_c - e_c,
                    extend_gap_score=-e_c,
                )
                for mode in ["local", "global"]
            }
            x, o, e, j = transform_score(l_c, x_c, o_c, e_c)
            for mode in cfg["mode"]:
                (align_dir / align_func.__name__ / score_schema / mode).mkdir(
                    parents=True, exist_ok=True
                )
                if mode == "local":
                    c_f = "L"
                    c_r = "L"
                else:
                    assert mode == "global", f"unknown mode {mode}"
                    c_f = "M"
                    c_r = "M"
                for length in cfg["length"]:
                    for err in cfg["err"]:
                        print(score_schema, mode, length, err)
                        df = pd.read_csv(
                            data_dir / mode / f"{length}_{err}_{end_up}.csv",
                            header=0,
                        )
                        s_ts = []
                        alns = []
                        scores = []
                        for t, q in tqdm.tqdm(zip(df["text"], df["query"])):
                            s_t, aln = align_func(q, t, x, o, e, j, c_f, c_r)
                            s_ts.append(s_t)
                            alns.append(aln)
                            assert s_t == eval_score(aln, q, t, x, o, e, j), (
                                "inconsistent penalty"
                            )

                            # check score
                            score = (l_c * (len(t) + len(q)) - s_t) // 2
                            scores.append(score)
                            assert score == aligners[mode].score(t, q), (
                                "inconsistent score"
                            )

                        df = pd.DataFrame({
                            "s_t": s_ts,
                            "score": scores,
                            "aln": alns,
                        }).to_csv(
                            align_dir
                            / align_func.__name__
                            / score_schema
                            / mode
                            / f"{length}_{err}_{end_up}.csv",
                            index=False,
                        )


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent)
    cfg = read_ini("config.ini")
    generate_test_data(data_dir="data", size=cfg["size"][0], seed=cfg["seed"][0])
    run_test(data_dir="data", align_dir="align", end_up=cfg["end_up"][0])
