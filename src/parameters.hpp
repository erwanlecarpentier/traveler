#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <fstream>
#include <sstream>

#include <agent.hpp>
#include <exceptions.hpp>
#include <policy.hpp>
//#include <mcts_policy.hpp>
#include <random_policy.hpp>
#include <state.hpp>

class parameters {
public:
    // Simulation parameters
    unsigned SIMULATION_LIMIT_TIME;
    unsigned POLICY_SELECTOR;
    std::string INITIAL_LOCATION;
    std::string TERMINAL_LOCATION;
    std::string GRAPH_DURATION_MATRIX_PATH;
    char CSV_SEP;
    // Policy parameters
    bool IS_MODEL_DYNAMIC;
    double DISCOUNT_FACTOR;
    double UCT_CST;
    unsigned TREE_SEARCH_BUDGET;
    unsigned DEFAULT_POLICY_HORIZON;

    /**
     * @brief Default constructor
     */
    parameters() {
        SIMULATION_LIMIT_TIME = 20;
        POLICY_SELECTOR = 0;
        INITIAL_LOCATION = "A";
        TERMINAL_LOCATION = "C";
        GRAPH_DURATION_MATRIX_PATH = "data/duration_matrix.csv";
        CSV_SEP = ';';

        IS_MODEL_DYNAMIC = true;
        DISCOUNT_FACTOR = 0.9;
        UCT_CST = 0.7;
        TREE_SEARCH_BUDGET = 2;
        DEFAULT_POLICY_HORIZON = 1;
    }

    agent build_agent(environment &en) const {
        auto po = build_policy();
        return agent(
            po,
            en.get_initial_node_ptr(INITIAL_LOCATION)
        );
    }

    std::unique_ptr<policy> build_policy() const {
        switch(POLICY_SELECTOR) {
            /*case 0: { // mcts policy
                return std::unique_ptr<policy> (new mcts_policy());
            }*/
            default: { // random policy
                return std::unique_ptr<policy> (new random_policy());
            }
        }
    }

    std::vector<std::vector<std::string>> extract_duration_matrix() const {
        std::filebuf fb;
        if (fb.open(GRAPH_DURATION_MATRIX_PATH,std::ios::in)) {
            std::vector<std::vector<std::string>> dm;
            std::istream is(&fb);
            while(true) {
                std::vector<std::string> result;
                std::string line;
                std::getline(is,line);
                std::stringstream ss(line);
                std::string cell;

                while(std::getline(ss,cell,CSV_SEP)) {
                    result.push_back(cell);
                }
                if(is.eof()) {
                    break;
                } else {
                    dm.push_back(result);
                }
            }
            fb.close();
            return dm;
        } else {
            throw duration_matrix_file_exception();
        }
    }

    bool is_node_already_created(std::string s, const std::vector<map_node> &v) const {
        for(auto &elt: v) {
            if(elt.name.compare(s) == 0) {
                return true;
            }
        }
        return false;
    }

    std::vector<unsigned> get_durations(
        unsigned i,
        const std::vector<std::vector<std::string>> &dm) const
    {
        std::vector<unsigned> v;
        for(unsigned j=2; j<dm.at(i).size(); ++j) {
            v.push_back(std::stoul(dm.at(i).at(j),nullptr,0));
        }
        return v;
    }

    environment build_environment() const {
        std::vector<unsigned> ts;
        std::vector<map_node> nv;
        parse_environment(ts,nv);
        return environment(ts,nv);
    }

    void parse_environment(
        std::vector<unsigned> &ts,
        std::vector<map_node> &nv) const
    {
        std::vector<std::vector<std::string>> dm = extract_duration_matrix();
        // 0. Extract time scale
        unsigned tref = std::stoul(dm.at(0).at(2));
        for(unsigned j=2; j<dm.at(0).size(); ++j) {
            ts.push_back(std::stoul(dm.at(0).at(j)) - tref);
        }
        // 1. Create nodes and assign termination criterion
        for(unsigned i=1; i<dm.size(); ++i) {
            for(unsigned j=0; j<2; ++j) {
                if(!is_node_already_created(dm.at(i).at(j),nv)) {
                    if(dm.at(i).at(j).compare(TERMINAL_LOCATION) == 0) {
                        nv.emplace_back(map_node(dm.at(i).at(j),true));
                    } else {
                        nv.emplace_back(map_node(dm.at(i).at(j),false));
                    }
                }
            }
        }
        // 2. Create edges
        for(unsigned i=1; i<dm.size(); ++i) {
            unsigned orig_nd_indice = 0, dest_nd_indice = 0;
            for(unsigned k=0; k<nv.size(); ++k) {
                if(nv.at(k).name.compare(dm.at(i).at(0)) == 0) {
                    orig_nd_indice = k;
                }
                if(nv.at(k).name.compare(dm.at(i).at(1)) == 0) {
                    dest_nd_indice = k;
                }
            }
            nv.at(orig_nd_indice).edges.emplace_back(&nv.at(dest_nd_indice));
            nv.at(orig_nd_indice).edges_costs.push_back(get_durations(i,dm));
        }
    }
};

#endif // PARAMETERS_HPP_
