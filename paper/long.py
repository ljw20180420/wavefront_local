#!/usr/bin/env python

import os
import pathlib
import sys

import pandas as pd
import py2bit
import pysam
import sh
from compile import compile_libs
from pod5.tools.pod5_convert_from_fast5 import convert_from_fast5

from wavefront_local import config


def download_long_reads(cfg: dict) -> None:
    accession = cfg["long"]["accession"]
    (cfg["real_dir"] / accession).mkdir(exist_ok=True, parents=True)
    sh.wget(
        "-c",
        cfg["long"]["url"],
        O=cfg["real_dir"] / accession / f"{accession}.fast5.tgz",
        _out=sys.stdout,
        _err=sys.stderr,
    )

    single_dir = cfg["real_dir"] / accession / "single"
    single_dir.mkdir(exist_ok=True, parents=True)
    sh.tar(
        "-zx",
        f=cfg["real_dir"] / accession / f"{accession}.fast5.tgz",
        directory=single_dir,
        strip_components=1,
        _out=sys.stdout,
        _err=sys.stderr,
    )


def single_to_multi_fast5(cfg: dict) -> None:
    from ont_fast5_api.conversion_tools.single_to_multi_fast5 import (
        batch_convert_single_to_multi,
    )

    accession = cfg["long"]["accession"]
    output_folder = cfg["real_dir"] / accession / "multi"
    output_folder.mkdir(exist_ok=True, parents=True)
    batch_convert_single_to_multi(
        input_path=os.fspath(cfg["real_dir"] / accession / "single"),
        output_folder=os.fspath(output_folder),
        filename_base="batch",
        batch_size=4000,
        threads=16,
        recursive=True,
        follow_symlinks=True,
        target_compression=None,
    )


def get_pod5_from_fast5(cfg: dict):
    accession = cfg["long"]["accession"]
    convert_from_fast5(
        inputs=[cfg["real_dir"] / accession / "multi"],
        output=cfg["real_dir"] / accession / f"{accession}.pod5",
        threads=16,
    )


def download_model(cfg: dict):
    accession = cfg["long"]["accession"]
    model_dir = cfg["real_dir"] / accession / "model"
    model_dir.mkdir(exist_ok=True, parents=True)
    sh.dorado.download(
        model=cfg["long"]["model"],
        models_directory=model_dir,
        _out=sys.stdout,
        _err=sys.stderr,
    )


def call_base_pod5(cfg: dict):
    accession = cfg["long"]["accession"]
    sh.dorado.basecaller(
        cfg["real_dir"] / accession / "model" / cfg["long"]["model"],
        cfg["real_dir"] / accession / f"{accession}.pod5",
        reference=cfg["long"]["hs1"],
        _out=cfg["real_dir"] / accession / f"{accession}.bam",
        _err=sys.stderr,
    )


def index_long_bam(cfg: dict) -> None:
    accession = cfg["long"]["accession"]
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
        "8",
        os.fspath(cfg["real_dir"] / accession / f"{accession}.bam"),
        catch_stdout=False,
    )


def generate_long_data(cfg: dict) -> None:
    accession = cfg["long"]["accession"]
    texts = []
    queries = []
    cigarstrings = []
    mapping_qualities = []
    with (
        pysam.AlignmentFile(cfg["real_dir"] / accession / f"{accession}.bam") as bam,
        py2bit.open(os.fspath(cfg["long"]["2bit"])) as tb,
    ):
        for read in bam.fetch():
            if (
                read.is_unmapped
                or read.is_supplementary
                or read.is_secondary
                or read.mapping_quality < cfg["long"]["min_mapping_quality"]
            ):
                continue

            query = read.query_sequence.upper()
            if (
                len(query) < cfg["long"]["min_length"]
                or len(query) > cfg["long"]["max_length"]
            ):
                continue

            aligned_pairs = read.get_aligned_pairs(matches_only=True, with_seq=True)
            match_num = sum(rbase.isupper() for qpos, rpos, rbase in aligned_pairs)
            if match_num / len(query) < cfg["long"]["min_mapping_rate"]:
                continue

            text = tb.sequence(
                read.reference_name,
                read.reference_start,
                read.reference_end,
            ).upper()

            texts.append(text)
            queries.append(query)

            cigarstrings.append(read.cigarstring)
            mapping_qualities.append(read.mapping_quality)

            if len(texts) >= cfg["long"]["size"]:
                break

    cfg["long"]["data_dir"].mkdir(exist_ok=True, parents=True)
    pd.DataFrame({
        "text": texts,
        "query": queries,
    }).to_csv(cfg["long"]["data_dir"] / "long.csv", index=False)
    pd.DataFrame({
        "cigarstring": cigarstrings,
        "mapping_quality": mapping_qualities,
    }).to_csv(cfg["long"]["data_dir"] / "long_auxilary.csv", index=False)


def bench_long_data(cfg: dict) -> None:
    compile_libs(cfg)
    cfg["long"]["result_dir"].mkdir(exist_ok=True, parents=True)
    with open(cfg["long"]["result_dir"] / "long.csv", "w") as fd:
        fd.write("method,time,memory\n")
        fd.flush()
        for lib, methods in cfg["libs"].items():
            for method in methods:
                print(method, file=sys.stderr)
                for time_memory in sh.Command(cfg["bin_dir"] / "bench")(
                    cfg["long"]["data_dir"] / "long.csv",
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

    # download_long_reads(cfg)
    # single_to_multi_fast5(cfg)
    # get_pod5_from_fast5(cfg)
    # download_model(cfg)
    # call_base_pod5(cfg)
    # index_long_bam(cfg)
    generate_long_data(cfg)
    bench_long_data(cfg)
