from mosek.fusion import *
import sys

remove_2_hop_loops = False
remove_selfloops = True

def TSP(A_i, A_j, C_v):
    n = max(max(A_i), max(A_j)) + 1
    A = Matrix.sparse(n, n, A_i, A_j, 1)
    C = Matrix.sparse(n, n, A_i, A_j, C_v)
    with Model() as M:
        M.setLogHandler(sys.stdout)
        x = M.variable([n, n], Domain.binary())
        M.constraint(Expr.sum(x, 0), Domain.equalsTo(1.0))
        M.constraint(Expr.sum(x, 1), Domain.equalsTo(1.0))
        M.constraint(x.diag(), Domain.equalsTo(0.))
        M.constraint(x, Domain.lessThan(A))
        M.objective(ObjectiveSense.Minimize, Expr.dot(C, x))
        #M.solve()
        if remove_2_hop_loops:
            M.constraint(Expr.add(x, x.transpose()), Domain.lessThan(1.0))

        if remove_selfloops:
            M.constraint(x.diag(), Domain.equalsTo(0.))

        it = 1
        M.writeTask("tsp-0-%s-%s.ptf" % ('t' if remove_selfloops else 'f', 't' if remove_2_hop_loops else 'f'))

        while True:
            print("\n\n--------------------\nIteration", it)
            M.solve()

            print('\nsolution cost:', M.primalObjValue())
            print('\nsolution:')

            cycles = []

            for i in range(n):
                xi = x.slice([i, 0], [i + 1, n])
                print(xi.level())

                for j in range(n):
                    if xi.level()[j] <= 0.5: continue

                    found = False
                    for c in cycles:
                        if len([a for a in c if i in a or j in a]) > 0:
                            c.append([i, j])
                            found = True
                            break

                    if not found:
                        cycles.append([[i, j]])

            print('\ncycles:')
            print([c for c in cycles])

            if len(cycles) == 1:
                break;

            for c in cycles:
                M.constraint(Expr.sum(x.pick(c)), Domain.lessThan(1.0 * len(c) - 1))
            it = it + 1

        print(x.level())
        return x.level(), n


def psc_mining(A_i, A_j, C_v):
    n = max(max(A_i), max(A_j)) + 1
    A = Matrix.sparse(n, n, A_i, A_j, 1)
    C = Matrix.sparse(n, n, A_i, A_j, C_v)
    with Model() as M:
        M.setLogHandler(sys.stdout)
        x = M.variable([n, n], Domain.binary())
        M.constraint(Expr.sum(Expr.add(x, x.transpose()),1), Domain.greaterThan(3))
        #M.constraint(Expr.add(x, x.transpose()), Domain.lessThan(1.0))
        #M.constraint(Expr.sum(x, 0), Domain.equalsTo(1.0))
        #M.constraint(Expr.sum(x, 1), Domain.equalsTo(1.0))
        M.constraint(x.diag(), Domain.equalsTo(0.))
        M.constraint(x, Domain.lessThan(A))
        M.objective(ObjectiveSense.Minimize, Expr.dot(C, x))
        M.solve()
        print(x.level())
        return x.level(), n




def main():
    n = 4
    A_i = [0, 1, 2, 3, 1, 0, 2, 0]
    A_j = [1, 2, 3, 0, 0, 2, 1, 3]
    C_v = [1., 1., 1., 1., 0.1, 0.1, 0.1, 0.1]
    n = max(max(A_i), max(A_j)) + 1
    A = Matrix.sparse(n, n, A_i, A_j, 1.0)
    C = Matrix.sparse(n, n, A_i, A_j, C_v)
    with Model() as M:
        M.setLogHandler(sys.stdout)
        x = M.variable([n, n], Domain.binary())
        M.constraint(Expr.sum(Expr.add(x, x.transpose()),0), Domain.greaterThan(1.0))
        #M.constraint(Expr.sum(x, 0), Domain.equalsTo(1.0))
        #M.constraint(Expr.sum(x, 1), Domain.equalsTo(1.0))
        M.constraint(x, Domain.lessThan(A))
        M.objective(ObjectiveSense.Minimize, Expr.dot(C, x))
        M.solve()
        print(x.level())
        #print('\nsolution cost:', M.primalObjValue())


if __name__ == "__main__":
    main()