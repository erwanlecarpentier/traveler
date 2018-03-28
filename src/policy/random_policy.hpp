#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

#include <utils.hpp>

class random_policy : public policy {
public:
    action apply(const state &s) const override {
        return rand_element(s.get_action_space());
        /*
        if(s.nd_ptr->edges.size() > 0) {
            unsigned indice = rand_indice(s.nd_ptr->edges);
            return action(s.nd_ptr->edges[indice]->name);
        } else {
            throw no_action_exception();
        }
        */
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
        // Nothing to process for random policy
    }
};

#endif // RANDOM_POLICY_HPP_
