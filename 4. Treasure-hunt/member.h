//
// Created by Bartosz Smolarczyk & Andrzej Sijka on 22.11.2021.
//

#ifndef MEMBER_H
#define MEMBER_H

#include "treasure.h"
#include <typeinfo>

using strength_t = unsigned int;

template <class T, bool armed>
class Adventurer {
private:
    T total_loot = 0;
    strength_t strength;
public:
    const bool isArmed = armed;

    Adventurer();
    ~Adventurer() = delete;

    constexpr explicit Adventurer(strength_t);

    constexpr strength_t getStrength() const;

    template<class R, bool trap>
    constexpr void loot(Treasure<R, trap>&&);

    constexpr T pay() const;
};

template <class T, size_t>
class Veteran {

};

template <class T, bool armed>
Adventurer<T, armed>::Adventurer() : strength(0) {
    static_assert(is_integral<T>::value, "Integral value required.");
    static_assert(armed, "Cannot create armed Adventurer with this constructor");
}

template <class T, bool armed>
constexpr Adventurer<T, armed>::Adventurer(strength_t strength) : strength(strength) {
    static_assert(is_integral<T>::value, "Integral value required.");
    static_assert(!armed, "Cannot create unarmed Adventurer with this constructor");
}

template <class T, bool armed>
constexpr strength_t Adventurer<T, armed>::getStrength() const {
    static_assert(!armed, "Strength not defined for unarmed Adventurer");
    return strength;
}

template <class T, bool armed>
template <class R, bool trap>
constexpr void Adventurer<T, armed>::loot(Treasure<R, trap>&& treasure) {
    static_assert(typeid(T) == typeid(R), "Cannot loot this type of treasure");
    if (trap) {
        total_loot += strength ? treasure.getLoot() : 0;
        strength /= 2;
    } else {
        total_loot += treasure.getLoot();
    }
}

template <class T, bool armed>
constexpr T Adventurer<T, armed>::pay() const {
    T temp = total_loot;
    total_loot = 0;
    return temp;
}

#endif // MEMBER_H
