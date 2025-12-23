#include "STMinCutSolver.h"

STMinCutSolver::STMinCutSolver(int s, int t)
    : s_(s)
    , t_(t)
{}

std::string STMinCutSolver::name() const
{
    return "s-t Minimum Cut (Dinic max-flow)";
}

std::string STMinCutSolver::statement() const
{
    return "Input: undirected weighted graph G=(V,E,w) and two distinct terminals s and t.\n"
           "Goal: find a partition V = S union T with S and T disjoint, s in S, t in T.\n"
           "Objective: minimize cut(S,T) = sum of w(u,v) over edges {u,v} with u in S and v in "
           "T.\n"
           "Equivalent: min s-t cut value equals max s->t flow value (max-flow/min-cut).\n"
           "Output: part[v]=0 means v is on the s-side (S), part[v]=1 means on the t-side (T).";
}

std::string STMinCutSolver::complexity() const
{
    return "Polynomial. Dinic: O(E*V^2) worst-case; often much faster in practice on sparse "
           "graphs.";
}

void STMinCutSolver::solve(const WeightedGraph &g)
{
    res_ = {};
    int n = g.n;
    if (n == 0)
        return;
    if (s_ < 0 || t_ < 0 || s_ >= n || t_ >= n || s_ == t_)
        throw std::invalid_argument("bad s,t");

    struct Dinic
    {
        struct E
        {
            int to;
            int rev;
            Weight cap;
        };
        int n;
        std::vector<std::vector<E>> g;
        std::vector<int> lvl, it;

        explicit Dinic(int n_)
            : n(n_)
            , g(n_)
            , lvl(n_)
            , it(n_)
        {}

        void add_edge(int u, int v, Weight c)
        {
            E a{v, (int) g[v].size(), c};
            E b{u, (int) g[u].size(), 0};
            g[u].push_back(a);
            g[v].push_back(b);
        }

        bool bfs(int s, int t)
        {
            std::fill(lvl.begin(), lvl.end(), -1);
            std::queue<int> q;
            lvl[s] = 0;
            q.push(s);
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (auto &e : g[u])
                    if (e.cap > 0 && lvl[e.to] < 0) {
                        lvl[e.to] = lvl[u] + 1;
                        q.push(e.to);
                    }
            }
            return lvl[t] >= 0;
        }

        Weight dfs(int u, int t, Weight f)
        {
            if (u == t)
                return f;
            for (int &i = it[u]; i < (int) g[u].size(); ++i) {
                E &e = g[u][i];
                if (e.cap <= 0 || lvl[e.to] != lvl[u] + 1)
                    continue;
                Weight pushed = dfs(e.to, t, std::min(f, e.cap));
                if (pushed > 0) {
                    e.cap -= pushed;
                    g[e.to][e.rev].cap += pushed;
                    return pushed;
                }
            }
            return 0;
        }

        Weight maxflow(int s, int t)
        {
            Weight flow = 0;
            while (bfs(s, t)) {
                std::fill(it.begin(), it.end(), 0);
                while (true) {
                    Weight pushed = dfs(s, t, std::numeric_limits<Weight>::max() / 4);
                    if (pushed == 0)
                        break;
                    flow += pushed;
                }
            }
            return flow;
        }

        std::vector<char> reachable_from(int s)
        {
            std::vector<char> vis(n, 0);
            std::queue<int> q;
            vis[s] = 1;
            q.push(s);
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (auto &e : g[u])
                    if (e.cap > 0 && !vis[e.to]) {
                        vis[e.to] = 1;
                        q.push(e.to);
                    }
            }
            return vis;
        }
    };

    Dinic din(n);
    for (int u = 0; u < n; ++u) {
        for (auto &e : g.adj[u]) {
            din.add_edge(u, e.to, e.w);
        }
    }

    Weight flow = din.maxflow(s_, t_);
    auto reach = din.reachable_from(s_);

    res_.part.assign(n, 0);
    for (int i = 0; i < n; ++i)
        res_.part[i] = reach[i] ? 0 : 1;
    res_.cut_weight = flow;
}

PartitionResult STMinCutSolver::result() const
{
    return res_;
}

void STMinCutSolver::print(std::ostream &os) const
{
    os << "\n=== " << name() << " ===\n";
    os << "Problem: " << statement() << "\n";
    os << "Complexity: " << complexity() << "\n";
    if (!res_.part.empty()) {
        int a = 0, b = 0;
        for (int v : res_.part)
            (v == 0 ? a : b)++;
        os << "Result: s=" << s_ << " t=" << t_ << " |S-side|=" << a << " |T-side|=" << b
           << " mincut=" << res_.cut_weight << "\n";
    }
    os << "\n";
}
