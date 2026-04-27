# CVRP_Greedy_vs_ClarkeWright_vs_LocalSearch_Synthetic

## Capacitated Vehicle Routing Problem: Algorithm Comparison

A comparative empirical study of three heuristic algorithms for the Capacitated Vehicle Routing Problem (CVRP), evaluated on the Solomon Benchmark.

### Team Members

- **Bryson Bargas** — [GitHub](https://github.com/BryceMani4c)
- **Noah Boggess** — [GitHub](https://github.com/NoahtheBman)

Tennessee Technological University — CSC 2400: Design of Algorithms (Spring 2026)

---

### Problem Description

The Capacitated Vehicle Routing Problem (CVRP) is the problem of determining optimal routes for a fleet of vehicles delivering goods to a set of customers from a central depot, subject to the following constraints:

1. Each customer is visited exactly once.
2. Vehicle capacity is respected.
3. Total travel distance is minimized.

CVRP is **NP-Hard** because it generalizes the Traveling Salesman Problem (TSP) with additional capacity constraints, making it at least as hard as TSP. Exact solutions become computationally infeasible for non-trivial customer counts, so heuristic and approximation algorithms are typically used in practice.

**Real-world applications:** logistics and package delivery (Amazon, UPS), ride-sharing route optimization (Uber), waste collection routing, and emergency dispatch routing.

---

### Algorithms Implemented

| Algorithm | Role | Time Complexity |
|-----------|------|-----------------|
| Greedy Nearest Neighbor | Baseline benchmark | O(n²) |
| Clarke-Wright Savings | Constructive heuristic | O(n² log n) |
| Local Search (2-opt) | Improvement heuristic over Greedy | O(k · n²) |

All three algorithms are fully implemented and tested.

> **Scope note:** Ant Colony Optimization (ACO) was originally planned as a fourth algorithm but was removed from scope after a team member left the project. The team refocused on a thorough comparison of the three algorithms above.

---

### Dataset

We use the **Solomon Benchmark**, a widely cited set of CVRP instances created by Professor Marius M. Solomon (1987). Five instances were used in the experiments, each with 100 customers but different distribution patterns and capacity constraints:

| Instance | Type | Capacity | Total Demand |
|----------|------|----------|--------------|
| C101 | Clustered, tight capacity | 200 | 1810 |
| C201 | Clustered, large capacity | 700 | 1810 |
| R101 | Random, tight capacity | 200 | 1458 |
| R201 | Random, large capacity | 1000 | 1458 |
| RC101 | Random + Clustered | 200 | 1724 |

We initially considered generating synthetic datasets, but switched to the established Solomon benchmark for more reliable, comparable results.

---

### Repository Structure

```
.
├── code/             # C++ algorithm implementations and experiment runner
├── data/             # Solomon Benchmark datasets (.txt)
├── results/          # Per-dataset run output saved by the program
├── graphs/           # Comparison figures (distance, runtime, improvement, vehicles)
├── report/           # Final scientific report (DOCX + PDF)
├── slides/           # Presentation deck (PPTX)
├── Documents/        # Project requirement docs and checkpoints
└── README.md
```

---

### Code Structure

The project is implemented in **C++17** as four self-contained `.cpp` files:

- **`greedy.cpp`** — Greedy Nearest Neighbor algorithm
- **`clarke_wright.cpp`** — Clarke-Wright Savings algorithm
- **`local_search.cpp`** — 2-opt Local Search that improves Greedy's solution
- **`run_experiments.cpp`** — Driver: loads a Solomon instance, runs all three algorithms, prints results, and offers to save them to `results/`

Each `.cpp` file is independent — there are no shared header files. Each algorithm file declares its own copy of the shared `Data` / `Result` struct definitions and `static` helper functions, and the runner forward-declares the three algorithm functions to call them.

---

### How to Run

#### Requirements

- **g++** with C++17 support (tested with g++ 15.2 from MSYS2 UCRT64)
- A Solomon Benchmark `.txt` file in the `data/` folder (five are included)

#### Build

From the `code/` folder, in a terminal:

```
g++ -std=c++17 greedy.cpp clarke_wright.cpp local_search.cpp run_experiments.cpp -o run_experiments.exe
```

#### Run

```
run_experiments.exe
```

The program lists every `.txt` instance found in `../data/`, prompts you to pick one, runs all three algorithms, prints route-by-route output and a summary comparison table, and finally asks whether to save the full report to `../results/<instance>_results.txt`.

#### Windows / VS Code setup

If you're on Windows and `g++` isn't recognized, install MSYS2 and add `C:\msys64\ucrt64\bin` to your PATH:

```
winget install MSYS2.MSYS2
```

Then in the **MSYS2 UCRT64** shell:

```
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
```

Restart your terminal (and VS Code) after updating PATH.

---

### Results

Empirical results across all five Solomon instances (5 runs per algorithm per dataset):

| Instance | Greedy Distance | Clarke-Wright Distance | 2-opt Distance | CW vs Greedy | 2-opt vs Greedy |
|----------|----------------:|-----------------------:|---------------:|-------------:|----------------:|
| C101 | 1311.50 | **833.51** | 1239.44 | +36.45% | +5.49% |
| C201 | 775.49 | **619.86** | 710.14 | +20.07% | +8.43% |
| R101 | 1174.36 | **889.00** | 1112.61 | +24.30% | +5.26% |
| R201 | 802.73 | **706.48** | 734.11 | +11.99% | +8.55% |
| RC101 | 1422.60 | **1040.76** | 1417.05 | +26.84% | +0.39% |

| Algorithm | Average Runtime |
|-----------|----------------:|
| Greedy Nearest Neighbor | ~0.2 ms |
| Clarke-Wright Savings | 14 – 18 ms |
| Local Search (2-opt) | 0.4 – 11 ms |

**Key findings:**

- Clarke-Wright produced the shortest routes on **every** dataset.
- Greedy was the fastest by a large margin but produced the longest routes on every dataset.
- Local Search's improvement over Greedy was highly dataset-dependent (0.39% to 8.55%) — its runtime also varied widely depending on how many improvement passes were needed.
- All three algorithms used the same number of vehicles within each dataset, so quality differences came from route reordering rather than fleet size.

See `graphs/` for the corresponding figures and `report/` for the full analysis.

---

### Research Questions

These were defined prior to running any experiments:

1. How does solution quality (total distance) compare between Greedy and Clarke-Wright?
2. Does Local Search significantly improve the Greedy solution?
3. How do the algorithms compare in terms of runtime efficiency?
4. What trade-offs exist between solution quality and computational cost?

### Performance Metrics

- Total route distance
- Number of vehicles used
- Wall-clock runtime (seconds, averaged over 5 runs per algorithm per dataset)
- Percent improvement over the Greedy baseline

---

### GenAI Usage Disclosure

Generative AI tools (Claude by Anthropic) were used to assist with translating the initial Python prototype to C++, configuring the MinGW / VS Code build environment, generating the matplotlib comparison graphs from raw experiment output, and proofreading sections of the report and this README. All algorithm logic, experimental design, and analysis were developed and verified by the team.

---

### References

- Solomon, M. M. (1987). *Algorithms for the vehicle routing and scheduling problems with time window constraints.* Operations Research, 35(2), 254–265.
- Clarke, G. & Wright, J. W. (1964). *Scheduling of vehicles from a central depot to a number of delivery points.* Operations Research, 12(4), 568–581.
