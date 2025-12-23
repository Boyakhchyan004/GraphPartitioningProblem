#pragma once
#include "GraphPartitionSolver.h"

class MultilevelKWayPartitionSolver final : public IGraphPartitionSolver
{
public:
    explicit MultilevelKWayPartitionSolver(int k,
                                           int bisection_passes = 8,
                                           int refine_passes = 4,
                                           int max_levels = 10);

    std::string name() const override;
    std::string statement() const override;
    std::string complexity() const override;

    void solve(const WeightedGraph &g) override;

    PartitionResult result() const override;

    void print(std::ostream &os) const override;

private:
    int k_;
    int bisection_passes_;
    int refine_passes_;
    int max_levels_;
    PartitionResult res_;
};
