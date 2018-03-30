#ifndef MCTS_POLICY_HPP_
#define MCTS_POLICY_HPP_

#include <cnode.hpp>
#include <dnode.hpp>
#include <environment.hpp>
#include <random_policy.hpp>
#include <utils.hpp>

class mcts_policy : public policy {
public:
    random_policy default_policy; ///< Default policy
    environment * envt_ptr; ///< Generative model (pointer to the real environment)
    bool is_model_dynamic; ///< Is the model dynamic
    double discount_factor; ///< Discount factor
    double uct_parameter; ///< UCT parameter
    double terminal_state_value; ///< Terminal state value
    unsigned budget; ///< Budget ie number of expanded nodes in the tree
    unsigned horizon; ///< Horizon for the default policy simulation
    unsigned mcts_strategy_switch; ///< Strategy switch for MCTS algorithm

    unsigned nb_calls; ///< Number of calls to the generative model
    unsigned nb_cnodes; ///< Number of expanded chance nodes

    /**
     * @brief Constructor
     */
    mcts_policy(
        environment * _envt_ptr,
        bool _is_model_dynamic,
        double _discount_factor,
        double _uct_parameter,
        double _terminal_state_value,
        unsigned _budget,
        unsigned _horizon,
        unsigned _mcts_strategy_switch) :
        envt_ptr(_envt_ptr),
        is_model_dynamic(_is_model_dynamic),
        discount_factor(_discount_factor),
        uct_parameter(_uct_parameter),
        terminal_state_value(_terminal_state_value),
        budget(_budget),
        horizon(_horizon),
        mcts_strategy_switch(_mcts_strategy_switch)
    {
        nb_calls = 0;
        nb_cnodes = 0;
    }

    void generative_model(
        const state &s,
        const action &a,
        double &r,
        state &s_p,
        double t_ref)
    {
        ++nb_calls;
        if(is_model_dynamic) {
            envt_ptr->transition(s,s.t,a,r,s_p);
        } else {
            envt_ptr->transition(s,t_ref,a,r,s_p);
        }
    }

    /**
     * @brief Sample return
     *
     * Sample a return with the default policy starting at the input state.
     * @param {state} s; input state
     * @return Return the sampled return.
     */
    double sample_return(cnode * ptr, double t_ref) {
        if(envt_ptr->is_state_terminal(ptr->s)) {
            return terminal_state_value;
        }
        double total_return = 0.;
        state s = ptr->s;
        action a = ptr->a;
        for(unsigned t=0; t<horizon; ++t) {
            state s_p;
            double r = 0.;
            generative_model(s,a,r,s_p,t_ref);
            total_return += pow(discount_factor,(double)t) * r;
            if(envt_ptr->is_state_terminal(s_p)) {
                break;
            }
            s = s_p;
            a = default_policy.apply(s);
        }
        return total_return;
    }

    /**
     * @brief Update value
     *
     * Update the value of a chance node by stacking a new sampled value.
     * @param {cnode *} ptr; pointer to the updated chance node
     * @param {double} q; new sampled value
     */
    void update_value(cnode * ptr, double q) const {
        ptr->sampled_returns.push_back(q);
    }

    /**
     * @brief MCTS strategy
     *
     * Select child of a decision node wrt the MCTS strategy.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * mcts_strategy(dnode * v) const {
        return v->children.at(rand_indice(v->children)).get();
    }

    /**
     * @brief UCT scores
     *
     * Compute the UCT scores of the given vector of chance nodes.
     * @param {const std::vector<std::unique_ptr<cnode>> &} cv; input vector
     * @return Return a vector containing the scores of the given vector of chance nodes.
     */
    std::vector<double> uct_scores(const std::vector<std::unique_ptr<cnode>> &cv) const {
        std::vector<double> scores;
        for(auto &c : cv) {
            scores.emplace_back(
                c->get_value()
                + 2 * uct_parameter *
                sqrt(log((double) nb_cnodes) / ((double) c->get_nb_visits()))
            );
        }
        return scores;
    }

