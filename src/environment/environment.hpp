#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <map_node.hpp>
#include <utils.hpp>

class environment {
public:
    double reward_scaling_max;
    double goal_reward;
    double dead_end_reward;
    std::vector<unsigned> time_scale;
    std::vector<map_node> nodes_vector;

    /**
     * @brief Constructor
     */
    environment(
        double _reward_scaling_max,
        double _goal_reward,
        double _dead_end_reward,
        std::vector<unsigned> &_time_scale,
        std::vector<map_node> &_nodes_vector) :
        reward_scaling_max(_reward_scaling_max),
        goal_reward(_goal_reward),
        dead_end_reward(_dead_end_reward),
        time_scale(_time_scale),
        nodes_vector(std::move(_nodes_vector))
    {
        //print_environment();
    }

    /**
     * @brief Is action valid
     *
     * Check whether the action is valid and if so modify the input indice as the one of the
     * successor edge.
     */
    bool is_action_valid(const state &s, const action &a, unsigned &indice) const {
        for(unsigned i=0; i<s.get_nb_edges(); ++i) {
            if(s.nd_ptr->edges[i]->name.compare(a.direction) == 0) {
                indice = i;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Is the state a dead-end
     */
    bool is_dead_end(const state &st) const {
        if((st.get_nb_edges() == 0) && !st.nd_ptr->is_goal) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Is state terminal
     *
     * Check whether the input state is terminal.
     * A terminal state is either a dead-end or the goal.
     */
    bool is_state_terminal(const state &st) const {
        return st.nd_ptr->is_goal || is_dead_end(st);
    }

    /**
     * @brief Get terminal reward
     */
    double get_terminal_reward(const state &st) const {
        assert(is_state_terminal(st));
        if(is_dead_end(st)) {
            return dead_end_reward;
        } else {
            return goal_reward;
        }
    }

    double reward_from_duration(double duration) const {
        assert(!is_less_than(duration,0.));
        if(is_greater_than(duration,reward_scaling_max)) {
            return -1.;
        } else {
            return 0.5 * (cos(3.1415926535897 * duration / reward_scaling_max) + 1) - 1.;
        }
    }

    /**
     * @brief Reward function
     */
    double reward_function(const state &st, double duration) {
        if(is_state_terminal(st)) {
            return get_terminal_reward(st);
        } else {
            return reward_from_duration(duration);
        }
    }

    /**
     * @brief Transition function
     */
    void transition(
        const state &s,
        double t,
        const action &a,
        double &r,
        state &s_p)
    {
        unsigned indice = 0;
        if(is_action_valid(s,a,indice)) { // Go to edge
            double duration = s.get_time_to_successor(indice,t,time_scale);
            s_p = state(
                s.t + duration,
                s.get_ptr_to_successor(indice)
            );
            r = reward_function(s_p,duration);
        } else { // Illegal action
            throw illegal_action_exception();
        }
    }

    /**
     * @brief Find node by name
     */
    map_node * find_node_by_name(const std::string &name) {
        for(auto &nd : nodes_vector) {
            if(nd.name.compare(name) == 0) {
                return &nd;
            }
        }
        throw nonexistent_node_exception();
        return nullptr;
    }

    /**
     * @brief Print
     */
    void print_environment() {
        std::cout << "Printing map graph:\nNodes:\n";
        for(auto &nd : nodes_vector) {
            std::cout << " - " << nd.name << std::endl;
        }
        std::cout << "Edges:\n";
        for(auto &nd : nodes_vector) {
            std::cout << "  - ";
            nd.print();
            std::cout << "    is linked to:\n";
            for(unsigned k=0; k<nd.edges.size(); ++k) {
                std::cout << "      - ";
                nd.edges.at(k)->print();
                std::cout << "        with costs: ";
                printv(nd.edges_costs.at(k));
                /*
                std::cout << "        which is linked to:\n";
                for(auto &ndnd : nd.edges.at(k)->edges) {
                    std::cout << "          - ";
                    ndnd->print();
                }
                */
            }
        }
        std::cout << "Time scale:\n";
        printv(time_scale);
    }
};

#endif // ENVIRONMENT_HPP_
