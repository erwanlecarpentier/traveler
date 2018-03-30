#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

#include <utils.hpp>

class random_policy : public policy {
public:
    action apply(const state &s) override {
        return rand_element(s.get_action_space());
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
        /* Nothing to process for random policy */
    }
};

#endif // RANDOM_POLICY_HPP_
