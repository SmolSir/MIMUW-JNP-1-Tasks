//
// Created by Bartosz Smolarczyk on 09.11.2021.
//

#ifndef __FUZZY_H__
#define __FUZZY_H__

#include "bits/stdc++.h"
using namespace std;

typedef double real_t;

class TriFuzzyNum;
class TriFuzzyNumSet;
using TFN = TriFuzzyNum;
using TFNS = TriFuzzyNumSet;

class TriFuzzyNum {

private:
    real_t l;
    real_t m;
    real_t u;

    constexpr void sort_params();

    constexpr TFN rank() const;

    static constexpr int compare_params(real_t param1, real_t param2);

public:
    TriFuzzyNum() = delete; // default, 0 arg constructor
    ~TriFuzzyNum() = default; // destructor

    constexpr TriFuzzyNum(real_t l, real_t m, real_t u); // constructor
    constexpr TriFuzzyNum(TFN& triFuzzyNum) = default; // copy constructor

    consteval real_t lower_value() const;
    consteval real_t modal_value() const;
    consteval real_t upper_value() const;

    friend ostream& operator<< (ostream& os, const TriFuzzyNum& tfn);

    constexpr bool operator== (const TFN& other) const;
    constexpr bool operator!= (const TFN& other) const;

    constexpr TFN& operator+= (const TFN& rhs);
    constexpr TFN& operator-= (const TFN& rhs);
    constexpr TFN& operator*= (const TFN& rhs);

    constexpr TFN operator+ (const TFN& other) const;
    constexpr TFN operator- (const TFN& other) const;
    constexpr TFN operator* (const TFN& other) const;

    constexpr int operator<=> (const TFN& other) const;

    constexpr bool operator<  (const TFN& other) const;
    constexpr bool operator<= (const TFN& other) const;
    constexpr bool operator>= (const TFN& other) const;
    constexpr bool operator>  (const TFN& other) const;
};

class TriFuzzyNumSet {
    /*
     * only declarations of functions go here -
     * their definitions will be in the commented space further down.
     * use TFN and TFNS in every possible place to make the code clean :)
     */
private:
    /*
     * private members
     */

public:
    /*
     * public members, this time we need a move constructor
     */

};

// should be consteval, neither g++-11.1.0 nor earlier versions do allow this
constexpr TFN crisp_number(real_t value);
const TFN crisp_zero = crisp_number(0.0);

constexpr void TFN::sort_params() {
    if (l > m) swap(l, m);
    if (m > u) swap(m, u);
    if (l > m) swap(l, m);
}

constexpr TFN TFN::rank() const {
    real_t z = (u - l) + sqrt(1 + (u - m) * (u - m)) + sqrt(1 + (m - l) * (m - l));
    real_t y = (u - l) / z;
    real_t x = ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l + sqrt(1 + (m - l) * (m - l)) * u) / z;

    return TFN{x - y / 2, 1 - y, m};
}

constexpr int TFN::compare_params(const real_t param1, const real_t param2) {
    return (param1 < param2 ? -1 : param1 == param2 ? 0 : 1);
}

constexpr TFN::TriFuzzyNum(const real_t l, const real_t m, const real_t u) :
    l(l), m(m), u(u)
    {
    sort_params();
}

consteval real_t TFN::lower_value() const { return l; }
consteval real_t TFN::modal_value() const { return m; }
consteval real_t TFN::upper_value() const { return u; }

ostream& operator<< (ostream& os, const TFN& tfn) {
    os << "(" << tfn.l << ", " << tfn.m << ", " << tfn.u << ")";
    return os;
}

constexpr bool TFN::operator==(const TFN& other) const {
    return (l == other.l && m == other.m && u == other.u);
}

constexpr bool TFN::operator!=(const TFN& other) const {
    return !(*this == other);
}

constexpr TFN& TFN::operator+=(const TFN& rhs) {
    l += rhs.l;
    m += rhs.m;
    u += rhs.u;

    sort_params();
    return *this;
}

constexpr TFN& TFN::operator-=(const TFN& rhs) {
    l -= rhs.u;
    m -= rhs.m;
    u -= rhs.l;

    sort_params();
    return *this;
}

constexpr TFN& TFN::operator*=(const TFN& rhs) {
    l *= rhs.l;
    m *= rhs.m;
    u *= rhs.u;

    sort_params();
    return *this;
}

constexpr TFN TFN::operator+(const TFN& other) const {
    return TFN(l, m, u) += other;
}

constexpr TFN TFN::operator-(const TFN& other) const {
    return TFN(l, m, u) -= other;
}

constexpr TFN TFN::operator*(const TFN& other) const {
    return TFN(l, m, u) *= other;
}


constexpr int TFN::operator<=>(const TFN &other) const {
    TFN my_rank = rank();
    TFN other_rank = other.rank();
    int l_compare = compare_params(my_rank.l, other_rank.l);
    int m_compare = compare_params(my_rank.m, other_rank.m);
    int u_compare = compare_params(my_rank.u, other_rank.u);

    return  (l_compare == 0 ?
                (m_compare == 0 ?
                    (u_compare == 0 ? 0 : u_compare)
                : m_compare)
            : l_compare);
}

constexpr bool TFN::operator<(const TFN &other) const {
    return (*this <=> other) < 0;
}

constexpr bool TFN::operator<=(const TFN &other) const {
    return (*this <=> other) <= 0;
}

constexpr bool TFN::operator>=(const TFN &other) const {
    return (*this <=> other) >= 0;
}

constexpr bool TFN::operator>(const TFN &other) const {
    return (*this <=> other) > 0;
}

/*
 * definitions of TFNS functions go here
 */

constexpr TFN crisp_number(const real_t value) {
    return TFN{value, value, value};
}

#endif // __FUZZY_H__
