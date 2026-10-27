#!/usr/bin/env python

import sys

import numpy as np
import pandas as pd
import pysam
import sh
from compile import compile_libs

from wavefront_local import config, test


def download_short_reads(cfg: dict) -> None:
    sh.prefetch(
        cfg["short"]["accession"],
        max_size="100GB",
        output_directory=cfg["real_dir"],
        _out=sys.stdout,
        _err=sys.stderr,
    )

    sh.vdb_validate(
        cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.sra",
        _out=sys.stdout,
        _err=sys.stderr,
    )

    sh.fasterq_dump(
        cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.sra",
        outdir=cfg["real_dir"] / cfg["short"]["accession"],
        _out=sys.stdout,
        _err=sys.stderr,
    )


def align_short_reads(cfg: dict) -> None:
    sh.samtools.view(
        "-b",
        "-",
        o=cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.bam",
        _in=sh.bowtie2(
            "-1",
            cfg["real_dir"]
            / cfg["short"]["accession"]
            / f"{cfg['short']['accession']}_1.fastq",
            "-2",
            cfg["real_dir"]
            / cfg["short"]["accession"]
            / f"{cfg['short']['accession']}_2.fastq",
            p=16,
            x=cfg["short"]["hs1"],
            _piped=True,
        ),
        _out=sys.stdout,
        _err=sys.stderr,
    )


def index_short_bam(cfg: dict) -> None:
    sh.samtools.sort(
        cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.bam",
        o=cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.sorted.bam",
        threads=8,
    )
    sh.mv(
        cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.sorted.bam",
        cfg["real_dir"]
        / cfg["short"]["accession"]
        / f"{cfg['short']['accession']}.bam",
    )
    sh.samtools.index(
        cfg["real_dir"] / cfg["short"]["accession"] / f"{cfg['short']['accession']}.bam"
    )


def generate_short_data(cfg: dict) -> None:
    texts = []
    queries = []

    rng = np.random.default_rng(cfg["seed"])
    with pysam.AlignmentFile(cfg["real_dir"] / "ERR091574/ERR091574.bam") as bam:
        for read in bam.fetch():
            if read.is_unmapped:
                continue

            text = read.get_reference_sequence().upper()
            tup, tdown = rng.integers(low=0, high=cfg["short"]["end_up"] + 1, size=2)
            text = (
                test._random_DNA(tup, rng)
                + text[tup : len(text) - tdown]
                + test._random_DNA(tdown, rng)
            )

            query = read.query_sequence.upper()

            texts.append(text)
            queries.append(query)

            if len(texts) >= cfg["short"]["size"]:
                break

    cfg["short"]["data_dir"].mkdir(exist_ok=True, parents=True)
    pd.DataFrame({
        "text": texts,
        "query": queries,
    }).to_csv(cfg["short"]["data_dir"] / "short.csv", index=False)


def bench_short_data(cfg: dict) -> None:
    compile_libs(cfg)
    cfg["short"]["result_dir"].mkdir(exist_ok=True, parents=True)
    with open(cfg["short"]["result_dir"] / "short.csv", "w") as fd:
        for lib, methods in cfg["libs"].items():
            for method in methods:
                fd.write("method,time,memory\n")
                fd.writelines(
                    f"{method},{time_memory}"
                    for time_memory in sh.Command(cfg["bin_dir"] / "bench")(
                        cfg["short"]["data_dir"] / "short.csv",
                        lib,
                        method,
                        _iter=True,
                    )
                )


if __name__ == "__main__":
    cfg = config.bench_config()
    # download_short_reads(cfg)
    # align_short_reads(cfg)
    # index_short_bam(cfg)
    # generate_short_data(cfg)
    bench_short_data(cfg)
