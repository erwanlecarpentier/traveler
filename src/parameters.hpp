#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>

#include <agent.hpp>
#include <exceptions.hpp>
#include <policy.hpp>
#include <mcts_policy.hpp>
#include <random_policy.hpp>
#include <state.hpp>

class parameters {
public:
    // Simulation parameters
    unsigned SIMULATION_LIMIT_TIME;
    std::string INITIAL_LOCATION;
    std::string TERMINAL_LOCATION;
    std::string GRAPH_DURATION_MATRIX_PATH;
    std::string CSV_SEP;
    // Policy parameters
    unsigned POLICY_SELECTOR;
    bool IS_MODEL_DYNAMIC;
    double DISCOUNT_FACTOR;
    double UCT_PARAMETER;
    unsigned TREE_SEARCH_BUDGET;
    unsigned DEFAULT_POLICY_HORIZON;

    /**
     * @brief Default constructor
     */
    parameters(const char *cfg_path) {
        libconfig::Config cfg;
        try {
            cfg.readFile(cfg_path);
        }
        catch(const libconfig::ParseException &e) {
            display_libconfig_parse_exception(e);
        }
        if(cfg.lookupValue("simulation_limit_time",SIMULATION_LIMIT_TIME)
        && cfg.lookupValue("initial_location",INITIAL_LOCATION)
        && cfg.lookupValue("terminal_location",TERMINAL_LOCATION)
        && cfg.lookupValue("graph_duration_matrix",GRAPH_DURATION_MATRIX_PATH)
        && cfg.lookupValue("csv_sep",CSV_SEP)
        && cfg.lookupValue("policy_selector",POLICY_SELECTOR)
        && cfg.lookupValue("is_model_dynamic",IS_MODEL_DYNAMIC)
        && cfg.lookupValue("discount_factor",DISCOUNT_FACTOR)
        && cfg.lookupValue("uct_parameter",UCT_PARAMETER)
        && cfg.lookupValue("tree_search_budget",TREE_SEARCH_BUDGET)
        && cfg.lookupValue("default_policy_horizon",DEFAULT_POLICY_HORIZON)) {
            /* Nothing to do */
        }
        else { // Error in config file
            throw wrong_syntax_configuration_file_exception();
        }
    }

    /**
     * @brief Display libconfig ParseException
     *
     * @param {const libconfig::ParseException &} e; displayed exception
     */
    void display_libconfig_parse_exception(const libconfig::ParseException &e) const {
        std::cerr << "Error in parameters(const char *cfg_path): ParseException ";
        std::cerr << "in file " << e.getFile() << " ";
        std::cerr << "at line " << e.getLine() << ": ";
        std::cerr << e.getError() << std::endl;
    }

    agent build_agent(environment &en) const {
        auto po = build_policy(en);
        return agent(
            po,
            en.get_initial_node_ptr(INITIAL_LOCATION)
        );
    }

    std::unique_ptr<policy> build_policy(environment &en) const {
        switch(POLICY_SELECTOR) {
            case 0: { // MCTS policy
                return std::unique_ptr<policy> (
                    new mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 38
                    )
                );
            }
            case 1: { // UCT policy
                return std::unique_ptr<policy> (
                    new mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 0
                    )
                );
            }
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

                while(std::getline(ss,cell,CSV_SEP.at(0))) {
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
