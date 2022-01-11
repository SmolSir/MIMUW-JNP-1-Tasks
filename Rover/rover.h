#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

using coordinate_t = int;

class Coordinates {
public:
    Coordinates() = delete;
    Coordinates(coordinate_t x, coordinate_t y) : _x(x), _y(y) {}

    friend std::ostream &operator<<(std::ostream &os, const Coordinates &c) {
        os << "(" << c._x << ", " << c._y << ")";
        return os;
    }

private:
    coordinate_t _x;
    coordinate_t _y;
};

class Sensor {
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};

enum class Direction {WEST, EAST, SOUTH, NORTH};

class Operation {
    virtual void run();
};

class move_forward : public Operation {
public:
    move_forward();
};

class move_backward : public Operation {
public:
    move_backward();
};

class rotate_right : public Operation {
public:
    rotate_right();
};

class rotate_left : public Operation {
public:
    rotate_left();
};

class compose : public Operation {
public:
    compose(std::initializer_list<Operation> list);
};

class Rover {
public:
    void execute(const std::string &s);

    void land(std::pair<coordinate_t, coordinate_t>, Direction d);

    friend std::ostream &operator<<(std::ostream &os, const Rover &r);

};

class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder program_command(char, const Operation &o);

    RoverBuilder add_sensor(std::unique_ptr<Sensor> s);

    Rover build();
};

#endif //ROVER_ROVER_H
