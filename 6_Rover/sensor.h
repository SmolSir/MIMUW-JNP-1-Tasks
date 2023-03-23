#ifndef ROVER_SENSOR_H
#define ROVER_SENSOR_H

#include "positioning.h"

class Sensor {
public:
    constexpr Sensor() = default;
    virtual ~Sensor() = default;

    virtual bool is_safe(coordinate_t x, coordinate_t y) = 0;
};

#endif //ROVER_SENSOR_H
