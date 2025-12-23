#include "MinimumBisectionSolver.h"

MinimumBisectionSolver::MinimumBisectionSolver(int max_passes)
    : max_passes_(max_passes)
{}

std::string MinimumBisectionSolver::name() const
{
    return "Minimum Bisection (Heuristic KL-style swaps)";
}

std::string MinimumBisectionSolver::statement() const
{
    return "Input: undirected weighted graph G=(V,E,w) with w(e) >= 0.\n"
           "Goal: split vertex set into two blocks A and B such that:\n"
           "  - A and B are disjoint and A U B = V\n"
           "  - balance: abs(|A| - |B|) <= 1\n"
           "Objective: minimize cut(A,B) = sum of w(u,v) over edges {u,v} with u in A and v in "
           "B.\n"
           "Output: part[v]=0 means v in A, part[v]=1 means v in B.";
}

std::string MinimumBisectionSolver::complexity() const
{
    return "Optimization is NP-hard. This heuristic is typically O(p*n^2 + p*m) where "
           "p=passes.";
}

void MinimumBisectionSolver::solve(const WeightedGraph &g)
{
    if (g.n == 0) {
        res_ = {};
        return;
    }
    std::vector<int> all(g.n);
    std::iota(all.begin(), all.end(), 0);
    res_.part = bisection_on_subset(g, all, max_passes_);
    res_.cut_weight = cut_weight_undirected(g, res_.part);
}

PartitionResult MinimumBisectionSolver::result() const
{
    return res_;
}

void MinimumBisectionSolver::print(std::ostream &os) const
{
    os << "\n=== " << name() << " ===\n";
    os << "Problem: " << statement() << "\n";
    os << "Complexity: " << complexity() << "\n";
    if (!res_.part.empty()) {
        int a = 0, b = 0;
        for (int v : res_.part)
            (v == 0 ? a : b)++;
        os << "Result: |A|=" << a << " |B|=" << b << " cut=" << res_.cut_weight << "\n";
    }
    os << "\n";
}

std::vector<int> MinimumBisectionSolver::bisection_on_subset(const WeightedGraph &g,
                                                             const std::vector<int> &vertices,
                                                             int max_passes)
{
    int n = g.n;
    std::vector<bool> in(n, 0);
    for (int v : vertices)
        in[v] = 1;
    int s = (int) vertices.size();
    int targetA = (s + 1) / 2;

    std::vector<int> part(n, -1);
    auto order = order_by_internal_degree(g, vertices);
    int cntA = 0;
    for (int v : order) {
        if (cntA < targetA) {
            part[v] = 0;
            cntA++;
        } else {
            part[v] = 1;
        }
    }

    auto compute_D = [&](const std::vector<int> &p) {
        std::vector<Weight> D(n, 0);
        for (int u = 0; u < n; ++u) {
            if (!in[u])
                continue;
            Weight internal = 0, external = 0;
            for (auto &e : g.adj[u]) {
                if (!in[e.to])
                    continue;
                if (p[u] == p[e.to])
                    internal += e.w;
                else
                    external += e.w;
            }
            D[u] = external - internal;
        }
        return D;
    };

    for (int pass = 0; pass < max_passes; ++pass) {
        bool improved = false;
        auto D = compute_D(part);

        Weight best_gain = 0;
        int best_u = -1, best_v = -1;

        for (int u : vertices)
            if (part[u] == 0) {
                for (int v : vertices)
                    if (part[v] == 1) {
                        Weight wuv = 0;
                        for (auto &e : g.adj[u])
                            if (e.to == v) {
                                wuv = e.w;
                                break;
                            }
                        Weight gain = D[u] + D[v] - 2 * wuv;
                        if (gain > best_gain) {
                            best_gain = gain;
                            best_u = u;
                            best_v = v;
                        }
                    }
            }

        if (best_gain > 0 && best_u != -1) {
            std::swap(part[best_u], part[best_v]);
            improved = true;
        }

        if (!improved)
            break;
    }

    for (int u = 0; u < n; ++u)
        if (!in[u])
            part[u] = 0;
    return part;
}
