#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <libconfig.h++>
#include <fstream>
#include <sstream>

#include <agent.hpp>
#include <exceptions.hpp>
#include <map_builder.hpp>
#include <state.hpp>

#include <policy.hpp>
#include <mcts_policy.hpp>
#include <random_policy.hpp>
#include <tmp_mcts_policy.hpp>

class parameters {
public:
    // Simulation parameters
    unsigned SIMULATION_LIMIT_TIME;

    // Environment parameters
    double REWARD_SCALING_MAX;
    double GOAL_REWARD;
    double DEAD_END_REWARD;
    bool GENERATE_MAP;

    unsigned SAMPLER_SELECTOR;
    bool SYMMETRIC_GRAPH;
    unsigned NB_TIME_STEPS;
    unsigned TIME_STEPS_WIDTH;
    unsigned NB_NODES;
    unsigned MIN_NB_EDGES_PER_NODE;
    double INITIAL_DURATION_MIN;
    double INITIAL_DURATION_MAX;
    double DURATION_VARIATION_MAX;

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
    double REGRESSION_REGULARIZATION;
    unsigned POLYNOMIAL_REGRESSION_DEGREE;

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
        && cfg.lookupValue("reward_scaling_max",REWARD_SCALING_MAX)
        && cfg.lookupValue("goal_reward",GOAL_REWARD)
        && cfg.lookupValue("dead_end_reward",DEAD_END_REWARD)
        && cfg.lookupValue("generate_map",GENERATE_MAP)
        && cfg.lookupValue("sampler_selector",SAMPLER_SELECTOR)
        && cfg.lookupValue("symmetric_graph",SYMMETRIC_GRAPH)
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
        && cfg.lookupValue("default_policy_horizon",DEFAULT_POLICY_HORIZON)
        && cfg.lookupValue("regression_regularization",REGRESSION_REGULARIZATION)
        && cfg.lookupValue("polynomial_regression_degree",POLYNOMIAL_REGRESSION_DEGREE)) {
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

    /**
     * @brief Agent builder
     */
    agent build_agent(environment &en) const {
        auto po = build_policy(en);
        return agent(
            po,
            en.find_node_by_name(INITIAL_LOCATION)
        );
    }

    /**
     * @brief Policy builder
     */
    std::unique_ptr<policy> build_policy(environment &en) const {
        switch(POLICY_SELECTOR) {
            case 0: { // random policy
                return std::unique_ptr<policy> (new random_policy());
            }
            case 1: { // MCTS policy
                return std::unique_ptr<policy> (
                    new mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 0
                    )
                );
            }
            case 2: { // UCT policy
                return std::unique_ptr<policy> (
                    new mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 1
                    )
                );
            }
            case 3: { // TMP_MCTS policy
                return std::unique_ptr<policy> (
                    new tmp_mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 0,
                        REGRESSION_REGULARIZATION, POLYNOMIAL_REGRESSION_DEGREE
                    )
                );
            }
            case 4: { // TMP_UCT policy
                return std::unique_ptr<policy> (
                    new tmp_mcts_policy(
                        &en, IS_MODEL_DYNAMIC, DISCOUNT_FACTOR, UCT_PARAMETER,
                        TREE_SEARCH_BUDGET, DEFAULT_POLICY_HORIZON, 1,
                        REGRESSION_REGULARIZATION, POLYNOMIAL_REGRESSION_DEGREE
                    )
                );
            }
            default: { // random policy
                return std::unique_ptr<policy> (new random_policy());
            }
        }
    }

    /**
     * @brief Environment builder
     */
    environment build_environment() const {
        map_builder mb(
            SAMPLER_SELECTOR,
            NB_TIME_STEPS,
            TIME_STEPS_WIDTH,
            NB_NODES,
            MIN_NB_EDGES_PER_NODE,
            INITIAL_DURATION_MIN,
            INITIAL_DURATION_MAX,
            DURATION_VARIATION_MAX,
            TERMINAL_LOCATION,
            GRAPH_DURATION_MATRIX_PATH,
            CSV_SEP
        );
        std::vector<std::vector<std::string>> dm;
        if(GENERATE_MAP) {
            if(SYMMETRIC_GRAPH) {
                dm = mb.build_random_connected_symmetric_directed_duration_matrix();
            } else {
                dm = mb.build_random_connected_directed_duration_matrix();
            }
        } else {
            dm = mb.extract_duration_matrix();
        }
        std::vector<unsigned> ts;
        std::vector<map_node> nv;
        mb.build_time_scale_and_map_from_duration_matrix(dm,ts,nv);
        return environment(REWARD_SCALING_MAX,GOAL_REWARD,DEAD_END_REWARD,ts,nv);
    }
};

#endif // PARAMETERS_HPP_
