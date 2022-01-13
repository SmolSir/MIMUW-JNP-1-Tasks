//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "cert-err58-cpp"
#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>
#include <memory>

#include "positioning.h"


class Sensor {
public:
    Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};


class Command;

class Rover {
public:
    Rover() = default;
    Rover(std::unordered_map<char, std::shared_ptr<Command>> &commands,
          std::vector<std::shared_ptr<Sensor>> &sensors);

    void execute(const std::string &s);

    void land(std::pair<coordinate_t, coordinate_t> coordinates, Direction direction);

    bool check_position(Position &to_check);

    friend std::ostream &operator<<(std::ostream &os, const Rover &rover);

    Position _position;
    int _direction;

private:
    std::unordered_map<char, std::shared_ptr<Command>> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;
    bool _land = false;
    bool _stop = false;
};


class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder& program_command(char c, const std::shared_ptr<Command>& command) {
        _commands[c] = command;
        return *this;
    }

    RoverBuilder& add_sensor(std::unique_ptr<Sensor> &&sensor) {
        std::shared_ptr<Sensor> temp = std::move(sensor);
        _sensors.push_back(temp);
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
    virtual bool run(Rover &rover) = 0; // operation failed - false, otherwise true
};


class MoveForward : public Command {
public:
    MoveForward() = default;

    bool run(Rover &rover) override {
        Position new_position = rover._position + move_vector[rover._direction];
        if (!rover.check_position(new_position))
            return false;
        rover._position = new_position;
        return true;
    }
};


class MoveBackward : public Command {
public:
    MoveBackward() = default;
    bool run(Rover &rover) override {
        Position new_position = rover._position - move_vector[rover._direction];
        if (!rover.check_position(new_position))
            return false;
        rover._position = new_position;
        return true;
    }
};


class RotateRight : public Command {
public:
    RotateRight() = default;
    bool run(Rover &rover) override {
        rover._direction = (rover._direction + 1) % 4;
        return true;
    }
};


class RotateLeft : public Command {
public:
    RotateLeft() = default;
    bool run(Rover &rover) override {
        rover._direction = (rover._direction + 3) % 4; // + 4 - 1 = 3
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

    bool run(Rover &rover) override {
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


Rover::Rover(std::unordered_map<char, std::shared_ptr<Command>> &commands,
             std::vector<std::shared_ptr<Sensor>> &sensors)
{
    _commands = commands;
    _sensors = sensors;
    _land = false;
    _stop = false;
}

void Rover::execute(const std::string &s) {
    if (!_land) {
        throw std::logic_error("Rover has not landed.");
    }

    _stop = false;
    for (auto _command_name : s) {
        if (!_commands.contains(_command_name) ||
            !_commands[_command_name]->run(*this))
        {
            _stop = true;
            return;
        }
    }
}

void Rover::land(std::pair<coordinate_t, coordinate_t> coordinates, Direction direction) {
    _land = true;
    _position = Position(coordinates.first, coordinates.second);
    _direction = static_cast<int>(direction);
}

bool Rover::check_position(Position &to_check) {
    bool good = true;
    for (auto &s : _sensors) {
        good &= s->is_safe(to_check._x, to_check._y);
    }
    return good;
}

std::ostream &operator<<(std::ostream &os, const Rover &rover) {
    if (!rover._land) {
        os << "unknown";
    }
    else {
        os << rover._position << " " << Direction_names[rover._direction]
           << (rover._stop ? " stopped" : "");
    }
    return os;
}

#endif //ROVER_ROVER_H

//#pragma clang diagnostic pop