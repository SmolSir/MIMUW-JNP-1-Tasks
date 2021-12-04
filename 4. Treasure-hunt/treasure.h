#ifndef JNP_ZADANIE4_TREASURE_H
#define JNP_ZADANIE4_TREASURE_H

#include <type_traits>
#include <concepts>
#include <cstdint>

template<class T>
concept Integral = std::is_integral<T>::value;

template<Integral ValueType, bool IsTrapped>
class Treasure {
private:
    ValueType treasure;
public:

    constexpr static bool isTrapped = IsTrapped;

    constexpr Treasure(ValueType value) : treasure(value) {};

    constexpr const ValueType& evaluate() const { return treasure; };

    constexpr ValueType getLoot() {
        ValueType res = treasure;
        treasure = 0;
        return res;
    };
};

template<Integral ValueType>
using SafeTreasure = Treasure<ValueType, false>;

template<Integral ValueType>
using TrappedTreasure = Treasure<ValueType, true>;

#endif //JNP_ZADANIE4_TREASURE_H