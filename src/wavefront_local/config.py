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
        "aligners": ["pylwfa", "pylbiwfa", "cpplwfa", "cpplbiwfa"],
        "generate": False,
    }


def bench_config() -> dict:
    return {
        "bin_dir": pathlib.Path("cpp/bench"),
        "libs": {
            "lwfa": ["lwfa"],
            "lbiwfa": ["lbiwfa"],
            "seqan": ["seqan"],
            "parasail": ["diag", "scan", "striped"],
        },
        "seed": 63036,
        "precision": {
            "time": 2,
            "memory": 2,
        },
        "sim": {
            "size": 100,
            "lengths": [250, 500, 1000, 2000, 4000],
            "err_nums": [10, 100],
            "end_ups": [10, 100],
            "data_dir": pathlib.Path("paper/sim/data"),
            "result_dir": pathlib.Path("paper/sim/result"),
        },
        "real_dir": pathlib.Path(os.environ["HOME"]) / "sdb1" / "lwfa",
        "short": {
            "accession": "ERR091574",
            "hs1": pathlib.Path(os.environ["HOME"])
            / ".local"
            / "share"
            / "genomes"
            / "hs1"
            / "index"
            / "bowtie2"
            / "hs1",
            "size": 100_000,
            "end_up": 10,
            "data_dir": pathlib.Path("paper/real/data"),
            "result_dir": pathlib.Path("paper/real/result"),
        },
        "long": {
            "url": "https://sra-pub-src-1.s3.amazonaws.com/ERR3278877/BOWDEN03_20161104_FNFAB42715_MN16561_WTON000140_NA12878.fast5.tgz.2",
            "accession": "ERR3278877",
            "model": "dna_r9.4.1_e8_sup@v3.6",
            "hs1": pathlib.Path(os.environ["HOME"])
            / ".local"
            / "share"
            / "genomes"
            / "hs1"
            / "index"
            / "minimap2"
            / "hs1.mmi",
            "size": 25_000,
            "end_up": 100,
            "min_length": 10_000,
            "data_dir": pathlib.Path("paper/real/data"),
            "result_dir": pathlib.Path("paper/real/result"),
        },
        "figure_dir": pathlib.Path("paper/draw/figure"),
        "table_dir": pathlib.Path("paper/draw/table"),
    }
