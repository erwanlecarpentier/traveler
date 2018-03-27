#ifndef MCTS_POLICY_HPP_
#define MCTS_POLICY_HPP_

#include <cnode.hpp>
#include <dnode.hpp>
#include <utils.hpp>

class mcts_policy : public policy {
public:
    action apply(const state &s) const override {
        return rand_element(s.get_action_space());
        //TODO
    }

    void process_reward(
        const state &s,
        const action &a,
        unsigned r,
        const state &s_p) override {
        (void) s;
        (void) a;
        (void) r;
        (void) s_p;
        // Nothing to process for MCTS policy
    }
};

#endif // MCTS_POLICY_HPP_
