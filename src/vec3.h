#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <iostream>

using std::sqrt;

class vec3 {
public:
    [[nodiscard]] constexpr vec3() : data{0, 0, 0} {}
    [[nodiscard]] constexpr vec3(double x, double y, double z)
        : data{x, y, z} {}

    [[nodiscard]] constexpr double x() const { return data[0]; }
    [[nodiscard]] constexpr double y() const { return data[1]; }
    [[nodiscard]] constexpr double z() const { return data[2]; }

    constexpr vec3 &operator-() {
        data[0] = -data[0];
        data[1] = -data[1];
        data[2] = -data[2];
        return *this;
    }

    [[nodiscard]] constexpr vec3 operator-() const {
        return vec3(-data[0], -data[1], -data[2]);
    }
    [[nodiscard]] constexpr double operator[](std::size_t i) const {
        return data[i];
    }
    [[nodiscard]] constexpr double &operator[](std::size_t i) {
        return data[i];
    }

    constexpr vec3 &operator+=(const vec3 &v) {
        data[0] += v.data[0];
        data[1] += v.data[1];
        data[2] += v.data[2];
        return *this;
    }

    constexpr vec3 &operator*=(const double t) {
        data[0] *= t;
        data[1] *= t;
        data[2] *= t;
        return *this;
    }

    constexpr vec3 &operator/=(const double t) { return *this *= 1 / t; }

    [[nodiscard]] double length() const { return sqrt(length_squared()); }

    [[nodiscard]] constexpr double length_squared() const {
        return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
    }

    friend std::ostream &operator<<(std::ostream &out, const vec3 &v) {
        return out << v.data[0] << ' ' << v.data[1] << ' ' << v.data[2];
    }

    [[nodiscard]] constexpr friend vec3 operator+(const vec3 &u,
                                                  const vec3 &v) {
        return vec3(u.data[0] + v.data[0], u.data[1] + v.data[1],
                    u.data[2] + v.data[2]);
    }

    [[nodiscard]] constexpr friend vec3 operator-(const vec3 &u,
                                                  const vec3 &v) {
        return vec3(u.data[0] - v.data[0], u.data[1] - v.data[1],
                    u.data[2] - v.data[2]);
    }

    [[nodiscard]] constexpr friend vec3 operator*(const vec3 &u,
                                                  const vec3 &v) {
        return vec3(u.data[0] * v.data[0], u.data[1] * v.data[1],
                    u.data[2] * v.data[2]);
    }

    [[nodiscard]] constexpr friend vec3 operator*(double t, const vec3 &v) {
        return vec3(t * v.data[0], t * v.data[1], t * v.data[2]);
    }

    [[nodiscard]] constexpr friend vec3 operator*(const vec3 &v, double t) {
        return t * v;
    }

    [[nodiscard]] constexpr friend vec3 operator/(const vec3 &v, double t) {
        return (1 / t) * v;
    }

    [[nodiscard]] constexpr friend double dot(const vec3 &u, const vec3 &v) {
        return u.data[0] * v.data[0] + u.data[1] * v.data[1] +
               u.data[2] * v.data[2];
    }

    [[nodiscard]] constexpr friend vec3 cross(const vec3 &u, const vec3 &v) {
        return vec3(u.data[1] * v.data[2] - u.data[2] * v.data[1],
                    u.data[2] * v.data[0] - u.data[0] * v.data[2],
                    u.data[0] * v.data[1] - u.data[1] * v.data[0]);
    }

    [[nodiscard]] constexpr friend vec3 unit_vector(vec3 v) {
        if (std::is_constant_evaluated()) {
            // uses fast inverse sqrt for constant evaluated contexts
            float num = static_cast<float>(v.length_squared());
            constexpr auto threehalfs = 1.5F;
            const auto x2 = num * 0.5F;

            auto i = std::bit_cast<uint32_t>(num);
            constexpr auto magic_number = 0x5f3759df;
            i = magic_number - (i >> 1);
            float y = std::bit_cast<float>(i);

            y = y * (threehalfs - (x2 * y * y));
            y = y * (threehalfs - (x2 * y * y));
            return v * static_cast<double>(y);
        }
        return v / v.length();
    }

private:
    std::array<double, 3> data;
};

using point3 = vec3; // 3D point
using color = vec3;  // RGB color
