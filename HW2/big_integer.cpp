#include <utility>

#include "big_integer.h"

#include <cstring>
#include <stdexcept>
#include <limits>
#include <algorithm>

big_integer::big_integer() {
    value = {0};
    sign = 1;
}

big_integer::big_integer(int a) : sign(a < 0 ? -1 : 1) {
    value = {(uint32_t)(llabs(a))};
}

big_integer::big_integer(uint32_t a) : value{a}, sign(1) {}

big_integer::big_integer(int s, std::vector<uint32_t> v) {
    sign = s;
    value = std::move(v);
    this->clean();
}

big_integer::big_integer(std::string const &str) : value{0}, sign(1) {
    if (str.empty()) {
        return;
    }
    size_t ind = 0;
    int s = 1;
    if (str[0] == '-') {
        s = -1;
        ++ind;
    }
    big_integer ans(0);
    while (ind < str.size()) {
        ans *= 10;
        ans += (str[ind] - '0');
        ++ind;
    }
    ans.sign = s;
    *this = ans;
    clean();
}

big_integer big_integer::abs_() const {
    big_integer ans = *this;
    ans.sign = 1;
    return ans;
}

uint32_t big_integer::operator[](size_t i) {
    return (i >= size()) ? 0 : value[i];
}

uint32_t big_integer::operator[](size_t i) const {
    return (i >= size()) ? 0 : value[i];
}

size_t big_integer::size() const {
    return this->value.size();

}

void big_integer::clean() {
    while (size() > 0 && value.back() == 0) {
        value.pop_back();
    }
    if (size() == 0) {
        sign = 1;
        value = {0};
    }
}

int big_integer::compare_abs(const big_integer &a) const {
    if (size() != a.size()) {
        return (size() > a.size() ? 1 : -1);
    }
    for (size_t i = size(); i-- > 0;) {
        if (value[i] != a[i]) {
            return (value[i] > a[i] ? 1 : -1);
        }
    }
    return 0;

}

///comparing
bool operator==(const big_integer &a, const big_integer &b) {
    return (a.compare_abs(b) == 0 && a.sign == b.sign);
}

bool operator!=(const big_integer &a, const big_integer &b) {
    return !(a == b);
}

bool operator<(const big_integer &a, const big_integer &b) {
    if (a.sign != b.sign) {
        return (a.sign < b.sign);
    }
    //int x = a.compare_abs(b);
    return (a.sign == 1) ? (a.compare_abs(b) == -1) : (a.compare_abs(b) == 1);
}

bool operator>(const big_integer &a, const big_integer &b) {
    if (a.sign != b.sign) {
        return (a.sign > b.sign);
    }
    return (a.sign == 1) ? (a.compare_abs(b) == 1) : (a.compare_abs(b) == -1);
}

bool operator<=(const big_integer &a, const big_integer &b) {
    return !(a > b);
}

bool operator>=(const big_integer &a, const big_integer &b) {
    return !(a < b);
}

///unary
big_integer big_integer::operator+() const {
    return (*this);
}

