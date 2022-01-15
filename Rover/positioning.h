#ifndef ROVER_POSITIONING_H
#define ROVER_POSITIONING_H

#define DIRECTION_NO 4

#include <ostream>

using coordinate_t = int32_t;

// North is zero, then incrementing in order of the wind rose clockwise.
// Notice that for move_back
enum class Direction : int {
    NORTH = 0,
    EAST  = 1,
    SOUTH = 2,
    WEST  = 3
};

// String aliases for our Direction codes.
static inline const std::string Direction_names[DIRECTION_NO] = {
        "NORTH",
        "EAST",
        "SOUTH",
        "WEST"
};

class Position {
public:
    Position() = delete;
    constexpr Position(coordinate_t x, coordinate_t y) noexcept : _x(x), _y(y) {}

    constexpr Position operator+ (const Position& rhs) const;
    constexpr Position operator- (const Position& rhs) const;
    constexpr coordinate_t get_x() const;
    constexpr coordinate_t get_y() const;

    friend std::ostream &operator<<(std::ostream &os, const Position &pos);

private:
    coordinate_t _x, _y;
};

// We will add and subtract these values to our rover's current position to move
// accordingly forward and backward, choosing the vector based of our direction code.
constinit Position move_vector[DIRECTION_NO] = {
        Position( 0,  1),
        Position( 1,  0),
        Position( 0, -1),
        Position(-1,  0)
};

std::ostream &operator<<(std::ostream &os, const Position &pos) {
    os << "(" << pos._x << ", " << pos._y << ")";
    return os;
}

constexpr Position Position::operator+ (const Position& rhs) const {
    auto new_position = Position(*this);
    new_position._x += rhs._x;
    new_position._y += rhs._y;
    return new_position;
}

constexpr Position Position::operator- (const Position& rhs) const {
    auto new_position = Position(*this);
    new_position._x -= rhs._x;
    new_position._y -= rhs._y;
    return new_position;
}

constexpr coordinate_t Position::get_x() const {
    return _x;
}

constexpr coordinate_t Position::get_y() const {
    return _y;
}

#endif //ROVER_POSITIONING_H
