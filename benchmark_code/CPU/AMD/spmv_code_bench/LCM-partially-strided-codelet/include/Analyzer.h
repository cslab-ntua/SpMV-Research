//
// Created by cetinicz on 2021-08-07.
//

#ifndef DDT_ANALYZER_H
#define DDT_ANALYZER_H

#include "DDT.h"
#include "DDTDef.h"

namespace DDT {

    template <typename... Args>
    void cleanPointerVectors(Args... args) {
        std::vector<std::vector<DDT::Codelet*>> vc = { args... };
        for (auto& vCodelets : vc) {
            for (auto& vCodelet : vCodelets) {
                delete vCodelet;
                vCodelet = nullptr;
            }
        }
    }
//    template<>
//    void cleanPointerVectors(std::vector<Codelet*>& c) {
//        for (auto & i : c) {
//            delete i;
//            i = nullptr;
//        }
//    }
    void analyzeData(const DDT::GlobalObject& d,  std::vector<DDT::Codelet*>** cll, const DDT::Config& config);
    void analyzeData(const DDT::GlobalObject& d, const std::vector<DDT::Codelet*>* cll, const DDT::Config& config);
    void analyzeCodeletExection(const DDT::GlobalObject& d, const std::vector<DDT::Codelet*>* cll, const DDT::Config& config);
}
#endif//DDT_ANALYZER_H
