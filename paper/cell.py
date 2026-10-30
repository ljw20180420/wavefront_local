#!/usr/bin/env python

import os
import pathlib
from collections import defaultdict

import numpy as np
import pandas as pd
import plotly.express as px
from draw import plotly_template

from wavefront_local import config
from wavefront_local.lbiwfa import populate_cell
from wavefront_local.lwfa import lwfa
from wavefront_local.score import bowtie_score, classical2wfa_sys
from wavefront_local.test import _mutate_DNA, _random_DNA


def Fr2cells(Fr: defaultdict) -> pd.DataFrame:
    ms = []
    ns = []
    for c in Fr:
        for s in Fr[c]:
            for d in Fr[c][s]:
                if hasattr(Fr[c][s][d], "f"):
                    f = Fr[c][s][d].f
                else:
                    f = Fr[c][s][d]
                n = (f + d) // 2
                m = (f - d) // 2
                ms.append(m)
                ns.append(n)

    df = pd.DataFrame({
        "m": ms,
        "n": ns,
    }).drop_duplicates(ignore_index=True)

    return df


def biFr2cells(
    Fr_f: defaultdict, Fr_r: defaultdict, tlen: int, qlen: int
) -> pd.DataFrame:
    df_f = Fr2cells(Fr_f)
    df_r = Fr2cells(Fr_r)
    df_r = df_r.assign(
        m=lambda df: df["m"].map(lambda m: tlen - m),
        n=lambda df: df["n"].map(lambda n: qlen - n),
    )
    df = pd.concat([df_f, df_r]).drop_duplicates(ignore_index=True)

    return df


def generate_example_reads(cfg: dict) -> tuple[str, str]:
    rng = np.random.default_rng(cfg["seed"])
    t = _random_DNA(cfg["length"], rng)
    end = cfg["end"]
    q = "".join([
        _random_DNA(end, rng),
        _mutate_DNA(t[end : len(t) - end], cfg["err"], rng),
        _random_DNA(end, rng),
    ])

    return t, q


def collect_cells(
    cfg: dict,
) -> tuple[pd.DataFrame, pd.DataFrame, memoryview, memoryview]:
    t, q = generate_example_reads(cfg)
    t = memoryview(t.encode())
    q = memoryview(q.encode())
    l_c, x_c, o_c, e_c = bowtie_score()
    x, o, e, j = classical2wfa_sys(l_c, x_c, o_c, e_c)

    _, _, Fr = lwfa(q, t, x, o, e, j, "L", "L")
    df = Fr2cells(Fr)

    bidfs = []
    pg = populate_cell(q, t, x, o, e, j, "L", "L")
    while True:
        try:
            Fr_f, Fr_r = next(pg)
            bidfs.append(biFr2cells(Fr_f, Fr_r, len(t), len(q)))
        except StopIteration as err:
            d_t, f_t, c_t, score_upper_t, score_lower_t, split = err.value
            break
    bidf = pd.concat(bidfs, ignore_index=True).drop_duplicates(ignore_index=True)

    return df, bidf, t, q


def display_cells(cfg: dict) -> None:
    df, bidf, t, q = collect_cells(cfg)
    cfg["cell_dir"].mkdir(exist_ok=True, parents=True)

    for name, ddf in zip(["lwfa", "lbiwfa"], [df, bidf]):
        mat = np.zeros([len(t) + 1, len(q) + 1])
        mat[ddf["m"].to_numpy(), ddf["n"].to_numpy()] = 1
        fig = px.imshow(
            mat,
            color_continuous_scale=["white", "red"],
            range_color=[0, 1],
            aspect="equal",
            labels={"x": "query", "y": "text"},
        )
        fig.update_layout(coloraxis_showscale=False, template=plotly_template())
        fig.update_xaxes(side="top")
        fig.write_image(cfg["cell_dir"] / f"{name}.pdf")


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.cell_config()

    display_cells(cfg)
