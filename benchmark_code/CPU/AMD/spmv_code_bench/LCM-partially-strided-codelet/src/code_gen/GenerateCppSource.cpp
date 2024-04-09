/*
 * =====================================================================================
 *
 *       Filename:  GenerateCppSource.cpp
 *
 *    Description:  Generates cpp source code from codelets 
 *
 *        Version:  1.0
 *        Created:  2021-07-17 01:04:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Zachary Cetinic, 
 *   Organization:  University of Toronto
 *
 * =====================================================================================
 */

#include "DDT.h"
#include "DDTDef.h"
#include "GenerateCppSource.h"

#include <iostream>

namespace DDT {
	void generateSingleStatement(std::stringstream& ss, int* ct) {
		ss << "y[" << ct[0] << "] += Lx[" << ct[1] << "] * x[" << ct[2] << "];\n";
	}

	void generateCodelet(std::stringstream& ss, DDT::GlobalObject& d, DDT::PatternDAG* c) {
		// Get codelet type
		auto TYPE = c->t;

		if (TYPE == DDT::TYPE_PSC1) {
			int buf[40];
			int rowCnt = 0;

			while (c->pt != c->ct) {
				buf[rowCnt++] = c->ct[0];
				int nc = (c->pt - d.mt.ip[0])/TPR;
				c->ct = nullptr;
				c = d.c + nc;
			}
			int oo = c->ct[0];
			int mo = c->ct[1];
			int vo = c->ct[2];

			ss << "{\n";
			ss << "auto yy = y+" << oo << ";";
			ss << "auto mm = Lx+" << mo << ";";
			ss << "auto xx = x+" << vo << ";\n";
			ss << "int of[] = {";
			for (int i = 0; i < rowCnt; i++) {
				ss << buf[rowCnt-i] << ",";
			}
			ss << "};\n";

			// Generated Code
			ss << "for (int i = 0; i < " << rowCnt+1 << "; i++) {\n";
			ss << "\tfor (int j = 0; j < " << c->sz+1 << "; j++) {\n";
			ss << "\t\tyy[i] += mm[of[i]+j] * x[i*j];\n";
			ss << "\t}\n";
			ss << "}\n";
			ss << "}\n";
			c->ct = nullptr;
		} else if (TYPE == DDT::TYPE_PSC2) {
			int buf[40];
			int rowCnt = 0;

			int mi = c->ct[1] - c->pt[1];
			int vi = c->ct[2] - c->pt[2];

			while (c->pt != c->ct) {
				int nc = (c->pt - d.mt.ip[0])/TPR;
				c->ct = nullptr;
				c = d.c + nc;
				rowCnt++;
			}
			//        std::cout << "Codelet At: (" << c->ct[0] << "," << c->ct[1]<< "," << c->ct[2] << ")\n";

			int oo = c->ct[0];
			int mo = c->ct[1];
			int vo = c->ct[2];

			ss << "{\n";
			ss << "auto yy = y+" << oo << ";";
			ss << "auto mm = Lx+" << mo << ";";
			ss << "auto xx = x+" << vo << ";\n";
			ss << "int of[] = {";
			for (int i = 0; i < c->sz+1; i++) {
				ss << c[i].ct[2] << ",";
			}
			ss << "};\n";

			// Generated Code
			ss << "for (int i = 0; i < " << rowCnt+1 << "; i++) {\n";
			ss << "\tfor (int j = 0; j < " << c->sz+1 << "; j++) {\n";
			ss << "\t\tyy[i] += mm[i*" << mi << "+j] * x[i*"<< vi << "+of[j]];\n";
			ss << "\t}\n";
			ss << "}\n";
			ss << "}\n";

			c->ct = nullptr;
		} else if (TYPE == DDT::TYPE_FSC) {
			int rowCnt = 0;

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

			int oo = c->ct[0];
			int mo = c->ct[1];
			int vo = c->ct[2];

			ss << "{\n";
			ss << "auto yy = y+" << oo << ";";
			ss << "auto mm = Lx+" << mo << ";";
			ss << "auto xx = x+" << vo << ";\n";
			ss << "for (int i = 0; i < " << rowCnt+1 << "; i++) {\n";
			ss << "\tfor (int j = 0; j < " << c->sz+1 << "; j++) {\n";
			ss << "\t\tyy[i] += mm[i*" << mi << "+j*" << mj << "] * xx[i*" << vi << "+j*"<<vj<<"];\n";
			ss << "\t}\n";
			ss << "}\n";
			ss << "}\n";

			c->ct = nullptr;
		}
	}

	void generateSource(DDT::GlobalObject& d) {
		int TPR = 3;

		std::stringstream ss;

		ss << "void f0(double* y, double* Lx, double* x) {\n";

		// Iterate through codelets
		for (int i = d.mt.ips-1; i >= 0; i--) {
			for (int j = 0; j < d.mt.ip[i+1]-d.mt.ip[i];) {
				int cn = ((d.mt.ip[i]+j)-d.mt.ip[0]) / TPR;
				if (d.c[cn].pt != nullptr && d.c[cn].ct != nullptr) {
					generateCodelet(ss, d, d.c+cn);
					j += (d.c[cn].sz+1) * TPR;
				} else if (d.c[cn].ct != nullptr) {
					// Regular codelet
					generateSingleStatement(ss, d.c[cn].ct);
					j += TPR;
				} else {
					j += TPR * (d.c[cn].sz + 1);
				}
			}
		}
		ss << "}\n";

		std::ofstream file("output.cpp");
		file << ss.str();
		file.close();
	}
}
