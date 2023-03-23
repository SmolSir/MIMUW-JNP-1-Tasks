#ifndef JNP_ZADANIE4_MEMBER_H
#define JNP_ZADANIE4_MEMBER_H

#include "treasure.h"
#include <cstdint>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <array>

using completed_expedition_t = size_t;

constexpr const int MAX_EXPEDITIONS = 25;
/*
constexpr auto Fibonacci_array() {
    std::array<uint32_t, MAX_EXPEDITIONS> arr{0};
    arr[1] = 1;
    for (int i = 2; i < MAX_EXPEDITIONS; i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }
    return arr;
}
constexpr auto Fibonacci = Fibonacci_array();
*/
constexpr uint32_t Fibonacci[MAX_EXPEDITIONS] =
        {0, 1, 1, 2, 3, 5, 8, 13, 21,
         34, 55, 89, 144, 233, 377, 610, 987,
         1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368};

template<Integral ValueType, bool armed>
class Adventurer {
private:
    ValueType collected_loot = 0;
public:

    using strength_t = uint32_t;
    strength_t strength;

    constexpr Adventurer() requires(armed == false) {strength = 0;};

    constexpr Adventurer(ValueType value) requires(armed == true) {strength = value;};

    constexpr static bool isArmed = armed;

    constexpr strength_t getStrength() const requires(armed == true) {return strength;};

    template<bool IsTrapped>
    constexpr void loot(Treasure<ValueType, IsTrapped> && treasure) {
        if (IsTrapped) {
            if (isArmed && strength > 0) {
                strength /= 2;
                int new_loot = treasure.getLoot();
                collected_loot += new_loot;
            }
        } else {
            int new_loot = treasure.getLoot();
            collected_loot += new_loot;
        }
    };

    constexpr ValueType pay() {
        ValueType res = collected_loot;
        collected_loot = 0;
        return res;
    };
};

template<class ValueType>
using Explorer = Adventurer<ValueType, false>;

template<Integral ValueType, completed_expedition_t completed_expeditions>
class Veteran {
private:
    completed_expedition_t total_expedition;
    ValueType collected_loot = 0;
public:

    using strength_t = uint32_t;
    strength_t strength;

    constexpr Veteran() requires(completed_expeditions < 25) {
        collected_loot = 0;
        strength = Fibonacci[completed_expeditions];
        total_expedition = completed_expeditions;
    };

    constexpr static bool isArmed = true;

    template<bool IsTrapped>
    constexpr void loot(Treasure<ValueType, IsTrapped> && treasure) {
        int new_loot = treasure.getLoot();
        collected_loot += new_loot;
    };

    constexpr ValueType pay() {
        ValueType res = collected_loot;
        collected_loot = 0;
        return res;
    };

    constexpr strength_t getStrength() const {return strength;};
};

#endif //JNP_ZADANIE4_MEMBER_H
