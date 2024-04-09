//
// Created by Kazem on 7/12/21.
//

#include "DDT.h"
#include "DDTDef.h"
#include "DDTUtils.h"
#include "Inspector.h"
#include "PatternMatching.h"

#include <algorithm>
#include <DDTCodelets.h>


namespace DDT {

    void FSCCodelet::print() {
      std::cout<<"FSC: y["<<lbr<<":"<<lbr+row_width<<"] = Ax["<<first_nnz_loc<<","
      <<row_offset<<"]*x["<<lbc<<":"<<lbc+col_width<<"]\n";
    }

    void PSCT1V1::print() {
     std::cout<<"T1: y["<<lbr<<":"<<lbr+row_width<<"] = Ax["<<offsets[0]<<","
              <<offsets[row_width]<<"]*x["<<lbc<<":"<<lbc+col_width<<"]\n";
    }

    void PSCT2V1::print() {
      std::cout<<"T2: y["<<lbr<<":"<<lbr+row_width<<"] = Ax["<<first_nnz_loc<<","
              <<first_nnz_loc+row_offset<<"]*x["<<offsets[0]<<":"<<offsets
              [col_width]<<"]\n";
    }

    void PSCT3V1::print() {
     std::cout<<"T3: y["<<lbr<<"] = Ax["<<first_nnz_loc<<","
              <<first_nnz_loc+col_width<<"]*x["<<offsets[0]<<":"<<offsets
              [col_width]<<"]\n";
    }

 void PSCT3V2::print() {
  std::cout<<"NOT IMPLEMENTED\n";
 }

 void PSCT3V3::print() {
  std::cout<<"NOT IMPLEMENTED\n";
 }

    void generateFullRowCodeletType(int i, int** ip, int ips, DDT::PatternDAG* c, DDT::PatternDAG* cc, std::vector<Codelet*>& cl) {
        auto type = cc->t;
        // Get codelet type
        if (type == DDT::TYPE_PSC3) {
            int colWidth = ((cc->pt - cc->ct) / TPR) + 1;

            int oo = cc->ct[0];
            int mo = cc->ct[1];
            auto cornerT = cc->ct+(colWidth-1)*TPR;
            assert(cornerT[0] == cornerT[2]);

            int cnt = 0;
            auto o = new int[colWidth]();
            while (cc->ct != cc->pt) {
                o[cnt++] = cc->ct[2];
                cc->ct += TPR;
            }
            o[cnt] = cc->ct[2];


            cl.emplace_back(new DDT::PSCT3V1(oo, colWidth, mo,o,true));
        }

        if (type == DDT::TYPE_PSC3_V1) {
            int rowCnt = 1;
            auto ccc = cc;
            while (ccc->ct != ccc->pt) {
                int nc = (ccc->pt - ip[0])/TPR;
                ccc = c + nc;
                rowCnt++;
            }
            auto ro = new int[rowCnt]();
            auto rls = new int[rowCnt]();

            assert((ip[i+1] - ip[i]) / TPR > 4);

            int cnt = rowCnt-1;
            while (cc->ct != cc->pt) {
                ro[cnt] = cc->ct[1];
                rls[cnt--] = (ip[i+1] - ip[i])/TPR;
                int nc = (cc->pt - ip[0])/TPR;
                cc = c + nc;
                --i;
            }
            ro[cnt] = cc->ct[1];
            rls[cnt] = (ip[i+1] - ip[i])/TPR;

            int oo = cc->ct[0];
            int vo = cc->ct[2];

            cl.emplace_back(new DDT::PSCT3V2(oo, vo, rowCnt, cc->sz, 0, ro, rls));
        }

        if (type == DDT::TYPE_PSC3_V2) {
            int rowCnt = 1;
            auto ccc = cc;
            while (ccc->ct != ccc->pt) {
                int nc = (ccc->pt - ip[0])/TPR;
                ccc = c + nc;
                rowCnt++;
            }
            auto ro = new int[rowCnt]();
            auto rls = new int[rowCnt]();
            auto rid = new int[rowCnt]();

            int cnt = rowCnt-1;
            while (cc->ct != cc->pt) {
                rid[cnt] = cc->ct[0];
                ro[cnt] = cc->ct[1];
                rls[cnt--] = (ip[i+1] - ip[i])/TPR;
                int nc = (cc->pt - ip[0])/TPR;
                cc = c + nc;
                --i;
            }
            rid[cnt] = cc->ct[0];
            ro[cnt] = cc->ct[1];
            rls[cnt] = (ip[i+1] - ip[i])/TPR;

            int oo = cc->ct[0];
            int vo = cc->ct[2];

            cl.emplace_back(new DDT::PSCT3V3(oo, vo, rowCnt, cc->sz, 0, ro, rls, rid));
        }
    }


