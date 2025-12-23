#pragma once
#include "GraphPartitionSolver.h"

class STMinCutSolver final : public IGraphPartitionSolver
{
public:
    STMinCutSolver(int s, int t);
    std::string name() const override;
    std::string statement() const override;
    std::string complexity() const override;
    void solve(const WeightedGraph &g) override;
    PartitionResult result() const override;
    void print(std::ostream &os) const override;

private:
    int s_, t_;
    PartitionResult res_;
};
