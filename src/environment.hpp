#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <map_node.hpp>
#include <utils.hpp>

class environment {
public:
    std::vector<map_node> nodes_vector;

    /**
     * @brief Constructor
     *
     * Constructs an object given the parameters.
     * @param {parameters &} p; used parameters
     */
    environment(parameters &p) {
        p.parse_map(nodes_vector);
    }

    state state_transition(const state &s, const action &a) const {
        //TODO
    }

    double reward_function(const state &s, const action &a, const state &s_p) const {
        //TODO
    }

    void transition(agent &ag) const {
        ag.s_p = state_transition(ag.s, ag.a);
        ag.r = reward_function(ag.s, ag.a, ag.s_p);
    }

    bool is_terminal(const agent &ag) const {
        //TODO
        return false;
    }

    void print_map() {
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
    }
};

#endif // ENVIRONMENT_HPP_
