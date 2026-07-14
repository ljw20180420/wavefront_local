def extend(q: memoryview, t: memoryview, offset: int, d: int) -> int:
    n = (offset + d) // 2
    m = (offset - d) // 2
    mu = 0
    while n + mu < len(q) and m + mu < len(t) and q[n + mu] == t[m + mu]:
        mu += 1
    return mu
