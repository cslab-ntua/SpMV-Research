//
// Created by cetinicz on 2021-07-21.
//

#ifndef DDT_DDTCODELETS_H
#define DDT_DDTCODELETS_H

#include "DDTDef.h"
namespace DDT {
    struct Codelet {
        int size;
        int *offsets;
        int *offsets2, *offset3;
        bool is_alloc;
        int lbr, lbc, row_width, col_width, col_offset; //LBR, RW, CW
        int first_nnz_loc, row_offset; //FNL, RO
        bool multi_stmt; //FIXME: we should remove this, each codelet has only one
        // statement (operation)
        Codelet(int br, int bc, int rw, int cw, int fnl, int ro, int co, int *offs) : lbr(br), row_width(rw),
                                                                                      lbc(bc),col_width(cw),first_nnz_loc(fnl),row_offset(ro), col_offset(co),
                                                                                      offsets(offs), multi_stmt(false){}

        Codelet(int br, int bc, int rw, int cw, int fnl, int ro, int co, int *offs,
                bool ms
        ) : lbr(br), row_width(rw),lbc(bc),col_width(cw),first_nnz_loc(fnl),
            row_offset(ro), col_offset(co),offsets(offs), multi_stmt(ms){}

        // Mostly for Type 3
        Codelet(int br, int bc, int rw, int cw, int fnl, int ro, int co, int *offs,
                int *offs2, int *offs3) : lbr(br), row_width(rw),
                                          lbc(bc),col_width(cw),first_nnz_loc(fnl),row_offset(ro), col_offset(co),
                                          offsets(offs), offsets2(offs2), offset3(offs3), multi_stmt(false){}

        Codelet(int br, int bc, int rw, int cw, int fnl, int ro, int co, int *offs,
                int *offs2, int *offs3, bool ms) : lbr(br), row_width(rw),lbc(bc),
                                                   col_width(cw),first_nnz_loc(fnl), row_offset(ro), col_offset(co),
                                                   offsets(offs), offsets2(offs2), offset3(offs3),multi_stmt(ms){}


        virtual ~Codelet()= default;

        virtual CodeletType get_type()=0;
        virtual void print()=0;

    };

    struct FUSED_PSCT2 : public Codelet {
        FUSED_PSCT2(int ro, int fnl, int cw, int lb, int ub, int co, int* offsets) :
                                    Codelet(lb,0,ub-lb,cw,fnl,ro,co,offsets){};
        CodeletType get_type() override{return CodeletType::TYPE_F_PSC; }
        void print()override;
    };

    struct FSCCodelet:public Codelet{
        /**
         * y[lbr:lbr+row_width] = Ax[FNL:FNL+CW, ..., FNL+RO:FNL+RO+CW]*x[lbc:lbc+CW];
         */
        FSCCodelet(int br, int bc, int rw, int cw, int fnl, int ro, int co) : Codelet(br,bc,
                                                                                      rw,cw,fnl,ro,co,NULL){};

        FSCCodelet(int br, int bc, int rw, int cw, int fnl, int ro, int co, bool
        ms) : Codelet(br,bc,rw,cw,fnl,ro,co,NULL,ms){};
        CodeletType get_type() override{return CodeletType::TYPE_FSC;}
        void print()override;
    };


    struct PSCT1V1 : public Codelet{
        /**
         * y[lbr:lbr+row_width] = Ax[RO[lbr]:RO[lbr]+CW, ...,
         * RO[lbr]:RO+CW]*x[lbc:lbc+CW];
         */
        //int lbr, lbc, row_width, col_width; //LBR, RW, CW
        //int *row_offsets; //RO
        PSCT1V1(int br, int bc, int rw, int cw, int co, int *ros): Codelet(br,bc,rw,cw,-1,
                                                                           -1,co,ros){};

        PSCT1V1(int br, int bc, int rw, int cw, int co, int *ros, bool ms): Codelet
                                                                                    (br,bc,rw,cw,-1,-1,co,ros, ms){};

