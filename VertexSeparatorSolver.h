#include "GraphPartitionSolver.h"

class VertexSeparatorSolver final : public IGraphPartitionSolver
{
public:
    explicit VertexSeparatorSolver(int bisection_passes = 15);
    std::string name() const override;
    std::string statement() const override;
    std::string complexity() const override;
    void solve(const WeightedGraph &g) override;
    PartitionResult result() const override;
    void print(std::ostream &os) const override;

private:
    int passes_;
    PartitionResult res_;
};
