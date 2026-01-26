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
## What we did (implementation summary)

We started from the official template repository **SGSSGene/ImplementingSearch**:

https://github.com/SGSSGene/ImplementingSearch

Following the instructions provided in the template README, we:

1. **Cloned the repository (including submodules)** to get the full project structure and dependencies.
2. Implemented the missing parts marked as `//!TODO ImplementMe` in:
   - `src/naive_search.cpp`
   - `src/suffixarray_search.cpp`
3. Built the C++ project using CMake and the provided build setup.
4. Ran the compiled binaries and tested different input sizes by changing the `--query_ct` argument.
5. Measured and compared the runtime of:
   - the **naive search** approach
   - the **suffix array based** search approach

The goal was to compare the performance difference between a direct search baseline and an indexed search method.

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

### Assignment 2:


```bash
DATA=/home/mi/tiloa00/Suffix-Array-main/data
R=$DATA/reference/text.dna4.short.fasta
```

Implement an fmindex based search
It requires the same dependencies and environment, in the addition of tracemalloc for memory checking.
Benchmarking Results:
```bash
for N in 1000 10000 100000 1000000; do

  for L in 100; do
    Q=$DATA/illumina_reads_${L}.fasta.gz

    echo "== suffix array len=$L N=$N =="
    /usr/bin/time -v python fmindex_search.py --reference "$R" --query "$Q" --query_ct "$N"
done
```

python suffix Benchmarks on home computer with text.dna4.short.fasta.index(Issues with running on the server):
--query_ct "100"
total_hits	40
build_ms	12544
search_ms	1

--query_ct "1000"
total_hits	448
build_ms	12545
search_ms	14

python fmindex Benchmarks on home computer:
--query_ct "100"
total_hits	40
build_ms	91156
search_ms	3

--query_ct "1000"
total_hits	448
build_ms	91220
search_ms	29

--query_ct "10000"
total_hits	4456
build_ms	94796
search_ms	286

--query_ct "100000"
total_hits	45335
build_ms	91413
search_ms	2641

--query_ct "1000000"
total_hits	453350
build_ms	91679
search_ms	26269

This Python's fmindex is always worse than the suffix array implementation in runtime, since it also requires a suffix array, and due to
```bash
self.Occ = {c: [0] * (self.n + 1) for c in self.alphabet}
```
since that uses Python integers and has a huge memory footprint.
It technically works, by having a stable FMindex buildtime of ~91500 ms, with a linearly scaling search ms time.

python fmindex Benchmarks on home computer while checking for memory usage using tracemalloc:
--query_ct "1000"
total_hits	448
index_mem_kb	10742296

--query_ct "10000"
total_hits	4456
index_mem_kb	10742295

Comparing to Server python execution:

== suffix array len=100 N=1000 ==
total_hits      448
build_ms        99906
search_ms       34
User time (seconds): 101.27
System time (seconds): 4.30
Elapsed (wall clock) time (h:mm:ss or m:ss): 1:45.63

== suffix array len=100 N=1000000 ==
total_hits      453350
build_ms        97869
search_ms       30759
User time (seconds): 130.09
System time (seconds): 4.32
Elapsed (wall clock) time (h:mm:ss or m:ss): 2:14.50

On the server, the runtime was a bit slower, but we can see with the linux time benchmark, that the runtime increase scales well with larger query_ct.

Benchmark the Human reference genome:

```bash
R=$DATA/reference/GCF_000001405.26_GRCh38_genomic.fna
N=10000
for L in 40 60 80 100; do
  Q=$DATA/illumina_reads_${L}.fasta.gz
  echo "== suffix array len=$L N=$N =="
  /usr/bin/time -v python fmindex_search.py --reference "$R" --query "$Q" --query_ct "$N"
done
```
At the time of submitting, it was still running. Attempts at running it locally caused PC crashes, and I only managed server access very late.


### Conclusion
Normally, the FMindex should be vastly superior in both runtime and memory compared to the suffix array. Our implementation is lacking in both. We also attempted implementing it in C++, but had issues with the Seqan3 integration, as described in the github, and didn't finish it in time. Our cpp FMindex is attached in the submission.
Our implementation did however achieve a runtime scaling well with query_ct.
