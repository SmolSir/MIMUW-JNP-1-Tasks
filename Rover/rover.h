#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <vector>

using coordinate_t = int;

enum class Direction {WEST, EAST, SOUTH, NORTH};

class Position {
public:
    Position() = delete;
    Position(coordinate_t x, coordinate_t y, Direction d) : _x(x), _y(y), _d(d){}

    friend std::ostream &operator<<(std::ostream &os, const Position &c) {
        os << "(" << c._x << ", " << c._y << ")" << " " << "TO DO (Direction)";
        return os;
    }

private:
    coordinate_t _x;
    coordinate_t _y;
    Direction _d;
};

class Sensor {
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};

class Command {
public:
    virtual void run() = 0;
};

// TODO
class MoveForward : public Command {
public:
    MoveForward() = default;

    void run() override {}
};

// TODO
class MoveBackward : public Command {
public:
    MoveBackward() = default;
    void run() override {}
};

// TODO
class RotateRight : public Command {
public:
    RotateRight() = default;
    void run() override {}
};

// TODO
class RotateLeft : public Command {
public:
    RotateLeft() = default;
    void run() override {}
};

// TODO
class Compose : public Command {
public:
    Compose(std::initializer_list<std::shared_ptr<Command>> list) {
        for (const auto & it : list) {
            _commands.push_back(it);
        }
    }

    void run() override {}

private:
    std::vector<std::shared_ptr<Command>> _commands;
};

std::shared_ptr<MoveForward> move_forward() {
    return std::make_shared<MoveForward>();
}

std::shared_ptr<MoveBackward> move_backward() {
    return std::make_shared<MoveBackward>();
}

std::shared_ptr<RotateRight> rotate_right() {
    return std::make_shared<RotateRight>();
}

std::shared_ptr<RotateLeft> rotate_left() {
    return std::make_shared<RotateLeft>();
}

std::shared_ptr<Compose> compose(std::initializer_list<std::shared_ptr<Command>> list) {
    return std::make_shared<Compose>(list);
}

//TO DO
class Rover {
public:
    Rover() = default;
    void execute(const std::string &s) {}

    void land(std::pair<coordinate_t, coordinate_t>, Direction d) {}

    friend std::ostream &operator<<(std::ostream &os, const Rover &r) {
        return os;
    }

};

// TODO
class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder& program_command(char, std::shared_ptr<Command> &o) {
        return *this;
    }

    RoverBuilder& add_sensor(std::unique_ptr<Sensor> s) {
        return *this;
    }

    Rover build() {
        return Rover();
    }
};

#endif //ROVER_ROVER_H
