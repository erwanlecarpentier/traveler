#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <map_node.hpp>

class action {
public:
    std::string direction;

    action(std::string _direction) : direction(_direction) {
        //
    }
};

#endif // ACTION_HPP_
