#!/usr/bin/env python

import os
import pathlib
import sys

import sh
from compile import compile_libs

from wavefront_local import config, test


def generate_sim(cfg: dict):
    cfg["sim"]["data_dir"].mkdir(exist_ok=True, parents=True)
    for length in cfg["sim"]["lengths"]:
        for err in cfg["sim"]["errs"]:
            data_file = cfg["sim"]["data_dir"] / f"{length}_{err}.csv"
            test.generate_sim_data(
                size=cfg["sim"]["size"],
                length=length,
                err=err,
                end_up=length * err,
                align_mode="local",
                seed=cfg["seed"],
                data_file=data_file,
            )


def bench_sim(cfg: dict):
    compile_libs(cfg)
    cfg["sim"]["result_dir"].mkdir(exist_ok=True, parents=True)
    for lib, methods in cfg["libs"].items():
        for method in methods:
            print(method, file=sys.stderr)
            with open(cfg["sim"]["result_dir"] / f"{method}.csv", "w") as fd:
                fd.write("length,err,method,time,memory\n")
                fd.flush()
                for length in cfg["sim"]["lengths"]:
                    for err in cfg["sim"]["errs"]:
                        data_file = cfg["sim"]["data_dir"] / f"{length}_{err}.csv"
                        for time_memory in sh.Command(cfg["bin_dir"] / "bench")(
                            data_file,
                            lib,
                            method,
                            _err=sys.stderr,
                            _iter=True,
                        ):
                            time, memory = time_memory.strip().split(",")
                            fd.write(f"{length},{err},{method},{time},{memory}\n")
                            fd.flush()


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()

    generate_sim(cfg)
    bench_sim(cfg)
