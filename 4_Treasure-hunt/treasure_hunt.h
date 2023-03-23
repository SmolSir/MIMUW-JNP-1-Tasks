#ifndef JNP_ZADANIE4_TREASURE_HUNT_H
#define JNP_ZADANIE4_TREASURE_HUNT_H

#include "member.h"
#include <type_traits>
#include <concepts>

template <class T>
struct is_mytype : std::false_type {};

template <std::integral A, bool B>
struct is_mytype<Treasure<A, B>> : std::true_type {};

template <class T>
concept TreasureType = is_mytype<T>::value;

template<class T>
concept ExpeditionMember = requires(T x) {
    typename T::strength_t;
    { [] () constexpr { return T::isArmed; } () };
    {T::isArmed} -> std::convertible_to<bool>;
    {x.pay()} -> std::integral;
    x.loot(Treasure<decltype(x.pay()), true>(0));
    x.loot(Treasure<decltype(x.pay()), false>(0));
};

template<class T>
concept EncounterSide = TreasureType<T> || ExpeditionMember<T>;

template<EncounterSide A, EncounterSide B>
class Encounter {
public:
    A &ref_a;
    B &ref_b;
};

template<ExpeditionMember A, ExpeditionMember B>
constexpr void run(Encounter<A, B> x) {
    if constexpr (x.ref_a.isArmed == true && x.ref_b.isArmed == false) {
    //    x.ref_a.loot(SafeTreasure{x.ref_b.pay()});
        SafeTreasure<decltype(x.ref_b.pay())> temp(x.ref_b.pay());
        x.ref_a.loot(std::forward<decltype(temp)>(temp));
    }

    else if constexpr (x.ref_a.isArmed == false && x.ref_b.isArmed == true) {
    //    x.ref_b.loot(SafeTreasure{x.ref_a.pay()});
        SafeTreasure<decltype(x.ref_a.pay())> temp(x.ref_a.pay());
        x.ref_b.loot(std::forward<decltype(temp)>(temp));
    }

    else if constexpr (x.ref_a.isArmed && x.ref_b.isArmed) {

        if (x.ref_a.getStrength() > x.ref_b.getStrength()) {
        //    x.ref_a.loot(SafeTreasure{x.ref_b.pay()});
            SafeTreasure<decltype(x.ref_b.pay())> temp(x.ref_b.pay());
            x.ref_a.loot(std::forward<decltype(temp)>(temp));
        }

        if (x.ref_a.getStrength() < x.ref_b.getStrength()) {
        //    x.ref_b.loot(SafeTreasure{x.ref_a.pay()});
            SafeTreasure<decltype(x.ref_a.pay())> temp(x.ref_a.pay());
            x.ref_b.loot(std::forward<decltype(temp)>(temp));
        }
    }
}

template<ExpeditionMember A, TreasureType B>
constexpr void run(Encounter<A, B> x) {
    x.ref_a.loot(std::forward<B>(x.ref_b));
}

template<TreasureType A, ExpeditionMember B>
constexpr void run(Encounter<A, B> x) {
    x.ref_b.loot(std::forward<A>(x.ref_a));
}

template<typename ...Es>
constexpr void expedition(Es... args) {
    (run(args), ...);
}

#endif //JNP_ZADANIE4_TREASURE_HUNT_H