    /**
     * @brief Determines if two FSC codelets are better executors than one PSC
     *
     * @description Determines if two FSC codelets which have number of rows equal
     * to nRows and number of columns dim0 and dim1, are more performant
     * than grouping all the columns into one PSC type codelet.
     *
     * @param nRows
     * @param dim0
     * @param dim1
     *
     * @return True if it is profitable to split PSC codelet into FSC codelet
     */
    bool determineFSCProfit(int nRows, int dim0, int dim1) {
        return (dim0 > 8 && dim1 > 8) && nRows > 3;
    }

 /**
  * @brief Generates run-time codelet object based on type in DDT::PatternDAG
  *
  * @param d  Global DDT object containing pattern information
  * @param c  Codelet to turn into run-time object
  * @param cl List of runtime codelet descriptions
  */
  void generateCodelet(DDT::GlobalObject& d, DDT::PatternDAG* c, std::vector<Codelet*>& cl) {
    switch (c->t) {
      case DDT::TYPE_FSC:
        generateCodeletType<DDT::TYPE_FSC>(d,c,cl);
        break;
      case DDT::TYPE_PSC1:
        generateCodeletType<DDT::TYPE_PSC1>(d,c,cl);
        break;
      case DDT::TYPE_PSC2:
        generateCodeletType<DDT::TYPE_PSC2>(d,c,cl);
        break;
      case DDT::TYPE_PSC3:
        generateCodeletType<DDT::TYPE_PSC3>(d,c,cl);
        break;
      case DDT::TYPE_PSC3_V1:
        generateCodeletType<DDT::TYPE_PSC3_V1>(d,c,cl);
        break;
      case DDT::TYPE_PSC3_V2:
        generateCodeletType<DDT::TYPE_PSC3_V2>(d,c,cl);
        break;
      default:
        break;
    }
  }

  inline int nnzLessThan(int ii, int** ip, int ub) {
      auto tr = (ip[ii+1]-ip[ii])/TPR;
      for (int i = 0; i < tr; i++) {
          auto ro = ip[ii][i*3+2];
          if (ub <= ro) {
              return i;
          }
      }
      return ip[ii+1]-ip[ii];
  }

  /**
   * @brief Finds bounds for PSC3v3
   * @param ip
   * @param c
   * @param i
   * @return
   */
  int findType3VBounds(int** ip, DDT::PatternDAG* c, int i) {
      int cnl = ((ip[i])-ip[0]) / TPR;
      int cnlt = cnl;
      int VW = 4; // Vector width

      if (0 == i) {
          int cn = ((ip[i + 1]) - ip[0]) / TPR;
          c[cnl].t = DDT::TYPE_PSC3;
          c[cnl].pt = c[cn-1].ct;
          return i;
      }


      int NNZ_P_R = 5;
      int ii = i;
      bool hai = hasAdacentIteration(ii, ip);
      auto TH = nnzLessThan(ii, ip,ip[ii][-1]);
      TH = TH - (TH % VW);

      if (hai && TH) {
          while (ii > 0 &&
                 nnzInIteration(ii-1, ip) > TH &&
                 hasAdacentIteration(ii, ip) &&
                 nnzLessThan(ii, ip, ip[ii][-1]) > TH
                 /* && nCodelets == 0 */ ) {
              ii--;
          }
          if (i != ii) {
              int iii = i;
              ii += (i - (ii - 1)) % 2;
              while (iii-- != ii) {
                  int cn = ((ip[iii]) - ip[0]) / TPR;
                  c[cnl].pt = c[cn].ct;
                  cnl = cn;
              }
              assert((i-ii+1)%2 == 0);
          }
      } else if (TH) {
          while (ii > 0 &&
                 nnzInIteration(ii-1, ip) > TH  &&
                 nnzLessThan(ii, ip, ip[ii][-1]) >TH
                 /* && nCodelets == 0 */) {
              ii--;
          }
          if (i != ii) {
              int iii = i;
              ii += (i - (ii - 1)) % 2;
              while (iii-- != ii) {
                  int cn = ((ip[iii]) - ip[0]) / TPR;
                  c[cnl].pt = c[cn].ct;
                  cnl = cn;
              }
              assert((i-ii+1)%2 == 0);
          }
      }

      if (ii == i) {
          // TODO: FIX HERE FOR PREVIOUS TYPE
          int cn = ((ip[ii+1]) - ip[0]) / TPR;
          c[cnlt].t = DDT::TYPE_PSC3;
          c[cnl].pt = c[cn-1].ct;
      } else if (hai) {
          c[cnl].pt = c[cnl].ct;
          c[cnlt].t  = DDT::TYPE_PSC3_V1;
          c[cnl].sz = TH;
      } else {
          c[cnl].pt = c[cnl].ct;
          c[cnlt].t  = DDT::TYPE_PSC3_V2;
          c[cnl].sz = TH;
      }

      return ii;
  }

