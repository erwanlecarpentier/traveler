#ifndef STATE_HPP_
#define STATE_HPP_

#include <map_node.hpp>

class state {
public:
    double t; ///< time
    map_node * node; ///< location in the graph

    state(unsigned _t, map_node * _node) : t(_t), node(_node) {
        //
    }

    std::string get_location_name() {
        return node->name;
    }

    /**
     * @brief Get pointer to successor
     *
     * Get a pointer to the successor designated by the given indice.
     * @param {unsigned} indice; indice of the successor in node->edges
     * @return Return a map_node pointer.
     */
    map_node * get_ptr_to_successor(unsigned indice) const {
        return node->edges[indice];
    }

    std::tuple<unsigned,unsigned> get_uplow_indices(
        double t,
        std::vector<unsigned> &ts) const
    {
        std::vector<unsigned>::iterator up = std::upper_bound (ts.begin(), ts.end(), t);
        unsigned upind = up - ts.begin();
        if(upind == 0) return std::make_tuple(0,0);
        else if(upind > ts.size()-1) return std::make_tuple(ts.size()-1,ts.size()-1);
        else return std::make_tuple(upind-1,upind);
    }

    /**
     * @brief Get time to successor
     *
     * Get the duration to go to the successor designated by the given indice.
     * @param {unsigned} su_ind; indice of the successor in node->edges
     * @return Return the duration as a double.
     */
    double get_time_to_successor(
        unsigned su_ind,
        std::vector<unsigned> &ts) const
    {
        std::tuple<unsigned,unsigned> ti_ind = get_uplow_indices(t,ts);
        std::vector<unsigned> c = node->edges_costs[su_ind];
        double c_m, c_p = c[std::get<1>(ti_ind)];
        double t_m, t_p = ts[std::get<1>(ti_ind)];
        if(std::get<0>(ti_ind) == std::get<1>(ti_ind)) {
            std::cout << "Warning, max range of duration matrix reached.\n";
            c_m = c[std::get<0>(ti_ind) - 1];
            t_m = ts[std::get<0>(ti_ind) - 1];
        } else {
            c_m = c[std::get<0>(ti_ind)];
            t_m = ts[std::get<0>(ti_ind)];
        }
        return ((c_p - c_m) / (t_p - t_m)) * t + (c_m * t_p - c_p * t_m) / (t_p - t_m);
    }

    std::vector<action> get_action_space() const {
        if(node->edges.size() > 0) {
            std::vector<action> v;
            for(auto &dest_nd_ptr : node->edges) {
                v.emplace_back(dest_nd_ptr->name);
            }
            return v;
        } else {
            throw no_action_exception();
        }
    }

    void print() {
        std::cout << "time: " << t << " location: " << node->name << std::endl;
    }
};

#endif // STATE_HPP_
