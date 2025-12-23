#include "GlobalMinCutSolver.h"

std::string GlobalMinCutSolver::name() const
{
    return "Global Minimum Cut (Stoer-Wagner)";
}

std::string GlobalMinCutSolver::statement() const
{
    return "Input: undirected weighted graph G=(V,E,w).\n"
           "Goal: find a nontrivial cut (A,B) where A is nonempty and A != V, and B = V\\A.\n"
           "Objective: minimize cut(A,B) = sum of w(u,v) over edges {u,v} with u in A and v in "
           "B.\n"
           "Output: part[v] indicates which side of the minimum cut each vertex belongs to.";
}

std::string GlobalMinCutSolver::complexity() const
{
    return "Polynomial: O(n^3) time (dense form), O(nm + n^2 log n) variants exist.";
}

void GlobalMinCutSolver::solve(const WeightedGraph &g)
{
    res_ = {};
    if (g.n == 0)
        return;
    if (g.n == 1) {
        res_.part.assign(1, 0);
        res_.cut_weight = 0;
        return;
    }

    int n = g.n;
    std::vector<std::vector<Weight>> w(n, std::vector<Weight>(n, 0));
    for (int u = 0; u < n; ++u) {
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (u < v)
                w[u][v] += e.w, w[v][u] += e.w;
        }
    }

    std::vector<int> vtx(n);
    std::iota(vtx.begin(), vtx.end(), 0);
    std::vector<std::vector<int>> clusters(n);
    for (int i = 0; i < n; ++i)
        clusters[i] = {i};

    Weight best = std::numeric_limits<Weight>::max();
    std::vector<int> bestA;

    int curN = n;
    while (curN > 1) {
        std::vector<Weight> dist(curN, 0);
        std::vector<char> added(curN, 0);
        int prev = -1;
        int last = -1;

        for (int it = 0; it < curN; ++it) {
            int sel = -1;
            for (int i = 0; i < curN; ++i)
                if (!added[i]) {
                    if (sel == -1 || dist[i] > dist[sel])
                        sel = i;
                }
            added[sel] = 1;
            prev = last;
            last = sel;

            for (int i = 0; i < curN; ++i)
                if (!added[i]) {
                    dist[i] += w[vtx[sel]][vtx[i]];
                }
        }

        int s = prev;
        int t = last;
        Weight cut = dist[t];

        if (cut < best) {
            best = cut;
            bestA = clusters[vtx[t]];
        }

        int vs = vtx[s], vt = vtx[t];
        for (int i = 0; i < curN; ++i) {
            int vi = vtx[i];
            if (vi == vs || vi == vt)
                continue;
            w[vs][vi] += w[vt][vi];
            w[vi][vs] += w[vi][vt];
        }

        clusters[vs].insert(clusters[vs].end(), clusters[vt].begin(), clusters[vt].end());

        vtx.erase(vtx.begin() + t);
        curN--;
    }

    res_.part.assign(n, 1);
    for (int v : bestA)
        res_.part[v] = 0;
    res_.cut_weight = best;
}

PartitionResult GlobalMinCutSolver::result() const
{
    return res_;
}

void GlobalMinCutSolver::print(std::ostream &os) const
{
    os << "\n=== " << name() << " ===\n";
    os << "Problem: " << statement() << "\n";
    os << "Complexity: " << complexity() << "\n";
    if (!res_.part.empty()) {
        int a = 0, b = 0;
        for (int v : res_.part)
            (v == 0 ? a : b)++;
        os << "Result: |A|=" << a << " |B|=" << b << " mincut=" << res_.cut_weight << "\n";
    }
    os << "\n";
}
