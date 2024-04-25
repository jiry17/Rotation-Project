#include "istool/sygus/parser/parser.h"
#include "istool/basic/config.h"
#include "istool/ext/z3/z3_verifier.h"
#include <string>
#include <iostream>
#include "glog/logging.h"
#include "istool/sygus/theory/basic/clia/clia.h"

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

PProgram cegis(Grammar* grammar, Verifier* v, IOExampleSpace* example_space, Env* env) {
    IOExampleList counter_examples; Example counter_example;
    FunctionContext res; res[example_space->func_name] = nullptr;
    while (1) {
        auto candidate = synthesisFromExample(grammar, counter_examples, env);
        res[example_space->func_name] = candidate;
        if (v->verify(res, &counter_example)) return candidate;
        auto new_example = example_space->getIOExample(counter_example);
        LOG(INFO) << "New example: " << example::ioExample2String(new_example);
        counter_examples.push_back(new_example);
    }
}

// An auxiliary function for generating random examples
void testGenRandomExamples(Z3ExampleSpace* example_space) {
    auto gen = [](Type* type) {
        if (dynamic_cast<TBool*>(type)) {
            return BuildData(Bool, rand() & 1);
        }
        if (dynamic_cast<TInt*>(type)) {
            return BuildData(Int, rand() % 11 - 5);
        }
        LOG(FATAL) << "Unknown type " << type->getName();
    };
    auto* io_space = dynamic_cast<IOExampleSpace*>(example_space);
    for (int _ = 0; _ < 10; ++_) {
        Example example;
        for (auto& inp_type: example_space->type_list) {
            example.push_back(gen(inp_type.get()));
        }
        if (io_space) {
            LOG(INFO) << "Random IO Example: " << example::ioExample2String(io_space->getIOExample(example));
        } else {
            LOG(INFO) << "Random Example: " << data::dataList2String(example);
        }
    }
}

int main(int argv, char** argc) {
    std::string task_path;
    if (argv == 2) {
        task_path = std::string(argc[1]);
    } else task_path = config::KSourcePath + "/tests/test_int.sl";

    auto spec = parser::getSyGuSSpecFromFile(task_path);
    Verifier *verifier = nullptr;
    {
        auto *example_space = dynamic_cast<FiniteExampleSpace*>(spec->example_space.get());
        if (example_space) verifier = new FiniteExampleVerifier(example_space);
    }

    {
        auto *z3_example_space = dynamic_cast<Z3ExampleSpace*>(spec->example_space.get());
        if (z3_example_space) {
            testGenRandomExamples(dynamic_cast<Z3ExampleSpace*>(spec->example_space.get()));
            verifier = new Z3Verifier(z3_example_space);
        }
    }
    assert(verifier);
    auto* io_space = dynamic_cast<IOExampleSpace*>(spec->example_space.get());
    auto* grammar = spec->info_list[0]->grammar;

    auto* recorder = new TimeRecorder();
    recorder->start("synthesis");
    auto res = cegis(grammar, verifier, io_space, spec->env.get());
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