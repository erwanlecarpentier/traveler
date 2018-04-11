#ifndef TMP_CNODE_HPP_
#define TMP_CNODE_HPP_

#include <estimates_history.hpp>
#include <linear_algebra.hpp>

class tmp_dnode; // forward declaration

/**
 * @brief Chance node class
 */
class tmp_cnode {
public:
    const estimates_history * eh_ptr;
    const state s; ///< Labelling state
    const action a; ///< Labelling action
    const double t_ref;
    const double regression_regularization;
    const unsigned polynomial_regression_degree;
    const double depth; ///< Depth
    std::vector<std::unique_ptr<tmp_dnode>> children; ///< Child nodes
    std::vector<double> sampled_returns; ///< Sampled returns

    /**
     * @brief Constructor
     */
    tmp_cnode(
        estimates_history * _eh_ptr,
        state _s,
        action _a,
        double _t_ref,
        double _regression_regularization,
        unsigned _polynomial_regression_degree,
        double _depth = 0) :
        eh_ptr(_eh_ptr),
        s(_s),
        a(_a),
        t_ref(_t_ref),
        regression_regularization(_regression_regularization),
        polynomial_regression_degree(_polynomial_regression_degree),
        depth(_depth)
    {
        //
    }

//// COPY OF CNODE //////////////////////////////////////////////////////////////////////////

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

//// END COPY OF CNODE //////////////////////////////////////////////////////////////////////

    /**
     * @brief Get the mean of the sampled returns
     */
    double get_sampled_returns_mean() const {
        return std::accumulate(sampled_returns.begin(),sampled_returns.end(),0.0) / ((double) get_nb_visits());
    }

    double polynomial_value_prediction() const {
        std::vector<double> x = {s.t - t_ref};
        std::vector<double> y = {get_sampled_returns_mean()};
        for(auto & e : eh_ptr->hist) {
            x.push_back(e.t_node - e.t_root);
            y.push_back(e.value);
        }
        return polynomial_regression_prediction_at(
            0.,polynomial_regression(
                x,
                y,
                regression_regularization,
                polynomial_regression_degree
            )
        );
    }

    /**
     * @brief Get the value estimate
     */
    double get_value() const {
        if((eh_ptr == nullptr) || eh_ptr->is_history_empty()) {
            return get_sampled_returns_mean();
        } else {
            //TODO switch here
            /*
            std::cout << std::endl;//TODO remove
            std::cout << "is nullptr       : " << (eh_ptr == nullptr) << std::endl;
            std::cout << "is_history_empty : " << eh_ptr->is_history_empty() << std::endl;
            std::cout << "size             : " << eh_ptr->hist.size() << std::endl;
            std::cout << "size < 100       : " << (eh_ptr->hist.size() < 100) << std::endl;
            std::cout << "location         : " << eh_ptr->location << std::endl;
            std::cout << "direction        : " << eh_ptr->direction << std::endl;
            */
            return get_sampled_returns_mean();//TODO remove and put next
            //return polynomial_value_prediction();
        }
    }
};

#endif // TMP_CNODE_HPP_
