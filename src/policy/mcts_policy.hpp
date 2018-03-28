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
    //double terminal_state_value; ///< Terminal state value
    unsigned budget; ///< Budget ie number of expanded nodes in the tree
    //unsigned nb_cnodes; ///< Number of expanded nodes
    //unsigned nb_calls; ///< Number of calls to the generative model
    unsigned horizon; ///< Horizon for the default policy simulation
    //unsigned mcts_strategy_switch; ///< Strategy switch for MCTS algorithm

    /**
     * @brief Constructor
     */
    mcts_policy(
        environment * _envt_ptr,
        bool _is_model_dynamic,
        double _discount_factor,
        double _uct_parameter,
        unsigned _budget,
        unsigned _horizon) :
        envt_ptr(_envt_ptr),
        is_model_dynamic(_is_model_dynamic),
        discount_factor(_discount_factor),
        uct_parameter(_uct_parameter),
        budget(_budget),
        horizon(_horizon)
    {
        //
    }

/*
    state generative_model(const state &s, std::shared_ptr<action> a, MD &mod) {
        ++nb_calls;
        state s_p;
        mod.state_transition(s,a,s_p);
        return s_p;
    }

    double sample_return(cnode * ptr, MD &mod) {
        if(mod.is_terminal(ptr->s)) {
            return terminal_state_value;
        }
        double total_return = 0.;
        state s = ptr->s;
        std::shared_ptr<action> a = ptr->a;
        for(unsigned t=0; t<horizon; ++t) {
            state s_p = generative_model(s,a,mod);
            total_return += pow(discount_factor,(double)t) * mod.reward_function(s,a,s_p);
            if(is_model_dynamic) {
                mod.step(s_p);
            }
            if(mod.is_terminal(s_p)) {
                break;
            }
            s = s_p;
            a = default_policy(s);
        }
        return total_return;
    }

    void update_value(cnode * ptr, double q) const {
        ptr->sampled_returns.push_back(q);
    }

    cnode * mcts_strategy(dnode * v) const {
        return v->children.at(rand_indice(v->children)).get();
    }

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

    cnode * uct_strategy(dnode * v) const {
        return v->children.at(argmax(uct_scores(v->children))).get();
    }

    cnode * tuct_strategy(dnode * v) const {
        std::vector<double> scores = uct_scores(v->children);
        unsigned maxind = argmax(scores);
        double delta = v->depth * lipschitz_q;
        if(are_equal(delta,0.)) {
            return v->children.at(maxind).get();
        } else {
            double deltamin = scores[maxind] - delta;
            std::vector<double> weights(scores.size(),0.);
            weights[maxind] = 2 * delta; // maximum weight
            for(unsigned i = 0; i < scores.size(); ++i) {
                if(i != maxind) {
                    if(is_greater_than(scores[i] + delta, deltamin)) {
                        weights[i] = scores[i] + delta - deltamin;
                    }
                }
            }
            return v->children.at(pick_weighted_indice(weights)).get();
        }
    }

    cnode * select_child(dnode * v) const {
        switch(mcts_strategy_switch) {
            case 0: { // UCT
                return uct_strategy(v);
            }
            case 1: { // TUCT
                return tuct_strategy(v);
            }
            default: { // Vanilla MCTS
                return mcts_strategy(v);
            }
        }
    }

    double evaluate(dnode * v, MD &mod) {
        nb_cnodes++; // a chance node will be created
        v->create_child();
        double q = sample_return(v->children.back().get(),mod);
        update_value(v->children.back().get(),q);
        return q;
    }

    bool is_state_already_sampled(cnode * ptr, state &s, unsigned &ind) const {
        for(unsigned i=0; i<ptr->children.size(); ++i) {
            if(s.is_equal_to(ptr->children[i]->s)) {
                ind = i;
                return true;
            }
        }
        return false;
    }

    double search_tree(dnode * v, MD &mod) {
        if(mod.is_terminal(v->s)) { // terminal node
            return terminal_state_value;
        } else if(!v->is_fully_expanded()) { // leaf node, expand it
            return evaluate(v, mod);
        } else { // apply tree policy
            cnode * ptr = select_child(v);
            state s_p = generative_model(v->s,ptr->a,mod);
            double r = mod.reward_function(v->s,ptr->a,s_p);
            if(is_model_dynamic) {
                mod.step(s_p);
            }
            double q = 0.;
            unsigned ind = 0; // indice of resulting child
            if(is_state_already_sampled(ptr,s_p,ind)) { // go to node
                q = r + discount_factor * search_tree(ptr->children.at(ind).get(), mod);
            } else { // leaf node, create a new node
                ptr->children.emplace_back(std::unique_ptr<dnode>(
                    new dnode(s_p,mod.get_action_space(s_p),ptr->depth+1)
                ));
                q = r + discount_factor * evaluate(ptr->get_last_child(), mod);
            }
            update_value(ptr,q);
            return q;
        }
    }

    void build_tree(dnode &root) {
        for(unsigned i=0; i<budget; ++i) {
            MD mod = model.get_copy();
            search_tree(&root, mod);
        }
        nb_cnodes = 0;
    }
*/

//    /**
//     * @brief Build tree
//     *
//     * Build a tree at the input root node.
//     * @param {dnode &} root; reference to the input root node
//     */
//    void build_tree(dnode &root) {
//        for(unsigned i=0; i<budget; ++i) {
//            MD mod = model.get_copy();
//            search_tree(&root, mod);
//        }
//        nb_cnodes = 0;
//    }
//
//    /**
//     * @brief Argmax value
//     *
//     * Get the indice of the child with the maximum value.
//     * @param {const dnode &} v; input decision node
//     * @return Return the indice of the child with the maximum value.
//     */
//    unsigned argmax_value(const dnode &v) const {
//        std::vector<double> values;
//        for(auto &c: v.children) {
//            values.emplace_back(c->get_value());
//        }
//        return argmax(values);
//    }
//
//    /**
//     * @brief Argmax visit counter
//     *
//     * Get the indice of the child with the maximum number of visits.
//     * @param {const dnode &} v; input decision node
//     * @return Return the indice of the child with the maximum number of visits.
//     */
//    unsigned argmax_nb_visits(const dnode &v) const {
//        std::vector<unsigned> nb_visits;
//        for(auto &c: v.children) {
//            nb_visits.emplace_back(c->get_nb_visits());
//        }
//        return argmax(nb_visits);
//    }
//
//    /**
//     * @brief Recommended action
//     *
//     * Get the recommended action from an input decision node.
//     * @param {const dnode &} v; input decision node
//     * @return Return the recommended action at the input decision node.
//     */
//    std::shared_ptr<action> recommended_action(const dnode &v) {
//        //return v.children.at(argmax_nb_visits(v))->a; // higher number of visits
//        return v.children.at(argmax_value(v))->a; // higher value
//    }

    action apply(const state &s) const override {
        return rand_element(s.get_action_space());
//        dnode root(s,s.get_action_space());
//        build_tree(root);
//        return recommended_action(root);
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
