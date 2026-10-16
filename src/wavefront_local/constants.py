from dataclasses import dataclass, field


@dataclass(frozen=True)
class BowtieSchema:
    l_c: int = field(default=2, init=False)
    x_c: int = field(default=6, init=False)
    o_c: int = field(default=5, init=False)
    e_c: int = field(default=3, init=False)


@dataclass(frozen=True)
class BWASchema:
    l_c: int = field(default=1, init=False)
    x_c: int = field(default=4, init=False)
    o_c: int = field(default=6, init=False)
    e_c: int = field(default=1, init=False)


@dataclass(frozen=True)
class ScoreSchema:
    bowtie: BowtieSchema = field(default=BowtieSchema(), init=False)
    bwa: BWASchema = field(default=BWASchema(), init=False)
