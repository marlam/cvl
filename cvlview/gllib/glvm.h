/*
 * glvm.h
 * 
 * C++ vector and matrix classes that resemble GLSL style.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * These are vector and matrix classes that resemble the GLSL types vec2, vec3,
 * vec4, mat2, mat3, mat4 (and the variants bvec, ivec, dvec, dmat).
 *
 * Vector elements are called (x,y,z,w) and (r,g,b,a) and (s,t,p,q). Swizzling
 * is possible in the limits set by C++: single elements can be accessed 
 * using .x, .r, .s etc., everything else must be accessed read-only using 
 * member functions .xy(), .rgb() etc.
 *
 * All data elements are accessible via an array v. For matrices, the array v is
 * two-dimensional, and an additional linear array vl exists. Both v and vl are
 * row-major, unlike OpenGL, but like everyone else. Use transpose() when
 * reading OpenGL data (or use the functions from glvm-gl.h).
 *
 * Everything that is specified by GLSL 1.20 should work, unless it is
 * impossible to implement in C++.
 */

#ifndef GLVM_H
#define GLVM_H

#include <cmath>
#include <cstdlib>
#include <cfloat>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

namespace glvm
{
    template<typename T>
    inline T sin(const T x)
    {
	return ::sin(x);
    }

    template<>
    inline float sin(const float x)
    {
	return ::sinf(x);
    }

    template<typename T>
    inline T cos(const T x)
    {
	return ::cos(x);
    }

    template<>
    inline float cos(const float x)
    {
	return ::cosf(x);
    }

    template<typename T>
    inline T tan(const T x)
    {
	return ::tan(x);
    }

    template<>
    inline float tan(const float x)
    {
	return ::tanf(x);
    }

    template<typename T>
    inline T asin(const T x)
    {
	return ::asin(x);
    }

    template<>
    inline float asin(const float x)
    {
	return ::asinf(x);
    }

    template<typename T>
    inline T acos(const T x)
    {
	return ::acos(x);
    }

    template<>
    inline float acos(const float x)
    {
	return ::acosf(x);
    }

    template<typename T>
    inline T atan(const T x)
    {
	return ::atan(x);
    }

    template<>
    inline float atan(const float x)
    {
	return ::atanf(x);
    }

    template<typename T>
    inline T atan(const T x, const T y)
    {
	return ::atan2(x, y);
    }

    template<>
    inline float atan(const float x, const float y)
    {
	return ::atan2f(x, y);
    }

    template<typename T>
    inline T radians(const T x)
    {
	return x * static_cast<T>(M_PI / 180.0);
    }

    template<typename T>
    inline T degrees(const T x)
    {
	return x * static_cast<T>(180.0 / M_PI);
    }

    template<typename T>
    inline T pow(const T x, const T p)
    {
	return ::pow(x, p);
    }

    template<>
    inline float pow(const float x, const float p)
    {
	return ::powf(x, p);
    }

    template<typename T>
    inline T exp(const T x)
    {
	return ::exp(x);
    }

    template<>
    inline float exp(const float x)
    {
	return ::expf(x);
    }

    template<typename T>
    inline T exp2(const T x)
    {
	return ::exp2(x);
    }

    template<>
    inline float exp2(const float x)
    {
	return ::exp2f(x);
    }

    template<typename T>
    inline T log(const T x)
    {
	return ::log(x);
    }

    template<>
    inline float log(const float x)
    {
	return ::logf(x);
    }

    template<typename T>
    inline T log2(const T x)
    {
	return ::log2(x);
    }

    template<>
    inline float log2(const float x)
    {
	return ::log2f(x);
    }

    template<typename T>
    inline T log10(const T x)
    {
	return ::log10(x);
    }

    template<>
    inline float log10(const float x)
    {
	return ::log10f(x);
    }

    template<typename T>
    inline T sqrt(const T x)
    {
	return ::sqrt(x);
    }

    template<>
    inline float sqrt(const float x)
    {
	return ::sqrtf(x);
    }

    template<typename T>
    inline T inversesqrt(const T x)
    {
	return static_cast<T>(1) / glvm::sqrt(x);
    }

    template<typename T>
    inline T cbrt(const T x)
    {
	return ::cbrt(x);
    }

    template<>
    inline float cbrt(const float x)
    {
	return ::cbrtf(x);
    }

    template<typename T>
    inline bool isfinite(const T x)
    {
	return static_cast<bool>(std::isfinite(x));
    }

    template<typename T>
    inline bool isinf(const T x)
    {
	return static_cast<bool>(std::isinf(x));
    }

    template<typename T>
    inline bool isnan(const T x)
    {
	return static_cast<bool>(std::isnan(x));
    }

    template<typename T>
    inline bool isnormal(const T x)
    {
	return static_cast<bool>(std::isnormal(x));
    }

    template<typename T>
    inline T abs(const T x)
    {
	return ::fabs(x);
    }

    template<>
    inline float abs(const float x)
    {
	return ::fabsf(x);
    }

    template<>
    inline int abs(const int x)
    {
	return ::abs(x);
    }

    template<typename T>
    inline T sign(const T x)
    {
	return (x < static_cast<T>(0) ? static_cast<T>(-1) : x > static_cast<T>(0) ? static_cast<T>(+1) : static_cast<T>(0));
    }

    template<typename T>
    inline T floor(const T x)
    {
	return ::floor(x);
    }

    template<>
    inline float floor(const float x)
    {
	return ::floorf(x);
    }

    template<>
    inline int floor(const int x)
    {
	return x;
    }

    template<typename T>
    inline T ceil(const T x)
    {
	return ::ceil(x);
    }

    template<>
    inline float ceil(const float x)
    {
	return ::ceilf(x);
    }

    template<>
    inline int ceil(const int x)
    {
	return x;
    }

    template<typename T>
    inline T round(const T x)
    {
	return ::round(x);
    }

    template<>
    inline float round(const float x)
    {
	return ::roundf(x);
    }

    template<>
    inline int round(const int x)
    {
	return x;
    }

    template<typename T>
    inline T fract(const T x)
    {
	return x - glvm::floor(x);
    }

    template<typename T>
    inline T min(const T x, const T y)
    {
	return (x < y ? x : y);
    }

    template<typename T>
    inline T max(const T x, const T y)
    {
	return (x > y ? x : y);
    }

