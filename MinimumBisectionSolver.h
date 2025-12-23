#pragma once
#include "GraphPartitionSolver.h"

class MinimumBisectionSolver final : public IGraphPartitionSolver
{
public:
    explicit MinimumBisectionSolver(int max_passes = 20);
    std::string name() const override;
    std::string statement() const override;
    std::string complexity() const override;
    void solve(const WeightedGraph &g) override;
    PartitionResult result() const override;
    void print(std::ostream &os) const override;

    static std::vector<int> bisection_on_subset(const WeightedGraph &g,
                                                const std::vector<int> &vertices,
                                                int max_passes);

private:
    int max_passes_;
    PartitionResult res_;
};
