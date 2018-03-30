#ifndef TCNODE_HPP_
#define TCNODE_HPP_

class tdnode; // forward declaration

class tcnode {
public:
    state s; ///< Labelling state
    action a; ///< Labelling action
    std::vector<std::unique_ptr<tdnode>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns
    std::vector<std::pair<double,double>> estimates_history; ///< History of the value estimates plus their estimated delay
    double depth; ///< Depth

    /**
     * @brief Constructor
     */
    tcnode(
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
    tdnode * get_last_child() const {
        return children.back().get();
    }

    /**
     * @brief Get number of visits
     *
     * Get the number of visits.
     * @return Return the number of visits of the node.
     */
    unsigned get_nb_visits() const {
        return sampled_returns.size();
    }

    /**
     * @brief Get value
     *
     * Get the value of the node.
     * This is the mean of the sampled returns.
     * @return Return the value of the node.
     */
    double get_value() const {
        return std::accumulate(sampled_returns.begin(),sampled_returns.end(),0.0) / ((double) get_nb_visits());
    }
};

#endif // TCNODE_HPP_
