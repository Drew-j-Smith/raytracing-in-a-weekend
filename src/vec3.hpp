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

    [[nodiscard]] constexpr bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        constexpr auto s = 1e-8;
        if (std::is_constant_evaluated()) {
            constexpr auto val_near_zero = [](double num) {
                if (num > 0) {
                    return num < s;
                }
                return num > -s;
            };
            return val_near_zero(data[0]) && val_near_zero(data[1]) &&
                   val_near_zero(data[2]);
        }
        return (fabs(data[0]) < s) && (fabs(data[1]) < s) &&
               (fabs(data[2]) < s);
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

    [[nodiscard]] inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    [[nodiscard]] inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max),
                    random_double(min, max));
    }

private:
    std::array<double, 3> data;
};

[[nodiscard]] inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) {
            continue;
        }
        return p;
    }
}

[[nodiscard]] inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

[[nodiscard]] vec3 random_in_hemisphere(const vec3 &normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) {
        // In the same hemisphere as the normal
        return in_unit_sphere;
    }
    return -in_unit_sphere;
}

[[nodiscard]] constexpr vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n) * n;
}

[[nodiscard]] inline vec3 refract(const vec3 &uv, const vec3 &n,
                                  double refraction_ratio) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = refraction_ratio * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

[[nodiscard]] inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) {
            continue;
        }
        return p;
    }
}

using point3 = vec3; // 3D point
using color = vec3;  // RGB color
