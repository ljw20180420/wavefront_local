from dataclasses import dataclass, field
from typing import Literal

from .constants import ScoreSchema


@dataclass(frozen=True)
class SimDataConfig:
    score_schema: ScoreSchema = field(default=ScoreSchema, init=False)
    seed: int = field(default=63036, init=False)
    align_mode: Literal["local", "global"]
    size: int
    length: int
    err: float
    end_up: int
