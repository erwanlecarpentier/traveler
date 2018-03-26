#ifndef STATE_HPP_
#define STATE_HPP_

#include <map_node.hpp>

class state {
public:
    double t; ///< time
    map_node * node; ///< location in the graph
};

#endif // STATE_HPP_
