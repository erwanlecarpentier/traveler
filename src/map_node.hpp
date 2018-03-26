#ifndef MAP_NODE_HPP_
#define MAP_NODE_HPP_

class map_node {
public:
    std::string name;
    std::vector<map_node*> edges;
    std::vector<std::vector<unsigned>> edges_costs;

    map_node(std::string _name) :name(_name) {
        //
    }
};

#endif // MAP_NODE_HPP_
