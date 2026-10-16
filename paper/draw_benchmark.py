#!/usr/bin/env python

import os
import pathlib

import matplotlib
import pandas as pd
from plotnine import aes, geom_line, ggplot, scale_x_continuous, scale_y_continuous

from wavefront_local.ini import read_ini

# Change to project folder
os.chdir(pathlib.Path(__file__).parent.parent.resolve())

matplotlib.use("Agg")

dfs = []
for method in ["lwfa", "lbiwfa", "diag", "scan", "seqan", "striped"]:
    df = pd.read_csv(f"paper/benchmark/result/{method}.csv", header=0)
    df["method"] = method
    dfs.append(df)

df = pd.concat(dfs, ignore_index=True)

figure_dir = pathlib.Path("paper/benchmark/figure")
figure_dir.mkdir(exist_ok=True, parents=True)
table_dir = pathlib.Path("paper/benchmark/table")
table_dir.mkdir(exist_ok=True, parents=True)

cfg = read_ini("paper/benchmark.ini")
for err_num in cfg["err_num"]:
    for end_up in cfg["end_up"]:
        df_filter: pd.DataFrame = df.query(
            "err_num == @err_num and end_up == @end_up"
        ).reset_index(drop=True)
        for target in ["time", "memory"]:
            df_filter.pivot_table(
                values=target, index="method", columns="length"
            ).rename_axis(index=None, columns=None).style.format(
                escape=True, precision=cfg[f"{target}_precision"]
            ).to_latex(buf=table_dir / f"{target}_{err_num}_{end_up}.tex")
            (
                ggplot(
                    data=df_filter,
                    mapping=aes(x="length", y=target, color="method"),
                )
                + geom_line()
                + scale_y_continuous(name=target, trans="log10")
                + scale_x_continuous(name="length")
            ).save(figure_dir / f"{target}_{err_num}_{end_up}.pdf")
