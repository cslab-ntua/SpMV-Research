/*
 * =====================================================================================
 *
 *       Filename:  Executor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021-07-13 09:31:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "DDT.h"
#include "Inspector.h"


#include <vector>


#ifndef DDT_EXECUTOR
#define DDT_EXECUTOR

namespace DDT {

    void executeParallelSPTRSVCodelets(const DDT::GlobalObject& d, const DDT::Config& cfg, int r, const int* Lp, const int* Li, const double* Lx, double* x);

    void executeParallelCodelets(const DDT::GlobalObject& d, const DDT::Config&
    cfg, Args& args);

void executeCodelets(const std::vector<DDT::Codelet*>* cl, const DDT::Config& c);

    void executeCodelets(const std::vector<DDT::Codelet*>* cl, const DDT::Config&
    c, Args& args);

    void executeSPTRSVCodelets(const std::vector<DDT::Codelet*>* cl, const
    DDT::Config& cfg, const int r, const int* Lp, const int *Li, const double*Lx,
                                double* x, double* y);
}

#endif  // DDT_EXECUTOR
