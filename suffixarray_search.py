import argparse
import time
import iv2py as iv
from common import load_fasta_concat, load_queries, duplicate_to_n

def lower_bound_sa(text: str, sa, pattern: str) -> int:
    # first suffix with prefix >= pattern
    lo, hi = 0, len(sa)
    m = len(pattern)
    while lo < hi:
        mid = (lo + hi) // 2
        pos = sa[mid]
        s = text[pos : pos + m]
        if s < pattern:
            lo = mid + 1
        else:
            hi = mid
    return lo

def upper_bound_sa(text: str, sa, pattern: str) -> int:
    # first suffix with prefix > pattern
    lo, hi = 0, len(sa)
    m = len(pattern)
    while lo < hi:
        mid = (lo + hi) // 2
        pos = sa[mid]
        s = text[pos : pos + m]
        if s <= pattern:
            lo = mid + 1
        else:
            hi = mid
    return lo

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--reference", required=True)
    ap.add_argument("--query", required=True)
    ap.add_argument("--query_ct", type=int, default=100)
    args = ap.parse_args()

    ref = load_fasta_concat(args.reference)
    queries = duplicate_to_n(load_queries(args.query), args.query_ct)

    # Build suffix array using IV2py. :contentReference[oaicite:3]{index=3}
    t0 = time.perf_counter()
    sa = iv.create_suffixarray(ref)
    t1 = time.perf_counter()

    total_hits = 0
    t2 = time.perf_counter()
    for q in queries:
        if not q:
            continue
        lb = lower_bound_sa(ref, sa, q)
        ub = upper_bound_sa(ref, sa, q)
        total_hits += (ub - lb)
    t3 = time.perf_counter()

    print(f"total_hits\t{total_hits}")
    print(f"build_ms\t{int((t1 - t0) * 1000)}")
    print(f"search_ms\t{int((t3 - t2) * 1000)}")

if __name__ == "__main__":
    main()
