#ifndef MAP_NODE_HPP_
#define MAP_NODE_HPP_

class map_node {
public:
    const std::string name;
    const bool is_goal;

    std::vector<map_node*> edges;
    std::vector<std::vector<double>> edges_costs;

    map_node(
        const std::string &_name,
        bool _is_goal) :
        name(_name),
        is_goal(_is_goal)
    {}

    void print() const {
        std::cout << "Map node - ";
        std::cout << "name: " << name << "; ";
        std::cout << "nbedges: " << edges.size() << "; ";
        std::cout << "terminal: " << is_goal << "\n";
        assert(edges.size() == edges_costs.size());
    }
};

#endif // MAP_NODE_HPP_
