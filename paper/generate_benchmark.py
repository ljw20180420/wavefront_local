#!/usr/bin/env python

import os
import pathlib

import pandas as pd

from wavefront_local.ini import read_ini
from wavefront_local.test import generate_random_reads

os.chdir(pathlib.Path(__file__).resolve().parent)

cfg = read_ini("benchmark.ini")

benchmark_data_dir = pathlib.Path("benchmark/data")
size = cfg["size"][0]
seed = cfg["seed"][0]
benchmark_data_dir.mkdir(exist_ok=True, parents=True)
for length in cfg["length"]:
    for err_num in cfg["err_num"]:
        for end_up in cfg["end_up"]:
            ts, qs = generate_random_reads(
                size=size,
                length=length,
                err=err_num / length,
                end_up=end_up,
                mode="local",
                seed=seed,
            )
            pd.DataFrame({
                "text": ts,
                "query": qs,
            }).to_csv(
                benchmark_data_dir / f"{length}_{err_num}_{end_up}.csv",
                index=False,
            )
