//
// Created by kazem on 7/12/21.
//

#ifndef DDT_INSPECTOR_H
#define DDT_INSPECTOR_H

#include "DDT.h"
#include "DDTDef.h"
#include "DDTUtils.h"
#include "DDTCodelets.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include <cassert>

//#define ENABLE_COST_MODEL

namespace DDT {
    inline bool hasAdacentIteration(int i, int** ip);

    void generateCodeletsFromParallelDag(const sparse_avx::Trace* tr, std::vector<DDT::Codelet*>& cc, const DDT::Config& cfg);

    void generateFullRowCodeletType(int i, int** ip, int ips, DDT::PatternDAG* c, DDT::PatternDAG* cc, std::vector<Codelet*>& cl);

    bool determineFSCProfit(int nRows, int dim0, int dim1);

    template <DDT::CodeletType Type>
    void generateCodeletType(DDT::GlobalObject& d, DDT::PatternDAG* c, std::vector<Codelet*>& cl) {


        // Get codelet type
        if constexpr (Type == DDT::TYPE_PSC3) {
            int colWidth = ((c->ct - c->pt) / TPR) + 1;

            while (c->ct != c->pt) {
                d.o[--d.onz] = c->ct[2];
                c->ct -= TPR;
            }
            d.o[--d.onz] = c->ct[2];

            int oo = c->ct[0];
            int mo = c->ct[1];

            auto cornerT = c->ct+(colWidth-1)*TPR;
            cl.emplace_back(new DDT::PSCT3V1(oo, colWidth, mo,d.o+d.onz,cornerT[0] == cornerT[2]));
        }

        if constexpr (Type == DDT::TYPE_FSC) {
            int rowCnt = 1;

            // Loop array induction variable coefficients
            int mi = c->ct[1] - c->pt[1];
            int vi = c->ct[2] - c->pt[2];

            int mj = c->ct[4] - c->ct[1];
            int vj = c->ct[5] - c->ct[2];

            while (c->pt != c->ct) {
                int nc = (c->pt - d.mt.ip[0])/TPR;
                c->ct = nullptr;
                c = d.c + nc;
                rowCnt++;
            }

            // if (vj != 1) { std::cout << c->ct[1] << std::endl; std::cout << vj << std::endl; assert(vj == 1); }

            // Loop Array Offsets
            int oo = c->ct[0];
            int mo = c->ct[1];
            int vo = c->ct[2];

            // Generate codelet
            cl.emplace_back(new DDT::FSCCodelet(oo,vo,rowCnt, c->sz+1,mo,mi,vi));
        }

        if constexpr (Type == DDT::TYPE_PSC1) {
            int rowCnt = 1;

            int vi = c->ct[2] - c->pt[2];
            int vj = c->ct[5] - c->ct[2];
            while (c->pt != c->ct) {
                d.o[--d.onz] = c->ct[1];
                int nc = (c->pt - d.mt.ip[0])/TPR;
                c->ct = nullptr;
                c = d.c + nc;
                rowCnt++;
            }
            d.o[--d.onz] = c->ct[1];
            assert(vi == 0);
            assert(vj == 1);

            // Loop Array Offsets
            int oo = c->ct[0];
            int vo = c->ct[2];

            // Generate codelet
            cl.push_back(new DDT::PSCT1V1(oo,vo,rowCnt,c->sz+1,vi,d.o+d.onz));
        }


        if constexpr (Type == DDT::TYPE_PSC2) {
            int rowCnt = 1;
            int REUSE_DIM = 2;

            // Loop array induction variable coefficients
            int mi = c->ct[1] - c->pt[1];
            int vi = c->ct[2] - c->pt[2];

            int mj = c->ct[4] - c->pt[1];

            while (c->pt != c->ct) {
                assert((c->ct[1] - c->pt[1]) == mi);
                int nc = (c->pt - d.mt.ip[0])/TPR;
                c->ct = nullptr;
                c = d.c + nc;
                rowCnt++;
            }

            // Loop Array Offsets
            int oo = c->ct[0];
            int mo = c->ct[1];

            // Loop Index offsets
            for (int i = c->sz; i >= 0; --i) {
                d.o[--d.onz] = c[i].ct[2];
            }

#ifdef ENABLE_COST_MODEL
            // Generate offset array into vector
            int nc = (c->pt - d.mt.ip[0])/TPR;
            int prevDim = d.c[nc].ct[REUSE_DIM] - 1;
            int cs = 0;

            int rowPartition[2] = { 0, 0 };
            int lastLocation = 0;
            for (int i = 0; i < c->sz+1; ++i) {
                if ((d.c[nc+i].ct[REUSE_DIM] - prevDim) == 1) {
                    cs++;
                } else {
                    if (rowPartition[0] && cs) {
                        rowPartition[1] = cs;
                        auto isFSCProfitable = determineFSCProfit(rowCnt,rowPartition[0],rowPartition[1]);
                        if (isFSCProfitable) {
                            // Generate PSC
                            int pscWidth = i-(rowPartition[0]+rowPartition[1]+lastLocation);
                            if (pscWidth != 0) {
                                cl.emplace_back(new DDT::PSCT2V1(oo, rowCnt, pscWidth, mo+lastLocation, mi, vi, d.o+d.onz+lastLocation));
                            }

                            // Generate two FSCs
                            cl.emplace_back(new DDT::FSCCodelet(oo,d.c[nc+lastLocation+pscWidth].ct[2],rowCnt, rowPartition[0],mo+lastLocation+pscWidth,mi,vi));
                            cl.emplace_back(new DDT::FSCCodelet(oo,d.c[nc+lastLocation+pscWidth+rowPartition[0]].ct[2],rowCnt, rowPartition[1],mo+lastLocation+pscWidth+rowPartition[0],mi,vi));

                            // Reset values
                            rowPartition[0] = 0, rowPartition[1] = 0, cs = 0;
                            lastLocation = i;
                        } else {
                            rowPartition[0] = rowPartition[1];
                            rowPartition[1] = 0;
                            cs = 1;
                        }
                    } else {
                        (rowPartition[0] ? rowPartition[1] : rowPartition[0]) = cs;
                        cs = 1;
                    }
                }
                prevDim = d.c[nc+i].ct[REUSE_DIM];
            }
            rowPartition[1] = cs;
            if (rowPartition[0] && rowPartition[1]) {
                auto isFSCProfitable = determineFSCProfit(rowCnt,rowPartition[0],rowPartition[1]);
                if (isFSCProfitable) {
                    // Generate PSC

                    int pscWidth = (c->sz+1)-(rowPartition[0]+rowPartition[1]+lastLocation);
                    if (pscWidth != 0) {
                        cl.emplace_back(new DDT::PSCT2V1(oo, rowCnt, (c->sz+1)-(rowPartition[0]+rowPartition[1]+lastLocation), mo+lastLocation, mi, vi, d.o+d.onz+lastLocation));
                    }
                    // Generate two FSCs
                    cl.emplace_back(new DDT::FSCCodelet(oo,d.c[nc+lastLocation+pscWidth].ct[2],rowCnt, rowPartition[0],mo+lastLocation+pscWidth,mi,vi));
                    cl.emplace_back(new DDT::FSCCodelet(oo,d.c[nc+lastLocation+pscWidth+rowPartition[0]].ct[2],rowCnt, rowPartition[1],mo+lastLocation+pscWidth+rowPartition[0],mi,vi));
                } else {
                    // Make PSC type codelet
                    cl.emplace_back(new DDT::PSCT2V1(oo, rowCnt, (c->sz+1)-lastLocation, mo+lastLocation, mi, vi, d.o+d.onz+lastLocation));
                }
            } else {
                // Only one codelet

                int pscWidth = (c->sz+1)-lastLocation;
                if (pscWidth) {
                    cl.emplace_back(new DDT::PSCT2V1(oo, rowCnt, pscWidth, mo+lastLocation, mi, vi, d.o+d.onz+lastLocation));
                } else {
                    cl.emplace_back(new DDT::FSCCodelet(oo,d.c[nc+rowPartition[0]].ct[2],rowCnt, rowPartition[0],mo+lastLocation,mi,vi));
                }
            }
#else
            // Generate codelet
            cl.emplace_back(new DDT::PSCT2V1(oo, rowCnt, c->sz+1, mo, mi, vi, d.o+d.onz));
#endif

        }
        c->ct = nullptr;
    }


    inline int nnzInIteration(int i, int** ip);

    void generateCodeletsFromSerialDAG(DDT::GlobalObject& d, std::vector<Codelet*>* cl, const DDT::Config& cfg);

    void inspectSerialTrace(DDT::GlobalObject& d, std::vector<Codelet*>* cl, const DDT::Config& cfg);

    void inspectParallelTrace(DDT::GlobalObject& d, const DDT::Config& cfg);

        void free(std::vector<DDT::Codelet*>& cl);
}


#endif //DDT_INSPECTOR_H
