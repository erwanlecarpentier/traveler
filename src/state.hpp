#ifndef STATE_HPP_
#define STATE_HPP_

#include <exceptions.hpp>
#include <map_node.hpp>

class state {
public:
    double t; ///< time
    map_node * nd_ptr; ///< location in the graph

    state() {
        //
    }

    state(unsigned _t, map_node * _nd_ptr) : t(_t), nd_ptr(_nd_ptr) {
        //
    }

    std::string get_location_name() {
        return nd_ptr->name;
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
        double t_request,
        std::vector<unsigned> &ts) const
    {
        std::tuple<unsigned,unsigned> ti_ind = get_uplow_indices(t_request,ts);
        std::vector<unsigned> c = nd_ptr->edges_costs.at(su_ind);
        double c_m, c_p = c.at(std::get<1>(ti_ind));
        double t_m, t_p = ts.at(std::get<1>(ti_ind));
        if(std::get<0>(ti_ind) == std::get<1>(ti_ind)) {
            std::cout << "Warning, max range of duration matrix reached.\n";
            c_m = c.at(std::get<0>(ti_ind) - 1);
            t_m = ts.at(std::get<0>(ti_ind) - 1);
        } else {
            c_m = c.at(std::get<0>(ti_ind));
            t_m = ts.at(std::get<0>(ti_ind));
        }
        return ((c_p - c_m) / (t_p - t_m)) * t_request + (c_m * t_p - c_p * t_m) / (t_p - t_m);
    }

    /**
     * @brief Get action space
     *
     * Get the available actions at this state.
     * @return Return the vector of available actions.
     */
    std::vector<action> get_action_space() const {
        if(nd_ptr->edges.size() > 0) {
            std::vector<action> v;
            for(auto &dest_nd_ptr : nd_ptr->edges) {
                v.emplace_back(dest_nd_ptr->name);
            }
            return v;
        } else {
            throw no_action_exception();
        }
    }

    bool is_equal_to(const state &s) const {
        return ((t == s.t) && (nd_ptr == s.nd_ptr));
    }

    void print() const {
        std::cout << "State - ";
        std::cout << "name: " << nd_ptr->name << "; ";
        std::cout << "time: " << t << "; ";
        std::cout << "nbedges: " << nd_ptr->edges.size() << "; ";
        std::cout << "terminal: " << nd_ptr->is_terminal << "\n";
        assert(nd_ptr->edges.size() == nd_ptr->edges_costs.size());
    }
};

#endif // STATE_HPP_
