#ifndef POLICY_HPP_
#define POLICY_HPP_

class policy {
public:
    virtual action apply (const state &s) const = 0;

    virtual void process_reward(
        const state &s,
        const action &a,
        double r,
        const state &s_p) = 0;
};

#endif // POLICY_HPP_
