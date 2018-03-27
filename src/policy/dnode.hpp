#ifndef DNODE_HPP_
#define DNODE_HPP_

#include <cnode.hpp>
#include <utils.hpp>

/**
 * @brief Decision node class
 */
class dnode {
public:
    state s; ///< Labelling state
    std::vector<action> actions; ///< Available actions, iteratively removed
    std::vector<std::unique_ptr<cnode>> children; ///< Child nodes
    double depth; ///< Depth

    /**
     * @brief Constructor
     */
    dnode(
        state _s,
        std::vector<action> _actions,
        double _depth = 0) :
        s(_s),
        actions(_actions),
        depth(_depth)
    {
        //
    }

    /**
     * @brief Create Child
     *
     * Create a child (hence a chance node).
     * The action of the child is randomly selected.
     * @return Return the sampled action.
     * @warning Remove the sampled action from the actions vector.
     */
    action create_child() {
        unsigned indice = rand_indice(actions);
        action sampled_action = actions.at(indice);
        actions.erase(actions.begin() + indice);
        children.emplace_back(std::unique_ptr<cnode>(new cnode(s,sampled_action,depth)));
        return sampled_action;
    }

    /**
     * @brief Get children values
     *
     * Get an ordered vector containing all the values of the children.
     * * @return Return the vector containing all the children values.
     */
    std::vector<double> get_children_values() const {
        std::vector<double> children_values;
        for(auto &c : children) {
            children_values.emplace_back(c->get_value());
        }
        return children_values;
    }

    /**
     * @brief Get value
     *
     * Get the value of the node.
     * This is the maximum value of its children.
     * @return Return the value of the node.
     */
    double get_value() const {
        std::vector<double> children_values = get_children_values();
        //return children.at(argmax(children_values))->get_value();
        return *std::max_element(children_values.begin(),children_values.end());
    }

    /**
     * @brief Is fully expanded
     *
     * Test whether the node is fully expanded ie if every actions have been sampled.
     * Node is a leaf.
     * @return Return a boolean answer to the test.
     */
    bool is_fully_expanded() const {
        return (actions.size() == 0);
    }
};

#endif // DNODE_HPP_
