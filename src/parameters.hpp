#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>

#include <agent.hpp>
#include <exceptions.hpp>
#include <mcts_policy.hpp>
#include <policy.hpp>
#include <random_policy.hpp>
#include <state.hpp>

class parameters {
public:
    // Simulation parameters
    unsigned SIMULATION_LIMIT_TIME;
    // Environment parameters
    bool GENERATE_MAP;

    unsigned NB_TIME_STEPS = 10;
    unsigned TIME_STEPS_WIDTH = 200;
    unsigned NB_NODES = 5;
    unsigned MIN_NB_EDGES_PER_NODE = 1;
    int INITIAL_DURATION_MIN = 50;
    int INITIAL_DURATION_MAX = 150;
    int DURATION_VARIATION_MAX = 50;

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
        && cfg.lookupValue("generate_map",GENERATE_MAP)
        && cfg.lookupValue("nb_time_steps",NB_TIME_STEPS)
        && cfg.lookupValue("time_steps_width",TIME_STEPS_WIDTH)
        && cfg.lookupValue("nb_nodes",NB_NODES)
        && cfg.lookupValue("min_nb_edges_per_node",MIN_NB_EDGES_PER_NODE)
        && cfg.lookupValue("initial_duration_min",INITIAL_DURATION_MIN)
        && cfg.lookupValue("initial_duration_max",INITIAL_DURATION_MAX)
        && cfg.lookupValue("duration_variation_max",DURATION_VARIATION_MAX)
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

    environment build_environment() const {
        std::vector<std::vector<std::string>> dm;
        if(GENERATE_MAP) {
            dm = build_random_duration_matrix();
        } else {
            dm = extract_duration_matrix();
        }
        return buil_environment_from_duration_matrix(dm);
    }

    std::vector<std::string> create_random_edge(
        unsigned orig_ind,
        unsigned dest_ind,
        const std::vector<std::string> &nodes_names) const
    {
        std::vector<std::string> new_line;
        new_line.push_back(nodes_names.at(orig_ind));
        new_line.push_back(nodes_names.at(dest_ind));
        unsigned d = (unsigned) abs(uniform_integer(INITIAL_DURATION_MIN,INITIAL_DURATION_MAX));
        for(unsigned j=0; j<NB_TIME_STEPS+1; ++j) {
            new_line.push_back(std::to_string(d));
            int var = uniform_integer(-DURATION_VARIATION_MAX,DURATION_VARIATION_MAX);
            if ((abs(var) > d) && var < 0) {
                d = 0;
            } else {
                d += var;
            }
        }
        return new_line;
    }

    std::vector<std::vector<std::string>> build_random_duration_matrix() const {
        std::vector<std::vector<std::string>> dm;
        // 0. time scale
        std::vector<std::string> first_line;
        first_line.push_back("start");
        first_line.push_back("goal");
        for(unsigned i=0; i<NB_TIME_STEPS+1; ++i) {
            first_line.push_back(std::to_string(i * TIME_STEPS_WIDTH));
        }
        dm.push_back(first_line);
        // 1. Nodes
        std::vector<std::string> nodes_names;
        for(unsigned i=0; i<NB_NODES; ++i) {
            nodes_names.push_back("n" + std::to_string(i));
        }
        std::vector<unsigned> nodes_edges_counter(nodes_names.size(),0);
        // 2. Edges
        for(unsigned i=0; i<NB_NODES; ++i) {
            for(unsigned k=nodes_edges_counter.at(i); k<MIN_NB_EDGES_PER_NODE; ++k) {
                unsigned dest_ind = i;
                while(dest_ind == i) {
                    dest_ind = rand_indice(nodes_names);
                }
                dm.push_back(create_random_edge(i,dest_ind,nodes_names));
            }
        }
        // 2. Additional edges for non-reachable nodes
        for(unsigned i=0; i<NB_NODES; ++i) {
            bool is_reachable = false;
            for(unsigned k=1; k<dm.size(); ++k) {
                if(dm.at(k).at(1).compare(nodes_names.at(i)) == 0) {
                    is_reachable = true;
                    break;
                }
            }
            if(!is_reachable) {
                unsigned orig_ind = i;
                while(orig_ind == i) {
                    orig_ind = rand_indice(nodes_names);
                }
                dm.push_back(create_random_edge(orig_ind,i,nodes_names));
            }
        }

        //TRM
        for(auto &line : dm) {
            for(auto &cell : line) {
                std::cout << cell << " | ";
            }
            std::cout << std::endl;
        }
        //TRM
        return dm;
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

    environment buil_environment_from_duration_matrix(
        const std::vector<std::vector<std::string>> &dm) const
    {
        std::vector<unsigned> ts;
        std::vector<map_node> nv;
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
        return environment(ts,nv);
    }
};

#endif // PARAMETERS_HPP_
