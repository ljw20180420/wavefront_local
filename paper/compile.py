#!/usr/bin/env python

import os
import pathlib
import sys

import sh

from wavefront_local import config


def compile_libs(cfg: dict):
    sh.Command("g++")(
        "-O3",
        "-std=gnu++23",
        "-Wl,-rpath,$ORIGIN/../parasail/build",
        cfg["bin_dir"] / "bench.cpp",
        "-lparasail",
        o=cfg["bin_dir"] / "bench",
        I="cpp/include",
        L="cpp/parasail/build",
        # _out=sys.stdout,
        # _err=sys.stderr,
    )


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.bench_config()

    compile_libs(cfg)
