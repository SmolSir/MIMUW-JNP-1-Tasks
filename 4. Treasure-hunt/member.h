//
// Created by Bartosz Smolarczyk & Andrzej Sijka on 22.11.2021.
//

#ifndef MEMBER_H
#define MEMBER_H

#include "treasure.h"
#include <typeinfo>
#include <array>

using strength_t = unsigned int;


static inline const int MAX_EXPEDITIONS = 25;
static inline consteval auto Fibonacci_array() {
    array<strength_t, MAX_EXPEDITIONS> arr{0};
    arr[1] = 1;
    for (int i = 2; i < MAX_EXPEDITIONS; i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }
    return arr;
}
static inline constexpr auto Fibonacci = Fibonacci_array();


template <class T, bool armed>
class Adventurer {
private:
    T total_loot = 0;
    strength_t strength;
public:
    static const bool isArmed = armed;

    Adventurer();
    ~Adventurer() = default;

    constexpr explicit Adventurer(strength_t);

    constexpr strength_t getStrength() const;

    template <class R, bool trap>
    constexpr void loot(Treasure<R, trap>&&);

    constexpr T pay();
};

template <class T>
class Explorer : public Adventurer<T, false> {};

template <class T, size_t completed_expeditions>
class Veteran {
private:
    T total_loot = 0;
    strength_t strength;
public:
    static const bool isArmed = true;

    constexpr Veteran();
    ~Veteran() = default;

    constexpr strength_t getStrength() const;

    template <class R, bool trap>
    constexpr void loot(Treasure<R, trap>&&);

    constexpr T pay();
};

/*
 * ADVENTURER CLASS IMPLEMENTATION
 */

template <class T, bool armed>
Adventurer<T, armed>::Adventurer() : strength(0) {
    static_assert(is_integral<T>::value, "Integral value required");
    static_assert(!armed, "Cannot create armed Adventurer with this constructor");
}

template <class T, bool armed>
constexpr Adventurer<T, armed>::Adventurer(strength_t strength) : strength(strength) {
    static_assert(is_integral<T>::value, "Integral value required");
    static_assert(armed, "Cannot create unarmed Adventurer with this constructor");
}

template <class T, bool armed>
constexpr strength_t Adventurer<T, armed>::getStrength() const {
    static_assert(armed, "Strength not defined for unarmed Adventurer");
    return strength;
}

template <class T, bool armed>
template <class R, bool trap>
constexpr void Adventurer<T, armed>::loot(Treasure<R, trap>&& treasure) {
    static_assert(typeid(T) == typeid(R), "Incompatible types of treasures");
    if (trap) {
        total_loot += strength ? treasure.getLoot() : 0;
        strength /= 2;
    } else {
        total_loot += treasure.getLoot();
    }
}

template <class T, bool armed>
constexpr T Adventurer<T, armed>::pay() {
    T temp = total_loot;
    total_loot = 0;
    return temp;
}

/*
 *  VETERAN CLASS IMPLEMENTATION
 */

template <class T, size_t completed_expeditions>
constexpr Veteran<T, completed_expeditions>::Veteran() {
    static_assert(is_integral<T>::value, "Integral value required");
    static_assert(completed_expeditions < 25, "Too many completed expeditions");
    strength = Fibonacci[completed_expeditions];
}

template <class T, size_t completed_expeditions>
constexpr strength_t Veteran<T, completed_expeditions>::getStrength() const {
    return strength;
}

template <class T, size_t completed_expeditions>
template <class R, bool trap>
constexpr void Veteran<T, completed_expeditions>::loot(Treasure<R, trap>&& treasure) {
    static_assert(typeid(T) == typeid(R), "Incompatible types of treasures");
    total_loot += treasure.getLoot();
}

template <class T, size_t completed_expeditions>
constexpr T Veteran<T, completed_expeditions>::pay() {
    T temp = total_loot;
    total_loot = 0;
    return temp;
}

#endif // MEMBER_H
