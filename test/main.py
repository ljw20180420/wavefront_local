#!/usr/bin/env python

import argparse
import os
import pathlib
import sys

import sh

from wavefront_local import config, test


def handle_config() -> dict:
    parser = argparse.ArgumentParser()
    parser.add_argument("--generate", action="store_true")
    parser.add_argument("--size", type=int)
    parser.add_argument("--lengths", type=int, nargs="+")
    parser.add_argument("--errs", type=int, nargs="+")
    parser.add_argument(
        "--align-modes", type=str, choices=["local", "global"], nargs="+"
    )
    parser.add_argument("--seed", type=int)
    parser.add_argument(
        "--score_schemas", type=str, choices=["bowtie", "bwa"], nargs="+"
    )
    parser.add_argument(
        "--aligners",
        type=str,
        choices=["pylwfa", "pylbiwfa", "cpplwfa", "cpplbiwfa"],
        nargs="+",
    )
    args = parser.parse_args()

    cfg = config.test_config() | {
        key: val for key, val in vars(args).items() if val is not None
    }

    return cfg


def run_test(cfg: dict) -> None:
    for length in cfg["lengths"]:
        for err in cfg["errs"]:
            for align_mode in cfg["align_modes"]:
                data_file = (
                    pathlib.Path("test/data")
                    / align_mode
                    / f"{length}_{err:.2}_{cfg['end_up']}.csv"
                )
                if cfg["generate"]:
                    test.generate_sim_data(
                        size=cfg["size"],
                        length=length,
                        err=err,
                        end_up=cfg["end_up"],
                        align_mode=align_mode,
                        seed=cfg["seed"],
                        data_file=data_file,
                    )
                for score_schema in cfg["score_schemas"]:
                    for aligner in cfg["aligners"]:
                        align_file = (
                            pathlib.Path("test/align")
                            / aligner
                            / align_mode
                            / score_schema
                            / f"{length}_{err:.2}_{cfg['end_up']}.csv"
                        )
                        test.test(
                            align_mode=align_mode,
                            data_file=data_file,
                            score_schema=score_schema,
                            aligner=aligner,
                            align_file=align_file,
                        )


def run_example() -> None:
    sh.Command("examples/lwfa.sh")(_out=sys.stdout, _err=sys.stderr)
    sh.Command("examples/lbiwfa.sh")(_out=sys.stdout, _err=sys.stderr)
    sh.Command("examples/example.py")(_out=sys.stdout, _err=sys.stderr)


if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)

    # cfg = handle_config()
    # run_test(cfg)

    run_example()
