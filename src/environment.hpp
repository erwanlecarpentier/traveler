#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <map_node.hpp>
#include <utils.hpp>

class environment {
public:
    std::vector<unsigned> time_scale;
    std::vector<map_node> nodes_vector;

    /**
     * @brief Constructor
     *
     * Constructs an object given the parameters.
     * @param {parameters &} p; used parameters
     */
    environment(parameters &p) {
        p.parse_environment(time_scale,nodes_vector);
        //print_environment();
    }

    bool is_action_valid(const state &s, action &a, unsigned &indice) const {
        for(unsigned i=0; i<s.node->edges.size(); ++i) {
            if(s.node->edges[i]->name.compare(a.direction) == 0) {
                indice = i;
                return true;
            }
        }
        return false;
    }

    /** @deprecated */
    /*
    state state_transition(const state &s, action &a) {
        unsigned indice = 0;
        if(is_action_valid(s,a,indice)) {
            return state(
                s.t + s.get_time_to_successor(indice,time_scale),
                s.get_ptr_to_successor(indice)
            );
        } else {
            throw illegal_action_exception();
        }
    }
    */

    /** @deprecated */
    /*
    unsigned reward_function(const state &s, state &s_p) const {
        for(unsigned i=0; i<s.node->edges.size(); ++i) {
            if(s.node->edges[i]->name.compare(s_p.get_location_name()) == 0) {
                return s.node->edges_costs[i][s.t];
            }
        }
        throw illegal_action_exception();
    }
    */

    void transition(agent &ag) {
        //ag.s_p = state_transition(ag.s, ag.a); /** @deprecated */
        //ag.r = reward_function(ag.s, ag.s_p); /** @deprecated */
        unsigned indice = 0;
        if(is_action_valid(ag.s,ag.a,indice)) {
            ag.r = ag.s.get_time_to_successor(indice,time_scale);
            ag.s_p = state(
                ag.s.t + ag.r,
                ag.s.get_ptr_to_successor(indice)
            );
        } else {
            throw illegal_action_exception();
        }
    }

    bool is_state_terminal(const agent &ag) const {
        return ag.s.node->is_terminal;
    }

    map_node * find_node_by_name(std::string &name) {
        for(auto &nd : nodes_vector) {
            if(nd.name.compare(name) == 0) {
                return &nd;
            }
        }
        throw nonexistent_node_exception();
        return nullptr;
    }

    map_node * get_initial_node_ptr(std::string &name) {
        return find_node_by_name(name);
    }

    void print_environment() {
        std::cout << "Printing map graph:\nNodes:\n";
        for(auto &nd : nodes_vector) {
            std::cout << " - " << nd.name << std::endl;
        }
        std::cout << "Edges:\n";
        for(auto &nd : nodes_vector) {
            std::cout << " - " << nd.name << " (" << nd.edges.size() << " edge(s)):" << std::endl;
            for(unsigned k=0; k<nd.edges.size(); ++k) {
                std::cout << "     - " << nd.edges.at(k)->name << std::endl;
                std::cout << "       costs: ";
                printv(nd.edges_costs.at(k));
            }
        }
        std::cout << "Time scale:\n";
        printv(time_scale);
    }
};

#endif // ENVIRONMENT_HPP_
