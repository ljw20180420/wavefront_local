#!/usr/bin/env python

import os
import pathlib

from wavefront_local import config, test

if __name__ == "__main__":
    os.chdir(pathlib.Path(__file__).resolve().parent)

    cfg = config.test_config()
    for length in cfg["lengths"]:
        for err in cfg["errs"]:
            for align_mode in cfg["align_modes"]:
                data_file = (
                    pathlib.Path("data")
                    / align_mode
                    / f"{length}_{err:.2}_{cfg['end_up']}.csv"
                )
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
                    for aligner in cfg["aligner"]:
                        align_file = (
                            pathlib.Path("align")
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
