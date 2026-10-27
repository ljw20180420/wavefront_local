#!/usr/bin/env python

import os
import pathlib

import sh

from wavefront_local import config


def compile_libs(cfg: dict):
    for lib in cfg["libs"]:
        if lib == "seqan":
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                "-Icpp/include",
                cfg["cpp_dir"] / f"bench_{lib}.cpp",
                o=cfg["cpp_dir"] / f"bench_{lib}",
            )
        elif lib == "parasail":
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                cfg["cpp_dir"] / f"bench_{lib}.cpp",
                "-L./cpp/parasail/build",
                "-lparasail",
                "-Wl,-rpath,$ORIGIN/../parasail/build",
                o=cfg["cpp_dir"] / f"bench_{lib}",
            )
        else:
            sh.Command("g++")(
                "-O3",
                "-std=gnu++23",
                cfg["cpp_dir"] / f"bench_{lib}.cpp",
                o=cfg["cpp_dir"] / f"bench_{lib}",
            )


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    cfg = config.compile_config()

    compile_libs(cfg)