  /**
   * Finds the continuous bounds of a TYPE_PSC3
   * @param d
   * @param i
   * @param j
   * @param jBound
   * @return
   */
  int findType3Bounds(DDT::GlobalObject& d, int i, int j, int jBound) {
      int cn = ((d.mt.ip[i]+j)-d.mt.ip[0]) / TPR;
      auto pscb = d.c[cn].ct;
      int jStart = j;

      while (j < jBound && d.c[cn].ct != nullptr && d.c[cn].pt == nullptr) {
          j += TPR;
          cn = ((d.mt.ip[i]+j)-d.mt.ip[0]) / TPR;
      }
      cn = ((d.mt.ip[i]+(j-TPR))-d.mt.ip[0]) / TPR;
      d.c[cn].t = DDT::TYPE_PSC3;
      d.c[cn].pt = pscb;

      return j - jStart;
  }

  /**
   * @brief Calculates if two integer ranges overlap [x1:x2], [y1:y2]
   *
   * @param x1 Lower bound for first range
   * @param x2 Upper bound for first range
   * @param y1 Lower bound for second range
   * @param y2 Upper bound for second range
   *
   * @return True if ranges overlap
   */
    inline bool isOverlapping(int x1, int x2, int y1, int y2) {
        return std::max(x1, y1) <= std::min(x2, y2);
    }

    /**
     * @brief Determines if iteration under sparsity threshold
     * @param ip
     * @param i
     * @return
     */
    inline bool isIterationSparse(int** ip, int i) {
        return (ip[i+1]-ip[i]) < SP_ITER_THRESHOLD;
    }

    /**
     * @brief Calculates periodic patterns in iteration sizes
     *
     * @param ipbt Type associated with lower bound
     * @param ipb  Pointer containing bounds of iterations
     * @param ipbc Size of current pruned iteration bounds
     * @param m0   Pointer to memoized differentials
     * @param m1
     * @param ip
     * @param ips
     * @param i
     * @param lim
     *
     * @return    New iteration # if detected period is > 1
     */
     int findIterationDifferential(int* ipbt, int* ipb, int& ipbs, bool* m0, bool* m1, int** ip, int ips, int i, int lim) {
        assert(i == 0 ? ipbs == 1 : true);

        int nb = i;
        bool st = false;
        for (int j = 0; j < std::min(ips-i, lim); ++j) {
            m0[j] = (ip[i+2+j]-ip[i+1+j]) == (ip[i+1]-ip[i]);
            st = st || (m1[j] && m0[j]);
        }
        if (!st || i == ips-2) {
            // This makes sure we have a minimum # iterations to indicate a pattern
            if ((i-ipb[ipbs-1]) < MIN_LIM_ITERATIONS) {
                ipb[ipbs] = i;
                ipbt[ipbs-1] = -1;
                for (int j = 0; j < lim; ++j) { m1[j] = m0[j]; }
                ipbs++;
            } else {
                // Check how many iterations the pattern is offset
                for (int j = 0; j < lim; ++j) {
                    if (m1[j]) { ipbt[ipbs - 1] = j; }
                    m1[j] = m0[j];
                }
                // Make sure offset pattern contains number of iterations
                // divisible by the period
                // (ie. period of 3 should have iterations % 3 == 0)
                if (int off = (i+ipbt[ipbs - 1]-ipb[ipbs-1]) % (ipbt[ipbs-1]+1) != 0) {
                    ipb[ipbs++] = i - off;
                    for (int j = 0; j < lim; ++j) {
                        m1[j] = true;
                    }
                    ipb[ipbs++] = i;
                } else {
                    ipb[ipbs] = i + ipbt[ipbs - 1];
                    nb = ipb[ipbs++];
                }
            }
            ipbt[ipbs-1] = -1;
        } else {
            for (int j = 0; j < lim; ++j) { m1[j] = m1[j] && m0[j]; }
        }
        return nb;
    }