    /**
     * @brief UCT strategy
     *
     * Select child of a decision node wrt the UCT strategy.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * uct_strategy(dnode * v) const {
        return v->children.at(argmax(uct_scores(v->children))).get();
    }

    /**
     * @brief Select child
     *
     * Select child of a decision node wrt one of the implemented strategies.
     * The node must be fully expanded.
     * @param {dnode *} v; decision node
     * @return Return to the select child, which is a chance node.
     */
    cnode * select_child(dnode * v) const {
        switch(mcts_strategy_switch) {
            case 0: { // Vanilla MCTS
                return mcts_strategy(v);
            }
            case 1: { // UCT
                return uct_strategy(v);
            }
            default: { // Vanilla MCTS
                return mcts_strategy(v);
            }
        }
    }

    /**
     * @brief Evaluate
     *
     * Create a new child node to a decision node and sample a return value with the default
     * policy.
     * @param {dnode *} v; pointer to the decision node
     * @return Return the sampled value.
     */
    double evaluate(dnode * v, double t_ref) {
        nb_cnodes++; // a chance node will be created
        v->create_child();
        double q = sample_return(v->children.back().get(),t_ref);
        update_value(v->children.back().get(),q);
        return q;
    }

    /**
     * @brief Is state already sampled
     *
     * Equality operator.
     * @param {cnode *} ptr; pointer to the chance node
     * @param {state &} s; sampled state
     * @param {unsigned &} ind; indice modified to the value of the indice of the existing
     * decision node with state s if the comparison succeeds.
     */
    bool is_state_already_sampled(cnode * ptr, state &s, unsigned &ind) const {
        for(unsigned i=0; i<ptr->children.size(); ++i) {
            if(s.is_equal_to(ptr->children[i]->s)) {
                ind = i;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Search tree
     *
     * Search within the tree, starting from the input decision node.
     * Recursive method.
     * @param {dnode *} v; input decision node
     * @param {double} t_root; time reference of the root node
     * @return Return the sampled return at the given decision node
     */
    double search_tree(dnode * v, double t_root) {
        if(envt_ptr->is_state_terminal(v->s)) { // terminal node
            return terminal_state_value;
        } else if(!v->is_fully_expanded()) { // leaf node, expand it
            return evaluate(v,t_root);
        } else { // apply tree policy
            cnode * ptr = select_child(v);
            state s_p;
            double r = 0.;
            generative_model(v->s,ptr->a,r,s_p,t_root);
            double q = 0.;
            unsigned ind = 0; // indice of resulting child
            if(is_state_already_sampled(ptr,s_p,ind)) { // go to node
                q = r + discount_factor * search_tree(ptr->children.at(ind).get(),t_root);
            } else { // leaf node, create a new node
                ptr->children.emplace_back(std::unique_ptr<dnode>(
                    new dnode(s_p,ptr->depth+1)
                ));
                q = r + discount_factor * evaluate(ptr->get_last_child(),t_root);
            }
            update_value(ptr,q);
            return q;
        }
    }

    /**
     * @brief Build tree
     *
     * Build a tree at the input node.
     * @param {dnode &} v; reference to the input node
     */
    void build_tree(dnode &v) {
        for(unsigned i=0; i<budget; ++i) {
            search_tree(&v,v.s.t);
        }
        nb_cnodes = 0;
    }

    /**
     * @brief Argmax value
     *
     * Get the indice of the child with the maximum value.
     * @param {const dnode &} v; input decision node
     * @return Return the indice of the child with the maximum value.
     */
    unsigned argmax_value(const dnode &v) const {
        std::vector<double> values;
        for(auto &c: v.children) {
            values.emplace_back(c->get_value());
        }
        return argmax(values);
    }

    /**
     * @brief Argmax visit counter
     *
     * Get the indice of the child with the maximum number of visits.
     * @param {const dnode &} v; input decision node
     * @return Return the indice of the child with the maximum number of visits.
     */
    unsigned argmax_nb_visits(const dnode &v) const {
        std::vector<unsigned> nb_visits;
        for(auto &c: v.children) {
            nb_visits.emplace_back(c->get_nb_visits());
        }
        return argmax(nb_visits);
    }

    /**
     * @brief Recommended action
     *
     * Get the recommended action from an input decision node.
     * @param {const dnode &} v; input decision node
     * @return Return the recommended action at the input decision node.
     */
    action recommended_action(const dnode &v) {
        //return v.children.at(argmax_nb_visits(v))->a; // higher number of visits
        return v.children.at(argmax_value(v))->a; // higher value
    }

    action apply(const state &s) override {
        dnode root(s);
        build_tree(root);
        return recommended_action(root);
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
