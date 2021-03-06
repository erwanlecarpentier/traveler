#ifndef POLICY_HPP_
#define POLICY_HPP_

#include <state.hpp>
#include <action.hpp>

class policy {
public:
    virtual ~policy() {}

    virtual action apply (const state &s) = 0;

    virtual void process_reward(
        const state &s,
        const action &a,
        unsigned r,
        const state &s_p) = 0;
};

#endif // POLICY_HPP_
