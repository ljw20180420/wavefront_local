#!/usr/bin/env python

import os
import pathlib

import sh

from wavefront_local import config, test


def generate_sim_bench(cfg: dict):
    for length in cfg["lengths"]:
        for err_num in cfg["err_nums"]:
            for end_up in cfg["end_ups"]:
                data_file = (
                    pathlib.Path("paper/sim/data") / f"{length}_{err_num}_{end_up}.csv"
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


def compile_sim(cfg: dict):
    for lib in cfg["libs"]:
        if lib == "seqan":
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                "-Icpp/include",
                cfg["cpp_dir"] / f"sim_{lib}.cpp",
                o=cfg["cpp_dir"] / f"sim_{lib}",
            )
        elif lib == "parasail":
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                cfg["cpp_dir"] / f"sim_{lib}.cpp",
                "-L./cpp/parasail/build",
                "-lparasail",
                "-Wl,-rpath,'$ORIGIN/../parasail/build'",
                o=cfg["cpp_dir"] / f"sim_{lib}",
            )
        else:
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                cfg["cpp_dir"] / f"sim_{lib}.cpp",
                o=cfg["cpp_dir"] / f"sim_{lib}",
            )


def bench_sim(cfg: dict):
    cfg["result_dir"].mkdir(exist_ok=True, parents=True)
    for methods in cfg["libs"].values():
        for method in methods:
            with open(cfg["result_dir"] / f"{method}.csv", "w") as fd:
                fd.write("length,err_num,end_up,time,memory\n")
                for length in cfg["lengths"]:
                    for err_num in cfg["err_nums"]:
                        for end_up in cfg["end_ups"]:
                            sh.Command(cfg["cpp_dir"] / "sim_lwfa")(
                                length,
                                err_num,
                                end_up,
                                method,
                                _out=fd,
                            )


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.sim_config()

    # generate_sim_bench(cfg)
    compile_sim(cfg)
    bench_sim(cfg)
