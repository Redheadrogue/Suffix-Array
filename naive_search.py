import argparse
import time
from common import load_fasta_concat, load_queries, duplicate_to_n

def count_occurrences(text: str, pattern: str) -> int:
    # Counts overlapping occurrences by scanning.
    # Pure Python scanning is very slow. That is expected for the "naive" baseline.
    n = len(text)
    m = len(pattern)
    if m == 0 or n < m:
        return 0

    hits = 0
    last = n - m
    for i in range(last + 1):
        if text[i : i + m] == pattern:
            hits += 1
    return hits

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--reference", required=True)
    ap.add_argument("--query", required=True)
    ap.add_argument("--query_ct", type=int, default=100)
    args = ap.parse_args()

    ref = load_fasta_concat(args.reference)
    queries = duplicate_to_n(load_queries(args.query), args.query_ct)

    total_hits = 0
    t0 = time.perf_counter()
    for q in queries:
        total_hits += count_occurrences(ref, q)
    t1 = time.perf_counter()

    print(f"total_hits\t{total_hits}")
    print(f"search_ms\t{int((t1 - t0) * 1000)}")

if __name__ == "__main__":
    main()
