#ifndef ROVER_COMMAND_H
#define ROVER_COMMAND_H

#include <initializer_list>
#include <memory>
#include <vector>

#include "rover_management.h"
#include "positioning.h"


class Command {
public:
    virtual ~Command() = default;

    virtual bool run(RoverManagement &rover) = 0; // operation failed - false, otherwise true
};

using command_ptr = std::shared_ptr<Command>;

class MoveForward : public Command {
public:
    MoveForward() = default;

    bool run(RoverManagement &rover) override {
        Position new_position = rover.get_position() + move_vector[rover.get_direction_int()];
        return rover.set_position_with_check(new_position);
    }
};

class MoveBackward : public Command {
public:
    MoveBackward() = default;

    bool run(RoverManagement &rover) override {
        Position new_position = rover.get_position() - move_vector[rover.get_direction_int()];
        return rover.set_position_with_check(new_position);
    }
};

class RotateRight : public Command {
public:
    RotateRight() = default;

    bool run(RoverManagement &rover) override {
        auto new_direction = Direction((rover.get_direction_int() + 1) % DIRECTION_NO);
        return rover.set_direction(new_direction);
    }
};

class RotateLeft : public Command {
public:
    RotateLeft() = default;

    bool run(RoverManagement &rover) override {
        auto new_direction = Direction((rover.get_direction_int() + 3) % DIRECTION_NO);
        return rover.set_direction(new_direction);
    }
};

class Compose : public Command {
public:
    Compose(std::initializer_list<command_ptr> list) {
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
    std::vector<command_ptr> _commands;
};

#endif //ROVER_COMMAND_H
