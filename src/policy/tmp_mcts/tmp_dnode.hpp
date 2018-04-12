#ifndef TMP_DNODE_HPP_
#define TMP_DNODE_HPP_

#include <tmp_cnode.hpp>
#include <utils.hpp>

/**
 * @brief Decision node class
 */
class tmp_dnode {
public:
//// COPY OF DNODE //////////////////////////////////////////////////////////////////////////

    const state s; ///< Labelling state
    const double depth; ///< Depth

    std::vector<action> actions; ///< Available actions, iteratively removed
    std::vector<std::unique_ptr<tmp_cnode>> children; ///< Child nodes

    /**
     * @brief Constructor
     */
    tmp_dnode(
        const state &_s,
        double _depth = 0) :
        s(_s),
        depth(_depth),
        actions(_s.get_action_space())
    {
        //
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

//// END COPY OF DNODE //////////////////////////////////////////////////////////////////////

    /**
     * @brief Get a pointer to the corresponding estimates history
     *
     * Given a vector of estimate histories, get a pointer to the one corresponding to the
     * input state-action pair.
     * @param {const std::vector<estimates_history> &} ehc; vector of estimate histories
     * @param {const state &} st; state
     * @param {const action &} ac; action
     * @return Return the pointer.
     */
    estimates_history * get_ptr_to_eh(
        std::list<estimates_history> &ehc,
        const state &st,
        const action &ac) const
    {
        for(auto &eh : ehc) {
            if(eh.corresponds_to(st,ac)) {
                return &eh;
            }
        }
        return nullptr; // no match
    }

    /**
     * @brief Create Child
     *
     * Create a chance node child.
     * The labelling action of the child is randomly selected.
     * @param {const std::vector<estimates_history> &} ehc; vector of estimate histories
     * @param {double} reference_time; time at the root node when the child was created
     * @return Return the sampled action.
     * @warning Remove the sampled action from the actions vector.
     */
    action create_child(
        std::list<estimates_history> &ehc,
        double reference_time,
        double regression_regularization,
        double polynomial_regression_degree) {
        unsigned indice = rand_indice(actions);
        action ac = actions.at(indice);
        actions.erase(actions.begin() + indice);
        children.emplace_back(
            std::unique_ptr<tmp_cnode>(
                new tmp_cnode(
                    get_ptr_to_eh(ehc,s,ac),
                    s,
                    ac,
                    reference_time,
                    regression_regularization,
                    polynomial_regression_degree,
                    depth
                )
            )
        );
        return ac;
    }
};

#endif // TMP_DNODE_HPP_
