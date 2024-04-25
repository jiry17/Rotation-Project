#include "istool/sygus/parser/parser.h"
#include "istool/basic/config.h"
#include <string>
#include <iostream>
#include "glog/logging.h"

// Construct all programs with size 'size' expanded from grammar rule 'rule'.
ProgramList constructPrograms(Rule* rule, int size, const std::vector<std::vector<ProgramList>>& program_storage);

/**
 * Enumerate programs in the grammar from small to large, and return the first program satisfying all examples
 * @param grammar a context-free grammar specifying the program space
 * @param examples a set of input-output examples
 * @param env an environment for running programs on a given input
 * @return a program in grammar satisfying all given example.
 */
PProgram synthesisFromExample(Grammar* grammar, const IOExampleList& examples, Env* env) {
    // Assign consecutive indices to non-terminals in the grammar.
    grammar->indexSymbol();

    // A verifier checking whether a program satisfying all examples
    auto verifier = [&](Program* candidate_program) -> bool {
        for (auto& [inp, oup]: examples) {
            if (!(env->run(candidate_program, inp) == oup)) return false;
        }
        return true;
    };

    // A storage for all enumerated programs
    // program_storage[i][s] stores the set of programs expanded from the i-th non-terminal whose size is s.
    std::vector<std::vector<ProgramList>> program_storage(grammar->symbol_list.size(), {{}});

    // TODO: (Task 1.2) Integrate observational equivalence to the enumeration below.
    for (int size = 1;; ++size) {
        LOG(INFO) << "Current size " << size;
        for (auto* symbol: grammar->symbol_list) {
            program_storage[symbol->id].emplace_back();
            for (auto* rule: symbol->rule_list) {
                for (auto& candidate_program: constructPrograms(rule, size, program_storage)) {
                    program_storage[symbol->id][size].push_back(candidate_program);
                    if (symbol->id == grammar->start->id && verifier(candidate_program.get())) {
                        return candidate_program;
                    }
                }
            }
        }
    }
}

PProgram cegis(Grammar* grammar, const IOExampleList& examples, Env* env) {
    // TODO: (Task 1.1) Implement counter-example guided inductive synthesis
    return synthesisFromExample(grammar, examples, env);
}

int main(int argv, char** argc) {
    std::string task_path;
    if (argv == 2) {
        task_path = std::string(argc[1]);
    } else task_path = config::KSourcePath + "/tests/plus.sl";

    auto spec = parser::getSyGuSSpecFromFile(task_path);
    auto* example_space = dynamic_cast<FiniteIOExampleSpace*>(spec->example_space.get());
    IOExampleList io_examples;
    for (auto& example: example_space->example_space) {
        io_examples.push_back(example_space->getIOExample(example));
    }
    auto* grammar = spec->info_list[0]->grammar;

    auto* recorder = new TimeRecorder();
    recorder->start("synthesis");
    auto res = cegis(grammar, io_examples, spec->env.get());
    recorder->end("synthesis");

    std::cout << "Result: " << res->toString() << std::endl;
    std::cout << "Time Cost: " << recorder->query("synthesis") << " seconds";
}



// The following are auxiliary functions used in enumeration
namespace {
    void _getAllSizeScheme(int pos, int rem, const std::vector<std::vector<int>>& pool, std::vector<int>& tmp, std::vector<std::vector<int>>& res);

    // Get all possible schemes to distribute @size to all parameters.
    // @pool[i] is the list of possible sizes of the i-th parameter.
    std::vector<std::vector<int>> getAllSizeScheme(int size, const std::vector<std::vector<int>>& pool) {
        std::vector<int> tmp;
        std::vector<std::vector<int>> res;
        _getAllSizeScheme(0, size, pool, tmp, res);
        return res;
    }

    void _getAllSizeScheme(int pos, int rem, const std::vector<std::vector<int>>& pool, std::vector<int>& tmp, std::vector<std::vector<int>>& res) {
        if (pos == pool.size()) {
            if (rem == 0) res.push_back(tmp); return;
        }
        for (auto size: pool[pos]) {
            if (size > rem) continue;
            tmp.push_back(size);
            _getAllSizeScheme(pos + 1, rem - size, pool, tmp, res);
            tmp.pop_back();
        }
    }

    // Construct programs expanded from rule @rule from a pool of sub-programs
    void buildAllCombination(int pos, const std::vector<ProgramList>& sub_programs, Rule* rule, ProgramList& sub_list, ProgramList& res) {
        if (pos == sub_programs.size()) {
            res.push_back(rule->buildProgram(sub_list)); return;
        }
        for (auto& sub_program: sub_programs[pos]) {
            sub_list.push_back(sub_program);
            buildAllCombination(pos + 1, sub_programs, rule, sub_list, res);
            sub_list.pop_back();
        }
    }
}

ProgramList constructPrograms(Rule* rule, int size, const std::vector<std::vector<ProgramList>>& program_storage) {
    std::vector<std::vector<int>> size_pool;
    for (auto* nt: rule->param_list) {
        std::vector<int> size_list;
        for (int i = 0; i < size; ++i) {
            if (!program_storage[nt->id][i].empty()) size_list.push_back(i);
        }
        size_pool.push_back(size_list);
    }
    ProgramList res;
    for (const auto& scheme: getAllSizeScheme(size - 1, size_pool)) {
        ProgramStorage sub_programs;
        for (int i = 0; i < scheme.size(); ++i) {
            sub_programs.push_back(program_storage[rule->param_list[i]->id][scheme[i]]);
        }
        ProgramList sub_list;
        buildAllCombination(0, sub_programs, rule, sub_list, res);
    }
    return res;
}