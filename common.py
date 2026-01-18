import iv2py as iv

def load_fasta_concat(path: str) -> str:
    # IV2py supports gz FASTA directly via iv.fasta.reader(file=...). :contentReference[oaicite:2]{index=2}
    parts = []
    for rec in iv.fasta.reader(file=path):
        parts.append(rec.seq)
    return "".join(parts)

def load_queries(path: str) -> list[str]:
    return [rec.seq for rec in iv.fasta.reader(file=path)]

def duplicate_to_n(queries: list[str], n: int) -> list[str]:
    if not queries:
        return []
    q = list(queries)
    while len(q) < n:
        q.extend(q[: min(len(q), n - len(q))])
    return q[:n]