    /**
     * @brief Prunes iteration space into segments based on patterns
     *
     * @param ip  Pointer to start first tuple at iteration ip[i]
     * @param ips Size of ip
     * @param lim Calculate iteration differences with offset up to lim
     */
    void pruneIterations(DDT::GlobalObject& d, int** ip, int ips, int lim = 1) {
        bool m0[MAX_LIM] = { true }, m1[MAX_LIM] = { true };

        for (int i = 0; i < ips-1; i++) {
            // 1) Detect n-order differences
            auto nb = findIterationDifferential(d.ipbt, d.ipb, d.ipbs, m0, m1, ip, ips, i, lim);

            std::cout << (ip[i+1] - ip[i]) / TPR << ",";
            // 2) Detect sparse iterations
            if (isIterationSparse(ip,i)) {
                d.sp[i] = true;
            }

            // 3) Detect overlapping dimensions
            for (int k = 0; k < TPR; k++) {
                d.rd[k] = d.rd[k] || isOverlapping(ip[i][k],ip[i+1][-3+k], ip[i+1][k], ip[i+2][-3+k]);
            }

            while (nb > i && nb != ips - 1) {
                i++;
                if (isIterationSparse(ip,i)) {
                    d.sp[i] = true;
                }
                for (int k = 0; k < TPR; k++) {
                    d.rd[k] = d.rd[k] || isOverlapping(ip[i][k],ip[i+1][-3+k], ip[i+1][k], ip[i+2][-3+k]);
                }
            }
            if (i > 500) {
                exit(1);
            }
        }
        d.ipb[d.ipbs] = d.mt.ips;
        if (isIterationSparse(ip,ips-1)) {
            d.sp[ips-1] = true;
        }
    }

    inline bool hasAdacentIteration(int i, int** ip) {
        return (ip[i][0] - ip[i-1][0]) == 1;
    }

    inline int nnzInIteration(int i, int** ip) {
        return (ip[i+1] - ip[i]) / TPR;
    }

    void generateCodeletsFromParallelDag(const sparse_avx::Trace* tr, std::vector<DDT::Codelet*>& cc, const DDT::Config& cfg) {
                for (int i = tr->_ni-1; i >= 0; --i) {
                    for (int j = 0; j < tr->_iter_pt[i+1]-tr->_iter_pt[i]; ++j) {
                        int cn = ((tr->_iter_pt[i] + j) - tr->_iter_pt[0]) / TPR;
                        if (tr->_c[cn].pt != nullptr && tr->_c[cn].ct != nullptr) {
                            // Generate (TYPE_FSC | TYPE_PSC1 | TYPE_PSC2)
                            //  generateCodelet(d, d.c + cn, cc);
                            // j += (d.c[cn].sz + 1) * TPR;
                        } else if (tr->_c[cn].ct != nullptr) { // Generate (TYPE_PSC3)
                            if (/* nCodelets == 0*/ true) {
                                int iEnd = findType3VBounds(tr->_iter_pt, tr->_c, i);
                                cn = ((tr->_iter_pt[i]) - tr->_iter_pt[0]) / TPR;
                                generateFullRowCodeletType(i, tr->_iter_pt, tr->_ni, tr->_c, tr->_c + cn, cc);
                                i = iEnd;
                                break;
                            } else {
                                // j = findType3Bounds(d, i, j, jbound)
                            }
                        } else {
                            j += TPR * (tr->_c[cn].sz + 1);
                        }
                    }
                }
                std::sort(cc.begin(), cc.end(), [](DDT::Codelet* lhs, DDT::Codelet* rhs) {
                  return lhs->first_nnz_loc < rhs->first_nnz_loc;
                });
    }

    bool traceSkewedCodelets(int** ip, DDT::PatternDAG* p, DDT::PatternDAG** ts, int tsn) {
        for (int i = 0; i < tsn; ++i) {
            auto cn = DDT::getTupleIndexFromRM(ip, ts[i]->pt);
            assert(p[cn].ct == ts[i]->pt);
            if (isCodeletOrigin(p+cn)) {
                return false;
            }
            ts[i] = p+cn;
        }
        return true;
    }

