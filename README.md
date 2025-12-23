# GraphPartitioning

Graph partitioning and cut solvers for undirected, weighted graphs in C++17. The project
includes an abstract solver interface you can inherit from, plus several concrete
implementations (exact and heuristic).

## Graph model

All solvers operate on `WeightedGraph` from `GraphUtils.h`.

- Undirected, weighted graph with nonnegative edge weights.
- Vertices are indexed `0..n-1`.
- Use `add_undirected(u, v, w)` to add an edge.

`PartitionResult` carries solver output:

- `part[v]`: block label for vertex `v` (meaning depends on solver).
- `separator`: optional list for separator problems.
- `cut_weight`: total cut weight (if applicable).
- `score`: optional additional metric.

## Abstract solver interface

To add a new algorithm, inherit from `IGraphPartitionSolver`
in `GraphPartitionSolver.h` and implement all methods.

```cpp
class IGraphPartitionSolver
{
public:
    virtual ~IGraphPartitionSolver() = default;
    virtual std::string name() const = 0;
    virtual std::string statement() const = 0;
    virtual std::string complexity() const = 0;
    virtual void solve(const WeightedGraph &g) = 0;
    virtual PartitionResult result() const = 0;
    virtual void print(std::ostream &os) const = 0;
};
```

Implementation expectations:

- `solve`: runs the algorithm and stores the output internally.
- `result`: returns the cached `PartitionResult`.
- `print`: writes a readable summary (name, statement, complexity, result).
- `name`, `statement`, `complexity`: user-facing metadata.

### Example skeleton

```cpp
#include "GraphPartitionSolver.h"

class MySolver final : public IGraphPartitionSolver
{
public:
    std::string name() const override { return "My Solver"; }
    std::string statement() const override { return "Problem statement..."; }
    std::string complexity() const override { return "Complexity..."; }

    void solve(const WeightedGraph &g) override
    {
        res_ = {};
        // compute partition into res_.part, res_.cut_weight, etc.
    }

    PartitionResult result() const override { return res_; }

    void print(std::ostream &os) const override
    {
        os << "\n=== " << name() << " ===\n";
        os << "Problem: " << statement() << "\n";
        os << "Complexity: " << complexity() << "\n";
        // print res_ details...
        os << "\n";
    }

private:
    PartitionResult res_;
};
```

When you add a new solver, include its `.cpp` and `.h` in
`GraphPartitioning.pro` so the build picks it up.

## Implementations

All solvers implement the same interface and print a short report.

- `MinimumBisectionSolver`: heuristic balanced bisection using KL-style swaps.
- `KWayPartitionSolver`: recursive bisection heuristic for k-way partitioning.
- `MultilevelKWayPartitionSolver`: multilevel coarsen-partition-refine heuristic
  with heavy-edge matching and local refinement.
- `VertexSeparatorSolver`: derives a vertex separator from a bisection boundary.
- `GlobalMinCutSolver`: Stoer-Wagner global minimum cut (exact).
- `STMinCutSolver`: s-t minimum cut via Dinic max-flow (exact).

Each solver exposes a concise problem statement and complexity in its `print` output.

## Example usage

`main.cpp` contains a minimal example. A typical usage pattern:

```cpp
WeightedGraph g(8);
g.add_undirected(0, 1, 3);
g.add_undirected(1, 2, 2);
// ... add edges ...

MinimumBisectionSolver bis;
bis.solve(g);
bis.print(std::cout);

KWayPartitionSolver kway(3);
kway.solve(g);
kway.print(std::cout);

MultilevelKWayPartitionSolver mk(3);
mk.solve(g);
mk.print(std::cout);
```

## Build

This is a Qt `.pro` project configured for C++17.

```bash
qmake GraphPartitioning.pro
make
```

On Windows with MSVC, replace `make` with the appropriate build tool
(for example `nmake`).

## Extending

1) Add a new solver class inheriting `IGraphPartitionSolver`.
2) Implement all interface methods.
3) Update `GraphPartitioning.pro` to include the new `.cpp` and `.h`.
4) Optionally add a usage demo in `main.cpp`.
