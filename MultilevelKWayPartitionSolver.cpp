#include "MultilevelKWayPartitionSolver.h"
#include "KWayPartitionSolver.h"
#include <unordered_map>

namespace {
static WeightedGraph coarsen_graph(const WeightedGraph &g, std::vector<int> &fine_to_coarse)
{
    int n = g.n;
    fine_to_coarse.assign(n, -1);
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    auto deg = g.degrees();
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        if (deg[a] != deg[b])
            return deg[a] > deg[b];
        return a < b;
    });

    std::vector<char> matched(n, 0);
    int coarse_n = 0;
    for (int u : order) {
        if (matched[u])
            continue;
        matched[u] = 1;
        int best = -1;
        Weight best_w = -1;
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (matched[v])
                continue;
            if (e.w > best_w) {
                best_w = e.w;
                best = v;
            }
        }
        if (best != -1) {
            matched[best] = 1;
            int id = coarse_n++;
            fine_to_coarse[u] = id;
            fine_to_coarse[best] = id;
        } else {
            int id = coarse_n++;
            fine_to_coarse[u] = id;
        }
    }

    WeightedGraph coarse(coarse_n);
    std::vector<std::unordered_map<int, Weight>> adj_map(coarse_n);
    for (int u = 0; u < n; ++u) {
        int cu = fine_to_coarse[u];
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (u < v) {
                int cv = fine_to_coarse[v];
                if (cu == cv)
                    continue;
                adj_map[cu][cv] += e.w;
                adj_map[cv][cu] += e.w;
            }
        }
    }
    for (int u = 0; u < coarse_n; ++u) {
        for (auto &p : adj_map[u]) {
            int v = p.first;
            if (u < v)
                coarse.add_undirected(u, v, p.second);
        }
    }
    return coarse;
}

static void refine_partition(const WeightedGraph &g,
                             std::vector<int> &part,
                             int k,
                             int max_passes)
{
    if (k <= 1)
        return;
    int n = g.n;
    int min_size = n / k;
    int max_size = (n + k - 1) / k;
    std::vector<Weight> weights(k, 0);
    for (int pass = 0; pass < max_passes; ++pass) {
        std::vector<int> sizes(k, 0);
        for (int p : part)
            if (p >= 0 && p < k)
                sizes[p]++;

        bool moved = false;
        for (int u = 0; u < n; ++u) {
            int p = part[u];
            if (p < 0 || p >= k)
                continue;
            if (sizes[p] <= min_size)
                continue;

            std::fill(weights.begin(), weights.end(), 0);
            for (auto &e : g.adj[u]) {
                int q = part[e.to];
                if (q >= 0 && q < k)
                    weights[q] += e.w;
            }
            Weight w_p = weights[p];
            int best = p;
            Weight best_gain = 0;
            for (int q = 0; q < k; ++q) {
                if (q == p)
                    continue;
                if (sizes[q] >= max_size)
                    continue;
                Weight gain = weights[q] - w_p;
                if (gain > best_gain) {
                    best_gain = gain;
                    best = q;
                }
            }
            if (best != p && best_gain > 0) {
                part[u] = best;
                sizes[p]--;
                sizes[best]++;
                moved = true;
            }
        }
        if (!moved)
            break;
    }
}
} // namespace

MultilevelKWayPartitionSolver::MultilevelKWayPartitionSolver(int k,
                                                             int bisection_passes,
                                                             int refine_passes,
                                                             int max_levels)
    : k_(k)
    , bisection_passes_(bisection_passes)
    , refine_passes_(refine_passes)
    , max_levels_(max_levels)
{}

std::string MultilevelKWayPartitionSolver::name() const
{
    return "k-Way Balanced Partition (Multilevel coarsen-refine heuristic)";
}

std::string MultilevelKWayPartitionSolver::statement() const
{
    return "Input: undirected weighted graph G=(V,E,w) and integer k >= 2.\n"
           "Goal: assign each vertex a label part[v] in {0..k-1} defining k disjoint blocks "
           "V0..Vk-1:\n"
           "  - blocks are disjoint and their union is V\n"
           "  - balance (typical): block sizes are as equal as possible\n"
           "Objective: minimize total inter-block cut weight:\n"
           "  Cut_k = sum of w(u,v) over edges {u,v} with part[u] != part[v].";
}

std::string MultilevelKWayPartitionSolver::complexity() const
{
    return "Optimization is NP-hard. Multilevel heuristic: O(L*m) coarsening + coarse "
           "partitioning + O(L*(m + n*k)) refinement (varies by level).";
}

void MultilevelKWayPartitionSolver::solve(const WeightedGraph &g)
{
    res_ = {};
    if (g.n == 0)
        return;
    int k = std::max(1, std::min(k_, g.n));
    res_.part.assign(g.n, 0);
    if (k == 1) {
        res_.cut_weight = 0;
        return;
    }

    std::vector<WeightedGraph> graphs;
    graphs.push_back(g);
    std::vector<std::vector<int>> maps;
    int min_coarse = std::max(2 * k, 20);
    for (int level = 0; level < max_levels_ && graphs.back().n > min_coarse; ++level) {
        std::vector<int> map;
        WeightedGraph coarse = coarsen_graph(graphs.back(), map);
        if (coarse.n >= graphs.back().n)
            break;
        maps.push_back(std::move(map));
        graphs.push_back(std::move(coarse));
    }

    KWayPartitionSolver base(k, bisection_passes_);
    base.solve(graphs.back());
    std::vector<int> part = base.result().part;

    for (int level = (int) graphs.size() - 2; level >= 0; --level) {
        const auto &map = maps[level];
        std::vector<int> fine_part(graphs[level].n, 0);
        for (int u = 0; u < graphs[level].n; ++u)
            fine_part[u] = part[map[u]];
        part = std::move(fine_part);
        refine_partition(graphs[level], part, k, refine_passes_);
    }

    res_.part = std::move(part);
    res_.cut_weight = cut_weight_undirected(g, res_.part);
}

PartitionResult MultilevelKWayPartitionSolver::result() const
{
    return res_;
}

void MultilevelKWayPartitionSolver::print(std::ostream &os) const
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
