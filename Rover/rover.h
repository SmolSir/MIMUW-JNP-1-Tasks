#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>
#include <memory>

#include "positioning.h"

class Sensor;
class Command;
class MoveForward;
class MoveBackward;
class RotateRight;
class RotateLeft;
class Compose;
class RoverManagement;

class Rover {
public:
    Rover() = delete;

    void execute(const std::string &s);
    void land(Position position, Direction direction);

    friend std::ostream &operator<<(std::ostream &os, const Rover &rover);

    friend std::shared_ptr<MoveForward> move_forward();
    friend std::shared_ptr<MoveBackward> move_backward();
    friend std::shared_ptr<RotateRight> rotate_right();
    friend std::shared_ptr<RotateLeft> rotate_left();
    friend std::shared_ptr<Compose> compose(std::initializer_list<std::shared_ptr<Command>> list);

    friend class RoverBuilder;
    friend class RoverManagement;

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;

    Position _position;
    Direction _direction;

    bool _has_landed = false;
    bool _has_stopped = false;

    Rover(std::unordered_map<char, std::shared_ptr<Command>> &commands,
          std::vector<std::shared_ptr<Sensor>> &sensors);
};

class RoverManagement {
public:
    RoverManagement() = delete;

    bool move(Position &new_position, Direction new_direction);

    Position &get_position();
    int get_direction_int();
    Direction get_direction();

    friend class Rover;

private:
    Rover &_r;

    explicit RoverManagement(Rover &r) : _r(r) {}

    bool check_position(Position &to_check);
};


class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder &program_command(char c, const std::shared_ptr<Command> &command);
    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor);
    Rover build();

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;
};

class Sensor {
public:
    constexpr Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
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
        Position new_position = rover.get_position() + move_vector[rover.get_direction_int()];
        return rover.move(new_position, rover.get_direction());
    }
};


class MoveBackward : public Command {
public:
    MoveBackward() = default;

    bool run(RoverManagement &rover) override {
        Position new_position = rover.get_position() - move_vector[rover.get_direction_int()];
        return rover.move(new_position, rover.get_direction());
    }
};


class RotateRight : public Command {
public:
    RotateRight() = default;

    bool run(RoverManagement &rover) override {
        auto new_direction = Direction((rover.get_direction_int() + 1) % DIRECTION_NO);
        return rover.move(rover.get_position(), new_direction);
    }
};


class RotateLeft : public Command {
public:
    RotateLeft() = default;

    bool run(RoverManagement &rover) override {
        auto new_direction = Direction((rover.get_direction_int() + 3) % DIRECTION_NO);
        return rover.move(rover.get_position(), new_direction);
    }
};

class Compose : public Command {
public:
    Compose(std::initializer_list<std::shared_ptr<Command>> list) {
        for (const auto &it: list)
            _commands.emplace_back(it);
    }

    bool run(RoverManagement &rover) override {
        for (const auto &command: _commands)
            if (!command->run(rover))
                return false;

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
        _has_landed(false),
        _has_stopped(false) {
}

void Rover::execute(const std::string &s) {
    if (!_has_landed) {
        throw std::logic_error("Rover has not landed.");
    }

    auto rm = RoverManagement(*this);
    _has_stopped = false;

    for (auto _command_name: s) {
        if (!_commands.contains(_command_name) ||
            !_commands[_command_name]->run(rm)) {
            _has_stopped = true;
            return;
        }
    }
}

void Rover::land(Position position, Direction direction) {
    _has_landed = true;
    _has_stopped = false;
    _position = Position(position);
    _direction = direction;
}

std::ostream &operator<<(std::ostream &os, const Rover &rover) {
    if (!rover._has_landed) {
        os << "unknown";
    } else {
        os << rover._position << " " << Direction_names[static_cast<int>(rover._direction)]
           << (rover._has_stopped ? " stopped" : "");
    }
    return os;
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

Position &RoverManagement::get_position() {
    return _r._position;
}

int RoverManagement::get_direction_int() {
    return static_cast<int>(_r._direction);
}

Direction RoverManagement::get_direction() {
    return _r._direction;
}

RoverBuilder & RoverBuilder::program_command(char c, const std::shared_ptr<Command> &command) {
    _commands[c] = command;
    return *this;
}

RoverBuilder &RoverBuilder::add_sensor(std::unique_ptr<Sensor> sensor) {
    _sensors.emplace_back(std::move(sensor));
    return *this;
}

Rover RoverBuilder::build() {
    return Rover{_commands, _sensors};
}

#endif //ROVER_ROVER_H