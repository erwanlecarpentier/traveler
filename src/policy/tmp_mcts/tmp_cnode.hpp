#ifndef TMP_CNODE_HPP_
#define TMP_CNODE_HPP_

#include <estimates_history.hpp>

class tmp_dnode; // forward declaration

/**
 * @brief Chance node class
 */
class tmp_cnode {
public:
    estimates_history * eh_ptr;

//// COPY OF CNODE //////////////////////////////////////////////////////////////////////////

    state s; ///< Labelling state
    action a; ///< Labelling action
    std::vector<std::unique_ptr<tmp_dnode>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns
    double depth; ///< Depth

    /**
     * @brief Constructor
     */
    tmp_cnode(
        estimates_history * _eh_ptr,
        state _s,
        action _a,
        double _depth = 0) :
        eh_ptr(_eh_ptr),
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
    tmp_dnode * get_last_child() const {
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

//// END COPY OF CNODE //////////////////////////////////////////////////////////////////////

};

#endif // TMP_CNODE_HPP_
