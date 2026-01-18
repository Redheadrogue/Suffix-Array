# Assignment 1 — Naive Search vs Suffix Array Search (Python + C++) by the Jonas Brothers 

<img width="1536" height="1024" alt="ChatGPT Image Jan 13, 2026, 12_40_28 AM" src="https://github.com/user-attachments/assets/98c0b52f-8786-4f6b-b9ab-1a825fbbea90" />

This repository contains implementations and benchmarks for exact string search on DNA data:

* **Naive search** (baseline): scans the reference for each query
* **Suffix array search** (indexed): builds a suffix array once and answers queries using binary search

The **Python versions** are used for the experiments/benchmarking. The **C++ files** are included as part of the submission.

---

## Contents

### Python

* `common.py` — shared helpers (FASTA loading, query handling)
* `naive_search.py` — naive exact search
* `suffixarray_search.py` — suffix array build + binary search queries

### C++

* `naive_search.cpp`
* `suffixarray_search.cpp`

---

## Requirements

Recommended environment: **Linux** (e.g., FU compute servers)

* Python **3.9+**
* `python3-venv`

Optional (only if your system needs build tools for any dependency):

* `build-essential`
* `python3-dev`

Install (Debian/Ubuntu):

```bash
sudo apt-get update
sudo apt-get install -y python3 python3-venv python3-dev build-essential
```

---

## Setup

Create a virtual environment and install dependencies:

```bash
python3 -m venv .venv
source .venv/bin/activate

python -m pip install -U pip wheel
python -m pip install -U iv2py
```

Sanity check:

```bash
python -c "import iv2py; print('iv2py OK')"
```

---

## Data

You need:

* a reference FASTA file (e.g. `hg38_partial.fasta.gz`)
* a query FASTA file (Illumina reads, e.g. `illumina_reads_100.fasta.gz`)

Example paths on FU:

```bash
DATA=/home/mi/danim02/advalg-assignment1-python/c++/ImplementingSearch/data
R=$DATA/hg38_partial.fasta.gz
```

Available query files (example):

* `illumina_reads_40.fasta.gz`
* `illumina_reads_60.fasta.gz`
* `illumina_reads_80.fasta.gz`
* `illumina_reads_100.fasta.gz`

---

## How to run

### Naive search (baseline)

```bash
python naive_search.py --reference "$R" --query "$DATA/illumina_reads_40.fasta.gz" --query_ct 1000
```

### Suffix array search

```bash
python suffixarray_search.py --reference "$R" --query "$DATA/illumina_reads_40.fasta.gz" --query_ct 1000
```

### Arguments

* `--reference` : path to reference FASTA/FASTA.GZ
* `--query` : path to query FASTA/FASTA.GZ
* `--query_ct` : number of queries to take from the query file (used for benchmarking)

---

## Output

### `naive_search.py`

Prints:

* `total_hits    <int>`
* `search_ms     <int>`

Example:

```
total_hits    12345
search_ms     789
```

### `suffixarray_search.py`

Prints:

* `total_hits    <int>`
* `build_ms      <int>`
* `search_ms     <int>`

Example:

```
total_hits    12345
build_ms      120
search_ms     50
```

---

## Correctness check

For the **same** inputs (`reference`, `query`, `query_ct`), both implementations must report the **same**:

* `total_hits`

---

## Benchmarking

Recommended on Linux with GNU `time` to measure both runtime and memory.

### A) Query length = 100, varying number of queries (10³ / 10⁴ / 10⁵ / 10⁶)

```bash
Q100=$DATA/illumina_reads_100.fasta.gz

for N in 1000 10000 100000 1000000; do
  echo "== naive len=100 N=$N =="
  /usr/bin/time -v python naive_search.py --reference "$R" --query "$Q100" --query_ct "$N"

  echo "== suffix array len=100 N=$N =="
  /usr/bin/time -v python suffixarray_search.py --reference "$R" --query "$Q100" --query_ct "$N"
done
```

Record from `/usr/bin/time -v`:

* `Elapsed (wall clock) time`
* `Maximum resident set size (kbytes)`

Also record from program output:

* naive: `search_ms`
* suffix array: `build_ms`, `search_ms`

---

### B) Fixed number of queries `N`, varying query length (40 / 60 / 80 / 100)

Pick an `N` that finishes reasonably fast for naive search (e.g. `1000` or `10000`):

```bash
N=10000

for L in 40 60 80 100; do
  Q=$DATA/illumina_reads_${L}.fasta.gz

  echo "== naive len=$L N=$N =="
  /usr/bin/time -v python naive_search.py --reference "$R" --query "$Q" --query_ct "$N"

  echo "== suffix array len=$L N=$N =="
  /usr/bin/time -v python suffixarray_search.py --reference "$R" --query "$Q" --query_ct "$N"
done
```
