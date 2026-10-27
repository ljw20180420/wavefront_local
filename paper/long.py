#!/usr/bin/env python

import os
import sys

import numpy as np
import pandas as pd
import pysam
import sh
from compile import compile_libs
from ont_fast5_api.conversion_tools.single_to_multi_fast5 import (
    batch_convert_single_to_multi,
)
from pod5.tools.pod5_convert_from_fast5 import convert_from_fast5

from wavefront_local import config, test


def download_long_reads(cfg: dict) -> None:
    (cfg["real_dir"] / cfg["long"]["accession"]).mkdir(exist_ok=True, parents=True)
    sh.wget(
        "-c",
        cfg["long"]["url"],
        O=cfg["real_dir"]
        / cfg["long"]["accession"]
        / f"{cfg['long']['accession']}.fast5.tgz",
        _out=sys.stdout,
        _err=sys.stderr,
    )

    single_dir = cfg["real_dir"] / cfg["long"]["accession"] / "single"
    single_dir.mkdir(exist_ok=True, parents=True)
    sh.tar(
        "-zx",
        f=cfg["real_dir"]
        / cfg["long"]["accession"]
        / f"{cfg['long']['accession']}.fast5.tgz",
        directory=single_dir,
        strip_components=1,
        _out=sys.stdout,
        _err=sys.stderr,
    )


def single_to_multi_fast5(cfg: dict) -> None:
    output_folder = cfg["real_dir"] / cfg["long"]["accession"] / "multi"
    output_folder.mkdir(exist_ok=True, parents=True)
    batch_convert_single_to_multi(
        input_path=os.fspath(cfg["real_dir"] / cfg["long"]["accession"] / "single"),
        output_folder=os.fspath(output_folder),
        filename_base="batch",
        batch_size=4000,
        threads=16,
        recursive=True,
        follow_symlinks=True,
        target_compression=None,
    )


def get_pod5_from_fast5(cfg: dict):
    convert_from_fast5(
        inputs=[cfg["real_dir"] / cfg["long"]["accession"] / "multi"],
        output=cfg["real_dir"]
        / cfg["long"]["accession"]
        / f"{cfg['long']['accession']}.pod5",
        threads=16,
    )


def download_model(cfg: dict):
    model_dir = cfg["real_dir"] / cfg["long"]["accession"] / "model"
    model_dir.mkdir(exist_ok=True, parents=True)
    sh.dorado.download(
        model=cfg["long"]["model"],
        models_directory=model_dir,
        _out=sys.stdout,
        _err=sys.stderr,
    )


def call_base_pod5(cfg: dict):
    sh.dorado.basecaller(
        cfg["real_dir"] / cfg["long"]["accession"] / "model" / cfg["long"]["model"],
        cfg["real_dir"] / cfg["long"]["accession"] / f"{cfg['long']['accession']}.pod5",
        reference=cfg["long"]["hs1"],
        _out=cfg["real_dir"]
        / cfg["long"]["accession"]
        / f"{cfg['long']['accession']}.bam",
        _err=sys.stderr,
    )


if __name__ == "__main__":
    cfg = config.bench_config()
    # download_long_reads(cfg)
    # single_to_multi_fast5(cfg)
    # get_pod5_from_fast5(cfg)
    # download_model(cfg)
    # call_base_pod5(cfg)
