#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <vector>
#include <string>
#include <unordered_map>

#define DIRECTION_CNT 4

using coordinate_t = int;

// Classes
class Position;
enum class Direction;
class Rover;
class Sensor;
class RoverBuilder;
class Command;

class Position {
public:
    Position() = default;
    Position(coordinate_t x, coordinate_t y) : _x(x), _y(y) {}

    friend std::ostream &operator<<(std::ostream &os, const Position &pos) {
        os << "(" << pos._x << ", " << pos._y << ")";
        return os;
    }

    constexpr Position& operator+= (const Position& rhs) {
        _x += rhs._x;
        _y += rhs._y;
        return *this;
    }
    constexpr Position& operator-= (const Position& rhs) {
        _x -= rhs._x;
        _y -= rhs._y;
        return *this;
    }

    coordinate_t _x;
    coordinate_t _y;
};

// North is zero, then incrementing in order of the wind rose clockwise.
// Notice that for move_back
enum class Direction : int {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
};

// String aliases for our Direction codes.
static inline const std::string Direction_names[DIRECTION_CNT] = {
    "NORTH",
    "EAST",
    "SOUTH",
    "WEST"
};

// We will add and subtract these values to our rover's current position to move
// accordingly forward and backward, choosing the vector based of our direction code.
static inline const Position move_vector[DIRECTION_CNT] = {
    Position( 0,  1),
    Position( 1,  0),
    Position( 0, -1),
    Position(-1,  0)
};

class Command {
public:
    virtual bool run(Rover* rover) = 0; // operation failed - false, otherwise true
};

// TODO
class Sensor {
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};

class Rover {
public:
    Rover() = default;

    void execute(const std::string &s) {
        if (!_land) {
            throw std::logic_error("Rover has not landed.");
        }

        _stop = false;
        for (auto _command_name : s) {
            if (!_commands.contains(_command_name) ||
                !_commands[_command_name]->run(this))
            {
                _stop = true;
                return;
            }
        }
    }

    void land(std::pair<coordinate_t, coordinate_t> coordinates, Direction direction) {
        _land = true;
        _position = Position(coordinates.first, coordinates.second);
        _direction = static_cast<int>(direction);
    }

    friend std::ostream &operator<<(std::ostream &os, const Rover &rover) {
        if (!rover._land) {
            os << "unknown\n";
        }
        else {
            os << rover._position << " " << Direction_names[rover._direction]
               << (rover._stop ? " stopped\n" : "\n");
        }
        return os;
    }

    Position _position;
    int _direction;

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::unique_ptr<Sensor>> _sensors;
    bool _build = false;
    bool _land = false;
    bool _stop = false;
};

// TODO
class MoveForward : public Command {
public:
    MoveForward() = default;

    bool run(Rover* rover) override {
        // if (!rover.sensor_stuff) return false;
        (*rover)._position += move_vector[rover->_direction];
        return true;
    }
};

// TODO
class MoveBackward : public Command {
public:
    MoveBackward() = default;
    bool run(Rover* rover) override {
        // if (!rover.sensor_stuff) return false;
        (*rover)._position -= move_vector[rover->_direction];
        return true;
    }
};


class RotateRight : public Command {
public:
    RotateRight() = default;
    bool run(Rover* rover) override {
        (*rover)._direction = (rover->_direction + 1) % 4;
        return true;
    }
};


class RotateLeft : public Command {
public:
    RotateLeft() = default;
    bool run(Rover* rover) override {
        (*rover)._direction = (rover->_direction + 3) % 4; // + 4 - 1 = 3
        return true;
    }
};

class Compose : public Command {
public:
    Compose(std::initializer_list<std::shared_ptr<Command>> list) {
        for (const auto& it : list) {
            _commands.push_back(it);
        }
    }

    bool run(Rover* rover) override {
        for (const auto& _command : _commands) {
            if (!_command->run(rover)) {
                return false;
            }
        }
        return true;
    }

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

// TODO
class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder& program_command(char c, const std::shared_ptr<Command>& command) {
        return *this;
    }

    RoverBuilder& add_sensor(std::unique_ptr<Sensor> sensor) {
        return *this;
    }

    Rover build() {
        return Rover();
    }
};

#endif //ROVER_ROVER_H

#pragma clang diagnostic pop