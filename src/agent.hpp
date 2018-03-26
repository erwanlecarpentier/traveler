#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <action.hpp>
#include <parameters.hpp>
#include <policy.hpp>
#include <state.hpp>

class agent {
public:
    std::unique_ptr<policy> po;
    state s;
    state s_p;
    action a;
    double r;

    /**
     * @brief Constructor
     *
     * Constructs an object given the parameters.
     * @param {parameters &} p; used parameters
     */
    agent(const parameters &p) {
        s = p.INITIAL_STATE;
    }

    void apply_policy() {
        a = po->apply(s);
    }

    void process_reward() {
        po->process_reward(s, a, r, s_p);
    }

    void step() {
        s = s_p;
    }
};

#endif // AGENT_HPP_