    void generateCodeletsFromSkewedIteration(
            DDT::PatternDAG* p,
            int** ip,
            int lb,
            int ub,
            int type,
            std::vector<DDT::Codelet*>& cc
    ) {
        DDT::PatternDAG* ts[MAX_CODELETS_PER_ITERATION] = {0};
        int ps[MAX_LIM] = {0};
        int tsc = 0;

        // 1) Find skew locations
        for (int j = type; j >= 0; --j) {
            for (int k = 0; k < getIterationSize(ip, ub-j-1);) {
                auto cn = getTupleIndex(ip, ub-1, k);
                if (DDT::isInCodelet(p+cn)) {
                    ts[tsc] = p+cn;
                    tsc++;
                    k += p[cn].sz + 1;
                } else {
                    throw std::runtime_error("Error: Skewed codelet has holes");
                }
            }
            ps[type-j] = j == type ? tsc : tsc - ps[type-j-1];
        }

        auto pl = new int[tsc*3+(type+1)]();
        auto ch = pl + type+1;
        auto cs = ch+tsc;
        auto cl = cs+tsc;

        for (int i = 0; i < type+1; ++i) {
            pl[i] = ps[i];
        }

        // 2) Get column hops and sizes
        int cw = 0;
        for (int i = 0; i < tsc; ++i) {
            ch[i] = ts[i]->ct[2] - ts[i]->pt[2];
            cs[i] = getCodeletSize(ts[i]);
            cw += cs[i];
        }

        // 3) Get column locations and fnl
        int fcr = 2;
        while (traceSkewedCodelets(ip, p, ts, tsc)) fcr++;
        assert(fcr*(type+1) == (ub-lb));  // Fused codelet length

        for (int i = 0; i < tsc; ++i) {
            traverseBack(p, ip, ts[i]);
            cl[i] = ts[i]->ct[2];
        }
        int fnl = ts[0]->ct[1];

        // 3) Generate fused skewed codelets
        cc.emplace_back(new DDT::FUSED_PSCT2(type+1,fnl, cw, lb, ub, tsc, pl));
    }

    /**
     * @brief Scans DAG to form memory codelets at marked locations
     *
     * @param i
     * @param ip
     * @param ips
     * @param p
     * @param cc
     */
    void generateCodeletsFromIteration(
            DDT::GlobalObject& d,
            int& i,
            int** ip,
            int ips,
            DDT::PatternDAG* p,
            std::vector<DDT::Codelet*>& cc
            ) {
        for (int j = 0; j < ip[i + 1] - ip[i];) {
            int cn = ((ip[i] + j) - ip[0]) / TPR;
            if (p[cn].pt != nullptr && p[cn].ct != nullptr) {
                // Generate (TYPE_FSC | TYPE_PSC1 | TYPE_PSC2)
                generateCodelet(d, p + cn, cc);
                j += (p[cn].sz + 1) * TPR;
            } else if (p[cn].ct != nullptr) {
                // Generate (TYPE_PSC3)
                if (/* nCodelets == 0*/ false) {
                    int iEnd = findType3VBounds(ip, p, i);
                    cn = ((ip[i]) - ip[0]) / TPR;
                    generateFullRowCodeletType(i, ip, ips,
                                               p, p + cn, cc);
                    i = iEnd;
                    break;
                } else {
                    j += findType3Bounds(
                            d, i, j, ip[i + 1] - ip[i]);
                    cn = ((ip[i] + (j - TPR)) - ip[0]) /
                         TPR;
                    generateCodelet(d, p + cn, cc);
                }
            } else {
                j += TPR * (p[cn].sz + 1);
            }
        }
    }

    /**
     * @brief Scans a pruned pattern DAG for codelets
     *
     * @param d
     * @param cl
     * @param cfg
     */
    void scanPrunedPatternDAG(DDT::GlobalObject& d, std::vector<Codelet*>& cl, const DDT::Config& cfg) {
        for (int t = d.ipbs-1; t >= 0; --t) {
                auto type = d.ipbt[t];
                if (type < 0) {
                    for (int i = d.ipb[t+1]-1; i >= d.ipb[t]; --i) {
                        generateCodeletsFromIteration(d, i, d.mt.ip,
                                                      d.mt.ips, d.c,cl);
                    }
                } else {
                    generateCodeletsFromSkewedIteration(d.c, d.mt.ip, d.ipb[t], d.ipb[t+1], type, cl);
                }
        }
        std::sort(cl.begin(), cl.end(), [](DDT::Codelet* lhs, DDT::Codelet* rhs) {
          return lhs->first_nnz_loc < rhs->first_nnz_loc;
        });
    }

