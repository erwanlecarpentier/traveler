#ifndef RANDOM_POLICY_HPP_
#define RANDOM_POLICY_HPP_

class random_policy : public policy{
public:
    action apply(const state &s) const override {
        //TODO
    }

    void process_reward(
        const state &s,
        const action &a,
        double r,
        const state &s_p) override {
        // Nothing to process for random policy
    }
};

#endif // RANDOM_POLICY_HPP_
