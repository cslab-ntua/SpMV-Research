//
// Created by cetinicz on 2021-07-29.
//

#ifndef DDT_DDTUTILS_H
#define DDT_DDTUTILS_H

#include "DDTDef.h"
#include <chrono>

namespace DDT {
    /**
   * Determines if memory location is part of codelet
   *
   * @param c Codelet memory location associated with tuple
   * @return True if c->pt == nullptr
   */
    inline bool isInCodelet(DDT::PatternDAG *c) { return c->pt != nullptr; }

    /**
    * Determines if DDT::Codelet is origin for codelet
    *
    * @param c Memory location of codelet pointer
    * @return True if codelet pointer is start of codelet
    */
    inline bool isCodeletOrigin(DDT::PatternDAG *c) { return c->pt == c->ct; }


    /**
     * Copies the rhs pointers into lhs
     *
     * @param lhs
     * @param rhs
     */
    inline void clonePatternDag(DDT::PatternDAG* lhs, DDT::PatternDAG* rhs) {
        lhs->pt = rhs->pt;
        lhs->ct = rhs->ct;
        lhs->sz = rhs->sz;
        lhs->t = rhs->t;
    }

    /**
     * Gets number of tuples at iteration @param i
     *
     * @param ip Pointer to start of iterations
     * @param i  Iteration for calculation
     * @return   Size of iteration at @param i
     */
    inline const int getIterationSize(int** ip, int i) {
        return (ip[i+1]-ip[i]) / DDT::TPR;
    }

    /**
     * Finds the absolute jth tuple at iteration i
     * @param ip
     * @param i  The iteration to
     * @param j  The jth tuple in iteration @param i
     *
     * @return   Index of the jth tuple at iteration i
     */
    inline const int getTupleIndex(int** ip, int i, int j) {
        return ((ip[i]+j*TPR)-ip[0]) / TPR;
    }

    /**
     * Finds the index of the tuple associated with the
     * raw memory tuple location (rmtl)
     *
     * @param ip
     * @param rmtl
     *
     * @return Index of tuple
     */
    inline const int getTupleIndexFromRM(int** ip, int* rmtl) {
        return (rmtl - ip[0]) / TPR;
    }

    /**
     *
     * @param md
     * @param ip
     * @param p
     */
    inline void traverseBack(PatternDAG* md, int** ip, DDT::PatternDAG*& p) {
        p = md + getTupleIndexFromRM(ip, p->pt);
    }

    /**
     * Get size of codelet at memory location c
     * @param c Memory location of DAG entry for codelet
     *
     * @return width of codelet from memory location
     */
    inline int getCodeletSize(PatternDAG* c) {
        return c->sz+1;
    }

    /**
     * Gets time difference between two points
     *
     * @param t1 First point in time
     * @param t2 Second point in time
     *
     * @return Time (in seconds) t2-t1
     */
    inline double getTimeDifference(std::chrono::steady_clock::time_point t1,
                                    std::chrono::steady_clock::time_point t2) {
        return std::chrono::duration_cast<std::chrono::duration<double>>(t2 -
        t1)
        .count();
    }
}
#endif//DDT_DDTUTILS_H
