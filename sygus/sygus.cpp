//
// Created by pro on 2021/12/26.
//

#include "istool/sygus/sygus.h"
#include "glog/logging.h"

Verifier * sygus::getVerifier(Specification *spec) {
    auto* example_space = spec->example_space.get();
    auto* finite_example_space = dynamic_cast<FiniteExampleSpace*>(example_space);
    if (finite_example_space) {
        return new FiniteExampleVerifier(finite_example_space);
    }
    LOG(FATAL) << "Unsupported type of the example space";
}