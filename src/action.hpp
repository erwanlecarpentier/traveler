#ifndef ACTION_HPP_
#define ACTION_HPP_

class action {
public:
    std::string direction;

    action() {}

    action(std::string _direction) : direction(_direction) {
        //
    }

    bool is_equal_to(action a) {
        return (direction.compare(a.direction) == 0);
    }
};

#endif // ACTION_HPP_