    /**
     * @brief Generates runtime information for executor codes
     *
     * @param d Object containing inspector information
     * @param cl Pointer to containers for codelets for each thread
     * @param cfg Configuration object for inspector/executor
     */
    void generateCodeletsFromSerialDAG(DDT::GlobalObject& d, std::vector<Codelet*>* cl, const DDT::Config& cfg) {
        for (int t = 0; t < cfg.nThread; t++) {
            auto& cc = cl[t];
            for (int i = d.tb[t+1]-1; i >= d.tb[t]; i--) {
                for (int j = 0; j < d.mt.ip[i + 1] - d.mt.ip[i];) {
                    int cn = ((d.mt.ip[i] + j) - d.mt.ip[0]) / TPR;
                    if (d.c[cn].pt != nullptr && d.c[cn].ct != nullptr) {
                        // Generate (TYPE_FSC | TYPE_PSC1 | TYPE_PSC2)
                        generateCodelet(d, d.c + cn, cc);
                        j += (d.c[cn].sz + 1) * TPR;
                    } else if (d.c[cn].ct != nullptr) {
                        // Generate (TYPE_PSC3)
                        j += findType3Bounds(d, i, j,
                                             d.mt.ip[i + 1] - d.mt.ip[i]);
                        cn = ((d.mt.ip[i] + (j - TPR)) - d.mt.ip[0]) / TPR;
                        generateCodelet(d, d.c + cn, cc);
                    } else {
                        j += TPR * (d.c[cn].sz + 1);
                    }
                }
            }
            std::sort(cc.begin(), cc.end(), [](DDT::Codelet* lhs, DDT::Codelet* rhs) {
              return lhs->first_nnz_loc < rhs->first_nnz_loc;
            });
        }
    }

    /**
     * @brief Inspects a trace partitioned into parallel segments
     *
     * @param d
     * @param cl
     * @param cfg
     */
    void inspectParallelTrace(DDT::GlobalObject& d, const DDT::Config& cfg) {
        for (int i = 0; i < d.sm->_final_level_no; i++) {
            for (int j = 0; j < d.sm->_wp_bounds[i]; ++j) {
                auto tr = d.t[i][j];
                auto& cc = d.sm->_cl[i][j];

                // Calculate overlap for each iteration
                // DDT::pruneIterations(d.t[i][j]->_iter_pt, d.t[i][j]->_ni);
#ifdef O3
                // Compute first order differences
                DDT::computeParallelizedFOD(d.t[i][j]->_iter_pt, d.t[i][j]->_ni, d.d, 1);

                // Mine trace for codelets
                DDT::mineDifferences(d.mt.ip, d.c, d.d, cfg.nThread, d.tb);
#endif
                // Generate codelets from pattern DAG
                DDT::generateCodeletsFromParallelDag(tr, cc, cfg);
            }
        }
    }

  /** 
   * @brief Generates the pattern DAG and creates run-time codelets
   *
   * @param d  Global DDT object containing pattern information
   * @param cl List of runtime codelet descriptions 
   */
  void inspectSerialTrace(DDT::GlobalObject& d, std::vector<Codelet*>* cl, const DDT::Config& cfg) {
      // Calculate overlap for each iteration
      // DDT::pruneIterations(d, d.mt.ip, d.mt.ips, cfg.lim);
      if (cfg.op == DDT::OP_SPMV || cfg.op == DDT::OP_SPMM) {
          // Compute first order differences
          DDT::computeThreadBoundParallelizedFOD(d.tb, d.mt.ip, d.mt.ips, d.d, cfg.nThread);

          // Mine trace for codelets
           DDT::mineDifferences(d.mt.ip, d.c, d.d, cfg.nThread, d.tb);
      }
      // Generate codelets from pattern DAG
      DDT::generateCodeletsFromSerialDAG(d, cl, cfg);
  }


 void free(std::vector<DDT::Codelet*>& cl){
  for (auto & i : cl) {
   delete i;
  }
 }
 void FUSED_PSCT2::print() {}
}
