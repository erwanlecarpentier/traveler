#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

#include <map_node.hpp>
#include <utils.hpp>

class environment {
public:
    double reward_scaling_max;
    std::vector<unsigned> time_scale;
    std::vector<map_node> nodes_vector;

    /**
     * @brief Constructor
     */
    environment(
        double _reward_scaling_max,
        std::vector<unsigned> &_time_scale,
        std::vector<map_node> &_nodes_vector) :
        reward_scaling_max(_reward_scaling_max),
        time_scale(_time_scale),
        nodes_vector(std::move(_nodes_vector))
    {
        //print_environment();
    }

    bool is_action_valid(const state &s, const action &a, unsigned &indice) const {
        for(unsigned i=0; i<s.nd_ptr->edges.size(); ++i) {
            if(s.nd_ptr->edges[i]->name.compare(a.direction) == 0) {
                indice = i;
                return true;
            }
        }
        return false;
    }

    double reward_from_duration(double duration) const {
        assert(!is_less_than(duration,0.));
        if(is_greater_than(duration,reward_scaling_max)) {
            return 0.;
        } else {
            return 0.5 * (cos(3.1415926535897 * duration / reward_scaling_max) + 1);
        }
    }

    void transition(
        const state &s,
        double t,
        const action &a,
        double &r,
        state &s_p)
    {
        unsigned indice = 0;
        if(is_action_valid(s,a,indice)) {
            double duration = s.get_time_to_successor(indice,t,time_scale);
            s_p = state(
                s.t + duration,
                s.get_ptr_to_successor(indice)
            );
            r = reward_from_duration(duration);
        } else {
            throw illegal_action_exception();
        }
    }

    bool is_state_terminal(const state &s) const {
        return s.nd_ptr->is_terminal;
    }

    map_node * find_node_by_name(const std::string &name) {
        for(auto &nd : nodes_vector) {
            if(nd.name.compare(name) == 0) {
                return &nd;
            }
        }
        throw nonexistent_node_exception();
        return nullptr;
    }

    map_node * get_initial_node_ptr(const std::string &name) {
        return find_node_by_name(name);
    }

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
