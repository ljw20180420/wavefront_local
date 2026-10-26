#!/usr/bin/env python

import os
import pathlib

import matplotlib
import pandas as pd
import sh
from plotnine import aes, geom_line, ggplot, scale_x_continuous, scale_y_continuous

from wavefront_local import config, test


def generate_sim(cfg: dict):
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
                fd.write("length,err_num,end_up,method,time,memory\n")
                fd.flush()
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


def draw_sim(cfg: dict):
    matplotlib.use("Agg")

    df = pd.concat(
        [
            pd.read_csv(cfg["result_dir"] / f"{method}.csv", header=0)
            for methods in cfg["libs"].values()
            for method in methods
        ],
        ignore_index=True,
    )
    cfg["figure_dir"].mkdir(exist_ok=True, parents=True)
    cfg["table_dir"].mkdir(exist_ok=True, parents=True)

    for err_num in cfg["err_nums"]:
        for end_up in cfg["end_ups"]:
            df_filter: pd.DataFrame = df.query(
                "err_num == @err_num and end_up == @end_up"
            ).reset_index(drop=True)
            for target in ["time", "memory"]:
                df_filter.pivot_table(
                    values=target, index="method", columns="length"
                ).rename_axis(index=None, columns=None).style.format(
                    escape=True, precision=cfg["precision"][target]
                ).to_latex(buf=cfg["table_dir"] / f"{target}_{err_num}_{end_up}.tex")
                (
                    ggplot(
                        data=df_filter,
                        mapping=aes(x="length", y=target, color="method"),
                    )
                    + geom_line()
                    + scale_y_continuous(name=target, trans="log10")
                    + scale_x_continuous(name="length")
                ).save(cfg["figure_dir"] / f"{target}_{err_num}_{end_up}.pdf")


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.sim_config()

    # generate_sim(cfg)
    compile_sim(cfg)
    bench_sim(cfg)
    draw_sim(cfg)