    template<typename T>
    inline T clamp(const T x, const T minval, const T maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    template<typename T>
    inline T mix(const T x, const T y, const T alpha)
    {
	return x * (static_cast<T>(1) - alpha) + y * alpha;
    }

    template<typename T>
    inline T step(const T x, const T edge)
    {
	return (x < edge ? static_cast<T>(0) : static_cast<T>(1));
    }

    template<typename T>
    inline T smoothstep(const T x, const T edge0, const T edge1)
    {
	const T t = glvm::clamp((x - edge0) / (edge1 - edge0), static_cast<T>(0), static_cast<T>(1));
	return t * t * (static_cast<T>(3) - t * static_cast<T>(2));
    }

    template<typename T>
    inline T mod(const T x, const T y)
    {
	return x - glvm::floor(x / y) * y;
    }


    template<typename T> class vector2
    {
	public:

	union
	{
	    struct
	    {
		T x, y;
	    };
	    struct
	    {
		T r, g;
	    };
	    struct
	    {
		T s, t;
	    };
	    T v[2];
	};

	/* Constructors, Destructor */

	vector2() {}

	vector2(const T x)
	{
	    this->x = x;
	    this->y = x;
	}

	vector2(const T x, const T y)
	{
	    this->x = x;
	    this->y = y;
	}

	vector2(const T *xy)
	{
	    this->x = xy[0];
	    this->y = xy[1];
	}

	template<typename U>
	vector2(const vector2<U> &v)
	{
	    this->x = v.x;
	    this->y = v.y;
	}

	vector2(const std::string &s)
	{
	    std::istringstream is(s);
	    is >> x >> y;
	}

	~vector2() {}

	/* "Swizzling" (as far as possible in C++) */

	vector2 xx() const
	{
	    return vector2(x, x);
	}

	vector2 xy() const
	{
	    return vector2(x, y);
	}

	vector2 yx() const
	{
	    return vector2(y, x);
	}

	vector2 yy() const
	{
	    return vector2(y, y);
	}

	vector2 rr() const
	{
	    return vector2(r, r);
	}

	vector2 rg() const
	{
	    return vector2(r, g);
	}

	vector2 gr() const
	{
	    return vector2(g, r);
	}

	vector2 gg() const
	{
	    return vector2(g, g);
	}

	vector2 ss() const
	{
	    return vector2(s, s);
	}

	vector2 st() const
	{
	    return vector2(s, t);
	}

	vector2 ts() const
	{
	    return vector2(t, s);
	}

	vector2 tt() const
	{
	    return vector2(t, t);
	}

	/* Operators */

	const vector2 &operator=(const vector2 &v)
	{
	    x = v.x;
	    y = v.y;
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return v[i];
	}

	const T &operator[](const size_t i) const
	{
	    return v[i];
	}
	
	vector2 operator*(const T s) const
	{
	    return vector2(x * s, y * s);
	}

    	friend vector2 operator*(const T s, const vector2 &w)
	{
	    return w * s;
	}

	vector2 operator/(const T s) const
	{
	    return vector2(x / s, y / s);
	}
     
	const vector2 &operator*=(const T s)
	{
	    x *= s;
	    y *= s;
	    return *this;
	}
     
	const vector2 &operator/=(const T s)
	{
	    x /= s;
	    y /= s;
	    return *this;
	}

	vector2 operator+(const vector2 &v) const
	{
	    return vector2(x + v.x, y + v.y);
	}

	vector2 operator-(const vector2 &v) const
	{
	    return vector2(x - v.x, y - v.y);
	}

	vector2 operator*(const vector2 &v) const 
	{
	    return vector2(x * v.x, y * v.y);
	}

	vector2 operator/(const vector2 &v) const 
	{
	    return vector2(x / v.x, y / v.y);
	}

	vector2 operator-() const
	{
	    return vector2(-x, -y);
	}

	const vector2 &operator+=(const vector2 &v)
	{
	    x += v.x;
	    y += v.y;
	    return *this;
	}

	const vector2 &operator-=(const vector2 &v)
	{
	    x -= v.x;
	    y -= v.y;
	    return *this;
	}

	const vector2 &operator*=(const vector2 &v)
	{
	    x *= v.x;
	    y *= v.y;
	    return *this;
	}

	const vector2 &operator/=(const vector2 &v)
	{
	    x /= v.x;
	    y /= v.y;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const vector2 &v)
	{
	    return os << v.str();
	}

	friend std::istream &operator>>(std::istream &is, vector2 &v)
	{
	    is >> v.x >> v.y;
	    return is;
	}

	/* Trigonometric functions */

	vector2 sin() const
	{
	    return vector2(glvm::sin(x), glvm::sin(y));
	}

	friend vector2 sin(const vector2 &v)
	{
    	    return v.sin();
       	}

	vector2 cos() const
	{
	    return vector2(glvm::cos(x), glvm::cos(y));
	}

	friend vector2 cos(const vector2 &v)
	{
    	    return v.cos();
       	}

	vector2 tan() const
	{
	    return vector2(glvm::tan(x), glvm::tan(y));
	}

	friend vector2 tan(const vector2 &v)
	{
    	    return v.tan();
       	}

	vector2 asin() const
	{
	    return vector2(glvm::asin(x), glvm::asin(y));
	}

	friend vector2 asin(const vector2 &v)
	{
    	    return v.asin();
       	}

	vector2 acos() const
	{
	    return vector2(glvm::acos(x), glvm::acos(y));
	}

	friend vector2 acos(const vector2 &v)
	{
    	    return v.acos();
       	}

	vector2 atan() const
	{
	    return vector2(glvm::atan(x), glvm::atan(y));
	}

	friend vector2 atan(const vector2 &v)
	{
    	    return v.atan();
       	}

	vector2 atan(const vector2 &v) const
	{
	    return vector2(glvm::atan(x, v.x), glvm::atan(y, v.y));
	}

	friend vector2 atan(const vector2 &v, const vector2 &w)
	{
    	    return v.atan(w);
       	}

	vector2 radians() const
	{
	    return *this * static_cast<T>(M_PI / 180.0);
	}

	friend vector2 radians(const vector2 &v)
	{
    	    return v.radians();
       	}

	vector2 degrees() const
	{
	    return *this * static_cast<T>(180.0 / M_PI);
	}

	friend vector2 degrees(const vector2 &v)
	{
    	    return v.degrees();
       	}

	/* Exponential functions */

	vector2 pow(const T p) const
	{
	    return vector2(glvm::pow(x, p), glvm::pow(y, p));
	}

	friend vector2 pow(const vector2 &v, const T p)
	{
    	    return v.pow(p);
       	}

	vector2 exp() const
	{
	    return vector2(glvm::exp(x), glvm::exp(y));
	}

	friend vector2 exp(const vector2 &v)
	{
    	    return v.exp();
       	}

	vector2 exp2() const
	{
	    return vector2(glvm::exp2(x), glvm::exp2(y));
	}

	friend vector2 exp2(const vector2 &v)
	{
    	    return v.exp2();
       	}

	vector2 log() const
	{
	    return vector2(glvm::log(x), glvm::log(y));
	}

	friend vector2 log(const vector2 &v)
	{
    	    return v.log();
       	}

	vector2 log2() const
	{
	    return vector2(glvm::log2(x), glvm::log2(y));
	}

	friend vector2 log2(const vector2 &v)
	{
    	    return v.log2();
       	}

	vector2 log10() const
	{
	    return vector2(glvm::log10(x), glvm::log10(y));
	}

	friend vector2 log10(const vector2 &v)
	{
    	    return v.log10();
       	}

	vector2 sqrt() const
	{
	    return vector2(glvm::sqrt(x), glvm::sqrt(y));
	}

	friend vector2 sqrt(const vector2 &v)
	{
    	    return v.sqrt();
       	}

	vector2 inversesqrt() const
	{
	    return static_cast<T>(1) / *this.sqrt();
	}

	friend vector2 inversesqrt(const vector2 &v)
	{
    	    return v.inversesqrt();
       	}

	vector2 cbrt() const
	{
	    return vector2(glvm::cbrt(x), glvm::cbrt(y));
	}

	friend vector2 cbrt(const vector2 &v)
	{
    	    return v.cbrt();
       	}

	/* Common functions */

	vector2<bool> isfinite() const
	{
	    return vector2<bool>(glvm::isfinite(x), glvm::isfinite(y));
	}

	friend vector2<bool> isfinite(const vector2 &v)
	{
	    return v.isfinite();
	}

	vector2<bool> isinf() const
	{
	    return vector2<bool>(glvm::isinf(x), glvm::isinf(y));
	}

	friend vector2<bool> isinf(const vector2 &v)
	{
	    return v.isinf();
	}

	vector2<bool> isnan() const
	{
	    return vector2<bool>(glvm::isnan(x), glvm::isnan(y));
	}

	friend vector2<bool> isnan(const vector2 &v)
	{
	    return v.isnan();
	}

	vector2<bool> isnormal() const
	{
	    return vector2<bool>(glvm::isnormal(x), glvm::isnormal(y));
	}

	friend vector2<bool> isnormal(const vector2 &v)
	{
	    return v.isnormal();
	}

	vector2 abs() const
	{
	    return vector2(glvm::abs(x), glvm::abs(y));
	}

	friend vector2 abs(const vector2 &v)
	{
    	    return v.abs();
       	}

	vector2 sign() const
	{
	    return vector2(glvm::sign(x), glvm::sign(y));
	}

	friend vector2 sign(const vector2 &v)
	{
    	    return v.sign();
       	}

	vector2 floor() const
	{
	    return vector2(glvm::floor(x), glvm::floor(y));
	}

	friend vector2 floor(const vector2 &v)
	{
    	    return v.floor();
       	}

	vector2 ceil() const
	{
	    return vector2(glvm::ceil(x), glvm::ceil(y));
	}

	friend vector2 ceil(const vector2 &v)
	{
    	    return v.ceil();
       	}

	vector2 round() const
	{
	    return vector2(glvm::round(x), glvm::round(y));
	}

	friend vector2 round(const vector2 &v)
	{
    	    return v.round();
       	}

	vector2 fract() const
	{
	    return *this - this->floor();
	}

	friend vector2 fract(const vector2 &v)
	{
    	    return v.fract();
       	}

	vector2 min(const T v) const
	{
	    return vector2(v < x ? v : x, v < y ? v : y);
	}

	friend vector2 min(const vector2 &v, const T w)
	{
    	    return v.min(w);
       	}

	vector2 min(const vector2 &v) const
	{
	    return vector2(v.x < x ? v.x : x, v.y < y ? v.y : y);
	}

	friend vector2 min(const vector2 &v, const vector2 &w)
	{
    	    return v.min(w);
       	}

	vector2 max(const T v) const
	{
	    return vector2(v > x ? v : x, v > y ? v : y);
	}

	friend vector2 max(const vector2 &v, const T w)
	{
    	    return v.max(w);
       	}

	vector2 max(const vector2 &v) const
	{
	    return vector2(v.x > x ? v.x : x, v.y > y ? v.y : y);
	}

	friend vector2 max(const vector2 &v, const vector2 &w)
	{
    	    return v.max(w);
       	}

	vector2 clamp(const T minval, const T maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector2 clamp(const vector2 &v, const T minval, const T maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector2 clamp(const vector2 &minval, const vector2 &maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector2 clamp(const vector2 &v, const vector2 &minval, const vector2 &maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector2 mix(const vector2 &v, const T alpha) const
	{
	    return *this * (static_cast<T>(1) - alpha) + v * alpha;
	}

	friend vector2 mix(const vector2 &v, const vector2 &w, const T alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector2 mix(const vector2 &v, const vector2 &alpha) const
	{
	    return (vector2(static_cast<T>(1)) - alpha) * *this + alpha * v;
	}

	friend vector2 mix(const vector2 &v, const vector2 &w, const vector2 &alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector2 step(const T edge) const
	{
	    return vector2(
		    x < edge ? static_cast<T>(0) : static_cast<T>(1), 
		    y < edge ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector2 step(const vector2 &v, const T edge)
	{
    	    return v.step(edge);
       	}

	vector2 step(const vector2 &edge) const
	{
	    return vector2(
		    x < edge.x ? static_cast<T>(0) : static_cast<T>(1),
		    y < edge.y ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector2 step(const vector2 &v, const vector2 &edge)
	{
    	    return v.step(edge);
       	}

	vector2 smoothstep(const T edge0, const T edge1) const
	{
	    vector2 t = (*this - vector2(edge0)) / (vector2(edge1) - vector2(edge0));
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector2(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector2 smoothstep(const vector2 &v, const T edge0, const T edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector2 smoothstep(const vector2 &edge0, const vector2 &edge1) const
	{
	    vector2 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector2(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector2 smoothstep(const vector2 &v, const vector2 &edge0, const vector2 &edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector2 mod(const T y) const
	{
	    vector2 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector2 mod(const vector2 &v, const T y)
	{
    	    return v.mod(y);
       	}

	vector2 mod(const vector2 &y) const
	{
	    vector2 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector2 mod(const vector2 &v, const vector2 &y)
	{
    	    return v.mod(y);
       	}

	/* Geometric functions */

	T length() const
	{
	    return glvm::sqrt(x * x + y * y);
	}

	friend T length(const vector2 &v)
	{
    	    return v.length();
       	}

	T distance(const vector2 &v) const
	{
	    return vector2(*this - v).length();
	}

	friend T distance(const vector2 &v, const vector2 &w)
	{
    	    return v.distance(w);
       	}

	T dot(const vector2 &v) const
	{
	    return x * v.x + y * v.y;
	}

	friend T dot(const vector2 &v, const vector2 &w)
	{
    	    return v.dot(w);
       	}

	vector2 normalize() const
	{
	    return *this / this->length();
	}

	friend vector2 normalize(const vector2 &v)
	{
    	    return v.normalize();
       	}

	vector2 faceforward(const vector2 &I, const vector2 &Nref) const
	{
	    return Nref.dot(I) < static_cast<T>(0) ? *this : - *this;
	}

	friend vector2 faceforward(const vector2 &N, const vector2 &I, const vector2 &Nref)
	{
    	    return N.faceforward(I, Nref);
       	}

	vector2 reflect(const vector2 &N) const
	{
	    return *this - N * (N.dot(*this) * static_cast<T>(2));
	}

	friend vector2 reflect(const vector2 &I, const vector2 &N)
	{
    	    return I.reflect(N);
       	}

	vector2 refract(const vector2 &N, T eta) const
	{
	    const T d = N.dot(*this);
	    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
	    return k < static_cast<T>(0) ? vector2<T>(static_cast<T>(0)) : *this * eta - N * (eta * d + glvm::sqrt(k));
	}

	friend vector2 refract(const vector2 &I, const vector2 &N, T eta)
	{
    	    return I.refract(N, eta);
       	}

	/* Comparison functions */

	vector2<bool> greaterThan(const vector2 &v) const
	{
	    return vector2<bool>(x > v.x, y > v.y);
	}

	friend vector2<bool> greaterThan(const vector2 &v, const vector2 &w)
	{
    	    return v.greaterThan(w);
       	}

	vector2<bool> greaterThanEqual(const vector2 &v) const
	{
	    return vector2<bool>(x >= v.x, y >= v.y);
	}

	friend vector2<bool> greaterThanEqual(const vector2 &v, const vector2 &w)
	{
    	    return v.greaterThanEqual(w);
       	}

	vector2<bool> lessThan(const vector2 &v) const
	{
	    return vector2<bool>(x < v.x, y < v.y);
	}

	friend vector2<bool> lessThan(const vector2 &v, const vector2 &w)
	{
    	    return v.lessThan(w);
       	}

	vector2<bool> lessThanEqual(const vector2 &v) const
	{
	    return vector2<bool>(x <= v.x, y <= v.y);
	}

	friend vector2<bool> lessThanEqual(const vector2 &v, const vector2 &w)
	{
    	    return v.lessThanEqual(w);
       	}

	vector2<bool> equal(const vector2 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return (*this - v).abs().lessThan(vector2(epsilon));
	}

	friend vector2<bool> equal(const vector2 &v, const vector2 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.equal(w, epsilon);
       	}

	vector2<bool> notEqual(const vector2 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	friend vector2<bool> notEqual(const vector2 &v, const vector2 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.notEqual(w, epsilon);
       	}

	bool any() const
	{
	    return x || y;
	}

	bool all() const
	{
	    return x && y;
	}

	vector2<bool> negate() const
	{
	    return vector2<bool>(!x, !y);
	}
    };

    template<typename T>
    inline std::string vector2<T>::str() const
    {
	std::ostringstream os;
	os << x << " " << y << " ";
	return os.str();
    }

    template<>
    inline std::string vector2<double>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	os << x << " " << y << " ";
	return os.str();
    }

    template<>
    inline std::string vector2<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	os << x << " " << y << " ";
	return os.str();
    }


    template<typename T> class vector3
    {
	public:

	union
	{
	    struct
	    {
		T x, y, z;
	    };
	    struct
	    {
		T r, g, b;
	    };
	    struct
	    {
		T s, t, p;
	    };
	    T v[3];
	};

	/* Constructors, Destructor */

	vector3() {}

	vector3(const T x)
	{
	    this->x = x;
	    this->y = x;
	    this->z = x;
	}

	vector3(const T x, const T y, const T z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	vector3(const vector2<T> xy, T z)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	}

	vector3(const T x, const vector2<T> yz)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	}

	vector3(const T *xyz)
	{
	    this->x = xyz[0];
	    this->y = xyz[1];
	    this->z = xyz[2];
	}

	template<typename U>
	vector3(const vector3<U> &v)
	{
	    this->x = v.x;
	    this->y = v.y;
	    this->z = v.z;
	}

	vector3(const std::string &s)
	{
	    std::istringstream is(s);
	    is >> x >> y >> z;
	}

	~vector3() {}

	/* "Swizzling" (as far as possible in C++) */

	vector2<T> xx() const
	{
	    return vector2<T>(x, x);
	}

	vector2<T> xy() const
	{
	    return vector2<T>(x, y);
	}

	vector2<T> xz() const
	{
	    return vector2<T>(x, z);
	}

	vector2<T> yx() const
	{
	    return vector2<T>(y, x);
	}

	vector2<T> yy() const
	{
	    return vector2<T>(y, y);
	}

	vector2<T> yz() const
	{
	    return vector2<T>(y, z);
	}

	vector2<T> zx() const
	{
	    return vector2<T>(z, x);
	}

	vector2<T> zy() const
	{
	    return vector2<T>(z, y);
	}

	vector2<T> zz() const
	{
	    return vector2<T>(z, z);
	}

	vector3 xxx() const
	{
	    return vector3(x, x, x);
	}

	vector3 xxy() const
	{
	    return vector3(x, x, y);
	}

	vector3 xxz() const
	{
	    return vector3(x, x, z);
	}

	vector3 xyx() const
	{
	    return vector3(x, y, x);
	}

	vector3 xyy() const
	{
	    return vector3(x, y, y);
	}

	vector3 xyz() const
	{
	    return vector3(x, y, z);
	}

	vector3 xzx() const
	{
	    return vector3(x, z, x);
	}

	vector3 xzy() const
	{
	    return vector3(x, z, y);
	}

	vector3 xzz() const
	{
	    return vector3(x, z, z);
	}

	vector3 yxx() const
	{
	    return vector3(y, x, x);
	}

	vector3 yxy() const
	{
	    return vector3(y, x, y);
	}

	vector3 yxz() const
	{
	    return vector3(y, x, z);
	}

	vector3 yyx() const
	{
	    return vector3(y, y, x);
	}

	vector3 yyy() const
	{
	    return vector3(y, y, y);
	}

	vector3 yyz() const
	{
	    return vector3(y, y, z);
	}

	vector3 yzx() const
	{
	    return vector3(y, z, x);
	}

	vector3 yzy() const
	{
	    return vector3(y, z, y);
	}

	vector3 yzz() const
	{
	    return vector3(y, z, z);
	}

	vector3 zxx() const
	{
	    return vector3(z, x, x);
	}

	vector3 zxy() const
	{
	    return vector3(z, x, y);
	}

	vector3 zxz() const
	{
	    return vector3(z, x, z);
	}

	vector3 zyx() const
	{
	    return vector3(z, y, x);
	}

	vector3 zyy() const
	{
	    return vector3(z, y, y);
	}

	vector3 zyz() const
	{
	    return vector3(z, y, z);
	}

	vector3 zzx() const
	{
	    return vector3(z, z, x);
	}

	vector3 zzy() const
	{
	    return vector3(z, z, y);
	}

	vector3 zzz() const
	{
	    return vector3(z, z, z);
	}

	vector2<T> rr() const
	{
	    return vector2<T>(r, r);
	}

	vector2<T> rg() const
	{
	    return vector2<T>(r, g);
	}

	vector2<T> rb() const
	{
	    return vector2<T>(r, b);
	}

	vector2<T> gr() const
	{
	    return vector2<T>(g, r);
	}

	vector2<T> gg() const
	{
	    return vector2<T>(g, g);
	}

	vector2<T> gb() const
	{
	    return vector2<T>(g, b);
	}

	vector2<T> br() const
	{
	    return vector2<T>(b, r);
	}

	vector2<T> bg() const
	{
	    return vector2<T>(b, g);
	}

	vector2<T> bb() const
	{
	    return vector2<T>(b, b);
	}

	vector3 rrr() const
	{
	    return vector3(r, r, r);
	}

	vector3 rrg() const
	{
	    return vector3(r, r, g);
	}

	vector3 rrb() const
	{
	    return vector3(r, r, b);
	}

	vector3 rgr() const
	{
	    return vector3(r, g, r);
	}

	vector3 rgg() const
	{
	    return vector3(r, g, g);
	}

	vector3 rgb() const
	{
	    return vector3(r, g, b);
	}

	vector3 rbr() const
	{
	    return vector3(r, b, r);
	}

	vector3 rbg() const
	{
	    return vector3(r, b, g);
	}

	vector3 rbb() const
	{
	    return vector3(r, b, b);
	}

	vector3 grr() const
	{
	    return vector3(g, r, r);
	}

	vector3 grg() const
	{
	    return vector3(g, r, g);
	}

	vector3 grb() const
	{
	    return vector3(g, r, b);
	}

	vector3 ggr() const
	{
	    return vector3(g, g, r);
	}

	vector3 ggg() const
	{
	    return vector3(g, g, g);
	}

	vector3 ggb() const
	{
	    return vector3(g, g, b);
	}

	vector3 gbr() const
	{
	    return vector3(g, b, r);
	}

	vector3 gbg() const
	{
	    return vector3(g, b, g);
	}

	vector3 gbb() const
	{
	    return vector3(g, b, b);
	}

	vector3 brr() const
	{
	    return vector3(b, r, r);
	}

	vector3 brg() const
	{
	    return vector3(b, r, g);
	}

	vector3 brb() const
	{
	    return vector3(b, r, b);
	}

	vector3 bgr() const
	{
	    return vector3(b, g, r);
	}

	vector3 bgg() const
	{
	    return vector3(b, g, g);
	}

	vector3 bgb() const
	{
	    return vector3(b, g, b);
	}

	vector3 bbr() const
	{
	    return vector3(b, b, r);
	}

	vector3 bbg() const
	{
	    return vector3(b, b, g);
	}

	vector3 bbb() const
	{
	    return vector3(b, b, b);
	}

	vector2<T> ss() const
	{
	    return vector2<T>(s, s);
	}

	vector2<T> st() const
	{
	    return vector2<T>(s, t);
	}

	vector2<T> sp() const
	{
	    return vector2<T>(s, p);
	}

	vector2<T> ts() const
	{
	    return vector2<T>(t, s);
	}

	vector2<T> tt() const
	{
	    return vector2<T>(t, t);
	}

	vector2<T> tp() const
	{
	    return vector2<T>(t, p);
	}

	vector2<T> ps() const
	{
	    return vector2<T>(p, s);
	}

	vector2<T> pt() const
	{
	    return vector2<T>(p, t);
	}

	vector2<T> pp() const
	{
	    return vector2<T>(p, p);
	}

	vector3 sss() const
	{
	    return vector3(s, s, s);
	}

	vector3 sst() const
	{
	    return vector3(s, s, t);
	}

	vector3 ssp() const
	{
	    return vector3(s, s, p);
	}

	vector3 sts() const
	{
	    return vector3(s, t, s);
	}

	vector3 stt() const
	{
	    return vector3(s, t, t);
	}

	vector3 stp() const
	{
	    return vector3(s, t, p);
	}

	vector3 sps() const
	{
	    return vector3(s, p, s);
	}

	vector3 spt() const
	{
	    return vector3(s, p, t);
	}

	vector3 spp() const
	{
	    return vector3(s, p, p);
	}

	vector3 tss() const
	{
	    return vector3(t, s, s);
	}

	vector3 tst() const
	{
	    return vector3(t, s, t);
	}

	vector3 tsp() const
	{
	    return vector3(t, s, p);
	}

	vector3 tts() const
	{
	    return vector3(t, t, s);
	}

	vector3 ttt() const
	{
	    return vector3(t, t, t);
	}

	vector3 ttp() const
	{
	    return vector3(t, t, p);
	}

	vector3 tps() const
	{
	    return vector3(t, p, s);
	}

	vector3 tpt() const
	{
	    return vector3(t, p, t);
	}

	vector3 tpp() const
	{
	    return vector3(t, p, p);
	}

	vector3 pss() const
	{
	    return vector3(p, s, s);
	}

	vector3 pst() const
	{
	    return vector3(p, s, t);
	}

	vector3 psp() const
	{
	    return vector3(p, s, p);
	}

	vector3 pts() const
	{
	    return vector3(p, t, s);
	}

	vector3 ptt() const
	{
	    return vector3(p, t, t);
	}

	vector3 ptp() const
	{
	    return vector3(p, t, p);
	}

	vector3 pps() const
	{
	    return vector3(p, p, s);
	}

	vector3 ppt() const
	{
	    return vector3(p, p, t);
	}

	vector3 ppp() const
	{
	    return vector3(p, p, p);
	}

	/* Operators */

	const vector3 &operator=(const vector3 &v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return v[i];
	}

	const T &operator[](const size_t i) const
	{
	    return v[i];
	}
	
	vector3 operator*(const T s) const
	{
	    return vector3(x * s, y * s, z * s);
	}

    	friend vector3 operator*(const T s, const vector3 &w)
	{
	    return w * s;
	}

	vector3 operator/(const T s) const
	{
	    return vector3(x / s, y / s, z / s);
	}
     
	const vector3 &operator*=(const T s)
	{
	    x *= s;
	    y *= s;
	    z *= s;
	    return *this;
	}
     
	const vector3 &operator/=(const T s)
	{
	    x /= s;
	    y /= s;
	    z /= s;
	    return *this;
	}

	vector3 operator+(const vector3 &v) const
	{
	    return vector3(x + v.x, y + v.y, z + v.z);
	}

	vector3 operator-(const vector3 &v) const
	{
	    return vector3(x - v.x, y - v.y, z - v.z);
	}

	vector3 operator*(const vector3 &v) const 
	{
	    return vector3(x * v.x, y * v.y, z * v.z);
	}

	vector3 operator/(const vector3 &v) const 
	{
	    return vector3(x / v.x, y / v.y, z / v.z);
	}

	vector3 operator-() const
	{
	    return vector3(-x, -y, -z);
	}

	const vector3 &operator+=(const vector3 &v)
	{
	    x += v.x;
	    y += v.y;
	    z += v.z;
	    return *this;
	}

	const vector3 &operator-=(const vector3 &v)
	{
	    x -= v.x;
	    y -= v.y;
	    z -= v.z;
	    return *this;
	}

	const vector3 &operator*=(const vector3 &v)
	{
	    x *= v.x;
	    y *= v.y;
	    z *= v.z;
	    return *this;
	}

	const vector3 &operator/=(const vector3 &v)
	{
	    x /= v.x;
	    y /= v.y;
	    z /= v.z;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const vector3 &v)
	{
	    return os << v.str();
	}

	friend std::istream &operator>>(std::istream &is, vector3 &v)
	{
	    is >> v.x >> v.y >> v.z;
	    return is;
	}

	/* Trigonometric functions */

	vector3 sin() const
	{
	    return vector3(glvm::sin(x), glvm::sin(y), glvm::sin(z));
	}

	friend vector3 sin(const vector3 &v)
	{
    	    return v.sin();
       	}

	vector3 cos() const
	{
	    return vector3(glvm::cos(x), glvm::cos(y), glvm::cos(z));
	}

	friend vector3 cos(const vector3 &v)
	{
    	    return v.cos();
       	}

	vector3 tan() const
	{
	    return vector3(glvm::tan(x), glvm::tan(y), glvm::tan(z));
	}

	friend vector3 tan(const vector3 &v)
	{
    	    return v.tan();
       	}

	vector3 asin() const
	{
	    return vector3(glvm::asin(x), glvm::asin(y), glvm::asin(z));
	}

	friend vector3 asin(const vector3 &v)
	{
    	    return v.asin();
       	}

	vector3 acos() const
	{
	    return vector3(glvm::acos(x), glvm::acos(y), glvm::acos(z));
	}

	friend vector3 acos(const vector3 &v)
	{
    	    return v.acos();
       	}

	vector3 atan() const
	{
	    return vector3(glvm::atan(x), glvm::atan(y), glvm::atan(z));
	}

	friend vector3 atan(const vector3 &v)
	{
    	    return v.atan();
       	}

	vector3 atan(const vector3 &v) const
	{
	    return vector3(glvm::atan(x, v.x), glvm::atan(y, v.y), glvm::atan(z, v.z));
	}

	friend vector3 atan(const vector3 &v, const vector3 &w)
	{
    	    return v.atan(w);
       	}

	vector3 radians() const
	{
	    return *this * static_cast<T>(M_PI / 180.0);
	}

	friend vector3 radians(const vector3 &v)
	{
    	    return v.radians();
       	}

	vector3 degrees() const
	{
	    return *this * static_cast<T>(180.0 / M_PI);
	}

	friend vector3 degrees(const vector3 &v)
	{
    	    return v.degrees();
       	}

	/* Exponential functions */

	vector3 pow(const T p) const
	{
	    return vector3(glvm::pow(x, p), glvm::pow(y, p), glvm::pow(z, p));
	}

	friend vector3 pow(const vector3 &v, const T p)
	{
    	    return v.pow(p);
       	}

	vector3 exp() const
	{
	    return vector3(glvm::exp(x), glvm::exp(y), glvm::exp(z));
	}

	friend vector3 exp(const vector3 &v)
	{
    	    return v.exp();
       	}

	vector3 exp2() const
	{
	    return vector3(glvm::exp2(x), glvm::exp2(y), glvm::exp2(z));
	}

	friend vector3 exp2(const vector3 &v)
	{
    	    return v.exp2();
       	}

	vector3 log() const
	{
	    return vector3(glvm::log(x), glvm::log(y), glvm::log(z));
	}

	friend vector3 log(const vector3 &v)
	{
    	    return v.log();
       	}

	vector3 log2() const
	{
	    return vector3(glvm::log2(x), glvm::log2(y), glvm::log2(z));
	}

	friend vector3 log2(const vector3 &v)
	{
    	    return v.log2();
       	}

	vector3 log10() const
	{
	    return vector3(glvm::log10(x), glvm::log10(y), glvm::log10(z));
	}

	friend vector3 log10(const vector3 &v)
	{
    	    return v.log10();
       	}

	vector3 sqrt() const
	{
	    return vector3(glvm::sqrt(x), glvm::sqrt(y), glvm::sqrt(z));
	}

	friend vector3 sqrt(const vector3 &v)
	{
    	    return v.sqrt();
       	}

	vector3 inversesqrt() const
	{
	    return static_cast<T>(1) / *this.sqrt();
	}

	friend vector3 inversesqrt(const vector3 &v)
	{
    	    return v.inversesqrt();
       	}

	vector3 cbrt() const
	{
	    return vector3(glvm::cbrt(x), glvm::cbrt(y), glvm::cbrt(z));
	}

	friend vector3 cbrt(const vector3 &v)
	{
    	    return v.cbrt();
       	}

	/* Common functions */

	vector3<bool> isfinite() const
	{
	    return vector3<bool>(glvm::isfinite(x), glvm::isfinite(y), glvm::isfinite(z));
	}

	friend vector3<bool> isfinite(const vector3 &v)
	{
	    return v.isfinite();
	}

	vector3<bool> isinf() const
	{
	    return vector3<bool>(glvm::isinf(x), glvm::isinf(y), glvm::isinf(z));
	}

	friend vector3<bool> isinf(const vector3 &v)
	{
	    return v.isinf();
	}

	vector3<bool> isnan() const
	{
	    return vector3<bool>(glvm::isnan(x), glvm::isnan(y), glvm::isnan(z));
	}

	friend vector3<bool> isnan(const vector3 &v)
	{
	    return v.isnan();
	}

	vector3<bool> isnormal() const
	{
	    return vector3<bool>(glvm::isnormal(x), glvm::isnormal(y), glvm::isnormal(z));
	}

	friend vector3<bool> isnormal(const vector3 &v)
	{
	    return v.isnormal();
	}

	vector3 abs() const
	{
	    return vector3(glvm::abs(x), glvm::abs(y), glvm::abs(z));
	}

	friend vector3 abs(const vector3 &v)
	{
    	    return v.abs();
       	}

	vector3 sign() const
	{
	    return vector3(glvm::sign(x), glvm::sign(y), glvm::sign(z));
	}

	friend vector3 sign(const vector3 &v)
	{
    	    return v.sign();
       	}

	vector3 floor() const
	{
	    return vector3(glvm::floor(x), glvm::floor(y), glvm::floor(z));
	}

	friend vector3 floor(const vector3 &v)
	{
    	    return v.floor();
       	}

	vector3 ceil() const
	{
	    return vector3(glvm::ceil(x), glvm::ceil(y), glvm::ceil(z));
	}

	friend vector3 ceil(const vector3 &v)
	{
    	    return v.ceil();
       	}

	vector3 round() const
	{
	    return vector3(glvm::round(x), glvm::round(y), glvm::round(z));
	}

	friend vector3 round(const vector3 &v)
	{
    	    return v.round();
       	}

	vector3 fract() const
	{
	    return *this - this->floor();
	}

	friend vector3 fract(const vector3 &v)
	{
    	    return v.fract();
       	}

	vector3 min(const T v) const
	{
	    return vector3(v < x ? v : x, v < y ? v : y, v < z ? v : z);
	}

	friend vector3 min(const vector3 &v, const T w)
	{
    	    return v.min(w);
       	}

	vector3 min(const vector3 &v) const
	{
	    return vector3(v.x < x ? v.x : x, v.y < y ? v.y : y, v.z < z ? v.z : z);
	}

	friend vector3 min(const vector3 &v, const vector3 &w)
	{
    	    return v.min(w);
       	}

	vector3 max(const T v) const
	{
	    return vector3(v > x ? v : x, v > y ? v : y, v > z ? v : z);
	}

	friend vector3 max(const vector3 &v, const T w)
	{
    	    return v.max(w);
       	}

	vector3 max(const vector3 &v) const
	{
	    return vector3(v.x > x ? v.x : x, v.y > y ? v.y : y, v.z > z ? v.z : z);
	}

	friend vector3 max(const vector3 &v, const vector3 &w)
	{
    	    return v.max(w);
       	}

	vector3 clamp(const T minval, const T maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector3 clamp(const vector3 &v, const T minval, const T maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector3 clamp(const vector3 &minval, const vector3 &maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector3 clamp(const vector3 &v, const vector3 &minval, const vector3 &maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector3 mix(const vector3 &v, const T alpha) const
	{
	    return *this * (static_cast<T>(1) - alpha) + v * alpha;
	}

	friend vector3 mix(const vector3 &v, const vector3 &w, const T alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector3 mix(const vector3 &v, const vector3 &alpha) const
	{
	    return (vector3(static_cast<T>(1)) - alpha) * *this + alpha * v;
	}

	friend vector3 mix(const vector3 &v, const vector3 &w, const vector3 &alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector3 step(const T edge) const
	{
	    return vector3(
		    x < edge ? static_cast<T>(0) : static_cast<T>(1), 
		    y < edge ? static_cast<T>(0) : static_cast<T>(1),
		    z < edge ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector3 step(const vector3 &v, const T edge)
	{
    	    return v.step(edge);
       	}

	vector3 step(const vector3 &edge) const
	{
	    return vector3(
		    x < edge.x ? static_cast<T>(0) : static_cast<T>(1),
		    y < edge.y ? static_cast<T>(0) : static_cast<T>(1),
		    z < edge.z ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector3 step(const vector3 &v, const vector3 &edge)
	{
    	    return v.step(edge);
       	}

	vector3 smoothstep(const T edge0, const T edge1) const
	{
	    vector3 t = (*this - vector3(edge0)) / (vector3(edge1) - vector3(edge0));
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector3(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector3 smoothstep(const vector3 &v, const T edge0, const T edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector3 smoothstep(const vector3 &edge0, const vector3 &edge1) const
	{
	    vector3 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector3(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector3 smoothstep(const vector3 &v, const vector3 &edge0, const vector3 &edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector3 mod(const T y) const
	{
	    vector3 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector3 mod(const vector3 &v, const T y)
	{
    	    return v.mod(y);
       	}

	vector3 mod(const vector3 &y) const
	{
	    vector3 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector3 mod(const vector3 &v, const vector3 &y)
	{
    	    return v.mod(y);
       	}

	/* Geometric functions */

	T length() const
	{
	    return glvm::sqrt(x * x + y * y + z * z);
	}

	friend T length(const vector3 &v)
	{
    	    return v.length();
       	}

	T distance(const vector3 &v) const
	{
	    return vector3(*this - v).length();
	}

	friend T distance(const vector3 &v, const vector3 &w)
	{
    	    return v.distance(w);
       	}

	T dot(const vector3 &v) const
	{
	    return x * v.x + y * v.y + z * v.z;
	}

	friend T dot(const vector3 &v, const vector3 &w)
	{
    	    return v.dot(w);
       	}

	vector3 normalize() const
	{
	    return *this / this->length();
	}

	friend vector3 normalize(const vector3 &v)
	{
    	    return v.normalize();
       	}

	vector3 faceforward(const vector3 &I, const vector3 &Nref) const
	{
	    return Nref.dot(I) < static_cast<T>(0) ? *this : - *this;
	}

	friend vector3 faceforward(const vector3 &N, const vector3 &I, const vector3 &Nref)
	{
    	    return N.faceforward(I, Nref);
       	}

	vector3 reflect(const vector3 &N) const
	{
	    return *this - N * (N.dot(*this) * static_cast<T>(2));
	}

	friend vector3 reflect(const vector3 &I, const vector3 &N)
	{
    	    return I.reflect(N);
       	}

	vector3 refract(const vector3 &N, T eta) const
	{
	    const T d = N.dot(*this);
	    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
	    return k < static_cast<T>(0) ? vector3<T>(static_cast<T>(0)) : *this * eta - N * (eta * d + glvm::sqrt(k));
	}

	friend vector3 refract(const vector3 &I, const vector3 &N, T eta)
	{
    	    return I.refract(N, eta);
       	}

	vector3 cross(const vector3 &v) const
	{
	    return vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	friend vector3 cross(const vector3 &v1, const vector3 &v2)
	{
	    return v1.cross(v2);
	}

	/* Comparison functions */

	vector3<bool> greaterThan(const vector3 &v) const
	{
	    return vector3<bool>(x > v.x, y > v.y, z > v.z);
	}

	friend vector3<bool> greaterThan(const vector3 &v, const vector3 &w)
	{
    	    return v.greaterThan(w);
       	}

	vector3<bool> greaterThanEqual(const vector3 &v) const
	{
	    return vector3<bool>(x >= v.x, y >= v.y, z >= v.z);
	}

	friend vector3<bool> greaterThanEqual(const vector3 &v, const vector3 &w)
	{
    	    return v.greaterThanEqual(w);
       	}

	vector3<bool> lessThan(const vector3 &v) const
	{
	    return vector3<bool>(x < v.x, y < v.y, z < v.z);
	}

	friend vector3<bool> lessThan(const vector3 &v, const vector3 &w)
	{
    	    return v.lessThan(w);
       	}

	vector3<bool> lessThanEqual(const vector3 &v) const
	{
	    return vector3<bool>(x <= v.x, y <= v.y, z <= v.z);
	}

	friend vector3<bool> lessThanEqual(const vector3 &v, const vector3 &w)
	{
    	    return v.lessThanEqual(w);
       	}

	vector3<bool> equal(const vector3 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return (*this - v).abs().lessThan(vector3(epsilon));
	}

	friend vector3<bool> equal(const vector3 &v, const vector3 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.equal(w, epsilon);
       	}

	vector3<bool> notEqual(const vector3 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	friend vector3<bool> notEqual(const vector3 &v, const vector3 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.notEqual(w, epsilon);
       	}

	bool any() const
	{
	    return x || y || z;
	}

	bool all() const
	{
	    return x && y && z;
	}

	vector3<bool> negate() const
	{
	    return vector3<bool>(!x, !y, !z);
	}
    };

    template<typename T>
    inline std::string vector3<T>::str() const
    {
	std::ostringstream os;
	os << x << " " << y << " " << z << " ";
	return os.str();
    }

    template<>
    inline std::string vector3<double>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	os << x << " " << y << " " << z << " ";
	return os.str();
    }

    template<>
    inline std::string vector3<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	os << x << " " << y << " " << z << " ";
	return os.str();
    }


    template<typename T> class vector4
    {
	public:

	union
	{
	    struct
	    {
		T x, y, z, w;
	    };
	    struct
	    {
		T r, g, b, a;
	    };
	    struct
	    {
		T s, t, p, q;
	    };
	    T v[4];
	};

	/* Constructors, Destructor */

	vector4() {}

	vector4(const T x)
	{
	    this->x = x;
	    this->y = x;
	    this->z = x;
	    this->w = x;
	}

	vector4(const T x, const T y, const T z, const T w)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	    this->w = w;
	}

	vector4(const vector2<T> xy, const T z, const T w)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	    this->w = w;
	}

	vector4(const vector2<T> xy, const vector2<T> zw)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	vector4(const T x, const vector2<T> yz, const T w)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	    this->w = w;
	}

	vector4(const T x, const T y, const vector2<T> zw)
	{
	    this->x = x;
	    this->y = y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	vector4(const vector3<T> xyz, const T w)
	{
	    this->x = xyz.x;
	    this->y = xyz.y;
	    this->z = xyz.z;
	    this->w = w;
	}

	vector4(const T x, const vector3<T> yzw)
	{
	    this->x = x;
	    this->y = yzw.x;
	    this->z = yzw.y;
	    this->w = yzw.z;
	}

	vector4(const T *xyzw)
	{
	    this->x = xyzw[0];
	    this->y = xyzw[1];
	    this->z = xyzw[2];
	    this->w = xyzw[3];
	}

	template<typename U>
	vector4(const vector4<U> &v)
	{
	    this->x = v.x;
	    this->y = v.y;
	    this->z = v.z;
	    this->w = v.w;
	}

	vector4(const std::string &s)
	{
	    std::istringstream is(s);
	    is >> x >> y >> z >> w;
	}

	~vector4() {}

	/* "Swizzling" (as far as possible in C++) */

	vector2<T> xx() const
	{
	    return vector2<T>(x, x);
	}

	vector2<T> xy() const
	{
	    return vector2<T>(x, y);
	}

	vector2<T> xz() const
	{
	    return vector2<T>(x, z);
	}

	vector2<T> xw() const
	{
	    return vector2<T>(x, w);
	}

	vector2<T> yx() const
	{
	    return vector2<T>(y, x);
	}

	vector2<T> yy() const
	{
	    return vector2<T>(y, y);
	}

	vector2<T> yz() const
	{
	    return vector2<T>(y, z);
	}

	vector2<T> yw() const
	{
	    return vector2<T>(y, w);
	}

	vector2<T> zx() const
	{
	    return vector2<T>(z, x);
	}

	vector2<T> zy() const
	{
	    return vector2<T>(z, y);
	}

	vector2<T> zz() const
	{
	    return vector2<T>(z, z);
	}

	vector2<T> zw() const
	{
	    return vector2<T>(w, w);
	}

	vector2<T> wx() const
	{
	    return vector2<T>(w, x);
	}

	vector2<T> wy() const
	{
	    return vector2<T>(w, y);
	}

	vector2<T> wz() const
	{
	    return vector2<T>(w, z);
	}

	vector2<T> ww() const
	{
	    return vector2<T>(w, w);
	}

	vector3<T> xxx() const
	{
	    return vector3<T>(x, x, x);
	}

	vector3<T> xxy() const
	{
	    return vector3<T>(x, x, y);
	}

	vector3<T> xxz() const
	{
	    return vector3<T>(x, x, z);
	}

	vector3<T> xxw() const
	{
	    return vector3<T>(x, x, w);
	}

	vector3<T> xyx() const
	{
	    return vector3<T>(x, y, x);
	}

	vector3<T> xyy() const
	{
	    return vector3<T>(x, y, y);
	}

	vector3<T> xyz() const
	{
	    return vector3<T>(x, y, z);
	}

	vector3<T> xyw() const
	{
	    return vector3<T>(x, y, w);
	}

	vector3<T> xzx() const
	{
	    return vector3<T>(x, z, x);
	}

	vector3<T> xzy() const
	{
	    return vector3<T>(x, z, y);
	}

	vector3<T> xzz() const
	{
	    return vector3<T>(x, z, z);
	}

	vector3<T> xzw() const
	{
	    return vector3<T>(x, w, w);
	}

	vector3<T> xwx() const
	{
	    return vector3<T>(x, w, x);
	}

	vector3<T> xwy() const
	{
	    return vector3<T>(x, w, y);
	}

	vector3<T> xwz() const
	{
	    return vector3<T>(x, w, z);
	}

	vector3<T> xww() const
	{
	    return vector3<T>(x, w, w);
	}

	vector3<T> yxx() const
	{
	    return vector3<T>(y, x, x);
	}

	vector3<T> yxy() const
	{
	    return vector3<T>(y, x, y);
	}

	vector3<T> yxz() const
	{
	    return vector3<T>(y, x, z);
	}

	vector3<T> yxw() const
	{
	    return vector3<T>(y, x, w);
	}

	vector3<T> yyx() const
	{
	    return vector3<T>(y, y, x);
	}

	vector3<T> yyy() const
	{
	    return vector3<T>(y, y, y);
	}

	vector3<T> yyz() const
	{
	    return vector3<T>(y, y, z);
	}

	vector3<T> yyw() const
	{
	    return vector3<T>(y, y, w);
	}

	vector3<T> yzx() const
	{
	    return vector3<T>(y, z, x);
	}

	vector3<T> yzy() const
	{
	    return vector3<T>(y, z, y);
	}

	vector3<T> yzz() const
	{
	    return vector3<T>(y, z, z);
	}

	vector3<T> yzw() const
	{
	    return vector3<T>(y, w, w);
	}

	vector3<T> ywx() const
	{
	    return vector3<T>(y, w, x);
	}

	vector3<T> ywy() const
	{
	    return vector3<T>(y, w, y);
	}

	vector3<T> ywz() const
	{
	    return vector3<T>(y, w, z);
	}

	vector3<T> yww() const
	{
	    return vector3<T>(y, w, w);
	}

	vector3<T> zxx() const
	{
	    return vector3<T>(z, x, x);
	}

	vector3<T> zxy() const
	{
	    return vector3<T>(z, x, y);
	}

	vector3<T> zxz() const
	{
	    return vector3<T>(z, x, z);
	}

	vector3<T> zxw() const
	{
	    return vector3<T>(z, x, w);
	}

	vector3<T> zyx() const
	{
	    return vector3<T>(z, y, x);
	}

	vector3<T> zyy() const
	{
	    return vector3<T>(z, y, y);
	}

	vector3<T> zyz() const
	{
	    return vector3<T>(z, y, z);
	}

	vector3<T> zyw() const
	{
	    return vector3<T>(z, y, w);
	}

	vector3<T> zzx() const
	{
	    return vector3<T>(z, z, x);
	}

	vector3<T> zzy() const
	{
	    return vector3<T>(z, z, y);
	}

	vector3<T> zzz() const
	{
	    return vector3<T>(z, z, z);
	}

	vector3<T> zzw() const
	{
	    return vector3<T>(z, w, w);
	}

	vector3<T> zwx() const
	{
	    return vector3<T>(z, w, x);
	}

	vector3<T> zwy() const
	{
	    return vector3<T>(z, w, y);
	}

	vector3<T> zwz() const
	{
	    return vector3<T>(z, w, z);
	}

	vector3<T> zww() const
	{
	    return vector3<T>(z, w, w);
	}

	vector3<T> wxx() const
	{
	    return vector3<T>(w, x, x);
	}

	vector3<T> wxy() const
	{
	    return vector3<T>(w, x, y);
	}

	vector3<T> wxz() const
	{
	    return vector3<T>(w, x, z);
	}

	vector3<T> wxw() const
	{
	    return vector3<T>(w, x, w);
	}

	vector3<T> wyx() const
	{
	    return vector3<T>(w, y, x);
	}

	vector3<T> wyy() const
	{
	    return vector3<T>(w, y, y);
	}

	vector3<T> wyz() const
	{
	    return vector3<T>(w, y, z);
	}

	vector3<T> wyw() const
	{
	    return vector3<T>(w, y, w);
	}

	vector3<T> wzx() const
	{
	    return vector3<T>(w, z, x);
	}

	vector3<T> wzy() const
	{
	    return vector3<T>(w, z, y);
	}

	vector3<T> wzz() const
	{
	    return vector3<T>(w, z, z);
	}

	vector3<T> wzw() const
	{
	    return vector3<T>(w, w, w);
	}

	vector3<T> wwx() const
	{
	    return vector3<T>(w, w, x);
	}

	vector3<T> wwy() const
	{
	    return vector3<T>(w, w, y);
	}

	vector3<T> wwz() const
	{
	    return vector3<T>(w, w, z);
	}

	vector3<T> www() const
	{
	    return vector3<T>(w, w, w);
	}

	vector4 xxxx() const
	{
	    return vector4(x, x, x, x);
	}

	vector4 xxxy() const
	{
	    return vector4(x, x, x, y);
	}

	vector4 xxxz() const
	{
	    return vector4(x, x, x, z);
	}

	vector4 xxxw() const
	{
	    return vector4(x, x, x, w);
	}

	vector4 xxyx() const
	{
	    return vector4(x, x, y, x);
	}

	vector4 xxyy() const
	{
	    return vector4(x, x, y, y);
	}

	vector4 xxyz() const
	{
	    return vector4(x, x, y, z);
	}

	vector4 xxyw() const
	{
	    return vector4(x, x, y, w);
	}

	vector4 xxzx() const
	{
	    return vector4(x, x, z, x);
	}

	vector4 xxzy() const
	{
	    return vector4(x, x, z, y);
	}

	vector4 xxzz() const
	{
	    return vector4(x, x, z, z);
	}

	vector4 xxzw() const
	{
	    return vector4(x, x, z, w);
	}

	vector4 xxwx() const
	{
	    return vector4(x, x, w, x);
	}

	vector4 xxwy() const
	{
	    return vector4(x, x, w, y);
	}

	vector4 xxwz() const
	{
	    return vector4(x, x, w, z);
	}

	vector4 xxww() const
	{
	    return vector4(x, x, w, w);
	}

	vector4 xyxx() const
	{
	    return vector4(x, y, x, x);
	}

	vector4 xyxy() const
	{
	    return vector4(x, y, x, y);
	}

	vector4 xyxz() const
	{
	    return vector4(x, y, x, z);
	}

	vector4 xyxw() const
	{
	    return vector4(x, y, x, w);
	}

	vector4 xyyx() const
	{
	    return vector4(x, y, y, x);
	}

	vector4 xyyy() const
	{
	    return vector4(x, y, y, y);
	}

	vector4 xyyz() const
	{
	    return vector4(x, y, y, z);
	}

	vector4 xyyw() const
	{
	    return vector4(x, y, y, w);
	}

	vector4 xyzx() const
	{
	    return vector4(x, y, z, x);
	}

	vector4 xyzy() const
	{
	    return vector4(x, y, z, y);
	}

	vector4 xyzz() const
	{
	    return vector4(x, y, z, z);
	}

	vector4 xyzw() const
	{
	    return vector4(x, y, z, w);
	}

	vector4 xywx() const
	{
	    return vector4(x, y, w, x);
	}

	vector4 xywy() const
	{
	    return vector4(x, y, w, y);
	}

	vector4 xywz() const
	{
	    return vector4(x, y, w, z);
	}

	vector4 xyww() const
	{
	    return vector4(x, y, w, w);
	}

	vector4 xzxx() const
	{
	    return vector4(x, z, x, x);
	}

	vector4 xzxy() const
	{
	    return vector4(x, z, x, y);
	}

	vector4 xzxz() const
	{
	    return vector4(x, z, x, z);
	}

	vector4 xzxw() const
	{
	    return vector4(x, z, x, w);
	}

	vector4 xzyx() const
	{
	    return vector4(x, z, y, x);
	}

	vector4 xzyy() const
	{
	    return vector4(x, z, y, y);
	}

	vector4 xzyz() const
	{
	    return vector4(x, z, y, z);
	}

	vector4 xzyw() const
	{
	    return vector4(x, z, y, w);
	}

	vector4 xzzx() const
	{
	    return vector4(x, z, z, x);
	}

	vector4 xzzy() const
	{
	    return vector4(x, z, z, y);
	}

	vector4 xzzz() const
	{
	    return vector4(x, z, z, z);
	}

	vector4 xzzw() const
	{
	    return vector4(x, z, z, w);
	}

	vector4 xzwx() const
	{
	    return vector4(x, z, w, x);
	}

	vector4 xzwy() const
	{
	    return vector4(x, z, w, y);
	}

	vector4 xzwz() const
	{
	    return vector4(x, z, w, z);
	}

	vector4 xzww() const
	{
	    return vector4(x, z, w, w);
	}

	vector4 xwxx() const
	{
	    return vector4(x, w, x, x);
	}

	vector4 xwxy() const
	{
	    return vector4(x, w, x, y);
	}

	vector4 xwxz() const
	{
	    return vector4(x, w, x, z);
	}

	vector4 xwxw() const
	{
	    return vector4(x, w, x, w);
	}

	vector4 xwyx() const
	{
	    return vector4(x, w, y, x);
	}

	vector4 xwyy() const
	{
	    return vector4(x, w, y, y);
	}

	vector4 xwyz() const
	{
	    return vector4(x, w, y, z);
	}

	vector4 xwyw() const
	{
	    return vector4(x, w, y, w);
	}

	vector4 xwzx() const
	{
	    return vector4(x, w, z, x);
	}

	vector4 xwzy() const
	{
	    return vector4(x, w, z, y);
	}

	vector4 xwzz() const
	{
	    return vector4(x, w, z, z);
	}

	vector4 xwzw() const
	{
	    return vector4(x, w, z, w);
	}

	vector4 xwwx() const
	{
	    return vector4(x, w, w, x);
	}

	vector4 xwwy() const
	{
	    return vector4(x, w, w, y);
	}

	vector4 xwwz() const
	{
	    return vector4(x, w, w, z);
	}

	vector4 xwww() const
	{
	    return vector4(x, w, w, w);
	}

	vector4 yxxx() const
	{
	    return vector4(y, x, x, x);
	}

	vector4 yxxy() const
	{
	    return vector4(y, x, x, y);
	}

	vector4 yxxz() const
	{
	    return vector4(y, x, x, z);
	}

	vector4 yxxw() const
	{
	    return vector4(y, x, x, w);
	}

	vector4 yxyx() const
	{
	    return vector4(y, x, y, x);
	}

	vector4 yxyy() const
	{
	    return vector4(y, x, y, y);
	}

	vector4 yxyz() const
	{
	    return vector4(y, x, y, z);
	}

	vector4 yxyw() const
	{
	    return vector4(y, x, y, w);
	}

	vector4 yxzx() const
	{
	    return vector4(y, x, z, x);
	}

	vector4 yxzy() const
	{
	    return vector4(y, x, z, y);
	}

	vector4 yxzz() const
	{
	    return vector4(y, x, z, z);
	}

	vector4 yxzw() const
	{
	    return vector4(y, x, z, w);
	}

	vector4 yxwx() const
	{
	    return vector4(y, x, w, x);
	}

	vector4 yxwy() const
	{
	    return vector4(y, x, w, y);
	}

	vector4 yxwz() const
	{
	    return vector4(y, x, w, z);
	}

	vector4 yxww() const
	{
	    return vector4(y, x, w, w);
	}

	vector4 yyxx() const
	{
	    return vector4(y, y, x, x);
	}

	vector4 yyxy() const
	{
	    return vector4(y, y, x, y);
	}

	vector4 yyxz() const
	{
	    return vector4(y, y, x, z);
	}

	vector4 yyxw() const
	{
	    return vector4(y, y, x, w);
	}

	vector4 yyyx() const
	{
	    return vector4(y, y, y, x);
	}

	vector4 yyyy() const
	{
	    return vector4(y, y, y, y);
	}

	vector4 yyyz() const
	{
	    return vector4(y, y, y, z);
	}

	vector4 yyyw() const
	{
	    return vector4(y, y, y, w);
	}

	vector4 yyzx() const
	{
	    return vector4(y, y, z, x);
	}

	vector4 yyzy() const
	{
	    return vector4(y, y, z, y);
	}

	vector4 yyzz() const
	{
	    return vector4(y, y, z, z);
	}

	vector4 yyzw() const
	{
	    return vector4(y, y, z, w);
	}

	vector4 yywx() const
	{
	    return vector4(y, y, w, x);
	}

	vector4 yywy() const
	{
	    return vector4(y, y, w, y);
	}

	vector4 yywz() const
	{
	    return vector4(y, y, w, z);
	}

	vector4 yyww() const
	{
	    return vector4(y, y, w, w);
	}

	vector4 yzxx() const
	{
	    return vector4(y, z, x, x);
	}

	vector4 yzxy() const
	{
	    return vector4(y, z, x, y);
	}

	vector4 yzxz() const
	{
	    return vector4(y, z, x, z);
	}

	vector4 yzxw() const
	{
	    return vector4(y, z, x, w);
	}

	vector4 yzyx() const
	{
	    return vector4(y, z, y, x);
	}

	vector4 yzyy() const
	{
	    return vector4(y, z, y, y);
	}

	vector4 yzyz() const
	{
	    return vector4(y, z, y, z);
	}

	vector4 yzyw() const
	{
	    return vector4(y, z, y, w);
	}

	vector4 yzzx() const
	{
	    return vector4(y, z, z, x);
	}

	vector4 yzzy() const
	{
	    return vector4(y, z, z, y);
	}

	vector4 yzzz() const
	{
	    return vector4(y, z, z, z);
	}

	vector4 yzzw() const
	{
	    return vector4(y, z, z, w);
	}

	vector4 yzwx() const
	{
	    return vector4(y, z, w, x);
	}

	vector4 yzwy() const
	{
	    return vector4(y, z, w, y);
	}

	vector4 yzwz() const
	{
	    return vector4(y, z, w, z);
	}

	vector4 yzww() const
	{
	    return vector4(y, z, w, w);
	}

	vector4 ywxx() const
	{
	    return vector4(y, w, x, x);
	}

	vector4 ywxy() const
	{
	    return vector4(y, w, x, y);
	}

	vector4 ywxz() const
	{
	    return vector4(y, w, x, z);
	}

	vector4 ywxw() const
	{
	    return vector4(y, w, x, w);
	}

	vector4 ywyx() const
	{
	    return vector4(y, w, y, x);
	}

	vector4 ywyy() const
	{
	    return vector4(y, w, y, y);
	}

	vector4 ywyz() const
	{
	    return vector4(y, w, y, z);
	}

	vector4 ywyw() const
	{
	    return vector4(y, w, y, w);
	}

	vector4 ywzx() const
	{
	    return vector4(y, w, z, x);
	}

	vector4 ywzy() const
	{
	    return vector4(y, w, z, y);
	}

	vector4 ywzz() const
	{
	    return vector4(y, w, z, z);
	}

	vector4 ywzw() const
	{
	    return vector4(y, w, z, w);
	}

	vector4 ywwx() const
	{
	    return vector4(y, w, w, x);
	}

	vector4 ywwy() const
	{
	    return vector4(y, w, w, y);
	}

	vector4 ywwz() const
	{
	    return vector4(y, w, w, z);
	}

	vector4 ywww() const
	{
	    return vector4(y, w, w, w);
	}

	vector4 zxxx() const
	{
	    return vector4(z, x, x, x);
	}

	vector4 zxxy() const
	{
	    return vector4(z, x, x, y);
	}

	vector4 zxxz() const
	{
	    return vector4(z, x, x, z);
	}

	vector4 zxxw() const
	{
	    return vector4(z, x, x, w);
	}

	vector4 zxyx() const
	{
	    return vector4(z, x, y, x);
	}

	vector4 zxyy() const
	{
	    return vector4(z, x, y, y);
	}

	vector4 zxyz() const
	{
	    return vector4(z, x, y, z);
	}

	vector4 zxyw() const
	{
	    return vector4(z, x, y, w);
	}

	vector4 zxzx() const
	{
	    return vector4(z, x, z, x);
	}

	vector4 zxzy() const
	{
	    return vector4(z, x, z, y);
	}

	vector4 zxzz() const
	{
	    return vector4(z, x, z, z);
	}

	vector4 zxzw() const
	{
	    return vector4(z, x, z, w);
	}

	vector4 zxwx() const
	{
	    return vector4(z, x, w, x);
	}

	vector4 zxwy() const
	{
	    return vector4(z, x, w, y);
	}

	vector4 zxwz() const
	{
	    return vector4(z, x, w, z);
	}

	vector4 zxww() const
	{
	    return vector4(z, x, w, w);
	}

	vector4 zyxx() const
	{
	    return vector4(z, y, x, x);
	}

	vector4 zyxy() const
	{
	    return vector4(z, y, x, y);
	}

	vector4 zyxz() const
	{
	    return vector4(z, y, x, z);
	}

	vector4 zyxw() const
	{
	    return vector4(z, y, x, w);
	}

	vector4 zyyx() const
	{
	    return vector4(z, y, y, x);
	}

	vector4 zyyy() const
	{
	    return vector4(z, y, y, y);
	}

	vector4 zyyz() const
	{
	    return vector4(z, y, y, z);
	}

	vector4 zyyw() const
	{
	    return vector4(z, y, y, w);
	}

	vector4 zyzx() const
	{
	    return vector4(z, y, z, x);
	}

	vector4 zyzy() const
	{
	    return vector4(z, y, z, y);
	}

	vector4 zyzz() const
	{
	    return vector4(z, y, z, z);
	}

	vector4 zyzw() const
	{
	    return vector4(z, y, z, w);
	}

	vector4 zywx() const
	{
	    return vector4(z, y, w, x);
	}

	vector4 zywy() const
	{
	    return vector4(z, y, w, y);
	}

	vector4 zywz() const
	{
	    return vector4(z, y, w, z);
	}

	vector4 zyww() const
	{
	    return vector4(z, y, w, w);
	}

	vector4 zzxx() const
	{
	    return vector4(z, z, x, x);
	}

	vector4 zzxy() const
	{
	    return vector4(z, z, x, y);
	}

	vector4 zzxz() const
	{
	    return vector4(z, z, x, z);
	}

	vector4 zzxw() const
	{
	    return vector4(z, z, x, w);
	}

	vector4 zzyx() const
	{
	    return vector4(z, z, y, x);
	}

	vector4 zzyy() const
	{
	    return vector4(z, z, y, y);
	}

	vector4 zzyz() const
	{
	    return vector4(z, z, y, z);
	}

	vector4 zzyw() const
	{
	    return vector4(z, z, y, w);
	}

	vector4 zzzx() const
	{
	    return vector4(z, z, z, x);
	}

	vector4 zzzy() const
	{
	    return vector4(z, z, z, y);
	}

	vector4 zzzz() const
	{
	    return vector4(z, z, z, z);
	}

	vector4 zzzw() const
	{
	    return vector4(z, z, z, w);
	}

	vector4 zzwx() const
	{
	    return vector4(z, z, w, x);
	}

	vector4 zzwy() const
	{
	    return vector4(z, z, w, y);
	}

	vector4 zzwz() const
	{
	    return vector4(z, z, w, z);
	}

	vector4 zzww() const
	{
	    return vector4(z, z, w, w);
	}

	vector4 zwxx() const
	{
	    return vector4(z, w, x, x);
	}

	vector4 zwxy() const
	{
	    return vector4(z, w, x, y);
	}

	vector4 zwxz() const
	{
	    return vector4(z, w, x, z);
	}

	vector4 zwxw() const
	{
	    return vector4(z, w, x, w);
	}

	vector4 zwyx() const
	{
	    return vector4(z, w, y, x);
	}

	vector4 zwyy() const
	{
	    return vector4(z, w, y, y);
	}

	vector4 zwyz() const
	{
	    return vector4(z, w, y, z);
	}

	vector4 zwyw() const
	{
	    return vector4(z, w, y, w);
	}

	vector4 zwzx() const
	{
	    return vector4(z, w, z, x);
	}

	vector4 zwzy() const
	{
	    return vector4(z, w, z, y);
	}

	vector4 zwzz() const
	{
	    return vector4(z, w, z, z);
	}

	vector4 zwzw() const
	{
	    return vector4(z, w, z, w);
	}

	vector4 zwwx() const
	{
	    return vector4(z, w, w, x);
	}

	vector4 zwwy() const
	{
	    return vector4(z, w, w, y);
	}

	vector4 zwwz() const
	{
	    return vector4(z, w, w, z);
	}

	vector4 zwww() const
	{
	    return vector4(z, w, w, w);
	}

	vector4 wxxx() const
	{
	    return vector4(w, x, x, x);
	}

	vector4 wxxy() const
	{
	    return vector4(w, x, x, y);
	}

	vector4 wxxz() const
	{
	    return vector4(w, x, x, z);
	}

	vector4 wxxw() const
	{
	    return vector4(w, x, x, w);
	}

	vector4 wxyx() const
	{
	    return vector4(w, x, y, x);
	}

	vector4 wxyy() const
	{
	    return vector4(w, x, y, y);
	}

	vector4 wxyz() const
	{
	    return vector4(w, x, y, z);
	}

	vector4 wxyw() const
	{
	    return vector4(w, x, y, w);
	}

	vector4 wxzx() const
	{
	    return vector4(w, x, z, x);
	}

	vector4 wxzy() const
	{
	    return vector4(w, x, z, y);
	}

	vector4 wxzz() const
	{
	    return vector4(w, x, z, z);
	}

	vector4 wxzw() const
	{
	    return vector4(w, x, z, w);
	}

	vector4 wxwx() const
	{
	    return vector4(w, x, w, x);
	}

	vector4 wxwy() const
	{
	    return vector4(w, x, w, y);
	}

	vector4 wxwz() const
	{
	    return vector4(w, x, w, z);
	}

	vector4 wxww() const
	{
	    return vector4(w, x, w, w);
	}

	vector4 wyxx() const
	{
	    return vector4(w, y, x, x);
	}

	vector4 wyxy() const
	{
	    return vector4(w, y, x, y);
	}

	vector4 wyxz() const
	{
	    return vector4(w, y, x, z);
	}

	vector4 wyxw() const
	{
	    return vector4(w, y, x, w);
	}

	vector4 wyyx() const
	{
	    return vector4(w, y, y, x);
	}

	vector4 wyyy() const
	{
	    return vector4(w, y, y, y);
	}

	vector4 wyyz() const
	{
	    return vector4(w, y, y, z);
	}

	vector4 wyyw() const
	{
	    return vector4(w, y, y, w);
	}

	vector4 wyzx() const
	{
	    return vector4(w, y, z, x);
	}

	vector4 wyzy() const
	{
	    return vector4(w, y, z, y);
	}

	vector4 wyzz() const
	{
	    return vector4(w, y, z, z);
	}

	vector4 wyzw() const
	{
	    return vector4(w, y, z, w);
	}

	vector4 wywx() const
	{
	    return vector4(w, y, w, x);
	}

	vector4 wywy() const
	{
	    return vector4(w, y, w, y);
	}

	vector4 wywz() const
	{
	    return vector4(w, y, w, z);
	}

	vector4 wyww() const
	{
	    return vector4(w, y, w, w);
	}

	vector4 wzxx() const
	{
	    return vector4(w, z, x, x);
	}

	vector4 wzxy() const
	{
	    return vector4(w, z, x, y);
	}

	vector4 wzxz() const
	{
	    return vector4(w, z, x, z);
	}

	vector4 wzxw() const
	{
	    return vector4(w, z, x, w);
	}

	vector4 wzyx() const
	{
	    return vector4(w, z, y, x);
	}

	vector4 wzyy() const
	{
	    return vector4(w, z, y, y);
	}

	vector4 wzyz() const
	{
	    return vector4(w, z, y, z);
	}

	vector4 wzyw() const
	{
	    return vector4(w, z, y, w);
	}

	vector4 wzzx() const
	{
	    return vector4(w, z, z, x);
	}

	vector4 wzzy() const
	{
	    return vector4(w, z, z, y);
	}

	vector4 wzzz() const
	{
	    return vector4(w, z, z, z);
	}

	vector4 wzzw() const
	{
	    return vector4(w, z, z, w);
	}

	vector4 wzwx() const
	{
	    return vector4(w, z, w, x);
	}

	vector4 wzwy() const
	{
	    return vector4(w, z, w, y);
	}

	vector4 wzwz() const
	{
	    return vector4(w, z, w, z);
	}

	vector4 wzww() const
	{
	    return vector4(w, z, w, w);
	}

	vector4 wwxx() const
	{
	    return vector4(w, w, x, x);
	}

	vector4 wwxy() const
	{
	    return vector4(w, w, x, y);
	}

	vector4 wwxz() const
	{
	    return vector4(w, w, x, z);
	}

	vector4 wwxw() const
	{
	    return vector4(w, w, x, w);
	}

	vector4 wwyx() const
	{
	    return vector4(w, w, y, x);
	}

	vector4 wwyy() const
	{
	    return vector4(w, w, y, y);
	}

	vector4 wwyz() const
	{
	    return vector4(w, w, y, z);
	}

	vector4 wwyw() const
	{
	    return vector4(w, w, y, w);
	}

	vector4 wwzx() const
	{
	    return vector4(w, w, z, x);
	}

	vector4 wwzy() const
	{
	    return vector4(w, w, z, y);
	}

	vector4 wwzz() const
	{
	    return vector4(w, w, z, z);
	}

	vector4 wwzw() const
	{
	    return vector4(w, w, z, w);
	}

	vector4 wwwx() const
	{
	    return vector4(w, w, w, x);
	}

	vector4 wwwy() const
	{
	    return vector4(w, w, w, y);
	}

	vector4 wwwz() const
	{
	    return vector4(w, w, w, z);
	}

	vector4 wwww() const
	{
	    return vector4(w, w, w, w);
	}

	vector2<T> rr() const
	{
	    return vector2<T>(r, r);
	}

	vector2<T> rg() const
	{
	    return vector2<T>(r, g);
	}

	vector2<T> rb() const
	{
	    return vector2<T>(r, b);
	}

	vector2<T> ra() const
	{
	    return vector2<T>(r, a);
	}

	vector2<T> gr() const
	{
	    return vector2<T>(g, r);
	}

	vector2<T> gg() const
	{
	    return vector2<T>(g, g);
	}

	vector2<T> gb() const
	{
	    return vector2<T>(g, b);
	}

	vector2<T> ga() const
	{
	    return vector2<T>(g, a);
	}

	vector2<T> br() const
	{
	    return vector2<T>(b, r);
	}

	vector2<T> bg() const
	{
	    return vector2<T>(b, g);
	}

	vector2<T> bb() const
	{
	    return vector2<T>(b, b);
	}

	vector2<T> ba() const
	{
	    return vector2<T>(b, a);
	}

	vector2<T> ar() const
	{
	    return vector2<T>(a, r);
	}

	vector2<T> ag() const
	{
	    return vector2<T>(a, g);
	}

	vector2<T> ab() const
	{
	    return vector2<T>(a, b);
	}

	vector2<T> aa() const
	{
	    return vector2<T>(a, a);
	}

	vector3<T> rrr() const
	{
	    return vector3<T>(r, r, r);
	}

	vector3<T> rrg() const
	{
	    return vector3<T>(r, r, g);
	}

	vector3<T> rrb() const
	{
	    return vector3<T>(r, r, b);
	}

	vector3<T> rra() const
	{
	    return vector3<T>(r, r, a);
	}

	vector3<T> rgr() const
	{
	    return vector3<T>(r, g, r);
	}

	vector3<T> rgg() const
	{
	    return vector3<T>(r, g, g);
	}

	vector3<T> rgb() const
	{
	    return vector3<T>(r, g, b);
	}

	vector3<T> rga() const
	{
	    return vector3<T>(r, g, a);
	}

	vector3<T> rbr() const
	{
	    return vector3<T>(r, b, r);
	}

	vector3<T> rbg() const
	{
	    return vector3<T>(r, b, g);
	}

	vector3<T> rbb() const
	{
	    return vector3<T>(r, b, b);
	}

	vector3<T> rba() const
	{
	    return vector3<T>(r, a, a);
	}

	vector3<T> rar() const
	{
	    return vector3<T>(r, a, r);
	}

	vector3<T> rag() const
	{
	    return vector3<T>(r, a, g);
	}

	vector3<T> rab() const
	{
	    return vector3<T>(r, a, b);
	}

	vector3<T> raa() const
	{
	    return vector3<T>(r, a, a);
	}

	vector3<T> grr() const
	{
	    return vector3<T>(g, r, r);
	}

	vector3<T> grg() const
	{
	    return vector3<T>(g, r, g);
	}

	vector3<T> grb() const
	{
	    return vector3<T>(g, r, b);
	}

	vector3<T> gra() const
	{
	    return vector3<T>(g, r, a);
	}

	vector3<T> ggr() const
	{
	    return vector3<T>(g, g, r);
	}

	vector3<T> ggg() const
	{
	    return vector3<T>(g, g, g);
	}

	vector3<T> ggb() const
	{
	    return vector3<T>(g, g, b);
	}

	vector3<T> gga() const
	{
	    return vector3<T>(g, g, a);
	}

	vector3<T> gbr() const
	{
	    return vector3<T>(g, b, r);
	}

	vector3<T> gbg() const
	{
	    return vector3<T>(g, b, g);
	}

	vector3<T> gbb() const
	{
	    return vector3<T>(g, b, b);
	}

	vector3<T> gba() const
	{
	    return vector3<T>(g, a, a);
	}

	vector3<T> gar() const
	{
	    return vector3<T>(g, a, r);
	}

	vector3<T> gag() const
	{
	    return vector3<T>(g, a, g);
	}

	vector3<T> gab() const
	{
	    return vector3<T>(g, a, b);
	}

	vector3<T> gaa() const
	{
	    return vector3<T>(g, a, a);
	}

	vector3<T> brr() const
	{
	    return vector3<T>(b, r, r);
	}

	vector3<T> brg() const
	{
	    return vector3<T>(b, r, g);
	}

	vector3<T> brb() const
	{
	    return vector3<T>(b, r, b);
	}

	vector3<T> bra() const
	{
	    return vector3<T>(b, r, a);
	}

	vector3<T> bgr() const
	{
	    return vector3<T>(b, g, r);
	}

	vector3<T> bgg() const
	{
	    return vector3<T>(b, g, g);
	}

	vector3<T> bgb() const
	{
	    return vector3<T>(b, g, b);
	}

	vector3<T> bga() const
	{
	    return vector3<T>(b, g, a);
	}

	vector3<T> bbr() const
	{
	    return vector3<T>(b, b, r);
	}

	vector3<T> bbg() const
	{
	    return vector3<T>(b, b, g);
	}

	vector3<T> bbb() const
	{
	    return vector3<T>(b, b, b);
	}

	vector3<T> bba() const
	{
	    return vector3<T>(b, a, a);
	}

	vector3<T> bar() const
	{
	    return vector3<T>(b, a, r);
	}

	vector3<T> bag() const
	{
	    return vector3<T>(b, a, g);
	}

	vector3<T> bab() const
	{
	    return vector3<T>(b, a, b);
	}

	vector3<T> baa() const
	{
	    return vector3<T>(b, a, a);
	}

	vector3<T> arr() const
	{
	    return vector3<T>(a, r, r);
	}

	vector3<T> arg() const
	{
	    return vector3<T>(a, r, g);
	}

	vector3<T> arb() const
	{
	    return vector3<T>(a, r, b);
	}

	vector3<T> ara() const
	{
	    return vector3<T>(a, r, a);
	}

	vector3<T> agr() const
	{
	    return vector3<T>(a, g, r);
	}

	vector3<T> agg() const
	{
	    return vector3<T>(a, g, g);
	}

	vector3<T> agb() const
	{
	    return vector3<T>(a, g, b);
	}

	vector3<T> aga() const
	{
	    return vector3<T>(a, g, a);
	}

	vector3<T> abr() const
	{
	    return vector3<T>(a, b, r);
	}

	vector3<T> abg() const
	{
	    return vector3<T>(a, b, g);
	}

	vector3<T> abb() const
	{
	    return vector3<T>(a, b, b);
	}

	vector3<T> aba() const
	{
	    return vector3<T>(a, a, a);
	}

	vector3<T> aar() const
	{
	    return vector3<T>(a, a, r);
	}

	vector3<T> aag() const
	{
	    return vector3<T>(a, a, g);
	}

	vector3<T> aab() const
	{
	    return vector3<T>(a, a, b);
	}

	vector3<T> aaa() const
	{
	    return vector3<T>(a, a, a);
	}

	vector4 rrrr() const
	{
	    return vector4(r, r, r, r);
	}

	vector4 rrrg() const
	{
	    return vector4(r, r, r, g);
	}

	vector4 rrrb() const
	{
	    return vector4(r, r, r, b);
	}

	vector4 rrra() const
	{
	    return vector4(r, r, r, a);
	}

	vector4 rrgr() const
	{
	    return vector4(r, r, g, r);
	}

	vector4 rrgg() const
	{
	    return vector4(r, r, g, g);
	}

	vector4 rrgb() const
	{
	    return vector4(r, r, g, b);
	}

	vector4 rrga() const
	{
	    return vector4(r, r, g, a);
	}

	vector4 rrbr() const
	{
	    return vector4(r, r, b, r);
	}

	vector4 rrbg() const
	{
	    return vector4(r, r, b, g);
	}

	vector4 rrbb() const
	{
	    return vector4(r, r, b, b);
	}

	vector4 rrba() const
	{
	    return vector4(r, r, b, a);
	}

	vector4 rrar() const
	{
	    return vector4(r, r, a, r);
	}

	vector4 rrag() const
	{
	    return vector4(r, r, a, g);
	}

	vector4 rrab() const
	{
	    return vector4(r, r, a, b);
	}

	vector4 rraa() const
	{
	    return vector4(r, r, a, a);
	}

	vector4 rgrr() const
	{
	    return vector4(r, g, r, r);
	}

	vector4 rgrg() const
	{
	    return vector4(r, g, r, g);
	}

	vector4 rgrb() const
	{
	    return vector4(r, g, r, b);
	}

	vector4 rgra() const
	{
	    return vector4(r, g, r, a);
	}

	vector4 rggr() const
	{
	    return vector4(r, g, g, r);
	}

	vector4 rggg() const
	{
	    return vector4(r, g, g, g);
	}

	vector4 rggb() const
	{
	    return vector4(r, g, g, b);
	}

	vector4 rgga() const
	{
	    return vector4(r, g, g, a);
	}

	vector4 rgbr() const
	{
	    return vector4(r, g, b, r);
	}

	vector4 rgbg() const
	{
	    return vector4(r, g, b, g);
	}

	vector4 rgbb() const
	{
	    return vector4(r, g, b, b);
	}

	vector4 rgba() const
	{
	    return vector4(r, g, b, a);
	}

	vector4 rgar() const
	{
	    return vector4(r, g, a, r);
	}

	vector4 rgag() const
	{
	    return vector4(r, g, a, g);
	}

	vector4 rgab() const
	{
	    return vector4(r, g, a, b);
	}

	vector4 rgaa() const
	{
	    return vector4(r, g, a, a);
	}

	vector4 rbrr() const
	{
	    return vector4(r, b, r, r);
	}

	vector4 rbrg() const
	{
	    return vector4(r, b, r, g);
	}

	vector4 rbrb() const
	{
	    return vector4(r, b, r, b);
	}

	vector4 rbra() const
	{
	    return vector4(r, b, r, a);
	}

	vector4 rbgr() const
	{
	    return vector4(r, b, g, r);
	}

	vector4 rbgg() const
	{
	    return vector4(r, b, g, g);
	}

	vector4 rbgb() const
	{
	    return vector4(r, b, g, b);
	}

	vector4 rbga() const
	{
	    return vector4(r, b, g, a);
	}

	vector4 rbbr() const
	{
	    return vector4(r, b, b, r);
	}

	vector4 rbbg() const
	{
	    return vector4(r, b, b, g);
	}

	vector4 rbbb() const
	{
	    return vector4(r, b, b, b);
	}

	vector4 rbba() const
	{
	    return vector4(r, b, b, a);
	}

	vector4 rbar() const
	{
	    return vector4(r, b, a, r);
	}

	vector4 rbag() const
	{
	    return vector4(r, b, a, g);
	}

	vector4 rbab() const
	{
	    return vector4(r, b, a, b);
	}

	vector4 rbaa() const
	{
	    return vector4(r, b, a, a);
	}

	vector4 rarr() const
	{
	    return vector4(r, a, r, r);
	}

	vector4 rarg() const
	{
	    return vector4(r, a, r, g);
	}

	vector4 rarb() const
	{
	    return vector4(r, a, r, b);
	}

	vector4 rara() const
	{
	    return vector4(r, a, r, a);
	}

	vector4 ragr() const
	{
	    return vector4(r, a, g, r);
	}

	vector4 ragg() const
	{
	    return vector4(r, a, g, g);
	}

	vector4 ragb() const
	{
	    return vector4(r, a, g, b);
	}

	vector4 raga() const
	{
	    return vector4(r, a, g, a);
	}

	vector4 rabr() const
	{
	    return vector4(r, a, b, r);
	}

	vector4 rabg() const
	{
	    return vector4(r, a, b, g);
	}

	vector4 rabb() const
	{
	    return vector4(r, a, b, b);
	}

	vector4 raba() const
	{
	    return vector4(r, a, b, a);
	}

	vector4 raar() const
	{
	    return vector4(r, a, a, r);
	}

	vector4 raag() const
	{
	    return vector4(r, a, a, g);
	}

	vector4 raab() const
	{
	    return vector4(r, a, a, b);
	}

	vector4 raaa() const
	{
	    return vector4(r, a, a, a);
	}

	vector4 grrr() const
	{
	    return vector4(g, r, r, r);
	}

	vector4 grrg() const
	{
	    return vector4(g, r, r, g);
	}

	vector4 grrb() const
	{
	    return vector4(g, r, r, b);
	}

	vector4 grra() const
	{
	    return vector4(g, r, r, a);
	}

	vector4 grgr() const
	{
	    return vector4(g, r, g, r);
	}

	vector4 grgg() const
	{
	    return vector4(g, r, g, g);
	}

	vector4 grgb() const
	{
	    return vector4(g, r, g, b);
	}

	vector4 grga() const
	{
	    return vector4(g, r, g, a);
	}

	vector4 grbr() const
	{
	    return vector4(g, r, b, r);
	}

	vector4 grbg() const
	{
	    return vector4(g, r, b, g);
	}

	vector4 grbb() const
	{
	    return vector4(g, r, b, b);
	}

	vector4 grba() const
	{
	    return vector4(g, r, b, a);
	}

	vector4 grar() const
	{
	    return vector4(g, r, a, r);
	}

	vector4 grag() const
	{
	    return vector4(g, r, a, g);
	}

	vector4 grab() const
	{
	    return vector4(g, r, a, b);
	}

	vector4 graa() const
	{
	    return vector4(g, r, a, a);
	}

	vector4 ggrr() const
	{
	    return vector4(g, g, r, r);
	}

	vector4 ggrg() const
	{
	    return vector4(g, g, r, g);
	}

	vector4 ggrb() const
	{
	    return vector4(g, g, r, b);
	}

	vector4 ggra() const
	{
	    return vector4(g, g, r, a);
	}

	vector4 gggr() const
	{
	    return vector4(g, g, g, r);
	}

	vector4 gggg() const
	{
	    return vector4(g, g, g, g);
	}

	vector4 gggb() const
	{
	    return vector4(g, g, g, b);
	}

	vector4 ggga() const
	{
	    return vector4(g, g, g, a);
	}

	vector4 ggbr() const
	{
	    return vector4(g, g, b, r);
	}

	vector4 ggbg() const
	{
	    return vector4(g, g, b, g);
	}

	vector4 ggbb() const
	{
	    return vector4(g, g, b, b);
	}

	vector4 ggba() const
	{
	    return vector4(g, g, b, a);
	}

	vector4 ggar() const
	{
	    return vector4(g, g, a, r);
	}

	vector4 ggag() const
	{
	    return vector4(g, g, a, g);
	}

	vector4 ggab() const
	{
	    return vector4(g, g, a, b);
	}

	vector4 ggaa() const
	{
	    return vector4(g, g, a, a);
	}

	vector4 gbrr() const
	{
	    return vector4(g, b, r, r);
	}

	vector4 gbrg() const
	{
	    return vector4(g, b, r, g);
	}

	vector4 gbrb() const
	{
	    return vector4(g, b, r, b);
	}

	vector4 gbra() const
	{
	    return vector4(g, b, r, a);
	}

	vector4 gbgr() const
	{
	    return vector4(g, b, g, r);
	}

	vector4 gbgg() const
	{
	    return vector4(g, b, g, g);
	}

	vector4 gbgb() const
	{
	    return vector4(g, b, g, b);
	}

	vector4 gbga() const
	{
	    return vector4(g, b, g, a);
	}

	vector4 gbbr() const
	{
	    return vector4(g, b, b, r);
	}

	vector4 gbbg() const
	{
	    return vector4(g, b, b, g);
	}

	vector4 gbbb() const
	{
	    return vector4(g, b, b, b);
	}

	vector4 gbba() const
	{
	    return vector4(g, b, b, a);
	}

	vector4 gbar() const
	{
	    return vector4(g, b, a, r);
	}

	vector4 gbag() const
	{
	    return vector4(g, b, a, g);
	}

	vector4 gbab() const
	{
	    return vector4(g, b, a, b);
	}

	vector4 gbaa() const
	{
	    return vector4(g, b, a, a);
	}

	vector4 garr() const
	{
	    return vector4(g, a, r, r);
	}

	vector4 garg() const
	{
	    return vector4(g, a, r, g);
	}

	vector4 garb() const
	{
	    return vector4(g, a, r, b);
	}

	vector4 gara() const
	{
	    return vector4(g, a, r, a);
	}

	vector4 gagr() const
	{
	    return vector4(g, a, g, r);
	}

	vector4 gagg() const
	{
	    return vector4(g, a, g, g);
	}

	vector4 gagb() const
	{
	    return vector4(g, a, g, b);
	}

	vector4 gaga() const
	{
	    return vector4(g, a, g, a);
	}

	vector4 gabr() const
	{
	    return vector4(g, a, b, r);
	}

	vector4 gabg() const
	{
	    return vector4(g, a, b, g);
	}

	vector4 gabb() const
	{
	    return vector4(g, a, b, b);
	}

	vector4 gaba() const
	{
	    return vector4(g, a, b, a);
	}

	vector4 gaar() const
	{
	    return vector4(g, a, a, r);
	}

	vector4 gaag() const
	{
	    return vector4(g, a, a, g);
	}

	vector4 gaab() const
	{
	    return vector4(g, a, a, b);
	}

	vector4 gaaa() const
	{
	    return vector4(g, a, a, a);
	}

	vector4 brrr() const
	{
	    return vector4(b, r, r, r);
	}

	vector4 brrg() const
	{
	    return vector4(b, r, r, g);
	}

	vector4 brrb() const
	{
	    return vector4(b, r, r, b);
	}

	vector4 brra() const
	{
	    return vector4(b, r, r, a);
	}

	vector4 brgr() const
	{
	    return vector4(b, r, g, r);
	}

	vector4 brgg() const
	{
	    return vector4(b, r, g, g);
	}

	vector4 brgb() const
	{
	    return vector4(b, r, g, b);
	}

	vector4 brga() const
	{
	    return vector4(b, r, g, a);
	}

	vector4 brbr() const
	{
	    return vector4(b, r, b, r);
	}

	vector4 brbg() const
	{
	    return vector4(b, r, b, g);
	}

	vector4 brbb() const
	{
	    return vector4(b, r, b, b);
	}

	vector4 brba() const
	{
	    return vector4(b, r, b, a);
	}

	vector4 brar() const
	{
	    return vector4(b, r, a, r);
	}

	vector4 brag() const
	{
	    return vector4(b, r, a, g);
	}

	vector4 brab() const
	{
	    return vector4(b, r, a, b);
	}

	vector4 braa() const
	{
	    return vector4(b, r, a, a);
	}

	vector4 bgrr() const
	{
	    return vector4(b, g, r, r);
	}

	vector4 bgrg() const
	{
	    return vector4(b, g, r, g);
	}

	vector4 bgrb() const
	{
	    return vector4(b, g, r, b);
	}

	vector4 bgra() const
	{
	    return vector4(b, g, r, a);
	}

	vector4 bggr() const
	{
	    return vector4(b, g, g, r);
	}

	vector4 bggg() const
	{
	    return vector4(b, g, g, g);
	}

	vector4 bggb() const
	{
	    return vector4(b, g, g, b);
	}

	vector4 bgga() const
	{
	    return vector4(b, g, g, a);
	}

	vector4 bgbr() const
	{
	    return vector4(b, g, b, r);
	}

	vector4 bgbg() const
	{
	    return vector4(b, g, b, g);
	}

	vector4 bgbb() const
	{
	    return vector4(b, g, b, b);
	}

	vector4 bgba() const
	{
	    return vector4(b, g, b, a);
	}

	vector4 bgar() const
	{
	    return vector4(b, g, a, r);
	}

	vector4 bgag() const
	{
	    return vector4(b, g, a, g);
	}

	vector4 bgab() const
	{
	    return vector4(b, g, a, b);
	}

	vector4 bgaa() const
	{
	    return vector4(b, g, a, a);
	}

	vector4 bbrr() const
	{
	    return vector4(b, b, r, r);
	}

	vector4 bbrg() const
	{
	    return vector4(b, b, r, g);
	}

	vector4 bbrb() const
	{
	    return vector4(b, b, r, b);
	}

	vector4 bbra() const
	{
	    return vector4(b, b, r, a);
	}

	vector4 bbgr() const
	{
	    return vector4(b, b, g, r);
	}

	vector4 bbgg() const
	{
	    return vector4(b, b, g, g);
	}

	vector4 bbgb() const
	{
	    return vector4(b, b, g, b);
	}

	vector4 bbga() const
	{
	    return vector4(b, b, g, a);
	}

	vector4 bbbr() const
	{
	    return vector4(b, b, b, r);
	}

	vector4 bbbg() const
	{
	    return vector4(b, b, b, g);
	}

	vector4 bbbb() const
	{
	    return vector4(b, b, b, b);
	}

	vector4 bbba() const
	{
	    return vector4(b, b, b, a);
	}

	vector4 bbar() const
	{
	    return vector4(b, b, a, r);
	}

	vector4 bbag() const
	{
	    return vector4(b, b, a, g);
	}

	vector4 bbab() const
	{
	    return vector4(b, b, a, b);
	}

	vector4 bbaa() const
	{
	    return vector4(b, b, a, a);
	}

	vector4 barr() const
	{
	    return vector4(b, a, r, r);
	}

	vector4 barg() const
	{
	    return vector4(b, a, r, g);
	}

	vector4 barb() const
	{
	    return vector4(b, a, r, b);
	}

	vector4 bara() const
	{
	    return vector4(b, a, r, a);
	}

	vector4 bagr() const
	{
	    return vector4(b, a, g, r);
	}

	vector4 bagg() const
	{
	    return vector4(b, a, g, g);
	}

	vector4 bagb() const
	{
	    return vector4(b, a, g, b);
	}

	vector4 baga() const
	{
	    return vector4(b, a, g, a);
	}

	vector4 babr() const
	{
	    return vector4(b, a, b, r);
	}

	vector4 babg() const
	{
	    return vector4(b, a, b, g);
	}

	vector4 babb() const
	{
	    return vector4(b, a, b, b);
	}

	vector4 baba() const
	{
	    return vector4(b, a, b, a);
	}

	vector4 baar() const
	{
	    return vector4(b, a, a, r);
	}

	vector4 baag() const
	{
	    return vector4(b, a, a, g);
	}

	vector4 baab() const
	{
	    return vector4(b, a, a, b);
	}

	vector4 baaa() const
	{
	    return vector4(b, a, a, a);
	}

	vector4 arrr() const
	{
	    return vector4(a, r, r, r);
	}

	vector4 arrg() const
	{
	    return vector4(a, r, r, g);
	}

	vector4 arrb() const
	{
	    return vector4(a, r, r, b);
	}

	vector4 arra() const
	{
	    return vector4(a, r, r, a);
	}

	vector4 argr() const
	{
	    return vector4(a, r, g, r);
	}

	vector4 argg() const
	{
	    return vector4(a, r, g, g);
	}

	vector4 argb() const
	{
	    return vector4(a, r, g, b);
	}

	vector4 arga() const
	{
	    return vector4(a, r, g, a);
	}

	vector4 arbr() const
	{
	    return vector4(a, r, b, r);
	}

	vector4 arbg() const
	{
	    return vector4(a, r, b, g);
	}

	vector4 arbb() const
	{
	    return vector4(a, r, b, b);
	}

	vector4 arba() const
	{
	    return vector4(a, r, b, a);
	}

	vector4 arar() const
	{
	    return vector4(a, r, a, r);
	}

	vector4 arag() const
	{
	    return vector4(a, r, a, g);
	}

	vector4 arab() const
	{
	    return vector4(a, r, a, b);
	}

	vector4 araa() const
	{
	    return vector4(a, r, a, a);
	}

	vector4 agrr() const
	{
	    return vector4(a, g, r, r);
	}

	vector4 agrg() const
	{
	    return vector4(a, g, r, g);
	}

	vector4 agrb() const
	{
	    return vector4(a, g, r, b);
	}

	vector4 agra() const
	{
	    return vector4(a, g, r, a);
	}

	vector4 aggr() const
	{
	    return vector4(a, g, g, r);
	}

	vector4 aggg() const
	{
	    return vector4(a, g, g, g);
	}

	vector4 aggb() const
	{
	    return vector4(a, g, g, b);
	}

	vector4 agga() const
	{
	    return vector4(a, g, g, a);
	}

	vector4 agbr() const
	{
	    return vector4(a, g, b, r);
	}

	vector4 agbg() const
	{
	    return vector4(a, g, b, g);
	}

	vector4 agbb() const
	{
	    return vector4(a, g, b, b);
	}

	vector4 agba() const
	{
	    return vector4(a, g, b, a);
	}

	vector4 agar() const
	{
	    return vector4(a, g, a, r);
	}

	vector4 agag() const
	{
	    return vector4(a, g, a, g);
	}

	vector4 agab() const
	{
	    return vector4(a, g, a, b);
	}

	vector4 agaa() const
	{
	    return vector4(a, g, a, a);
	}

	vector4 abrr() const
	{
	    return vector4(a, b, r, r);
	}

	vector4 abrg() const
	{
	    return vector4(a, b, r, g);
	}

	vector4 abrb() const
	{
	    return vector4(a, b, r, b);
	}

	vector4 abra() const
	{
	    return vector4(a, b, r, a);
	}

	vector4 abgr() const
	{
	    return vector4(a, b, g, r);
	}

	vector4 abgg() const
	{
	    return vector4(a, b, g, g);
	}

	vector4 abgb() const
	{
	    return vector4(a, b, g, b);
	}

	vector4 abga() const
	{
	    return vector4(a, b, g, a);
	}

	vector4 abbr() const
	{
	    return vector4(a, b, b, r);
	}

	vector4 abbg() const
	{
	    return vector4(a, b, b, g);
	}

	vector4 abbb() const
	{
	    return vector4(a, b, b, b);
	}

	vector4 abba() const
	{
	    return vector4(a, b, b, a);
	}

	vector4 abar() const
	{
	    return vector4(a, b, a, r);
	}

	vector4 abag() const
	{
	    return vector4(a, b, a, g);
	}

	vector4 abab() const
	{
	    return vector4(a, b, a, b);
	}

	vector4 abaa() const
	{
	    return vector4(a, b, a, a);
	}

	vector4 aarr() const
	{
	    return vector4(a, a, r, r);
	}

	vector4 aarg() const
	{
	    return vector4(a, a, r, g);
	}

	vector4 aarb() const
	{
	    return vector4(a, a, r, b);
	}

	vector4 aara() const
	{
	    return vector4(a, a, r, a);
	}

	vector4 aagr() const
	{
	    return vector4(a, a, g, r);
	}

	vector4 aagg() const
	{
	    return vector4(a, a, g, g);
	}

	vector4 aagb() const
	{
	    return vector4(a, a, g, b);
	}

	vector4 aaga() const
	{
	    return vector4(a, a, g, a);
	}

	vector4 aabr() const
	{
	    return vector4(a, a, b, r);
	}

	vector4 aabg() const
	{
	    return vector4(a, a, b, g);
	}

	vector4 aabb() const
	{
	    return vector4(a, a, b, b);
	}

	vector4 aaba() const
	{
	    return vector4(a, a, b, a);
	}

	vector4 aaar() const
	{
	    return vector4(a, a, a, r);
	}

	vector4 aaag() const
	{
	    return vector4(a, a, a, g);
	}

	vector4 aaab() const
	{
	    return vector4(a, a, a, b);
	}

	vector4 aaaa() const
	{
	    return vector4(a, a, a, a);
	}

	vector2<T> ss() const
	{
	    return vector2<T>(s, s);
	}

	vector2<T> st() const
	{
	    return vector2<T>(s, t);
	}

	vector2<T> sp() const
	{
	    return vector2<T>(s, p);
	}

	vector2<T> sq() const
	{
	    return vector2<T>(s, q);
	}

	vector2<T> ts() const
	{
	    return vector2<T>(t, s);
	}

	vector2<T> tt() const
	{
	    return vector2<T>(t, t);
	}

	vector2<T> tp() const
	{
	    return vector2<T>(t, p);
	}

	vector2<T> tq() const
	{
	    return vector2<T>(t, q);
	}

	vector2<T> ps() const
	{
	    return vector2<T>(p, s);
	}

	vector2<T> pt() const
	{
	    return vector2<T>(p, t);
	}

	vector2<T> pp() const
	{
	    return vector2<T>(p, p);
	}

	vector2<T> pq() const
	{
	    return vector2<T>(p, q);
	}

	vector2<T> qs() const
	{
	    return vector2<T>(q, s);
	}

	vector2<T> qt() const
	{
	    return vector2<T>(q, t);
	}

	vector2<T> qp() const
	{
	    return vector2<T>(q, p);
	}

	vector2<T> qq() const
	{
	    return vector2<T>(q, q);
	}

	vector3<T> sss() const
	{
	    return vector3<T>(s, s, s);
	}

	vector3<T> sst() const
	{
	    return vector3<T>(s, s, t);
	}

	vector3<T> ssp() const
	{
	    return vector3<T>(s, s, p);
	}

	vector3<T> ssq() const
	{
	    return vector3<T>(s, s, q);
	}

	vector3<T> sts() const
	{
	    return vector3<T>(s, t, s);
	}

	vector3<T> stt() const
	{
	    return vector3<T>(s, t, t);
	}

	vector3<T> stp() const
	{
	    return vector3<T>(s, t, p);
	}

	vector3<T> stq() const
	{
	    return vector3<T>(s, t, q);
	}

	vector3<T> sps() const
	{
	    return vector3<T>(s, p, s);
	}

	vector3<T> spt() const
	{
	    return vector3<T>(s, p, t);
	}

	vector3<T> spp() const
	{
	    return vector3<T>(s, p, p);
	}

	vector3<T> spq() const
	{
	    return vector3<T>(s, q, q);
	}

	vector3<T> sqs() const
	{
	    return vector3<T>(s, q, s);
	}

	vector3<T> sqt() const
	{
	    return vector3<T>(s, q, t);
	}

	vector3<T> sqp() const
	{
	    return vector3<T>(s, q, p);
	}

	vector3<T> sqq() const
	{
	    return vector3<T>(s, q, q);
	}

	vector3<T> tss() const
	{
	    return vector3<T>(t, s, s);
	}

	vector3<T> tst() const
	{
	    return vector3<T>(t, s, t);
	}

	vector3<T> tsp() const
	{
	    return vector3<T>(t, s, p);
	}

	vector3<T> tsq() const
	{
	    return vector3<T>(t, s, q);
	}

	vector3<T> tts() const
	{
	    return vector3<T>(t, t, s);
	}

	vector3<T> ttt() const
	{
	    return vector3<T>(t, t, t);
	}

	vector3<T> ttp() const
	{
	    return vector3<T>(t, t, p);
	}

	vector3<T> ttq() const
	{
	    return vector3<T>(t, t, q);
	}

	vector3<T> tps() const
	{
	    return vector3<T>(t, p, s);
	}

	vector3<T> tpt() const
	{
	    return vector3<T>(t, p, t);
	}

	vector3<T> tpp() const
	{
	    return vector3<T>(t, p, p);
	}

	vector3<T> tpq() const
	{
	    return vector3<T>(t, q, q);
	}

	vector3<T> tqs() const
	{
	    return vector3<T>(t, q, s);
	}

	vector3<T> tqt() const
	{
	    return vector3<T>(t, q, t);
	}

	vector3<T> tqp() const
	{
	    return vector3<T>(t, q, p);
	}

	vector3<T> tqq() const
	{
	    return vector3<T>(t, q, q);
	}

	vector3<T> pss() const
	{
	    return vector3<T>(p, s, s);
	}

	vector3<T> pst() const
	{
	    return vector3<T>(p, s, t);
	}

	vector3<T> psp() const
	{
	    return vector3<T>(p, s, p);
	}

	vector3<T> psq() const
	{
	    return vector3<T>(p, s, q);
	}

	vector3<T> pts() const
	{
	    return vector3<T>(p, t, s);
	}

	vector3<T> ptt() const
	{
	    return vector3<T>(p, t, t);
	}

	vector3<T> ptp() const
	{
	    return vector3<T>(p, t, p);
	}

	vector3<T> ptq() const
	{
	    return vector3<T>(p, t, q);
	}

	vector3<T> pps() const
	{
	    return vector3<T>(p, p, s);
	}

	vector3<T> ppt() const
	{
	    return vector3<T>(p, p, t);
	}

	vector3<T> ppp() const
	{
	    return vector3<T>(p, p, p);
	}

	vector3<T> ppq() const
	{
	    return vector3<T>(p, q, q);
	}

	vector3<T> pqs() const
	{
	    return vector3<T>(p, q, s);
	}

	vector3<T> pqt() const
	{
	    return vector3<T>(p, q, t);
	}

	vector3<T> pqp() const
	{
	    return vector3<T>(p, q, p);
	}

	vector3<T> pqq() const
	{
	    return vector3<T>(p, q, q);
	}

	vector3<T> qss() const
	{
	    return vector3<T>(q, s, s);
	}

	vector3<T> qst() const
	{
	    return vector3<T>(q, s, t);
	}

	vector3<T> qsp() const
	{
	    return vector3<T>(q, s, p);
	}

	vector3<T> qsq() const
	{
	    return vector3<T>(q, s, q);
	}

	vector3<T> qts() const
	{
	    return vector3<T>(q, t, s);
	}

	vector3<T> qtt() const
	{
	    return vector3<T>(q, t, t);
	}

	vector3<T> qtp() const
	{
	    return vector3<T>(q, t, p);
	}

	vector3<T> qtq() const
	{
	    return vector3<T>(q, t, q);
	}

	vector3<T> qps() const
	{
	    return vector3<T>(q, p, s);
	}

	vector3<T> qpt() const
	{
	    return vector3<T>(q, p, t);
	}

	vector3<T> qpp() const
	{
	    return vector3<T>(q, p, p);
	}

	vector3<T> qpq() const
	{
	    return vector3<T>(q, q, q);
	}

	vector3<T> qqs() const
	{
	    return vector3<T>(q, q, s);
	}

	vector3<T> qqt() const
	{
	    return vector3<T>(q, q, t);
	}

	vector3<T> qqp() const
	{
	    return vector3<T>(q, q, p);
	}

	vector3<T> qqq() const
	{
	    return vector3<T>(q, q, q);
	}

	vector4 ssss() const
	{
	    return vector4(s, s, s, s);
	}

	vector4 ssst() const
	{
	    return vector4(s, s, s, t);
	}

	vector4 sssp() const
	{
	    return vector4(s, s, s, p);
	}

	vector4 sssq() const
	{
	    return vector4(s, s, s, q);
	}

	vector4 ssts() const
	{
	    return vector4(s, s, t, s);
	}

	vector4 sstt() const
	{
	    return vector4(s, s, t, t);
	}

	vector4 sstp() const
	{
	    return vector4(s, s, t, p);
	}

	vector4 sstq() const
	{
	    return vector4(s, s, t, q);
	}

	vector4 ssps() const
	{
	    return vector4(s, s, p, s);
	}

	vector4 sspt() const
	{
	    return vector4(s, s, p, t);
	}

	vector4 sspp() const
	{
	    return vector4(s, s, p, p);
	}

	vector4 sspq() const
	{
	    return vector4(s, s, p, q);
	}

	vector4 ssqs() const
	{
	    return vector4(s, s, q, s);
	}

	vector4 ssqt() const
	{
	    return vector4(s, s, q, t);
	}

	vector4 ssqp() const
	{
	    return vector4(s, s, q, p);
	}

	vector4 ssqq() const
	{
	    return vector4(s, s, q, q);
	}

	vector4 stss() const
	{
	    return vector4(s, t, s, s);
	}

	vector4 stst() const
	{
	    return vector4(s, t, s, t);
	}

	vector4 stsp() const
	{
	    return vector4(s, t, s, p);
	}

	vector4 stsq() const
	{
	    return vector4(s, t, s, q);
	}

	vector4 stts() const
	{
	    return vector4(s, t, t, s);
	}

	vector4 sttt() const
	{
	    return vector4(s, t, t, t);
	}

	vector4 sttp() const
	{
	    return vector4(s, t, t, p);
	}

	vector4 sttq() const
	{
	    return vector4(s, t, t, q);
	}

	vector4 stps() const
	{
	    return vector4(s, t, p, s);
	}

	vector4 stpt() const
	{
	    return vector4(s, t, p, t);
	}

	vector4 stpp() const
	{
	    return vector4(s, t, p, p);
	}

	vector4 stpq() const
	{
	    return vector4(s, t, p, q);
	}

	vector4 stqs() const
	{
	    return vector4(s, t, q, s);
	}

	vector4 stqt() const
	{
	    return vector4(s, t, q, t);
	}

	vector4 stqp() const
	{
	    return vector4(s, t, q, p);
	}

	vector4 stqq() const
	{
	    return vector4(s, t, q, q);
	}

	vector4 spss() const
	{
	    return vector4(s, p, s, s);
	}

	vector4 spst() const
	{
	    return vector4(s, p, s, t);
	}

	vector4 spsp() const
	{
	    return vector4(s, p, s, p);
	}

	vector4 spsq() const
	{
	    return vector4(s, p, s, q);
	}

	vector4 spts() const
	{
	    return vector4(s, p, t, s);
	}

	vector4 sptt() const
	{
	    return vector4(s, p, t, t);
	}

	vector4 sptp() const
	{
	    return vector4(s, p, t, p);
	}

	vector4 sptq() const
	{
	    return vector4(s, p, t, q);
	}

	vector4 spps() const
	{
	    return vector4(s, p, p, s);
	}

	vector4 sppt() const
	{
	    return vector4(s, p, p, t);
	}

	vector4 sppp() const
	{
	    return vector4(s, p, p, p);
	}

	vector4 sppq() const
	{
	    return vector4(s, p, p, q);
	}

	vector4 spqs() const
	{
	    return vector4(s, p, q, s);
	}

	vector4 spqt() const
	{
	    return vector4(s, p, q, t);
	}

	vector4 spqp() const
	{
	    return vector4(s, p, q, p);
	}

	vector4 spqq() const
	{
	    return vector4(s, p, q, q);
	}

	vector4 sqss() const
	{
	    return vector4(s, q, s, s);
	}

	vector4 sqst() const
	{
	    return vector4(s, q, s, t);
	}

	vector4 sqsp() const
	{
	    return vector4(s, q, s, p);
	}

	vector4 sqsq() const
	{
	    return vector4(s, q, s, q);
	}

	vector4 sqts() const
	{
	    return vector4(s, q, t, s);
	}

	vector4 sqtt() const
	{
	    return vector4(s, q, t, t);
	}

	vector4 sqtp() const
	{
	    return vector4(s, q, t, p);
	}

	vector4 sqtq() const
	{
	    return vector4(s, q, t, q);
	}

	vector4 sqps() const
	{
	    return vector4(s, q, p, s);
	}

	vector4 sqpt() const
	{
	    return vector4(s, q, p, t);
	}

	vector4 sqpp() const
	{
	    return vector4(s, q, p, p);
	}

	vector4 sqpq() const
	{
	    return vector4(s, q, p, q);
	}

	vector4 sqqs() const
	{
	    return vector4(s, q, q, s);
	}

	vector4 sqqt() const
	{
	    return vector4(s, q, q, t);
	}

	vector4 sqqp() const
	{
	    return vector4(s, q, q, p);
	}

	vector4 sqqq() const
	{
	    return vector4(s, q, q, q);
	}

	vector4 tsss() const
	{
	    return vector4(t, s, s, s);
	}

	vector4 tsst() const
	{
	    return vector4(t, s, s, t);
	}

	vector4 tssp() const
	{
	    return vector4(t, s, s, p);
	}

	vector4 tssq() const
	{
	    return vector4(t, s, s, q);
	}

	vector4 tsts() const
	{
	    return vector4(t, s, t, s);
	}

	vector4 tstt() const
	{
	    return vector4(t, s, t, t);
	}

	vector4 tstp() const
	{
	    return vector4(t, s, t, p);
	}

	vector4 tstq() const
	{
	    return vector4(t, s, t, q);
	}

	vector4 tsps() const
	{
	    return vector4(t, s, p, s);
	}

	vector4 tspt() const
	{
	    return vector4(t, s, p, t);
	}

	vector4 tspp() const
	{
	    return vector4(t, s, p, p);
	}

	vector4 tspq() const
	{
	    return vector4(t, s, p, q);
	}

	vector4 tsqs() const
	{
	    return vector4(t, s, q, s);
	}

	vector4 tsqt() const
	{
	    return vector4(t, s, q, t);
	}

	vector4 tsqp() const
	{
	    return vector4(t, s, q, p);
	}

	vector4 tsqq() const
	{
	    return vector4(t, s, q, q);
	}

	vector4 ttss() const
	{
	    return vector4(t, t, s, s);
	}

	vector4 ttst() const
	{
	    return vector4(t, t, s, t);
	}

	vector4 ttsp() const
	{
	    return vector4(t, t, s, p);
	}

	vector4 ttsq() const
	{
	    return vector4(t, t, s, q);
	}

	vector4 ttts() const
	{
	    return vector4(t, t, t, s);
	}

	vector4 tttt() const
	{
	    return vector4(t, t, t, t);
	}

	vector4 tttp() const
	{
	    return vector4(t, t, t, p);
	}

	vector4 tttq() const
	{
	    return vector4(t, t, t, q);
	}

	vector4 ttps() const
	{
	    return vector4(t, t, p, s);
	}

	vector4 ttpt() const
	{
	    return vector4(t, t, p, t);
	}

	vector4 ttpp() const
	{
	    return vector4(t, t, p, p);
	}

	vector4 ttpq() const
	{
	    return vector4(t, t, p, q);
	}

	vector4 ttqs() const
	{
	    return vector4(t, t, q, s);
	}

	vector4 ttqt() const
	{
	    return vector4(t, t, q, t);
	}

	vector4 ttqp() const
	{
	    return vector4(t, t, q, p);
	}

	vector4 ttqq() const
	{
	    return vector4(t, t, q, q);
	}

	vector4 tpss() const
	{
	    return vector4(t, p, s, s);
	}

	vector4 tpst() const
	{
	    return vector4(t, p, s, t);
	}

	vector4 tpsp() const
	{
	    return vector4(t, p, s, p);
	}

	vector4 tpsq() const
	{
	    return vector4(t, p, s, q);
	}

	vector4 tpts() const
	{
	    return vector4(t, p, t, s);
	}

	vector4 tptt() const
	{
	    return vector4(t, p, t, t);
	}

	vector4 tptp() const
	{
	    return vector4(t, p, t, p);
	}

	vector4 tptq() const
	{
	    return vector4(t, p, t, q);
	}

	vector4 tpps() const
	{
	    return vector4(t, p, p, s);
	}

	vector4 tppt() const
	{
	    return vector4(t, p, p, t);
	}

	vector4 tppp() const
	{
	    return vector4(t, p, p, p);
	}

	vector4 tppq() const
	{
	    return vector4(t, p, p, q);
	}

	vector4 tpqs() const
	{
	    return vector4(t, p, q, s);
	}

	vector4 tpqt() const
	{
	    return vector4(t, p, q, t);
	}

	vector4 tpqp() const
	{
	    return vector4(t, p, q, p);
	}

	vector4 tpqq() const
	{
	    return vector4(t, p, q, q);
	}

	vector4 tqss() const
	{
	    return vector4(t, q, s, s);
	}

	vector4 tqst() const
	{
	    return vector4(t, q, s, t);
	}

	vector4 tqsp() const
	{
	    return vector4(t, q, s, p);
	}

	vector4 tqsq() const
	{
	    return vector4(t, q, s, q);
	}

	vector4 tqts() const
	{
	    return vector4(t, q, t, s);
	}

	vector4 tqtt() const
	{
	    return vector4(t, q, t, t);
	}

	vector4 tqtp() const
	{
	    return vector4(t, q, t, p);
	}

	vector4 tqtq() const
	{
	    return vector4(t, q, t, q);
	}

	vector4 tqps() const
	{
	    return vector4(t, q, p, s);
	}

	vector4 tqpt() const
	{
	    return vector4(t, q, p, t);
	}

	vector4 tqpp() const
	{
	    return vector4(t, q, p, p);
	}

	vector4 tqpq() const
	{
	    return vector4(t, q, p, q);
	}

	vector4 tqqs() const
	{
	    return vector4(t, q, q, s);
	}

	vector4 tqqt() const
	{
	    return vector4(t, q, q, t);
	}

	vector4 tqqp() const
	{
	    return vector4(t, q, q, p);
	}

	vector4 tqqq() const
	{
	    return vector4(t, q, q, q);
	}

	vector4 psss() const
	{
	    return vector4(p, s, s, s);
	}

	vector4 psst() const
	{
	    return vector4(p, s, s, t);
	}

	vector4 pssp() const
	{
	    return vector4(p, s, s, p);
	}

	vector4 pssq() const
	{
	    return vector4(p, s, s, q);
	}

	vector4 psts() const
	{
	    return vector4(p, s, t, s);
	}

	vector4 pstt() const
	{
	    return vector4(p, s, t, t);
	}

	vector4 pstp() const
	{
	    return vector4(p, s, t, p);
	}

	vector4 pstq() const
	{
	    return vector4(p, s, t, q);
	}

	vector4 psps() const
	{
	    return vector4(p, s, p, s);
	}

	vector4 pspt() const
	{
	    return vector4(p, s, p, t);
	}

	vector4 pspp() const
	{
	    return vector4(p, s, p, p);
	}

	vector4 pspq() const
	{
	    return vector4(p, s, p, q);
	}

	vector4 psqs() const
	{
	    return vector4(p, s, q, s);
	}

	vector4 psqt() const
	{
	    return vector4(p, s, q, t);
	}

	vector4 psqp() const
	{
	    return vector4(p, s, q, p);
	}

	vector4 psqq() const
	{
	    return vector4(p, s, q, q);
	}

	vector4 ptss() const
	{
	    return vector4(p, t, s, s);
	}

	vector4 ptst() const
	{
	    return vector4(p, t, s, t);
	}

	vector4 ptsp() const
	{
	    return vector4(p, t, s, p);
	}

	vector4 ptsq() const
	{
	    return vector4(p, t, s, q);
	}

	vector4 ptts() const
	{
	    return vector4(p, t, t, s);
	}

	vector4 pttt() const
	{
	    return vector4(p, t, t, t);
	}

	vector4 pttp() const
	{
	    return vector4(p, t, t, p);
	}

	vector4 pttq() const
	{
	    return vector4(p, t, t, q);
	}

	vector4 ptps() const
	{
	    return vector4(p, t, p, s);
	}

	vector4 ptpt() const
	{
	    return vector4(p, t, p, t);
	}

	vector4 ptpp() const
	{
	    return vector4(p, t, p, p);
	}

	vector4 ptpq() const
	{
	    return vector4(p, t, p, q);
	}

	vector4 ptqs() const
	{
	    return vector4(p, t, q, s);
	}

	vector4 ptqt() const
	{
	    return vector4(p, t, q, t);
	}

	vector4 ptqp() const
	{
	    return vector4(p, t, q, p);
	}

	vector4 ptqq() const
	{
	    return vector4(p, t, q, q);
	}

	vector4 ppss() const
	{
	    return vector4(p, p, s, s);
	}

	vector4 ppst() const
	{
	    return vector4(p, p, s, t);
	}

	vector4 ppsp() const
	{
	    return vector4(p, p, s, p);
	}

	vector4 ppsq() const
	{
	    return vector4(p, p, s, q);
	}

	vector4 ppts() const
	{
	    return vector4(p, p, t, s);
	}

	vector4 pptt() const
	{
	    return vector4(p, p, t, t);
	}

	vector4 pptp() const
	{
	    return vector4(p, p, t, p);
	}

	vector4 pptq() const
	{
	    return vector4(p, p, t, q);
	}

	vector4 ppps() const
	{
	    return vector4(p, p, p, s);
	}

	vector4 pppt() const
	{
	    return vector4(p, p, p, t);
	}

	vector4 pppp() const
	{
	    return vector4(p, p, p, p);
	}

	vector4 pppq() const
	{
	    return vector4(p, p, p, q);
	}

	vector4 ppqs() const
	{
	    return vector4(p, p, q, s);
	}

	vector4 ppqt() const
	{
	    return vector4(p, p, q, t);
	}

	vector4 ppqp() const
	{
	    return vector4(p, p, q, p);
	}

	vector4 ppqq() const
	{
	    return vector4(p, p, q, q);
	}

	vector4 pqss() const
	{
	    return vector4(p, q, s, s);
	}

	vector4 pqst() const
	{
	    return vector4(p, q, s, t);
	}

	vector4 pqsp() const
	{
	    return vector4(p, q, s, p);
	}

	vector4 pqsq() const
	{
	    return vector4(p, q, s, q);
	}

	vector4 pqts() const
	{
	    return vector4(p, q, t, s);
	}

	vector4 pqtt() const
	{
	    return vector4(p, q, t, t);
	}

	vector4 pqtp() const
	{
	    return vector4(p, q, t, p);
	}

	vector4 pqtq() const
	{
	    return vector4(p, q, t, q);
	}

	vector4 pqps() const
	{
	    return vector4(p, q, p, s);
	}

	vector4 pqpt() const
	{
	    return vector4(p, q, p, t);
	}

	vector4 pqpp() const
	{
	    return vector4(p, q, p, p);
	}

	vector4 pqpq() const
	{
	    return vector4(p, q, p, q);
	}

	vector4 pqqs() const
	{
	    return vector4(p, q, q, s);
	}

	vector4 pqqt() const
	{
	    return vector4(p, q, q, t);
	}

	vector4 pqqp() const
	{
	    return vector4(p, q, q, p);
	}

	vector4 pqqq() const
	{
	    return vector4(p, q, q, q);
	}

	vector4 qsss() const
	{
	    return vector4(q, s, s, s);
	}

	vector4 qsst() const
	{
	    return vector4(q, s, s, t);
	}

	vector4 qssp() const
	{
	    return vector4(q, s, s, p);
	}

	vector4 qssq() const
	{
	    return vector4(q, s, s, q);
	}

	vector4 qsts() const
	{
	    return vector4(q, s, t, s);
	}

	vector4 qstt() const
	{
	    return vector4(q, s, t, t);
	}

	vector4 qstp() const
	{
	    return vector4(q, s, t, p);
	}

	vector4 qstq() const
	{
	    return vector4(q, s, t, q);
	}

	vector4 qsps() const
	{
	    return vector4(q, s, p, s);
	}

	vector4 qspt() const
	{
	    return vector4(q, s, p, t);
	}

	vector4 qspp() const
	{
	    return vector4(q, s, p, p);
	}

	vector4 qspq() const
	{
	    return vector4(q, s, p, q);
	}

	vector4 qsqs() const
	{
	    return vector4(q, s, q, s);
	}

	vector4 qsqt() const
	{
	    return vector4(q, s, q, t);
	}

	vector4 qsqp() const
	{
	    return vector4(q, s, q, p);
	}

	vector4 qsqq() const
	{
	    return vector4(q, s, q, q);
	}

	vector4 qtss() const
	{
	    return vector4(q, t, s, s);
	}

	vector4 qtst() const
	{
	    return vector4(q, t, s, t);
	}

	vector4 qtsp() const
	{
	    return vector4(q, t, s, p);
	}

	vector4 qtsq() const
	{
	    return vector4(q, t, s, q);
	}

	vector4 qtts() const
	{
	    return vector4(q, t, t, s);
	}

	vector4 qttt() const
	{
	    return vector4(q, t, t, t);
	}

	vector4 qttp() const
	{
	    return vector4(q, t, t, p);
	}

	vector4 qttq() const
	{
	    return vector4(q, t, t, q);
	}

	vector4 qtps() const
	{
	    return vector4(q, t, p, s);
	}

	vector4 qtpt() const
	{
	    return vector4(q, t, p, t);
	}

	vector4 qtpp() const
	{
	    return vector4(q, t, p, p);
	}

	vector4 qtpq() const
	{
	    return vector4(q, t, p, q);
	}

	vector4 qtqs() const
	{
	    return vector4(q, t, q, s);
	}

	vector4 qtqt() const
	{
	    return vector4(q, t, q, t);
	}

	vector4 qtqp() const
	{
	    return vector4(q, t, q, p);
	}

	vector4 qtqq() const
	{
	    return vector4(q, t, q, q);
	}

	vector4 qpss() const
	{
	    return vector4(q, p, s, s);
	}

	vector4 qpst() const
	{
	    return vector4(q, p, s, t);
	}

	vector4 qpsp() const
	{
	    return vector4(q, p, s, p);
	}

	vector4 qpsq() const
	{
	    return vector4(q, p, s, q);
	}

	vector4 qpts() const
	{
	    return vector4(q, p, t, s);
	}

	vector4 qptt() const
	{
	    return vector4(q, p, t, t);
	}

	vector4 qptp() const
	{
	    return vector4(q, p, t, p);
	}

	vector4 qptq() const
	{
	    return vector4(q, p, t, q);
	}

	vector4 qpps() const
	{
	    return vector4(q, p, p, s);
	}

	vector4 qppt() const
	{
	    return vector4(q, p, p, t);
	}

	vector4 qppp() const
	{
	    return vector4(q, p, p, p);
	}

	vector4 qppq() const
	{
	    return vector4(q, p, p, q);
	}

	vector4 qpqs() const
	{
	    return vector4(q, p, q, s);
	}

	vector4 qpqt() const
	{
	    return vector4(q, p, q, t);
	}

	vector4 qpqp() const
	{
	    return vector4(q, p, q, p);
	}

	vector4 qpqq() const
	{
	    return vector4(q, p, q, q);
	}

	vector4 qqss() const
	{
	    return vector4(q, q, s, s);
	}

	vector4 qqst() const
	{
	    return vector4(q, q, s, t);
	}

	vector4 qqsp() const
	{
	    return vector4(q, q, s, p);
	}

	vector4 qqsq() const
	{
	    return vector4(q, q, s, q);
	}

	vector4 qqts() const
	{
	    return vector4(q, q, t, s);
	}

	vector4 qqtt() const
	{
	    return vector4(q, q, t, t);
	}

	vector4 qqtp() const
	{
	    return vector4(q, q, t, p);
	}

	vector4 qqtq() const
	{
	    return vector4(q, q, t, q);
	}

	vector4 qqps() const
	{
	    return vector4(q, q, p, s);
	}

	vector4 qqpt() const
	{
	    return vector4(q, q, p, t);
	}

	vector4 qqpp() const
	{
	    return vector4(q, q, p, p);
	}

	vector4 qqpq() const
	{
	    return vector4(q, q, p, q);
	}

	vector4 qqqs() const
	{
	    return vector4(q, q, q, s);
	}

	vector4 qqqt() const
	{
	    return vector4(q, q, q, t);
	}

	vector4 qqqp() const
	{
	    return vector4(q, q, q, p);
	}

	vector4 qqqq() const
	{
	    return vector4(q, q, q, q);
	}

	/* Operators */

	const vector4 &operator=(const vector4 &v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    w = v.w;
	    return *this;
	}

	T &operator[](size_t i)
	{
	    return v[i];
	}

	const T &operator[](size_t i) const
	{
	    return v[i];
	}
	
	vector4 operator*(const T s) const
	{
	    return vector4(x * s, y * s, z * s, w * s);
	}

    	friend vector4 operator*(const T s, const vector4 &w)
	{
	    return w * s;
	}

	vector4 operator/(const T s) const
	{
	    return vector4(x / s, y / s, z / s, w / s);
	}
     
	const vector4 &operator*=(const T s)
	{
	    x *= s;
	    y *= s;
	    z *= s;
	    w *= s;
	    return *this;
	}
     
	const vector4 &operator/=(const T s)
	{
	    x /= s;
	    y /= s;
	    z /= s;
	    w /= s;
	    return *this;
	}

	vector4 operator+(const vector4 &v) const
	{
	    return vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	vector4 operator-(const vector4 &v) const
	{
	    return vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	vector4 operator*(const vector4 &v) const 
	{
	    return vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	vector4 operator/(const vector4 &v) const 
	{
	    return vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	vector4 operator-() const
	{
	    return vector4(-x, -y, -z, -w);
	}

	const vector4 &operator+=(const vector4 &v)
	{
	    x += v.x;
	    y += v.y;
	    z += v.z;
	    w += v.w;
	    return *this;
	}

	const vector4 &operator-=(const vector4 &v)
	{
	    x -= v.x;
	    y -= v.y;
	    z -= v.z;
	    w -= v.w;
	    return *this;
	}

	const vector4 &operator*=(const vector4 &v)
	{
	    x *= v.x;
	    y *= v.y;
	    z *= v.z;
	    w *= v.w;
	    return *this;
	}

	const vector4 &operator/=(const vector4 &v)
	{
	    x /= v.x;
	    y /= v.y;
	    z /= v.z;
	    w /= v.w;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const vector4 &v)
	{
	    return os << v.str();
	}

	friend std::istream &operator>>(std::istream &is, vector4 &v)
	{
	    is >> v.x >> v.y >> v.z >> v.w;
	    return is;
	}

	/* Trigonometric functions */

	vector4 sin() const
	{
	    return vector4(glvm::sin(x), glvm::sin(y), glvm::sin(z), glvm::sin(w));
	}

	friend vector4 sin(const vector4 &v)
	{
    	    return v.sin();
       	}

	vector4 cos() const
	{
	    return vector4(glvm::cos(x), glvm::cos(y), glvm::cos(z), glvm::cos(w));
	}

	friend vector4 cos(const vector4 &v)
	{
    	    return v.cos();
       	}

	vector4 tan() const
	{
	    return vector4(glvm::tan(x), glvm::tan(y), glvm::tan(z), glvm::tan(w));
	}

	friend vector4 tan(const vector4 &v)
	{
    	    return v.tan();
       	}

	vector4 asin() const
	{
	    return vector4(glvm::asin(x), glvm::asin(y), glvm::asin(z), glvm::asin(w));
	}

	friend vector4 asin(const vector4 &v)
	{
    	    return v.asin();
       	}

	vector4 acos() const
	{
	    return vector4(glvm::acos(x), glvm::acos(y), glvm::acos(z), glvm::acos(w));
	}

	friend vector4 acos(const vector4 &v)
	{
    	    return v.acos();
       	}

	vector4 atan() const
	{
	    return vector4(glvm::atan(x), glvm::atan(y), glvm::atan(z), glvm::atan(w));
	}

	friend vector4 atan(const vector4 &v)
	{
    	    return v.atan();
       	}

	vector4 atan(const vector4 &v) const
	{
	    return vector4(glvm::atan(x, v.x), glvm::atan(y, v.y), glvm::atan(z, v.z), glvm::atan(w, v.w));
	}

	friend vector4 atan(const vector4 &v, const vector4 &w)
	{
    	    return v.atan(w);
       	}

	vector4 radians() const
	{
	    return *this * static_cast<T>(M_PI / 180.0);
	}

	friend vector4 radians(const vector4 &v)
	{
    	    return v.radians();
       	}

	vector4 degrees() const
	{
	    return *this * static_cast<T>(180.0 / M_PI);
	}

	friend vector4 degrees(const vector4 &v)
	{
    	    return v.degrees();
       	}

	/* Exponential functions */

	vector4 pow(const T p) const
	{
	    return vector4(glvm::pow(x, p), glvm::pow(y, p), glvm::pow(z, p), glvm::pow(w, p));
	}

	friend vector4 pow(const vector4 &v, const T p)
	{
    	    return v.pow(p);
       	}

	vector4 exp() const
	{
	    return vector4(glvm::exp(x), glvm::exp(y), glvm::exp(z), glvm::exp(w));
	}

	friend vector4 exp(const vector4 &v)
	{
    	    return v.exp();
       	}

	vector4 exp2() const
	{
	    return vector4(glvm::exp2(x), glvm::exp2(y), glvm::exp2(z), glvm::exp2(w));
	}

	friend vector4 exp2(const vector4 &v)
	{
    	    return v.exp2();
       	}

	vector4 log() const
	{
	    return vector4(glvm::log(x), glvm::log(y), glvm::log(z), glvm::log(w));
	}

	friend vector4 log(const vector4 &v)
	{
    	    return v.log();
       	}

	vector4 log2() const
	{
	    return vector4(glvm::log2(x), glvm::log2(y), glvm::log2(z), glvm::log2(w));
	}

	friend vector4 log2(const vector4 &v)
	{
    	    return v.log2();
       	}

	vector4 log10() const
	{
	    return vector4(glvm::log10(x), glvm::log10(y), glvm::log10(z), glvm::log10(w));
	}

	friend vector4 log10(const vector4 &v)
	{
    	    return v.log10();
       	}

	vector4 sqrt() const
	{
	    return vector4(glvm::sqrt(x), glvm::sqrt(y), glvm::sqrt(z), glvm::sqrt(w));
	}

	friend vector4 sqrt(const vector4 &v)
	{
    	    return v.sqrt();
       	}

	vector4 inversesqrt() const
	{
	    return static_cast<T>(1) / *this.sqrt();
	}

	friend vector4 inversesqrt(const vector4 &v)
	{
    	    return v.inversesqrt();
       	}

	vector4 cbrt() const
	{
	    return vector4(glvm::cbrt(x), glvm::cbrt(y), glvm::cbrt(z), glvm::cbrt(w));
	}

	friend vector4 cbrt(const vector4 &v)
	{
    	    return v.cbrt();
       	}

	/* Common functions */

	vector4<bool> isfinite() const
	{
	    return vector4<bool>(glvm::isfinite(x), glvm::isfinite(y), glvm::isfinite(z), glvm::isfinite(w));
	}

	friend vector4<bool> isfinite(const vector4 &v)
	{
	    return v.isfinite();
	}

	vector4<bool> isinf() const
	{
	    return vector4<bool>(glvm::isinf(x), glvm::isinf(y), glvm::isinf(z), glvm::isinf(w));
	}

	friend vector4<bool> isinf(const vector4 &v)
	{
	    return v.isinf();
	}

	vector4<bool> isnan() const
	{
	    return vector4<bool>(glvm::isnan(x), glvm::isnan(y), glvm::isnan(z), glvm::isnan(w));
	}

	friend vector4<bool> isnan(const vector4 &v)
	{
	    return v.isnan();
	}

	vector4<bool> isnormal() const
	{
	    return vector4<bool>(glvm::isnormal(x), glvm::isnormal(y), glvm::isnormal(z), glvm::isnormal(w));
	}

	friend vector4<bool> isnormal(const vector4 &v)
	{
	    return v.isnormal();
	}

	vector4 abs() const
	{
	    return vector4(glvm::abs(x), glvm::abs(y), glvm::abs(z), glvm::abs(w));
	}

	friend vector4 abs(const vector4 &v)
	{
    	    return v.abs();
       	}

	vector4 sign() const
	{
	    return vector4(glvm::sign(x), glvm::sign(y), glvm::sign(z), glvm::sign(w));
	}

	friend vector4 sign(const vector4 &v)
	{
    	    return v.sign();
       	}

	vector4 floor() const
	{
	    return vector4<bool>(glvm::floor(x), glvm::floor(y), glvm::floor(z), glvm::floor(w));
	}

	friend vector4 floor(const vector4 &v)
	{
    	    return v.floor();
       	}

	vector4 ceil() const
	{
	    return vector4<bool>(glvm::ceil(x), glvm::ceil(y), glvm::ceil(z), glvm::ceil(w));
	}

	friend vector4 ceil(const vector4 &v)
	{
    	    return v.ceil();
       	}

	vector4 round() const
	{
	    return vector4<bool>(glvm::round(x), glvm::round(y), glvm::round(z), glvm::round(w));
	}

	friend vector4 round(const vector4 &v)
	{
    	    return v.round();
       	}

	vector4 fract() const
	{
	    return *this - this->floor();
	}

	friend vector4 fract(const vector4 &v)
	{
    	    return v.fract();
       	}

	vector4 min(const T v) const
	{
	    return vector4(v < x ? v : x, v < y ? v : y, v < z ? v : z, v < w ? v : w);
	}

	friend vector4 min(const vector4 &v, const T w)
	{
    	    return v.min(w);
       	}

	vector4 min(const vector4 &v) const
	{
	    return vector4(v.x < x ? v.x : x, v.y < y ? v.y : y, v.z < z ? v.z : z, v.w < w ? v.w : w);
	}

	friend vector4 min(const vector4 &v, const vector4 &w)
	{
    	    return v.min(w);
       	}

	vector4 max(const T v) const
	{
	    return vector4(v > x ? v : x, v > y ? v : y, v > z ? v : z, v > w ? v : w);
	}

	friend vector4 max(const vector4 &v, const T w)
	{
    	    return v.max(w);
       	}

	vector4 max(const vector4 &v) const
	{
	    return vector4(v.x > x ? v.x : x, v.y > y ? v.y : y, v.z > z ? v.z : z, v.w > w ? v.w : w);
	}

	friend vector4 max(const vector4 &v, const vector4 &w)
	{
    	    return v.max(w);
       	}

	vector4 clamp(const T minval, const T maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector4 clamp(const vector4 &v, const T minval, const T maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector4 clamp(const vector4 &minval, const vector4 &maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	friend vector4 clamp(const vector4 &v, const vector4 &minval, const vector4 &maxval)
	{
    	    return v.clamp(minval, maxval);
       	}

	vector4 mix(const vector4 &v, const T alpha) const
	{
	    return *this * (static_cast<T>(1) - alpha) + v * alpha;
	}

	friend vector4 mix(const vector4 &v, const vector4 &w, const T alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector4 mix(const vector4 &v, const vector4 &alpha) const
	{
	    return (vector4(static_cast<T>(1)) - alpha) * *this + alpha * v;
	}

	friend vector4 mix(const vector4 &v, const vector4 &w, const vector4 &alpha)
	{
    	    return v.mix(w, alpha);
       	}

	vector4 step(const T edge) const
	{
	    return vector4(
		    x < edge ? static_cast<T>(0) : static_cast<T>(1), 
		    y < edge ? static_cast<T>(0) : static_cast<T>(1),
		    z < edge ? static_cast<T>(0) : static_cast<T>(1),
		    w < edge ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector4 step(const vector4 &v, const T edge)
	{
    	    return v.step(edge);
       	}

	vector4 step(const vector4 &edge) const
	{
	    return vector4(
		    x < edge.x ? static_cast<T>(0) : static_cast<T>(1),
		    y < edge.y ? static_cast<T>(0) : static_cast<T>(1),
		    z < edge.z ? static_cast<T>(0) : static_cast<T>(1),
		    w < edge.w ? static_cast<T>(0) : static_cast<T>(1));
	}

	friend vector4 step(const vector4 &v, const vector4 &edge)
	{
    	    return v.step(edge);
       	}

	vector4 smoothstep(const T edge0, const T edge1) const
	{
	    vector4 t = (*this - vector4(edge0)) / (vector4(edge1) - vector4(edge0));
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector4(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector4 smoothstep(const vector4 &v, const T edge0, const T edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector4 smoothstep(const vector4 &edge0, const vector4 &edge1) const
	{
	    vector4 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(static_cast<T>(0), static_cast<T>(1));
	    return t * t * (vector4(static_cast<T>(3)) - t * static_cast<T>(2));
	}

	friend vector4 smoothstep(const vector4 &v, const vector4 &edge0, const vector4 &edge1)
	{
    	    return v.smoothstep(edge0, edge1);
       	}

	vector4 mod(const T y) const
	{
	    vector4 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector4 mod(const vector4 &v, const T y)
	{
    	    return v.mod(y);
       	}

	vector4 mod(const vector4 &y) const
	{
	    vector4 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	friend vector4 mod(const vector4 &v, const vector4 &y)
	{
    	    return v.mod(y);
       	}

	/* Geometric functions */

	T length() const
	{
	    return glvm::sqrt(x * x + y * y + z * z + w * w);
	}

	friend T length(const vector4 &v)
	{
    	    return v.length();
       	}

	T distance(const vector4 &v) const
	{
	    return vector4(*this - v).length();
	}

	friend T distance(const vector4 &v, const vector4 &w)
	{
    	    return v.distance(w);
       	}

	T dot(const vector4 &v) const
	{
	    return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	friend T dot(const vector4 &v, const vector4 &w)
	{
    	    return v.dot(w);
       	}

	vector4 normalize() const
	{
	    return *this / this->length();
	}

	friend vector4 normalize(const vector4 &v)
	{
    	    return v.normalize();
       	}

	vector4 faceforward(const vector4 &I, const vector4 &Nref) const
	{
	    return Nref.dot(I) < static_cast<T>(0) ? *this : - *this;
	}

	friend vector4 faceforward(const vector4 &N, const vector4 &I, const vector4 &Nref)
	{
    	    return N.faceforward(I, Nref);
       	}

	vector4 reflect(const vector4 &N) const
	{
	    return *this - N * (N.dot(*this) * static_cast<T>(2));
	}

	friend vector4 reflect(const vector4 &I, const vector4 &N)
	{
    	    return I.reflect(N);
       	}

	vector4 refract(const vector4 &N, T eta) const
	{
	    const T d = N.dot(*this);
	    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
	    return k < static_cast<T>(0) ? vector4<T>(static_cast<T>(0)) : *this * eta - N * (eta * d + glvm::sqrt(k));
	}

	friend vector4 refract(const vector4 &I, const vector4 &N, T eta)
	{
    	    return I.refract(N, eta);
       	}

	/* Comparison functions */

	vector4<bool> greaterThan(const vector4 &v) const
	{
	    return vector4<bool>(x > v.x, y > v.y, z > v.z, w > v.w);
	}

	friend vector4<bool> greaterThan(const vector4 &v, const vector4 &w)
	{
    	    return v.greaterThan(w);
       	}

	vector4<bool> greaterThanEqual(const vector4 &v) const
	{
	    return vector4<bool>(x >= v.x, y >= v.y, z >= v.z, w >= v.w);
	}

	friend vector4<bool> greaterThanEqual(const vector4 &v, const vector4 &w)
	{
    	    return v.greaterThanEqual(w);
       	}

	vector4<bool> lessThan(const vector4 &v) const
	{
	    return vector4<bool>(x < v.x, y < v.y, z < v.z, w < v.w);
	}

	friend vector4<bool> lessThan(const vector4 &v, const vector4 &w)
	{
    	    return v.lessThan(w);
       	}

	vector4<bool> lessThanEqual(const vector4 &v) const
	{
	    return vector4<bool>(x <= v.x, y <= v.y, z <= v.z, w <= v.w);
	}

	friend vector4<bool> lessThanEqual(const vector4 &v, const vector4 &w)
	{
    	    return v.lessThanEqual(w);
       	}

	vector4<bool> equal(const vector4 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return (*this - v).abs().lessThan(vector4(epsilon));
	}

	friend vector4<bool> equal(const vector4 &v, const vector4 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.equal(w, epsilon);
       	}

	vector4<bool> notEqual(const vector4 &v, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	friend vector4<bool> notEqual(const vector4 &v, const vector4 &w, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return v.notEqual(w, epsilon);
       	}

	bool any() const
	{
	    return x || y || z || w;
	}

	bool all() const
	{
	    return x && y && z && w;
	}

	vector4<bool> negate() const
	{
	    return vector4<bool>(!x, !y, !z, !w);
	}
    };

    template<typename T>
    inline std::string vector4<T>::str() const
    {
	std::ostringstream os;
	os << x << " " << y << " " << z << " " << w << " ";
	return os.str();
    }

    template<>
    inline std::string vector4<double>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	os << x << " " << y << " " << z << " " << w << " ";
	return os.str();
    }

    template<>
    inline std::string vector4<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	os << x << " " << y << " " << z << " " << w << " ";
	return os.str();
    }


    template<typename T>
    class matrix2
    {
	public:

	union
	{
	    T v[2][2];
	    T vl[4];
	};

	/* Constructors, Destructor */

	matrix2() {}

	matrix2(const T v00, const T v01, const T v10, const T v11)
	{
	    this->v[0][0] = v00;
	    this->v[0][1] = v01;
	    this->v[1][0] = v10;
	    this->v[1][1] = v11;
	}

	matrix2(const T *vl)
	{
	    memcpy(this->vl, vl, 4 * sizeof(T));
	}

	template<typename U>
	matrix2(const matrix2<U> &m)
	{
	    for (int i = 0; i < 4; i++)
		this->vl[i] = m.vl[i];
	}

	matrix2(const T x)
	{
	    v[0][0] = x;
	    v[0][1] = static_cast<T>(0);
	    v[1][0] = static_cast<T>(0);
	    v[1][1] = x;
	}

	matrix2(const vector2<T> &col0, const vector2<T> &col1)
	{
	    v[0][0] = col0.x;
	    v[1][0] = col0.y;
	    v[0][1] = col1.x;
	    v[1][1] = col1.y;
	}

	matrix2(const std::string &s)
	{
	    std::istringstream is(s);
	    for (int i = 0; i < 4; i++)
		is >> vl[i];
	}

	~matrix2() {}

	/* Get / Set */

	vector2<T> row(const int r) const
	{
	    return vector2<T>(v[r][0], v[r][1]);
	}

	vector2<T> col(const int c) const
	{
	    return vector2<T>(v[0][c], v[1][c]);
	}

	void row(const int r, const vector2<T> &row)
	{
	    v[r][0] = row.x;
	    v[r][1] = row.y;
	}

	void col(const int c, const vector2<T> &col)
	{
	    v[0][c] = col.x;
	    v[1][c] = col.y;
	}

	/* Operators */

	const matrix2 &operator=(const matrix2 &v)
	{
	    memcpy(this->vl, v.vl, 4 * sizeof(T));
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return vl[i];
	}

	const T &operator[](const size_t i) const
	{
	    return vl[i];
	}
	
	matrix2 operator*(const T s) const
	{
	    matrix2 m = *this;
	    for (int i = 0; i < 4; i++)
		m.vl[i] *= s;
	    return m;
	}

	friend matrix2 operator*(const T s, const matrix2 &m)
	{
	    return m * s;
	}
	
	vector2<T> operator*(const vector2<T> &w) const
	{
	    return vector2<T>(
		    v[0][0] * w.x + v[0][1] * w.y, 
		    v[1][0] * w.x + v[1][1] * w.y);
	}

    	friend vector2<T> operator*(const vector2<T> &w, const matrix2 &m)
	{
	    return vector2<T>(
	    	    w.x * m.v[0][0] + w.y * m.v[1][0],
	    	    w.x * m.v[0][1] + w.y * m.v[1][1]);
	}

	matrix2 operator/(const T s) const
	{
	    matrix2 m = *this;
	    for (int i = 0; i < 4; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const matrix2 &operator*=(const T s)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const matrix2 &operator/=(const T s)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] /= s;
	    return *this;
	}

	matrix2 operator+(const matrix2 &m) const
	{
	    matrix2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	matrix2 operator-(const matrix2 &m) const
	{
	    matrix2 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	matrix2 operator*(const matrix2 &n) const 
	{
	    matrix2 r;
	    r.v[0][0] = v[0][0] * n.v[0][0] + v[0][1] * n.v[1][0];
	    r.v[1][0] = v[1][0] * n.v[0][0] + v[1][1] * n.v[1][0];
	    r.v[0][1] = v[0][0] * n.v[0][1] + v[0][1] * n.v[1][1];
	    r.v[1][1] = v[1][0] * n.v[0][1] + v[1][1] * n.v[1][1];
	    return r;
	}

	matrix2 operator-() const
	{
	    matrix2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const matrix2 &operator+=(const matrix2 &m)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const matrix2 &operator-=(const matrix2 &m)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const matrix2 &operator*=(const matrix2 &m)
	{
	    matrix2 r = *this * m;
	    *this = r;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const matrix2 &m)
	{
	    return os << m.str();
	}

	friend std::istream &operator>>(std::istream &is, matrix2 &m)
	{
	    for (int i = 0; i < 4; i++)
		is >> m.vl[i];
	    return is;
	}

	/* Matrix operations */

    	friend matrix2 outerProduct(const vector2<T> &v, const vector2<T> &w)
	{
	    matrix2 m;
	    for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		    m.v[i][j] = v.v[i] * w.v[j];
	    return m;
	}

	matrix2 transpose() const
	{
	    matrix2 r;
	    for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

    	friend matrix2 transpose(const matrix2 &m)
	{
	    return m.transpose();
	}

	matrix2 matrixCompMult(const matrix2 &m) const
	{
	    matrix2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}

    	friend matrix2 matrixCompMult(const matrix2 &m, const matrix2 &n)
	{
	    return m.matrixCompMult(n);
	}

	T det() const
	{
	    return v[0][0] * v[1][1] - v[0][1] * v[1][0];
	}

	friend T det(const matrix2<T> &m)
	{
	    return m.det();
	}

	bool invertible(const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    const T d = det();
	    return (d > epsilon || d < -epsilon);
	}

	friend bool invertible(const matrix2<T> &m, const T epsilon = std::numeric_limits<T>::epsilon())
	{
	    return m.invertible();
	}

	matrix2<T> inverse() const
	{
	    return matrix2<T>(v[1][1], -v[0][1], -v[1][0], v[0][0]) / det();
	}

	friend matrix2<T> inverse(const matrix2<T> &m)
	{
	    return m.inverse();
	}
    };

    template<typename T>
    inline std::string matrix2<T>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	for (int i = 0; i < 4; i++)
	    os << vl[i] << " ";
	return os.str();
    }

    template<>
    inline std::string matrix2<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	for (int i = 0; i < 4; i++)
	    os << vl[i] << " ";
	return os.str();
    }


    template<typename T>
    class matrix3
    {
	public:

	union
	{
	    T v[3][3];
	    T vl[9];
	};

	/* Constructors, Destructor */

	matrix3() {}

	matrix3(const T v00, const T v01, const T v02,
		const T v10, const T v11, const T v12,
		const T v20, const T v21, const T v22)
	{
	    this->v[0][0] = v00;
	    this->v[0][1] = v01;
	    this->v[0][2] = v02;
	    this->v[1][0] = v10;
	    this->v[1][1] = v11;
	    this->v[1][2] = v12;
	    this->v[2][0] = v20;
	    this->v[2][1] = v21;
	    this->v[2][2] = v22;
	}

	matrix3(const T *vl)
	{
	    memcpy(this->vl, vl, 9 * sizeof(T));
	}

	template<typename U>
	matrix3(const matrix3<U> &m)
	{
	    for (int i = 0; i < 9; i++)
		this->vl[i] = m.vl[i];
	}

	matrix3(const T x)
	{
	    v[0][0] = x;
	    v[0][1] = static_cast<T>(0);
	    v[0][2] = static_cast<T>(0);
	    v[1][0] = static_cast<T>(0);
	    v[1][1] = x;
	    v[1][2] = static_cast<T>(0);
	    v[2][0] = static_cast<T>(0);
	    v[2][1] = static_cast<T>(0);
	    v[2][2] = x;
	}

	matrix3(const vector3<T> &col0, const vector3<T> &col1, const vector3<T> &col2)
	{
	    v[0][0] = col0.x;
	    v[1][0] = col0.y;
	    v[2][0] = col0.z;
	    v[0][1] = col1.x;
	    v[1][1] = col1.y;
	    v[2][1] = col1.z;
	    v[0][2] = col2.x;
	    v[1][2] = col2.y;
	    v[2][2] = col2.z;
	}

	matrix3(const vector3<T> &axis, const T angle)
	{
	    const vector3<T> nv = normalize(axis);
	    const T x = nv.x;
	    const T y = nv.y;
	    const T z = nv.z;
	    const T c = glvm::cos(angle);
	    const T s = glvm::sin(angle);
	    const T mc = static_cast<T>(1) - c;
	    v[0][0] = x * x * mc + c;
	    v[0][1] = x * y * mc - z * s;
	    v[0][2] = x * z * mc + y * s;
	    v[1][0] = y * x * mc + z * s;
	    v[1][1] = y * y * mc + c;
	    v[1][2] = y * z * mc - x * s;
	    v[2][0] = x * z * mc - y * s;
	    v[2][1] = y * z * mc + x * s;
	    v[2][2] = z * z * mc + c;
	}

	matrix3(const std::string &s)
	{
	    std::istringstream is(s);
	    for (int i = 0; i < 9; i++)
		is >> vl[i];
	}

	~matrix3() {}

	/* Get / Set */

	vector3<T> row(const int r) const
	{
	    return vector3<T>(v[r][0], v[r][1], v[r][2]);
	}

	vector3<T> col(const int c) const
	{
	    return vector3<T>(v[0][c], v[1][c], v[2][c]);
	}

	void row(const int r, const vector3<T> &row)
	{
	    v[r][0] = row.x;
	    v[r][1] = row.y;
	    v[r][2] = row.z;
	}

	void col(const int c, const vector3<T> &col)
	{
	    v[0][c] = col.x;
	    v[1][c] = col.y;
	    v[2][c] = col.z;
	}

	void set2x2(const matrix2<T> &m, const int row = 0, const int col = 0)
	{
	    v[row][col] = m.v[0][0];
	    v[row][col + 1] = m.v[0][1];
	    v[row + 1][col] = m.v[1][0];
	    v[row + 1][col + 1] = m.v[1][1];
	}

	matrix2<T> strike(const int row, const int col) const
	{
	    matrix2<T> m;
	    int ii = 0;
	    for (int i = 0; i < 3; i++)
	    {
		if (i == row)
		{
		    continue;
		}
		int jj = 0;
		for (int j = 0; j < 3; j++)
		{
		    if (j == col)
		    {
			continue;
		    }
		    m.v[ii][jj] = v[i][j];
		    jj++;
		}
		ii++;
	    }
	    return m;
	}

	friend matrix2<T> strike(const matrix3<T> &m, const int row, const int col)
	{
	    return m.strike(row, col);
	}

	/* Operators */

	const matrix3 &operator=(const matrix3 &v)
	{
	    memcpy(this->vl, v.vl, 9 * sizeof(T));
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return vl[i];
	}

	const T &operator[](const size_t i) const
	{
	    return vl[i];
	}
	
	matrix3 operator*(const T s) const
	{
	    matrix3 m = *this;
	    for (int i = 0; i < 9; i++)
		m.vl[i] *= s;
	    return m;
	}

	friend matrix3 operator*(const T s, const matrix3 &m)
	{
	    return m * s;
	}
	
	vector3<T> operator*(const vector3<T> &w) const
	{
	    return vector3<T>(
		    v[0][0] * w.x + v[0][1] * w.y + v[0][2] * w.z,
		    v[1][0] * w.x + v[1][1] * w.y + v[1][2] * w.z,
		    v[2][0] * w.x + v[2][1] * w.y + v[2][2] * w.z);
	}

    	friend vector3<T> operator*(const vector3<T> &w, const matrix3 &m)
	{
	    return vector3<T>(
	    	    w.x * m.v[0][0] + w.y * m.v[1][0] + w.z * m.v[2][0],
	    	    w.x * m.v[0][1] + w.y * m.v[1][1] + w.z * m.v[2][1],
	    	    w.x * m.v[0][2] + w.y * m.v[1][2] + w.z * m.v[2][2]);
	}

	matrix3 operator/(const T s) const
	{
	    matrix3 m = *this;
	    for (int i = 0; i < 9; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const matrix3 &operator*=(const T s)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const matrix3 &operator/=(const T s)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] /= s;
	    return *this;
	}

	matrix3 operator+(const matrix3 &m) const
	{
	    matrix3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	matrix3 operator-(const matrix3 &m) const
	{
	    matrix3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	matrix3 operator*(const matrix3 &n) const 
	{
	    matrix3 r;
	    r.v[0][0] = v[0][0] * n.v[0][0] + v[0][1] * n.v[1][0] + v[0][2] * n.v[2][0];
	    r.v[1][0] = v[1][0] * n.v[0][0] + v[1][1] * n.v[1][0] + v[1][2] * n.v[2][0];
	    r.v[2][0] = v[2][0] * n.v[0][0] + v[2][1] * n.v[1][0] + v[2][2] * n.v[2][0];
	    r.v[0][1] = v[0][0] * n.v[0][1] + v[0][1] * n.v[1][1] + v[0][2] * n.v[2][1];
	    r.v[1][1] = v[1][0] * n.v[0][1] + v[1][1] * n.v[1][1] + v[1][2] * n.v[2][1];
	    r.v[2][1] = v[2][0] * n.v[0][1] + v[2][1] * n.v[1][1] + v[2][2] * n.v[2][1];
	    r.v[0][2] = v[0][0] * n.v[0][2] + v[0][1] * n.v[1][2] + v[0][2] * n.v[2][2];
	    r.v[1][2] = v[1][0] * n.v[0][2] + v[1][1] * n.v[1][2] + v[1][2] * n.v[2][2];
	    r.v[2][2] = v[2][0] * n.v[0][2] + v[2][1] * n.v[1][2] + v[2][2] * n.v[2][2];
	    return r;
	}

	matrix3 operator-() const
	{
	    matrix3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const matrix3 &operator+=(const matrix3 &m)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const matrix3 &operator-=(const matrix3 &m)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const matrix3 &operator*=(const matrix3 &m)
	{
	    matrix3 r = *this * m;
	    *this = r;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const matrix3 &m)
	{
	    return os << m.str();
	}

	friend std::istream &operator>>(std::istream &is, matrix3 &m)
	{
	    for (int i = 0; i < 9; i++)
		is >> m.vl[i];
	    return is;
	}

	/* Matrix operations */

	friend matrix3 outerProduct(const vector3<T> &v, const vector3<T> &w)
	{
	    matrix3 m;
	    for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		    m.v[i][j] = v.v[i] * w.v[j];
	    return m;
	}

	matrix3 transpose() const
	{
	    matrix3 r;
	    for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

    	friend matrix3 transpose(const matrix3 &m)
	{
	    return m.transpose();
	}

	matrix3 matrixCompMult(const matrix3 &m) const
	{
	    matrix3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}

    	friend matrix3 matrixCompMult(const matrix3 &m, const matrix3 &n)
	{
	    return m.matrixCompMult(n);
	}

	T det() const
	{
	    return v[0][0] * (v[1][1] * v[2][2] - v[1][2] * v[2][1]) 
		+ v[0][1] * (v[1][2] * v[2][0] - v[1][0] * v[2][2]) 
		+ v[0][2] * (v[1][0] * v[2][1] - v[1][1] * v[2][0]);
	}

	friend T det(const matrix3<T> &m)
	{
	    return m.det();
	}

	bool invertible(const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    const T d = det();
	    return (d > epsilon || d < -epsilon);
	}

	friend bool invertible(const matrix3<T> &m, const T epsilon = std::numeric_limits<T>::epsilon())
	{
	    return m.invertible();
	}

	matrix3<T> inverse() const
	{
	    // Using cofactors; see 
	    // http://en.wikipedia.org/wiki/Invertible_matrix
	    // http://en.wikipedia.org/wiki/Minor_(linear_algebra)#Cofactors
	    matrix3<T> I;
	    I.v[0][0] = v[1][1] * v[2][2] - v[1][2] * v[2][1];
	    I.v[0][1] = v[0][2] * v[2][1] - v[0][1] * v[2][2];
	    I.v[0][2] = v[0][1] * v[1][2] - v[0][2] * v[1][1];
	    I.v[1][0] = v[1][2] * v[2][0] - v[1][0] * v[2][2];
	    I.v[1][1] = v[0][0] * v[2][2] - v[0][2] * v[2][0];
	    I.v[1][2] = v[0][2] * v[1][0] - v[0][0] * v[1][2];
	    I.v[2][0] = v[1][0] * v[2][1] - v[1][1] * v[2][0];
	    I.v[2][1] = v[0][1] * v[2][0] - v[0][0] * v[2][1];
	    I.v[2][2] = v[0][0] * v[1][1] - v[0][1] * v[1][0];
	    const T invdet = static_cast<T>(1) / (v[0][0] * I.v[0][0] + v[0][1] * I.v[1][0] + v[0][2] * I.v[2][0]);
	    return I * invdet;
	}

	friend matrix3<T> inverse(const matrix3<T> &m)
	{
	    return m.inverse();
	}
    };

    template<typename T>
    inline std::string matrix3<T>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	for (int i = 0; i < 9; i++)
	    os << vl[i] << " ";
	return os.str();
    }

    template<>
    inline std::string matrix3<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	for (int i = 0; i < 9; i++)
	    os << vl[i] << " ";
	return os.str();
    }


    template<typename T>
    class matrix4
    {
	public:

	union
	{
	    T v[4][4];
	    T vl[16];
	};

	/* Constructors, Destructor */

	matrix4() {}

	matrix4(const T v00, const T v01, const T v02, const T v03,
		const T v10, const T v11, const T v12, const T v13,
		const T v20, const T v21, const T v22, const T v23,
		const T v30, const T v31, const T v32, const T v33)
	{
	    this->v[0][0] = v00;
	    this->v[0][1] = v01;
	    this->v[0][2] = v02;
	    this->v[0][3] = v03;
	    this->v[1][0] = v10;
	    this->v[1][1] = v11;
	    this->v[1][2] = v12;
	    this->v[1][3] = v13;
	    this->v[2][0] = v20;
	    this->v[2][1] = v21;
	    this->v[2][2] = v22;
	    this->v[2][3] = v23;
	    this->v[3][0] = v30;
	    this->v[3][1] = v31;
	    this->v[3][2] = v32;
	    this->v[3][3] = v33;
	}

	matrix4(const T *vl)
	{
	    memcpy(this->vl, vl, 16 * sizeof(T));
	}

	template<typename U>
	matrix4(const matrix4<U> &m)
	{
	    for (int i = 0; i < 16; i++)
		this->vl[i] = m.vl[i];
	}

	matrix4(const T x)
	{
	    v[0][0] = x;
	    v[0][1] = static_cast<T>(0);
	    v[0][2] = static_cast<T>(0);
	    v[0][3] = static_cast<T>(0);
	    v[1][0] = static_cast<T>(0);
	    v[1][1] = x;
	    v[1][2] = static_cast<T>(0);
	    v[1][3] = static_cast<T>(0);
	    v[2][0] = static_cast<T>(0);
	    v[2][1] = static_cast<T>(0);
	    v[2][2] = x;
	    v[2][3] = static_cast<T>(0);
	    v[3][0] = static_cast<T>(0);
	    v[3][1] = static_cast<T>(0);
	    v[3][2] = static_cast<T>(0);
	    v[3][3] = x;
	}

	matrix4(const vector4<T> &col0, const vector4<T> &col1, const vector4<T> &col2, const vector4<T> &col3)
	{
	    v[0][0] = col0.x;
	    v[1][0] = col0.y;
	    v[2][0] = col0.z;
	    v[3][0] = col0.w;
	    v[0][1] = col1.x;
	    v[1][1] = col1.y;
	    v[2][1] = col1.z;
	    v[3][1] = col1.w;
	    v[0][2] = col2.x;
	    v[1][2] = col2.y;
	    v[2][2] = col2.z;
	    v[3][2] = col2.w;
	    v[0][3] = col3.x;
	    v[1][3] = col3.y;
	    v[2][3] = col3.z;
	    v[3][3] = col3.w;
	}

	matrix4(const std::string &s)
	{
	    std::istringstream is(s);
	    for (int i = 0; i < 16; i++)
		is >> vl[i];
	}

	matrix4(const vector3<T> &axis, const T angle)
	{
	    const matrix3<T> m(angle, axis);
	    v[0][0] = m.v[0][0];
	    v[0][1] = m.v[0][1];
	    v[0][2] = m.v[0][2];
	    v[0][3] = static_cast<T>(0);
	    v[1][0] = m.v[1][0];
	    v[1][1] = m.v[1][1];
	    v[1][2] = m.v[1][2];
	    v[1][3] = static_cast<T>(0);
	    v[2][0] = m.v[2][0];
	    v[2][1] = m.v[2][1];
	    v[2][2] = m.v[2][2];
	    v[2][3] = static_cast<T>(0);
	    v[3][0] = static_cast<T>(0);
	    v[3][1] = static_cast<T>(0);
	    v[3][2] = static_cast<T>(0);
	    v[3][3] = static_cast<T>(1);
	}

	~matrix4() {}

	/* Get / Set */

	vector4<T> row(const int r) const
	{
	    return vector4<T>(v[r][0], v[r][1], v[r][2], v[r][3]);
	}

	vector4<T> col(const int c) const
	{
	    return vector4<T>(v[0][c], v[1][c], v[2][c], v[3][c]);
	}

	void row(const int r, const vector4<T> &row)
	{
	    v[r][0] = row.x;
	    v[r][1] = row.y;
	    v[r][2] = row.z;
	    v[r][3] = row.w;
	}

	void col(const int c, const vector4<T> &col)
	{
	    v[0][c] = col.x;
	    v[1][c] = col.y;
	    v[2][c] = col.z;
	    v[3][c] = col.w;
	}

	void set2x2(const matrix2<T> &m, const int row = 0, const int col = 0)
	{
	    v[row][col] = m.v[0][0];
	    v[row][col + 1] = m.v[0][1];
	    v[row + 1][col] = m.v[1][0];
	    v[row + 1][col + 1] = m.v[1][1];
	}

	void set3x3(const matrix3<T> &m, const int row = 0, const int col = 0)
	{
	    v[row][col] = m.v[0][0];
	    v[row][col + 1] = m.v[0][1];
	    v[row][col + 2] = m.v[0][2];
	    v[row + 1][col] = m.v[1][0];
	    v[row + 1][col + 1] = m.v[1][1];
	    v[row + 1][col + 2] = m.v[1][2];
	    v[row + 2][col] = m.v[2][0];
	    v[row + 2][col + 1] = m.v[2][1];
	    v[row + 2][col + 2] = m.v[2][2];
	}

	matrix3<T> strike(const int row, const int col) const
	{
	    matrix3<T> m;
	    int ii = 0;
	    for (int i = 0; i < 4; i++)
	    {
		if (i == row)
		{
		    continue;
		}
		int jj = 0;
		for (int j = 0; j < 4; j++)
		{
		    if (j == col)
		    {
			continue;
		    }
		    m.v[ii][jj] = v[i][j];
		    jj++;
		}
		ii++;
	    }
	    return m;
	}

	friend matrix3<T> strike(const matrix4<T> &m, const int row, const int col)
	{
	    return m.strike(row, col);
	}

	/* Operators */

	const matrix4 &operator=(const matrix4 &v)
	{
	    memcpy(this->vl, v.vl, 16 * sizeof(T));
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return vl[i];
	}

	const T &operator[](const size_t i) const
	{
	    return vl[i];
	}
	
	matrix4 operator*(const T s) const
	{
	    matrix4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.vl[i] *= s;
	    return m;
	}

	friend matrix4 operator*(const T s, const matrix4 &m)
	{
	    return m * s;
	}

	vector4<T> operator*(const vector4<T> &w) const
	{
	    return vector4<T>(
		    v[0][0] * w.x + v[0][1] * w.y + v[0][2] * w.z + v[0][3] * w.w,
		    v[1][0] * w.x + v[1][1] * w.y + v[1][2] * w.z + v[1][3] * w.w,
		    v[2][0] * w.x + v[2][1] * w.y + v[2][2] * w.z + v[2][3] * w.w,
		    v[3][0] * w.x + v[3][1] * w.y + v[3][2] * w.z + v[3][3] * w.w);
	}

    	friend vector4<T> operator*(const vector4<T> &w, const matrix4 &m)
	{
	    return vector4<T>(
	    	    w.x * m.v[0][0] + w.y * m.v[1][0] + w.z * m.v[2][0] + w.w * m.v[3][0],
	    	    w.x * m.v[0][1] + w.y * m.v[1][1] + w.z * m.v[2][1] + w.w * m.v[3][1],
	    	    w.x * m.v[0][2] + w.y * m.v[1][2] + w.z * m.v[2][2] + w.w * m.v[3][2],
	    	    w.x * m.v[0][3] + w.y * m.v[1][3] + w.z * m.v[2][3] + w.w * m.v[3][3]);
	}

	matrix4 operator/(const T s) const
	{
	    matrix4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const matrix4 &operator*=(const T s)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const matrix4 &operator/=(const T s)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] /= s;
	    return *this;
	}

	matrix4 operator+(const matrix4 &m) const
	{
	    matrix4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	matrix4 operator-(const matrix4 &m) const
	{
	    matrix4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	matrix4 operator*(const matrix4 &n) const 
	{
	    matrix4 r;
	    r.v[0][0] = v[0][0] * n.v[0][0] + v[0][1] * n.v[1][0] + v[0][2] * n.v[2][0] + v[0][3] * n.v[3][0];
	    r.v[1][0] = v[1][0] * n.v[0][0] + v[1][1] * n.v[1][0] + v[1][2] * n.v[2][0] + v[1][3] * n.v[3][0];
	    r.v[2][0] = v[2][0] * n.v[0][0] + v[2][1] * n.v[1][0] + v[2][2] * n.v[2][0] + v[2][3] * n.v[3][0];
	    r.v[3][0] = v[3][0] * n.v[0][0] + v[3][1] * n.v[1][0] + v[3][2] * n.v[2][0] + v[3][3] * n.v[3][0];
	    r.v[0][1] = v[0][0] * n.v[0][1] + v[0][1] * n.v[1][1] + v[0][2] * n.v[2][1] + v[0][3] * n.v[3][1];
	    r.v[1][1] = v[1][0] * n.v[0][1] + v[1][1] * n.v[1][1] + v[1][2] * n.v[2][1] + v[1][3] * n.v[3][1];
	    r.v[2][1] = v[2][0] * n.v[0][1] + v[2][1] * n.v[1][1] + v[2][2] * n.v[2][1] + v[2][3] * n.v[3][1];
	    r.v[3][1] = v[3][0] * n.v[0][1] + v[3][1] * n.v[1][1] + v[3][2] * n.v[2][1] + v[3][3] * n.v[3][1];
	    r.v[0][2] = v[0][0] * n.v[0][2] + v[0][1] * n.v[1][2] + v[0][2] * n.v[2][2] + v[0][3] * n.v[3][2];
	    r.v[1][2] = v[1][0] * n.v[0][2] + v[1][1] * n.v[1][2] + v[1][2] * n.v[2][2] + v[1][3] * n.v[3][2];
	    r.v[2][2] = v[2][0] * n.v[0][2] + v[2][1] * n.v[1][2] + v[2][2] * n.v[2][2] + v[2][3] * n.v[3][2];
	    r.v[3][2] = v[3][0] * n.v[0][2] + v[3][1] * n.v[1][2] + v[3][2] * n.v[2][2] + v[3][3] * n.v[3][2];
	    r.v[0][3] = v[0][0] * n.v[0][3] + v[0][1] * n.v[1][3] + v[0][2] * n.v[2][3] + v[0][3] * n.v[3][3];
	    r.v[1][3] = v[1][0] * n.v[0][3] + v[1][1] * n.v[1][3] + v[1][2] * n.v[2][3] + v[1][3] * n.v[3][3];
	    r.v[2][3] = v[2][0] * n.v[0][3] + v[2][1] * n.v[1][3] + v[2][2] * n.v[2][3] + v[2][3] * n.v[3][3];
	    r.v[3][3] = v[3][0] * n.v[0][3] + v[3][1] * n.v[1][3] + v[3][2] * n.v[2][3] + v[3][3] * n.v[3][3];
	    return r;
	}

	matrix4 operator-() const
	{
	    matrix4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const matrix4 &operator+=(const matrix4 &m)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const matrix4 &operator-=(const matrix4 &m)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const matrix4 &operator*=(const matrix4 &m)
	{
	    matrix4 r = *this * m;
	    *this = r;
	    return *this;
	}

	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const matrix4 &m)
	{
	    return os << m.str();
	}

	friend std::istream &operator>>(std::istream &is, matrix4 &m)
	{
	    for (int i = 0; i < 16; i++)
		is >> m.vl[i];
	    return is;
	}

	/* Matrix operations */

    	friend matrix4 outerProduct(const vector4<T> &v, const vector4<T> &w)
	{
	    matrix4 m;
	    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		    m.v[i][j] = v.v[i] * w.v[j];
	    return m;
	}

	matrix4 transpose() const
	{
	    matrix4 r;
	    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

	friend matrix4 transpose(const matrix4 &m)
	{
	    return m.transpose();
	}

	matrix4 matrixCompMult(const matrix4 &m) const
	{
	    matrix4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}

    	friend matrix4 matrixCompMult(const matrix4 &m, const matrix4 &n)
	{
	    return m.matrixCompMult(n);
	}

	T det() const
	{
	    /*
	    return v[0][0] * strike(0, 0).det() 
		- v[0][1] * strike(0, 1).det() 
		+ v[0][2] * strike(0, 2).det() 
		- v[0][3] * strike(0, 3).det();
            */
	    // The following can be better optimized by the compiler
	    // (checked with gcc 4.1):
	    const T d0 = v[1][1] * (v[2][2] * v[3][3] - v[2][3] * v[3][2]) 
		+ v[1][2] * (v[2][3] * v[3][1] - v[2][1] * v[3][3]) 
		+ v[1][3] * (v[2][1] * v[3][2] - v[2][2] * v[3][1]);
	    const T d1 = v[1][0] * (v[2][2] * v[3][3] - v[2][3] * v[3][2]) 
		+ v[1][2] * (v[2][3] * v[3][0] - v[2][0] * v[3][3]) 
		+ v[1][3] * (v[2][0] * v[3][2] - v[2][2] * v[3][0]);
	    const T d2 = v[1][0] * (v[2][1] * v[3][3] - v[2][3] * v[3][1]) 
		+ v[1][1] * (v[2][3] * v[3][0] - v[2][0] * v[3][3]) 
		+ v[1][3] * (v[2][0] * v[3][1] - v[2][1] * v[3][0]);
	    const T d3 = v[1][0] * (v[2][1] * v[3][2] - v[2][2] * v[3][1]) 
		+ v[1][1] * (v[2][2] * v[3][0] - v[2][0] * v[3][2]) 
		+ v[1][2] * (v[2][0] * v[3][1] - v[2][1] * v[3][0]);
	    return v[0][0] * d0 - v[0][1] * d1 + v[0][2] * d2 - v[0][3] * d3;
	}

	friend T det(const matrix4<T> &m)
	{
	    return m.det();
	}

	bool invertible(const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    const T d = det();
	    return (d > epsilon || d < -epsilon);
	}

	friend bool invertible(const matrix4<T> &m, const T epsilon = std::numeric_limits<T>::epsilon())
	{
	    return m.invertible();
	}

	matrix4<T> inverse() const
	{
	    // Using cofactors; see 
	    // http://en.wikipedia.org/wiki/Invertible_matrix
	    // http://en.wikipedia.org/wiki/Minor_(linear_algebra)#Cofactors

	    /* This code was adapted from Equalizer-0.5.0,
	     * src/externals/vmmlib/matrix4.h:
	     *
	     * VMMLib - Vector & Matrix Math Lib
	     *  
	     * @author Jonas Boesch
	     * @author Stefan Eilemann
	     * @author Renato Pajarola
	     * @author David H. Eberly ( Wild Magic )
	     * @author Andrew Willmott ( VL )
	     *
	     * @license revised BSD license, check LICENSE
	     *
	     * parts of the source code of VMMLib were inspired by David Eberly's 
	     * Wild Magic and Andrew Willmott's VL.
	     *
	     * tuned version from Claude Knaus
	     */

	    matrix4<T> result;

	    /* first set of 2x2 determinants: 12 multiplications, 6 additions */
	    const T t1[6] = 
	    { 
		vl[ 2] * vl[ 7] - vl[ 6] * vl[ 3],
		vl[ 2] * vl[11] - vl[10] * vl[ 3],
		vl[ 2] * vl[15] - vl[14] * vl[ 3],
	    	vl[ 6] * vl[11] - vl[10] * vl[ 7],
      		vl[ 6] * vl[15] - vl[14] * vl[ 7],
		vl[10] * vl[15] - vl[14] * vl[11] 
	    };

	    /* first half of comatrix: 24 multiplications, 16 additions */
	    result.vl[0] = vl[ 5] * t1[5] - vl[ 9] * t1[4] + vl[13] * t1[3];
	    result.vl[1] = vl[ 9] * t1[2] - vl[13] * t1[1] - vl[ 1] * t1[5];
	    result.vl[2] = vl[13] * t1[0] - vl[ 5] * t1[2] + vl[ 1] * t1[4];
	    result.vl[3] = vl[ 5] * t1[1] - vl[ 1] * t1[3] - vl[ 9] * t1[0];
	    result.vl[4] = vl[ 8] * t1[4] - vl[ 4] * t1[5] - vl[12] * t1[3];
	    result.vl[5] = vl[ 0] * t1[5] - vl[ 8] * t1[2] + vl[12] * t1[1];
	    result.vl[6] = vl[ 4] * t1[2] - vl[12] * t1[0] - vl[ 0] * t1[4];
	    result.vl[7] = vl[ 0] * t1[3] - vl[ 4] * t1[1] + vl[ 8] * t1[0];

	    /* second set of 2x2 determinants: 12 multiplications, 6 additions */
	    const T t2[6] = 
	    {
		vl[ 0] * vl[ 5] - vl[ 4] * vl[ 1],
		vl[ 0] * vl[ 9] - vl[ 8] * vl[ 1],
		vl[ 0] * vl[13] - vl[12] * vl[ 1],
	   	vl[ 4] * vl[ 9] - vl[ 8] * vl[ 5],
     		vl[ 4] * vl[13] - vl[12] * vl[ 5],
		vl[ 8] * vl[13] - vl[12] * vl[ 9] 
	    };

	    /* second half of comatrix: 24 multiplications, 16 additions */
	    result.vl[ 8] = vl[ 7] * t2[5] - vl[11] * t2[4] + vl[15] * t2[3];
	    result.vl[ 9] = vl[11] * t2[2] - vl[15] * t2[1] - vl[ 3] * t2[5];
	    result.vl[10] = vl[15] * t2[0] - vl[ 7] * t2[2] + vl[ 3] * t2[4];
	    result.vl[11] = vl[ 7] * t2[1] - vl[ 3] * t2[3] - vl[11] * t2[0];
	    result.vl[12] = vl[10] * t2[4] - vl[ 6] * t2[5] - vl[14] * t2[3];
	    result.vl[13] = vl[ 2] * t2[5] - vl[10] * t2[2] + vl[14] * t2[1];
	    result.vl[14] = vl[ 6] * t2[2] - vl[14] * t2[0] - vl[ 2] * t2[4];
	    result.vl[15] = vl[ 2] * t2[3] - vl[ 6] * t2[1] + vl[10] * t2[0];

	    /* determinant: 4 multiplications, 3 additions */
	    const T determinant =
		vl[0] * result.vl[0] + vl[4] * result.vl[1] +
		vl[8] * result.vl[2] + vl[12] * result.vl[3];

	    /* division: 16 multiplications, 1 division */
	    const T detinv = static_cast<T>(1) / determinant;

	    return result * detinv;
	}

	friend matrix4<T> inverse(const matrix4<T> &m)
	{
	    return m.inverse();
	}

	vector3<T> getTranslation(void) const
	{
	    return vector3<T>(v[0][3], v[1][3], v[2][3]);
	}

	void setTranslation(const vector3<T> &t)
	{
	    v[0][3] = t.x;
	    v[1][3] = t.y;
	    v[2][3] = t.z;
	}

	const matrix4 &translate(const vector3<T> &v)
	{
	    vector4<T> t(v, static_cast<T>(1));
	    v[0][3] = dot(row(0), t);
	    v[1][3] = dot(row(1), t);
	    v[2][3] = dot(row(2), t);
	    v[3][3] = dot(row(3), t);
	    return *this;
	}

	friend matrix4 translate(const matrix4 &m, const vector3<T> &v)
	{
	    matrix4 r = m.translate(v);
	    return r;
	}

	const matrix4 &scale(const vector3<T> &v)
	{
	    for (int row = 0; row < 4; row++)
	    {
		v[row][0] *= v.x;
		v[row][1] *= v.y;
		v[row][2] *= v.z;
	    }
	    return *this;
	}

	friend matrix4 scale(const matrix4<T> &m, const vector3<T> &v)
	{
	    matrix4 r = m.scale(v);
	    return r;
	}

	const matrix4 &rotate(const T angle, const vector3<T> &v)
	{
	    *this *= mat4(angle, v);
	    return *this;
	}

	friend matrix4 rotate(const matrix4<T> &m, T angle, const vector3<T> &v)
	{
	    matrix4 r = m.rotate(angle, v);
	    return r;
	}
    };

    template<typename T>
    inline std::string matrix4<T>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	for (int i = 0; i < 16; i++)
	    os << vl[i] << " ";
	return os.str();
    }

    template<>
    inline std::string matrix4<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	for (int i = 0; i < 16; i++)
	    os << vl[i] << " ";
	return os.str();
    }


    typedef vector2<bool> bvec2;
    typedef vector2<int> ivec2;
    typedef vector2<float> vec2;
    typedef vector2<double> dvec2;
    typedef vector3<bool> bvec3;
    typedef vector3<int> ivec3;
    typedef vector3<float> vec3;
    typedef vector3<double> dvec3;
    typedef vector4<bool> bvec4;
    typedef vector4<int> ivec4;
    typedef vector4<float> vec4;
    typedef vector4<double> dvec4;
    typedef matrix2<float> mat2;
    typedef matrix2<double> dmat2;
    typedef matrix3<float> mat3;
    typedef matrix3<double> dmat3;
    typedef matrix4<float> mat4;
    typedef matrix4<double> dmat4;


    template<typename T>
    class quaternion
    {
	public:

	union
	{
	    struct
	    {
		T w, x, y, z;
	    };
	    T v[4];
	};

	/* Constructors, Destructor */

	quaternion() {}

	quaternion(const T w, const T x, const T y, const T z)
	{
	    this->w = w;
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	quaternion(const T *v)
	{
	    memcpy(this->v, v, 4 * sizeof(T));
	}

	template<typename U>
	quaternion(const quaternion<U> &m)
	{
	    for (int i = 0; i < 4; i++)
		this->v[i] = m.v[i];
	}

	quaternion(const T w)
	{
	    this->w = w;
	    this->x = static_cast<T>(0);
	    this->y = static_cast<T>(0);
	    this->z = static_cast<T>(0);
	}

	quaternion(const vector4<T> &wxyz)
	{
	    memcpy(this->v, wxyz.v, 4 * sizeof(T));
	}

	quaternion(const T w, const vector3<T> &xyz)
	{
	    this->w = w;
	    memcpy(this->v + 1, xyz.v, 3 * sizeof(T));
	}

	quaternion(const std::string &s)
	{
	    std::istringstream is(s);
	    for (int i = 0; i < 4; i++)
		is >> v[i];
	}

	~quaternion() {}

	/* Operators */

	const quaternion &operator=(const quaternion &v)
	{
	    memcpy(this->v, v.v, 4 * sizeof(T));
	    return *this;
	}

	T &operator[](const size_t i)
	{
	    return v[i];
	}

	const T &operator[](const size_t i) const
	{
	    return v[i];
	}
	
	quaternion operator+(const quaternion &q) const
	{
	    return quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
	}

	quaternion operator-(const quaternion &q) const
	{
	    return quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
	}

	quaternion operator-() const
	{
	    return quaternion(-w, -x, -y, -z);
	}

	quaternion operator*(const T s) const
	{
	    quaternion q = *this;
	    for (int i = 0; i < 4; i++)
		q.v[i] *= s;
	    return q;
	}

	friend quaternion operator*(const T s, const quaternion &q)
	{
	    return q * s;
	}
	
	quaternion operator/(const T s) const
	{
	    quaternion q = *this;
	    for (int i = 0; i < 4; i++)
		q.v[i] /= s;
	    return q;
	}

	friend quaternion operator/(const T s, const quaternion &q)
	{
	    return q / s;
	}

	const quaternion &operator+=(const quaternion &q)
	{
	    w += q.w;
	    x += q.x;
	    y += q.y;
	    z += q.z;
	    return *this;
	}

	const quaternion &operator-=(const quaternion &q)
	{
	    w -= q.w;
	    x -= q.x;
	    y -= q.y;
	    z -= q.z;
	    return *this;
	}

	const quaternion &operator*=(const T s)
	{
	    for (int i = 0; i < 4; i++)
		v[i] *= s;
	    return *this;
	}

	const quaternion &operator/=(const T s)
	{
	    for (int i = 0; i < 4; i++)
		v[i] /= s;
	    return *this;
	}
	
	std::string str() const;

	friend std::ostream &operator<<(std::ostream &os, const quaternion &q)
	{
	    return os << q.str();
	}

	friend std::istream &operator>>(std::istream &is, quaternion &q)
	{
	    for (int i = 0; i < 4; i++)
		is >> q.v[i];
	    return is;
	}

	/* Quaternion operations */

	T magnitude() const
	{
	    return glvm::sqrt(w * w + x * x + y * y + z * z);
	}

	friend T magnitude(const quaternion &q)
	{
	    return q.magnitude();
	}

	quaternion normalize() const
	{
  	    return *this / this->magnitude();
	}

 	friend quaternion normalize(const quaternion &v)
 	{
      	    return v.normalize();
	}

	quaternion conjugate() const
	{
	    return quaternion(w, -x, -y, -z);
	}

    	friend quaternion conjugate(const quaternion &q)
	{
	    return q.conjugate();
	}

	quaternion inverse() const
	{
	    return this->conjugate() / this->magnitude();
	}
	
	quaternion operator*(const quaternion &q) const
	{
	    quaternion p;
	    p.w = this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z;
	    p.x = this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y;
	    p.y = this->w * q.y + this->y * q.w + this->z * q.x - this->x * q.z;
	    p.z = this->w * q.z + this->z * q.w + this->x * q.y - this->y * q.x;
	    return p;
	}

	const quaternion &operator*=(const quaternion &q)
	{
	    *this = *this * q;
	    return *this;
	}

	/* Conversions */

	matrix3<T> to_matrix3() const
	{
	    matrix3<T> M;
	    const T xx = v[1] * v[1];
	    const T xy = v[1] * v[2];
	    const T xz = v[1] * v[3];
	    const T xw = v[1] * v[0];
	    const T yy = v[2] * v[2];
	    const T yz = v[2] * v[3];
	    const T yw = v[2] * v[0];
	    const T zz = v[3] * v[3];
	    const T zw = v[3] * v[0];
	    M.vl[0] = static_cast<T>(1) - static_cast<T>(2) * (yy + zz);
	    M.vl[1] = static_cast<T>(2) * (xy - zw);
	    M.vl[2] = static_cast<T>(2) * (xz + yw);
	    M.vl[3] = static_cast<T>(2) * (xy + zw);
	    M.vl[4] = static_cast<T>(1) - static_cast<T>(2) * (xx + zz);
	    M.vl[5] = static_cast<T>(2) * (yz - xw);
	    M.vl[6] = static_cast<T>(2) * (xz - yw);
	    M.vl[7] = static_cast<T>(2) * (yz + xw);
	    M.vl[8] = static_cast<T>(1) - static_cast<T>(2) * (xx + yy);
	    return M;
	}

	matrix4<T> to_matrix4() const
	{
	    matrix4<T> M;
	    const T xx = v[1] * v[1];
	    const T xy = v[1] * v[2];
	    const T xz = v[1] * v[3];
	    const T xw = v[1] * v[0];
	    const T yy = v[2] * v[2];
	    const T yz = v[2] * v[3];
	    const T yw = v[2] * v[0];
	    const T zz = v[3] * v[3];
	    const T zw = v[3] * v[0];
	    M.vl[0]  = static_cast<T>(1) - static_cast<T>(2) * (yy + zz);
	    M.vl[1]  = static_cast<T>(2) * (xy - zw);
	    M.vl[2]  = static_cast<T>(2) * (xz + yw);
	    M.vl[3]  = static_cast<T>(0);
	    M.vl[4]  = static_cast<T>(2) * (xy + zw);
	    M.vl[5]  = static_cast<T>(1) - static_cast<T>(2) * (xx + zz);
	    M.vl[6]  = static_cast<T>(2) * (yz - xw);
	    M.vl[7]  = static_cast<T>(0);
	    M.vl[8]  = static_cast<T>(2) * (xz - yw);
	    M.vl[9]  = static_cast<T>(2) * (yz + xw);
	    M.vl[10] = static_cast<T>(1) - static_cast<T>(2) * (xx + yy);
	    M.vl[11] = static_cast<T>(0);
	    M.vl[12] = static_cast<T>(0);
	    M.vl[13] = static_cast<T>(0);
	    M.vl[14] = static_cast<T>(0);
	    M.vl[15] = static_cast<T>(1);
	    return M;
	}

	void to_axis_angle(vector3<T> &axis, T *angle) const
	{
	    quaternion<T> nq = this->normalize();
	    T cos_a = nq.w;
	    *angle = glvm::acos(cos_a) * static_cast<T>(2);
	    T sin_a = glvm::sqrt(static_cast<T>(1) - cos_a * cos_a);
	    if (glvm::abs(sin_a) < static_cast<T>(0.0005))
	    {
		sin_a = static_cast<T>(1);
	    }
	    axis.x = nq.x / sin_a;
	    axis.y = nq.y / sin_a;
	    axis.z = nq.z / sin_a;
	}

	void from_axis_angle(const vector3<T> &axis, const T angle)
	{
	    vector3<T> naxis = axis.normalize();
	    T sin_a = glvm::sin(angle / static_cast<T>(2));
	    T cos_a = glvm::cos(angle / static_cast<T>(2));
	    w = cos_a;
	    x = naxis.x * sin_a;
	    y = naxis.y * sin_a;
	    z = naxis.z * sin_a;
	    normalize();
	}

	void to_euler_angles(T *rx, T *ry, T *rz) const
	{
	    *rx = glvm::atan(static_cast<T>(2) * (w * x + y * z) / (static_cast<T>(1) - static_cast<T>(2) * (x * x + y * y)));
	    *ry = glvm::asin(static_cast<T>(2) * (w * y - x * z));
	    *rz = glvm::atan(static_cast<T>(2) * (w * z + x * y) / (static_cast<T>(1) - static_cast<T>(2) * (y * y + z * z)));
	}
	
	void from_euler_angles(const T rx, const T ry, const T rz)
	{
	    // quaternion<T> qx, qy, qz;
	    // qx.from_axis_angle(vector3<T>(1, 0, 0), rx)
	    // qy.from_axis_angle(vector3<T>(1, 0, 0), ry)
	    // qz.from_axis_angle(vector3<T>(1, 0, 0), rz)
	    quaternion<T> qx(glvm::cos(rx / static_cast<T>(2)), glvm::sin(rx / static_cast<T>(2)), static_cast<T>(0), static_cast<T>(0));
	    quaternion<T> qy(glvm::cos(ry / static_cast<T>(2)), static_cast<T>(0), glvm::sin(ry / static_cast<T>(2)), static_cast<T>(0));
	    quaternion<T> qz(glvm::cos(rz / static_cast<T>(2)), static_cast<T>(0), static_cast<T>(0), glvm::sin(rz / static_cast<T>(2)));
	    *this = qx * qy * qz;
	    // TODO: Does this need to be optimized by cancelling out the
	    // multiplications with zero? Or is the compiler smart enough to do this
	    // for us?
	}
    };

    template<typename T>
    inline std::string quaternion<T>::str() const
    {
	std::ostringstream os;
	os.precision(DBL_DIG);
	for (int i = 0; i < 4; i++)
	    os << v[i] << " ";
	return os.str();
    }

    template<>
    inline std::string quaternion<float>::str() const
    {
	std::ostringstream os;
	os.precision(FLT_DIG);
	for (int i = 0; i < 4; i++)
	    os << v[i] << " ";
	return os.str();
    }


    typedef quaternion<float> quat;
    typedef quaternion<double> dquat;
}

#endif
