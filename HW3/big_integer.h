#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iosfwd>
#include <vector>
#include <cstdint>
#include "shared_vector.h"

//set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
const uint32_t MX32 = UINT32_MAX;
const size_t block_size = 32;

struct big_integer
{
    big_integer();
    big_integer(int, const std::vector<uint32_t>&);
    big_integer(int , const SharedVector<uint32_t>&);
    big_integer(big_integer const& other) = default;
    big_integer(int a);
    big_integer(uint32_t a);
    explicit big_integer(std::string const& str);
    ~big_integer() = default;
    big_integer& operator=(big_integer const& other) = default;

    size_t size() const;
    uint32_t operator[](size_t);
    uint32_t operator[](size_t) const;
    uint32_t bit_digit(size_t) const;

    big_integer& operator+=(const big_integer&);
    friend big_integer operator+(const big_integer&, const big_integer&);
    big_integer& operator-=(const big_integer&);
    big_integer& operator*=(const big_integer&);
    friend big_integer operator*(const big_integer&, const big_integer&);
    friend big_integer operator-(const big_integer&, const big_integer&);
    friend bool operator==(const big_integer&, const big_integer&);
    friend bool operator!=(const big_integer&, const big_integer&);
    friend bool operator<(const big_integer&, const big_integer&);
    friend bool operator<=(const big_integer&, const big_integer&);
    friend bool operator>(const big_integer&, const big_integer&);
    friend bool operator>=(const big_integer&, const big_integer&);

    big_integer operator-() const;
    big_integer operator+() const;
    big_integer operator~() const;

    friend big_integer operator&(const big_integer&, const big_integer&);
    friend big_integer operator|(const big_integer&, const big_integer&);
    friend big_integer operator^(const big_integer&, const big_integer&);
    big_integer& operator&=(const big_integer&);
    big_integer& operator|=(const big_integer&);
    big_integer& operator^=(const big_integer&);

    big_integer& operator<<=(int);
    friend big_integer operator<<(big_integer, int);
    big_integer& operator>>=(int);
    friend big_integer operator>>(big_integer, int);
    big_integer& operator/=(const big_integer&);
    friend big_integer operator/(const big_integer&, const big_integer&);
    big_integer& operator%=(const big_integer&);
    friend big_integer operator%(const big_integer&, const big_integer&);
    friend std::string to_string(big_integer);
    big_integer abs_() const;

private:
    void clean();
    int compare_abs(const big_integer&) const;
    SharedVector<uint32_t> value;
    int sign;

    template <class F>
    friend big_integer binary_f(const big_integer &lhs, const big_integer &rhs, F f);

    big_integer to_bit() const;
    big_integer from_bit() const;
    big_integer short_divide(uint32_t) const;
    friend big_integer long_divide(const big_integer&, const big_integer&);
};

#endif // BIG_INTEGER_H
