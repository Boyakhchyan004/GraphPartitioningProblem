#include "GlobalMinCutSolver.h"
#include "KWayPartitionSolver.h"
#include "MinimumBisectionSolver.h"
#include "MultilevelKWayPartitionSolver.h"
#include "STMinCutSolver.h"
#include "VertexSeparatorSolver.h"

int main()
{
    WeightedGraph g(8);
    g.add_undirected(0, 1, 3);
    g.add_undirected(1, 2, 2);
    g.add_undirected(2, 3, 4);
    g.add_undirected(3, 0, 1);
    g.add_undirected(4, 5, 3);
    g.add_undirected(5, 6, 2);
    g.add_undirected(6, 7, 4);
    g.add_undirected(7, 4, 1);
    g.add_undirected(3, 4, 1);
    g.add_undirected(2, 5, 1);

    MinimumBisectionSolver bis;
    bis.solve(g);
    bis.print(std::cout);

    KWayPartitionSolver kway(3);
    kway.solve(g);
    kway.print(std::cout);

    MultilevelKWayPartitionSolver kwaymulti(3);
    kwaymulti.solve(g);
    kwaymulti.print(std::cout);

    VertexSeparatorSolver sep;
    sep.solve(g);
    sep.print(std::cout);

    GlobalMinCutSolver gmin;
    gmin.solve(g);
    gmin.print(std::cout);

    STMinCutSolver st(0, 6);
    st.solve(g);
    st.print(std::cout);

    return 0;
}
