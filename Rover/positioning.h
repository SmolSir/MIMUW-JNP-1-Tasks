#ifndef ROVER_POSITIONING_H
#define ROVER_POSITIONING_H

#define DIRECTION_CNT 4

#include <ostream>

using coordinate_t = int;

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

class Position {
public:
    Position() = default;
    Position(coordinate_t x, coordinate_t y) : _x(x), _y(y) {}

    friend std::ostream &operator<<(std::ostream &os, const Position &pos) {
        os << "(" << pos._x << ", " << pos._y << ")";
        return os;
    }

    constexpr Position operator+ (const Position& rhs) {
        return Position(*this) += rhs;
    }

    constexpr Position operator- (const Position& rhs) {
        return Position(*this) -= rhs;
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

// We will add and subtract these values to our rover's current position to move
// accordingly forward and backward, choosing the vector based of our direction code.
static inline const Position move_vector[DIRECTION_CNT] = {
        Position( 0,  1),
        Position( 1,  0),
        Position( 0, -1),
        Position(-1,  0)
};

#endif //ROVER_POSITIONING_H
