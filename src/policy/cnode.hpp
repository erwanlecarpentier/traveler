#ifndef CNODE_HPP_
#define CNODE_HPP_

class dnode; // forward declaration

/**
 * @brief Chance node class
 */
class cnode {
public:
    state s; ///< Labelling state
    action a; ///< Labelling action
    std::vector<std::unique_ptr<dnode>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns
    double depth; ///< Depth

    /**
     * @brief Constructor
     */
    cnode(
        state _s,
        action _a,
        double _depth = 0) :
        s(_s),
        a(_a),
        depth(_depth)
    {
        //
    }

    /**
     * @brief Get pointer to last child
     *
     * @return Return a pointer to the lastly created child.
     */
    dnode * get_last_child() const {
        return children.back().get();
    }

    /**
     * @brief Get number of visits
     *
     * Get the value of the node.
     * This is the mean of the sampled returns.
     * @return Return the value of the node.
     */
    double get_nb_visits() const {
        return (double) sampled_returns.size();
    }

    /**
     * @brief Get value
     *
     * Get the value of the node.
     * This is the mean of the sampled returns.
     * @return Return the value of the node.
     */
    double get_value() const {
        return std::accumulate(sampled_returns.begin(),sampled_returns.end(),0.0) / get_nb_visits();
    }
};

#endif // CNODE_HPP_
