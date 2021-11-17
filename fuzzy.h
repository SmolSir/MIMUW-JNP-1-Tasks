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
using Triple = tuple<real_t, real_t, real_t>;

class TriFuzzyNum {

private:
    real_t l;
    real_t m;
    real_t u;

    constexpr void sort_params();

    constexpr Triple rank() const;

    static constexpr int compare_params(real_t param1, real_t param2);

public:
    TriFuzzyNum() = delete; // default, 0 arg constructor
    ~TriFuzzyNum() = default; // destructor

    constexpr TriFuzzyNum(real_t l, real_t m, real_t u); // constructor
    constexpr TriFuzzyNum(const TFN& triFuzzyNum) = default; // copy constructor
    /*
     * should be consteval, but compiler does not support it
     */
    constexpr real_t lower_value() const;
    constexpr real_t modal_value() const;
    constexpr real_t upper_value() const;

    friend ostream& operator<< (ostream& os, const TriFuzzyNum& tfn);

    constexpr bool operator== (const TFN& other) const;

    constexpr TFN& operator+= (const TFN& rhs);
    constexpr TFN& operator-= (const TFN& rhs);
    constexpr TFN& operator*= (const TFN& rhs);

    constexpr TFN operator+ (const TFN& other) const;
    constexpr TFN operator- (const TFN& other) const;
    constexpr TFN operator* (const TFN& other) const;

    constexpr auto operator<=> (const TFN& other) const;
};


class TriFuzzyNumSet : public multiset<TFN> {
    using multiset<TFN>::multiset;
public:
    TriFuzzyNumSet() = default; // default, 0 arg constructor
    TriFuzzyNumSet (const TFNS& x) = default; // copy constructor
    TriFuzzyNumSet (TFNS&& x) = default; // move constructor
    ~TriFuzzyNumSet() = default; // destructor

    void remove (const TFN& val);
    TFN arithmetic_mean();
};

// should be consteval, neither g++-11.1.0 nor earlier versions do allow this
constexpr TFN crisp_number(real_t value);
const TFN crisp_zero = crisp_number(0.0);

constexpr void TFN::sort_params() {
    if (l > m) swap(l, m);
    if (m > u) swap(m, u);
    if (l > m) swap(l, m);
}

constexpr Triple TFN::rank() const {
    real_t z = (u - l) + sqrt(1 + (u - m) * (u - m)) + sqrt(1 + (m - l) * (m - l));
    real_t y = (u - l) / z;
    real_t x = ((u - l) * m + sqrt(1 + (u - m) * (u - m)) * l + sqrt(1 + (m - l) * (m - l)) * u) / z;

    return Triple{x - y / 2, 1 - y, m};
}

constexpr int TFN::compare_params(const real_t param1, const real_t param2) {
    return (param1 < param2 ? -1 : param1 == param2 ? 0 : 1);
}

constexpr TFN::TriFuzzyNum(const real_t l, const real_t m, const real_t u) :
l(l), m(m), u(u)
{
    sort_params();
}
/*
 * should be consteval, but the compiler does not support it
 */
constexpr real_t TFN::lower_value() const { return l; }
constexpr real_t TFN::modal_value() const { return m; }
constexpr real_t TFN::upper_value() const { return u; }

ostream& operator<< (ostream& os, const TFN& tfn) {
    os << "(" << tfn.l << ", " << tfn.m << ", " << tfn.u << ")";
    return os;
}

constexpr bool TFN::operator==(const TFN& other) const {
    return (l == other.l && m == other.m && u == other.u);
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


constexpr auto TFN::operator<=>(const TFN &other) const {
    Triple my_rank = rank();
    Triple other_rank = other.rank();

    int l_compare = compare_params(get<0>(my_rank), get<0>(other_rank));
    int m_compare = compare_params(get<1>(my_rank), get<1>(other_rank));
    int u_compare = compare_params(get<2>(my_rank), get<2>(other_rank));

    return  (l_compare == 0 ?
    (m_compare == 0 ?
    (u_compare == 0 ? 0 : u_compare)
    : m_compare)
    : l_compare);
}

void TFNS::remove(const TFN& val) {
    this->erase(val);
}

TFN TFNS::arithmetic_mean(){
    if (this->empty()) throw length_error("TriFuzzyNumSet::arithmetic_mean - the set is empty.");
    real_t l_out = 0, m_out = 0, u_out = 0;
    int n = 0;
    for (multiset<TFN>::iterator it = this->begin(); it != this->end(); ++it){
        l_out += it->lower_value();
        m_out += it->modal_value();
        u_out += it->upper_value();
        n++;
    }
    l_out /= n;
    m_out /= n;
    u_out /= n;
    return TriFuzzyNum(l_out, m_out, u_out);
}

constexpr TFN crisp_number(const real_t value) {
    return TFN{value, value, value};
}

#endif // __FUZZY_H__