#!/usr/bin/env python

import os
import pathlib

from wavefront_local import config, test

if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent)
    cfg = config.bench_config()

    for length in cfg["lengths"]:
        for err_num in cfg["err_nums"]:
            for end_up in cfg["end_ups"]:
                data_file = (
                    pathlib.Path("benchmark/data") / f"{length}_{err_num}_{end_up}.csv"
                )
                test.generate_sim_data(
                    size=cfg["size"],
                    length=length,
                    err=err_num / length,
                    end_up=end_up,
                    align_mode="local",
                    seed=cfg["seed"],
                    data_file=data_file,
                )
