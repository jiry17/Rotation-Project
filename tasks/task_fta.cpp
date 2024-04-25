#include "istool/sygus/parser/parser.h"
#include "istool/basic/config.h"
#include <string>
#include <iostream>
#include "glog/logging.h"

// Declarations of data structures related to an FTA. You can change them on your demand.
class FTAEdge;
class FTANode {
public:
    NonTerminal* symbol; // The corresponding non-terminal.
    DataList oup_list; // The corresponding outputs of this node on all considered examples.
    std::vector<FTAEdge*> edge_list;
    FTANode(NonTerminal* _symbol, const DataList& _oup): symbol(_symbol), oup_list(_oup) {};
    ~FTANode();
};

class FTAEdge {
public:
    Rule* rule; // The corresponding grammar rule.
    std::vector<FTANode*> node_list;
    FTAEdge(Rule* _rule, const std::vector<FTANode*> _node_list): rule(_rule), node_list(_node_list) {
    }
};

FTANode::~FTANode() {
    for (auto* edge: edge_list) delete edge;
}

class FTA {
public:
    std::vector<FTANode*> final_list; // All final nodes in the FTA.
    std::vector<FTANode*> node_list; // All nodes in the FTA.
    FTA(const std::vector<FTANode*> _final_list, const std::vector<FTANode*>& _node_list):
        final_list(_final_list), node_list(_node_list) {
    }
    ~FTA() {
        for (auto* node: node_list) delete node;
    }
};

/**
 * Construct an FTA for grammar @grammar and input-output example @example.
 * @limit is a threshold limiting the FTA to finite. It can be the depth limit of the FTA or an upper bound of the
 * program size. You can choose a meaning convenient for your implementation.
 */
FTA* constructFTA(Grammar* grammar, const IOExample& example, Env* env, int limit) {
    // TODO: (Task 2) Complete this function
    // I use a nullptr to represent an empty FTA. You can change this usage on demand.
    return nullptr;
}

// Get the intersection of two FTAs.
FTA* intersectFTA(FTA* x, FTA* y) {
    // TODO: (Task 2) Complete this function;
    return nullptr;
}

// Extract a valid program from a non-empty FTA
PProgram extractProgramFromFTA(FTA* x) {
    assert(x);
    // TODO: (Task 2) Complete this function
}

PProgram synthesisFromExample(Grammar* grammar, const IOExampleList& examples, Env* env) {
    static int limit = 0;
    for (;; limit++) {
        auto* fta = constructFTA(grammar, examples[0], env, limit);

        for (int i = 1; i < examples.size() && fta; ++i) {
            auto* current = constructFTA(grammar, examples[i], env, limit);
            auto* new_fta = intersectFTA(fta, current);
            delete fta; delete current;
            fta = new_fta;
        }

        if (fta) {
            auto res = extractProgramFromFTA(fta);
            delete fta; return res;
        }
    }
}

PProgram cegis(Grammar* grammar, const IOExampleList& examples, Env* env) {
    // TODO: Copy the implementation of CEGIS from Task 1 to here
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