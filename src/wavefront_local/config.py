import os
import pathlib


def test_config() -> dict:
    return {
        "size": 100,
        "lengths": [100, 200],
        "errs": [0.01, 0.05, 0.1],
        "end_up": 10,
        "align_modes": ["local", "global"],
        "seed": 63036,
        "score_schemas": ["bowtie", "bwa"],
        "aligner": ["pylwfa", "pylbiwfa", "cpplwfa", "cpplbiwfa"],
    }


def bench_config() -> dict:
    return {
        "size": 100,
        "lengths": [250, 500, 1000, 2000, 4000],
        "err_nums": [10, 100],
        "end_ups": [10, 100],
        "seed": 63036,
        "methods": ["lwfa", "lbiwfa", "diag", "scan", "seqan", "striped"],
        "precision": {
            "time": 2,
            "memory": 2,
        },
    }


def real_config() -> dict:
    return {"real_dir": pathlib.Path(os.environ["HOME"]) / "sdb1" / "lwfa"}
