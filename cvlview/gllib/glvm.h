/*
 * glvm.h
 * 
 * C++ vector and matrix classes that resemble GLSL style.
 *
 * Copyright (C) 2008, 2009  Martin Lambers <marlam@marlam.de>
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
 * vec4, mat2, mat3, mat4, mat2x3, mat3x2, mat2x4, mat4x2, mat3x4, mat4x3 (and 
 * the variants bvec, ivec, dvec, dmat).
 * Additionally, there's a quaternion class (quat and dquat) and a frustum class
 * (frust and dfrust).
 *
 * Vector elements are called (x,y,z,w) and (r,g,b,a) and (s,t,p,q). Swizzling
 * is fully supported, including assignments! Just remember that the swizzlers
 * are member functions (use parentheses).
 * Example: "v4.wz() = v3.rg() + v2;"
 *
 * All data elements are accessible via a two-dimensional array v or a linear
 * array vl. Both v and vl are row-major, unlike OpenGL, but like everyone else
 * (but note that a mat2x3 matrix still has two columns and 3 rows and not
 * vice-versa). Use transpose() when reading OpenGL data (or use the functions 
 * from glvm-gl.h).
 *
 * Everything that is specified by GLSL 1.30 should work, unless it is
 * impossible to implement in C++.
 */


#ifndef GLVM_H
#define GLVM_H

/* Define to 1 if you have C++0x math functions in the std namespace */
#define GLVM_HAVE_CXX0X_MATH 0

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <stdint.h>

#include <limits>
#include <string>
#include <iostream>
#include <sstream>

#ifndef M_PIl
# define M_PIl		3.1415926535897932384626433832795029L  /* pi */
#endif


namespace glvm
{
    /* Define the GLSL functions for the subset of the base data types 
     * for which the function makes sense. The base data types are:
     *
     * bool
     * signed char, unsigned char
     * short, unsigned short
     * int, unsigned int
     * long, unsigned long
     * long long, unsigned long long
     * float, double, long double
     *
     * We cannot use templates here because then the compiler would try to apply
     * these functions to vec and mat. We use templates to define a different
     * behaviour for vec and mat below.
     */

    inline signed char min(const signed char x, const signed char y)
    {
	return (x < y ? x : y);
    }

    inline unsigned char min(const unsigned char x, const unsigned char y)
    {
	return (x < y ? x : y);
    }

    inline short min(const short x, const short y)
    {
	return (x < y ? x : y);
    }

    inline unsigned short min(const unsigned short x, const unsigned short y)
    {
	return (x < y ? x : y);
    }

    inline int min(const int x, const int y)
    {
	return (x < y ? x : y);
    }

    inline unsigned int min(const unsigned int x, const unsigned int y)
    {
	return (x < y ? x : y);
    }

    inline long min(const long x, const long y)
    {
	return (x < y ? x : y);
    }

    inline unsigned long min(const unsigned long x, const unsigned long y)
    {
	return (x < y ? x : y);
    }

    inline long long min(const long long x, const long long y)
    {
	return (x < y ? x : y);
    }

    inline unsigned long long min(const unsigned long long x, const unsigned long long y)
    {
	return (x < y ? x : y);
    }

    inline float min(const float x, const float y)
    {
	return (x < y ? x : y);
    }

    inline double min(const double x, const double y)
    {
	return (x < y ? x : y);
    }

    inline long double min(const long double x, const long double y)
    {
	return (x < y ? x : y);
    }

    inline signed char max(const signed char x, const signed char y)
    {
	return (x > y ? x : y);
    }

    inline unsigned char max(const unsigned char x, const unsigned char y)
    {
	return (x > y ? x : y);
    }

    inline short max(const short x, const short y)
    {
	return (x > y ? x : y);
    }

    inline unsigned short max(const unsigned short x, const unsigned short y)
    {
	return (x > y ? x : y);
    }

    inline int max(const int x, const int y)
    {
	return (x > y ? x : y);
    }

    inline unsigned int max(const unsigned int x, const unsigned int y)
    {
	return (x > y ? x : y);
    }

    inline long max(const long x, const long y)
    {
	return (x > y ? x : y);
    }

    inline unsigned long max(const unsigned long x, const unsigned long y)
    {
	return (x > y ? x : y);
    }

    inline long long max(const long long x, const long long y)
    {
	return (x > y ? x : y);
    }

    inline unsigned long long max(const unsigned long long x, const unsigned long long y)
    {
	return (x > y ? x : y);
    }

    inline float max(const float x, const float y)
    {
	return (x > y ? x : y);
    }

    inline double max(const double x, const double y)
    {
	return (x > y ? x : y);
    }

    inline long double max(const long double x, const long double y)
    {
	return (x > y ? x : y);
    }

