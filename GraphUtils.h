#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using Weight = long long;

struct WeightedGraph
{
    struct Edge
    {
        int to{};
        Weight w{};
    };

    int n{};
    std::vector<std::vector<Edge>> adj;

    explicit WeightedGraph(int n_ = 0)
        : n(n_)
        , adj(n_)
    {}

    void add_undirected(int u, int v, Weight w)
    {
        if (u < 0 || v < 0 || u >= n || v >= n)
            throw std::out_of_range("vertex");
        if (w < 0)
            throw std::invalid_argument("weight must be nonnegative");
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    std::vector<Weight> degrees() const
    {
        std::vector<Weight> deg(n, 0);
        for (int u = 0; u < n; ++u) {
            Weight s = 0;
            for (auto &e : adj[u])
                s += e.w;
            deg[u] = s;
        }
        return deg;
    }
};

struct PartitionResult
{
    std::vector<int> part;
    std::vector<int> separator;
    Weight cut_weight = 0;
    double score = 0.0;
};
namespace {

static Weight cut_weight_undirected(const WeightedGraph &g, const std::vector<int> &part)
{
    Weight sum = 0;
    for (int u = 0; u < g.n; ++u) {
        for (auto &e : g.adj[u]) {
            int v = e.to;
            if (u < v && part[u] != part[v])
                sum += e.w;
        }
    }
    return sum;
}

static std::vector<int> order_by_internal_degree(const WeightedGraph &g,
                                                 const std::vector<int> &vertices)
{
    std::vector<bool> in(g.n, 0);
    for (int v : vertices)
        in[v] = 1;
    std::vector<std::pair<Weight, int>> dv;
    dv.reserve(vertices.size());
    for (int v : vertices) {
        Weight d = 0;
        for (auto &e : g.adj[v])
            if (in[e.to])
                d += e.w;
        dv.push_back({d, v});
    }
    std::sort(dv.begin(), dv.end(), [](auto &a, auto &b) {
        if (a.first != b.first)
            return a.first > b.first;
        return a.second < b.second;
    });
    std::vector<int> out;
    out.reserve(vertices.size());
    for (auto &p : dv)
        out.push_back(p.second);
    return out;
}
} // namespace
