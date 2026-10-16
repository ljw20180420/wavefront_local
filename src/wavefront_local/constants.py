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
    l_c: int
    x_c: int
    o_c: int
    e_c: int

    @classmethod
    def bowtie(cls):
        return cls(
            l_c=2,
            x_c=6,
            o_c=5,
            e_c=3,
        )

    @classmethod
    def bwa(cls):
        return cls(
            l_c=1,
            x_c=4,
            o_c=6,
            e_c=1,
        )
