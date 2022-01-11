#ifndef ROVER_ROVER_H
#define ROVER_ROVER_H

using coordinate_t = int;

class Sensor {
    virtual bool is_safe([[maybe_unused]] coordinate_t x,
                         [[maybe_unused]] coordinate_t y) = 0;
};

#endif //ROVER_ROVER_H
