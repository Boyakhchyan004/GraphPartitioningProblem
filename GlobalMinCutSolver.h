#pragma once
#include "GraphPartitionSolver.h"

class GlobalMinCutSolver final : public IGraphPartitionSolver
{
public:
    std::string name() const override;
    std::string statement() const override;
    std::string complexity() const override;
    void solve(const WeightedGraph &g) override;
    PartitionResult result() const override;
    void print(std::ostream &os) const override;

private:
    PartitionResult res_;
};