        CodeletType get_type() override{return CodeletType::TYPE_PSC1;}
        void print()override;

        ~PSCT1V1() {
            delete[] offsets;
        }
    };


    struct PSCT2V1:public Codelet{
        /**
         * y[lbr:lbr+row_width] = Ax[FNL:FNL+CW, ..., FNL+RO:FNL+RO+CW]
         * * x[CIO[0]:CIO[CW]];
         */
        //int lbr, row_width, col_width; //LBR, RW, CW
        //int first_nnz_loc, row_offset; //FNL, RO
        //int *coli_offset;// CIO

        PSCT2V1(int br, int rw, int cw, int fnl, int ro, int co, int *cio): Codelet(br, -1,
                                                                                    rw, cw,
                                                                                    fnl,
                                                                                    ro, co, cio){};

        PSCT2V1(int br, int rw, int cw, int fnl, int ro, int co, int *cio, bool ms):
                Codelet(br, -1, rw, cw, fnl, ro, co, cio, ms){};

        CodeletType get_type() override{return CodeletType::TYPE_PSC2;}
        void print()override;

        ~PSCT2V1() {
            delete[] offsets;
        }
    };

    struct PSCT3V1:public Codelet{
        /**
         * y[RS] = Ax[FNL:FNL+NN] * x[CIO[0],CIO[NN]];
         */
//  int row_start, num_nnz; //RS, NN
//  int first_nnz_loc; //FNL
//  int *coli_offset;// CIO

        PSCT3V1(int rs, int nn, int fnl, int *cio ): Codelet(rs, -1, 1, nn, fnl,
                                                             -1, 1, cio){};

        PSCT3V1(int rs, int nn, int fnl, int *cio, bool ms ): Codelet(rs, -1, 1, nn,
                                                                      fnl,-1, 1, cio, ms){};

        CodeletType get_type() override{return CodeletType::TYPE_PSC3;}
        void print()override;

        ~PSCT3V1() override {
            delete[] offsets;
        }
    };

    struct PSCT3V2:public Codelet{
        /** similar to T1 with variable iteration space
         * y[lbr:lbr+row_width] = Ax[RO[lbr]:RO[lbr]+RL[0], ...,
         * RO[lbr]:RO+RL[rw]*x[RO[lbr]:RO[lbr]+RL[0], ...,];
         */

        //int lbr, lbc, row_width, col_width; //LBR, RW, CW
        //int *row_offsets; //RO

        PSCT3V2(int br, int bc, int rw, int cw, int co, int *ros, int *rls): Codelet
                                                                                     (br,-1,rw,cw,ros[0],-1,co,ros,rls,NULL){};

        PSCT3V2(int br, int bc, int rw, int cw, int co, int *ros, int *rls, bool ms):
                Codelet(br,-1,rw,cw,-1,-1,co,ros,rls,NULL,ms){};

        CodeletType get_type() override{return CodeletType::TYPE_PSC3_V1;}
        void print()override;
    };


    struct PSCT3V3:public Codelet{
        /** similar to T1 with variable iteration space
         * y[LBR[0]: LBR[row_width]] = Ax[RO[lbr]:RO[lbr]+RL[0], ...,
         * RO[lbr]:RO+RL[rw]*x[RO[lbr]:RO[lbr]+RL[0], ...,];
         */

        //int lbr, lbc, row_width, col_width; //LBR, RW, CW
        //int *row_offsets; //RO

        PSCT3V3(int br, int bc, int rw, int cw, int co, int *ros, int *rls, int
        *lbr): Codelet(br,-1,rw,cw,ros[0],-1,co,ros,rls,lbr){};

        PSCT3V3(int br, int bc, int rw, int cw, int co, int *ros, int *rls, int *lbr,
                bool ms):Codelet(br,-1,rw,cw,-1,-1,co,ros,rls,lbr,ms){};

        CodeletType get_type() override{return CodeletType::TYPE_PSC3_V2;}
        void print()override;
    };
}

#endif//DDT_DDTCODELETS_H
