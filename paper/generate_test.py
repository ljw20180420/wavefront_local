#!/usr/bin/env python

import os
import pathlib

import pandas as pd

from wavefront_local.ini import read_ini
from wavefront_local.test import generate_random_reads

os.chdir(pathlib.Path(__file__).resolve().parent)

cfg = read_ini("test.ini")

test_data_dir = pathlib.Path("test/data")
size = cfg["size"][0]
seed = cfg["seed"][0]
for mode in cfg["mode"]:
    (test_data_dir / mode).mkdir(parents=True, exist_ok=True)
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
                    test_data_dir / mode / f"{length}_{err}_{end_up}.csv",
                    index=False,
                )
