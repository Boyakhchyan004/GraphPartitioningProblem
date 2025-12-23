#include "VertexSeparatorSolver.h"
#include "MinimumBisectionSolver.h"

VertexSeparatorSolver::VertexSeparatorSolver(int bisection_passes)
    : passes_(bisection_passes)
{}

std::string VertexSeparatorSolver::name() const
{
    return "Balanced Vertex Separator (Heuristic from bisection boundary)";
}

std::string VertexSeparatorSolver::statement() const
{
    return "Input: undirected graph G=(V,E,w).\n"
           "Goal: find a vertex separator S subset of V and two nonempty sets A,B subset of "
           "V\\S such that:\n"
           "  - A and B are disjoint\n"
           "  - there is NO edge between A and B (removing S disconnects A from B)\n"
           "  - balance: A and B should both be large (often max(|A|,|B|) <= alpha*|V| with "
           "alpha<1, e.g. 2/3)\n"
           "Objective: minimize |S| (or weight(S)).\n"
           "Output: separator[] stores S; part[] is the 2-way labeling used to derive S.";
}

std::string VertexSeparatorSolver::complexity() const
{
    return "Optimization is NP-hard. This heuristic: bisection heuristic + boundary scan, "
           "~O(p*n^2 + m).";
}

void VertexSeparatorSolver::solve(const WeightedGraph &g)
{
    res_ = {};
    if (g.n == 0)
        return;

    MinimumBisectionSolver bis(passes_);
    bis.solve(g);
    auto p = bis.result().part;

    std::vector<char> isSep(g.n, 0);
    for (int u = 0; u < g.n; ++u) {
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (p[u] != p[v]) {
                isSep[u] = 1;
                isSep[v] = 1;
            }
        }
    }

    res_.part = std::move(p);
    for (int i = 0; i < g.n; ++i)
        if (isSep[i])
            res_.separator.push_back(i);
    res_.cut_weight = bis.result().cut_weight;
    res_.score = (double) res_.separator.size();
}

PartitionResult VertexSeparatorSolver::result() const
{
    return res_;
}

void VertexSeparatorSolver::print(std::ostream &os) const
{
    os << "\n=== " << name() << " ===\n";
    os << "Problem: " << statement() << "\n";
    os << "Complexity: " << complexity() << "\n";
    if (!res_.part.empty()) {
        int a = 0, b = 0;
        for (int v : res_.part)
            (v == 0 ? a : b)++;
        os << "Result: |A|=" << a << " |B|=" << b << " |S|=" << res_.separator.size()
           << " boundary-cut=" << res_.cut_weight << "\n";
    }
    os << "\n";
}
