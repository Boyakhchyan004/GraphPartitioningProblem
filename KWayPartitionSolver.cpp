#include "KWayPartitionSolver.h"
#include "MinimumBisectionSolver.h"

KWayPartitionSolver::KWayPartitionSolver(int k, int bisection_passes)
    : k_(k)
    , passes_(bisection_passes)
{}

std::string KWayPartitionSolver::name() const
{
    return "k-Way Balanced Partition (Recursive bisection heuristic)";
}

std::string KWayPartitionSolver::statement() const
{
    return "Input: undirected weighted graph G=(V,E,w) and integer k >= 2.\n"
           "Goal: assign each vertex a label part[v] in {0..k-1} defining k disjoint blocks "
           "V0..Vk-1:\n"
           "  - blocks are disjoint and their union is V\n"
           "  - balance (typical): block sizes are as equal as possible\n"
           "Objective: minimize total inter-block cut weight:\n"
           "  Cut_k = sum of w(u,v) over edges {u,v} with part[u] != part[v].";
}

std::string KWayPartitionSolver::complexity() const
{
    return "Optimization is NP-hard. Recursive bisection heuristic: ~O((k-1)*p*n^2) on splits "
           "(varies by split sizes).";
}

void KWayPartitionSolver::solve(const WeightedGraph &g)
{
    res_ = {};
    if (g.n == 0)
        return;
    int k = std::max(1, k_);
    res_.part.assign(g.n, 0);
    if (k == 1) {
        res_.cut_weight = 0;
        return;
    }

    std::vector<std::vector<int>> parts;
    parts.push_back(std::vector<int>(g.n));
    std::iota(parts[0].begin(), parts[0].end(), 0);

    while ((int) parts.size() < k) {
        int idx = 0;
        for (int i = 1; i < (int) parts.size(); ++i)
            if (parts[i].size() > parts[idx].size())
                idx = i;

        if (parts[idx].size() <= 1)
            break;

        auto subset = parts[idx];
        auto bi = MinimumBisectionSolver::bisection_on_subset(g, subset, passes_);

        std::vector<int> A, B;
        A.reserve(subset.size());
        B.reserve(subset.size());
        for (int v : subset)
            (bi[v] == 0 ? A : B).push_back(v);

        if (A.empty() || B.empty())
            break;

        parts[idx] = std::move(A);
        parts.push_back(std::move(B));
    }

    for (int i = 0; i < (int) parts.size(); ++i) {
        for (int v : parts[i])
            res_.part[v] = i;
    }

    res_.cut_weight = cut_weight_undirected(g, res_.part);
}

PartitionResult KWayPartitionSolver::result() const
{
    return res_;
}

void KWayPartitionSolver::print(std::ostream &os) const
{
    os << "\n=== " << name() << " ===\n";
    os << "Problem: " << statement() << "\n";
    os << "Complexity: " << complexity() << "\n";
    if (!res_.part.empty()) {
        std::vector<int> sizes;
        int maxp = *std::max_element(res_.part.begin(), res_.part.end());
        sizes.assign(maxp + 1, 0);
        for (int p : res_.part)
            sizes[p]++;
        os << "Result: k=" << sizes.size() << " cut=" << res_.cut_weight << " sizes=[";
        for (size_t i = 0; i < sizes.size(); ++i) {
            if (i)
                os << ",";
            os << sizes[i];
        }
        os << "]\n";
    }
    os << "\n";
}
