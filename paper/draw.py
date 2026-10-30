#!/usr/bin/env python

import os
import pathlib

import pandas as pd
import plotly.express as px
import plotly.graph_objects as go

from wavefront_local import config


def plotly_template() -> go.layout.Template:
    return go.layout.Template(
        layout=go.Layout(
            width=500,
            height=500,
            font={
                "size": 25,
                "color": "black",
            },
            xaxis={
                "ticks": "outside",
                "ticklen": 10,
                "automargin": True,
            },
            yaxis={
                "ticks": "outside",
                "ticklen": 10,
                "automargin": True,
            },
        )
    )


def draw_sim(cfg: dict):
    df = pd.concat(
        [
            pd.read_csv(cfg["sim"]["result_dir"] / f"{method}.csv", header=0)
            for methods in cfg["libs"].values()
            for method in methods
        ],
        ignore_index=True,
    ).assign(
        method=lambda df: df["method"].map(config.method_name_mapping),
        err=lambda df: df["err"].map(lambda err: f"{err:.0%}"),
    )

    for target in ["time", "memory"]:
        df.pivot_table(
            values=target, index="method", columns=["length", "err"]
        ).rename_axis(index=None, columns=["length", "error rate"]).style.format(
            escape="latex-math", precision=cfg["precision"][target]
        ).to_latex(buf=cfg["table_dir"] / f"sim_{target}.tex")

    for length in cfg["sim"]["lengths"]:
        df_slice = df.query("length == @length").reset_index(drop=True)
        for target in ["time", "memory"]:
            fig = px.bar(
                df_slice,
                x="err",
                y=target,
                color="method",
                barmode="group",
                labels={
                    target: "peak space (KiB)" if target == "memory" else "time (ms)",
                    "method": "library",
                    "err": "error rate",
                },
                category_orders={
                    "err": [f"{err:.0%}" for err in cfg["sim"]["errs"]],
                    "method": [
                        method
                        for lib, methods in cfg["libs"].items()
                        for method in methods
                    ],
                },
            )
            fig.update_layout(template=plotly_template())
            fig.update_xaxes(type="category")
            fig.write_image(cfg["figure_dir"] / f"sim_{length}_{target}.pdf")


def draw_short(cfg: dict) -> None:
    df = pd.read_csv(cfg["short"]["result_dir"] / "short.csv", header=0).assign(
        method=lambda df: df["method"].map(config.method_name_mapping)
    )
    df.set_index("method").rename_axis(index=None).style.format(
        escape="latex-math", precision=2
    ).to_latex(buf=cfg["table_dir"] / "short.tex")
    for target in ["time", "memory"]:
        fig = px.bar(
            df,
            x="method",
            y=target,
            category_orders=[
                method for lib, methods in cfg["libs"].items() for method in methods
            ],
            labels={
                "method": "library",
                target: "peak space (KiB)" if target == "memory" else "time (ms)",
            },
        )
        fig.update_layout(template=plotly_template())
        fig.write_image(cfg["figure_dir"] / f"short_{target}.pdf")


def draw_long(cfg: dict) -> None:
    df = pd.read_csv(cfg["long"]["result_dir"] / "long.csv", header=0).assign(
        method=lambda df: df["method"].map(config.method_name_mapping)
    )
    df.set_index("method").rename_axis(index=None).style.format(
        escape="latex-math", precision=2
    ).to_latex(buf=cfg["table_dir"] / "long.tex")
    for target in ["time", "memory"]:
        fig = px.bar(
            df,
            x="method",
            y=target,
            category_orders=[
                method for lib, methods in cfg["libs"].items() for method in methods
            ],
            labels={
                "method": "library",
                target: "peak space (KiB)" if target == "memory" else "time (ms)",
            },
        )
        fig.update_layout(template=plotly_template())
        fig.write_image(cfg["figure_dir"] / f"long_{target}.pdf")


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()
    cfg["figure_dir"].mkdir(exist_ok=True, parents=True)
    cfg["table_dir"].mkdir(exist_ok=True, parents=True)

    draw_sim(cfg)
    draw_short(cfg)
    draw_long(cfg)
