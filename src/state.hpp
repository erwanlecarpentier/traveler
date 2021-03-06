#ifndef STATE_HPP_
#define STATE_HPP_

#include <exceptions.hpp>
#include <map_node.hpp>
#include <utils.hpp>

class state {
public:
    double t; ///< time
    map_node * nd_ptr; ///< location in the graph

    state() : t(0.) {}

    state(double _t, map_node * _nd_ptr) : t(_t), nd_ptr(_nd_ptr) {}

    std::string get_name() const {
        return nd_ptr->name;
    }

    unsigned get_nb_edges() const {
        return nd_ptr->edges.size();
    }

    /**
     * @brief Get pointer to successor
     *
     * Get a pointer to the successor designated by the given indice.
     * @param {unsigned} indice; indice of the successor in node->edges
     * @return Return a map_node pointer.
     */
    map_node * get_ptr_to_successor(unsigned indice) const {
        return nd_ptr->edges.at(indice);
    }

    /**
     * @brief Get action space
     *
     * Get the available actions at this state.
     * @return Return the vector of available actions.
     */
    std::vector<action> get_action_space() const {
        if(get_nb_edges() > 0) {
            std::vector<action> v;
            for(auto &dest_nd_ptr : nd_ptr->edges) {
                v.emplace_back(dest_nd_ptr->name);
            }
            return v;
        } else {
            std::vector<action> v{get_name()};
            return v;
            //throw no_action_exception();
        }
    }

    bool is_equal_to(const state &s) const {
        return ((t == s.t) && (nd_ptr == s.nd_ptr));
    }

    void print() const {
        std::cout << "State - ";
        std::cout << "name: " << get_name() << "; ";
        std::cout << "time: " << t << "; ";
        std::cout << "nbedges: " << get_nb_edges() << "; ";
        std::cout << "goal: " << nd_ptr->is_goal << "\n";
        assert(get_nb_edges() == nd_ptr->edges_costs.size());
    }
};

#endif // STATE_HPP_
