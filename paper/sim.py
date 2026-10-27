#!/usr/bin/env python

import os
import pathlib

import sh
from compile import compile_libs

from wavefront_local import config, test


def generate_sim(cfg: dict):
    cfg["sim"]["data_dir"].mkdir(exist_ok=True, parents=True)
    for length in cfg["sim"]["lengths"]:
        for err_num in cfg["sim"]["err_nums"]:
            for end_up in cfg["sim"]["end_ups"]:
                data_file = cfg["sim"]["data_dir"] / f"{length}_{err_num}_{end_up}.csv"
                test.generate_sim_data(
                    size=cfg["sim"]["size"],
                    length=length,
                    err=err_num / length,
                    end_up=end_up,
                    align_mode="local",
                    seed=cfg["seed"],
                    data_file=data_file,
                )


def bench_sim(cfg: dict):
    compile_libs(cfg)
    cfg["sim"]["result_dir"].mkdir(exist_ok=True, parents=True)
    for lib, methods in cfg["libs"].items():
        for method in methods:
            with open(cfg["sim"]["result_dir"] / f"{method}.csv", "w") as fd:
                fd.write("length,err_num,end_up,method,time,memory\n")
                for length in cfg["sim"]["lengths"]:
                    for err_num in cfg["sim"]["err_nums"]:
                        for end_up in cfg["sim"]["end_ups"]:
                            data_file = (
                                cfg["sim"]["data_dir"]
                                / f"{length}_{err_num}_{end_up}.csv"
                            )
                            fd.writelines(
                                f"{length},{err_num},{end_up},{method},{time_memory}"
                                for time_memory in sh.Command(cfg["bin_dir"] / "bench")(
                                    data_file,
                                    lib,
                                    method,
                                    _iter=True,
                                )
                            )


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()

    # generate_sim(cfg)
    bench_sim(cfg)
