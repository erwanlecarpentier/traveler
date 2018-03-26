#ifndef MAP_NODE_HPP_
#define MAP_NODE_HPP_

class map_node {
public:
    std::string name;
    std::vector<std::shared_ptr<map_node>> edges;

    map_node() {
        //TODO
    }
};

#endif // MAP_NODE_HPP_
