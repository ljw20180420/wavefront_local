import os
import pathlib

import matplotlib
import pandas as pd
from plotnine import (
    aes,
    geom_col,
    geom_line,
    geom_point,
    ggplot,
    scale_x_continuous,
    scale_x_discrete,
    scale_y_continuous,
)

from wavefront_local import config


def draw_sim(cfg: dict):
    df = pd.concat(
        [
            pd.read_csv(cfg["sim"]["result_dir"] / f"{method}.csv", header=0)
            for methods in cfg["libs"].values()
            for method in methods
        ],
        ignore_index=True,
    )
    for err_num in cfg["sim"]["err_nums"]:
        for end_up in cfg["sim"]["end_ups"]:
            df_slice: pd.DataFrame = df.query(
                "err_num == @err_num and end_up == @end_up"
            ).reset_index(drop=True)
            for target in ["time", "memory"]:
                df_slice.pivot_table(
                    values=target, index="method", columns="length"
                ).rename_axis(index=None, columns=None).style.format(
                    escape=True, precision=cfg["precision"][target]
                ).to_latex(
                    buf=cfg["table_dir"] / f"sim_{err_num}_{end_up}_{target}.tex"
                )
                (
                    ggplot(
                        data=df_slice,
                        mapping=aes(x="length", y=target, color="method"),
                    )
                    + geom_point()
                    + geom_line()
                    + scale_x_continuous(name="length")
                    + scale_y_continuous(name=target, trans="log10")
                ).save(cfg["figure_dir"] / f"sim_{err_num}_{end_up}_{target}.pdf")


def draw_short(cfg: dict) -> None:
    df = pd.read_csv(cfg["short"]["result_dir"] / "short.csv", header=0)
    for target in ["time", "memory"]:
        df[["method", target]].style.format(
            escape=True, precision=cfg["precision"][target]
        ).to_latex(buf=cfg["table_dir"] / f"short_{target}.tex")
        (
            ggplot(
                data=df,
                mapping=aes(x="method", y=target),
            )
            + geom_col()
            + scale_x_discrete(name="length")
            + scale_y_continuous(name=target)
        ).save(cfg["figure_dir"] / f"short_{target}.pdf")


def draw_long(cfg: dict) -> None:
    df = pd.read_csv(cfg["long"]["result_dir"] / "long.csv", header=0)
    for target in ["time", "memory"]:
        df[["method", target]].style.format(
            escape=True, precision=cfg["precision"][target]
        ).to_latex(buf=cfg["table_dir"] / f"long_{target}.tex")
        (
            ggplot(
                data=df,
                mapping=aes(x="method", y=target),
            )
            + geom_col()
            + scale_x_discrete(name="length")
            + scale_y_continuous(name=target)
        ).save(cfg["figure_dir"] / f"long_{target}.pdf")


if __name__ == "__main__":
    matplotlib.use("Agg")
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()
    cfg["figure_dir"].mkdir(exist_ok=True, parents=True)
    cfg["table_dir"].mkdir(exist_ok=True, parents=True)

    draw_sim(cfg)
    draw_short(cfg)
    draw_long(cfg)
