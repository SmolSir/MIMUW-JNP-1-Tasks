//
// Created by Bartosz Smolarczyk & Andrzej Sijka on 22.11.2021.
//

#ifndef TREASURE_H
#define TREASURE_H

#include <type_traits>

using namespace std;

template <class T, bool trap>
class Treasure {
private:
    T loot;
public:
    static const bool isTrapped = trap;

    Treasure() = delete;
    ~Treasure() = default;

    constexpr explicit Treasure(T value) : loot(value) {
        static_assert(is_integral<T>::value, "Integral value required.");
    };

    constexpr T evaluate() const;
    constexpr T getLoot();
};

template <class T>
class SafeTreasure : public Treasure<T, false> {
public:
    SafeTreasure() = delete;
    ~SafeTreasure() = default;

    constexpr explicit SafeTreasure(T value) : Treasure<T, false>(value) {}
};

template <class T>
class TrappedTreasure : public Treasure<T, true> {
public:
    TrappedTreasure() = delete;
    ~TrappedTreasure() = default;

    constexpr explicit TrappedTreasure(T value) : Treasure<T, true>(value) {}
};


template <class T, bool trap>
constexpr T Treasure<T, trap>::evaluate() const  {
    return loot;
}

template <class T, bool trap>
constexpr T Treasure<T, trap>::getLoot() {
    T temp = loot;
    loot = 0;
    return temp;
}


#endif // TREASURE_H
