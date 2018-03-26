#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

class environment {
public:
    map_graph m;

    /**
     * @brief Constructor
     *
     * Constructs an object given the parameters.
     * @param {parameters &} p; used parameters
     */
    environment(const parameters &p) {
        p.parse_map(m);
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
};

#endif // ENVIRONMENT_HPP_
