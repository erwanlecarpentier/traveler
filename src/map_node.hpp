#ifndef MAP_NODE_HPP_
#define MAP_NODE_HPP_

class map_node {
public:
    std::string name;
    std::vector<map_node*> edges;
    std::vector<std::vector<unsigned>> edges_costs;
    bool is_terminal;

    map_node(std::string _name, bool _is_terminal) : name(_name), is_terminal(_is_terminal) {
        //
    }

    void print() const {
        std::cout << "Map node - ";
        std::cout << "name: " << name << "; ";
        std::cout << "nbedges: " << edges.size() << "; ";
        std::cout << "terminal: " << is_terminal << "\n";
        assert(edges.size() == edges_costs.size());
    }
};

#endif // MAP_NODE_HPP_
