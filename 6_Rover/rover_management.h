#ifndef ROVER_ROVER_MANAGEMENT_H
#define ROVER_ROVER_MANAGEMENT_H

#include <memory>
#include <vector>

#include "positioning.h"
#include "sensor.h"

class RoverManagement {
public:
    RoverManagement() = delete;

    bool check_position(Position &to_check);

    void set_position(Position &new_position);
    bool set_position_with_check(Position &new_position);
    bool set_direction(Direction &new_direction);

    constexpr Position get_position() const;
    constexpr int get_direction_int() const;

    friend class Rover;

private:
    std::vector<std::shared_ptr<Sensor>> _sensors;
    Position _position;
    Direction _direction;

    explicit RoverManagement(std::vector<std::shared_ptr<Sensor>> &sensors);
};

RoverManagement::RoverManagement(std::vector<std::shared_ptr<Sensor>> &sensors) :
        _sensors(sensors),
        _position({0, 0}),
        _direction(Direction::NORTH) {}

bool RoverManagement::check_position(Position &to_check) {
    for (auto &s: _sensors)
        if (!s->is_safe(to_check.get_x(), to_check.get_y()))
            return false;

    return true;
}

constexpr Position RoverManagement::get_position() const {
    return _position;
}

constexpr int RoverManagement::get_direction_int() const {
    return static_cast<int>(_direction);
}

void RoverManagement::set_position(Position &new_position) {
    _position = new_position;
}

bool RoverManagement::set_position_with_check(Position &new_position) {
    if (!check_position(new_position))
        return false;

    _position = new_position;
    return true;
}

bool RoverManagement::set_direction(Direction &new_direction) {
    _direction = new_direction;
    return true;
}

#endif //ROVER_ROVER_MANAGEMENT_H
