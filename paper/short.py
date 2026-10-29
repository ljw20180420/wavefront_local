#!/usr/bin/env python

import os
import pathlib
import sys

import pandas as pd
import pysam
import sh
from compile import compile_libs

from wavefront_local import config


def download_short_reads(cfg: dict) -> None:
    accession = cfg["short"]["accession"]
    sh.prefetch(
        accession,
        max_size="100GB",
        output_directory=cfg["real_dir"],
        _out=sys.stdout,
        _err=sys.stderr,
    )

    sh.vdb_validate(
        cfg["real_dir"] / accession / f"{accession}.sra",
        _out=sys.stdout,
        _err=sys.stderr,
    )

    sh.fasterq_dump(
        cfg["real_dir"] / accession / f"{accession}.sra",
        outdir=cfg["real_dir"] / cfg["short"]["accession"],
        _out=sys.stdout,
        _err=sys.stderr,
    )


def align_short_reads(cfg: dict) -> None:
    accession = cfg["short"]["accession"]
    sh.samtools.view(
        "-b",
        "-",
        o=cfg["real_dir"] / accession / f"{accession}.bam",
        _in=sh.bowtie2(
            "-1",
            cfg["real_dir"] / accession / f"{accession}_1.fastq",
            "-2",
            cfg["real_dir"] / accession / f"{accession}_2.fastq",
            p=16,
            x=cfg["short"]["hs1"],
            _piped=True,
        ),
        _out=sys.stdout,
        _err=sys.stderr,
    )


def index_short_bam(cfg: dict) -> None:
    accession = cfg["short"]["accession"]
    pysam.samtools.sort(
        "-@",
        "8",
        "-o",
        os.fspath(cfg["real_dir"] / accession / f"{accession}.sorted.bam"),
        os.fspath(cfg["real_dir"] / accession / f"{accession}.bam"),
        catch_stdout=False,
    )
    sh.mv(
        cfg["real_dir"] / accession / f"{accession}.sorted.bam",
        cfg["real_dir"] / accession / f"{accession}.bam",
    )
    pysam.samtools.index(
        "-@",
        8,
        os.fspath(cfg["real_dir"] / accession / f"{accession}.bam"),
        catch_stdout=False,
    )


def generate_short_data(cfg: dict) -> None:
    accession = cfg["short"]["accession"]
    texts = []
    queries = []
    with pysam.AlignmentFile(cfg["real_dir"] / accession / f"{accession}.bam") as bam:
        for read in bam.fetch():
            if read.is_unmapped:
                continue

            text = read.get_reference_sequence().upper()
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
        fd.write("method,time,memory\n")
        fd.flush()
        for lib, methods in cfg["libs"].items():
            for method in methods:
                print(method, file=sys.stderr)
                for time_memory in sh.Command(cfg["bin_dir"] / "bench")(
                    cfg["short"]["data_dir"] / "short.csv",
                    lib,
                    method,
                    _err=sys.stderr,
                    _iter=True,
                ):
                    time, memory = time_memory.strip().split(",")
                    fd.write(f"{method},{time},{memory}\n")
                    fd.flush()


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()

    # download_short_reads(cfg)
    # align_short_reads(cfg)
    # index_short_bam(cfg)
    generate_short_data(cfg)
    bench_short_data(cfg)