big_integer big_integer::operator-() const {
    big_integer ans(*this);
    ans.sign *= -1;
    ans.clean();
    return ans;
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

///add
big_integer &big_integer::operator+=(const big_integer &x) {
    (*this) = *this + x;
    return *this;
}

big_integer operator+(const big_integer &a, const big_integer &b) {
    if (a.sign != b.sign) {
        return a - (-b);
    }
    int carry = 0;
    size_t newsz = std::max(a.size(), b.size());
    uint64_t sum = 0;
    std::vector<uint32_t> res(newsz, 0);
    for (size_t i = 0; i < newsz || carry; ++i) {
        if (i == res.size()) {
            res.push_back(0);
        }
        sum = (uint64_t)(a[i]) + b[i] + carry;
        carry = (sum > MX32);
        res[i] = (uint32_t)(sum & MX32);
    }
    big_integer ans(a.sign, res);
    res.clear();
    return ans;
}

///end add
///sub
big_integer &big_integer::operator-=(const big_integer &x) {
    (*this) = *this - x;
    return *this;
}

big_integer operator-(const big_integer &a, const big_integer &b) {
    if (a.sign != b.sign) {
        return a + (-b);
    }
    if (a.compare_abs(b) == -1) {
        return -(b - a);
    }
    int carry = 0;
    std::vector<uint32_t> res = a.value;
    uint64_t sub = 0;
    for (size_t i = 0; i < a.size(); i++) {
        sub = (uint64_t)(b[i]) + carry;
        carry = (sub > res[i]);
        res[i] = carry ? (uint32_t)((sub - res[i] - 1) ^ MX32) : res[i] - (uint32_t)sub;
    }
    return big_integer(a.sign, res);
}
///end sub

///mul
big_integer &big_integer::operator*=(const big_integer &a) {
    (*this) = *this * a;
    return *this;
}

big_integer operator*(const big_integer &a, const big_integer &b) {
    std::vector<uint32_t> x(a.size() + b.size() + 1, 0);
    for (size_t i = 0; i < a.size(); i++) {
        uint32_t carry = 0;
        uint64_t cur;
        for (size_t j = 0; j < b.size() || carry; ++j) {
            cur = (uint64_t) (a[i]) * b[j] + x[i + j] + carry;
            x[i + j] = (uint32_t) (cur & MX32);
            carry = (uint32_t) (cur >> (32)); //in bits
        }
    }
    big_integer ans(a.sign * b.sign, x);
    x.clear();
    return ans;
}
///endmul

///bitsimp
big_integer big_integer::to_bit() const {
    if (sign == 1) {
        return *this;
    }
    big_integer ans = (*this + 1);
    if (ans == 0) {
        ans.sign = -1;
    }
    return ans;
}

big_integer big_integer::from_bit() const {
    if (sign == 1) {
        return *this;
    }
    std::vector<uint32_t> inv(value);
    for (uint32_t &i : inv) {
        i = ~i;
    }
    return big_integer(-1, inv) - 1;
}

uint32_t big_integer::bit_digit(size_t i) const {
    if (sign == 1) {
        return ((i < size()) ? value[i] : 0);
    }
    return ((i < size()) ? value[i] ^ MX32 : MX32);
}

template<class F>
big_integer binary_f(const big_integer &a, const big_integer &b, F f) {
    size_t sz = std::max(a.size(), b.size());
    std::vector<uint32_t> res(sz, 0);
    big_integer a_bit(a.to_bit());
    big_integer b_bit(b.to_bit());
    for (size_t i = 0; i < sz; ++i) {
        res[i] = f(a_bit.bit_digit(i), b_bit.bit_digit(i));
    }
    bool a_neg = (a.sign == -1);
    bool b_neg = (b.sign == -1);
    int s = (bool) (f((uint32_t) a_neg, (uint32_t) b_neg)) ? -1 : 1;
    big_integer ans = big_integer(s, res).from_bit();
    ans.clean();
    return ans;
}

big_integer operator&(const big_integer &a, const big_integer &b) {
    return binary_f(a, b, [](uint32_t x, uint32_t y) -> uint32_t { return x & y; });
}

big_integer &big_integer::operator&=(const big_integer &x) {
    (*this) = *this & x;
    return *this;
}

big_integer operator|(const big_integer &a, const big_integer &b) {
    return binary_f(a, b, [](uint32_t x, uint32_t y) -> uint32_t { return x | y; });
}

big_integer &big_integer::operator|=(const big_integer &x) {
    (*this) = *this | x;
    return *this;
}

big_integer operator^(const big_integer &a, const big_integer &b) {
    return binary_f(a, b, [](uint32_t x, uint32_t y) -> uint32_t { return x ^ y; });
}

big_integer &big_integer::operator^=(const big_integer &x) {
    (*this) = *this ^ x;
    return *this;
}
///end bitsimp

big_integer &big_integer::operator<<=(int d) {
    (*this) = *this << d;
    return *this;
}

big_integer &big_integer::operator>>=(int d) {
    (*this) = *this >> d;
    return *this;
}

big_integer operator<<(big_integer a, int d) {
    if (d < 0) {
        return a >> (-d);
    }
    size_t block_size = 32;
    size_t delta = d / block_size;
    size_t move = d % block_size;
    big_integer a_bit = a.to_bit();
    size_t newsz = a.size() + delta + 1;
    std::vector<uint32_t> x(newsz, 0);
    for (size_t i = 0; i < a.size(); ++i) {
        if (move) {
            uint32_t rest = a_bit.bit_digit(i) >> (block_size - move);
            x[i + delta + 1] += rest;
        }
        x[i + delta] += a_bit.bit_digit(i) << move;
    }
    if (a.sign == -1) {
        x[newsz - 1] += (MX32 << move);
    }
    big_integer ans(a.sign, x);
    ans = ans.from_bit();
    ans.clean();
    x.clear();
    return ans;
}

big_integer operator>>(big_integer a, int d) {
    if (d < 0) {
        return a << (-d);
    }
    size_t block_size = 32;
    size_t delta = d / block_size;
    size_t move = d % block_size;
    big_integer a_bit = a.to_bit();
    std::vector<uint32_t> x(a.size() - delta, 0);
    for (size_t i = 0; i < x.size(); ++i) {
        if (move) {
            uint32_t rest = a_bit.bit_digit(i + delta + 1) << (block_size - move);
            x[i] += rest;
        }
        x[i] += a_bit.bit_digit(i + delta) >> move;
    }
    big_integer ans(a.sign, x);
    ans = ans.from_bit();
    ans.clean();
    x.clear();
    return ans;
}
///endshifts
///div
big_integer big_integer::short_divide(uint32_t a) const {
    uint64_t rest = 0, cur = 0;
    uint32_t cur_digit = 0;
    std::vector<uint32_t> x;
    size_t block_size = 32;
    for (size_t i = size(); i-- > 0;) {
        cur = (rest << block_size) + value[i];
        cur_digit = (uint32_t)(cur / a);
        x.push_back(cur_digit);
        rest = cur % a;
    }
    reverse(x.begin(), x.end());
    big_integer ans(1, x);
    x.clear();
    ans.clean();
    return ans;
}

big_integer long_divide(const big_integer &a, const big_integer &b) {
    big_integer res(0);
    int nsign = a.sign * b.sign;
    if (a.abs_() < b.abs_()) {
        res = 0;
    } else if (a.abs_() == b.abs_()) {
        res = big_integer(a.sign * b.sign, {1});
    } else if (b.size() == 1) {
        res = big_integer(a.short_divide(b[0]));
        res.sign = nsign;
    } else {
        big_integer A(a.abs_()), B(b.abs_());
        A.clean(); B.clean();
        int block_size = 32;
        uint32_t norm = (uint32_t) ((1LL << block_size) / ((uint64_t) B[B.size() - 1] + 1LL));
        A *= norm;
        B *= norm; //normalization

        size_t n = B.size();
        size_t m = A.size() - n;
        std::vector<uint32_t> ans;
        big_integer Bm = (B << (m * block_size));
        if (Bm <= A) {
            ans.push_back(1);
            A -= Bm;
        }
        big_integer Bn_1(1, {B[n - 1]});
        for (size_t i = m; i-- > 0;) {
            Bm >>= block_size;
            big_integer q(1, {A[n + i - 1], A[n + i]});
            q /= Bn_1;
            if (q.size() > 1) {
                q.value = {MX32};
                q.sign = 1;
            }
            A -= Bm * q;
            while (A < 0) {
                q -= 1;
                A += Bm;
            }
            if (!(q.value.empty())) {
                ans.push_back(q[0]);
            } else {
                ans.push_back(0);
            }
        }
        reverse(ans.begin(), ans.end());
        res = big_integer(a.sign * b.sign, ans);
        ans.clear();
    }
    res.clean();
    return res;
}
big_integer &big_integer::operator/=(const big_integer & a) {
    *this = *this / a;
    return *this;
}

big_integer operator/(const big_integer &a, const big_integer &b) {
    big_integer res(0);
    res = long_divide(a, b);
    return res;
}

big_integer &big_integer::operator%=(const big_integer & a) {
    *this = *this % a;
    return *this;
}

big_integer operator%(const big_integer &a, const big_integer &b) {
    big_integer res(0);
    res = a - b * long_divide(a, b);
    return res;
}

std::string to_string(big_integer a) {
    a.clean();
    if (a == 0) {
        return "0";
    }
    std::string res;
    big_integer curr = a.abs_(), digit;
    while (curr > 0) {
        digit = curr % 10;
        if (digit == 0) {
            res.push_back('0');
        } else {
            res.push_back((char)('0' + digit[0]));
        }
        curr /= 10;
    }
    if (a.sign == -1) {
        res.push_back('-');
    }
    reverse(res.begin(), res.end());
    return res;
}

///enddiv












