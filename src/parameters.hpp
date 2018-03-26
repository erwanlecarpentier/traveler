#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <map_graph.hpp>
#include <state.hpp>

class parameters {
public:
    unsigned SIMULATION_LIMIT_TIME = 10;
    std::string GRAPH_DURATION_MATRIX_PATH = "data/duration_matrix.csv";
    state INITIAL_STATE;

    parameters() {
        //TODO
    }

    void parse_map(map_graph &m) const {
        //TODO
    }
};

#endif // PARAMETERS_HPP_
