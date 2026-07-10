#!/usr/bin/env python


def safe_converter(value: str) -> int | float | str:
    try:
        return int(value)
    except ValueError:
        try:
            return float(value)
        except ValueError:
            return value


def read_ini(file_name: str) -> dict:
    cfg = {}
    with open(file_name, "r") as fd:
        for line in fd:
            key, value = line.split("=")
            key = key.strip()
            value = [safe_converter(v.strip()) for v in value.split(",")]
            cfg[key] = value

    return cfg


if __name__ == "__main__":
    cfg = read_ini("paper/test.ini")
