#pragma once
#include "GraphUtils.h"
class IGraphPartitionSolver
{
public:
    virtual ~IGraphPartitionSolver() = default;
    virtual std::string name() const = 0;
    virtual std::string statement() const = 0;
    virtual std::string complexity() const = 0;
    virtual void solve(const WeightedGraph &g) = 0;
    virtual PartitionResult result() const = 0;
    virtual void print(std::ostream &os) const = 0;
};
