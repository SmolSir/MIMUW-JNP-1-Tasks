#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

#include <initializer_list>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "command.h"
#include "positioning.h"
#include "rover_management.h"
#include "sensor.h"

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

private:
    std::unordered_map<char, command_ptr> _commands;
    bool _has_landed = false;
    bool _has_stopped = false;
    RoverManagement rover_management;

    Rover(std::unordered_map<char, command_ptr> &commands,
          std::vector<std::shared_ptr<Sensor>> &sensors);
};

class RoverBuilder {
public:
    RoverBuilder() = default;

    RoverBuilder &program_command(char c, command_ptr command);
    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor);
    Rover build();

private:
    std::unordered_map<char, command_ptr> _commands;
    std::vector<std::shared_ptr<Sensor>> _sensors;
};

Rover::Rover(std::unordered_map<char, command_ptr> &commands,
             std::vector<std::shared_ptr<Sensor>> &sensors) :
        _commands(commands),
        _has_landed(false),
        _has_stopped(false),
        rover_management(sensors) {}

void Rover::execute(const std::string &s) {
    if (!_has_landed) {
        throw std::logic_error("Rover has not landed.");
    }

    _has_stopped = false;

    for (auto _command_name: s) {
        if (!_commands.contains(_command_name) ||
            !_commands[_command_name]->run(rover_management)) {
            _has_stopped = true;
            return;
        }
    }
}

void Rover::land(Position position, Direction direction) {
    _has_landed = true;
    _has_stopped = false;
    rover_management.set_position(position);
    rover_management.set_direction(direction);
}

std::ostream &operator<<(std::ostream &os, const Rover &rover) {
    if (!rover._has_landed) {
        os << "unknown";
    } else {
        os << rover.rover_management.get_position() << " " << Direction_names[rover.rover_management.get_direction_int()]
           << (rover._has_stopped ? " stopped" : "");
    }

    return os;
}

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

std::shared_ptr<Compose> compose(std::initializer_list<command_ptr> list) {
    return std::make_shared<Compose>(list);
}

RoverBuilder &RoverBuilder::program_command(char c, command_ptr command) {
    _commands[c] = std::move(command);
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