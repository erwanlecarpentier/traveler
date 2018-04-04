#ifndef ESTIMATES_HISTORY_HPP_
#define ESTIMATES_HISTORY_HPP_

/**
 * @brief Estimate class
 */
class estimate {
public:
    double t_root; ///< Time of the root node at the moment of the estimation
    double t_node; ///< Estimated time of the chance node
    double value; ///< Estimated value of the chance node

    estimate(double _t_root, double _t_node, double _value) :
        t_root(_t_root),
        t_node(_t_node),
        value(_value)
    {}

    void print() const {
        std::cout << "troot: " << t_root << " ";
        std::cout << "tnode: " << t_node << " ";
        std::cout << "val: " << value << "\n";
    }
};

/**
 * @brief Estimates history class
 *
 * Estimate history class associated to a state-action pair.
 */
class estimates_history {
public:
    std::string location; ///< State
    std::string direction; ///< Action
    std::vector<estimate> hist; ///< Associated history of estimates

    /**
     * @brief Constructor
     *
     * Constructor adding one element to the history.
     */
    estimates_history(
        std::string _location,
        std::string _direction,
        double _t_root,
        double _t_node,
        double _value) :
        location(_location),
        direction(_direction)
    {
        add_estimate(_t_root,_t_node,_value);
    }

    /**
     * @brief Add estimate
     *
     * Add a new estimate to the state-action pair.
     */
    void add_estimate(double _t_root, double _t_node, double _value) {
        hist.emplace_back(_t_root,_t_node,_value);
    }

    /**
     * @brief Corresponds to a state-action pair
     *
     * Test whether the history corresponds to the given state-action pair.
     */
    bool corresponds_to(const state &st, const action &ac) const {
        return (
            location.compare(st.get_name()) == 0 &&
            direction.compare(ac.direction) == 0
        );
    }

    void print() const {
        std::cout << "loc: " << location << " ";
        std::cout << "dir: " << direction << " ";
        std::cout << "hist:\n";
        for(auto &e : hist) {
            e.print();
        }
    }

    /**
     * @brief Is history empty
     */
    bool is_history_empty() {
        return hist.size() == 0;
    }
};

#endif // ESTIMATES_HISTORY_HPP_
