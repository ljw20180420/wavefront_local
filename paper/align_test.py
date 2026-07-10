#!/usr/bin/env python

import os
import pathlib
import time
import tracemalloc

import pandas as pd
import tqdm

from wavefront_local import (
    bowtie_score,
    bwa_score,
    eval_score,
    get_aligners,
    transform_score,
)
from wavefront_local.biwfa import biwfa
from wavefront_local.ini import read_ini
from wavefront_local.wfa import countM, track, wfa

os.chdir(pathlib.Path(__file__).resolve().parent)

cfg = read_ini("test.ini")

test_data_dir = pathlib.Path("test/data")
test_align_dir = pathlib.Path("test/align")
end_up = cfg["end_up"][0]
for score_schema in cfg["score_schema"]:
    if score_schema == "bowtie":
        l_c, x_c, o_c, e_c = bowtie_score()
    elif score_schema == "bwa":
        l_c, x_c, o_c, e_c = bwa_score()
    else:
        raise ValueError("score schema must be bowtie or bwa")
    aligners = get_aligners(l_c, x_c, o_c, e_c)
    x, o, e, j = transform_score(l_c, x_c, o_c, e_c)
    for mode in cfg["mode"]:
        (test_align_dir / score_schema / mode).mkdir(parents=True, exist_ok=True)
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
                    test_data_dir / mode / f"{length}_{err}_{end_up}.csv",
                    header=0,
                )
                s_ts = []
                scores = []
                times = []
                bi_times = []
                c_memories = []
                bi_c_memories = []
                p_memories = []
                bi_p_memories = []
                M_counts = []
                M_counts_nodup = []
                alns = []
                bi_alns = []
                for t_str, q_str in tqdm.tqdm(zip(df["text"], df["query"])):
                    t = memoryview(t_str.encode())
                    q = memoryview(q_str.encode())

                    # wfa
                    start_time = time.perf_counter()
                    tracemalloc.start()
                    s_t, d_t, Fr = wfa(q, t, x, o, e, j, c_f, c_r)
                    end_time = time.perf_counter()
                    c_memory, p_memory = tracemalloc.get_traced_memory()
                    tracemalloc.stop()
                    times.append(end_time - start_time)
                    c_memories.append(c_memory)
                    p_memories.append(p_memory)
                    M_count, M_count_nodup = countM(Fr, len(t), len(q))
                    M_counts.append(M_count)
                    M_counts_nodup.append(M_count_nodup)
                    s_ts.append(s_t)
                    aln = track(c_r, s_t, d_t, Fr)
                    alns.append(aln)
                    assert s_t == eval_score(aln, q, t, x, o, e, j), (
                        "inconsistent penalty"
                    )

                    # biwfa
                    bi_start_time = time.perf_counter()
                    tracemalloc.start()
                    bi_aln = biwfa(q, t, x, o, e, j, c_f, c_r)
                    bi_end_time = time.perf_counter()
                    bi_c_memory, bi_p_memory = tracemalloc.get_traced_memory()
                    tracemalloc.stop()
                    bi_times.append(bi_end_time - bi_start_time)
                    bi_c_memories.append(bi_c_memory)
                    bi_p_memories.append(bi_p_memory)
                    bi_alns.append(bi_aln)
                    assert s_t == eval_score(bi_aln, q, t, x, o, e, j), (
                        "inconsistent penalty"
                    )

                    # check score
                    score = (l_c * (len(t) + len(q)) - s_t) // 2
                    scores.append(score)
                    assert score == aligners[mode].score(t_str, q_str), (
                        "inconsistent score"
                    )

                df = pd.DataFrame({
                    "s_t": s_ts,
                    "score": scores,
                    "time": times,
                    "bi_time": bi_times,
                    "c_memory": c_memories,
                    "bi_c_memory": bi_c_memories,
                    "p_memory": p_memories,
                    "bi_p_memory": bi_p_memories,
                    "M_count": M_counts,
                    "M_count_nodup": M_counts_nodup,
                    "aln": alns,
                    "bi_alns": bi_aln,
                }).to_csv(
                    test_align_dir
                    / score_schema
                    / mode
                    / f"{length}_{err}_{end_up}.csv",
                    index=False,
                )