    inline signed char clamp(const signed char x, const signed char minval, const signed char maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline unsigned char clamp(const unsigned char x, const unsigned char minval, const unsigned char maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline short clamp(const short x, const short minval, const short maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline unsigned short clamp(const unsigned short x, const unsigned short minval, const unsigned short maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline int clamp(const int x, const int minval, const int maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline unsigned int clamp(const unsigned int x, const unsigned int minval, const unsigned int maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline long clamp(const long x, const long minval, const long maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline unsigned long clamp(const unsigned long x, const unsigned long minval, const unsigned long maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline long long clamp(const long long x, const long long minval, const long long maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline unsigned long long clamp(const unsigned long long x, const unsigned long long minval, const unsigned long long maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline float clamp(const float x, const float minval, const float maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline double clamp(const double x, const double minval, const double maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline long double clamp(const long double x, const long double minval, const long double maxval)
    {
	return glvm::min(maxval, glvm::max(minval, x));
    }

    inline signed char step(const signed char x, const signed char edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline unsigned char step(const unsigned char x, const unsigned char edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline short step(const short x, const short edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline unsigned short step(const unsigned short x, const unsigned short edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline int step(const int x, const int edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline unsigned int step(const unsigned int x, const unsigned int edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline long step(const long x, const long edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline unsigned long step(const unsigned long x, const unsigned long edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline long long step(const long long x, const long long edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline unsigned long long step(const unsigned long long x, const unsigned long long edge)
    {
	return (x < edge ? 0 : 1);
    }

    inline float step(const float x, const float edge)
    {
	return (x < edge ? 0.0f : 1.0f);
    }

    inline double step(const double x, const double edge)
    {
	return (x < edge ? 0.0 : 1.0);
    }

    inline long double step(const long double x, const long double edge)
    {
	return (x < edge ? 0.0L : 1.0L);
    }

    inline signed char mod(const signed char x, const signed char y)
    {
	return x - (x / y) * y;
    }

    inline unsigned char mod(const unsigned char x, const unsigned char y)
    {
	return x - (x / y) * y;
    }

    inline short mod(const short x, const short y)
    {
	return x - (x / y) * y;
    }

    inline unsigned short mod(const unsigned short x, const unsigned short y)
    {
	return x - (x / y) * y;
    }

    inline int mod(const int x, const int y)
    {
	return x - (x / y) * y;
    }

    inline unsigned int mod(const unsigned int x, const unsigned int y)
    {
	return x - (x / y) * y;
    }

    inline long mod(const long x, const long y)
    {
	return x - (x / y) * y;
    }

    inline unsigned long mod(const unsigned long x, const unsigned long y)
    {
	return x - (x / y) * y;
    }

    inline long long mod(const long long x, const long long y)
    {
	return x - (x / y) * y;
    }

    inline unsigned long long mod(const unsigned long long x, const unsigned long long y)
    {
	return x - (x / y) * y;
    }

    inline float mod(const float x, const float y)
    {
	return x - std::floor(x / y) * y;
    }

    inline double mod(const double x, const double y)
    {
	return x - std::floor(x / y) * y;
    }

    inline long double mod(const long double x, const long double y)
    {
	return x - std::floor(x / y) * y;
    }

    inline signed char sign(const signed char x)
    {
	return (x < 0 ? -1 : x > 0 ? +1 : 0);
    }

    inline unsigned char sign(const unsigned char x)
    {
	return (x > 0 ? +1 : 0);
    }

    inline short sign(const short x)
    {
	return (x < 0 ? -1 : x > 0 ? +1 : 0);
    }

    inline unsigned short sign(const unsigned short x)
    {
	return (x > 0 ? +1 : 0);
    }

    inline int sign(const int x)
    {
	return (x < 0 ? -1 : x > 0 ? +1 : 0);
    }

    inline unsigned int sign(const unsigned int x)
    {
	return (x > 0 ? +1 : 0);
    }

    inline long sign(const long x)
    {
	return (x < 0 ? -1 : x > 0 ? +1 : 0);
    }

    inline unsigned long sign(const unsigned long x)
    {
	return (x > 0 ? +1 : 0);
    }

    inline long long sign(const long long x)
    {
	return (x < 0 ? -1 : x > 0 ? +1 : 0);
    }

    inline unsigned long long sign(const unsigned long long x)
    {
	return (x > 0 ? +1 : 0);
    }

    inline float sign(const float x)
    {
	return (x < 0.0f ? -1.0f : x > 0.0f ? +1.0f : 0.0f);
    }

    inline double sign(const double x)
    {
	return (x < 0.0 ? -1.0 : x > 0.0 ? +1.0 : 0.0);
    }

    inline long double sign(const long double x)
    {
	return (x < 0.0L ? -1.0L : x > 0.0L ? +1.0L : 0.0L);
    }

    using std::abs;

    inline bool abs(const bool x)
    {
	return x;
    }

    inline signed char abs(const signed char x)
    {
	return (x < 0 ? -x : x);
    }

    inline unsigned char abs(const unsigned char x)
    {
	return x;
    }

    inline short abs(const short x)
    {
	return (x < 0 ? -x : x);
    }

    inline unsigned short abs(const unsigned short x)
    {
	return x;
    }

    inline unsigned int abs(const unsigned int x)
    {
	return x;
    }

    inline unsigned long abs(const unsigned long x)
    {
	return x;
    }

    inline unsigned long long abs(const unsigned long long x)
    {
	return x;
    }

    inline float radians(const float x)
    {
	return x * static_cast<float>(M_PI / 180.0);
    }

    inline double radians(const double x)
    {
	return x * (M_PI / 180.0);
    }

    inline long double radians(const long double x)
    {
	return x * (M_PIl / 180.0L);
    }

    inline float degrees(const float x)
    {
	return x * static_cast<float>(180.0 / M_PI);
    }

    inline double degrees(const double x)
    {
	return x * (180.0 / M_PI);
    }

    inline long double degrees(const long double x)
    {
	return x * (180.0L / M_PIl);
    }

    using std::sin;

    using std::cos;

    using std::tan;

    using std::asin;

    using std::acos;

    using std::atan;

    inline float atan(const float x, const float y)
    {
	return std::atan2(x, y);
    }

    inline double atan(const double x, const double y)
    {
	return std::atan2(x, y);
    }

    inline long double atan(const long double x, const long double y)
    {
	return std::atan2(x, y);
    }

    using std::pow;

    using std::exp;

#if GLVM_HAVE_CXX0X_MATH
    using std::exp2;
#else
    using ::exp2;

    inline float exp2(const float x)
    {
	return ::exp2f(x);
    }

    inline long double exp2(const long double x)
    {
	return ::exp2l(x);
    }
#endif

    using std::log;

#if GLVM_HAVE_CXX0X_MATH
    using std::log2;
#else
    using ::log2;

    inline float log2(const float x)
    {
	return ::log2f(x);
    }

    inline long double log2(const long double x)
    {
	return ::log2l(x);
    }
#endif

    using std::log10;

    using std::sqrt;

    inline float inversesqrt(const float x)
    {
	return 1.0f / std::sqrt(x);
    }

    inline double inversesqrt(const double x)
    {
	return 1.0 / std::sqrt(x);
    }

    inline long double inversesqrt(const long double x)
    {
	return 1.0L / std::sqrt(x);
    }

#if GLVM_HAVE_CXX0X_MATH
    using std::cbrt;
#else
    using ::cbrt;

    inline float cbrt(const float x)
    {
	return ::cbrtf(x);
    }

    inline long double cbrt(const long double x)
    {
	return ::cbrtl(x);
    }
#endif

#if GLVM_HAVE_CXX0X_MATH
    using std::round;
#else
    inline float round(const float x)
    {
	return ::roundf(x);
    }

    inline long double round(const long double x)
    {
	return ::roundl(x);
    }
#endif

    using std::floor;

    using std::ceil;

    inline float fract(const float x)
    {
	return x - std::floor(x);
    }

    inline double fract(const double x)
    {
	return x - std::floor(x);
    }

    inline long double fract(const long double x)
    {
	return x - std::floor(x);
    }

    using std::isfinite;

    using std::isinf;

    using std::isnan;

    using std::isnormal;

    inline float mix(const float x, const float y, const float alpha)
    {
	return x * (1.0f - alpha) + y * alpha;
    }

    inline double mix(const double x, const double y, const double alpha)
    {
	return x * (1.0 - alpha) + y * alpha;
    }

    inline long double mix(const long double x, const long double y, const long double alpha)
    {
	return x * (1.0L - alpha) + y * alpha;
    }

    inline float smoothstep(const float x, const float edge0, const float edge1)
    {
	const float t = glvm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return t * t * (3.0f - t * 2.0f);
    }

    inline double smoothstep(const double x, const double edge0, const double edge1)
    {
	const double t = glvm::clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return t * t * (3.0 - t * 2.0);
    }

    inline long double smoothstep(const long double x, const long double edge0, const long double edge1)
    {
	const long double t = glvm::clamp((x - edge0) / (edge1 - edge0), 0.0L, 1.0L);
	return t * t * (3.0L - t * 2.0L);
    }

    inline bool greaterThan(const bool a, const bool b)
    {
	return a > b;
    }

    inline bool greaterThan(const signed char a, const signed char b)
    {
	return a > b;
    }

    inline bool greaterThan(const unsigned char a, const unsigned char b)
    {
	return a > b;
    }

    inline bool greaterThan(const short a, const short b)
    {
	return a > b;
    }

    inline bool greaterThan(const unsigned short a, const unsigned short b)
    {
	return a > b;
    }

    inline bool greaterThan(const int a, const int b)
    {
	return a > b;
    }

    inline bool greaterThan(const unsigned int a, const unsigned int b)
    {
	return a > b;
    }

    inline bool greaterThan(const long a, const long b)
    {
	return a > b;
    }

    inline bool greaterThan(const unsigned long a, const unsigned long b)
    {
	return a > b;
    }

    inline bool greaterThan(const long long a, const long long b)
    {
	return a > b;
    }

    inline bool greaterThan(const unsigned long long a, const unsigned long long b)
    {
	return a > b;
    }

    inline bool greaterThan(const float a, const float b)
    {
	return a > b;
    }

    inline bool greaterThan(const double a, const double b)
    {
	return a > b;
    }

    inline bool greaterThan(const long double a, const long double b)
    {
	return a > b;
    }

    inline bool greaterThanEqual(const bool a, const bool b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const signed char a, const signed char b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const unsigned char a, const unsigned char b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const short a, const short b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const unsigned short a, const unsigned short b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const int a, const int b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const unsigned int a, const unsigned int b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const long a, const long b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const unsigned long a, const unsigned long b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const long long a, const long long b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const unsigned long long a, const unsigned long long b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const float a, const float b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const double a, const double b)
    {
	return a >= b;
    }

    inline bool greaterThanEqual(const long double a, const long double b)
    {
	return a >= b;
    }

    inline bool lessThan(const bool a, const bool b)
    {
	return a < b;
    }

    inline bool lessThan(const signed char a, const signed char b)
    {
	return a < b;
    }

    inline bool lessThan(const unsigned char a, const unsigned char b)
    {
	return a < b;
    }

    inline bool lessThan(const short a, const short b)
    {
	return a < b;
    }

    inline bool lessThan(const unsigned short a, const unsigned short b)
    {
	return a < b;
    }

    inline bool lessThan(const int a, const int b)
    {
	return a < b;
    }

    inline bool lessThan(const unsigned int a, const unsigned int b)
    {
	return a < b;
    }

    inline bool lessThan(const long a, const long b)
    {
	return a < b;
    }

    inline bool lessThan(const unsigned long a, const unsigned long b)
    {
	return a < b;
    }

    inline bool lessThan(const long long a, const long long b)
    {
	return a < b;
    }

    inline bool lessThan(const unsigned long long a, const unsigned long long b)
    {
	return a < b;
    }

    inline bool lessThan(const float a, const float b)
    {
	return a < b;
    }

    inline bool lessThan(const double a, const double b)
    {
	return a < b;
    }

    inline bool lessThan(const long double a, const long double b)
    {
	return a < b;
    }

    inline bool lessThanEqual(const bool a, const bool b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const signed char a, const signed char b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const unsigned char a, const unsigned char b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const short a, const short b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const unsigned short a, const unsigned short b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const int a, const int b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const unsigned int a, const unsigned int b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const long a, const long b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const unsigned long a, const unsigned long b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const long long a, const long long b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const unsigned long long a, const unsigned long long b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const float a, const float b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const double a, const double b)
    {
	return a <= b;
    }

    inline bool lessThanEqual(const long double a, const long double b)
    {
	return a <= b;
    }

    inline bool equal(const bool a, const bool b)
    {
	return a == b;
    }

    inline bool equal(const signed char a, const signed char b)
    {
	return a == b;
    }

    inline bool equal(const unsigned char a, const unsigned char b)
    {
	return a == b;
    }

    inline bool equal(const short a, const short b)
    {
	return a == b;
    }

    inline bool equal(const unsigned short a, const unsigned short b)
    {
	return a == b;
    }

    inline bool equal(const int a, const int b)
    {
	return a == b;
    }

    inline bool equal(const unsigned int a, const unsigned int b)
    {
	return a == b;
    }

    inline bool equal(const long a, const long b)
    {
	return a == b;
    }

    inline bool equal(const unsigned long a, const unsigned long b)
    {
	return a == b;
    }

    inline bool equal(const long long a, const long long b)
    {
	return a == b;
    }

    inline bool equal(const unsigned long long a, const unsigned long long b)
    {
	return a == b;
    }

    /* The 'equal' function for floating point numbers is adapted from
     * http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
     *
     * The max_ulps parameter measures the maximum number of floating point
     * numbers that lie inbetween a and b.
     *
     * These functions relies on IEEE 754 floating point numbers and
     * two-complement integers of the same size. The second requirement is the
     * reason why there is no version for long doubles.
     */

    inline bool equal(const float a, const float b, const int max_ulps = 1)
    {
	bool result;

	if (std::isinf(a) || std::isinf(b))
	{
	    result = (std::isinf(a) && std::isinf(b)
		    && static_cast<int>(glvm::sign(a)) == static_cast<int>(glvm::sign(b)));
	}
	else if (std::isnan(a) || std::isnan(b))
	{
	    result = false;
	}
	else
	{
	    assert(sizeof(float) == sizeof(int32_t));
	    assert(max_ulps > 0);
	    union { float f; int32_t i; } x;
	    union { float f; int32_t i; } y;
	    x.f = a;
	    y.f = b;
	    if (x.i < 0)
	    {
		x.i = 0x80000000 - x.i;
	    }
	    if (y.i < 0)
	    {
		y.i = 0x80000000 - y.i;
	    }
	    int32_t intdiff = glvm::abs(x.i - y.i);
	    result = (intdiff <= max_ulps);
	}
	return result;
    }

    inline bool equal(const double a, const double b, const int max_ulps = 1)
    {
	bool result;

	if (std::isinf(a) || std::isinf(b))
	{
	    result = (std::isinf(a) && std::isinf(b)
		    && static_cast<int>(glvm::sign(a)) == static_cast<int>(glvm::sign(b)));
	}
	else if (std::isnan(a) || std::isnan(b))
	{
	    result = false;
	}
	else
	{
	    assert(sizeof(double) == sizeof(int64_t));
	    assert(max_ulps > 0);
	    union { double d; int64_t i; } x;
	    union { double d; int64_t i; } y;
	    x.d = a;
	    y.d = b;
	    if (x.i < 0)
	    {
		x.i = 0x8000000000000000LL - x.i;
	    }
	    if (y.i < 0)
	    {
		y.i = 0x8000000000000000LL - y.i;
	    }
	    int64_t intdiff = glvm::abs(x.i - y.i);
	    result = (intdiff <= max_ulps);
	}
	return result;
    }

    inline bool equal(const long double a, const long double b, const int max_ulps = 1)
    {
	bool result;

	if (std::isinf(a) || std::isinf(b))
	{
	    result = (std::isinf(a) && std::isinf(b)
		    && static_cast<int>(glvm::sign(a)) == static_cast<int>(glvm::sign(b)));
	}
	else if (std::isnan(a) || std::isnan(b))
	{
	    result = false;
	}
	else
	{
	    assert(0);
	    assert(max_ulps > 0);
	    /* FIXME: Implement this for long double.
	     * Problem: There is no int128_t on most platforms. */
	    result = (max_ulps < 1);
	}
	return result;
    }

    inline bool notEqual(const bool a, const bool b)
    {
	return a != b;
    }

    inline bool notEqual(const signed char a, const signed char b)
    {
	return a != b;
    }

    inline bool notEqual(const unsigned char a, const unsigned char b)
    {
	return a != b;
    }

    inline bool notEqual(const short a, const short b)
    {
	return a != b;
    }

    inline bool notEqual(const unsigned short a, const unsigned short b)
    {
	return a != b;
    }

    inline bool notEqual(const int a, const int b)
    {
	return a != b;
    }

    inline bool notEqual(const unsigned int a, const unsigned int b)
    {
	return a != b;
    }

    inline bool notEqual(const long a, const long b)
    {
	return a != b;
    }

    inline bool notEqual(const unsigned long a, const unsigned long b)
    {
	return a != b;
    }

    inline bool notEqual(const long long a, const long long b)
    {
	return a != b;
    }

    inline bool notEqual(const unsigned long long a, const unsigned long long b)
    {
	return a != b;
    }

    inline bool notEqual(const float a, const float b, const int max_ulps = 1)
    {
	return !glvm::equal(a, b, max_ulps);
    }

    inline bool notEqual(const double a, const double b, const int max_ulps = 1)
    {
	return !glvm::equal(a, b, max_ulps);
    }

    inline bool notEqual(const long double a, const long double b, const int max_ulps = 1)
    {
	return !glvm::equal(a, b, max_ulps);
    }

    inline bool any(const bool a)
    {
	return a;
    }

    inline bool all(const bool a)
    {
	return a;
    }

    inline bool negate(const bool a)
    {
	return !a;
    }


    /* The array class is the base for both the vec and the mat class. */

    template<typename T, int rows, int cols>
    class array
    {
	public:

	union
	{
    	    T v[rows][cols];
	    T vl[rows * cols];
	};

	/* Constructors, Destructor */

	array() {}

	// The following constructor are needed to construct vec and mat
	// objects from a series of values. We need constructors for 1, 2, 3, 
	// 4, 6, 8, 9, 12, and 16 elements.

	array(const T x)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] = x;
	}
	
	array(const T x, const T y)
	{
	    vl[0] = x;
	    vl[1] = y;
	}

	array(const T x, const T y, const T z)
	{
	    vl[0] = x;
	    vl[1] = y;
	    vl[2] = z;
	}

	array(const T x, const T y, const T z, const T w)
	{
	    vl[0] = x;
	    vl[1] = y;
	    vl[2] = z;
	    vl[3] = w;
	}

	array(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5)
	{
	    vl[0] = v0;
	    vl[1] = v1;
	    vl[2] = v2;
	    vl[3] = v3;
	    vl[4] = v4;
	    vl[5] = v5;
	}

	array(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7)
	{
	    vl[0] = v0;
	    vl[1] = v1;
	    vl[2] = v2;
	    vl[3] = v3;
	    vl[4] = v4;
	    vl[5] = v5;
	    vl[6] = v6;
	    vl[7] = v7;
	}

	array(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7, const T v8)
	{
	    vl[0] = v0;
	    vl[1] = v1;
	    vl[2] = v2;
	    vl[3] = v3;
	    vl[4] = v4;
	    vl[5] = v5;
	    vl[6] = v6;
	    vl[7] = v7;
	    vl[8] = v8;
	}

	array(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5,
		const T v6, const T v7, const T v8, const T v9, const T v10, const T v11)
	{
	    vl[0] = v0;
	    vl[1] = v1;
	    vl[2] = v2;
	    vl[3] = v3;
	    vl[4] = v4;
	    vl[5] = v5;
	    vl[6] = v6;
	    vl[7] = v7;
	    vl[8] = v8;
	    vl[9] = v9;
	    vl[10] = v10;
	    vl[11] = v11;
	}

	array(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7,
		const T v8, const T v9, const T v10, const T v11, const T v12, const T v13, const T v14, const T v15)
	{
	    vl[0] = v0;
	    vl[1] = v1;
	    vl[2] = v2;
	    vl[3] = v3;
	    vl[4] = v4;
	    vl[5] = v5;
	    vl[6] = v6;
	    vl[7] = v7;
	    vl[8] = v8;
	    vl[9] = v9;
	    vl[10] = v10;
	    vl[11] = v11;
	    vl[12] = v12;
	    vl[13] = v13;
	    vl[14] = v14;
	    vl[15] = v15;
	}

	array(const T *a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] = a[i];
	}

	template<typename U>
	array(const array<U, rows, cols> &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] = a.vl[i];
	}

	array(const std::string &s)
	{
	    std::istringstream is(s);
	    for (int i = 0; i < rows * cols; i++)
		is >> vl[i];
	}

	~array() {}

	array<T, rows - 1, cols - 1> _strike(const int row, const int col) const
	{
	    array<T, rows - 1, cols - 1> r;
	    int ii = 0;
	    for (int i = 0; i < rows; i++)
	    {
		if (i == row)
		{
		    continue;
		}
		int jj = 0;
		for (int j = 0; j < cols; j++)
		{
		    if (j == col)
		    {
			continue;
		    }
		    r.v[ii][jj] = v[i][j];
		    jj++;
		}
		ii++;
	    }
	    return r;
	}

	template<int subrows, int subcols>
	void _setSubArray(const array<T, subrows, subcols> &m, const int row = 0, const int col = 0)
	{
	    for (int i = 0; i < subrows; i++)
		for (int j = 0; j < subcols; j++)
		    v[row + i][col + j] = m.v[i][j];
	}

	/* Convert to/from string */

	std::string str() const
	{
	    std::ostringstream os;
	    os.precision(std::numeric_limits<T>::digits10 * 2);
	    for (int i = 0; i < rows * cols; i++)
		os << vl[i] << " ";
	    return os.str();
	}

	friend std::ostream &operator<<(std::ostream &os, const array &a)
	{
	    return os << a.str();
	}

	friend std::istream &operator>>(std::istream &is, array &a)
	{
	    char blank;
	    for (int i = 0; i < rows * cols; i++)
		is >> a.vl[i];
	    is >> blank;
	    return is;
	}

	/* Operators */

	const array &_op_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] = a.vl[i];
	    return *this;
	}
	
	array _op_plus(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] + a.vl[i];
	    return r;
	}

	const array &_op_plus_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] += a.vl[i];
	    return *this;
	}

	array _op_unary_plus() const
	{
	    return *this;
	}

	array _op_minus(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] - a.vl[i];
	    return r;
	}

	const array &_op_minus_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] -= a.vl[i];
	    return *this;
	}

	array _op_unary_minus() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	array _op_scal_mult(const T s) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] * s;
	    return r;
	}

	const array &_op_scal_mult_assign(const T s)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] *= s;
	    return *this;
	}

	array _op_comp_mult(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] * a.vl[i];
	    return r;
	}

	const array &_op_comp_mult_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] *= a.vl[i];
	    return *this;
	}

	array _op_scal_div(const T s) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] / s;
	    return r;
	}
     
	const array &_op_scal_div_assign(const T s)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] /= s;
	    return *this;
	}

	array _op_comp_div(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] / a.vl[i];
	    return r;
	}

	const array &_op_comp_div_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] /= a.vl[i];
	    return *this;
	}

	array _op_scal_mod(const T s) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] % s;
	    return r;
	}
     
	const array &_op_scal_mod_assign(const T s)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] %= s;
	    return *this;
	}

	array _op_comp_mod(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] % a.vl[i];
	    return r;
	}

	const array &_op_comp_mod_assign(const array &a)
	{
	    for (int i = 0; i < rows * cols; i++)
		vl[i] %= a.vl[i];
	    return *this;
	}

	bool _op_equal(const array &a) const
	{
	    bool ret = true;
	    for (int i = 0; i < rows * cols; i++)
	    {
		if (vl[i] != a.vl[i])
		{
		    ret = false;
		    break;
		}
	    }
	    return ret;
	}

	bool _op_notequal(const array &a) const
	{
	    return !this->_op_equal(a);
	}

	/* Trigonometric functions */

	array _sin() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sin(vl[i]);
	    return r;
	}

	array _cos() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::cos(vl[i]);
	    return r;
	}

	array _tan() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::tan(vl[i]);
	    return r;
	}

	array _asin() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::asin(vl[i]);
	    return r;
	}

	array _acos() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::acos(vl[i]);
	    return r;
	}

	array _atan() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::atan(vl[i]);
	    return r;
	}

	array _atan(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::atan(vl[i], a.vl[i]);
	    return r;
	}

	array _radians() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::radians(vl[i]);
	    return r;
	}

	array _degrees() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::degrees(vl[i]);
	    return r;
	}

	/* Exponential functions */

	array _pow(const T p) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::pow(vl[i], p);
	    return r;
	}

	array _exp() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::exp(vl[i]);
	    return r;
	}

	array _exp2() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::exp2(vl[i]);
	    return r;
	}

	array _log() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log(vl[i]);
	    return r;
	}

	array _log2() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log2(vl[i]);
	    return r;
	}

	array _log10() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log10(vl[i]);
	    return r;
	}

	array _sqrt() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sqrt(vl[i]);
	    return r;
	}

	array _inversesqrt() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::inversesqrt(vl[i]);
	    return r;
	}

	array _cbrt() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::cbrt(vl[i]);
	    return r;
	}

	/* Common functions */

	array<bool, rows, cols> _isfinite() const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isfinite(vl[i]);
	    return r;
	}

	array<bool, rows, cols> _isinf() const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isinf(vl[i]);
	    return r;
	}

	array<bool, rows, cols> _isnan() const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isnan(vl[i]);
	    return r;
	}

	array<bool, rows, cols> _isnormal() const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isnormal(vl[i]);
	    return r;
	}

	array _sign() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sign(vl[i]);
	    return r;
	}

	array _abs() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::abs(vl[i]);
	    return r;
	}

	array _floor() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::floor(vl[i]);
	    return r;
	}

	array _ceil() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::ceil(vl[i]);
	    return r;
	}

	array _round() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::round(vl[i]);
	    return r;
	}

	array _fract() const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::fract(vl[i]);
	    return r;
	}

	array _min(const T x) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::min(x, vl[i]);
	    return r;
	}

	array _min(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::min(a.vl[i], vl[i]);
	    return r;
	}

	array _max(const T x) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::max(x, vl[i]);
	    return r;
	}

	array _max(const array &a) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::max(a.vl[i], vl[i]);
	    return r;
	}

	array _clamp(const T minval, const T maxval) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::clamp(vl[i], minval, maxval);
	    return r;
	}

	array _clamp(const array &minval, const array &maxval) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::clamp(vl[i], minval.vl[i], maxval.vl[i]);
	    return r;
	}

	array _mix(const array &a, const T alpha) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mix(vl[i], a.vl[i], alpha);
	    return r;
	}

	array _mix(const array &a, const array &alpha) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mix(vl[i], a.vl[i], alpha.vl[i]);
	    return r;
	}

	array _step(const T edge) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::step(vl[i], edge);
	    return r;
	}

	array _step(const array &edge) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::step(vl[i], edge.vl[i]);
	    return r;
	}

	array _smoothstep(const T edge0, const T edge1) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::smoothstep(vl[i], edge0, edge1);
	    return r;
	}

	array _smoothstep(const array &edge0, const array &edge1) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::smoothstep(vl[i], edge0.vl[i], edge1.vl[i]);
	    return r;
	}

	array _mod(const T y) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mod(vl[i], y);
	    return r;
	}

	array _mod(const array &y) const
	{
	    array r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mod(vl[i], y.vl[i]);
	    return r;
	}

	/* Comparison functions */

	array<bool, rows, cols> _greaterThan(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::greaterThan(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _greaterThanEqual(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::greaterThanEqual(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _lessThan(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::lessThan(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _lessThanEqual(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::lessThanEqual(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _equal(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::equal(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _equal(const array &a, const int max_ulps) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::equal(vl[i], a.vl[i], max_ulps);
	    return r;
	}

	array<bool, rows, cols> _notEqual(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::notEqual(vl[i], a.vl[i]);
	    return r;
	}

	array<bool, rows, cols> _notEqual(const array &a, const int max_ulps) const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::notEqual(vl[i], a.vl[i], max_ulps);
	    return r;
	}

	bool _any() const
	{
	    bool ret = false;
	    for (int i = 0; i < rows * cols; i++)
	    {
		if (vl[i])
		{
		    ret = true;
		    break;
		}
	    }
	    return ret;
	}

	bool _all() const
	{
	    bool ret = true;
	    for (int i = 0; i < rows * cols; i++)
	    {
		if (!vl[i])
		{
		    ret = false;
		    break;
		}
	    }
	    return ret;
	}

	array<bool, rows, cols> _negate() const
	{
	    array<bool, rows, cols> r;
	    for (int i = 0; i < rows * cols; i++)
		r.vl[i] = !vl[i];
	    return r;
	}

	/* Geometric functions */

	T _length() const
	{
	    T l = static_cast<T>(0);
	    for (int i = 0; i < rows * cols; i++)
		l += vl[i] * vl[i];
	    return glvm::sqrt(l);
	}

	T _distance(const array &a) const
	{
	    return (*this - a).length();
	}

	T _dot(const array &a) const
	{
	    T d = static_cast<T>(0);
	    for (int i = 0; i < rows; i++)
		d += vl[i] * a.vl[i];
	    return d;
	}

	array _normalize() const
	{
	    return this->_op_scal_div(this->_length());
	}

	array _faceforward(const array &I, const array &Nref) const
	{
	    return Nref.dot(I) < static_cast<T>(0) ? *this : - *this;
	}

	array _reflect(const array &N) const
	{
	    return *this - N * (N.dot(*this) * static_cast<T>(2));
	}

	array _refract(const array &N, T eta) const
	{
	    const T d = N.dot(*this);
	    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
	    return k < static_cast<T>(0) ? array<T, rows, 1>(static_cast<T>(0)) : *this * eta - N * (eta * d + glvm::sqrt(k));
	}

	array<T, cols, rows> _transpose() const
	{
	    array<T, cols, rows> r;
	    for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		    r.v[i][j] = array<T, rows, cols>::v[j][i];
	    return r;
	}
    };


    /* Swizzlers */

    template<typename T>
    class swizzler2 : public array<T, 2, 1>
    {
	public:
        T &x, &y;

    	swizzler2(T &s0, T &s1) : array<T, 2, 1>(s0, s1), x(s0), y(s1) {}

    	const swizzler2 &operator=(const swizzler2 &s)
	{
	    T a = s.x;
	    T b = s.y;
	    x = a;
	    y = b;
	    array<T, 2, 1>::vl[0] = a;
	    array<T, 2, 1>::vl[1] = b;
	    return *this;
	}

    	const swizzler2 &operator=(const array<T, 2, 1> &v)
	{
	    x = v.vl[0];
	    y = v.vl[1];
	    array<T, 2, 1>::vl[0] = v.vl[0];
	    array<T, 2, 1>::vl[1] = v.vl[1];
	    return *this;
	}
    };

    template<typename T> 
    class swizzler3 : public array<T, 3, 1>
    {
	public:
        T &x, &y, &z;

    	swizzler3(T &s0, T &s1, T &s2) : array<T, 3, 1>(s0, s1, s2), x(s0), y(s1), z(s2) {}

    	const swizzler3 &operator=(const swizzler3 &s)
	{
	    T a = s.x;
	    T b = s.y;
	    T c = s.z;
	    x = a;
	    y = b;
	    z = c;
	    array<T, 3, 1>::vl[0] = a;
	    array<T, 3, 1>::vl[1] = b;
	    array<T, 3, 1>::vl[2] = c;
	    return *this;
	}

    	const swizzler3 &operator=(const array<T, 3, 1> &v)
	{
	    x = v.vl[0];
	    y = v.vl[1];
	    z = v.vl[2];
	    array<T, 3, 1>::vl[0] = v.vl[0];
	    array<T, 3, 1>::vl[1] = v.vl[1];
	    array<T, 3, 1>::vl[2] = v.vl[2];
	    return *this;
	}
    };

    template<typename T>
    class swizzler4 : public array<T, 4, 1>
    {
	public:
        T &x, &y, &z, &w;

    	swizzler4(T &s0, T &s1, T &s2, T &s3) : array<T, 4, 1>(s0, s1, s2, s3), x(s0), y(s1), z(s2), w(s3) {}

    	const swizzler4 &operator=(const swizzler4 &s)
	{
	    T a = s.x;
	    T b = s.y;
	    T c = s.z;
	    T d = s.w;
	    x = a;
	    y = b;
	    z = c;
	    w = d;
	    array<T, 4, 1>::vl[0] = a;
	    array<T, 4, 1>::vl[1] = b;
	    array<T, 4, 1>::vl[2] = c;
	    array<T, 4, 1>::vl[3] = d;
	    return *this;
	}

    	const swizzler4 &operator=(const array<T, 4, 1> &v)
	{
	    x = v.vl[0];
	    y = v.vl[1];
	    z = v.vl[2];
	    w = v.vl[3];
	    array<T, 4, 1>::vl[0] = v.vl[0];
	    array<T, 4, 1>::vl[1] = v.vl[1];
	    array<T, 4, 1>::vl[2] = v.vl[2];
	    array<T, 4, 1>::vl[3] = v.vl[3];
	    return *this;
	}
    };


    /* Vectors */

    template<typename T, int rows>
    class vec : public array<T, rows, 1>
    {
	public:

	/* Constructors, Destructor */

	vec() {}
	vec(const array<T, rows, 1> &v) : array<T, rows, 1>(v) {}
	vec(const T x) : array<T, rows, 1>(x) {}
	vec(const T x, const T y) : array<T, rows, 1>(x, y) {}
	vec(const vec<T, 2> &xy) : array<T, rows, 1>(xy.vl[0], xy.vl[1]) {}
	vec(const swizzler2<T> &xy) : array<T, rows, 1>(xy.vl[0], xy.vl[1]) {}
	vec(const T x, const T y, const T z) : array<T, rows, 1>(x, y, z) {}
	vec(const vec<T, 3> &xyz) : array<T, rows, 1>(xyz.vl[0], xyz.vl[1], xyz.vl[2]) {}
	vec(const swizzler3<T> &xyz) : array<T, rows, 1>(xyz.vl[0], xyz.vl[1], xyz.vl[2]) {}
	vec(const vec<T, 2> &xy, const T z) : array<T, rows, 1>(xy.vl[0], xy.vl[1], z) {}
	vec(const swizzler2<T> &xy, const T z) : array<T, rows, 1>(xy.vl[0], xy.vl[1], z) {}
	vec(const T x, const vec<T, 2> &yz) : array<T, rows, 1>(z, yz.vl[0], yz.vl[1]) {}
	vec(const T x, const swizzler2<T> &yz) : array<T, rows, 1>(z, yz.vl[0], yz.vl[1]) {}
	vec(const T x, const T y, const T z, const T w) : array<T, rows, 1>(x, y, z, w) {}
	vec(const vec<T, 4> &xyzw) : array<T, rows, 1>(xyzw.vl[0], xyzw.vl[1], xyzw.vl[2], xyzw.vl[3]) {}
	vec(const swizzler4<T> &xyzw) : array<T, rows, 1>(xyzw.vl[0], xyzw.vl[1], xyzw.vl[2], xyzw.vl[3]) {}
	vec(const vec<T, 2> xy, const T z, const T w) : array<T, rows, 1>(xy.vl[0], xy.vl[1], z, w) {}
	vec(const swizzler2<T> xy, const T z, const T w) : array<T, rows, 1>(xy.vl[0], xy.vl[1], z, w) {}
	vec(const vec<T, 2> xy, const vec<T, 2> zw) : array<T, rows, 1>(xy.vl[0], xy.vl[1], zw.vl[0], zw.vl[1]) {}
	vec(const swizzler2<T> xy, const swizzler2<T> zw) : array<T, rows, 1>(xy.vl[0], xy.vl[1], zw.vl[0], zw.vl[1]) {}
	vec(const T x, const vec<T, 2> yz, const T w) : array<T, rows, 1>(x, yz.vl[0], yz.vl[1], w) {}
	vec(const T x, const swizzler2<T> yz, const T w) : array<T, rows, 1>(x, yz.vl[0], yz.vl[1], w) {}
	vec(const T x, const T y, const vec<T, 2> zw) : array<T, rows, 1>(x, y, zw.vl[0], zw.vl[1]) {}
	vec(const T x, const T y, const swizzler3<T> zw) : array<T, rows, 1>(x, y, zw.vl[0], zw.vl[1]) {}
	vec(const vec<T, 3> xyz, const T w) : array<T, rows, 1>(xyz.vl[0], xyz.vl[1], xyz.vl[2], w) {}
	vec(const swizzler3<T> xyz, const T w) : array<T, rows, 1>(xyz.vl[0], xyz.vl[1], xyz.vl[2], w) {}
	vec(const T x, const vec<T, 3> yzw) : array<T, rows, 1>(x, yzw.vl[0], yzw.vl[1], yzw.vl[2]) {}
	vec(const T x, const swizzler3<T> yzw) : array<T, rows, 1>(x, yzw.vl[0], yzw.vl[1], yzw.vl[2]) {}
	vec(const T *v) : array<T, rows, 1>(v) {}
	template<typename U>
	vec(const vec<U, rows> &v) : array<T, rows, 1>(v) {}
	vec(const std::string &s) : array<T, rows, 1>(s) {}
	~vec() {}

	/* Operators */

	T &operator[](const unsigned int i)
	{
	    return array<T, rows, 1>::vl[i];
	}

	const T &operator[](const unsigned int i) const
	{
	    return array<T, rows, 1>::vl[i];
	}

	const vec &operator=(const vec &a)                { array<T, rows, 1>::_op_assign(a); return *this; }
	vec operator+(const array<T, rows, 1> &a) const   { return array<T, rows, 1>::_op_plus(a); }
	const vec &operator+=(const array<T, rows, 1> &a) { array<T, rows, 1>::_op_plus_assign(a); return *this; }
	vec operator+() const                             { return array<T, rows, 1>::_op_unary_plus(); }
	vec operator-(const array<T, rows, 1> &a) const   { return array<T, rows, 1>::_op_minus(a); }
	const vec &operator-=(const array<T, rows, 1> &a) { array<T, rows, 1>::_op_minus_assign(a); return *this; }
	vec operator-() const                             { return array<T, rows, 1>::_op_unary_minus(); }
	vec operator*(const T s) const                    { return array<T, rows, 1>::_op_scal_mult(s); }
	friend vec operator*(const T s, const array<T, rows, 1> &a) { return a._op_scal_mult(s); }
	const vec &operator*=(const T s)                  { array<T, rows, 1>::_op_scal_mult_assign(s); return *this; }
	vec operator*(const array<T, rows, 1> &a) const   { return array<T, rows, 1>::_op_comp_mult(a); }
	const vec &operator*=(const array<T, rows, 1> &a) { array<T, rows, 1>::_op_comp_mult_assign(a); return *this; }
	vec operator/(const T s) const                    { return array<T, rows, 1>::_op_scal_div(s); }
	const vec &operator/=(const T s)                  { array<T, rows, 1>::_op_scal_div_assign(s); return *this; }
	vec operator/(const array<T, rows, 1> &a) const   { return array<T, rows, 1>::_op_comp_div(a); }
	const vec &operator/=(const array<T, rows, 1> &a) { array<T, rows, 1>::_op_comp_div_assign(a); return *this; }
	vec operator%(const T s) const                    { return array<T, rows, 1>::_op_scal_mod(s); }
	const vec &operator%=(const T s)                  { array<T, rows, 1>::_op_scal_mod_assign(s); return *this; }
	vec operator%(const array<T, rows, 1> &a) const   { return array<T, rows, 1>::_op_comp_mod(a); }
	const vec &operator%=(const array<T, rows, 1> &a) { array<T, rows, 1>::_op_comp_mod_assign(a); return *this; }
	bool operator==(const array<T, rows, 1> &a) const { return array<T, rows, 1>::_op_equal(a); }
	bool operator!=(const array<T, rows, 1> &a) const { return array<T, rows, 1>::_op_notequal(a); }

	/* Swizzling */

	const T &x() const { return array<T, rows, 1>::vl[0]; }
	const T &r() const { return array<T, rows, 1>::vl[0]; }
	const T &s() const { return array<T, rows, 1>::vl[0]; }
	const T &y() const { return array<T, rows, 1>::vl[1]; }
	const T &g() const { return array<T, rows, 1>::vl[1]; }
	const T &t() const { return array<T, rows, 1>::vl[1]; }
	const T &z() const { return array<T, rows, 1>::vl[2]; }
	const T &b() const { return array<T, rows, 1>::vl[2]; }
	const T &p() const { return array<T, rows, 1>::vl[2]; }
	const T &w() const { return array<T, rows, 1>::vl[3]; }
	const T &a() const { return array<T, rows, 1>::vl[3]; }
	const T &q() const { return array<T, rows, 1>::vl[3]; }
	vec<T, 2> xx() const { return vec<T, 2>(x(), x()); }
	vec<T, 2> rr() const { return vec<T, 2>(r(), r()); }
	vec<T, 2> ss() const { return vec<T, 2>(s(), s()); }
	vec<T, 2> xy() const { return vec<T, 2>(x(), y()); }
	vec<T, 2> rg() const { return vec<T, 2>(r(), g()); }
	vec<T, 2> st() const { return vec<T, 2>(s(), t()); }
	vec<T, 2> xz() const { return vec<T, 2>(x(), z()); }
	vec<T, 2> rb() const { return vec<T, 2>(r(), b()); }
	vec<T, 2> sp() const { return vec<T, 2>(s(), p()); }
	vec<T, 2> xw() const { return vec<T, 2>(x(), w()); }
	vec<T, 2> ra() const { return vec<T, 2>(r(), a()); }
	vec<T, 2> sq() const { return vec<T, 2>(s(), q()); }
	vec<T, 2> yx() const { return vec<T, 2>(y(), x()); }
	vec<T, 2> gr() const { return vec<T, 2>(g(), r()); }
	vec<T, 2> ts() const { return vec<T, 2>(t(), s()); }
	vec<T, 2> yy() const { return vec<T, 2>(y(), y()); }
	vec<T, 2> gg() const { return vec<T, 2>(g(), g()); }
	vec<T, 2> tt() const { return vec<T, 2>(t(), t()); }
	vec<T, 2> yz() const { return vec<T, 2>(y(), z()); }
	vec<T, 2> gb() const { return vec<T, 2>(g(), b()); }
	vec<T, 2> tp() const { return vec<T, 2>(t(), p()); }
	vec<T, 2> yw() const { return vec<T, 2>(y(), w()); }
	vec<T, 2> ga() const { return vec<T, 2>(g(), a()); }
	vec<T, 2> tq() const { return vec<T, 2>(t(), q()); }
	vec<T, 2> zx() const { return vec<T, 2>(z(), x()); }
	vec<T, 2> br() const { return vec<T, 2>(b(), r()); }
	vec<T, 2> ps() const { return vec<T, 2>(p(), s()); }
	vec<T, 2> zy() const { return vec<T, 2>(z(), y()); }
	vec<T, 2> bg() const { return vec<T, 2>(b(), g()); }
	vec<T, 2> pt() const { return vec<T, 2>(p(), t()); }
	vec<T, 2> zz() const { return vec<T, 2>(z(), z()); }
	vec<T, 2> bb() const { return vec<T, 2>(b(), b()); }
	vec<T, 2> pp() const { return vec<T, 2>(p(), p()); }
	vec<T, 2> zw() const { return vec<T, 2>(z(), w()); }
	vec<T, 2> ba() const { return vec<T, 2>(b(), a()); }
	vec<T, 2> pq() const { return vec<T, 2>(p(), q()); }
	vec<T, 2> wx() const { return vec<T, 2>(w(), x()); }
	vec<T, 2> ar() const { return vec<T, 2>(a(), r()); }
	vec<T, 2> qs() const { return vec<T, 2>(q(), s()); }
	vec<T, 2> wy() const { return vec<T, 2>(w(), y()); }
	vec<T, 2> ag() const { return vec<T, 2>(a(), g()); }
	vec<T, 2> qt() const { return vec<T, 2>(q(), t()); }
	vec<T, 2> wz() const { return vec<T, 2>(w(), z()); }
	vec<T, 2> ab() const { return vec<T, 2>(a(), b()); }
	vec<T, 2> qp() const { return vec<T, 2>(q(), p()); }
	vec<T, 2> ww() const { return vec<T, 2>(w(), w()); }
	vec<T, 2> aa() const { return vec<T, 2>(a(), a()); }
	vec<T, 2> qq() const { return vec<T, 2>(q(), q()); }
	vec<T, 3> xxx() const { return vec<T, 3>(x(), x(), x()); }
	vec<T, 3> rrr() const { return vec<T, 3>(r(), r(), r()); }
	vec<T, 3> sss() const { return vec<T, 3>(s(), s(), s()); }
	vec<T, 3> xxy() const { return vec<T, 3>(x(), x(), y()); }
	vec<T, 3> rrg() const { return vec<T, 3>(r(), r(), g()); }
	vec<T, 3> sst() const { return vec<T, 3>(s(), s(), t()); }
	vec<T, 3> xxz() const { return vec<T, 3>(x(), x(), z()); }
	vec<T, 3> rrb() const { return vec<T, 3>(r(), r(), b()); }
	vec<T, 3> ssp() const { return vec<T, 3>(s(), s(), p()); }
	vec<T, 3> xxw() const { return vec<T, 3>(x(), x(), w()); }
	vec<T, 3> rra() const { return vec<T, 3>(r(), r(), a()); }
	vec<T, 3> ssq() const { return vec<T, 3>(s(), s(), q()); }
	vec<T, 3> xyx() const { return vec<T, 3>(x(), y(), x()); }
	vec<T, 3> rgr() const { return vec<T, 3>(r(), g(), r()); }
	vec<T, 3> sts() const { return vec<T, 3>(s(), t(), s()); }
	vec<T, 3> xyy() const { return vec<T, 3>(x(), y(), y()); }
	vec<T, 3> rgg() const { return vec<T, 3>(r(), g(), g()); }
	vec<T, 3> stt() const { return vec<T, 3>(s(), t(), t()); }
	vec<T, 3> xyz() const { return vec<T, 3>(x(), y(), z()); }
	vec<T, 3> rgb() const { return vec<T, 3>(r(), g(), b()); }
	vec<T, 3> stp() const { return vec<T, 3>(s(), t(), p()); }
	vec<T, 3> xyw() const { return vec<T, 3>(x(), y(), w()); }
	vec<T, 3> rga() const { return vec<T, 3>(r(), g(), a()); }
	vec<T, 3> stq() const { return vec<T, 3>(s(), t(), q()); }
	vec<T, 3> xzx() const { return vec<T, 3>(x(), z(), x()); }
	vec<T, 3> rbr() const { return vec<T, 3>(r(), b(), r()); }
	vec<T, 3> sps() const { return vec<T, 3>(s(), p(), s()); }
	vec<T, 3> xzy() const { return vec<T, 3>(x(), z(), y()); }
	vec<T, 3> rbg() const { return vec<T, 3>(r(), b(), g()); }
	vec<T, 3> spt() const { return vec<T, 3>(s(), p(), t()); }
	vec<T, 3> xzz() const { return vec<T, 3>(x(), z(), z()); }
	vec<T, 3> rbb() const { return vec<T, 3>(r(), b(), b()); }
	vec<T, 3> spp() const { return vec<T, 3>(s(), p(), p()); }
	vec<T, 3> xzw() const { return vec<T, 3>(x(), z(), w()); }
	vec<T, 3> rba() const { return vec<T, 3>(r(), b(), a()); }
	vec<T, 3> spq() const { return vec<T, 3>(s(), p(), q()); }
	vec<T, 3> xwx() const { return vec<T, 3>(x(), w(), x()); }
	vec<T, 3> rar() const { return vec<T, 3>(r(), a(), r()); }
	vec<T, 3> sqs() const { return vec<T, 3>(s(), q(), s()); }
	vec<T, 3> xwy() const { return vec<T, 3>(x(), w(), y()); }
	vec<T, 3> rag() const { return vec<T, 3>(r(), a(), g()); }
	vec<T, 3> sqt() const { return vec<T, 3>(s(), q(), t()); }
	vec<T, 3> xwz() const { return vec<T, 3>(x(), w(), z()); }
	vec<T, 3> rab() const { return vec<T, 3>(r(), a(), b()); }
	vec<T, 3> sqp() const { return vec<T, 3>(s(), q(), p()); }
	vec<T, 3> xww() const { return vec<T, 3>(x(), w(), w()); }
	vec<T, 3> raa() const { return vec<T, 3>(r(), a(), a()); }
	vec<T, 3> sqq() const { return vec<T, 3>(s(), q(), q()); }
	vec<T, 3> yxx() const { return vec<T, 3>(y(), x(), x()); }
	vec<T, 3> grr() const { return vec<T, 3>(g(), r(), r()); }
	vec<T, 3> tss() const { return vec<T, 3>(t(), s(), s()); }
	vec<T, 3> yxy() const { return vec<T, 3>(y(), x(), y()); }
	vec<T, 3> grg() const { return vec<T, 3>(g(), r(), g()); }
	vec<T, 3> tst() const { return vec<T, 3>(t(), s(), t()); }
	vec<T, 3> yxz() const { return vec<T, 3>(y(), x(), z()); }
	vec<T, 3> grb() const { return vec<T, 3>(g(), r(), b()); }
	vec<T, 3> tsp() const { return vec<T, 3>(t(), s(), p()); }
	vec<T, 3> yxw() const { return vec<T, 3>(y(), x(), w()); }
	vec<T, 3> gra() const { return vec<T, 3>(g(), r(), a()); }
	vec<T, 3> tsq() const { return vec<T, 3>(t(), s(), q()); }
	vec<T, 3> yyx() const { return vec<T, 3>(y(), y(), x()); }
	vec<T, 3> ggr() const { return vec<T, 3>(g(), g(), r()); }
	vec<T, 3> tts() const { return vec<T, 3>(t(), t(), s()); }
	vec<T, 3> yyy() const { return vec<T, 3>(y(), y(), y()); }
	vec<T, 3> ggg() const { return vec<T, 3>(g(), g(), g()); }
	vec<T, 3> ttt() const { return vec<T, 3>(t(), t(), t()); }
	vec<T, 3> yyz() const { return vec<T, 3>(y(), y(), z()); }
	vec<T, 3> ggb() const { return vec<T, 3>(g(), g(), b()); }
	vec<T, 3> ttp() const { return vec<T, 3>(t(), t(), p()); }
	vec<T, 3> yyw() const { return vec<T, 3>(y(), y(), w()); }
	vec<T, 3> gga() const { return vec<T, 3>(g(), g(), a()); }
	vec<T, 3> ttq() const { return vec<T, 3>(t(), t(), q()); }
	vec<T, 3> yzx() const { return vec<T, 3>(y(), z(), x()); }
	vec<T, 3> gbr() const { return vec<T, 3>(g(), b(), r()); }
	vec<T, 3> tps() const { return vec<T, 3>(t(), p(), s()); }
	vec<T, 3> yzy() const { return vec<T, 3>(y(), z(), y()); }
	vec<T, 3> gbg() const { return vec<T, 3>(g(), b(), g()); }
	vec<T, 3> tpt() const { return vec<T, 3>(t(), p(), t()); }
	vec<T, 3> yzz() const { return vec<T, 3>(y(), z(), z()); }
	vec<T, 3> gbb() const { return vec<T, 3>(g(), b(), b()); }
	vec<T, 3> tpp() const { return vec<T, 3>(t(), p(), p()); }
	vec<T, 3> yzw() const { return vec<T, 3>(y(), z(), w()); }
	vec<T, 3> gba() const { return vec<T, 3>(g(), b(), a()); }
	vec<T, 3> tpq() const { return vec<T, 3>(t(), p(), q()); }
	vec<T, 3> ywx() const { return vec<T, 3>(y(), w(), x()); }
	vec<T, 3> gar() const { return vec<T, 3>(g(), a(), r()); }
	vec<T, 3> tqs() const { return vec<T, 3>(t(), q(), s()); }
	vec<T, 3> ywy() const { return vec<T, 3>(y(), w(), y()); }
	vec<T, 3> gag() const { return vec<T, 3>(g(), a(), g()); }
	vec<T, 3> tqt() const { return vec<T, 3>(t(), q(), t()); }
	vec<T, 3> ywz() const { return vec<T, 3>(y(), w(), z()); }
	vec<T, 3> gab() const { return vec<T, 3>(g(), a(), b()); }
	vec<T, 3> tqp() const { return vec<T, 3>(t(), q(), p()); }
	vec<T, 3> yww() const { return vec<T, 3>(y(), w(), w()); }
	vec<T, 3> gaa() const { return vec<T, 3>(g(), a(), a()); }
	vec<T, 3> tqq() const { return vec<T, 3>(t(), q(), q()); }
	vec<T, 3> zxx() const { return vec<T, 3>(z(), x(), x()); }
	vec<T, 3> brr() const { return vec<T, 3>(b(), r(), r()); }
	vec<T, 3> pss() const { return vec<T, 3>(p(), s(), s()); }
	vec<T, 3> zxy() const { return vec<T, 3>(z(), x(), y()); }
	vec<T, 3> brg() const { return vec<T, 3>(b(), r(), g()); }
	vec<T, 3> pst() const { return vec<T, 3>(p(), s(), t()); }
	vec<T, 3> zxz() const { return vec<T, 3>(z(), x(), z()); }
	vec<T, 3> brb() const { return vec<T, 3>(b(), r(), b()); }
	vec<T, 3> psp() const { return vec<T, 3>(p(), s(), p()); }
	vec<T, 3> zxw() const { return vec<T, 3>(z(), x(), w()); }
	vec<T, 3> bra() const { return vec<T, 3>(b(), r(), a()); }
	vec<T, 3> psq() const { return vec<T, 3>(p(), s(), q()); }
	vec<T, 3> zyx() const { return vec<T, 3>(z(), y(), x()); }
	vec<T, 3> bgr() const { return vec<T, 3>(b(), g(), r()); }
	vec<T, 3> pts() const { return vec<T, 3>(p(), t(), s()); }
	vec<T, 3> zyy() const { return vec<T, 3>(z(), y(), y()); }
	vec<T, 3> bgg() const { return vec<T, 3>(b(), g(), g()); }
	vec<T, 3> ptt() const { return vec<T, 3>(p(), t(), t()); }
	vec<T, 3> zyz() const { return vec<T, 3>(z(), y(), z()); }
	vec<T, 3> bgb() const { return vec<T, 3>(b(), g(), b()); }
	vec<T, 3> ptp() const { return vec<T, 3>(p(), t(), p()); }
	vec<T, 3> zyw() const { return vec<T, 3>(z(), y(), w()); }
	vec<T, 3> bga() const { return vec<T, 3>(b(), g(), a()); }
	vec<T, 3> ptq() const { return vec<T, 3>(p(), t(), q()); }
	vec<T, 3> zzx() const { return vec<T, 3>(z(), z(), x()); }
	vec<T, 3> bbr() const { return vec<T, 3>(b(), b(), r()); }
	vec<T, 3> pps() const { return vec<T, 3>(p(), p(), s()); }
	vec<T, 3> zzy() const { return vec<T, 3>(z(), z(), y()); }
	vec<T, 3> bbg() const { return vec<T, 3>(b(), b(), g()); }
	vec<T, 3> ppt() const { return vec<T, 3>(p(), p(), t()); }
	vec<T, 3> zzz() const { return vec<T, 3>(z(), z(), z()); }
	vec<T, 3> bbb() const { return vec<T, 3>(b(), b(), b()); }
	vec<T, 3> ppp() const { return vec<T, 3>(p(), p(), p()); }
	vec<T, 3> zzw() const { return vec<T, 3>(z(), z(), w()); }
	vec<T, 3> bba() const { return vec<T, 3>(b(), b(), a()); }
	vec<T, 3> ppq() const { return vec<T, 3>(p(), p(), q()); }
	vec<T, 3> zwx() const { return vec<T, 3>(z(), w(), x()); }
	vec<T, 3> bar() const { return vec<T, 3>(b(), a(), r()); }
	vec<T, 3> pqs() const { return vec<T, 3>(p(), q(), s()); }
	vec<T, 3> zwy() const { return vec<T, 3>(z(), w(), y()); }
	vec<T, 3> bag() const { return vec<T, 3>(b(), a(), g()); }
	vec<T, 3> pqt() const { return vec<T, 3>(p(), q(), t()); }
	vec<T, 3> zwz() const { return vec<T, 3>(z(), w(), z()); }
	vec<T, 3> bab() const { return vec<T, 3>(b(), a(), b()); }
	vec<T, 3> pqp() const { return vec<T, 3>(p(), q(), p()); }
	vec<T, 3> zww() const { return vec<T, 3>(z(), w(), w()); }
	vec<T, 3> baa() const { return vec<T, 3>(b(), a(), a()); }
	vec<T, 3> pqq() const { return vec<T, 3>(p(), q(), q()); }
	vec<T, 3> wxx() const { return vec<T, 3>(w(), x(), x()); }
	vec<T, 3> arr() const { return vec<T, 3>(a(), r(), r()); }
	vec<T, 3> qss() const { return vec<T, 3>(q(), s(), s()); }
	vec<T, 3> wxy() const { return vec<T, 3>(w(), x(), y()); }
	vec<T, 3> arg() const { return vec<T, 3>(a(), r(), g()); }
	vec<T, 3> qst() const { return vec<T, 3>(q(), s(), t()); }
	vec<T, 3> wxz() const { return vec<T, 3>(w(), x(), z()); }
	vec<T, 3> arb() const { return vec<T, 3>(a(), r(), b()); }
	vec<T, 3> qsp() const { return vec<T, 3>(q(), s(), p()); }
	vec<T, 3> wxw() const { return vec<T, 3>(w(), x(), w()); }
	vec<T, 3> ara() const { return vec<T, 3>(a(), r(), a()); }
	vec<T, 3> qsq() const { return vec<T, 3>(q(), s(), q()); }
	vec<T, 3> wyx() const { return vec<T, 3>(w(), y(), x()); }
	vec<T, 3> agr() const { return vec<T, 3>(a(), g(), r()); }
	vec<T, 3> qts() const { return vec<T, 3>(q(), t(), s()); }
	vec<T, 3> wyy() const { return vec<T, 3>(w(), y(), y()); }
	vec<T, 3> agg() const { return vec<T, 3>(a(), g(), g()); }
	vec<T, 3> qtt() const { return vec<T, 3>(q(), t(), t()); }
	vec<T, 3> wyz() const { return vec<T, 3>(w(), y(), z()); }
	vec<T, 3> agb() const { return vec<T, 3>(a(), g(), b()); }
	vec<T, 3> qtp() const { return vec<T, 3>(q(), t(), p()); }
	vec<T, 3> wyw() const { return vec<T, 3>(w(), y(), w()); }
	vec<T, 3> aga() const { return vec<T, 3>(a(), g(), a()); }
	vec<T, 3> qtq() const { return vec<T, 3>(q(), t(), q()); }
	vec<T, 3> wzx() const { return vec<T, 3>(w(), z(), x()); }
	vec<T, 3> abr() const { return vec<T, 3>(a(), b(), r()); }
	vec<T, 3> qps() const { return vec<T, 3>(q(), p(), s()); }
	vec<T, 3> wzy() const { return vec<T, 3>(w(), z(), y()); }
	vec<T, 3> abg() const { return vec<T, 3>(a(), b(), g()); }
	vec<T, 3> qpt() const { return vec<T, 3>(q(), p(), t()); }
	vec<T, 3> wzz() const { return vec<T, 3>(w(), z(), z()); }
	vec<T, 3> abb() const { return vec<T, 3>(a(), b(), b()); }
	vec<T, 3> qpp() const { return vec<T, 3>(q(), p(), p()); }
	vec<T, 3> wzw() const { return vec<T, 3>(w(), z(), w()); }
	vec<T, 3> aba() const { return vec<T, 3>(a(), b(), a()); }
	vec<T, 3> qpq() const { return vec<T, 3>(q(), p(), q()); }
	vec<T, 3> wwx() const { return vec<T, 3>(w(), w(), x()); }
	vec<T, 3> aar() const { return vec<T, 3>(a(), a(), r()); }
	vec<T, 3> qqs() const { return vec<T, 3>(q(), q(), s()); }
	vec<T, 3> wwy() const { return vec<T, 3>(w(), w(), y()); }
	vec<T, 3> aag() const { return vec<T, 3>(a(), a(), g()); }
	vec<T, 3> qqt() const { return vec<T, 3>(q(), q(), t()); }
	vec<T, 3> wwz() const { return vec<T, 3>(w(), w(), z()); }
	vec<T, 3> aab() const { return vec<T, 3>(a(), a(), b()); }
	vec<T, 3> qqp() const { return vec<T, 3>(q(), q(), p()); }
	vec<T, 3> www() const { return vec<T, 3>(w(), w(), w()); }
	vec<T, 3> aaa() const { return vec<T, 3>(a(), a(), a()); }
	vec<T, 3> qqq() const { return vec<T, 3>(q(), q(), q()); }
	vec<T, 4> xxxx() const { return vec<T, 4>(x(), x(), x(), x()); }
	vec<T, 4> rrrr() const { return vec<T, 4>(r(), r(), r(), r()); }
	vec<T, 4> ssss() const { return vec<T, 4>(s(), s(), s(), s()); }
	vec<T, 4> xxxy() const { return vec<T, 4>(x(), x(), x(), y()); }
	vec<T, 4> rrrg() const { return vec<T, 4>(r(), r(), r(), g()); }
	vec<T, 4> ssst() const { return vec<T, 4>(s(), s(), s(), t()); }
	vec<T, 4> xxxz() const { return vec<T, 4>(x(), x(), x(), z()); }
	vec<T, 4> rrrb() const { return vec<T, 4>(r(), r(), r(), b()); }
	vec<T, 4> sssp() const { return vec<T, 4>(s(), s(), s(), p()); }
	vec<T, 4> xxxw() const { return vec<T, 4>(x(), x(), x(), w()); }
	vec<T, 4> rrra() const { return vec<T, 4>(r(), r(), r(), a()); }
	vec<T, 4> sssq() const { return vec<T, 4>(s(), s(), s(), q()); }
	vec<T, 4> xxyx() const { return vec<T, 4>(x(), x(), y(), x()); }
	vec<T, 4> rrgr() const { return vec<T, 4>(r(), r(), g(), r()); }
	vec<T, 4> ssts() const { return vec<T, 4>(s(), s(), t(), s()); }
	vec<T, 4> xxyy() const { return vec<T, 4>(x(), x(), y(), y()); }
	vec<T, 4> rrgg() const { return vec<T, 4>(r(), r(), g(), g()); }
	vec<T, 4> sstt() const { return vec<T, 4>(s(), s(), t(), t()); }
	vec<T, 4> xxyz() const { return vec<T, 4>(x(), x(), y(), z()); }
	vec<T, 4> rrgb() const { return vec<T, 4>(r(), r(), g(), b()); }
	vec<T, 4> sstp() const { return vec<T, 4>(s(), s(), t(), p()); }
	vec<T, 4> xxyw() const { return vec<T, 4>(x(), x(), y(), w()); }
	vec<T, 4> rrga() const { return vec<T, 4>(r(), r(), g(), a()); }
	vec<T, 4> sstq() const { return vec<T, 4>(s(), s(), t(), q()); }
	vec<T, 4> xxzx() const { return vec<T, 4>(x(), x(), z(), x()); }
	vec<T, 4> rrbr() const { return vec<T, 4>(r(), r(), b(), r()); }
	vec<T, 4> ssps() const { return vec<T, 4>(s(), s(), p(), s()); }
	vec<T, 4> xxzy() const { return vec<T, 4>(x(), x(), z(), y()); }
	vec<T, 4> rrbg() const { return vec<T, 4>(r(), r(), b(), g()); }
	vec<T, 4> sspt() const { return vec<T, 4>(s(), s(), p(), t()); }
	vec<T, 4> xxzz() const { return vec<T, 4>(x(), x(), z(), z()); }
	vec<T, 4> rrbb() const { return vec<T, 4>(r(), r(), b(), b()); }
	vec<T, 4> sspp() const { return vec<T, 4>(s(), s(), p(), p()); }
	vec<T, 4> xxzw() const { return vec<T, 4>(x(), x(), z(), w()); }
	vec<T, 4> rrba() const { return vec<T, 4>(r(), r(), b(), a()); }
	vec<T, 4> sspq() const { return vec<T, 4>(s(), s(), p(), q()); }
	vec<T, 4> xxwx() const { return vec<T, 4>(x(), x(), w(), x()); }
	vec<T, 4> rrar() const { return vec<T, 4>(r(), r(), a(), r()); }
	vec<T, 4> ssqs() const { return vec<T, 4>(s(), s(), q(), s()); }
	vec<T, 4> xxwy() const { return vec<T, 4>(x(), x(), w(), y()); }
	vec<T, 4> rrag() const { return vec<T, 4>(r(), r(), a(), g()); }
	vec<T, 4> ssqt() const { return vec<T, 4>(s(), s(), q(), t()); }
	vec<T, 4> xxwz() const { return vec<T, 4>(x(), x(), w(), z()); }
	vec<T, 4> rrab() const { return vec<T, 4>(r(), r(), a(), b()); }
	vec<T, 4> ssqp() const { return vec<T, 4>(s(), s(), q(), p()); }
	vec<T, 4> xxww() const { return vec<T, 4>(x(), x(), w(), w()); }
	vec<T, 4> rraa() const { return vec<T, 4>(r(), r(), a(), a()); }
	vec<T, 4> ssqq() const { return vec<T, 4>(s(), s(), q(), q()); }
	vec<T, 4> xyxx() const { return vec<T, 4>(x(), y(), x(), x()); }
	vec<T, 4> rgrr() const { return vec<T, 4>(r(), g(), r(), r()); }
	vec<T, 4> stss() const { return vec<T, 4>(s(), t(), s(), s()); }
	vec<T, 4> xyxy() const { return vec<T, 4>(x(), y(), x(), y()); }
	vec<T, 4> rgrg() const { return vec<T, 4>(r(), g(), r(), g()); }
	vec<T, 4> stst() const { return vec<T, 4>(s(), t(), s(), t()); }
	vec<T, 4> xyxz() const { return vec<T, 4>(x(), y(), x(), z()); }
	vec<T, 4> rgrb() const { return vec<T, 4>(r(), g(), r(), b()); }
	vec<T, 4> stsp() const { return vec<T, 4>(s(), t(), s(), p()); }
	vec<T, 4> xyxw() const { return vec<T, 4>(x(), y(), x(), w()); }
	vec<T, 4> rgra() const { return vec<T, 4>(r(), g(), r(), a()); }
	vec<T, 4> stsq() const { return vec<T, 4>(s(), t(), s(), q()); }
	vec<T, 4> xyyx() const { return vec<T, 4>(x(), y(), y(), x()); }
	vec<T, 4> rggr() const { return vec<T, 4>(r(), g(), g(), r()); }
	vec<T, 4> stts() const { return vec<T, 4>(s(), t(), t(), s()); }
	vec<T, 4> xyyy() const { return vec<T, 4>(x(), y(), y(), y()); }
	vec<T, 4> rggg() const { return vec<T, 4>(r(), g(), g(), g()); }
	vec<T, 4> sttt() const { return vec<T, 4>(s(), t(), t(), t()); }
	vec<T, 4> xyyz() const { return vec<T, 4>(x(), y(), y(), z()); }
	vec<T, 4> rggb() const { return vec<T, 4>(r(), g(), g(), b()); }
	vec<T, 4> sttp() const { return vec<T, 4>(s(), t(), t(), p()); }
	vec<T, 4> xyyw() const { return vec<T, 4>(x(), y(), y(), w()); }
	vec<T, 4> rgga() const { return vec<T, 4>(r(), g(), g(), a()); }
	vec<T, 4> sttq() const { return vec<T, 4>(s(), t(), t(), q()); }
	vec<T, 4> xyzx() const { return vec<T, 4>(x(), y(), z(), x()); }
	vec<T, 4> rgbr() const { return vec<T, 4>(r(), g(), b(), r()); }
	vec<T, 4> stps() const { return vec<T, 4>(s(), t(), p(), s()); }
	vec<T, 4> xyzy() const { return vec<T, 4>(x(), y(), z(), y()); }
	vec<T, 4> rgbg() const { return vec<T, 4>(r(), g(), b(), g()); }
	vec<T, 4> stpt() const { return vec<T, 4>(s(), t(), p(), t()); }
	vec<T, 4> xyzz() const { return vec<T, 4>(x(), y(), z(), z()); }
	vec<T, 4> rgbb() const { return vec<T, 4>(r(), g(), b(), b()); }
	vec<T, 4> stpp() const { return vec<T, 4>(s(), t(), p(), p()); }
	vec<T, 4> xyzw() const { return vec<T, 4>(x(), y(), z(), w()); }
	vec<T, 4> rgba() const { return vec<T, 4>(r(), g(), b(), a()); }
	vec<T, 4> stpq() const { return vec<T, 4>(s(), t(), p(), q()); }
	vec<T, 4> xywx() const { return vec<T, 4>(x(), y(), w(), x()); }
	vec<T, 4> rgar() const { return vec<T, 4>(r(), g(), a(), r()); }
	vec<T, 4> stqs() const { return vec<T, 4>(s(), t(), q(), s()); }
	vec<T, 4> xywy() const { return vec<T, 4>(x(), y(), w(), y()); }
	vec<T, 4> rgag() const { return vec<T, 4>(r(), g(), a(), g()); }
	vec<T, 4> stqt() const { return vec<T, 4>(s(), t(), q(), t()); }
	vec<T, 4> xywz() const { return vec<T, 4>(x(), y(), w(), z()); }
	vec<T, 4> rgab() const { return vec<T, 4>(r(), g(), a(), b()); }
	vec<T, 4> stqp() const { return vec<T, 4>(s(), t(), q(), p()); }
	vec<T, 4> xyww() const { return vec<T, 4>(x(), y(), w(), w()); }
	vec<T, 4> rgaa() const { return vec<T, 4>(r(), g(), a(), a()); }
	vec<T, 4> stqq() const { return vec<T, 4>(s(), t(), q(), q()); }
	vec<T, 4> xzxx() const { return vec<T, 4>(x(), z(), x(), x()); }
	vec<T, 4> rbrr() const { return vec<T, 4>(r(), b(), r(), r()); }
	vec<T, 4> spss() const { return vec<T, 4>(s(), p(), s(), s()); }
	vec<T, 4> xzxy() const { return vec<T, 4>(x(), z(), x(), y()); }
	vec<T, 4> rbrg() const { return vec<T, 4>(r(), b(), r(), g()); }
	vec<T, 4> spst() const { return vec<T, 4>(s(), p(), s(), t()); }
	vec<T, 4> xzxz() const { return vec<T, 4>(x(), z(), x(), z()); }
	vec<T, 4> rbrb() const { return vec<T, 4>(r(), b(), r(), b()); }
	vec<T, 4> spsp() const { return vec<T, 4>(s(), p(), s(), p()); }
	vec<T, 4> xzxw() const { return vec<T, 4>(x(), z(), x(), w()); }
	vec<T, 4> rbra() const { return vec<T, 4>(r(), b(), r(), a()); }
	vec<T, 4> spsq() const { return vec<T, 4>(s(), p(), s(), q()); }
	vec<T, 4> xzyx() const { return vec<T, 4>(x(), z(), y(), x()); }
	vec<T, 4> rbgr() const { return vec<T, 4>(r(), b(), g(), r()); }
	vec<T, 4> spts() const { return vec<T, 4>(s(), p(), t(), s()); }
	vec<T, 4> xzyy() const { return vec<T, 4>(x(), z(), y(), y()); }
	vec<T, 4> rbgg() const { return vec<T, 4>(r(), b(), g(), g()); }
	vec<T, 4> sptt() const { return vec<T, 4>(s(), p(), t(), t()); }
	vec<T, 4> xzyz() const { return vec<T, 4>(x(), z(), y(), z()); }
	vec<T, 4> rbgb() const { return vec<T, 4>(r(), b(), g(), b()); }
	vec<T, 4> sptp() const { return vec<T, 4>(s(), p(), t(), p()); }
	vec<T, 4> xzyw() const { return vec<T, 4>(x(), z(), y(), w()); }
	vec<T, 4> rbga() const { return vec<T, 4>(r(), b(), g(), a()); }
	vec<T, 4> sptq() const { return vec<T, 4>(s(), p(), t(), q()); }
	vec<T, 4> xzzx() const { return vec<T, 4>(x(), z(), z(), x()); }
	vec<T, 4> rbbr() const { return vec<T, 4>(r(), b(), b(), r()); }
	vec<T, 4> spps() const { return vec<T, 4>(s(), p(), p(), s()); }
	vec<T, 4> xzzy() const { return vec<T, 4>(x(), z(), z(), y()); }
	vec<T, 4> rbbg() const { return vec<T, 4>(r(), b(), b(), g()); }
	vec<T, 4> sppt() const { return vec<T, 4>(s(), p(), p(), t()); }
	vec<T, 4> xzzz() const { return vec<T, 4>(x(), z(), z(), z()); }
	vec<T, 4> rbbb() const { return vec<T, 4>(r(), b(), b(), b()); }
	vec<T, 4> sppp() const { return vec<T, 4>(s(), p(), p(), p()); }
	vec<T, 4> xzzw() const { return vec<T, 4>(x(), z(), z(), w()); }
	vec<T, 4> rbba() const { return vec<T, 4>(r(), b(), b(), a()); }
	vec<T, 4> sppq() const { return vec<T, 4>(s(), p(), p(), q()); }
	vec<T, 4> xzwx() const { return vec<T, 4>(x(), z(), w(), x()); }
	vec<T, 4> rbar() const { return vec<T, 4>(r(), b(), a(), r()); }
	vec<T, 4> spqs() const { return vec<T, 4>(s(), p(), q(), s()); }
	vec<T, 4> xzwy() const { return vec<T, 4>(x(), z(), w(), y()); }
	vec<T, 4> rbag() const { return vec<T, 4>(r(), b(), a(), g()); }
	vec<T, 4> spqt() const { return vec<T, 4>(s(), p(), q(), t()); }
	vec<T, 4> xzwz() const { return vec<T, 4>(x(), z(), w(), z()); }
	vec<T, 4> rbab() const { return vec<T, 4>(r(), b(), a(), b()); }
	vec<T, 4> spqp() const { return vec<T, 4>(s(), p(), q(), p()); }
	vec<T, 4> xzww() const { return vec<T, 4>(x(), z(), w(), w()); }
	vec<T, 4> rbaa() const { return vec<T, 4>(r(), b(), a(), a()); }
	vec<T, 4> spqq() const { return vec<T, 4>(s(), p(), q(), q()); }
	vec<T, 4> xwxx() const { return vec<T, 4>(x(), w(), x(), x()); }
	vec<T, 4> rarr() const { return vec<T, 4>(r(), a(), r(), r()); }
	vec<T, 4> sqss() const { return vec<T, 4>(s(), q(), s(), s()); }
	vec<T, 4> xwxy() const { return vec<T, 4>(x(), w(), x(), y()); }
	vec<T, 4> rarg() const { return vec<T, 4>(r(), a(), r(), g()); }
	vec<T, 4> sqst() const { return vec<T, 4>(s(), q(), s(), t()); }
	vec<T, 4> xwxz() const { return vec<T, 4>(x(), w(), x(), z()); }
	vec<T, 4> rarb() const { return vec<T, 4>(r(), a(), r(), b()); }
	vec<T, 4> sqsp() const { return vec<T, 4>(s(), q(), s(), p()); }
	vec<T, 4> xwxw() const { return vec<T, 4>(x(), w(), x(), w()); }
	vec<T, 4> rara() const { return vec<T, 4>(r(), a(), r(), a()); }
	vec<T, 4> sqsq() const { return vec<T, 4>(s(), q(), s(), q()); }
	vec<T, 4> xwyx() const { return vec<T, 4>(x(), w(), y(), x()); }
	vec<T, 4> ragr() const { return vec<T, 4>(r(), a(), g(), r()); }
	vec<T, 4> sqts() const { return vec<T, 4>(s(), q(), t(), s()); }
	vec<T, 4> xwyy() const { return vec<T, 4>(x(), w(), y(), y()); }
	vec<T, 4> ragg() const { return vec<T, 4>(r(), a(), g(), g()); }
	vec<T, 4> sqtt() const { return vec<T, 4>(s(), q(), t(), t()); }
	vec<T, 4> xwyz() const { return vec<T, 4>(x(), w(), y(), z()); }
	vec<T, 4> ragb() const { return vec<T, 4>(r(), a(), g(), b()); }
	vec<T, 4> sqtp() const { return vec<T, 4>(s(), q(), t(), p()); }
	vec<T, 4> xwyw() const { return vec<T, 4>(x(), w(), y(), w()); }
	vec<T, 4> raga() const { return vec<T, 4>(r(), a(), g(), a()); }
	vec<T, 4> sqtq() const { return vec<T, 4>(s(), q(), t(), q()); }
	vec<T, 4> xwzx() const { return vec<T, 4>(x(), w(), z(), x()); }
	vec<T, 4> rabr() const { return vec<T, 4>(r(), a(), b(), r()); }
	vec<T, 4> sqps() const { return vec<T, 4>(s(), q(), p(), s()); }
	vec<T, 4> xwzy() const { return vec<T, 4>(x(), w(), z(), y()); }
	vec<T, 4> rabg() const { return vec<T, 4>(r(), a(), b(), g()); }
	vec<T, 4> sqpt() const { return vec<T, 4>(s(), q(), p(), t()); }
	vec<T, 4> xwzz() const { return vec<T, 4>(x(), w(), z(), z()); }
	vec<T, 4> rabb() const { return vec<T, 4>(r(), a(), b(), b()); }
	vec<T, 4> sqpp() const { return vec<T, 4>(s(), q(), p(), p()); }
	vec<T, 4> xwzw() const { return vec<T, 4>(x(), w(), z(), w()); }
	vec<T, 4> raba() const { return vec<T, 4>(r(), a(), b(), a()); }
	vec<T, 4> sqpq() const { return vec<T, 4>(s(), q(), p(), q()); }
	vec<T, 4> xwwx() const { return vec<T, 4>(x(), w(), w(), x()); }
	vec<T, 4> raar() const { return vec<T, 4>(r(), a(), a(), r()); }
	vec<T, 4> sqqs() const { return vec<T, 4>(s(), q(), q(), s()); }
	vec<T, 4> xwwy() const { return vec<T, 4>(x(), w(), w(), y()); }
	vec<T, 4> raag() const { return vec<T, 4>(r(), a(), a(), g()); }
	vec<T, 4> sqqt() const { return vec<T, 4>(s(), q(), q(), t()); }
	vec<T, 4> xwwz() const { return vec<T, 4>(x(), w(), w(), z()); }
	vec<T, 4> raab() const { return vec<T, 4>(r(), a(), a(), b()); }
	vec<T, 4> sqqp() const { return vec<T, 4>(s(), q(), q(), p()); }
	vec<T, 4> xwww() const { return vec<T, 4>(x(), w(), w(), w()); }
	vec<T, 4> raaa() const { return vec<T, 4>(r(), a(), a(), a()); }
	vec<T, 4> sqqq() const { return vec<T, 4>(s(), q(), q(), q()); }
	vec<T, 4> yxxx() const { return vec<T, 4>(y(), x(), x(), x()); }
	vec<T, 4> grrr() const { return vec<T, 4>(g(), r(), r(), r()); }
	vec<T, 4> tsss() const { return vec<T, 4>(t(), s(), s(), s()); }
	vec<T, 4> yxxy() const { return vec<T, 4>(y(), x(), x(), y()); }
	vec<T, 4> grrg() const { return vec<T, 4>(g(), r(), r(), g()); }
	vec<T, 4> tsst() const { return vec<T, 4>(t(), s(), s(), t()); }
	vec<T, 4> yxxz() const { return vec<T, 4>(y(), x(), x(), z()); }
	vec<T, 4> grrb() const { return vec<T, 4>(g(), r(), r(), b()); }
	vec<T, 4> tssp() const { return vec<T, 4>(t(), s(), s(), p()); }
	vec<T, 4> yxxw() const { return vec<T, 4>(y(), x(), x(), w()); }
	vec<T, 4> grra() const { return vec<T, 4>(g(), r(), r(), a()); }
	vec<T, 4> tssq() const { return vec<T, 4>(t(), s(), s(), q()); }
	vec<T, 4> yxyx() const { return vec<T, 4>(y(), x(), y(), x()); }
	vec<T, 4> grgr() const { return vec<T, 4>(g(), r(), g(), r()); }
	vec<T, 4> tsts() const { return vec<T, 4>(t(), s(), t(), s()); }
	vec<T, 4> yxyy() const { return vec<T, 4>(y(), x(), y(), y()); }
	vec<T, 4> grgg() const { return vec<T, 4>(g(), r(), g(), g()); }
	vec<T, 4> tstt() const { return vec<T, 4>(t(), s(), t(), t()); }
	vec<T, 4> yxyz() const { return vec<T, 4>(y(), x(), y(), z()); }
	vec<T, 4> grgb() const { return vec<T, 4>(g(), r(), g(), b()); }
	vec<T, 4> tstp() const { return vec<T, 4>(t(), s(), t(), p()); }
	vec<T, 4> yxyw() const { return vec<T, 4>(y(), x(), y(), w()); }
	vec<T, 4> grga() const { return vec<T, 4>(g(), r(), g(), a()); }
	vec<T, 4> tstq() const { return vec<T, 4>(t(), s(), t(), q()); }
	vec<T, 4> yxzx() const { return vec<T, 4>(y(), x(), z(), x()); }
	vec<T, 4> grbr() const { return vec<T, 4>(g(), r(), b(), r()); }
	vec<T, 4> tsps() const { return vec<T, 4>(t(), s(), p(), s()); }
	vec<T, 4> yxzy() const { return vec<T, 4>(y(), x(), z(), y()); }
	vec<T, 4> grbg() const { return vec<T, 4>(g(), r(), b(), g()); }
	vec<T, 4> tspt() const { return vec<T, 4>(t(), s(), p(), t()); }
	vec<T, 4> yxzz() const { return vec<T, 4>(y(), x(), z(), z()); }
	vec<T, 4> grbb() const { return vec<T, 4>(g(), r(), b(), b()); }
	vec<T, 4> tspp() const { return vec<T, 4>(t(), s(), p(), p()); }
	vec<T, 4> yxzw() const { return vec<T, 4>(y(), x(), z(), w()); }
	vec<T, 4> grba() const { return vec<T, 4>(g(), r(), b(), a()); }
	vec<T, 4> tspq() const { return vec<T, 4>(t(), s(), p(), q()); }
	vec<T, 4> yxwx() const { return vec<T, 4>(y(), x(), w(), x()); }
	vec<T, 4> grar() const { return vec<T, 4>(g(), r(), a(), r()); }
	vec<T, 4> tsqs() const { return vec<T, 4>(t(), s(), q(), s()); }
	vec<T, 4> yxwy() const { return vec<T, 4>(y(), x(), w(), y()); }
	vec<T, 4> grag() const { return vec<T, 4>(g(), r(), a(), g()); }
	vec<T, 4> tsqt() const { return vec<T, 4>(t(), s(), q(), t()); }
	vec<T, 4> yxwz() const { return vec<T, 4>(y(), x(), w(), z()); }
	vec<T, 4> grab() const { return vec<T, 4>(g(), r(), a(), b()); }
	vec<T, 4> tsqp() const { return vec<T, 4>(t(), s(), q(), p()); }
	vec<T, 4> yxww() const { return vec<T, 4>(y(), x(), w(), w()); }
	vec<T, 4> graa() const { return vec<T, 4>(g(), r(), a(), a()); }
	vec<T, 4> tsqq() const { return vec<T, 4>(t(), s(), q(), q()); }
	vec<T, 4> yyxx() const { return vec<T, 4>(y(), y(), x(), x()); }
	vec<T, 4> ggrr() const { return vec<T, 4>(g(), g(), r(), r()); }
	vec<T, 4> ttss() const { return vec<T, 4>(t(), t(), s(), s()); }
	vec<T, 4> yyxy() const { return vec<T, 4>(y(), y(), x(), y()); }
	vec<T, 4> ggrg() const { return vec<T, 4>(g(), g(), r(), g()); }
	vec<T, 4> ttst() const { return vec<T, 4>(t(), t(), s(), t()); }
	vec<T, 4> yyxz() const { return vec<T, 4>(y(), y(), x(), z()); }
	vec<T, 4> ggrb() const { return vec<T, 4>(g(), g(), r(), b()); }
	vec<T, 4> ttsp() const { return vec<T, 4>(t(), t(), s(), p()); }
	vec<T, 4> yyxw() const { return vec<T, 4>(y(), y(), x(), w()); }
	vec<T, 4> ggra() const { return vec<T, 4>(g(), g(), r(), a()); }
	vec<T, 4> ttsq() const { return vec<T, 4>(t(), t(), s(), q()); }
	vec<T, 4> yyyx() const { return vec<T, 4>(y(), y(), y(), x()); }
	vec<T, 4> gggr() const { return vec<T, 4>(g(), g(), g(), r()); }
	vec<T, 4> ttts() const { return vec<T, 4>(t(), t(), t(), s()); }
	vec<T, 4> yyyy() const { return vec<T, 4>(y(), y(), y(), y()); }
	vec<T, 4> gggg() const { return vec<T, 4>(g(), g(), g(), g()); }
	vec<T, 4> tttt() const { return vec<T, 4>(t(), t(), t(), t()); }
	vec<T, 4> yyyz() const { return vec<T, 4>(y(), y(), y(), z()); }
	vec<T, 4> gggb() const { return vec<T, 4>(g(), g(), g(), b()); }
	vec<T, 4> tttp() const { return vec<T, 4>(t(), t(), t(), p()); }
	vec<T, 4> yyyw() const { return vec<T, 4>(y(), y(), y(), w()); }
	vec<T, 4> ggga() const { return vec<T, 4>(g(), g(), g(), a()); }
	vec<T, 4> tttq() const { return vec<T, 4>(t(), t(), t(), q()); }
	vec<T, 4> yyzx() const { return vec<T, 4>(y(), y(), z(), x()); }
	vec<T, 4> ggbr() const { return vec<T, 4>(g(), g(), b(), r()); }
	vec<T, 4> ttps() const { return vec<T, 4>(t(), t(), p(), s()); }
	vec<T, 4> yyzy() const { return vec<T, 4>(y(), y(), z(), y()); }
	vec<T, 4> ggbg() const { return vec<T, 4>(g(), g(), b(), g()); }
	vec<T, 4> ttpt() const { return vec<T, 4>(t(), t(), p(), t()); }
	vec<T, 4> yyzz() const { return vec<T, 4>(y(), y(), z(), z()); }
	vec<T, 4> ggbb() const { return vec<T, 4>(g(), g(), b(), b()); }
	vec<T, 4> ttpp() const { return vec<T, 4>(t(), t(), p(), p()); }
	vec<T, 4> yyzw() const { return vec<T, 4>(y(), y(), z(), w()); }
	vec<T, 4> ggba() const { return vec<T, 4>(g(), g(), b(), a()); }
	vec<T, 4> ttpq() const { return vec<T, 4>(t(), t(), p(), q()); }
	vec<T, 4> yywx() const { return vec<T, 4>(y(), y(), w(), x()); }
	vec<T, 4> ggar() const { return vec<T, 4>(g(), g(), a(), r()); }
	vec<T, 4> ttqs() const { return vec<T, 4>(t(), t(), q(), s()); }
	vec<T, 4> yywy() const { return vec<T, 4>(y(), y(), w(), y()); }
	vec<T, 4> ggag() const { return vec<T, 4>(g(), g(), a(), g()); }
	vec<T, 4> ttqt() const { return vec<T, 4>(t(), t(), q(), t()); }
	vec<T, 4> yywz() const { return vec<T, 4>(y(), y(), w(), z()); }
	vec<T, 4> ggab() const { return vec<T, 4>(g(), g(), a(), b()); }
	vec<T, 4> ttqp() const { return vec<T, 4>(t(), t(), q(), p()); }
	vec<T, 4> yyww() const { return vec<T, 4>(y(), y(), w(), w()); }
	vec<T, 4> ggaa() const { return vec<T, 4>(g(), g(), a(), a()); }
	vec<T, 4> ttqq() const { return vec<T, 4>(t(), t(), q(), q()); }
	vec<T, 4> yzxx() const { return vec<T, 4>(y(), z(), x(), x()); }
	vec<T, 4> gbrr() const { return vec<T, 4>(g(), b(), r(), r()); }
	vec<T, 4> tpss() const { return vec<T, 4>(t(), p(), s(), s()); }
	vec<T, 4> yzxy() const { return vec<T, 4>(y(), z(), x(), y()); }
	vec<T, 4> gbrg() const { return vec<T, 4>(g(), b(), r(), g()); }
	vec<T, 4> tpst() const { return vec<T, 4>(t(), p(), s(), t()); }
	vec<T, 4> yzxz() const { return vec<T, 4>(y(), z(), x(), z()); }
	vec<T, 4> gbrb() const { return vec<T, 4>(g(), b(), r(), b()); }
	vec<T, 4> tpsp() const { return vec<T, 4>(t(), p(), s(), p()); }
	vec<T, 4> yzxw() const { return vec<T, 4>(y(), z(), x(), w()); }
	vec<T, 4> gbra() const { return vec<T, 4>(g(), b(), r(), a()); }
	vec<T, 4> tpsq() const { return vec<T, 4>(t(), p(), s(), q()); }
	vec<T, 4> yzyx() const { return vec<T, 4>(y(), z(), y(), x()); }
	vec<T, 4> gbgr() const { return vec<T, 4>(g(), b(), g(), r()); }
	vec<T, 4> tpts() const { return vec<T, 4>(t(), p(), t(), s()); }
	vec<T, 4> yzyy() const { return vec<T, 4>(y(), z(), y(), y()); }
	vec<T, 4> gbgg() const { return vec<T, 4>(g(), b(), g(), g()); }
	vec<T, 4> tptt() const { return vec<T, 4>(t(), p(), t(), t()); }
	vec<T, 4> yzyz() const { return vec<T, 4>(y(), z(), y(), z()); }
	vec<T, 4> gbgb() const { return vec<T, 4>(g(), b(), g(), b()); }
	vec<T, 4> tptp() const { return vec<T, 4>(t(), p(), t(), p()); }
	vec<T, 4> yzyw() const { return vec<T, 4>(y(), z(), y(), w()); }
	vec<T, 4> gbga() const { return vec<T, 4>(g(), b(), g(), a()); }
	vec<T, 4> tptq() const { return vec<T, 4>(t(), p(), t(), q()); }
	vec<T, 4> yzzx() const { return vec<T, 4>(y(), z(), z(), x()); }
	vec<T, 4> gbbr() const { return vec<T, 4>(g(), b(), b(), r()); }
	vec<T, 4> tpps() const { return vec<T, 4>(t(), p(), p(), s()); }
	vec<T, 4> yzzy() const { return vec<T, 4>(y(), z(), z(), y()); }
	vec<T, 4> gbbg() const { return vec<T, 4>(g(), b(), b(), g()); }
	vec<T, 4> tppt() const { return vec<T, 4>(t(), p(), p(), t()); }
	vec<T, 4> yzzz() const { return vec<T, 4>(y(), z(), z(), z()); }
	vec<T, 4> gbbb() const { return vec<T, 4>(g(), b(), b(), b()); }
	vec<T, 4> tppp() const { return vec<T, 4>(t(), p(), p(), p()); }
	vec<T, 4> yzzw() const { return vec<T, 4>(y(), z(), z(), w()); }
	vec<T, 4> gbba() const { return vec<T, 4>(g(), b(), b(), a()); }
	vec<T, 4> tppq() const { return vec<T, 4>(t(), p(), p(), q()); }
	vec<T, 4> yzwx() const { return vec<T, 4>(y(), z(), w(), x()); }
	vec<T, 4> gbar() const { return vec<T, 4>(g(), b(), a(), r()); }
	vec<T, 4> tpqs() const { return vec<T, 4>(t(), p(), q(), s()); }
	vec<T, 4> yzwy() const { return vec<T, 4>(y(), z(), w(), y()); }
	vec<T, 4> gbag() const { return vec<T, 4>(g(), b(), a(), g()); }
	vec<T, 4> tpqt() const { return vec<T, 4>(t(), p(), q(), t()); }
	vec<T, 4> yzwz() const { return vec<T, 4>(y(), z(), w(), z()); }
	vec<T, 4> gbab() const { return vec<T, 4>(g(), b(), a(), b()); }
	vec<T, 4> tpqp() const { return vec<T, 4>(t(), p(), q(), p()); }
	vec<T, 4> yzww() const { return vec<T, 4>(y(), z(), w(), w()); }
	vec<T, 4> gbaa() const { return vec<T, 4>(g(), b(), a(), a()); }
	vec<T, 4> tpqq() const { return vec<T, 4>(t(), p(), q(), q()); }
	vec<T, 4> ywxx() const { return vec<T, 4>(y(), w(), x(), x()); }
	vec<T, 4> garr() const { return vec<T, 4>(g(), a(), r(), r()); }
	vec<T, 4> tqss() const { return vec<T, 4>(t(), q(), s(), s()); }
	vec<T, 4> ywxy() const { return vec<T, 4>(y(), w(), x(), y()); }
	vec<T, 4> garg() const { return vec<T, 4>(g(), a(), r(), g()); }
	vec<T, 4> tqst() const { return vec<T, 4>(t(), q(), s(), t()); }
	vec<T, 4> ywxz() const { return vec<T, 4>(y(), w(), x(), z()); }
	vec<T, 4> garb() const { return vec<T, 4>(g(), a(), r(), b()); }
	vec<T, 4> tqsp() const { return vec<T, 4>(t(), q(), s(), p()); }
	vec<T, 4> ywxw() const { return vec<T, 4>(y(), w(), x(), w()); }
	vec<T, 4> gara() const { return vec<T, 4>(g(), a(), r(), a()); }
	vec<T, 4> tqsq() const { return vec<T, 4>(t(), q(), s(), q()); }
	vec<T, 4> ywyx() const { return vec<T, 4>(y(), w(), y(), x()); }
	vec<T, 4> gagr() const { return vec<T, 4>(g(), a(), g(), r()); }
	vec<T, 4> tqts() const { return vec<T, 4>(t(), q(), t(), s()); }
	vec<T, 4> ywyy() const { return vec<T, 4>(y(), w(), y(), y()); }
	vec<T, 4> gagg() const { return vec<T, 4>(g(), a(), g(), g()); }
	vec<T, 4> tqtt() const { return vec<T, 4>(t(), q(), t(), t()); }
	vec<T, 4> ywyz() const { return vec<T, 4>(y(), w(), y(), z()); }
	vec<T, 4> gagb() const { return vec<T, 4>(g(), a(), g(), b()); }
	vec<T, 4> tqtp() const { return vec<T, 4>(t(), q(), t(), p()); }
	vec<T, 4> ywyw() const { return vec<T, 4>(y(), w(), y(), w()); }
	vec<T, 4> gaga() const { return vec<T, 4>(g(), a(), g(), a()); }
	vec<T, 4> tqtq() const { return vec<T, 4>(t(), q(), t(), q()); }
	vec<T, 4> ywzx() const { return vec<T, 4>(y(), w(), z(), x()); }
	vec<T, 4> gabr() const { return vec<T, 4>(g(), a(), b(), r()); }
	vec<T, 4> tqps() const { return vec<T, 4>(t(), q(), p(), s()); }
	vec<T, 4> ywzy() const { return vec<T, 4>(y(), w(), z(), y()); }
	vec<T, 4> gabg() const { return vec<T, 4>(g(), a(), b(), g()); }
	vec<T, 4> tqpt() const { return vec<T, 4>(t(), q(), p(), t()); }
	vec<T, 4> ywzz() const { return vec<T, 4>(y(), w(), z(), z()); }
	vec<T, 4> gabb() const { return vec<T, 4>(g(), a(), b(), b()); }
	vec<T, 4> tqpp() const { return vec<T, 4>(t(), q(), p(), p()); }
	vec<T, 4> ywzw() const { return vec<T, 4>(y(), w(), z(), w()); }
	vec<T, 4> gaba() const { return vec<T, 4>(g(), a(), b(), a()); }
	vec<T, 4> tqpq() const { return vec<T, 4>(t(), q(), p(), q()); }
	vec<T, 4> ywwx() const { return vec<T, 4>(y(), w(), w(), x()); }
	vec<T, 4> gaar() const { return vec<T, 4>(g(), a(), a(), r()); }
	vec<T, 4> tqqs() const { return vec<T, 4>(t(), q(), q(), s()); }
	vec<T, 4> ywwy() const { return vec<T, 4>(y(), w(), w(), y()); }
	vec<T, 4> gaag() const { return vec<T, 4>(g(), a(), a(), g()); }
	vec<T, 4> tqqt() const { return vec<T, 4>(t(), q(), q(), t()); }
	vec<T, 4> ywwz() const { return vec<T, 4>(y(), w(), w(), z()); }
	vec<T, 4> gaab() const { return vec<T, 4>(g(), a(), a(), b()); }
	vec<T, 4> tqqp() const { return vec<T, 4>(t(), q(), q(), p()); }
	vec<T, 4> ywww() const { return vec<T, 4>(y(), w(), w(), w()); }
	vec<T, 4> gaaa() const { return vec<T, 4>(g(), a(), a(), a()); }
	vec<T, 4> tqqq() const { return vec<T, 4>(t(), q(), q(), q()); }
	vec<T, 4> zxxx() const { return vec<T, 4>(z(), x(), x(), x()); }
	vec<T, 4> brrr() const { return vec<T, 4>(b(), r(), r(), r()); }
	vec<T, 4> psss() const { return vec<T, 4>(p(), s(), s(), s()); }
	vec<T, 4> zxxy() const { return vec<T, 4>(z(), x(), x(), y()); }
	vec<T, 4> brrg() const { return vec<T, 4>(b(), r(), r(), g()); }
	vec<T, 4> psst() const { return vec<T, 4>(p(), s(), s(), t()); }
	vec<T, 4> zxxz() const { return vec<T, 4>(z(), x(), x(), z()); }
	vec<T, 4> brrb() const { return vec<T, 4>(b(), r(), r(), b()); }
	vec<T, 4> pssp() const { return vec<T, 4>(p(), s(), s(), p()); }
	vec<T, 4> zxxw() const { return vec<T, 4>(z(), x(), x(), w()); }
	vec<T, 4> brra() const { return vec<T, 4>(b(), r(), r(), a()); }
	vec<T, 4> pssq() const { return vec<T, 4>(p(), s(), s(), q()); }
	vec<T, 4> zxyx() const { return vec<T, 4>(z(), x(), y(), x()); }
	vec<T, 4> brgr() const { return vec<T, 4>(b(), r(), g(), r()); }
	vec<T, 4> psts() const { return vec<T, 4>(p(), s(), t(), s()); }
	vec<T, 4> zxyy() const { return vec<T, 4>(z(), x(), y(), y()); }
	vec<T, 4> brgg() const { return vec<T, 4>(b(), r(), g(), g()); }
	vec<T, 4> pstt() const { return vec<T, 4>(p(), s(), t(), t()); }
	vec<T, 4> zxyz() const { return vec<T, 4>(z(), x(), y(), z()); }
	vec<T, 4> brgb() const { return vec<T, 4>(b(), r(), g(), b()); }
	vec<T, 4> pstp() const { return vec<T, 4>(p(), s(), t(), p()); }
	vec<T, 4> zxyw() const { return vec<T, 4>(z(), x(), y(), w()); }
	vec<T, 4> brga() const { return vec<T, 4>(b(), r(), g(), a()); }
	vec<T, 4> pstq() const { return vec<T, 4>(p(), s(), t(), q()); }
	vec<T, 4> zxzx() const { return vec<T, 4>(z(), x(), z(), x()); }
	vec<T, 4> brbr() const { return vec<T, 4>(b(), r(), b(), r()); }
	vec<T, 4> psps() const { return vec<T, 4>(p(), s(), p(), s()); }
	vec<T, 4> zxzy() const { return vec<T, 4>(z(), x(), z(), y()); }
	vec<T, 4> brbg() const { return vec<T, 4>(b(), r(), b(), g()); }
	vec<T, 4> pspt() const { return vec<T, 4>(p(), s(), p(), t()); }
	vec<T, 4> zxzz() const { return vec<T, 4>(z(), x(), z(), z()); }
	vec<T, 4> brbb() const { return vec<T, 4>(b(), r(), b(), b()); }
	vec<T, 4> pspp() const { return vec<T, 4>(p(), s(), p(), p()); }
	vec<T, 4> zxzw() const { return vec<T, 4>(z(), x(), z(), w()); }
	vec<T, 4> brba() const { return vec<T, 4>(b(), r(), b(), a()); }
	vec<T, 4> pspq() const { return vec<T, 4>(p(), s(), p(), q()); }
	vec<T, 4> zxwx() const { return vec<T, 4>(z(), x(), w(), x()); }
	vec<T, 4> brar() const { return vec<T, 4>(b(), r(), a(), r()); }
	vec<T, 4> psqs() const { return vec<T, 4>(p(), s(), q(), s()); }
	vec<T, 4> zxwy() const { return vec<T, 4>(z(), x(), w(), y()); }
	vec<T, 4> brag() const { return vec<T, 4>(b(), r(), a(), g()); }
	vec<T, 4> psqt() const { return vec<T, 4>(p(), s(), q(), t()); }
	vec<T, 4> zxwz() const { return vec<T, 4>(z(), x(), w(), z()); }
	vec<T, 4> brab() const { return vec<T, 4>(b(), r(), a(), b()); }
	vec<T, 4> psqp() const { return vec<T, 4>(p(), s(), q(), p()); }
	vec<T, 4> zxww() const { return vec<T, 4>(z(), x(), w(), w()); }
	vec<T, 4> braa() const { return vec<T, 4>(b(), r(), a(), a()); }
	vec<T, 4> psqq() const { return vec<T, 4>(p(), s(), q(), q()); }
	vec<T, 4> zyxx() const { return vec<T, 4>(z(), y(), x(), x()); }
	vec<T, 4> bgrr() const { return vec<T, 4>(b(), g(), r(), r()); }
	vec<T, 4> ptss() const { return vec<T, 4>(p(), t(), s(), s()); }
	vec<T, 4> zyxy() const { return vec<T, 4>(z(), y(), x(), y()); }
	vec<T, 4> bgrg() const { return vec<T, 4>(b(), g(), r(), g()); }
	vec<T, 4> ptst() const { return vec<T, 4>(p(), t(), s(), t()); }
	vec<T, 4> zyxz() const { return vec<T, 4>(z(), y(), x(), z()); }
	vec<T, 4> bgrb() const { return vec<T, 4>(b(), g(), r(), b()); }
	vec<T, 4> ptsp() const { return vec<T, 4>(p(), t(), s(), p()); }
	vec<T, 4> zyxw() const { return vec<T, 4>(z(), y(), x(), w()); }
	vec<T, 4> bgra() const { return vec<T, 4>(b(), g(), r(), a()); }
	vec<T, 4> ptsq() const { return vec<T, 4>(p(), t(), s(), q()); }
	vec<T, 4> zyyx() const { return vec<T, 4>(z(), y(), y(), x()); }
	vec<T, 4> bggr() const { return vec<T, 4>(b(), g(), g(), r()); }
	vec<T, 4> ptts() const { return vec<T, 4>(p(), t(), t(), s()); }
	vec<T, 4> zyyy() const { return vec<T, 4>(z(), y(), y(), y()); }
	vec<T, 4> bggg() const { return vec<T, 4>(b(), g(), g(), g()); }
	vec<T, 4> pttt() const { return vec<T, 4>(p(), t(), t(), t()); }
	vec<T, 4> zyyz() const { return vec<T, 4>(z(), y(), y(), z()); }
	vec<T, 4> bggb() const { return vec<T, 4>(b(), g(), g(), b()); }
	vec<T, 4> pttp() const { return vec<T, 4>(p(), t(), t(), p()); }
	vec<T, 4> zyyw() const { return vec<T, 4>(z(), y(), y(), w()); }
	vec<T, 4> bgga() const { return vec<T, 4>(b(), g(), g(), a()); }
	vec<T, 4> pttq() const { return vec<T, 4>(p(), t(), t(), q()); }
	vec<T, 4> zyzx() const { return vec<T, 4>(z(), y(), z(), x()); }
	vec<T, 4> bgbr() const { return vec<T, 4>(b(), g(), b(), r()); }
	vec<T, 4> ptps() const { return vec<T, 4>(p(), t(), p(), s()); }
	vec<T, 4> zyzy() const { return vec<T, 4>(z(), y(), z(), y()); }
	vec<T, 4> bgbg() const { return vec<T, 4>(b(), g(), b(), g()); }
	vec<T, 4> ptpt() const { return vec<T, 4>(p(), t(), p(), t()); }
	vec<T, 4> zyzz() const { return vec<T, 4>(z(), y(), z(), z()); }
	vec<T, 4> bgbb() const { return vec<T, 4>(b(), g(), b(), b()); }
	vec<T, 4> ptpp() const { return vec<T, 4>(p(), t(), p(), p()); }
	vec<T, 4> zyzw() const { return vec<T, 4>(z(), y(), z(), w()); }
	vec<T, 4> bgba() const { return vec<T, 4>(b(), g(), b(), a()); }
	vec<T, 4> ptpq() const { return vec<T, 4>(p(), t(), p(), q()); }
	vec<T, 4> zywx() const { return vec<T, 4>(z(), y(), w(), x()); }
	vec<T, 4> bgar() const { return vec<T, 4>(b(), g(), a(), r()); }
	vec<T, 4> ptqs() const { return vec<T, 4>(p(), t(), q(), s()); }
	vec<T, 4> zywy() const { return vec<T, 4>(z(), y(), w(), y()); }
	vec<T, 4> bgag() const { return vec<T, 4>(b(), g(), a(), g()); }
	vec<T, 4> ptqt() const { return vec<T, 4>(p(), t(), q(), t()); }
	vec<T, 4> zywz() const { return vec<T, 4>(z(), y(), w(), z()); }
	vec<T, 4> bgab() const { return vec<T, 4>(b(), g(), a(), b()); }
	vec<T, 4> ptqp() const { return vec<T, 4>(p(), t(), q(), p()); }
	vec<T, 4> zyww() const { return vec<T, 4>(z(), y(), w(), w()); }
	vec<T, 4> bgaa() const { return vec<T, 4>(b(), g(), a(), a()); }
	vec<T, 4> ptqq() const { return vec<T, 4>(p(), t(), q(), q()); }
	vec<T, 4> zzxx() const { return vec<T, 4>(z(), z(), x(), x()); }
	vec<T, 4> bbrr() const { return vec<T, 4>(b(), b(), r(), r()); }
	vec<T, 4> ppss() const { return vec<T, 4>(p(), p(), s(), s()); }
	vec<T, 4> zzxy() const { return vec<T, 4>(z(), z(), x(), y()); }
	vec<T, 4> bbrg() const { return vec<T, 4>(b(), b(), r(), g()); }
	vec<T, 4> ppst() const { return vec<T, 4>(p(), p(), s(), t()); }
	vec<T, 4> zzxz() const { return vec<T, 4>(z(), z(), x(), z()); }
	vec<T, 4> bbrb() const { return vec<T, 4>(b(), b(), r(), b()); }
	vec<T, 4> ppsp() const { return vec<T, 4>(p(), p(), s(), p()); }
	vec<T, 4> zzxw() const { return vec<T, 4>(z(), z(), x(), w()); }
	vec<T, 4> bbra() const { return vec<T, 4>(b(), b(), r(), a()); }
	vec<T, 4> ppsq() const { return vec<T, 4>(p(), p(), s(), q()); }
	vec<T, 4> zzyx() const { return vec<T, 4>(z(), z(), y(), x()); }
	vec<T, 4> bbgr() const { return vec<T, 4>(b(), b(), g(), r()); }
	vec<T, 4> ppts() const { return vec<T, 4>(p(), p(), t(), s()); }
	vec<T, 4> zzyy() const { return vec<T, 4>(z(), z(), y(), y()); }
	vec<T, 4> bbgg() const { return vec<T, 4>(b(), b(), g(), g()); }
	vec<T, 4> pptt() const { return vec<T, 4>(p(), p(), t(), t()); }
	vec<T, 4> zzyz() const { return vec<T, 4>(z(), z(), y(), z()); }
	vec<T, 4> bbgb() const { return vec<T, 4>(b(), b(), g(), b()); }
	vec<T, 4> pptp() const { return vec<T, 4>(p(), p(), t(), p()); }
	vec<T, 4> zzyw() const { return vec<T, 4>(z(), z(), y(), w()); }
	vec<T, 4> bbga() const { return vec<T, 4>(b(), b(), g(), a()); }
	vec<T, 4> pptq() const { return vec<T, 4>(p(), p(), t(), q()); }
	vec<T, 4> zzzx() const { return vec<T, 4>(z(), z(), z(), x()); }
	vec<T, 4> bbbr() const { return vec<T, 4>(b(), b(), b(), r()); }
	vec<T, 4> ppps() const { return vec<T, 4>(p(), p(), p(), s()); }
	vec<T, 4> zzzy() const { return vec<T, 4>(z(), z(), z(), y()); }
	vec<T, 4> bbbg() const { return vec<T, 4>(b(), b(), b(), g()); }
	vec<T, 4> pppt() const { return vec<T, 4>(p(), p(), p(), t()); }
	vec<T, 4> zzzz() const { return vec<T, 4>(z(), z(), z(), z()); }
	vec<T, 4> bbbb() const { return vec<T, 4>(b(), b(), b(), b()); }
	vec<T, 4> pppp() const { return vec<T, 4>(p(), p(), p(), p()); }
	vec<T, 4> zzzw() const { return vec<T, 4>(z(), z(), z(), w()); }
	vec<T, 4> bbba() const { return vec<T, 4>(b(), b(), b(), a()); }
	vec<T, 4> pppq() const { return vec<T, 4>(p(), p(), p(), q()); }
	vec<T, 4> zzwx() const { return vec<T, 4>(z(), z(), w(), x()); }
	vec<T, 4> bbar() const { return vec<T, 4>(b(), b(), a(), r()); }
	vec<T, 4> ppqs() const { return vec<T, 4>(p(), p(), q(), s()); }
	vec<T, 4> zzwy() const { return vec<T, 4>(z(), z(), w(), y()); }
	vec<T, 4> bbag() const { return vec<T, 4>(b(), b(), a(), g()); }
	vec<T, 4> ppqt() const { return vec<T, 4>(p(), p(), q(), t()); }
	vec<T, 4> zzwz() const { return vec<T, 4>(z(), z(), w(), z()); }
	vec<T, 4> bbab() const { return vec<T, 4>(b(), b(), a(), b()); }
	vec<T, 4> ppqp() const { return vec<T, 4>(p(), p(), q(), p()); }
	vec<T, 4> zzww() const { return vec<T, 4>(z(), z(), w(), w()); }
	vec<T, 4> bbaa() const { return vec<T, 4>(b(), b(), a(), a()); }
	vec<T, 4> ppqq() const { return vec<T, 4>(p(), p(), q(), q()); }
	vec<T, 4> zwxx() const { return vec<T, 4>(z(), w(), x(), x()); }
	vec<T, 4> barr() const { return vec<T, 4>(b(), a(), r(), r()); }
	vec<T, 4> pqss() const { return vec<T, 4>(p(), q(), s(), s()); }
	vec<T, 4> zwxy() const { return vec<T, 4>(z(), w(), x(), y()); }
	vec<T, 4> barg() const { return vec<T, 4>(b(), a(), r(), g()); }
	vec<T, 4> pqst() const { return vec<T, 4>(p(), q(), s(), t()); }
	vec<T, 4> zwxz() const { return vec<T, 4>(z(), w(), x(), z()); }
	vec<T, 4> barb() const { return vec<T, 4>(b(), a(), r(), b()); }
	vec<T, 4> pqsp() const { return vec<T, 4>(p(), q(), s(), p()); }
	vec<T, 4> zwxw() const { return vec<T, 4>(z(), w(), x(), w()); }
	vec<T, 4> bara() const { return vec<T, 4>(b(), a(), r(), a()); }
	vec<T, 4> pqsq() const { return vec<T, 4>(p(), q(), s(), q()); }
	vec<T, 4> zwyx() const { return vec<T, 4>(z(), w(), y(), x()); }
	vec<T, 4> bagr() const { return vec<T, 4>(b(), a(), g(), r()); }
	vec<T, 4> pqts() const { return vec<T, 4>(p(), q(), t(), s()); }
	vec<T, 4> zwyy() const { return vec<T, 4>(z(), w(), y(), y()); }
	vec<T, 4> bagg() const { return vec<T, 4>(b(), a(), g(), g()); }
	vec<T, 4> pqtt() const { return vec<T, 4>(p(), q(), t(), t()); }
	vec<T, 4> zwyz() const { return vec<T, 4>(z(), w(), y(), z()); }
	vec<T, 4> bagb() const { return vec<T, 4>(b(), a(), g(), b()); }
	vec<T, 4> pqtp() const { return vec<T, 4>(p(), q(), t(), p()); }
	vec<T, 4> zwyw() const { return vec<T, 4>(z(), w(), y(), w()); }
	vec<T, 4> baga() const { return vec<T, 4>(b(), a(), g(), a()); }
	vec<T, 4> pqtq() const { return vec<T, 4>(p(), q(), t(), q()); }
	vec<T, 4> zwzx() const { return vec<T, 4>(z(), w(), z(), x()); }
	vec<T, 4> babr() const { return vec<T, 4>(b(), a(), b(), r()); }
	vec<T, 4> pqps() const { return vec<T, 4>(p(), q(), p(), s()); }
	vec<T, 4> zwzy() const { return vec<T, 4>(z(), w(), z(), y()); }
	vec<T, 4> babg() const { return vec<T, 4>(b(), a(), b(), g()); }
	vec<T, 4> pqpt() const { return vec<T, 4>(p(), q(), p(), t()); }
	vec<T, 4> zwzz() const { return vec<T, 4>(z(), w(), z(), z()); }
	vec<T, 4> babb() const { return vec<T, 4>(b(), a(), b(), b()); }
	vec<T, 4> pqpp() const { return vec<T, 4>(p(), q(), p(), p()); }
	vec<T, 4> zwzw() const { return vec<T, 4>(z(), w(), z(), w()); }
	vec<T, 4> baba() const { return vec<T, 4>(b(), a(), b(), a()); }
	vec<T, 4> pqpq() const { return vec<T, 4>(p(), q(), p(), q()); }
	vec<T, 4> zwwx() const { return vec<T, 4>(z(), w(), w(), x()); }
	vec<T, 4> baar() const { return vec<T, 4>(b(), a(), a(), r()); }
	vec<T, 4> pqqs() const { return vec<T, 4>(p(), q(), q(), s()); }
	vec<T, 4> zwwy() const { return vec<T, 4>(z(), w(), w(), y()); }
	vec<T, 4> baag() const { return vec<T, 4>(b(), a(), a(), g()); }
	vec<T, 4> pqqt() const { return vec<T, 4>(p(), q(), q(), t()); }
	vec<T, 4> zwwz() const { return vec<T, 4>(z(), w(), w(), z()); }
	vec<T, 4> baab() const { return vec<T, 4>(b(), a(), a(), b()); }
	vec<T, 4> pqqp() const { return vec<T, 4>(p(), q(), q(), p()); }
	vec<T, 4> zwww() const { return vec<T, 4>(z(), w(), w(), w()); }
	vec<T, 4> baaa() const { return vec<T, 4>(b(), a(), a(), a()); }
	vec<T, 4> pqqq() const { return vec<T, 4>(p(), q(), q(), q()); }
	vec<T, 4> wxxx() const { return vec<T, 4>(w(), x(), x(), x()); }
	vec<T, 4> arrr() const { return vec<T, 4>(a(), r(), r(), r()); }
	vec<T, 4> qsss() const { return vec<T, 4>(q(), s(), s(), s()); }
	vec<T, 4> wxxy() const { return vec<T, 4>(w(), x(), x(), y()); }
	vec<T, 4> arrg() const { return vec<T, 4>(a(), r(), r(), g()); }
	vec<T, 4> qsst() const { return vec<T, 4>(q(), s(), s(), t()); }
	vec<T, 4> wxxz() const { return vec<T, 4>(w(), x(), x(), z()); }
	vec<T, 4> arrb() const { return vec<T, 4>(a(), r(), r(), b()); }
	vec<T, 4> qssp() const { return vec<T, 4>(q(), s(), s(), p()); }
	vec<T, 4> wxxw() const { return vec<T, 4>(w(), x(), x(), w()); }
	vec<T, 4> arra() const { return vec<T, 4>(a(), r(), r(), a()); }
	vec<T, 4> qssq() const { return vec<T, 4>(q(), s(), s(), q()); }
	vec<T, 4> wxyx() const { return vec<T, 4>(w(), x(), y(), x()); }
	vec<T, 4> argr() const { return vec<T, 4>(a(), r(), g(), r()); }
	vec<T, 4> qsts() const { return vec<T, 4>(q(), s(), t(), s()); }
	vec<T, 4> wxyy() const { return vec<T, 4>(w(), x(), y(), y()); }
	vec<T, 4> argg() const { return vec<T, 4>(a(), r(), g(), g()); }
	vec<T, 4> qstt() const { return vec<T, 4>(q(), s(), t(), t()); }
	vec<T, 4> wxyz() const { return vec<T, 4>(w(), x(), y(), z()); }
	vec<T, 4> argb() const { return vec<T, 4>(a(), r(), g(), b()); }
	vec<T, 4> qstp() const { return vec<T, 4>(q(), s(), t(), p()); }
	vec<T, 4> wxyw() const { return vec<T, 4>(w(), x(), y(), w()); }
	vec<T, 4> arga() const { return vec<T, 4>(a(), r(), g(), a()); }
	vec<T, 4> qstq() const { return vec<T, 4>(q(), s(), t(), q()); }
	vec<T, 4> wxzx() const { return vec<T, 4>(w(), x(), z(), x()); }
	vec<T, 4> arbr() const { return vec<T, 4>(a(), r(), b(), r()); }
	vec<T, 4> qsps() const { return vec<T, 4>(q(), s(), p(), s()); }
	vec<T, 4> wxzy() const { return vec<T, 4>(w(), x(), z(), y()); }
	vec<T, 4> arbg() const { return vec<T, 4>(a(), r(), b(), g()); }
	vec<T, 4> qspt() const { return vec<T, 4>(q(), s(), p(), t()); }
	vec<T, 4> wxzz() const { return vec<T, 4>(w(), x(), z(), z()); }
	vec<T, 4> arbb() const { return vec<T, 4>(a(), r(), b(), b()); }
	vec<T, 4> qspp() const { return vec<T, 4>(q(), s(), p(), p()); }
	vec<T, 4> wxzw() const { return vec<T, 4>(w(), x(), z(), w()); }
	vec<T, 4> arba() const { return vec<T, 4>(a(), r(), b(), a()); }
	vec<T, 4> qspq() const { return vec<T, 4>(q(), s(), p(), q()); }
	vec<T, 4> wxwx() const { return vec<T, 4>(w(), x(), w(), x()); }
	vec<T, 4> arar() const { return vec<T, 4>(a(), r(), a(), r()); }
	vec<T, 4> qsqs() const { return vec<T, 4>(q(), s(), q(), s()); }
	vec<T, 4> wxwy() const { return vec<T, 4>(w(), x(), w(), y()); }
	vec<T, 4> arag() const { return vec<T, 4>(a(), r(), a(), g()); }
	vec<T, 4> qsqt() const { return vec<T, 4>(q(), s(), q(), t()); }
	vec<T, 4> wxwz() const { return vec<T, 4>(w(), x(), w(), z()); }
	vec<T, 4> arab() const { return vec<T, 4>(a(), r(), a(), b()); }
	vec<T, 4> qsqp() const { return vec<T, 4>(q(), s(), q(), p()); }
	vec<T, 4> wxww() const { return vec<T, 4>(w(), x(), w(), w()); }
	vec<T, 4> araa() const { return vec<T, 4>(a(), r(), a(), a()); }
	vec<T, 4> qsqq() const { return vec<T, 4>(q(), s(), q(), q()); }
	vec<T, 4> wyxx() const { return vec<T, 4>(w(), y(), x(), x()); }
	vec<T, 4> agrr() const { return vec<T, 4>(a(), g(), r(), r()); }
	vec<T, 4> qtss() const { return vec<T, 4>(q(), t(), s(), s()); }
	vec<T, 4> wyxy() const { return vec<T, 4>(w(), y(), x(), y()); }
	vec<T, 4> agrg() const { return vec<T, 4>(a(), g(), r(), g()); }
	vec<T, 4> qtst() const { return vec<T, 4>(q(), t(), s(), t()); }
	vec<T, 4> wyxz() const { return vec<T, 4>(w(), y(), x(), z()); }
	vec<T, 4> agrb() const { return vec<T, 4>(a(), g(), r(), b()); }
	vec<T, 4> qtsp() const { return vec<T, 4>(q(), t(), s(), p()); }
	vec<T, 4> wyxw() const { return vec<T, 4>(w(), y(), x(), w()); }
	vec<T, 4> agra() const { return vec<T, 4>(a(), g(), r(), a()); }
	vec<T, 4> qtsq() const { return vec<T, 4>(q(), t(), s(), q()); }
	vec<T, 4> wyyx() const { return vec<T, 4>(w(), y(), y(), x()); }
	vec<T, 4> aggr() const { return vec<T, 4>(a(), g(), g(), r()); }
	vec<T, 4> qtts() const { return vec<T, 4>(q(), t(), t(), s()); }
	vec<T, 4> wyyy() const { return vec<T, 4>(w(), y(), y(), y()); }
	vec<T, 4> aggg() const { return vec<T, 4>(a(), g(), g(), g()); }
	vec<T, 4> qttt() const { return vec<T, 4>(q(), t(), t(), t()); }
	vec<T, 4> wyyz() const { return vec<T, 4>(w(), y(), y(), z()); }
	vec<T, 4> aggb() const { return vec<T, 4>(a(), g(), g(), b()); }
	vec<T, 4> qttp() const { return vec<T, 4>(q(), t(), t(), p()); }
	vec<T, 4> wyyw() const { return vec<T, 4>(w(), y(), y(), w()); }
	vec<T, 4> agga() const { return vec<T, 4>(a(), g(), g(), a()); }
	vec<T, 4> qttq() const { return vec<T, 4>(q(), t(), t(), q()); }
	vec<T, 4> wyzx() const { return vec<T, 4>(w(), y(), z(), x()); }
	vec<T, 4> agbr() const { return vec<T, 4>(a(), g(), b(), r()); }
	vec<T, 4> qtps() const { return vec<T, 4>(q(), t(), p(), s()); }
	vec<T, 4> wyzy() const { return vec<T, 4>(w(), y(), z(), y()); }
	vec<T, 4> agbg() const { return vec<T, 4>(a(), g(), b(), g()); }
	vec<T, 4> qtpt() const { return vec<T, 4>(q(), t(), p(), t()); }
	vec<T, 4> wyzz() const { return vec<T, 4>(w(), y(), z(), z()); }
	vec<T, 4> agbb() const { return vec<T, 4>(a(), g(), b(), b()); }
	vec<T, 4> qtpp() const { return vec<T, 4>(q(), t(), p(), p()); }
	vec<T, 4> wyzw() const { return vec<T, 4>(w(), y(), z(), w()); }
	vec<T, 4> agba() const { return vec<T, 4>(a(), g(), b(), a()); }
	vec<T, 4> qtpq() const { return vec<T, 4>(q(), t(), p(), q()); }
	vec<T, 4> wywx() const { return vec<T, 4>(w(), y(), w(), x()); }
	vec<T, 4> agar() const { return vec<T, 4>(a(), g(), a(), r()); }
	vec<T, 4> qtqs() const { return vec<T, 4>(q(), t(), q(), s()); }
	vec<T, 4> wywy() const { return vec<T, 4>(w(), y(), w(), y()); }
	vec<T, 4> agag() const { return vec<T, 4>(a(), g(), a(), g()); }
	vec<T, 4> qtqt() const { return vec<T, 4>(q(), t(), q(), t()); }
	vec<T, 4> wywz() const { return vec<T, 4>(w(), y(), w(), z()); }
	vec<T, 4> agab() const { return vec<T, 4>(a(), g(), a(), b()); }
	vec<T, 4> qtqp() const { return vec<T, 4>(q(), t(), q(), p()); }
	vec<T, 4> wyww() const { return vec<T, 4>(w(), y(), w(), w()); }
	vec<T, 4> agaa() const { return vec<T, 4>(a(), g(), a(), a()); }
	vec<T, 4> qtqq() const { return vec<T, 4>(q(), t(), q(), q()); }
	vec<T, 4> wzxx() const { return vec<T, 4>(w(), z(), x(), x()); }
	vec<T, 4> abrr() const { return vec<T, 4>(a(), b(), r(), r()); }
	vec<T, 4> qpss() const { return vec<T, 4>(q(), p(), s(), s()); }
	vec<T, 4> wzxy() const { return vec<T, 4>(w(), z(), x(), y()); }
	vec<T, 4> abrg() const { return vec<T, 4>(a(), b(), r(), g()); }
	vec<T, 4> qpst() const { return vec<T, 4>(q(), p(), s(), t()); }
	vec<T, 4> wzxz() const { return vec<T, 4>(w(), z(), x(), z()); }
	vec<T, 4> abrb() const { return vec<T, 4>(a(), b(), r(), b()); }
	vec<T, 4> qpsp() const { return vec<T, 4>(q(), p(), s(), p()); }
	vec<T, 4> wzxw() const { return vec<T, 4>(w(), z(), x(), w()); }
	vec<T, 4> abra() const { return vec<T, 4>(a(), b(), r(), a()); }
	vec<T, 4> qpsq() const { return vec<T, 4>(q(), p(), s(), q()); }
	vec<T, 4> wzyx() const { return vec<T, 4>(w(), z(), y(), x()); }
	vec<T, 4> abgr() const { return vec<T, 4>(a(), b(), g(), r()); }
	vec<T, 4> qpts() const { return vec<T, 4>(q(), p(), t(), s()); }
	vec<T, 4> wzyy() const { return vec<T, 4>(w(), z(), y(), y()); }
	vec<T, 4> abgg() const { return vec<T, 4>(a(), b(), g(), g()); }
	vec<T, 4> qptt() const { return vec<T, 4>(q(), p(), t(), t()); }
	vec<T, 4> wzyz() const { return vec<T, 4>(w(), z(), y(), z()); }
	vec<T, 4> abgb() const { return vec<T, 4>(a(), b(), g(), b()); }
	vec<T, 4> qptp() const { return vec<T, 4>(q(), p(), t(), p()); }
	vec<T, 4> wzyw() const { return vec<T, 4>(w(), z(), y(), w()); }
	vec<T, 4> abga() const { return vec<T, 4>(a(), b(), g(), a()); }
	vec<T, 4> qptq() const { return vec<T, 4>(q(), p(), t(), q()); }
	vec<T, 4> wzzx() const { return vec<T, 4>(w(), z(), z(), x()); }
	vec<T, 4> abbr() const { return vec<T, 4>(a(), b(), b(), r()); }
	vec<T, 4> qpps() const { return vec<T, 4>(q(), p(), p(), s()); }
	vec<T, 4> wzzy() const { return vec<T, 4>(w(), z(), z(), y()); }
	vec<T, 4> abbg() const { return vec<T, 4>(a(), b(), b(), g()); }
	vec<T, 4> qppt() const { return vec<T, 4>(q(), p(), p(), t()); }
	vec<T, 4> wzzz() const { return vec<T, 4>(w(), z(), z(), z()); }
	vec<T, 4> abbb() const { return vec<T, 4>(a(), b(), b(), b()); }
	vec<T, 4> qppp() const { return vec<T, 4>(q(), p(), p(), p()); }
	vec<T, 4> wzzw() const { return vec<T, 4>(w(), z(), z(), w()); }
	vec<T, 4> abba() const { return vec<T, 4>(a(), b(), b(), a()); }
	vec<T, 4> qppq() const { return vec<T, 4>(q(), p(), p(), q()); }
	vec<T, 4> wzwx() const { return vec<T, 4>(w(), z(), w(), x()); }
	vec<T, 4> abar() const { return vec<T, 4>(a(), b(), a(), r()); }
	vec<T, 4> qpqs() const { return vec<T, 4>(q(), p(), q(), s()); }
	vec<T, 4> wzwy() const { return vec<T, 4>(w(), z(), w(), y()); }
	vec<T, 4> abag() const { return vec<T, 4>(a(), b(), a(), g()); }
	vec<T, 4> qpqt() const { return vec<T, 4>(q(), p(), q(), t()); }
	vec<T, 4> wzwz() const { return vec<T, 4>(w(), z(), w(), z()); }
	vec<T, 4> abab() const { return vec<T, 4>(a(), b(), a(), b()); }
	vec<T, 4> qpqp() const { return vec<T, 4>(q(), p(), q(), p()); }
	vec<T, 4> wzww() const { return vec<T, 4>(w(), z(), w(), w()); }
	vec<T, 4> abaa() const { return vec<T, 4>(a(), b(), a(), a()); }
	vec<T, 4> qpqq() const { return vec<T, 4>(q(), p(), q(), q()); }
	vec<T, 4> wwxx() const { return vec<T, 4>(w(), w(), x(), x()); }
	vec<T, 4> aarr() const { return vec<T, 4>(a(), a(), r(), r()); }
	vec<T, 4> qqss() const { return vec<T, 4>(q(), q(), s(), s()); }
	vec<T, 4> wwxy() const { return vec<T, 4>(w(), w(), x(), y()); }
	vec<T, 4> aarg() const { return vec<T, 4>(a(), a(), r(), g()); }
	vec<T, 4> qqst() const { return vec<T, 4>(q(), q(), s(), t()); }
	vec<T, 4> wwxz() const { return vec<T, 4>(w(), w(), x(), z()); }
	vec<T, 4> aarb() const { return vec<T, 4>(a(), a(), r(), b()); }
	vec<T, 4> qqsp() const { return vec<T, 4>(q(), q(), s(), p()); }
	vec<T, 4> wwxw() const { return vec<T, 4>(w(), w(), x(), w()); }
	vec<T, 4> aara() const { return vec<T, 4>(a(), a(), r(), a()); }
	vec<T, 4> qqsq() const { return vec<T, 4>(q(), q(), s(), q()); }
	vec<T, 4> wwyx() const { return vec<T, 4>(w(), w(), y(), x()); }
	vec<T, 4> aagr() const { return vec<T, 4>(a(), a(), g(), r()); }
	vec<T, 4> qqts() const { return vec<T, 4>(q(), q(), t(), s()); }
	vec<T, 4> wwyy() const { return vec<T, 4>(w(), w(), y(), y()); }
	vec<T, 4> aagg() const { return vec<T, 4>(a(), a(), g(), g()); }
	vec<T, 4> qqtt() const { return vec<T, 4>(q(), q(), t(), t()); }
	vec<T, 4> wwyz() const { return vec<T, 4>(w(), w(), y(), z()); }
	vec<T, 4> aagb() const { return vec<T, 4>(a(), a(), g(), b()); }
	vec<T, 4> qqtp() const { return vec<T, 4>(q(), q(), t(), p()); }
	vec<T, 4> wwyw() const { return vec<T, 4>(w(), w(), y(), w()); }
	vec<T, 4> aaga() const { return vec<T, 4>(a(), a(), g(), a()); }
	vec<T, 4> qqtq() const { return vec<T, 4>(q(), q(), t(), q()); }
	vec<T, 4> wwzx() const { return vec<T, 4>(w(), w(), z(), x()); }
	vec<T, 4> aabr() const { return vec<T, 4>(a(), a(), b(), r()); }
	vec<T, 4> qqps() const { return vec<T, 4>(q(), q(), p(), s()); }
	vec<T, 4> wwzy() const { return vec<T, 4>(w(), w(), z(), y()); }
	vec<T, 4> aabg() const { return vec<T, 4>(a(), a(), b(), g()); }
	vec<T, 4> qqpt() const { return vec<T, 4>(q(), q(), p(), t()); }
	vec<T, 4> wwzz() const { return vec<T, 4>(w(), w(), z(), z()); }
	vec<T, 4> aabb() const { return vec<T, 4>(a(), a(), b(), b()); }
	vec<T, 4> qqpp() const { return vec<T, 4>(q(), q(), p(), p()); }
	vec<T, 4> wwzw() const { return vec<T, 4>(w(), w(), z(), w()); }
	vec<T, 4> aaba() const { return vec<T, 4>(a(), a(), b(), a()); }
	vec<T, 4> qqpq() const { return vec<T, 4>(q(), q(), p(), q()); }
	vec<T, 4> wwwx() const { return vec<T, 4>(w(), w(), w(), x()); }
	vec<T, 4> aaar() const { return vec<T, 4>(a(), a(), a(), r()); }
	vec<T, 4> qqqs() const { return vec<T, 4>(q(), q(), q(), s()); }
	vec<T, 4> wwwy() const { return vec<T, 4>(w(), w(), w(), y()); }
	vec<T, 4> aaag() const { return vec<T, 4>(a(), a(), a(), g()); }
	vec<T, 4> qqqt() const { return vec<T, 4>(q(), q(), q(), t()); }
	vec<T, 4> wwwz() const { return vec<T, 4>(w(), w(), w(), z()); }
	vec<T, 4> aaab() const { return vec<T, 4>(a(), a(), a(), b()); }
	vec<T, 4> qqqp() const { return vec<T, 4>(q(), q(), q(), p()); }
	vec<T, 4> wwww() const { return vec<T, 4>(w(), w(), w(), w()); }
	vec<T, 4> aaaa() const { return vec<T, 4>(a(), a(), a(), a()); }
	vec<T, 4> qqqq() const { return vec<T, 4>(q(), q(), q(), q()); }
	T &x() { return array<T, rows, 1>::vl[0]; }
	T &r() { return array<T, rows, 1>::vl[0]; }
	T &s() { return array<T, rows, 1>::vl[0]; }
	T &y() { return array<T, rows, 1>::vl[1]; }
	T &g() { return array<T, rows, 1>::vl[1]; }
	T &t() { return array<T, rows, 1>::vl[1]; }
	T &z() { return array<T, rows, 1>::vl[2]; }
	T &b() { return array<T, rows, 1>::vl[2]; }
	T &p() { return array<T, rows, 1>::vl[2]; }
	T &w() { return array<T, rows, 1>::vl[3]; }
	T &a() { return array<T, rows, 1>::vl[3]; }
	T &q() { return array<T, rows, 1>::vl[3]; }
	swizzler2<T> xy() { return swizzler2<T>(x(), y()); }
	swizzler2<T> rg() { return swizzler2<T>(r(), g()); }
	swizzler2<T> st() { return swizzler2<T>(s(), t()); }
	swizzler2<T> xz() { return swizzler2<T>(x(), z()); }
	swizzler2<T> rb() { return swizzler2<T>(r(), b()); }
	swizzler2<T> sp() { return swizzler2<T>(s(), p()); }
	swizzler2<T> xw() { return swizzler2<T>(x(), w()); }
	swizzler2<T> ra() { return swizzler2<T>(r(), a()); }
	swizzler2<T> sq() { return swizzler2<T>(s(), q()); }
	swizzler2<T> yx() { return swizzler2<T>(y(), x()); }
	swizzler2<T> gr() { return swizzler2<T>(g(), r()); }
	swizzler2<T> ts() { return swizzler2<T>(t(), s()); }
	swizzler2<T> yz() { return swizzler2<T>(y(), z()); }
	swizzler2<T> gb() { return swizzler2<T>(g(), b()); }
	swizzler2<T> tp() { return swizzler2<T>(t(), p()); }
	swizzler2<T> yw() { return swizzler2<T>(y(), w()); }
	swizzler2<T> ga() { return swizzler2<T>(g(), a()); }
	swizzler2<T> tq() { return swizzler2<T>(t(), q()); }
	swizzler2<T> zx() { return swizzler2<T>(z(), x()); }
	swizzler2<T> br() { return swizzler2<T>(b(), r()); }
	swizzler2<T> ps() { return swizzler2<T>(p(), s()); }
	swizzler2<T> zy() { return swizzler2<T>(z(), y()); }
	swizzler2<T> bg() { return swizzler2<T>(b(), g()); }
	swizzler2<T> pt() { return swizzler2<T>(p(), t()); }
	swizzler2<T> zw() { return swizzler2<T>(z(), w()); }
	swizzler2<T> ba() { return swizzler2<T>(b(), a()); }
	swizzler2<T> pq() { return swizzler2<T>(p(), q()); }
	swizzler2<T> wx() { return swizzler2<T>(w(), x()); }
	swizzler2<T> ar() { return swizzler2<T>(a(), r()); }
	swizzler2<T> qs() { return swizzler2<T>(q(), s()); }
	swizzler2<T> wy() { return swizzler2<T>(w(), y()); }
	swizzler2<T> ag() { return swizzler2<T>(a(), g()); }
	swizzler2<T> qt() { return swizzler2<T>(q(), t()); }
	swizzler2<T> wz() { return swizzler2<T>(w(), z()); }
	swizzler2<T> ab() { return swizzler2<T>(a(), b()); }
	swizzler2<T> qp() { return swizzler2<T>(q(), p()); }
	swizzler3<T> xyz() { return swizzler3<T>(x(), y(), z()); }
	swizzler3<T> rgb() { return swizzler3<T>(r(), g(), b()); }
	swizzler3<T> stp() { return swizzler3<T>(s(), t(), p()); }
	swizzler3<T> xyw() { return swizzler3<T>(x(), y(), w()); }
	swizzler3<T> rga() { return swizzler3<T>(r(), g(), a()); }
	swizzler3<T> stq() { return swizzler3<T>(s(), t(), q()); }
	swizzler3<T> xzy() { return swizzler3<T>(x(), z(), y()); }
	swizzler3<T> rbg() { return swizzler3<T>(r(), b(), g()); }
	swizzler3<T> spt() { return swizzler3<T>(s(), p(), t()); }
	swizzler3<T> xzw() { return swizzler3<T>(x(), z(), w()); }
	swizzler3<T> rba() { return swizzler3<T>(r(), b(), a()); }
	swizzler3<T> spq() { return swizzler3<T>(s(), p(), q()); }
	swizzler3<T> xwy() { return swizzler3<T>(x(), w(), y()); }
	swizzler3<T> rag() { return swizzler3<T>(r(), a(), g()); }
	swizzler3<T> sqt() { return swizzler3<T>(s(), q(), t()); }
	swizzler3<T> xwz() { return swizzler3<T>(x(), w(), z()); }
	swizzler3<T> rab() { return swizzler3<T>(r(), a(), b()); }
	swizzler3<T> sqp() { return swizzler3<T>(s(), q(), p()); }
	swizzler3<T> yxz() { return swizzler3<T>(y(), x(), z()); }
	swizzler3<T> grb() { return swizzler3<T>(g(), r(), b()); }
	swizzler3<T> tsp() { return swizzler3<T>(t(), s(), p()); }
	swizzler3<T> yxw() { return swizzler3<T>(y(), x(), w()); }
	swizzler3<T> gra() { return swizzler3<T>(g(), r(), a()); }
	swizzler3<T> tsq() { return swizzler3<T>(t(), s(), q()); }
	swizzler3<T> yzx() { return swizzler3<T>(y(), z(), x()); }
	swizzler3<T> gbr() { return swizzler3<T>(g(), b(), r()); }
	swizzler3<T> tps() { return swizzler3<T>(t(), p(), s()); }
	swizzler3<T> yzw() { return swizzler3<T>(y(), z(), w()); }
	swizzler3<T> gba() { return swizzler3<T>(g(), b(), a()); }
	swizzler3<T> tpq() { return swizzler3<T>(t(), p(), q()); }
	swizzler3<T> ywx() { return swizzler3<T>(y(), w(), x()); }
	swizzler3<T> gar() { return swizzler3<T>(g(), a(), r()); }
	swizzler3<T> tqs() { return swizzler3<T>(t(), q(), s()); }
	swizzler3<T> ywz() { return swizzler3<T>(y(), w(), z()); }
	swizzler3<T> gab() { return swizzler3<T>(g(), a(), b()); }
	swizzler3<T> tqp() { return swizzler3<T>(t(), q(), p()); }
	swizzler3<T> zxy() { return swizzler3<T>(z(), x(), y()); }
	swizzler3<T> brg() { return swizzler3<T>(b(), r(), g()); }
	swizzler3<T> pst() { return swizzler3<T>(p(), s(), t()); }
	swizzler3<T> zxw() { return swizzler3<T>(z(), x(), w()); }
	swizzler3<T> bra() { return swizzler3<T>(b(), r(), a()); }
	swizzler3<T> psq() { return swizzler3<T>(p(), s(), q()); }
	swizzler3<T> zyx() { return swizzler3<T>(z(), y(), x()); }
	swizzler3<T> bgr() { return swizzler3<T>(b(), g(), r()); }
	swizzler3<T> pts() { return swizzler3<T>(p(), t(), s()); }
	swizzler3<T> zyw() { return swizzler3<T>(z(), y(), w()); }
	swizzler3<T> bga() { return swizzler3<T>(b(), g(), a()); }
	swizzler3<T> ptq() { return swizzler3<T>(p(), t(), q()); }
	swizzler3<T> zwx() { return swizzler3<T>(z(), w(), x()); }
	swizzler3<T> bar() { return swizzler3<T>(b(), a(), r()); }
	swizzler3<T> pqs() { return swizzler3<T>(p(), q(), s()); }
	swizzler3<T> zwy() { return swizzler3<T>(z(), w(), y()); }
	swizzler3<T> bag() { return swizzler3<T>(b(), a(), g()); }
	swizzler3<T> pqt() { return swizzler3<T>(p(), q(), t()); }
	swizzler3<T> wxy() { return swizzler3<T>(w(), x(), y()); }
	swizzler3<T> arg() { return swizzler3<T>(a(), r(), g()); }
	swizzler3<T> qst() { return swizzler3<T>(q(), s(), t()); }
	swizzler3<T> wxz() { return swizzler3<T>(w(), x(), z()); }
	swizzler3<T> arb() { return swizzler3<T>(a(), r(), b()); }
	swizzler3<T> qsp() { return swizzler3<T>(q(), s(), p()); }
	swizzler3<T> wyx() { return swizzler3<T>(w(), y(), x()); }
	swizzler3<T> agr() { return swizzler3<T>(a(), g(), r()); }
	swizzler3<T> qts() { return swizzler3<T>(q(), t(), s()); }
	swizzler3<T> wyz() { return swizzler3<T>(w(), y(), z()); }
	swizzler3<T> agb() { return swizzler3<T>(a(), g(), b()); }
	swizzler3<T> qtp() { return swizzler3<T>(q(), t(), p()); }
	swizzler3<T> wzx() { return swizzler3<T>(w(), z(), x()); }
	swizzler3<T> abr() { return swizzler3<T>(a(), b(), r()); }
	swizzler3<T> qps() { return swizzler3<T>(q(), p(), s()); }
	swizzler3<T> wzy() { return swizzler3<T>(w(), z(), y()); }
	swizzler3<T> abg() { return swizzler3<T>(a(), b(), g()); }
	swizzler3<T> qpt() { return swizzler3<T>(q(), p(), t()); }
	swizzler4<T> xyzw() { return swizzler4<T>(x(), y(), z(), w()); }
	swizzler4<T> rgba() { return swizzler4<T>(r(), g(), b(), a()); }
	swizzler4<T> stpq() { return swizzler4<T>(s(), t(), p(), q()); }
	swizzler4<T> xywz() { return swizzler4<T>(x(), y(), w(), z()); }
	swizzler4<T> rgab() { return swizzler4<T>(r(), g(), a(), b()); }
	swizzler4<T> stqp() { return swizzler4<T>(s(), t(), q(), p()); }
	swizzler4<T> xzyw() { return swizzler4<T>(x(), z(), y(), w()); }
	swizzler4<T> rbga() { return swizzler4<T>(r(), b(), g(), a()); }
	swizzler4<T> sptq() { return swizzler4<T>(s(), p(), t(), q()); }
	swizzler4<T> xzwy() { return swizzler4<T>(x(), z(), w(), y()); }
	swizzler4<T> rbag() { return swizzler4<T>(r(), b(), a(), g()); }
	swizzler4<T> spqt() { return swizzler4<T>(s(), p(), q(), t()); }
	swizzler4<T> xwyz() { return swizzler4<T>(x(), w(), y(), z()); }
	swizzler4<T> ragb() { return swizzler4<T>(r(), a(), g(), b()); }
	swizzler4<T> sqtp() { return swizzler4<T>(s(), q(), t(), p()); }
	swizzler4<T> xwzy() { return swizzler4<T>(x(), w(), z(), y()); }
	swizzler4<T> rabg() { return swizzler4<T>(r(), a(), b(), g()); }
	swizzler4<T> sqpt() { return swizzler4<T>(s(), q(), p(), t()); }
	swizzler4<T> yxzw() { return swizzler4<T>(y(), x(), z(), w()); }
	swizzler4<T> grba() { return swizzler4<T>(g(), r(), b(), a()); }
	swizzler4<T> tspq() { return swizzler4<T>(t(), s(), p(), q()); }
	swizzler4<T> yxwz() { return swizzler4<T>(y(), x(), w(), z()); }
	swizzler4<T> grab() { return swizzler4<T>(g(), r(), a(), b()); }
	swizzler4<T> tsqp() { return swizzler4<T>(t(), s(), q(), p()); }
	swizzler4<T> yzxw() { return swizzler4<T>(y(), z(), x(), w()); }
	swizzler4<T> gbra() { return swizzler4<T>(g(), b(), r(), a()); }
	swizzler4<T> tpsq() { return swizzler4<T>(t(), p(), s(), q()); }
	swizzler4<T> yzwx() { return swizzler4<T>(y(), z(), w(), x()); }
	swizzler4<T> gbar() { return swizzler4<T>(g(), b(), a(), r()); }
	swizzler4<T> tpqs() { return swizzler4<T>(t(), p(), q(), s()); }
	swizzler4<T> ywxz() { return swizzler4<T>(y(), w(), x(), z()); }
	swizzler4<T> garb() { return swizzler4<T>(g(), a(), r(), b()); }
	swizzler4<T> tqsp() { return swizzler4<T>(t(), q(), s(), p()); }
	swizzler4<T> ywzx() { return swizzler4<T>(y(), w(), z(), x()); }
	swizzler4<T> gabr() { return swizzler4<T>(g(), a(), b(), r()); }
	swizzler4<T> tqps() { return swizzler4<T>(t(), q(), p(), s()); }
	swizzler4<T> zxyw() { return swizzler4<T>(z(), x(), y(), w()); }
	swizzler4<T> brga() { return swizzler4<T>(b(), r(), g(), a()); }
	swizzler4<T> pstq() { return swizzler4<T>(p(), s(), t(), q()); }
	swizzler4<T> zxwy() { return swizzler4<T>(z(), x(), w(), y()); }
	swizzler4<T> brag() { return swizzler4<T>(b(), r(), a(), g()); }
	swizzler4<T> psqt() { return swizzler4<T>(p(), s(), q(), t()); }
	swizzler4<T> zyxw() { return swizzler4<T>(z(), y(), x(), w()); }
	swizzler4<T> bgra() { return swizzler4<T>(b(), g(), r(), a()); }
	swizzler4<T> ptsq() { return swizzler4<T>(p(), t(), s(), q()); }
	swizzler4<T> zywx() { return swizzler4<T>(z(), y(), w(), x()); }
	swizzler4<T> bgar() { return swizzler4<T>(b(), g(), a(), r()); }
	swizzler4<T> ptqs() { return swizzler4<T>(p(), t(), q(), s()); }
	swizzler4<T> zwxy() { return swizzler4<T>(z(), w(), x(), y()); }
	swizzler4<T> barg() { return swizzler4<T>(b(), a(), r(), g()); }
	swizzler4<T> pqst() { return swizzler4<T>(p(), q(), s(), t()); }
	swizzler4<T> zwyx() { return swizzler4<T>(z(), w(), y(), x()); }
	swizzler4<T> bagr() { return swizzler4<T>(b(), a(), g(), r()); }
	swizzler4<T> pqts() { return swizzler4<T>(p(), q(), t(), s()); }
	swizzler4<T> wxyz() { return swizzler4<T>(w(), x(), y(), z()); }
	swizzler4<T> argb() { return swizzler4<T>(a(), r(), g(), b()); }
	swizzler4<T> qstp() { return swizzler4<T>(q(), s(), t(), p()); }
	swizzler4<T> wxzy() { return swizzler4<T>(w(), x(), z(), y()); }
	swizzler4<T> arbg() { return swizzler4<T>(a(), r(), b(), g()); }
	swizzler4<T> qspt() { return swizzler4<T>(q(), s(), p(), t()); }
	swizzler4<T> wyxz() { return swizzler4<T>(w(), y(), x(), z()); }
	swizzler4<T> agrb() { return swizzler4<T>(a(), g(), r(), b()); }
	swizzler4<T> qtsp() { return swizzler4<T>(q(), t(), s(), p()); }
	swizzler4<T> wyzx() { return swizzler4<T>(w(), y(), z(), x()); }
	swizzler4<T> agbr() { return swizzler4<T>(a(), g(), b(), r()); }
	swizzler4<T> qtps() { return swizzler4<T>(q(), t(), p(), s()); }
	swizzler4<T> wzxy() { return swizzler4<T>(w(), z(), x(), y()); }
	swizzler4<T> abrg() { return swizzler4<T>(a(), b(), r(), g()); }
	swizzler4<T> qpst() { return swizzler4<T>(q(), p(), s(), t()); }
	swizzler4<T> wzyx() { return swizzler4<T>(w(), z(), y(), x()); }
	swizzler4<T> abgr() { return swizzler4<T>(a(), b(), g(), r()); }
	swizzler4<T> qpts() { return swizzler4<T>(q(), p(), t(), s()); }
    };

    template<typename T, int r> vec<T, r> sin(const array<T, r, 1> &v) { return v._sin(); }
    template<typename T, int r> vec<T, r> cos(const array<T, r, 1> &v) { return v._cos(); }
    template<typename T, int r> vec<T, r> tan(const array<T, r, 1> &v) { return v._tan(); }
    template<typename T, int r> vec<T, r> asin(const array<T, r, 1> &v) { return v._asin(); }
    template<typename T, int r> vec<T, r> acos(const array<T, r, 1> &v) { return v._acos(); }
    template<typename T, int r> vec<T, r> atan(const array<T, r, 1> &v) { return v._atan(); }
    template<typename T, int r> vec<T, r> atan(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._atan(w); }
    template<typename T, int r> vec<T, r> radians(const array<T, r, 1> &v) { return v._radians(); }
    template<typename T, int r> vec<T, r> degrees(const array<T, r, 1> &v) { return v._degrees(); }

    template<typename T, int r> vec<T, r> pow(const array<T, r, 1> &v, const T p) { return v._pow(p); }
    template<typename T, int r> vec<T, r> exp(const array<T, r, 1> &v) { return v._exp(); }
    template<typename T, int r> vec<T, r> exp2(const array<T, r, 1> &v) { return v._exp2(); }
    template<typename T, int r> vec<T, r> log(const array<T, r, 1> &v) { return v._log(); }
    template<typename T, int r> vec<T, r> log2(const array<T, r, 1> &v) { return v._log2(); }
    template<typename T, int r> vec<T, r> log10(const array<T, r, 1> &v) { return v._log10(); }
    template<typename T, int r> vec<T, r> sqrt(const array<T, r, 1> &v) { return v._sqrt(); }
    template<typename T, int r> vec<T, r> inversesqrt(const array<T, r, 1> &v) { return v._inversesqrt(); }
    template<typename T, int r> vec<T, r> cbrt(const array<T, r, 1> &v) { return v._cbrt(); }

    template<typename T, int r> vec<bool, r> isfinite(const array<T, r, 1> &v) { return v._isfinite(); }
    template<typename T, int r> vec<bool, r> isinf(const array<T, r, 1> &v) { return v._isinf(); }
    template<typename T, int r> vec<bool, r> isnan(const array<T, r, 1> &v) { return v._isnan(); }
    template<typename T, int r> vec<bool, r> isnormal(const array<T, r, 1> &v) { return v._isnormal(); }

    template<typename T, int r> vec<T, r> abs(const array<T, r, 1> &v) { return v._abs(); }
    template<typename T, int r> vec<T, r> sign(const array<T, r, 1> &v) { return v._sign(); }
    template<typename T, int r> vec<T, r> floor(const array<T, r, 1> &v) { return v._floor(); }
    template<typename T, int r> vec<T, r> ceil(const array<T, r, 1> &v) { return v._ceil(); }
    template<typename T, int r> vec<T, r> round(const array<T, r, 1> &v) { return v._round(); }
    template<typename T, int r> vec<T, r> fract(const array<T, r, 1> &v) { return v._fract(); }

    template<typename T, int r> vec<T, r> min(const array<T, r, 1> &v, const T x) { return v._min(x); }
    template<typename T, int r> vec<T, r> min(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._min(w); }
    template<typename T, int r> vec<T, r> max(const array<T, r, 1> &v, const T x) { return v._max(x); }
    template<typename T, int r> vec<T, r> max(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._max(w); }

    template<typename T, int r> vec<T, r> clamp(const array<T, r, 1> &v, const T a, const T b) { return v._clamp(a, b); }
    template<typename T, int r> vec<T, r> clamp(const array<T, r, 1> &v, const array<T, r, 1> &a, const array<T, r, 1> &b) { return v._clamp(a, b); }
    template<typename T, int r> vec<T, r> mix(const array<T, r, 1> &v, const array<T, r, 1> &w, const T a) { return v._mix(w, a); }
    template<typename T, int r> vec<T, r> mix(const array<T, r, 1> &v, const array<T, r, 1> &w, const array<T, r, 1> &a) { return v._mix(w, a); }
    template<typename T, int r> vec<T, r> step(const array<T, r, 1> &v, const T a) { return v._step(a); }
    template<typename T, int r> vec<T, r> step(const array<T, r, 1> &v, const array<T, r, 1> &a) { return v._step(a); }
    template<typename T, int r> vec<T, r> smoothstep(const array<T, r, 1> &v, const T e0, const T e1) { return v._smoothstep(e0, e1); }
    template<typename T, int r> vec<T, r> smoothstep(const array<T, r, 1> &v, const array<T, r, 1> &e0, const array<T, r, 1> &e1) { return v._smoothstep(e0, e1); }
    template<typename T, int r> vec<T, r> mod(const array<T, r, 1> &v, const T x) { return v._mod(x); }
    template<typename T, int r> vec<T, r> mod(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._mod(w); }

    template<typename T, int r> vec<bool, r> greaterThan(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._greaterThan(w); }
    template<typename T, int r> vec<bool, r> greaterThanEqual(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._greaterThanEqual(w); }
    template<typename T, int r> vec<bool, r> lessThan(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._lessThan(w); }
    template<typename T, int r> vec<bool, r> lessThanEqual(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._lessThanEqual(w); }
    template<typename T, int r> vec<bool, r> equal(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._equal(w); }
    template<typename T, int r> vec<bool, r> equal(const array<T, r, 1> &v, const array<T, r, 1> &w, const int max_ulps) { return v._equal(w, max_ulps); }
    template<typename T, int r> vec<bool, r> notEqual(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._notEqual(w); }
    template<typename T, int r> vec<bool, r> notEqual(const array<T, r, 1> &v, const array<T, r, 1> &w, const int max_ulps) { return v._notEqual(w, max_ulps); }
    template<int r> bool any(const array<bool, r, 1> &v) { return v._any(); }
    template<int r> bool all(const array<bool, r, 1> &v) { return v._all(); }
    template<int r> bool negate(const array<bool, r, 1> &v) { return v._negate(); }

    template<typename T, int r> T length(const array<T, r, 1> &v) { return v._length(); }
    template<typename T, int r> T distance(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._distance(w); }
    template<typename T, int r> T dot(const array<T, r, 1> &v, const array<T, r, 1> &w) { return v._dot(w); }
    template<typename T, int r> vec<T, r> normalize(const array<T, r, 1> &v) { return v._normalize(); }

    template<typename T, int r> vec<T, r> faceforward(const array<T, r, 1> &v, const array<T, r, 1> &I, const array<T, r, 1> &Nref) { return v._faceforward(I, Nref); } 
    template<typename T, int r> vec<T, r> reflect(const array<T, r, 1> &I, const array<T, r, 1> &N) { return I._reflect(N); }
    template<typename T, int r> vec<T, r> refract(const array<T, r, 1> &I, const array<T, r, 1> &N, T eta) { return I._refract(N, eta); }

    template<typename T> vec<T, 3> cross(const array<T, 3, 1> &v, const array<T, 3, 1> &w)
    {
	return vec<T, 3>(
		v.vl[1] * w.vl[2] - v.vl[2] * w.vl[1], 
		v.vl[2] * w.vl[0] - v.vl[0] * w.vl[2], 
		v.vl[0] * w.vl[1] - v.vl[1] * w.vl[0]);
    }

    typedef vec<bool, 2> bvec2;
    typedef vec<int, 2> ivec2;
    typedef vec<float, 2> vec2;
    typedef vec<double, 2> dvec2;
    typedef vec<bool, 3> bvec3;
    typedef vec<int, 3> ivec3;
    typedef vec<float, 3> vec3;
    typedef vec<double, 3> dvec3;
    typedef vec<bool, 4> bvec4;
    typedef vec<int, 4> ivec4;
    typedef vec<float, 4> vec4;
    typedef vec<double, 4> dvec4;


    /* Matrices */

    template<typename T, int rows, int cols>
    class mat : public array<T, rows, cols>
    {
	public:

	/* Constructors, Destructor */

	mat() {}
	mat(const array<T, rows, cols> &a) : array<T, rows, cols>(a) {}
	mat(const T x)
	{
	    for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
		    array<T, rows, cols>::v[i][j] = (i == j ? x : static_cast<T>(0));
	}
	mat(const T v0, const T v1, const T v2, const T v3) 
	    : array<T, rows, cols>(v0, v1, v2, v3) {}
	mat(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5) {}
	mat(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7) {}
	mat(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7, const T v8)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8) {}
	mat(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5,
		const T v6, const T v7, const T v8, const T v9, const T v10, const T v11)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) {}
	mat(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7,
		const T v8, const T v9, const T v10, const T v11, const T v12, const T v13, const T v14, const T v15)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) {}

	mat(const vec<T, rows> &col0, const vec<T, rows> &col1)
	{
	    for (int i = 0; i < rows; i++)
	    {
		array<T, rows, cols>::v[i][0] = col0.vl[i];
		array<T, rows, cols>::v[i][1] = col1.vl[i];
	    }
	}
	mat(const vec<T, rows> &col0, const vec<T, rows> &col1, const vec<T, rows> &col2)
	{
	    for (int i = 0; i < rows; i++)
	    {
		array<T, rows, cols>::v[i][0] = col0.vl[i];
		array<T, rows, cols>::v[i][1] = col1.vl[i];
		array<T, rows, cols>::v[i][2] = col2.vl[i];
	    }
	}
	mat(const vec<T, rows> &col0, const vec<T, rows> &col1, const vec<T, rows> &col2, const vec<T, rows> &col3)
	{
	    for (int i = 0; i < rows; i++)
	    {
		array<T, rows, cols>::v[i][0] = col0.vl[i];
		array<T, rows, cols>::v[i][1] = col1.vl[i];
		array<T, rows, cols>::v[i][2] = col2.vl[i];
		array<T, rows, cols>::v[i][3] = col3.vl[i];
	    }
	}

	mat(const T *a) : array<T, rows, cols>(a) {}
	template<typename U>
	mat(const mat<U, rows, cols> &a) : array<T, rows, cols>(a) {}
	mat(const std::string &s) : array<T, rows, cols>(s) {}
	~mat() {}

	/* Operators */

	T *operator[](const int row)
	{
	    return array<T, rows, cols>::v[row];
	}

	const T *operator[](const int row) const
	{
	    return array<T, rows, cols>::v[row];
	}

	const mat &operator=(const mat &a)  { array<T, rows, cols>::_op_assign(a); return *this; }
	mat operator+(const mat &a) const   { return array<T, rows, cols>::_op_plus(a); }
	const mat &operator+=(const mat &a) { array<T, rows, cols>::_op_plus_assign(a); return *this; }
	mat operator+() const               { return array<T, rows, cols>::_op_unary_plus(); }
	mat operator-(const mat &a) const   { return array<T, rows, cols>::_op_minus(a); }
	const mat &operator-=(const mat &a) { array<T, rows, cols>::_op_minus_assign(a); return *this; }
	mat operator-() const               { return array<T, rows, cols>::_op_unary_minus(); }
	mat operator*(const T s) const      { return array<T, rows, cols>::_op_scal_mult(s); }
	friend mat operator*(const T s, const mat &a) { return a._op_scal_mult(s); }
	const mat &operator*=(const T s)    { array<T, rows, cols>::_op_scal_mult_assign(s); return *this; }
	mat operator/(const T s) const      { return array<T, rows, cols>::_op_scal_div(s); }
	const mat &operator/=(const T s)    { array<T, rows, cols>::_op_scal_div_assign(s); return *this; }
	mat operator%(const T s) const      { return array<T, rows, cols>::_op_scal_mod(s); }
	const mat &operator%=(const T s)    { array<T, rows, cols>::_op_scal_mod_assign(s); return *this; }
	bool operator==(const mat &a) const { return array<T, rows, cols>::_op_equal(a); }
	bool operator!=(const mat &a) const { return array<T, rows, cols>::_op_notequal(a); }
	
	vec<T, rows> operator*(const vec<T, cols> &w) const
	{
	    vec<T, rows> r;
	    for (int i = 0; i < rows; i++)
	    {
		r.vl[i] = static_cast<T>(0);
		for (int j = 0; j < cols; j++)
	       	{
    		    r.vl[i] += array<T, rows, cols>::v[i][j] * w.vl[j];
		}
	    }
	    return r;
	}

    	friend vec<T, cols> operator*(const vec<T, rows> &w, const mat &m)
	{
	    vec<T, cols> r;
	    for (int i = 0; i < rows; i++)
	    {
		r.vl[i] = static_cast<T>(0);
		for (int j = 0; j < cols; j++)
	       	{
    		    r.vl[i] += m.v[i][j] * w.vl[j];
		}
	    }
	    return r;

	}

	mat<T, rows, rows> operator*(const mat<T, cols, rows> &n) const 
	{
	    mat<T, rows, rows> r;
	    for (int i = 0; i < rows; i++)
	    {
		for (int j = 0; j < rows; j++)
		{
		    r.v[i][j] = static_cast<T>(0);
		    for (int k = 0; k < rows; k++)
		    {
			r.v[i][j] += array<T, rows, cols>::v[i][k] * n.v[k][j];
		    }
		}
	    }
	    return r;
	}

	const mat<T, rows, cols> &operator*=(const mat<T, rows, cols> &n) const 
	{
	    mat<T, rows, cols> r = *this * n;
	    *this = r;
	    return *this;
	}
    };

    template<typename T, int r> vec<T, 2> row(const mat<T, r, 2> &m, const int row) { return vec<T, 2>(m[row][0], m[row][1]); }
    template<typename T, int r> vec<T, 3> row(const mat<T, r, 3> &m, const int row) { return vec<T, 3>(m[row][0], m[row][1], m[row][2]); }
    template<typename T, int r> vec<T, 4> row(const mat<T, r, 4> &m, const int row) { return vec<T, 4>(m[row][0], m[row][1], m[row][2], m[row][3]); }
    template<typename T, int r> swizzler2<T> row(mat<T, r, 2> &m, const int row) { return swizzler2<T>(m[row][0], m[row][1]); }
    template<typename T, int r> swizzler3<T> row(mat<T, r, 3> &m, const int row) { return swizzler3<T>(m[row][0], m[row][1], m[row][2]); }
    template<typename T, int r> swizzler4<T> row(mat<T, r, 4> &m, const int row) { return swizzler4<T>(m[row][0], m[row][1], m[row][2], m[row][3]); }
    template<typename T, int c> vec<T, 2> col(const mat<T, 2, c> &m, const int col) { return vec<T, 2>(m[0][col], m[1][col]); }
    template<typename T, int c> vec<T, 3> col(const mat<T, 3, c> &m, const int col) { return vec<T, 3>(m[0][col], m[1][col], m[2][col]); }
    template<typename T, int c> vec<T, 4> col(const mat<T, 4, c> &m, const int col) { return vec<T, 4>(m[0][col], m[1][col], m[2][col], m[3][col]); }
    template<typename T, int c> swizzler2<T> col(mat<T, 2, c> &m, const int col) { return swizzler2<T>(m[0][col], m[1][col]); }
    template<typename T, int c> swizzler3<T> col(mat<T, 3, c> &m, const int col) { return swizzler3<T>(m[0][col], m[1][col], m[2][col]); }
    template<typename T, int c> swizzler4<T> col(mat<T, 4, c> &m, const int col) { return swizzler4<T>(m[0][col], m[1][col], m[2][col], m[3][col]); }

    template<typename T, int r, int c> mat<T, r - 1, c - 1> strike(const mat<T, r, c> &m, const int row, const int col) { return m._strike(row, col); }

    template<typename T, int r, int c, int rs, int cs> mat<T, r, c> set(
	    const mat<T, r, c> &m, const mat<T, rs, cs> &s, const int row = 0, const int col = 0)
    { 
	mat<T, r, c> result(m);
	result._setSubArray(s, row, col); 
	return result;
    }

    template<typename T, int r, int c> mat<T, r, c> sin(const mat<T, r, c> &v) { return v._sin(); }
    template<typename T, int r, int c> mat<T, r, c> cos(const mat<T, r, c> &v) { return v._cos(); }
    template<typename T, int r, int c> mat<T, r, c> tan(const mat<T, r, c> &v) { return v._tan(); }
    template<typename T, int r, int c> mat<T, r, c> asin(const mat<T, r, c> &v) { return v._asin(); }
    template<typename T, int r, int c> mat<T, r, c> acos(const mat<T, r, c> &v) { return v._acos(); }
    template<typename T, int r, int c> mat<T, r, c> atan(const mat<T, r, c> &v) { return v._atan(); }
    template<typename T, int r, int c> mat<T, r, c> atan(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._atan(w); }
    template<typename T, int r, int c> mat<T, r, c> radians(const mat<T, r, c> &v) { return v._radians(); }
    template<typename T, int r, int c> mat<T, r, c> degrees(const mat<T, r, c> &v) { return v._degrees(); }

    template<typename T, int r, int c> mat<T, r, c> pow(const mat<T, r, c> &v, const T p) { return v._pow(p); }
    template<typename T, int r, int c> mat<T, r, c> exp(const mat<T, r, c> &v) { return v._exp(); }
    template<typename T, int r, int c> mat<T, r, c> exp2(const mat<T, r, c> &v) { return v._exp2(); }
    template<typename T, int r, int c> mat<T, r, c> log(const mat<T, r, c> &v) { return v._log(); }
    template<typename T, int r, int c> mat<T, r, c> log2(const mat<T, r, c> &v) { return v._log2(); }
    template<typename T, int r, int c> mat<T, r, c> log10(const mat<T, r, c> &v) { return v._log10(); }
    template<typename T, int r, int c> mat<T, r, c> sqrt(const mat<T, r, c> &v) { return v._sqrt(); }
    template<typename T, int r, int c> mat<T, r, c> inversesqrt(const mat<T, r, c> &v) { return v._inversesqrt(); }
    template<typename T, int r, int c> mat<T, r, c> cbrt(const mat<T, r, c> &v) { return v._cbrt(); }

    template<typename T, int r, int c> mat<bool, r, c> isfinite(const mat<T, r, c> &v) { return v._isfinite(); }
    template<typename T, int r, int c> mat<bool, r, c> isinf(const mat<T, r, c> &v) { return v._isinf(); }
    template<typename T, int r, int c> mat<bool, r, c> isnan(const mat<T, r, c> &v) { return v._isnan(); }
    template<typename T, int r, int c> mat<bool, r, c> isnormal(const mat<T, r, c> &v) { return v._isnormal(); }

    template<typename T, int r, int c> mat<T, r, c> abs(const mat<T, r, c> &v) { return v._abs(); }
    template<typename T, int r, int c> mat<T, r, c> sign(const mat<T, r, c> &v) { return v._sign(); }
    template<typename T, int r, int c> mat<T, r, c> floor(const mat<T, r, c> &v) { return v._floor(); }
    template<typename T, int r, int c> mat<T, r, c> ceil(const mat<T, r, c> &v) { return v._ceil(); }
    template<typename T, int r, int c> mat<T, r, c> round(const mat<T, r, c> &v) { return v._round(); }
    template<typename T, int r, int c> mat<T, r, c> fract(const mat<T, r, c> &v) { return v._fract(); }

    template<typename T, int r, int c> mat<T, r, c> min(const mat<T, r, c> &v, const T x) { return v._min(x); }
    template<typename T, int r, int c> mat<T, r, c> min(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._min(w); }
    template<typename T, int r, int c> mat<T, r, c> max(const mat<T, r, c> &v, const T x) { return v._max(x); }
    template<typename T, int r, int c> mat<T, r, c> max(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._max(w); }

    template<typename T, int r, int c> mat<T, r, c> clamp(const mat<T, r, c> &v, const T a, const T b) { return v._clamp(a, b); }
    template<typename T, int r, int c> mat<T, r, c> clamp(const mat<T, r, c> &v, const mat<T, r, c> &a, const mat<T, r, c> &b) { return v._clamp(a, b); }
    template<typename T, int r, int c> mat<T, r, c> mix(const mat<T, r, c> &v, const mat<T, r, c> &w, const T a) { return v._mix(w, a); }
    template<typename T, int r, int c> mat<T, r, c> mix(const mat<T, r, c> &v, const mat<T, r, c> &w, const mat<T, r, c> &a) { return v._mix(w, a); }
    template<typename T, int r, int c> mat<T, r, c> step(const mat<T, r, c> &v, const T a) { return v._step(a); }
    template<typename T, int r, int c> mat<T, r, c> step(const mat<T, r, c> &v, const mat<T, r, c> &a) { return v._step(a); }
    template<typename T, int r, int c> mat<T, r, c> smoothstep(const mat<T, r, c> &v, const T e0, const T e1) { return v._smoothstep(e0, e1); }
    template<typename T, int r, int c> mat<T, r, c> smoothstep(const mat<T, r, c> &v, const vec<T, 1> &e0, const vec<T, 1> &e1) { return v._smoothstep(e0, e1); }
    template<typename T, int r, int c> mat<T, r, c> mod(const mat<T, r, c> &v, const T x) { return v._mod(x); }
    template<typename T, int r, int c> mat<T, r, c> mod(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._mod(w); }

    template<typename T, int r, int c> mat<bool, r, c> greaterThan(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._greaterThan(w); }
    template<typename T, int r, int c> mat<bool, r, c> greaterThanEqual(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._greaterThanEqual(w); }
    template<typename T, int r, int c> mat<bool, r, c> lessThan(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._lessThan(w); }
    template<typename T, int r, int c> mat<bool, r, c> lessThanEqual(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._lessThanEqual(w); }
    template<typename T, int r, int c> mat<bool, r, c> equal(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._equal(w); }
    template<typename T, int r, int c> mat<bool, r, c> equal(const mat<T, r, c> &v, const mat<T, r, c> &w, const int max_ulps) { return v._equal(w, max_ulps); }
    template<typename T, int r, int c> mat<bool, r, c> notEqual(const mat<T, r, c> &v, const mat<T, r, c> &w) { return v._notEqual(w); }
    template<typename T, int r, int c> mat<bool, r, c> notEqual(const mat<T, r, c> &v, const mat<T, r, c> &w, const int max_ulps) { return v._notEqual(w, max_ulps); }
    template<int r, int c> bool any(const mat<bool, r, c> &v) { return v._any(); }
    template<int r, int c> bool all(const mat<bool, r, c> &v) { return v._all(); }
    template<int r, int c> bool negate(const mat<bool, r, c> &v) { return v._negate(); }

    template<typename T, int r, int c> mat<T, c, r> transpose(const mat<T, r, c> &m) { return m._transpose(); }

    template<typename T, int r, int c> mat<T, r, c> matrixCompMult(const mat<T, r, c> &m, const mat<T, r, c> &n) { return m._op_comp_mult(n); }
    
    template<typename T, int r, int c> mat<T, r, c> outerProduct(const vec<T, r> &v, const vec<T, c> &w)
    {
	mat<T, r, c> m;
	for (int i = 0; i < r; i++)
	    for (int j = 0; j < c; j++)
		m[i][j] = v[i] * w[j];
	return m;
    }

    template<typename T> T det(const mat<T, 2, 2> &m)
    {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }

    template<typename T> bool invertible(const mat<T, 2, 2> &m, const T epsilon = std::numeric_limits<T>::epsilon())
    {
	const T d = det(m);
    	return (d > epsilon || d < -epsilon);
    }

    template<typename T> mat<T, 2, 2> invert(const mat<T, 2, 2> &m)
    {
	return mat<T, 2, 2>(m[1][1], -m[0][1], -m[1][0], m[0][0]) / det(m);
    }

    template<typename T> T det(const mat<T, 3, 3> &m)
    {
	return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) 
    	    + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) 
	    + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    template<typename T> bool invertible(const mat<T, 3, 3> &m, const T epsilon = std::numeric_limits<T>::epsilon())
    {
	const T d = det(m);
    	return (d > epsilon || d < -epsilon);
    }

    template<typename T> mat<T, 3, 3> invert(const mat<T, 3, 3> &m)
    {
	// Using cofactors; see 
    	// http://en.wikipedia.org/wiki/Invertible_matrix
	// http://en.wikipedia.org/wiki/Minor_(linear_algebra)#Cofactors
	mat<T, 3, 3> I;
	I.v[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	I.v[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
	I.v[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
	I.v[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	I.v[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
	I.v[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
	I.v[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	I.v[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
	I.v[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	const T det = m[0][0] * I.v[0][0] + m[0][1] * I.v[1][0] + m[0][2] * I.v[2][0];
	return I / det;
    }

    template<typename T> T det(const mat<T, 4, 4> &m)
    {
	const T d0 = m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) 
	    + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) 
	    + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]);
	const T d1 = m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) 
	    + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) 
	    + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]);
	const T d2 = m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) 
	    + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) 
	    + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
	const T d3 = m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) 
	    + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) 
	    + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
	return m[0][0] * d0 - m[0][1] * d1 + m[0][2] * d2 - m[0][3] * d3;
    }

    template<typename T> bool invertible(const mat<T, 4, 4> &m, const T epsilon = std::numeric_limits<T>::epsilon())
    {
	const T d = det(m);
    	return (d > epsilon || d < -epsilon);
    }

    template<typename T> mat<T, 4, 4> invert(const mat<T, 4, 4> &m)
    {
	// Using cofactors; see 
    	// http://en.wikipedia.org/wiki/Invertible_matrix
	// http://en.wikipedia.org/wiki/Minor_(linear_algebra)#Cofactors

    	/* This code was adapted from Equalizer-0.5.0,
	 * src/externals/vmmlib/matrix4.h:
	 *
	 * VMMLib - _vector & _matrix Math Lib
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

	mat<T, 4, 4> result;

    	/* first set of 2x2 determinants: 12 multiplications, 6 additions */
	const T t1[6] = 
	{ 
	    m[0][2] * m[1][3] - m[1][2] * m[0][3],
	    m[0][2] * m[2][3] - m[2][2] * m[0][3],
	    m[0][2] * m[3][3] - m[3][2] * m[0][3],
	    m[1][2] * m[2][3] - m[2][2] * m[1][3],
	    m[1][2] * m[3][3] - m[3][2] * m[1][3],
	    m[2][2] * m[3][3] - m[3][2] * m[2][3] 
	};

    	/* first half of co_matrix: 24 multiplications, 16 additions */
	result[0][0] = m[1][1] * t1[5] - m[2][1] * t1[4] + m[3][1] * t1[3];
	result[0][1] = m[2][1] * t1[2] - m[3][1] * t1[1] - m[0][1] * t1[5];
	result[0][2] = m[3][1] * t1[0] - m[1][1] * t1[2] + m[0][1] * t1[4];
	result[0][3] = m[1][1] * t1[1] - m[0][1] * t1[3] - m[2][1] * t1[0];
	result[1][0] = m[2][0] * t1[4] - m[1][0] * t1[5] - m[3][0] * t1[3];
	result[1][1] = m[0][0] * t1[5] - m[2][0] * t1[2] + m[3][0] * t1[1];
	result[1][2] = m[1][0] * t1[2] - m[3][0] * t1[0] - m[0][0] * t1[4];
	result[1][3] = m[0][0] * t1[3] - m[1][0] * t1[1] + m[2][0] * t1[0];

    	/* second set of 2x2 determinants: 12 multiplications, 6 additions */
	const T t2[6] = 
	{
	    m[0][0] * m[1][1] - m[1][0] * m[0][1],
	    m[0][0] * m[2][1] - m[2][0] * m[0][1],
	    m[0][0] * m[3][1] - m[3][0] * m[0][1],
	    m[1][0] * m[2][1] - m[2][0] * m[1][1],
	    m[1][0] * m[3][1] - m[3][0] * m[1][1],
	    m[2][0] * m[3][1] - m[3][0] * m[2][1] 
	};

    	/* second half of co_matrix: 24 multiplications, 16 additions */
	result[2][0] = m[1][3] * t2[5] - m[2][3] * t2[4] + m[3][3] * t2[3];
	result[2][1] = m[2][3] * t2[2] - m[3][3] * t2[1] - m[0][3] * t2[5];
	result[2][2] = m[3][3] * t2[0] - m[1][3] * t2[2] + m[0][3] * t2[4];
	result[2][3] = m[1][3] * t2[1] - m[0][3] * t2[3] - m[2][3] * t2[0];
	result[3][0] = m[2][2] * t2[4] - m[1][2] * t2[5] - m[3][2] * t2[3];
	result[3][1] = m[0][2] * t2[5] - m[2][2] * t2[2] + m[3][2] * t2[1];
	result[3][2] = m[1][2] * t2[2] - m[3][2] * t2[0] - m[0][2] * t2[4];
	result[3][3] = m[0][2] * t2[3] - m[1][2] * t2[1] + m[2][2] * t2[0];

    	/* determinant: 4 multiplications, 3 additions */
	const T determinant =
	    m[0][0] * result[0][0] + m[1][0] * result[0][1] +
	    m[2][0] * result[0][2] + m[3][0] * result[0][3];

    	/* division: 16 multiplications, 1 division */
    	return result / determinant;
    }

    template<typename T> vec<T, 3> translation(const mat<T, 4, 4> &m)
    {
	return vec<T, 3>(m[0][3], m[1][3], m[2][3]);
    }

    template<typename T> swizzler3<T> translation(mat<T, 4, 4> &m)
    {
	return swizzler3<T>(m[0][3], m[1][3], m[2][3]);
    }

    template<typename T> mat<T, 4, 4> translate(const mat<T, 4, 4> &m, const vec<T, 3> &v)
    {
	vec<T, 4> t(v, static_cast<T>(1));
	mat<T, 4, 4> r;
	r[0][0] = m[0][0];
	r[0][1] = m[0][1];
	r[0][2] = m[0][2];
	r[0][3] = dot(t, row(m, 0));
	r[1][0] = m[1][0];
	r[1][1] = m[1][1];
	r[1][2] = m[1][2];
	r[1][3] = dot(t, row(m, 1));
	r[2][0] = m[2][0];
	r[2][1] = m[2][1];
	r[2][2] = m[2][2];
	r[2][3] = dot(t, row(m, 2));
	r[3][0] = m[2][0];
	r[3][1] = m[2][1];
	r[3][2] = m[2][2];
	r[3][3] = dot(t, row(m, 3));
	return r;
    }

    template<typename T> mat<T, 4, 4> scale(const mat<T, 4, 4> &m, const vec<T, 3> &v)
    {
	mat<T, 4, 4> r;
	col(r, 0) = col(m, 0) * v.x();
	col(r, 1) = col(m, 1) * v.y();
	col(r, 2) = col(m, 2) * v.z();
	col(r, 3) = col(m, 3);
	return r;
    }

    typedef mat<float, 2, 2> mat2;
    typedef mat<double, 2, 2> dmat2;
    typedef mat<float, 3, 3> mat3;
    typedef mat<double, 3, 3> dmat3;
    typedef mat<float, 4, 4> mat4;
    typedef mat<double, 4, 4> dmat4;
    typedef mat<float, 3, 2> mat2x3;
    typedef mat<double, 3, 2> dmat2x3;
    typedef mat<float, 2, 3> mat3x2;
    typedef mat<double, 2, 3> dmat3x2;
    typedef mat<float, 4, 2> mat2x4;
    typedef mat<double, 4, 2> dmat2x4;
    typedef mat<float, 2, 4> mat4x2;
    typedef mat<double, 2, 4> dmat4x2;
    typedef mat<float, 4, 3> mat3x4;
    typedef mat<double, 4, 3> dmat3x4;
    typedef mat<float, 3, 4> mat4x3;
    typedef mat<double, 3, 4> dmat4x3;

    template<typename T>
    class quaternion : public array<T, 4, 1>
    {
	public:

	/* Constructors, Destructor */

	quaternion() {}
	quaternion(const array<T, 4, 1> &a) : array<T, 4, 1>(a) {}
	quaternion(const vec<T, 4> &a) : array<T, 4, 1>(a) {}
	quaternion(const T *v) : array<T, 4, 1>(v) {}
	quaternion(const std::string &s) : array<T, 4, 1>(s) {}
	template<typename U>
	quaternion(const quaternion<U> &q) : array<T, 4, 1>(q) {}
	quaternion(const T w, const T x, const T y, const T z) : array<T, 4, 1>(w, x, y, z) {}

	~quaternion() {}

	/* Get / set components */

	T &w()
	{
	    return array<T, 4, 1>::vl[0];
	}
	
	const T &w() const
	{
	    return array<T, 4, 1>::vl[0];
   	}

	T &x()
	{
	    return array<T, 4, 1>::vl[1];
	}
	
	const T &x() const
	{
	    return array<T, 4, 1>::vl[1];
   	}

	T &y()
	{
	    return array<T, 4, 1>::vl[2];
	}
	
	const T &y() const
	{
	    return array<T, 4, 1>::vl[2];
   	}

	T &z()
	{
	    return array<T, 4, 1>::vl[3];
	}
	
	const T &z() const
	{
	    return array<T, 4, 1>::vl[3];
   	}

	vec<T, 3> axis() const
	{
	    T cos_a = this->w();
	    T sin_a = glvm::sqrt(static_cast<T>(1) - cos_a * cos_a);
	    if (glvm::abs(sin_a) < static_cast<T>(0.0005))
	    {
		sin_a = static_cast<T>(1);
	    }
	    return vec<T, 3>(this->x() / sin_a, this->y() / sin_a, this->z() / sin_a);
	}

	T angle() const
	{
	    return glvm::acos(this->w()) * static_cast<T>(2);
	}

	quaternion operator+() const
	{
	    return *this;
	}

	quaternion operator-() const
	{
	    return conjugate(*this);
	}
	
	quaternion operator*(const quaternion &q) const
	{
	    quaternion p;
	    p.w() = w() * q.w() - x() * q.x() - y() * q.y() - z() * q.z();
	    p.x() = w() * q.x() + x() * q.w() + y() * q.z() - z() * q.y();
	    p.y() = w() * q.y() + y() * q.w() + z() * q.x() - x() * q.z();
	    p.z() = w() * q.z() + z() * q.w() + x() * q.y() - y() * q.x();
	    return p;
	}

	const quaternion &operator*=(const quaternion &q)
	{
	    *this = *this * q;
	    return *this;
	}

	vec<T, 3> operator*(const vec<T, 3> &v) const
	{
	    return toMat3(*this) * v;
	}

	vec<T, 4> operator*(const vec<T, 4> &v) const
	{
	    return toMat4(*this) * v;
	}

    };

    template<typename T> T magnitude(const quaternion<T> &q) { return q._length(); }
    template<typename T> quaternion<T> conjugate(const quaternion<T> &q) { return quaternion<T>(q.w(), -q.x(), -q.y(), -q.z()); }
    template<typename T> quaternion<T> inverse(const quaternion<T> &q) { return conjugate(q)._op_scal_div(magnitude(q)); }
    template<typename T> quaternion<T> normalize(const quaternion<T> &q) { return q._op_scal_div(magnitude(q)); }

    typedef quaternion<float> quat;
    typedef quaternion<double> dquat;


    template<typename T>
    class frustum : public array<T, 6, 1>
    {
	public:

	/* Constructors, Destructor */

	frustum() {}

	frustum(const array<T, 6, 1> &a) : array<T, 6, 1>(a) {}
	frustum(const vec<T, 6> &a) : array<T, 6, 1>(a) {}
	frustum(const T *v) : array<T, 6, 1>(v) {}
	frustum(const std::string &s) : array<T, 6, 1>(s) {}
	template<typename U>
	frustum(const frustum<U> &q) : array<T, 6, 1>(q) {}
	frustum(const T l, const T r, const T b, const T t, const T n, const T f) : array<T, 6, 1>(l, r, b, t, n ,f) {}
	~frustum() {}

	/* Get / set components */

	const T &l() const { return array<T, 6, 1>::vl[0]; }
	const T &r() const { return array<T, 6, 1>::vl[1]; }
	const T &b() const { return array<T, 6, 1>::vl[2]; }
	const T &t() const { return array<T, 6, 1>::vl[3]; }
	const T &n() const { return array<T, 6, 1>::vl[4]; }
	const T &f() const { return array<T, 6, 1>::vl[5]; }
	T &l() { return array<T, 6, 1>::vl[0]; }
	T &r() { return array<T, 6, 1>::vl[1]; }
	T &b() { return array<T, 6, 1>::vl[2]; }
	T &t() { return array<T, 6, 1>::vl[3]; }
	T &n() { return array<T, 6, 1>::vl[4]; }
	T &f() { return array<T, 6, 1>::vl[5]; }

	/* Frustum operations */

	void adjustNear(const T new_near)
	{
	    const T q = new_near / n();
	    l() *= q;
   	    r() *= q;
	    b() *= q;
	    t() *= q;
	    n() = new_near;
	}
    };

    template<typename T> mat<T, 4, 4> mat4Frustum(const frustum<T> &f)
    {
	mat<T, 4, 4> m;
	m[0][0] = static_cast<T>(2) * f.n() / (f.r() - f.l());
	m[0][1] = static_cast<T>(0);
	m[0][2] = (f.r() + f.l()) / (f.r() - f.l());
    	m[0][3] = static_cast<T>(0);
	m[1][0] = static_cast<T>(0);
	m[1][1] = static_cast<T>(2) * f.n() / (f.t() - f.b());
	m[1][2] = (f.t() + f.b()) / (f.t() - f.b());
	m[1][3] = static_cast<T>(0);
	m[2][0] = static_cast<T>(0);
	m[2][1] = static_cast<T>(0);
	m[2][2] = - (f.f() + f.n()) / (f.f() - f.n());
	m[2][3] = - static_cast<T>(2) * f.f() * f.n() / (f.f() - f.n());
	m[3][0] = static_cast<T>(0);
	m[3][1] = static_cast<T>(0);
	m[3][2] = static_cast<T>(-1);
	m[3][3] = static_cast<T>(0);
	return m;
    }

    typedef frustum<float> frust;
    typedef frustum<double> dfrust;


    /* Conversions between different representations of rotations:
     * from angle/axis, euler angles, mat3 to quat
     * from angle/axis, euler angles, quat to mat3
     * from angle/axis, euler angles, quat to mat4
     * from angle/axis, mat3, quat to euler angles
     * TODO: Conversions from euler angles, mat3, quat to angle/axis.
     */

    template<typename T> quaternion<T> toQuat(const T angle, const vec<T, 3> &axis)
    {
	vec<T, 3> naxis = normalize(axis);
	T sin_a = glvm::sin(angle / static_cast<T>(2));
	T cos_a = glvm::cos(angle / static_cast<T>(2));
	return quaternion<T>(cos_a, naxis.x() * sin_a, naxis.y() * sin_a, naxis.z() * sin_a);
    }

    template<typename T> quaternion<T> toQuat(const vec<T, 3> &euler_rot)
    {
	// quaternion<T> qx, qy, qz;
	// qx.from_axis_angle(vec<T, 3>(1, 0, 0), rx)
	// qy.from_axis_angle(vec<T, 3>(1, 0, 0), ry)
	// qz.from_axis_angle(vec<T, 3>(1, 0, 0), rz)
	quaternion<T> qx(glvm::cos(euler_rot.x() / static_cast<T>(2)), glvm::sin(euler_rot.x() / static_cast<T>(2)), static_cast<T>(0), static_cast<T>(0));
	quaternion<T> qy(glvm::cos(euler_rot.y() / static_cast<T>(2)), static_cast<T>(0), glvm::sin(euler_rot.y() / static_cast<T>(2)), static_cast<T>(0));
	quaternion<T> qz(glvm::cos(euler_rot.z() / static_cast<T>(2)), static_cast<T>(0), static_cast<T>(0), glvm::sin(euler_rot.z() / static_cast<T>(2)));
	return qx * qy * qz;
	// TODO: Does this need to be optimized by cancelling out the
	// multiplications with zero? Or is the compiler smart enough to do this
	// for us?
    }

    template<typename T> quaternion<T> toQuat(const mat<T, 3, 3> &rot_mat)
    {
	quaternion<T> q;
	// From "Matrix and Quaternion FAQ", Q55
	T t = static_cast<T>(1) + rot_mat[0][0] + rot_mat[1][1] + rot_mat[2][2];
	if (t > static_cast<T>(1e-8))
	{
	    const T s = sqrt(t) * static_cast<T>(2);
	    q.x() = (rot_mat[2][1] - rot_mat[1][2]) / s;
	    q.y() = (rot_mat[0][2] - rot_mat[2][0]) / s;
	    q.z() = (rot_mat[1][0] - rot_mat[0][1]) / s;
	    q.w() = s / static_cast<T>(4);
	}
	else if (rot_mat[0][0] > rot_mat[1][1] && rot_mat[0][0] > rot_mat[2][2])
	{
	    t = static_cast<T>(1) + rot_mat[0][0] - rot_mat[1][1] - rot_mat[2][2];
	    const T s = sqrt(t) * static_cast<T>(2);
	    q.x() = s / static_cast<T>(4);
	    q.y() = (rot_mat[1][0] + rot_mat[0][1]) / s;
	    q.z() = (rot_mat[0][2] + rot_mat[2][0]) / s;
	    q.w() = (rot_mat[2][1] - rot_mat[1][2]) / s;
	} 
	else if (rot_mat[1][1] > rot_mat[2][2])
	{
	    t = static_cast<T>(1) + rot_mat[1][1] - rot_mat[0][0] - rot_mat[2][2];
	    const T s = sqrt(t) * static_cast<T>(2);
	    q.x() = (rot_mat[1][0] + rot_mat[0][1]) / s;
	    q.y() = s / static_cast<T>(4);
	    q.z() = (rot_mat[2][1] + rot_mat[1][2]) / s;
	    q.w() = (rot_mat[0][2] - rot_mat[2][0]) / s;
	}
	else
	{
	    t = static_cast<T>(1) + rot_mat[2][2] - rot_mat[0][0] - rot_mat[1][1];
	    const T s = sqrt(t) * static_cast<T>(2);
	    q.x() = (rot_mat[0][2] + rot_mat[2][0]) / s;
	    q.y() = (rot_mat[2][1] + rot_mat[1][2]) / s;
	    q.z() = s / static_cast<T>(4);
	    q.w() = (rot_mat[1][0] - rot_mat[0][1]) / s;
	}
    }

    template<typename T> mat<T, 3, 3> toMat3(const T angle, const vec<T, 3> &axis)
    {
	const vec<T, 3> n = normalize(axis);
	const T c = glvm::cos(angle);
	const T s = glvm::sin(angle);
	const T mc = static_cast<T>(1) - c;
	mat<T, 3, 3> m;
	m[0][0] = n.x() * n.x() * mc + c;
	m[0][1] = n.x() * n.y() * mc - n.z() * s;
	m[0][2] = n.x() * n.z() * mc + n.y() * s;
	m[1][0] = n.y() * n.x() * mc + n.z() * s;
	m[1][1] = n.y() * n.y() * mc + c;
	m[1][2] = n.y() * n.z() * mc - n.x() * s;
	m[2][0] = n.x() * n.z() * mc - n.y() * s;
	m[2][1] = n.y() * n.z() * mc + n.x() * s;
	m[2][2] = n.z() * n.z() * mc + c;
	return m;
    }

    template<typename T> mat<T, 3, 3> toMat3(const vec<T, 3> &euler_rot)
    {
	return toMat3(toQuat(euler_rot));
    }

    template<typename T> mat<T, 3, 3> toMat3(const quaternion<T> &q)
    {
	mat<T, 3, 3> m;
	const T xx = q.x() * q.x();
    	const T xy = q.x() * q.y();
	const T xz = q.x() * q.z();
	const T xw = q.x() * q.w();
	const T yy = q.y() * q.y();
	const T yz = q.y() * q.z();
	const T yw = q.y() * q.w();
	const T zz = q.z() * q.z();
	const T zw = q.z() * q.w();
	m[0][0] = static_cast<T>(1) - static_cast<T>(2) * (yy + zz);
	m[0][1] = static_cast<T>(2) * (xy - zw);
	m[0][2] = static_cast<T>(2) * (xz + yw);
	m[1][0] = static_cast<T>(2) * (xy + zw);
	m[1][1] = static_cast<T>(1) - static_cast<T>(2) * (xx + zz);
	m[1][2] = static_cast<T>(2) * (yz - xw);
	m[2][0] = static_cast<T>(2) * (xz - yw);
	m[2][1] = static_cast<T>(2) * (yz + xw);
	m[2][2] = static_cast<T>(1) - static_cast<T>(2) * (xx + yy);
	return m;
    }

    template<typename T> mat<T, 4, 4> toMat4(const T angle, const vec<T, 3> &axis)
    {
	mat<T, 4, 4> m;
	m._setSubArray(mat3(angle, axis));
	m[0][3] = static_cast<T>(0);
	m[1][3] = static_cast<T>(0);
	m[2][3] = static_cast<T>(0);
	m[3][0] = static_cast<T>(0);
	m[3][1] = static_cast<T>(0);
	m[3][2] = static_cast<T>(0);
	m[3][3] = static_cast<T>(1);
	return m;
    }

    template<typename T> mat<T, 4, 4> toMat4(const vec<T, 3> &euler_rot)
    {
	return toMat4(toQuat(euler_rot));
    }

    template<typename T> mat<T, 4, 4> toMat4(const quaternion<T> &q)
    {
	mat<T, 4, 4> m;
	m._setSubArray(toMat3(q));
	m[0][3] = static_cast<T>(0);
    	m[1][3] = static_cast<T>(0);
	m[2][3] = static_cast<T>(0);
	m[3][0] = static_cast<T>(0);
	m[3][1] = static_cast<T>(0);
	m[3][2] = static_cast<T>(0);
	m[3][3] = static_cast<T>(1);
	return m;
    }
    
    template<typename T> vec<T, 3> toEuler(const T angle, const vec<T, 3> &axis)
    {
	return toEuler(toQuat(angle, axis));
    }

    template<typename T> mat<T, 3, 3> toEuler(const mat<T, 3, 3> &rot_mat)
    {
	return toEuler(toQuat(rot_mat));
    }

    template<typename T> vec<T, 3> toEuler(const quaternion<T> &q)
    {
	return vec<T, 3>(
		glvm::atan(static_cast<T>(2) * (q.w() * q.x() + q.y() * q.z()) 
		    / (static_cast<T>(1) - static_cast<T>(2) * (q.x() * q.x() + q.y() * q.y()))),
		glvm::asin(static_cast<T>(2) * (q.w() * q.y() - q.x() * q.z())),
		glvm::atan(static_cast<T>(2) * (q.w() * q.z() + q.x() * q.y()) 
		    / (static_cast<T>(1) - static_cast<T>(2) * (q.y() * q.y() + q.z() * q.z()))));
    }
}

#endif
