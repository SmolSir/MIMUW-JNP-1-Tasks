#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>
#include <memory>

#include "positioning.h"


class Sensor {
public:
    constexpr Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};


class Command;
class RoverManagement;

class Rover {
public:
    Rover() = delete;

    void execute(const std::string &s);
    void land(Position position, Direction direction);
    friend std::ostream &operator<<(std::ostream &os, const Rover &rover);

    friend class RoverBuilder;
    friend class RoverManagement;

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;
    Position _position;
    Direction _direction;
    bool _land = false;
    bool _stop = false;

    Rover(std::unordered_map<char, std::shared_ptr<Command>> &commands,
          std::vector<std::shared_ptr<Sensor>> &sensors);
};

class RoverManagement {
public:
    RoverManagement() = delete;

    bool move(Position &new_position, Direction new_direction);
    Position &get_position();
    int get_direction() const;
    friend class Rover;

private:
    Rover &_r;
    RoverManagement(Rover &r) : _r(r) {}
    bool check_position(Position &to_check);
};


class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder &program_command(char c, const std::shared_ptr<Command> &command) {
        _commands[c] = command;
        return *this;
    }

    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor) {
        _sensors.emplace_back(std::move(sensor));
        return *this;
    }

    Rover build() {
        return Rover{_commands, _sensors};
    }

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;
};


class Command {
public:
    virtual ~Command() = default;

    virtual bool run(RoverManagement &rover) = 0; // operation failed - false, otherwise true
};


class MoveForward : public Command {
public:
    MoveForward() = default;

    bool run(RoverManagement &rover) override {
        Position new_position = rover.get_position() + move_vector[rover.get_direction()];
        return rover.move(new_position, Direction(rover.get_direction()));
    }
};


class MoveBackward : public Command {
public:
    MoveBackward() = default;

    bool run(RoverManagement &rover) override {
        Position new_position = rover.get_position() - move_vector[rover.get_direction()];
        return rover.move(new_position, Direction(rover.get_direction()));
    }
};


class RotateRight : public Command {
public:
    RotateRight() = default;

    bool run(RoverManagement &rover) override {
        return rover.move(rover.get_position(), Direction((rover.get_direction() + 1) % 4));
    }
};


class RotateLeft : public Command {
public:
    RotateLeft() = default;

    bool run(RoverManagement &rover) override {
        return rover.move(rover.get_position(), Direction((rover.get_direction() + 3) % 4));; // + 4 - 1 = 3
    }
};

class Compose : public Command {
public:
    Compose(std::initializer_list<std::shared_ptr<Command>> list) {
        for (const auto &it: list) {
            _commands.push_back(it);
        }
    }

    bool run(RoverManagement &rover) override {
        for (const auto &_command: _commands) {
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


Rover::Rover(std::unordered_map<char, std::shared_ptr<Command>> &commands,
             std::vector<std::shared_ptr<Sensor>> &sensors) :
        _commands(commands),
        _sensors(sensors),
        _position(0, 0),
        _direction(Direction::NORTH),
        _land(false),
        _stop(false) {
}

void Rover::execute(const std::string &s) {
    if (!_land) {
        throw std::logic_error("Rover has not landed.");
    }

    auto rm = RoverManagement(*this);
    _stop = false;
    for (auto _command_name: s) {
        if (!_commands.contains(_command_name) ||
            !_commands[_command_name]->run(rm)) {
            _stop = true;
            return;
        }
    }
}

void Rover::land(Position position, Direction direction) {
    _land = true;
    _stop = false;
    _position = Position(position);
    _direction = direction;
}

bool RoverManagement::check_position(Position &to_check) {
    bool good = true;
    for (auto &s: _r._sensors) {
        good &= s->is_safe(to_check.get_x(), to_check.get_y());
    }
    return good;
}

bool RoverManagement::move(Position &new_position, Direction new_direction) {
    if (new_position == _r._position) {
        _r._direction = new_direction;
    } else {
        if (check_position(new_position))
            _r._position = new_position;
        else
            return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const Rover &rover) {
    if (!rover._land) {
        os << "unknown";
    } else {
        os << rover._position << " " << Direction_names[static_cast<int>(rover._direction)]
           << (rover._stop ? " stopped" : "");
    }
    return os;
}

Position &RoverManagement::get_position() {
    return _r._position;
}

int RoverManagement::get_direction() const {
    return static_cast<int>(_r._direction);
}

#endif //ROVER_ROVER_H