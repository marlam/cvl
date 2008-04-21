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

#ifndef GLVM_H
#define GLVM_H

#include <cmath>
#include <cstring>


namespace glvm
{
    class bvec2
    {
	public:

	union
	{
	    struct
	    {
		bool x, y;
	    };
	    struct
	    {
		bool r, g;
	    };
	    struct
	    {
		bool s, t;
	    };
	    bool v[2];
	};

	/* Constructors, Destructor */

	bvec2() {}

	bvec2(bool x, bool y)
	{
	    this->x = x;
	    this->y = y;
	}

	bvec2(const bool *xy)
	{
	    this->x = xy[0];
	    this->y = xy[1];
	}

	~bvec2() {}

	/* Operators */

	const bvec2& operator=(const bvec2& v)
	{
	    x = v.x;
	    y = v.y;
	    return *this;
	}

	bool &operator[](size_t i)
	{
	    return v[i];
	}

	const bool &operator[](size_t i) const
	{
	    return v[i];
	}

	/* "Swizzling" (as far as possible in C++) */

	bvec2 xx() const
	{
	    return bvec2(x, x);
	}

	bvec2 xy() const
	{
	    return bvec2(x, y);
	}

	bvec2 yx() const
	{
	    return bvec2(y, x);
	}

	bvec2 yy() const
	{
	    return bvec2(y, y);
	}

	bvec2 rr() const
	{
	    return bvec2(r, r);
	}

	bvec2 rg() const
	{
	    return bvec2(r, g);
	}

	bvec2 gr() const
	{
	    return bvec2(g, r);
	}

	bvec2 gg() const
	{
	    return bvec2(g, g);
	}

	bvec2 ss() const
	{
	    return bvec2(s, s);
	}

	bvec2 st() const
	{
	    return bvec2(s, t);
	}

	bvec2 ts() const
	{
	    return bvec2(t, s);
	}

	bvec2 tt() const
	{
	    return bvec2(t, t);
	}

	/* Functions */

	bool any() const
	{
	    return x || y;
	}

	bool all() const
	{
	    return x && y;
	}

	bvec2 negate() const
	{
	    return bvec2(!x, !y);
	}
    };

    class vec2
    {
	public:

	union
	{
	    struct
	    {
		float x, y;
	    };
	    struct
	    {
		float r, g;
	    };
	    struct
	    {
		float s, t;
	    };
	    float v[2];
	};

	/* Constructors, Destructor */

	vec2() {}

	vec2(float x, float y)
	{
	    this->x = x;
	    this->y = y;
	}

	vec2(const float *xy)
	{
	    this->x = xy[0];
	    this->y = xy[1];
	}

	~vec2() {}

	/* "Swizzling" (as far as possible in C++) */

	vec2 xx() const
	{
	    return vec2(x, x);
	}

	vec2 xy() const
	{
	    return vec2(x, y);
	}

	vec2 yx() const
	{
	    return vec2(y, x);
	}

	vec2 yy() const
	{
	    return vec2(y, y);
	}

	vec2 rr() const
	{
	    return vec2(r, r);
	}

	vec2 rg() const
	{
	    return vec2(r, g);
	}

	vec2 gr() const
	{
	    return vec2(g, r);
	}

	vec2 gg() const
	{
	    return vec2(g, g);
	}

	vec2 ss() const
	{
	    return vec2(s, s);
	}

	vec2 st() const
	{
	    return vec2(s, t);
	}

	vec2 ts() const
	{
	    return vec2(t, s);
	}

	vec2 tt() const
	{
	    return vec2(t, t);
	}

	/* Operators */

	const vec2& operator=(const vec2& v)
	{
	    x = v.x;
	    y = v.y;
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return v[i];
	}

	const float &operator[](size_t i) const
	{
	    return v[i];
	}
	
	vec2 operator*(const float s) const
	{
	    return vec2(x * s, y * s);
	}

	vec2 operator/(const float s) const
	{
	    return vec2(x / s, y / s);
	}
     
	const vec2 &operator*=(float s)
	{
	    x *= s;
	    y *= s;
	    return *this;
	}
     
	const vec2 &operator/=(float s)
	{
	    x /= s;
	    y /= s;
	    return *this;
	}

	vec2 operator+(const vec2& v) const
	{
	    return vec2(x + v.x, y + v.y);
	}

	vec2 operator-(const vec2& v) const
	{
	    return vec2(x - v.x, y - v.y);
	}

	vec2 operator*(const vec2& v) const 
	{
	    return vec2(x * v.x, y * v.y);
	}

	vec2 operator/(const vec2& v) const 
	{
	    return vec2(x / v.x, y / v.y);
	}

	vec2 operator-() const
	{
	    return vec2(-x, -y);
	}

	const vec2& operator+=(const vec2& v)
	{
	    x += v.x;
	    y += v.y;
	    return *this;
	}

	const vec2& operator-=(const vec2& v)
	{
	    x -= v.x;
	    y -= v.y;
	    return *this;
	}

	const vec2& operator*=(const vec2& v)
	{
	    x *= v.x;
	    y *= v.y;
	    return *this;
	}

	const vec2& operator/=(const vec2& v)
	{
	    x /= v.x;
	    y /= v.y;
	    return *this;
	}

	/* Trigonometric functions */

	vec2 sin() const
	{
	    return vec2(sinf(x), sinf(y));
	}

	vec2 cos() const
	{
	    return vec2(cosf(x), cosf(y));
	}

	vec2 tan() const
	{
	    return vec2(tanf(x), tanf(y));
	}

	vec2 asin() const
	{
	    return vec2(asinf(x), asinf(y));
	}

	vec2 acos() const
	{
	    return vec2(acosf(x), acosf(y));
	}

	vec2 atan() const
	{
	    return vec2(atanf(x), atanf(y));
	}

	vec2 atan(const vec2 v) const
	{
	    return vec2(atan2f(x, v.x), atan2f(y, v.y));
	}

	vec2 radians() const
	{
	    return vec2(x * static_cast<float>(M_PI / 180.0f), y * static_cast<float>(M_PI / 180.0f));
	}

	vec2 degrees() const
	{
	    return vec2(x * 180.0f / static_cast<float>(M_PI), y * 180.0f / static_cast<float>(M_PI));
	}

	/* Exponential functions */

	vec2 pow(const float p) const
	{
	    return vec2(powf(x, p), powf(y, p));
	}

	vec2 exp() const
	{
	    return vec2(expf(x), expf(y));
	}

	vec2 log() const
	{
	    return vec2(logf(x), logf(y));
	}

	vec2 sqrt() const
	{
	    return vec2(sqrtf(x), sqrtf(y));
	}
	
	/* Common functions */

	vec2 abs() const
	{
	    return vec2(fabsf(x), fabsf(y));
	}

	vec2 sign() const
	{
	    return vec2(
		    x < 0.0f ? -1.0f : x > 0.0f ? +1.0f : 0.0f,
		    y < 0.0f ? -1.0f : y > 0.0f ? +1.0f : 0.0f);
	}

	vec2 floor() const
	{
	    return vec2(floorf(x), floorf(y));
	}

	vec2 ceil() const
	{
	    return vec2(ceilf(x), ceilf(y));
	}

	vec2 fract() const
	{
	    return *this - this->floor();
	}

	vec2 min(const float v) const
	{
	    return vec2(v < x ? v : x, v < y ? v : y);
	}

	vec2 min(const vec2 v) const
	{
	    return vec2(v.x < x ? v.x : x, v.y < y ? v.y : y);
	}

	vec2 max(const float v) const
	{
	    return vec2(v > x ? v : x, v > y ? v : y);
	}

	vec2 max(const vec2 v) const
	{
	    return vec2(v.x > x ? v.x : x, v.y > y ? v.y : y);
	}

	vec2 clamp(const float minval, const float maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec2 clamp(const vec2 minval, const vec2 maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec2 mix(const vec2 v, const float alpha) const
	{
	    return *this * (1.0f - alpha) + v * alpha;
	}

	vec2 mix(const vec2 v, const vec2 alpha) const
	{
	    return (vec2(1.0f, 1.0f) - alpha) * *this + alpha * v;
	}

	vec2 step(const float edge) const
	{
	    return vec2(x < edge ? 0.0f : 1.0f, y < edge ? 0.0f : 1.0f);
	}

	vec2 step(const vec2 edge) const
	{
	    return vec2(x < edge.x ? 0.0f : 1.0f, y < edge.y ? 0.0f : 1.0f);
	}

	vec2 smoothstep(const float edge0, const float edge1) const
	{
	    vec2 t = (*this - vec2(edge0, edge0)) / (vec2(edge1, edge1) - vec2(edge0, edge0));
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec2(3.0f, 3.0f) - t * 2.0f);
	}

	vec2 smoothstep(const vec2 edge0, const vec2 edge1) const
	{
	    vec2 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec2(3.0f, 3.0f) - t * 2.0f);
	}
	
	vec2 mod(const float y) const
	{
	    vec2 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}
	
	vec2 mod(const vec2 y) const
	{
	    vec2 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	/* Geometric functions */

	float length() const
	{
	    return sqrtf(x * x + y * y);
	}

	float distance(const vec2 v) const
	{
	    return vec2(*this - v).length();
	}

	float dot(const vec2 v) const
	{
	    return x * v.x + y * v.y;
	}

	vec2 normalize() const
	{
	    return *this / this->length();
	}

	vec2 faceforward(const vec2 I, const vec2 Nref) const
	{
	    return Nref.dot(I) < 0.0f ? *this : - *this;
	}

	vec2 reflect(const vec2 N) const
	{
	    return *this - N * (N.dot(*this) * 2.0f);
	}

	vec2 refract(const vec2 N, float eta) const
	{
	    float d = N.dot(*this);
	    float k = 1.0f - eta * eta * (1.0f - d * d);
	    return k < 0.0f ? vec2(0.0f, 0.0f) : *this * eta - N * (eta * d + sqrtf(k));
	}
	
	/* Comparion functions */

	bvec2 equal(const vec2 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return bvec2(
		    fabsf(x - v.x) < epsilon,
		    fabsf(y - v.y) < epsilon);
	}

	bvec2 notEqual(const vec2 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	bvec2 greaterThan(const vec2 v) const
	{
	    return bvec2(x > v.x, y > v.y);
	}

	bvec2 greaterThanEqual(const vec2 v) const
	{
	    return bvec2(x >= v.x, y >= v.y);
	}

	bvec2 lessThan(const vec2 v) const
	{
	    return bvec2(x < v.x, y < v.y);
	}

	bvec2 lessThanEqual(const vec2 v) const
	{
	    return bvec2(x <= v.x, y <= v.y);
	}
    };

    /* Trigonometric functions */

    vec2 sin(const vec2 v)
    {
	return v.sin();
    }

    vec2 cos(const vec2 v)
    {
	return v.cos();
    }

    vec2 tan(const vec2 v)
    {
	return v.tan();
    }

    vec2 asin(const vec2 v)
    {
	return v.asin();
    }

    vec2 acos(const vec2 v)
    {
	return v.acos();
    }

    vec2 atan(const vec2 v)
    {
	return v.atan();
    }

    vec2 atan(const vec2 v, const vec2 w)
    {

	return v.atan(w);
    }

    vec2 radians(const vec2 v)
    {
	return v.radians();
    }

    vec2 degrees(const vec2 v)
    {
	return v.degrees();
    }

    /* Exponential functions */

    vec2 pow(const vec2 v, const float p)
    {
	return v.pow(p);
    }

    vec2 exp(const vec2 v)
    {
	return v.exp();
    }

    vec2 log(const vec2 v)
    {
	return v.log();
    }

    vec2 sqrt(const vec2 v)
    {
	return v.sqrt();
    }

    /* Common functions */

    vec2 abs(const vec2 v)
    {
	return v.abs();
    }

    vec2 sign(const vec2 v)
    {
	return v.sign();
    }

    vec2 floor(const vec2 v)
    {
	return v.floor();
    }

    vec2 ceil(const vec2 v)
    {
	return v.ceil();
    }

    vec2 fract(const vec2 v)
    {
	return v.fract();
    }

    vec2 min(const vec2 v, const float w)
    {
	return v.min(w);
    }

    vec2 min(const vec2 v, const vec2 w)
    {
	return v.min(w);
    }

    vec2 max(const vec2 v, const float w)
    {
	return v.max(w);
    }

    vec2 max(const vec2 v, const vec2 w)
    {
	return v.max(w);
    }

    vec2 clamp(const vec2 v, const float minval, const float maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec2 clamp(const vec2 v, const vec2 minval, const vec2 maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec2 mix(const vec2 v, const vec2 w, const float alpha)
    {
	return v.mix(w, alpha);
    }

    vec2 mix(const vec2 v, const vec2 w, const vec2 alpha)
    {
	return v.mix(w, alpha);
    }

    vec2 step(const vec2 v, const float edge)
    {
	return v.step(edge);
    }

    vec2 step(const vec2 v, const vec2 edge)
    {
	return v.step(edge);
    }

    vec2 smoothstep(const vec2 v, const float edge0, const float edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec2 smoothstep(const vec2 v, const vec2 edge0, const vec2 edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec2 mod(const vec2 v, const float y)
    {
	return v.mod(y);
    }

    vec2 mod(const vec2 v, const vec2 y)
    {
	return v.mod(y);
    }

    /* Geometric functions */

    float length(const vec2 v)
    {
	return v.length();
    }

    float distance(const vec2 v, const vec2 w)
    {
	return v.distance(w);
    }

    float dot(const vec2 v, const vec2 w)
    {
	return v.dot(w);
    }

    vec2 normalize(const vec2 v)
    {
	return v.normalize();
    }

    vec2 faceforward(const vec2 N, const vec2 I, const vec2 Nref)
    {
	return N.faceforward(I, Nref);
    }

    vec2 reflect(const vec2 I, const vec2 N)
    {
	return I.reflect(N);
    }

    vec2 refract(const vec2 I, const vec2 N, float eta)
    {
	return I.refract(N, eta);
    }

    /* Comparion functions */

    bvec2 equal(const vec2 v, const vec2 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.equal(w, epsilon);
    }

    bvec2 notEqual(const vec2 v, const vec2 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.notEqual(w, epsilon);
    }

    bvec2 greaterThan(const vec2 v, const vec2 w)
    {
	return v.greaterThan(w);
    }

    bvec2 greaterThanEqual(const vec2 v, const vec2 w)
    {
	return v.greaterThanEqual(w);
    }

    bvec2 lessThan(const vec2 v, const vec2 w)
    {
	return v.lessThan(w);
    }

    bvec2 lessThanEqual(const vec2 v, const vec2 w)
    {
	return v.lessThanEqual(w);
    }

    class bvec3
    {
	public:

	union
	{
	    struct
	    {
		bool x, y, z;
	    };
	    struct
	    {
		bool r, g, b;
	    };
	    struct
	    {
		bool s, t, p;
	    };
	    bool v[3];
	};

	/* Constructors, Destructor */

	bvec3() {}

	bvec3(bool x, bool y, bool z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	bvec3(const bvec2 xy, bool z)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	}

	bvec3(bool x, const bvec2 yz)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	}

	bvec3(const bool *xyz)
	{
	    this->x = xyz[0];
	    this->y = xyz[1];
	    this->z = xyz[2];
	}

	~bvec3() {}

	/* Operators */

	const bvec3& operator=(const bvec3& v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    return *this;
	}

	bool &operator[](size_t i)
	{
	    return v[i];
	}

	const bool &operator[](size_t i) const
	{
	    return v[i];
	}

	/* "Swizzling" (as far as possible in C++) */

	bvec2 xx() const
	{
	    return bvec2(x, x);
	}

	bvec2 xy() const
	{
	    return bvec2(x, y);
	}

	bvec2 xz() const
	{
	    return bvec2(x, z);
	}

	bvec2 yx() const
	{
	    return bvec2(y, x);
	}

	bvec2 yy() const
	{
	    return bvec2(y, y);
	}

	bvec2 yz() const
	{
	    return bvec2(y, z);
	}

	bvec2 zx() const
	{
	    return bvec2(z, x);
	}

	bvec2 zy() const
	{
	    return bvec2(z, y);
	}

	bvec2 zz() const
	{
	    return bvec2(z, z);
	}

	bvec3 xxx() const
	{
	    return bvec3(x, x, x);
	}

	bvec3 xxy() const
	{
	    return bvec3(x, x, y);
	}

	bvec3 xxz() const
	{
	    return bvec3(x, x, z);
	}

	bvec3 xyx() const
	{
	    return bvec3(x, y, x);
	}

	bvec3 xyy() const
	{
	    return bvec3(x, y, y);
	}

	bvec3 xyz() const
	{
	    return bvec3(x, y, z);
	}

	bvec3 xzx() const
	{
	    return bvec3(x, z, x);
	}

	bvec3 xzy() const
	{
	    return bvec3(x, z, y);
	}

	bvec3 xzz() const
	{
	    return bvec3(x, z, z);
	}

	bvec3 yxx() const
	{
	    return bvec3(y, x, x);
	}

	bvec3 yxy() const
	{
	    return bvec3(y, x, y);
	}

	bvec3 yxz() const
	{
	    return bvec3(y, x, z);
	}

	bvec3 yyx() const
	{
	    return bvec3(y, y, x);
	}

	bvec3 yyy() const
	{
	    return bvec3(y, y, y);
	}

	bvec3 yyz() const
	{
	    return bvec3(y, y, z);
	}

	bvec3 yzx() const
	{
	    return bvec3(y, z, x);
	}

	bvec3 yzy() const
	{
	    return bvec3(y, z, y);
	}

	bvec3 yzz() const
	{
	    return bvec3(y, z, z);
	}

	bvec3 zxx() const
	{
	    return bvec3(z, x, x);
	}

	bvec3 zxy() const
	{
	    return bvec3(z, x, y);
	}

	bvec3 zxz() const
	{
	    return bvec3(z, x, z);
	}

	bvec3 zyx() const
	{
	    return bvec3(z, y, x);
	}

	bvec3 zyy() const
	{
	    return bvec3(z, y, y);
	}

	bvec3 zyz() const
	{
	    return bvec3(z, y, z);
	}

	bvec3 zzx() const
	{
	    return bvec3(z, z, x);
	}

	bvec3 zzy() const
	{
	    return bvec3(z, z, y);
	}

	bvec3 zzz() const
	{
	    return bvec3(z, z, z);
	}

	bvec2 rr() const
	{
	    return bvec2(r, r);
	}

	bvec2 rg() const
	{
	    return bvec2(r, g);
	}

	bvec2 rb() const
	{
	    return bvec2(r, b);
	}

	bvec2 gr() const
	{
	    return bvec2(g, r);
	}

	bvec2 gg() const
	{
	    return bvec2(g, g);
	}

	bvec2 gb() const
	{
	    return bvec2(g, b);
	}

	bvec2 br() const
	{
	    return bvec2(b, r);
	}

	bvec2 bg() const
	{
	    return bvec2(b, g);
	}

	bvec2 bb() const
	{
	    return bvec2(b, b);
	}

	bvec3 rrr() const
	{
	    return bvec3(r, r, r);
	}

	bvec3 rrg() const
	{
	    return bvec3(r, r, g);
	}

	bvec3 rrb() const
	{
	    return bvec3(r, r, b);
	}

	bvec3 rgr() const
	{
	    return bvec3(r, g, r);
	}

	bvec3 rgg() const
	{
	    return bvec3(r, g, g);
	}

	bvec3 rgb() const
	{
	    return bvec3(r, g, b);
	}

	bvec3 rbr() const
	{
	    return bvec3(r, b, r);
	}

	bvec3 rbg() const
	{
	    return bvec3(r, b, g);
	}

	bvec3 rbb() const
	{
	    return bvec3(r, b, b);
	}

	bvec3 grr() const
	{
	    return bvec3(g, r, r);
	}

	bvec3 grg() const
	{
	    return bvec3(g, r, g);
	}

	bvec3 grb() const
	{
	    return bvec3(g, r, b);
	}

	bvec3 ggr() const
	{
	    return bvec3(g, g, r);
	}

	bvec3 ggg() const
	{
	    return bvec3(g, g, g);
	}

	bvec3 ggb() const
	{
	    return bvec3(g, g, b);
	}

	bvec3 gbr() const
	{
	    return bvec3(g, b, r);
	}

	bvec3 gbg() const
	{
	    return bvec3(g, b, g);
	}

	bvec3 gbb() const
	{
	    return bvec3(g, b, b);
	}

	bvec3 brr() const
	{
	    return bvec3(b, r, r);
	}

	bvec3 brg() const
	{
	    return bvec3(b, r, g);
	}

	bvec3 brb() const
	{
	    return bvec3(b, r, b);
	}

	bvec3 bgr() const
	{
	    return bvec3(b, g, r);
	}

	bvec3 bgg() const
	{
	    return bvec3(b, g, g);
	}

	bvec3 bgb() const
	{
	    return bvec3(b, g, b);
	}

	bvec3 bbr() const
	{
	    return bvec3(b, b, r);
	}

	bvec3 bbg() const
	{
	    return bvec3(b, b, g);
	}

	bvec3 bbb() const
	{
	    return bvec3(b, b, b);
	}

	bvec2 ss() const
	{
	    return bvec2(s, s);
	}

	bvec2 st() const
	{
	    return bvec2(s, t);
	}

	bvec2 sp() const
	{
	    return bvec2(s, p);
	}

	bvec2 ts() const
	{
	    return bvec2(t, s);
	}

	bvec2 tt() const
	{
	    return bvec2(t, t);
	}

	bvec2 tp() const
	{
	    return bvec2(t, p);
	}

	bvec2 ps() const
	{
	    return bvec2(p, s);
	}

	bvec2 pt() const
	{
	    return bvec2(p, t);
	}

	bvec2 pp() const
	{
	    return bvec2(p, p);
	}

	bvec3 sss() const
	{
	    return bvec3(s, s, s);
	}

	bvec3 sst() const
	{
	    return bvec3(s, s, t);
	}

	bvec3 ssp() const
	{
	    return bvec3(s, s, p);
	}

	bvec3 sts() const
	{
	    return bvec3(s, t, s);
	}

	bvec3 stt() const
	{
	    return bvec3(s, t, t);
	}

	bvec3 stp() const
	{
	    return bvec3(s, t, p);
	}

	bvec3 sps() const
	{
	    return bvec3(s, p, s);
	}

	bvec3 spt() const
	{
	    return bvec3(s, p, t);
	}

	bvec3 spp() const
	{
	    return bvec3(s, p, p);
	}

	bvec3 tss() const
	{
	    return bvec3(t, s, s);
	}

	bvec3 tst() const
	{
	    return bvec3(t, s, t);
	}

	bvec3 tsp() const
	{
	    return bvec3(t, s, p);
	}

	bvec3 tts() const
	{
	    return bvec3(t, t, s);
	}

	bvec3 ttt() const
	{
	    return bvec3(t, t, t);
	}

	bvec3 ttp() const
	{
	    return bvec3(t, t, p);
	}

	bvec3 tps() const
	{
	    return bvec3(t, p, s);
	}

	bvec3 tpt() const
	{
	    return bvec3(t, p, t);
	}

	bvec3 tpp() const
	{
	    return bvec3(t, p, p);
	}

	bvec3 pss() const
	{
	    return bvec3(p, s, s);
	}

	bvec3 pst() const
	{
	    return bvec3(p, s, t);
	}

	bvec3 psp() const
	{
	    return bvec3(p, s, p);
	}

	bvec3 pts() const
	{
	    return bvec3(p, t, s);
	}

	bvec3 ptt() const
	{
	    return bvec3(p, t, t);
	}

	bvec3 ptp() const
	{
	    return bvec3(p, t, p);
	}

	bvec3 pps() const
	{
	    return bvec3(p, p, s);
	}

	bvec3 ppt() const
	{
	    return bvec3(p, p, t);
	}

	bvec3 ppp() const
	{
	    return bvec3(p, p, p);
	}

	/* Functions */

	bool any() const
	{
	    return x || y || z;
	}

	bool all() const
	{
	    return x && y && z;
	}

	bvec3 negate() const
	{
	    return bvec3(!x, !y, !z);
	}
    };

    class vec3
    {
	public:

	union
	{
	    struct
	    {
		float x, y, z;
	    };
	    struct
	    {
		float r, g, b;
	    };
	    struct
	    {
		float s, t, p;
	    };
	    float v[3];
	};

	/* Constructors, Destructor */

	vec3() {}

	vec3(float x, float y, float z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	vec3(const vec2 xy, float z)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	}

	vec3(float x, const vec2 yz)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	}

	vec3(const float *xyz)
	{
	    this->x = xyz[0];
	    this->y = xyz[1];
	    this->z = xyz[2];
	}

	~vec3() {}

	/* "Swizzling" (as far as possible in C++) */

	vec2 xx() const
	{
	    return vec2(x, x);
	}

	vec2 xy() const
	{
	    return vec2(x, y);
	}

	vec2 xz() const
	{
	    return vec2(x, z);
	}

	vec2 yx() const
	{
	    return vec2(y, x);
	}

	vec2 yy() const
	{
	    return vec2(y, y);
	}

	vec2 yz() const
	{
	    return vec2(y, z);
	}

	vec2 zx() const
	{
	    return vec2(z, x);
	}

	vec2 zy() const
	{
	    return vec2(z, y);
	}

	vec2 zz() const
	{
	    return vec2(z, z);
	}

	vec3 xxx() const
	{
	    return vec3(x, x, x);
	}

	vec3 xxy() const
	{
	    return vec3(x, x, y);
	}

	vec3 xxz() const
	{
	    return vec3(x, x, z);
	}

	vec3 xyx() const
	{
	    return vec3(x, y, x);
	}

	vec3 xyy() const
	{
	    return vec3(x, y, y);
	}

	vec3 xyz() const
	{
	    return vec3(x, y, z);
	}

	vec3 xzx() const
	{
	    return vec3(x, z, x);
	}

	vec3 xzy() const
	{
	    return vec3(x, z, y);
	}

	vec3 xzz() const
	{
	    return vec3(x, z, z);
	}

	vec3 yxx() const
	{
	    return vec3(y, x, x);
	}

	vec3 yxy() const
	{
	    return vec3(y, x, y);
	}

	vec3 yxz() const
	{
	    return vec3(y, x, z);
	}

	vec3 yyx() const
	{
	    return vec3(y, y, x);
	}

	vec3 yyy() const
	{
	    return vec3(y, y, y);
	}

	vec3 yyz() const
	{
	    return vec3(y, y, z);
	}

	vec3 yzx() const
	{
	    return vec3(y, z, x);
	}

	vec3 yzy() const
	{
	    return vec3(y, z, y);
	}

	vec3 yzz() const
	{
	    return vec3(y, z, z);
	}

	vec3 zxx() const
	{
	    return vec3(z, x, x);
	}

	vec3 zxy() const
	{
	    return vec3(z, x, y);
	}

	vec3 zxz() const
	{
	    return vec3(z, x, z);
	}

	vec3 zyx() const
	{
	    return vec3(z, y, x);
	}

	vec3 zyy() const
	{
	    return vec3(z, y, y);
	}

	vec3 zyz() const
	{
	    return vec3(z, y, z);
	}

	vec3 zzx() const
	{
	    return vec3(z, z, x);
	}

	vec3 zzy() const
	{
	    return vec3(z, z, y);
	}

	vec3 zzz() const
	{
	    return vec3(z, z, z);
	}

	vec2 rr() const
	{
	    return vec2(r, r);
	}

	vec2 rg() const
	{
	    return vec2(r, g);
	}

	vec2 rb() const
	{
	    return vec2(r, b);
	}

	vec2 gr() const
	{
	    return vec2(g, r);
	}

	vec2 gg() const
	{
	    return vec2(g, g);
	}

	vec2 gb() const
	{
	    return vec2(g, b);
	}

	vec2 br() const
	{
	    return vec2(b, r);
	}

	vec2 bg() const
	{
	    return vec2(b, g);
	}

	vec2 bb() const
	{
	    return vec2(b, b);
	}

	vec3 rrr() const
	{
	    return vec3(r, r, r);
	}

	vec3 rrg() const
	{
	    return vec3(r, r, g);
	}

	vec3 rrb() const
	{
	    return vec3(r, r, b);
	}

	vec3 rgr() const
	{
	    return vec3(r, g, r);
	}

	vec3 rgg() const
	{
	    return vec3(r, g, g);
	}

	vec3 rgb() const
	{
	    return vec3(r, g, b);
	}

	vec3 rbr() const
	{
	    return vec3(r, b, r);
	}

	vec3 rbg() const
	{
	    return vec3(r, b, g);
	}

	vec3 rbb() const
	{
	    return vec3(r, b, b);
	}

	vec3 grr() const
	{
	    return vec3(g, r, r);
	}

	vec3 grg() const
	{
	    return vec3(g, r, g);
	}

	vec3 grb() const
	{
	    return vec3(g, r, b);
	}

	vec3 ggr() const
	{
	    return vec3(g, g, r);
	}

	vec3 ggg() const
	{
	    return vec3(g, g, g);
	}

	vec3 ggb() const
	{
	    return vec3(g, g, b);
	}

	vec3 gbr() const
	{
	    return vec3(g, b, r);
	}

	vec3 gbg() const
	{
	    return vec3(g, b, g);
	}

	vec3 gbb() const
	{
	    return vec3(g, b, b);
	}

	vec3 brr() const
	{
	    return vec3(b, r, r);
	}

	vec3 brg() const
	{
	    return vec3(b, r, g);
	}

	vec3 brb() const
	{
	    return vec3(b, r, b);
	}

	vec3 bgr() const
	{
	    return vec3(b, g, r);
	}

	vec3 bgg() const
	{
	    return vec3(b, g, g);
	}

	vec3 bgb() const
	{
	    return vec3(b, g, b);
	}

	vec3 bbr() const
	{
	    return vec3(b, b, r);
	}

	vec3 bbg() const
	{
	    return vec3(b, b, g);
	}

	vec3 bbb() const
	{
	    return vec3(b, b, b);
	}

	vec2 ss() const
	{
	    return vec2(s, s);
	}

	vec2 st() const
	{
	    return vec2(s, t);
	}

	vec2 sp() const
	{
	    return vec2(s, p);
	}

	vec2 ts() const
	{
	    return vec2(t, s);
	}

	vec2 tt() const
	{
	    return vec2(t, t);
	}

	vec2 tp() const
	{
	    return vec2(t, p);
	}

	vec2 ps() const
	{
	    return vec2(p, s);
	}

	vec2 pt() const
	{
	    return vec2(p, t);
	}

	vec2 pp() const
	{
	    return vec2(p, p);
	}

	vec3 sss() const
	{
	    return vec3(s, s, s);
	}

	vec3 sst() const
	{
	    return vec3(s, s, t);
	}

	vec3 ssp() const
	{
	    return vec3(s, s, p);
	}

	vec3 sts() const
	{
	    return vec3(s, t, s);
	}

	vec3 stt() const
	{
	    return vec3(s, t, t);
	}

	vec3 stp() const
	{
	    return vec3(s, t, p);
	}

	vec3 sps() const
	{
	    return vec3(s, p, s);
	}

	vec3 spt() const
	{
	    return vec3(s, p, t);
	}

	vec3 spp() const
	{
	    return vec3(s, p, p);
	}

	vec3 tss() const
	{
	    return vec3(t, s, s);
	}

	vec3 tst() const
	{
	    return vec3(t, s, t);
	}

	vec3 tsp() const
	{
	    return vec3(t, s, p);
	}

	vec3 tts() const
	{
	    return vec3(t, t, s);
	}

	vec3 ttt() const
	{
	    return vec3(t, t, t);
	}

	vec3 ttp() const
	{
	    return vec3(t, t, p);
	}

	vec3 tps() const
	{
	    return vec3(t, p, s);
	}

	vec3 tpt() const
	{
	    return vec3(t, p, t);
	}

	vec3 tpp() const
	{
	    return vec3(t, p, p);
	}

	vec3 pss() const
	{
	    return vec3(p, s, s);
	}

	vec3 pst() const
	{
	    return vec3(p, s, t);
	}

	vec3 psp() const
	{
	    return vec3(p, s, p);
	}

	vec3 pts() const
	{
	    return vec3(p, t, s);
	}

	vec3 ptt() const
	{
	    return vec3(p, t, t);
	}

	vec3 ptp() const
	{
	    return vec3(p, t, p);
	}

	vec3 pps() const
	{
	    return vec3(p, p, s);
	}

	vec3 ppt() const
	{
	    return vec3(p, p, t);
	}

	vec3 ppp() const
	{
	    return vec3(p, p, p);
	}

	/* Operators */

	const vec3& operator=(const vec3& v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return v[i];
	}

	const float &operator[](size_t i) const
	{
	    return v[i];
	}
	
	vec3 operator*(const float s) const
	{
	    return vec3(x * s, y * s, z * s);
	}

	vec3 operator/(const float s) const
	{
	    return vec3(x / s, y / s, z / s);
	}
     
	const vec3 &operator*=(float s)
	{
	    x *= s;
	    y *= s;
	    z *= s;
	    return *this;
	}
     
	const vec3 &operator/=(float s)
	{
	    x /= s;
	    y /= s;
	    z /= s;
	    return *this;
	}

	vec3 operator+(const vec3& v) const
	{
	    return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3 operator-(const vec3& v) const
	{
	    return vec3(x - v.x, y - v.y, z - v.z);
	}

	vec3 operator*(const vec3& v) const 
	{
	    return vec3(x * v.x, y * v.y, z * v.z);
	}

	vec3 operator/(const vec3& v) const 
	{
	    return vec3(x / v.x, y / v.y, z / v.z);
	}

	vec3 operator-() const
	{
	    return vec3(-x, -y, -z);
	}

	const vec3& operator+=(const vec3& v)
	{
	    x += v.x;
	    y += v.y;
	    z += v.z;
	    return *this;
	}

	const vec3& operator-=(const vec3& v)
	{
	    x -= v.x;
	    y -= v.y;
	    z -= v.z;
	    return *this;
	}

	const vec3& operator*=(const vec3& v)
	{
	    x *= v.x;
	    y *= v.y;
	    z *= v.z;
	    return *this;
	}

	const vec3& operator/=(const vec3& v)
	{
	    x /= v.x;
	    y /= v.y;
	    z /= v.z;
	    return *this;
	}

	/* Trigonometric functions */

	vec3 sin() const
	{
	    return vec3(sinf(x), sinf(y), sinf(z));
	}

	vec3 cos() const
	{
	    return vec3(cosf(x), cosf(y), cosf(z));
	}

	vec3 tan() const
	{
	    return vec3(tanf(x), tanf(y), tanf(z));
	}

	vec3 asin() const
	{
	    return vec3(asinf(x), asinf(y), asinf(z));
	}

	vec3 acos() const
	{
	    return vec3(acosf(x), acosf(y), acosf(z));
	}

	vec3 atan() const
	{
	    return vec3(atanf(x), atanf(y), atanf(z));
	}

	vec3 atan(const vec3 v) const
	{
	    return vec3(atan2f(x, v.x), atan2f(y, v.y), atan2f(z, v.z));
	}

	vec3 radians() const
	{
	    return vec3(x * static_cast<float>(M_PI / 180.0f), y * static_cast<float>(M_PI / 180.0f), z * static_cast<float>(M_PI / 180.0f));
	}

	vec3 degrees() const
	{
	    return vec3(x * 180.0f / static_cast<float>(M_PI), y * 180.0f / static_cast<float>(M_PI), z * 180.0f / static_cast<float>(M_PI));
	}

	/* Exponential functions */

	vec3 pow(const float p) const
	{
	    return vec3(powf(x, p), powf(y, p), powf(z, p));
	}

	vec3 exp() const
	{
	    return vec3(expf(x), expf(y), expf(z));
	}

	vec3 log() const
	{
	    return vec3(logf(x), logf(y), logf(z));
	}

	vec3 sqrt() const
	{
	    return vec3(sqrtf(x), sqrtf(y), sqrtf(z));
	}
	
	/* Common functions */

	vec3 abs() const
	{
	    return vec3(fabsf(x), fabsf(y), fabsf(z));
	}

	vec3 sign() const
	{
	    return vec3(
		    x < 0.0f ? -1.0f : x > 0.0f ? +1.0f : 0.0f,
		    y < 0.0f ? -1.0f : y > 0.0f ? +1.0f : 0.0f,
		    z < 0.0f ? -1.0f : z > 0.0f ? +1.0f : 0.0f);
	}

	vec3 floor() const
	{
	    return vec3(floorf(x), floorf(y), floorf(z));
	}

	vec3 ceil() const
	{
	    return vec3(ceilf(x), ceilf(y), ceilf(z));
	}

	vec3 fract() const
	{
	    return *this - this->floor();
	}

	vec3 min(const float v) const
	{
	    return vec3(v < x ? v : x, v < y ? v : y, v < z ? v : z);
	}

	vec3 min(const vec3 v) const
	{
	    return vec3(v.x < x ? v.x : x, v.y < y ? v.y : y, v.z < z ? v.z : z);
	}

	vec3 max(const float v) const
	{
	    return vec3(v > x ? v : x, v > y ? v : y, v > z ? v : z);
	}

	vec3 max(const vec3 v) const
	{
	    return vec3(v.x > x ? v.x : x, v.y > y ? v.y : y, v.z > z ? v.z : z);
	}

	vec3 clamp(const float minval, const float maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec3 clamp(const vec3 minval, const vec3 maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec3 mix(const vec3 v, const float alpha) const
	{
	    return *this * (1.0f - alpha) + v * alpha;
	}

	vec3 mix(const vec3 v, const vec3 alpha) const
	{
	    return (vec3(1.0f, 1.0f, 1.0f) - alpha) * *this + alpha * v;
	}

	vec3 step(const float edge) const
	{
	    return vec3(x < edge ? 0.0f : 1.0f, y < edge ? 0.0f : 1.0f, z < edge ? 0.0f : 1.0f);
	}

	vec3 step(const vec3 edge) const
	{
	    return vec3(x < edge.x ? 0.0f : 1.0f, y < edge.y ? 0.0f : 1.0f, z < edge.z ? 0.0f : 1.0f);
	}

	vec3 smoothstep(const float edge0, const float edge1) const
	{
	    vec3 t = (*this - vec3(edge0, edge0, edge0)) / (vec3(edge1, edge1, edge1) - vec3(edge0, edge0, edge0));
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec3(3.0f, 3.0f, 3.0f) - t * 2.0f);
	}

	vec3 smoothstep(const vec3 edge0, const vec3 edge1) const
	{
	    vec3 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec3(3.0f, 3.0f, 3.0f) - t * 2.0f);
	}
	
	vec3 mod(const float y) const
	{
	    vec3 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}
	
	vec3 mod(const vec3 y) const
	{
	    vec3 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	/* Geometric functions */

	float length() const
	{
	    return sqrtf(x * x + y * y + z * z);
	}

	float distance(const vec3 v) const
	{
	    return vec3(*this - v).length();
	}

	float dot(const vec3 v) const
	{
	    return x * v.x + y * v.y + z * v.z;
	}

	vec3 cross(const vec3 v) const
	{
	    return vec3(
		    y * v.z - v.y * z,
		    z * v.x - v.z * x,
		    x * v.y - v.x * y);
	}

	vec3 normalize() const
	{
	    return *this / this->length();
	}

	vec3 faceforward(const vec3 I, const vec3 Nref) const
	{
	    return Nref.dot(I) < 0.0f ? *this : - *this;
	}

	vec3 reflect(const vec3 N) const
	{
	    return *this - N * (N.dot(*this) * 2.0f);
	}

	vec3 refract(const vec3 N, float eta) const
	{
	    float d = N.dot(*this);
	    float k = 1.0f - eta * eta * (1.0f - d * d);
	    return k < 0.0f ? vec3(0.0f, 0.0f, 0.0f) : *this * eta - N * (eta * d + sqrtf(k));
	}
	
	/* Comparion functions */

	bvec3 equal(const vec3 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return bvec3(
		    fabsf(x - v.x) < epsilon,
		    fabsf(y - v.y) < epsilon,
		    fabsf(z - v.z) < epsilon);
	}

	bvec3 notEqual(const vec3 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	bvec3 greaterThan(const vec3 v) const
	{
	    return bvec3(x > v.x, y > v.y, z > v.z);
	}

	bvec3 greaterThanEqual(const vec3 v) const
	{
	    return bvec3(x >= v.x, y >= v.y, z >= v.z);
	}

	bvec3 lessThan(const vec3 v) const
	{
	    return bvec3(x < v.x, y < v.y, z < v.z);
	}

	bvec3 lessThanEqual(const vec3 v) const
	{
	    return bvec3(x <= v.x, y <= v.y, z <= v.z);
	}
    };

    /* Trigonometric functions */

    vec3 sin(const vec3 v)
    {
	return v.sin();
    }

    vec3 cos(const vec3 v)
    {
	return v.cos();
    }

    vec3 tan(const vec3 v)
    {
	return v.tan();
    }

    vec3 asin(const vec3 v)
    {
	return v.asin();
    }

    vec3 acos(const vec3 v)
    {
	return v.acos();
    }

    vec3 atan(const vec3 v)
    {
	return v.atan();
    }

    vec3 atan(const vec3 v, const vec3 w)
    {

	return v.atan(w);
    }

    vec3 radians(const vec3 v)
    {
	return v.radians();
    }

    vec3 degrees(const vec3 v)
    {
	return v.degrees();
    }

    /* Exponential functions */

    vec3 pow(const vec3 v, const float p)
    {
	return v.pow(p);
    }

    vec3 exp(const vec3 v)
    {
	return v.exp();
    }

    vec3 log(const vec3 v)
    {
	return v.log();
    }

    vec3 sqrt(const vec3 v)
    {
	return v.sqrt();
    }

    /* Common functions */

    vec3 abs(const vec3 v)
    {
	return v.abs();
    }

    vec3 sign(const vec3 v)
    {
	return v.sign();
    }

    vec3 floor(const vec3 v)
    {
	return v.floor();
    }

    vec3 ceil(const vec3 v)
    {
	return v.ceil();
    }

    vec3 fract(const vec3 v)
    {
	return v.fract();
    }

    vec3 min(const vec3 v, const float w)
    {
	return v.min(w);
    }

    vec3 min(const vec3 v, const vec3 w)
    {
	return v.min(w);
    }

    vec3 max(const vec3 v, const float w)
    {
	return v.max(w);
    }

    vec3 max(const vec3 v, const vec3 w)
    {
	return v.max(w);
    }

    vec3 clamp(const vec3 v, const float minval, const float maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec3 clamp(const vec3 v, const vec3 minval, const vec3 maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec3 mix(const vec3 v, const vec3 w, const float alpha)
    {
	return v.mix(w, alpha);
    }

    vec3 mix(const vec3 v, const vec3 w, const vec3 alpha)
    {
	return v.mix(w, alpha);
    }

    vec3 step(const vec3 v, const float edge)
    {
	return v.step(edge);
    }

    vec3 step(const vec3 v, const vec3 edge)
    {
	return v.step(edge);
    }

    vec3 smoothstep(const vec3 v, const float edge0, const float edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec3 smoothstep(const vec3 v, const vec3 edge0, const vec3 edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec3 mod(const vec3 v, const float y)
    {
	return v.mod(y);
    }

    vec3 mod(const vec3 v, const vec3 y)
    {
	return v.mod(y);
    }

    /* Geometric functions */

    float length(const vec3 v)
    {
	return v.length();
    }

    float distance(const vec3 v, const vec3 w)
    {
	return v.distance(w);
    }

    float dot(const vec3 v, const vec3 w)
    {
	return v.dot(w);
    }

    vec3 cross(const vec3 v, const vec3 w)
    {
	return v.cross(w);
    }

    vec3 normalize(const vec3 v)
    {
	return v.normalize();
    }

    vec3 faceforward(const vec3 N, const vec3 I, const vec3 Nref)
    {
	return N.faceforward(I, Nref);
    }

    vec3 reflect(const vec3 I, const vec3 N)
    {
	return I.reflect(N);
    }

    vec3 refract(const vec3 I, const vec3 N, float eta)
    {
	return I.refract(N, eta);
    }

    /* Comparion functions */

    bvec3 equal(const vec3 v, const vec3 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.equal(w, epsilon);
    }

    bvec3 notEqual(const vec3 v, const vec3 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.notEqual(w, epsilon);
    }

    bvec3 greaterThan(const vec3 v, const vec3 w)
    {
	return v.greaterThan(w);
    }

    bvec3 greaterThanEqual(const vec3 v, const vec3 w)
    {
	return v.greaterThanEqual(w);
    }

    bvec3 lessThan(const vec3 v, const vec3 w)
    {
	return v.lessThan(w);
    }

    bvec3 lessThanEqual(const vec3 v, const vec3 w)
    {
	return v.lessThanEqual(w);
    }

    class bvec4
    {
	public:

	union
	{
	    struct
	    {
		bool x, y, z, w;
	    };
	    struct
	    {
		bool r, g, b, a;
	    };
	    struct
	    {
		bool s, t, p, q;
	    };
	    bool v[4];
	};

	/* Constructors, Destructor */

	bvec4() {}

	bvec4(bool x, bool y, bool z, bool w)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	    this->w = w;
	}

	bvec4(const bvec2 xy, bool z, bool w)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	    this->w = w;
	}

	bvec4(const bvec2 xy, const bvec2 zw)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	bvec4(bool x, const bvec2 yz, bool w)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	    this->w = w;
	}

	bvec4(bool x, bool y, const bvec2 zw)
	{
	    this->x = x;
	    this->y = y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	bvec4(const bvec3 xyz, bool w)
	{
	    this->x = xyz.x;
	    this->y = xyz.y;
	    this->z = xyz.z;
	    this->w = w;
	}

	bvec4(bool x, const bvec3 yzw)
	{
	    this->x = x;
	    this->y = yzw.x;
	    this->z = yzw.y;
	    this->w = yzw.z;
	}

	bvec4(const bool *xyzw)
	{
	    this->x = xyzw[0];
	    this->y = xyzw[1];
	    this->z = xyzw[2];
	    this->w = xyzw[3];
	}

	~bvec4() {}

	/* Operators */

	const bvec4& operator=(const bvec4& v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    w = v.w;
	    return *this;
	}

	bool &operator[](size_t i)
	{
	    return v[i];
	}

	const bool &operator[](size_t i) const
	{
	    return v[i];
	}

	/* "Swizzling" (as far as possible in C++) */

	bvec2 xx() const
	{
	    return bvec2(x, x);
	}

	bvec2 xy() const
	{
	    return bvec2(x, y);
	}

	bvec2 xz() const
	{
	    return bvec2(x, z);
	}

	bvec2 xw() const
	{
	    return bvec2(x, w);
	}

	bvec2 yx() const
	{
	    return bvec2(y, x);
	}

	bvec2 yy() const
	{
	    return bvec2(y, y);
	}

	bvec2 yz() const
	{
	    return bvec2(y, z);
	}

	bvec2 yw() const
	{
	    return bvec2(y, w);
	}

	bvec2 zx() const
	{
	    return bvec2(z, x);
	}

	bvec2 zy() const
	{
	    return bvec2(z, y);
	}

	bvec2 zz() const
	{
	    return bvec2(z, z);
	}

	bvec2 zw() const
	{
	    return bvec2(w, w);
	}

	bvec2 wx() const
	{
	    return bvec2(w, x);
	}

	bvec2 wy() const
	{
	    return bvec2(w, y);
	}

	bvec2 wz() const
	{
	    return bvec2(w, z);
	}

	bvec2 ww() const
	{
	    return bvec2(w, w);
	}

	bvec3 xxx() const
	{
	    return bvec3(x, x, x);
	}

	bvec3 xxy() const
	{
	    return bvec3(x, x, y);
	}

	bvec3 xxz() const
	{
	    return bvec3(x, x, z);
	}

	bvec3 xxw() const
	{
	    return bvec3(x, x, w);
	}

	bvec3 xyx() const
	{
	    return bvec3(x, y, x);
	}

	bvec3 xyy() const
	{
	    return bvec3(x, y, y);
	}

	bvec3 xyz() const
	{
	    return bvec3(x, y, z);
	}

	bvec3 xyw() const
	{
	    return bvec3(x, y, w);
	}

	bvec3 xzx() const
	{
	    return bvec3(x, z, x);
	}

	bvec3 xzy() const
	{
	    return bvec3(x, z, y);
	}

	bvec3 xzz() const
	{
	    return bvec3(x, z, z);
	}

	bvec3 xzw() const
	{
	    return bvec3(x, w, w);
	}

	bvec3 xwx() const
	{
	    return bvec3(x, w, x);
	}

	bvec3 xwy() const
	{
	    return bvec3(x, w, y);
	}

	bvec3 xwz() const
	{
	    return bvec3(x, w, z);
	}

	bvec3 xww() const
	{
	    return bvec3(x, w, w);
	}

	bvec3 yxx() const
	{
	    return bvec3(y, x, x);
	}

	bvec3 yxy() const
	{
	    return bvec3(y, x, y);
	}

	bvec3 yxz() const
	{
	    return bvec3(y, x, z);
	}

	bvec3 yxw() const
	{
	    return bvec3(y, x, w);
	}

	bvec3 yyx() const
	{
	    return bvec3(y, y, x);
	}

	bvec3 yyy() const
	{
	    return bvec3(y, y, y);
	}

	bvec3 yyz() const
	{
	    return bvec3(y, y, z);
	}

	bvec3 yyw() const
	{
	    return bvec3(y, y, w);
	}

	bvec3 yzx() const
	{
	    return bvec3(y, z, x);
	}

	bvec3 yzy() const
	{
	    return bvec3(y, z, y);
	}

	bvec3 yzz() const
	{
	    return bvec3(y, z, z);
	}

	bvec3 yzw() const
	{
	    return bvec3(y, w, w);
	}

	bvec3 ywx() const
	{
	    return bvec3(y, w, x);
	}

	bvec3 ywy() const
	{
	    return bvec3(y, w, y);
	}

	bvec3 ywz() const
	{
	    return bvec3(y, w, z);
	}

	bvec3 yww() const
	{
	    return bvec3(y, w, w);
	}

	bvec3 zxx() const
	{
	    return bvec3(z, x, x);
	}

	bvec3 zxy() const
	{
	    return bvec3(z, x, y);
	}

	bvec3 zxz() const
	{
	    return bvec3(z, x, z);
	}

	bvec3 zxw() const
	{
	    return bvec3(z, x, w);
	}

	bvec3 zyx() const
	{
	    return bvec3(z, y, x);
	}

	bvec3 zyy() const
	{
	    return bvec3(z, y, y);
	}

	bvec3 zyz() const
	{
	    return bvec3(z, y, z);
	}

	bvec3 zyw() const
	{
	    return bvec3(z, y, w);
	}

	bvec3 zzx() const
	{
	    return bvec3(z, z, x);
	}

	bvec3 zzy() const
	{
	    return bvec3(z, z, y);
	}

	bvec3 zzz() const
	{
	    return bvec3(z, z, z);
	}

	bvec3 zzw() const
	{
	    return bvec3(z, w, w);
	}

	bvec3 zwx() const
	{
	    return bvec3(z, w, x);
	}

	bvec3 zwy() const
	{
	    return bvec3(z, w, y);
	}

	bvec3 zwz() const
	{
	    return bvec3(z, w, z);
	}

	bvec3 zww() const
	{
	    return bvec3(z, w, w);
	}

	bvec3 wxx() const
	{
	    return bvec3(w, x, x);
	}

	bvec3 wxy() const
	{
	    return bvec3(w, x, y);
	}

	bvec3 wxz() const
	{
	    return bvec3(w, x, z);
	}

	bvec3 wxw() const
	{
	    return bvec3(w, x, w);
	}

	bvec3 wyx() const
	{
	    return bvec3(w, y, x);
	}

	bvec3 wyy() const
	{
	    return bvec3(w, y, y);
	}

	bvec3 wyz() const
	{
	    return bvec3(w, y, z);
	}

	bvec3 wyw() const
	{
	    return bvec3(w, y, w);
	}

	bvec3 wzx() const
	{
	    return bvec3(w, z, x);
	}

	bvec3 wzy() const
	{
	    return bvec3(w, z, y);
	}

	bvec3 wzz() const
	{
	    return bvec3(w, z, z);
	}

	bvec3 wzw() const
	{
	    return bvec3(w, w, w);
	}

	bvec3 wwx() const
	{
	    return bvec3(w, w, x);
	}

	bvec3 wwy() const
	{
	    return bvec3(w, w, y);
	}

	bvec3 wwz() const
	{
	    return bvec3(w, w, z);
	}

	bvec3 www() const
	{
	    return bvec3(w, w, w);
	}

	bvec4 xxxx() const
	{
	    return bvec4(x, x, x, x);
	}

	bvec4 xxxy() const
	{
	    return bvec4(x, x, x, y);
	}

	bvec4 xxxz() const
	{
	    return bvec4(x, x, x, z);
	}

	bvec4 xxxw() const
	{
	    return bvec4(x, x, x, w);
	}

	bvec4 xxyx() const
	{
	    return bvec4(x, x, y, x);
	}

	bvec4 xxyy() const
	{
	    return bvec4(x, x, y, y);
	}

	bvec4 xxyz() const
	{
	    return bvec4(x, x, y, z);
	}

	bvec4 xxyw() const
	{
	    return bvec4(x, x, y, w);
	}

	bvec4 xxzx() const
	{
	    return bvec4(x, x, z, x);
	}

	bvec4 xxzy() const
	{
	    return bvec4(x, x, z, y);
	}

	bvec4 xxzz() const
	{
	    return bvec4(x, x, z, z);
	}

	bvec4 xxzw() const
	{
	    return bvec4(x, x, z, w);
	}

	bvec4 xxwx() const
	{
	    return bvec4(x, x, w, x);
	}

	bvec4 xxwy() const
	{
	    return bvec4(x, x, w, y);
	}

	bvec4 xxwz() const
	{
	    return bvec4(x, x, w, z);
	}

	bvec4 xxww() const
	{
	    return bvec4(x, x, w, w);
	}

	bvec4 xyxx() const
	{
	    return bvec4(x, y, x, x);
	}

	bvec4 xyxy() const
	{
	    return bvec4(x, y, x, y);
	}

	bvec4 xyxz() const
	{
	    return bvec4(x, y, x, z);
	}

	bvec4 xyxw() const
	{
	    return bvec4(x, y, x, w);
	}

	bvec4 xyyx() const
	{
	    return bvec4(x, y, y, x);
	}

	bvec4 xyyy() const
	{
	    return bvec4(x, y, y, y);
	}

	bvec4 xyyz() const
	{
	    return bvec4(x, y, y, z);
	}

	bvec4 xyyw() const
	{
	    return bvec4(x, y, y, w);
	}

	bvec4 xyzx() const
	{
	    return bvec4(x, y, z, x);
	}

	bvec4 xyzy() const
	{
	    return bvec4(x, y, z, y);
	}

	bvec4 xyzz() const
	{
	    return bvec4(x, y, z, z);
	}

	bvec4 xyzw() const
	{
	    return bvec4(x, y, z, w);
	}

	bvec4 xywx() const
	{
	    return bvec4(x, y, w, x);
	}

	bvec4 xywy() const
	{
	    return bvec4(x, y, w, y);
	}

	bvec4 xywz() const
	{
	    return bvec4(x, y, w, z);
	}

	bvec4 xyww() const
	{
	    return bvec4(x, y, w, w);
	}

	bvec4 xzxx() const
	{
	    return bvec4(x, z, x, x);
	}

	bvec4 xzxy() const
	{
	    return bvec4(x, z, x, y);
	}

	bvec4 xzxz() const
	{
	    return bvec4(x, z, x, z);
	}

	bvec4 xzxw() const
	{
	    return bvec4(x, z, x, w);
	}

	bvec4 xzyx() const
	{
	    return bvec4(x, z, y, x);
	}

	bvec4 xzyy() const
	{
	    return bvec4(x, z, y, y);
	}

	bvec4 xzyz() const
	{
	    return bvec4(x, z, y, z);
	}

	bvec4 xzyw() const
	{
	    return bvec4(x, z, y, w);
	}

	bvec4 xzzx() const
	{
	    return bvec4(x, z, z, x);
	}

	bvec4 xzzy() const
	{
	    return bvec4(x, z, z, y);
	}

	bvec4 xzzz() const
	{
	    return bvec4(x, z, z, z);
	}

	bvec4 xzzw() const
	{
	    return bvec4(x, z, z, w);
	}

	bvec4 xzwx() const
	{
	    return bvec4(x, z, w, x);
	}

	bvec4 xzwy() const
	{
	    return bvec4(x, z, w, y);
	}

	bvec4 xzwz() const
	{
	    return bvec4(x, z, w, z);
	}

	bvec4 xzww() const
	{
	    return bvec4(x, z, w, w);
	}

	bvec4 xwxx() const
	{
	    return bvec4(x, w, x, x);
	}

	bvec4 xwxy() const
	{
	    return bvec4(x, w, x, y);
	}

	bvec4 xwxz() const
	{
	    return bvec4(x, w, x, z);
	}

	bvec4 xwxw() const
	{
	    return bvec4(x, w, x, w);
	}

	bvec4 xwyx() const
	{
	    return bvec4(x, w, y, x);
	}

	bvec4 xwyy() const
	{
	    return bvec4(x, w, y, y);
	}

	bvec4 xwyz() const
	{
	    return bvec4(x, w, y, z);
	}

	bvec4 xwyw() const
	{
	    return bvec4(x, w, y, w);
	}

	bvec4 xwzx() const
	{
	    return bvec4(x, w, z, x);
	}

	bvec4 xwzy() const
	{
	    return bvec4(x, w, z, y);
	}

	bvec4 xwzz() const
	{
	    return bvec4(x, w, z, z);
	}

	bvec4 xwzw() const
	{
	    return bvec4(x, w, z, w);
	}

	bvec4 xwwx() const
	{
	    return bvec4(x, w, w, x);
	}

	bvec4 xwwy() const
	{
	    return bvec4(x, w, w, y);
	}

	bvec4 xwwz() const
	{
	    return bvec4(x, w, w, z);
	}

	bvec4 xwww() const
	{
	    return bvec4(x, w, w, w);
	}

	bvec4 yxxx() const
	{
	    return bvec4(y, x, x, x);
	}

	bvec4 yxxy() const
	{
	    return bvec4(y, x, x, y);
	}

	bvec4 yxxz() const
	{
	    return bvec4(y, x, x, z);
	}

	bvec4 yxxw() const
	{
	    return bvec4(y, x, x, w);
	}

	bvec4 yxyx() const
	{
	    return bvec4(y, x, y, x);
	}

	bvec4 yxyy() const
	{
	    return bvec4(y, x, y, y);
	}

	bvec4 yxyz() const
	{
	    return bvec4(y, x, y, z);
	}

	bvec4 yxyw() const
	{
	    return bvec4(y, x, y, w);
	}

	bvec4 yxzx() const
	{
	    return bvec4(y, x, z, x);
	}

	bvec4 yxzy() const
	{
	    return bvec4(y, x, z, y);
	}

	bvec4 yxzz() const
	{
	    return bvec4(y, x, z, z);
	}

	bvec4 yxzw() const
	{
	    return bvec4(y, x, z, w);
	}

	bvec4 yxwx() const
	{
	    return bvec4(y, x, w, x);
	}

	bvec4 yxwy() const
	{
	    return bvec4(y, x, w, y);
	}

	bvec4 yxwz() const
	{
	    return bvec4(y, x, w, z);
	}

	bvec4 yxww() const
	{
	    return bvec4(y, x, w, w);
	}

	bvec4 yyxx() const
	{
	    return bvec4(y, y, x, x);
	}

	bvec4 yyxy() const
	{
	    return bvec4(y, y, x, y);
	}

	bvec4 yyxz() const
	{
	    return bvec4(y, y, x, z);
	}

	bvec4 yyxw() const
	{
	    return bvec4(y, y, x, w);
	}

	bvec4 yyyx() const
	{
	    return bvec4(y, y, y, x);
	}

	bvec4 yyyy() const
	{
	    return bvec4(y, y, y, y);
	}

	bvec4 yyyz() const
	{
	    return bvec4(y, y, y, z);
	}

	bvec4 yyyw() const
	{
	    return bvec4(y, y, y, w);
	}

	bvec4 yyzx() const
	{
	    return bvec4(y, y, z, x);
	}

	bvec4 yyzy() const
	{
	    return bvec4(y, y, z, y);
	}

	bvec4 yyzz() const
	{
	    return bvec4(y, y, z, z);
	}

	bvec4 yyzw() const
	{
	    return bvec4(y, y, z, w);
	}

	bvec4 yywx() const
	{
	    return bvec4(y, y, w, x);
	}

	bvec4 yywy() const
	{
	    return bvec4(y, y, w, y);
	}

	bvec4 yywz() const
	{
	    return bvec4(y, y, w, z);
	}

	bvec4 yyww() const
	{
	    return bvec4(y, y, w, w);
	}

	bvec4 yzxx() const
	{
	    return bvec4(y, z, x, x);
	}

	bvec4 yzxy() const
	{
	    return bvec4(y, z, x, y);
	}

	bvec4 yzxz() const
	{
	    return bvec4(y, z, x, z);
	}

	bvec4 yzxw() const
	{
	    return bvec4(y, z, x, w);
	}

	bvec4 yzyx() const
	{
	    return bvec4(y, z, y, x);
	}

	bvec4 yzyy() const
	{
	    return bvec4(y, z, y, y);
	}

	bvec4 yzyz() const
	{
	    return bvec4(y, z, y, z);
	}

	bvec4 yzyw() const
	{
	    return bvec4(y, z, y, w);
	}

	bvec4 yzzx() const
	{
	    return bvec4(y, z, z, x);
	}

	bvec4 yzzy() const
	{
	    return bvec4(y, z, z, y);
	}

	bvec4 yzzz() const
	{
	    return bvec4(y, z, z, z);
	}

	bvec4 yzzw() const
	{
	    return bvec4(y, z, z, w);
	}

	bvec4 yzwx() const
	{
	    return bvec4(y, z, w, x);
	}

	bvec4 yzwy() const
	{
	    return bvec4(y, z, w, y);
	}

	bvec4 yzwz() const
	{
	    return bvec4(y, z, w, z);
	}

	bvec4 yzww() const
	{
	    return bvec4(y, z, w, w);
	}

	bvec4 ywxx() const
	{
	    return bvec4(y, w, x, x);
	}

	bvec4 ywxy() const
	{
	    return bvec4(y, w, x, y);
	}

	bvec4 ywxz() const
	{
	    return bvec4(y, w, x, z);
	}

	bvec4 ywxw() const
	{
	    return bvec4(y, w, x, w);
	}

	bvec4 ywyx() const
	{
	    return bvec4(y, w, y, x);
	}

	bvec4 ywyy() const
	{
	    return bvec4(y, w, y, y);
	}

	bvec4 ywyz() const
	{
	    return bvec4(y, w, y, z);
	}

	bvec4 ywyw() const
	{
	    return bvec4(y, w, y, w);
	}

	bvec4 ywzx() const
	{
	    return bvec4(y, w, z, x);
	}

	bvec4 ywzy() const
	{
	    return bvec4(y, w, z, y);
	}

	bvec4 ywzz() const
	{
	    return bvec4(y, w, z, z);
	}

	bvec4 ywzw() const
	{
	    return bvec4(y, w, z, w);
	}

	bvec4 ywwx() const
	{
	    return bvec4(y, w, w, x);
	}

	bvec4 ywwy() const
	{
	    return bvec4(y, w, w, y);
	}

	bvec4 ywwz() const
	{
	    return bvec4(y, w, w, z);
	}

	bvec4 ywww() const
	{
	    return bvec4(y, w, w, w);
	}

	bvec4 zxxx() const
	{
	    return bvec4(z, x, x, x);
	}

	bvec4 zxxy() const
	{
	    return bvec4(z, x, x, y);
	}

	bvec4 zxxz() const
	{
	    return bvec4(z, x, x, z);
	}

	bvec4 zxxw() const
	{
	    return bvec4(z, x, x, w);
	}

	bvec4 zxyx() const
	{
	    return bvec4(z, x, y, x);
	}

	bvec4 zxyy() const
	{
	    return bvec4(z, x, y, y);
	}

	bvec4 zxyz() const
	{
	    return bvec4(z, x, y, z);
	}

	bvec4 zxyw() const
	{
	    return bvec4(z, x, y, w);
	}

	bvec4 zxzx() const
	{
	    return bvec4(z, x, z, x);
	}

	bvec4 zxzy() const
	{
	    return bvec4(z, x, z, y);
	}

	bvec4 zxzz() const
	{
	    return bvec4(z, x, z, z);
	}

	bvec4 zxzw() const
	{
	    return bvec4(z, x, z, w);
	}

	bvec4 zxwx() const
	{
	    return bvec4(z, x, w, x);
	}

	bvec4 zxwy() const
	{
	    return bvec4(z, x, w, y);
	}

	bvec4 zxwz() const
	{
	    return bvec4(z, x, w, z);
	}

	bvec4 zxww() const
	{
	    return bvec4(z, x, w, w);
	}

	bvec4 zyxx() const
	{
	    return bvec4(z, y, x, x);
	}

	bvec4 zyxy() const
	{
	    return bvec4(z, y, x, y);
	}

	bvec4 zyxz() const
	{
	    return bvec4(z, y, x, z);
	}

	bvec4 zyxw() const
	{
	    return bvec4(z, y, x, w);
	}

	bvec4 zyyx() const
	{
	    return bvec4(z, y, y, x);
	}

	bvec4 zyyy() const
	{
	    return bvec4(z, y, y, y);
	}

	bvec4 zyyz() const
	{
	    return bvec4(z, y, y, z);
	}

	bvec4 zyyw() const
	{
	    return bvec4(z, y, y, w);
	}

	bvec4 zyzx() const
	{
	    return bvec4(z, y, z, x);
	}

	bvec4 zyzy() const
	{
	    return bvec4(z, y, z, y);
	}

	bvec4 zyzz() const
	{
	    return bvec4(z, y, z, z);
	}

	bvec4 zyzw() const
	{
	    return bvec4(z, y, z, w);
	}

	bvec4 zywx() const
	{
	    return bvec4(z, y, w, x);
	}

	bvec4 zywy() const
	{
	    return bvec4(z, y, w, y);
	}

	bvec4 zywz() const
	{
	    return bvec4(z, y, w, z);
	}

	bvec4 zyww() const
	{
	    return bvec4(z, y, w, w);
	}

	bvec4 zzxx() const
	{
	    return bvec4(z, z, x, x);
	}

	bvec4 zzxy() const
	{
	    return bvec4(z, z, x, y);
	}

	bvec4 zzxz() const
	{
	    return bvec4(z, z, x, z);
	}

	bvec4 zzxw() const
	{
	    return bvec4(z, z, x, w);
	}

	bvec4 zzyx() const
	{
	    return bvec4(z, z, y, x);
	}

	bvec4 zzyy() const
	{
	    return bvec4(z, z, y, y);
	}

	bvec4 zzyz() const
	{
	    return bvec4(z, z, y, z);
	}

	bvec4 zzyw() const
	{
	    return bvec4(z, z, y, w);
	}

	bvec4 zzzx() const
	{
	    return bvec4(z, z, z, x);
	}

	bvec4 zzzy() const
	{
	    return bvec4(z, z, z, y);
	}

	bvec4 zzzz() const
	{
	    return bvec4(z, z, z, z);
	}

	bvec4 zzzw() const
	{
	    return bvec4(z, z, z, w);
	}

	bvec4 zzwx() const
	{
	    return bvec4(z, z, w, x);
	}

	bvec4 zzwy() const
	{
	    return bvec4(z, z, w, y);
	}

	bvec4 zzwz() const
	{
	    return bvec4(z, z, w, z);
	}

	bvec4 zzww() const
	{
	    return bvec4(z, z, w, w);
	}

	bvec4 zwxx() const
	{
	    return bvec4(z, w, x, x);
	}

	bvec4 zwxy() const
	{
	    return bvec4(z, w, x, y);
	}

	bvec4 zwxz() const
	{
	    return bvec4(z, w, x, z);
	}

	bvec4 zwxw() const
	{
	    return bvec4(z, w, x, w);
	}

	bvec4 zwyx() const
	{
	    return bvec4(z, w, y, x);
	}

	bvec4 zwyy() const
	{
	    return bvec4(z, w, y, y);
	}

	bvec4 zwyz() const
	{
	    return bvec4(z, w, y, z);
	}

	bvec4 zwyw() const
	{
	    return bvec4(z, w, y, w);
	}

	bvec4 zwzx() const
	{
	    return bvec4(z, w, z, x);
	}

	bvec4 zwzy() const
	{
	    return bvec4(z, w, z, y);
	}

	bvec4 zwzz() const
	{
	    return bvec4(z, w, z, z);
	}

	bvec4 zwzw() const
	{
	    return bvec4(z, w, z, w);
	}

	bvec4 zwwx() const
	{
	    return bvec4(z, w, w, x);
	}

	bvec4 zwwy() const
	{
	    return bvec4(z, w, w, y);
	}

	bvec4 zwwz() const
	{
	    return bvec4(z, w, w, z);
	}

	bvec4 zwww() const
	{
	    return bvec4(z, w, w, w);
	}

	bvec4 wxxx() const
	{
	    return bvec4(w, x, x, x);
	}

	bvec4 wxxy() const
	{
	    return bvec4(w, x, x, y);
	}

	bvec4 wxxz() const
	{
	    return bvec4(w, x, x, z);
	}

	bvec4 wxxw() const
	{
	    return bvec4(w, x, x, w);
	}

	bvec4 wxyx() const
	{
	    return bvec4(w, x, y, x);
	}

	bvec4 wxyy() const
	{
	    return bvec4(w, x, y, y);
	}

	bvec4 wxyz() const
	{
	    return bvec4(w, x, y, z);
	}

	bvec4 wxyw() const
	{
	    return bvec4(w, x, y, w);
	}

	bvec4 wxzx() const
	{
	    return bvec4(w, x, z, x);
	}

	bvec4 wxzy() const
	{
	    return bvec4(w, x, z, y);
	}

	bvec4 wxzz() const
	{
	    return bvec4(w, x, z, z);
	}

	bvec4 wxzw() const
	{
	    return bvec4(w, x, z, w);
	}

	bvec4 wxwx() const
	{
	    return bvec4(w, x, w, x);
	}

	bvec4 wxwy() const
	{
	    return bvec4(w, x, w, y);
	}

	bvec4 wxwz() const
	{
	    return bvec4(w, x, w, z);
	}

	bvec4 wxww() const
	{
	    return bvec4(w, x, w, w);
	}

	bvec4 wyxx() const
	{
	    return bvec4(w, y, x, x);
	}

	bvec4 wyxy() const
	{
	    return bvec4(w, y, x, y);
	}

	bvec4 wyxz() const
	{
	    return bvec4(w, y, x, z);
	}

	bvec4 wyxw() const
	{
	    return bvec4(w, y, x, w);
	}

	bvec4 wyyx() const
	{
	    return bvec4(w, y, y, x);
	}

	bvec4 wyyy() const
	{
	    return bvec4(w, y, y, y);
	}

	bvec4 wyyz() const
	{
	    return bvec4(w, y, y, z);
	}

	bvec4 wyyw() const
	{
	    return bvec4(w, y, y, w);
	}

	bvec4 wyzx() const
	{
	    return bvec4(w, y, z, x);
	}

	bvec4 wyzy() const
	{
	    return bvec4(w, y, z, y);
	}

	bvec4 wyzz() const
	{
	    return bvec4(w, y, z, z);
	}

	bvec4 wyzw() const
	{
	    return bvec4(w, y, z, w);
	}

	bvec4 wywx() const
	{
	    return bvec4(w, y, w, x);
	}

	bvec4 wywy() const
	{
	    return bvec4(w, y, w, y);
	}

	bvec4 wywz() const
	{
	    return bvec4(w, y, w, z);
	}

	bvec4 wyww() const
	{
	    return bvec4(w, y, w, w);
	}

	bvec4 wzxx() const
	{
	    return bvec4(w, z, x, x);
	}

	bvec4 wzxy() const
	{
	    return bvec4(w, z, x, y);
	}

	bvec4 wzxz() const
	{
	    return bvec4(w, z, x, z);
	}

	bvec4 wzxw() const
	{
	    return bvec4(w, z, x, w);
	}

	bvec4 wzyx() const
	{
	    return bvec4(w, z, y, x);
	}

	bvec4 wzyy() const
	{
	    return bvec4(w, z, y, y);
	}

	bvec4 wzyz() const
	{
	    return bvec4(w, z, y, z);
	}

	bvec4 wzyw() const
	{
	    return bvec4(w, z, y, w);
	}

	bvec4 wzzx() const
	{
	    return bvec4(w, z, z, x);
	}

	bvec4 wzzy() const
	{
	    return bvec4(w, z, z, y);
	}

	bvec4 wzzz() const
	{
	    return bvec4(w, z, z, z);
	}

	bvec4 wzzw() const
	{
	    return bvec4(w, z, z, w);
	}

	bvec4 wzwx() const
	{
	    return bvec4(w, z, w, x);
	}

	bvec4 wzwy() const
	{
	    return bvec4(w, z, w, y);
	}

	bvec4 wzwz() const
	{
	    return bvec4(w, z, w, z);
	}

	bvec4 wzww() const
	{
	    return bvec4(w, z, w, w);
	}

	bvec4 wwxx() const
	{
	    return bvec4(w, w, x, x);
	}

	bvec4 wwxy() const
	{
	    return bvec4(w, w, x, y);
	}

	bvec4 wwxz() const
	{
	    return bvec4(w, w, x, z);
	}

	bvec4 wwxw() const
	{
	    return bvec4(w, w, x, w);
	}

	bvec4 wwyx() const
	{
	    return bvec4(w, w, y, x);
	}

	bvec4 wwyy() const
	{
	    return bvec4(w, w, y, y);
	}

	bvec4 wwyz() const
	{
	    return bvec4(w, w, y, z);
	}

	bvec4 wwyw() const
	{
	    return bvec4(w, w, y, w);
	}

	bvec4 wwzx() const
	{
	    return bvec4(w, w, z, x);
	}

	bvec4 wwzy() const
	{
	    return bvec4(w, w, z, y);
	}

	bvec4 wwzz() const
	{
	    return bvec4(w, w, z, z);
	}

	bvec4 wwzw() const
	{
	    return bvec4(w, w, z, w);
	}

	bvec4 wwwx() const
	{
	    return bvec4(w, w, w, x);
	}

	bvec4 wwwy() const
	{
	    return bvec4(w, w, w, y);
	}

	bvec4 wwwz() const
	{
	    return bvec4(w, w, w, z);
	}

	bvec4 wwww() const
	{
	    return bvec4(w, w, w, w);
	}

	bvec2 rr() const
	{
	    return bvec2(r, r);
	}

	bvec2 rg() const
	{
	    return bvec2(r, g);
	}

	bvec2 rb() const
	{
	    return bvec2(r, b);
	}

	bvec2 ra() const
	{
	    return bvec2(r, a);
	}

	bvec2 gr() const
	{
	    return bvec2(g, r);
	}

	bvec2 gg() const
	{
	    return bvec2(g, g);
	}

	bvec2 gb() const
	{
	    return bvec2(g, b);
	}

	bvec2 ga() const
	{
	    return bvec2(g, a);
	}

	bvec2 br() const
	{
	    return bvec2(b, r);
	}

	bvec2 bg() const
	{
	    return bvec2(b, g);
	}

	bvec2 bb() const
	{
	    return bvec2(b, b);
	}

	bvec2 ba() const
	{
	    return bvec2(b, a);
	}

	bvec2 ar() const
	{
	    return bvec2(a, r);
	}

	bvec2 ag() const
	{
	    return bvec2(a, g);
	}

	bvec2 ab() const
	{
	    return bvec2(a, b);
	}

	bvec2 aa() const
	{
	    return bvec2(a, a);
	}

	bvec3 rrr() const
	{
	    return bvec3(r, r, r);
	}

	bvec3 rrg() const
	{
	    return bvec3(r, r, g);
	}

	bvec3 rrb() const
	{
	    return bvec3(r, r, b);
	}

	bvec3 rra() const
	{
	    return bvec3(r, r, a);
	}

	bvec3 rgr() const
	{
	    return bvec3(r, g, r);
	}

	bvec3 rgg() const
	{
	    return bvec3(r, g, g);
	}

	bvec3 rgb() const
	{
	    return bvec3(r, g, b);
	}

	bvec3 rga() const
	{
	    return bvec3(r, g, a);
	}

	bvec3 rbr() const
	{
	    return bvec3(r, b, r);
	}

	bvec3 rbg() const
	{
	    return bvec3(r, b, g);
	}

	bvec3 rbb() const
	{
	    return bvec3(r, b, b);
	}

	bvec3 rba() const
	{
	    return bvec3(r, a, a);
	}

	bvec3 rar() const
	{
	    return bvec3(r, a, r);
	}

	bvec3 rag() const
	{
	    return bvec3(r, a, g);
	}

	bvec3 rab() const
	{
	    return bvec3(r, a, b);
	}

	bvec3 raa() const
	{
	    return bvec3(r, a, a);
	}

	bvec3 grr() const
	{
	    return bvec3(g, r, r);
	}

	bvec3 grg() const
	{
	    return bvec3(g, r, g);
	}

	bvec3 grb() const
	{
	    return bvec3(g, r, b);
	}

	bvec3 gra() const
	{
	    return bvec3(g, r, a);
	}

	bvec3 ggr() const
	{
	    return bvec3(g, g, r);
	}

	bvec3 ggg() const
	{
	    return bvec3(g, g, g);
	}

	bvec3 ggb() const
	{
	    return bvec3(g, g, b);
	}

	bvec3 gga() const
	{
	    return bvec3(g, g, a);
	}

	bvec3 gbr() const
	{
	    return bvec3(g, b, r);
	}

	bvec3 gbg() const
	{
	    return bvec3(g, b, g);
	}

	bvec3 gbb() const
	{
	    return bvec3(g, b, b);
	}

	bvec3 gba() const
	{
	    return bvec3(g, a, a);
	}

	bvec3 gar() const
	{
	    return bvec3(g, a, r);
	}

	bvec3 gag() const
	{
	    return bvec3(g, a, g);
	}

	bvec3 gab() const
	{
	    return bvec3(g, a, b);
	}

	bvec3 gaa() const
	{
	    return bvec3(g, a, a);
	}

	bvec3 brr() const
	{
	    return bvec3(b, r, r);
	}

	bvec3 brg() const
	{
	    return bvec3(b, r, g);
	}

	bvec3 brb() const
	{
	    return bvec3(b, r, b);
	}

	bvec3 bra() const
	{
	    return bvec3(b, r, a);
	}

	bvec3 bgr() const
	{
	    return bvec3(b, g, r);
	}

	bvec3 bgg() const
	{
	    return bvec3(b, g, g);
	}

	bvec3 bgb() const
	{
	    return bvec3(b, g, b);
	}

	bvec3 bga() const
	{
	    return bvec3(b, g, a);
	}

	bvec3 bbr() const
	{
	    return bvec3(b, b, r);
	}

	bvec3 bbg() const
	{
	    return bvec3(b, b, g);
	}

	bvec3 bbb() const
	{
	    return bvec3(b, b, b);
	}

	bvec3 bba() const
	{
	    return bvec3(b, a, a);
	}

	bvec3 bar() const
	{
	    return bvec3(b, a, r);
	}

	bvec3 bag() const
	{
	    return bvec3(b, a, g);
	}

	bvec3 bab() const
	{
	    return bvec3(b, a, b);
	}

	bvec3 baa() const
	{
	    return bvec3(b, a, a);
	}

	bvec3 arr() const
	{
	    return bvec3(a, r, r);
	}

	bvec3 arg() const
	{
	    return bvec3(a, r, g);
	}

	bvec3 arb() const
	{
	    return bvec3(a, r, b);
	}

	bvec3 ara() const
	{
	    return bvec3(a, r, a);
	}

	bvec3 agr() const
	{
	    return bvec3(a, g, r);
	}

	bvec3 agg() const
	{
	    return bvec3(a, g, g);
	}

	bvec3 agb() const
	{
	    return bvec3(a, g, b);
	}

	bvec3 aga() const
	{
	    return bvec3(a, g, a);
	}

	bvec3 abr() const
	{
	    return bvec3(a, b, r);
	}

	bvec3 abg() const
	{
	    return bvec3(a, b, g);
	}

	bvec3 abb() const
	{
	    return bvec3(a, b, b);
	}

	bvec3 aba() const
	{
	    return bvec3(a, a, a);
	}

	bvec3 aar() const
	{
	    return bvec3(a, a, r);
	}

	bvec3 aag() const
	{
	    return bvec3(a, a, g);
	}

	bvec3 aab() const
	{
	    return bvec3(a, a, b);
	}

	bvec3 aaa() const
	{
	    return bvec3(a, a, a);
	}

	bvec4 rrrr() const
	{
	    return bvec4(r, r, r, r);
	}

	bvec4 rrrg() const
	{
	    return bvec4(r, r, r, g);
	}

	bvec4 rrrb() const
	{
	    return bvec4(r, r, r, b);
	}

	bvec4 rrra() const
	{
	    return bvec4(r, r, r, a);
	}

	bvec4 rrgr() const
	{
	    return bvec4(r, r, g, r);
	}

	bvec4 rrgg() const
	{
	    return bvec4(r, r, g, g);
	}

	bvec4 rrgb() const
	{
	    return bvec4(r, r, g, b);
	}

	bvec4 rrga() const
	{
	    return bvec4(r, r, g, a);
	}

	bvec4 rrbr() const
	{
	    return bvec4(r, r, b, r);
	}

	bvec4 rrbg() const
	{
	    return bvec4(r, r, b, g);
	}

	bvec4 rrbb() const
	{
	    return bvec4(r, r, b, b);
	}

	bvec4 rrba() const
	{
	    return bvec4(r, r, b, a);
	}

	bvec4 rrar() const
	{
	    return bvec4(r, r, a, r);
	}

	bvec4 rrag() const
	{
	    return bvec4(r, r, a, g);
	}

	bvec4 rrab() const
	{
	    return bvec4(r, r, a, b);
	}

	bvec4 rraa() const
	{
	    return bvec4(r, r, a, a);
	}

	bvec4 rgrr() const
	{
	    return bvec4(r, g, r, r);
	}

	bvec4 rgrg() const
	{
	    return bvec4(r, g, r, g);
	}

	bvec4 rgrb() const
	{
	    return bvec4(r, g, r, b);
	}

	bvec4 rgra() const
	{
	    return bvec4(r, g, r, a);
	}

	bvec4 rggr() const
	{
	    return bvec4(r, g, g, r);
	}

	bvec4 rggg() const
	{
	    return bvec4(r, g, g, g);
	}

	bvec4 rggb() const
	{
	    return bvec4(r, g, g, b);
	}

	bvec4 rgga() const
	{
	    return bvec4(r, g, g, a);
	}

	bvec4 rgbr() const
	{
	    return bvec4(r, g, b, r);
	}

	bvec4 rgbg() const
	{
	    return bvec4(r, g, b, g);
	}

	bvec4 rgbb() const
	{
	    return bvec4(r, g, b, b);
	}

	bvec4 rgba() const
	{
	    return bvec4(r, g, b, a);
	}

	bvec4 rgar() const
	{
	    return bvec4(r, g, a, r);
	}

	bvec4 rgag() const
	{
	    return bvec4(r, g, a, g);
	}

	bvec4 rgab() const
	{
	    return bvec4(r, g, a, b);
	}

	bvec4 rgaa() const
	{
	    return bvec4(r, g, a, a);
	}

	bvec4 rbrr() const
	{
	    return bvec4(r, b, r, r);
	}

	bvec4 rbrg() const
	{
	    return bvec4(r, b, r, g);
	}

	bvec4 rbrb() const
	{
	    return bvec4(r, b, r, b);
	}

	bvec4 rbra() const
	{
	    return bvec4(r, b, r, a);
	}

	bvec4 rbgr() const
	{
	    return bvec4(r, b, g, r);
	}

	bvec4 rbgg() const
	{
	    return bvec4(r, b, g, g);
	}

	bvec4 rbgb() const
	{
	    return bvec4(r, b, g, b);
	}

	bvec4 rbga() const
	{
	    return bvec4(r, b, g, a);
	}

	bvec4 rbbr() const
	{
	    return bvec4(r, b, b, r);
	}

	bvec4 rbbg() const
	{
	    return bvec4(r, b, b, g);
	}

	bvec4 rbbb() const
	{
	    return bvec4(r, b, b, b);
	}

	bvec4 rbba() const
	{
	    return bvec4(r, b, b, a);
	}

	bvec4 rbar() const
	{
	    return bvec4(r, b, a, r);
	}

	bvec4 rbag() const
	{
	    return bvec4(r, b, a, g);
	}

	bvec4 rbab() const
	{
	    return bvec4(r, b, a, b);
	}

	bvec4 rbaa() const
	{
	    return bvec4(r, b, a, a);
	}

	bvec4 rarr() const
	{
	    return bvec4(r, a, r, r);
	}

	bvec4 rarg() const
	{
	    return bvec4(r, a, r, g);
	}

	bvec4 rarb() const
	{
	    return bvec4(r, a, r, b);
	}

	bvec4 rara() const
	{
	    return bvec4(r, a, r, a);
	}

	bvec4 ragr() const
	{
	    return bvec4(r, a, g, r);
	}

	bvec4 ragg() const
	{
	    return bvec4(r, a, g, g);
	}

	bvec4 ragb() const
	{
	    return bvec4(r, a, g, b);
	}

	bvec4 raga() const
	{
	    return bvec4(r, a, g, a);
	}

	bvec4 rabr() const
	{
	    return bvec4(r, a, b, r);
	}

	bvec4 rabg() const
	{
	    return bvec4(r, a, b, g);
	}

	bvec4 rabb() const
	{
	    return bvec4(r, a, b, b);
	}

	bvec4 raba() const
	{
	    return bvec4(r, a, b, a);
	}

	bvec4 raar() const
	{
	    return bvec4(r, a, a, r);
	}

	bvec4 raag() const
	{
	    return bvec4(r, a, a, g);
	}

	bvec4 raab() const
	{
	    return bvec4(r, a, a, b);
	}

	bvec4 raaa() const
	{
	    return bvec4(r, a, a, a);
	}

	bvec4 grrr() const
	{
	    return bvec4(g, r, r, r);
	}

	bvec4 grrg() const
	{
	    return bvec4(g, r, r, g);
	}

	bvec4 grrb() const
	{
	    return bvec4(g, r, r, b);
	}

	bvec4 grra() const
	{
	    return bvec4(g, r, r, a);
	}

	bvec4 grgr() const
	{
	    return bvec4(g, r, g, r);
	}

	bvec4 grgg() const
	{
	    return bvec4(g, r, g, g);
	}

	bvec4 grgb() const
	{
	    return bvec4(g, r, g, b);
	}

	bvec4 grga() const
	{
	    return bvec4(g, r, g, a);
	}

	bvec4 grbr() const
	{
	    return bvec4(g, r, b, r);
	}

	bvec4 grbg() const
	{
	    return bvec4(g, r, b, g);
	}

	bvec4 grbb() const
	{
	    return bvec4(g, r, b, b);
	}

	bvec4 grba() const
	{
	    return bvec4(g, r, b, a);
	}

	bvec4 grar() const
	{
	    return bvec4(g, r, a, r);
	}

	bvec4 grag() const
	{
	    return bvec4(g, r, a, g);
	}

	bvec4 grab() const
	{
	    return bvec4(g, r, a, b);
	}

	bvec4 graa() const
	{
	    return bvec4(g, r, a, a);
	}

	bvec4 ggrr() const
	{
	    return bvec4(g, g, r, r);
	}

	bvec4 ggrg() const
	{
	    return bvec4(g, g, r, g);
	}

	bvec4 ggrb() const
	{
	    return bvec4(g, g, r, b);
	}

	bvec4 ggra() const
	{
	    return bvec4(g, g, r, a);
	}

	bvec4 gggr() const
	{
	    return bvec4(g, g, g, r);
	}

	bvec4 gggg() const
	{
	    return bvec4(g, g, g, g);
	}

	bvec4 gggb() const
	{
	    return bvec4(g, g, g, b);
	}

	bvec4 ggga() const
	{
	    return bvec4(g, g, g, a);
	}

	bvec4 ggbr() const
	{
	    return bvec4(g, g, b, r);
	}

	bvec4 ggbg() const
	{
	    return bvec4(g, g, b, g);
	}

	bvec4 ggbb() const
	{
	    return bvec4(g, g, b, b);
	}

	bvec4 ggba() const
	{
	    return bvec4(g, g, b, a);
	}

	bvec4 ggar() const
	{
	    return bvec4(g, g, a, r);
	}

	bvec4 ggag() const
	{
	    return bvec4(g, g, a, g);
	}

	bvec4 ggab() const
	{
	    return bvec4(g, g, a, b);
	}

	bvec4 ggaa() const
	{
	    return bvec4(g, g, a, a);
	}

	bvec4 gbrr() const
	{
	    return bvec4(g, b, r, r);
	}

	bvec4 gbrg() const
	{
	    return bvec4(g, b, r, g);
	}

	bvec4 gbrb() const
	{
	    return bvec4(g, b, r, b);
	}

	bvec4 gbra() const
	{
	    return bvec4(g, b, r, a);
	}

	bvec4 gbgr() const
	{
	    return bvec4(g, b, g, r);
	}

	bvec4 gbgg() const
	{
	    return bvec4(g, b, g, g);
	}

	bvec4 gbgb() const
	{
	    return bvec4(g, b, g, b);
	}

	bvec4 gbga() const
	{
	    return bvec4(g, b, g, a);
	}

	bvec4 gbbr() const
	{
	    return bvec4(g, b, b, r);
	}

	bvec4 gbbg() const
	{
	    return bvec4(g, b, b, g);
	}

	bvec4 gbbb() const
	{
	    return bvec4(g, b, b, b);
	}

	bvec4 gbba() const
	{
	    return bvec4(g, b, b, a);
	}

	bvec4 gbar() const
	{
	    return bvec4(g, b, a, r);
	}

	bvec4 gbag() const
	{
	    return bvec4(g, b, a, g);
	}

	bvec4 gbab() const
	{
	    return bvec4(g, b, a, b);
	}

	bvec4 gbaa() const
	{
	    return bvec4(g, b, a, a);
	}

	bvec4 garr() const
	{
	    return bvec4(g, a, r, r);
	}

	bvec4 garg() const
	{
	    return bvec4(g, a, r, g);
	}

	bvec4 garb() const
	{
	    return bvec4(g, a, r, b);
	}

	bvec4 gara() const
	{
	    return bvec4(g, a, r, a);
	}

	bvec4 gagr() const
	{
	    return bvec4(g, a, g, r);
	}

	bvec4 gagg() const
	{
	    return bvec4(g, a, g, g);
	}

	bvec4 gagb() const
	{
	    return bvec4(g, a, g, b);
	}

	bvec4 gaga() const
	{
	    return bvec4(g, a, g, a);
	}

	bvec4 gabr() const
	{
	    return bvec4(g, a, b, r);
	}

	bvec4 gabg() const
	{
	    return bvec4(g, a, b, g);
	}

	bvec4 gabb() const
	{
	    return bvec4(g, a, b, b);
	}

	bvec4 gaba() const
	{
	    return bvec4(g, a, b, a);
	}

	bvec4 gaar() const
	{
	    return bvec4(g, a, a, r);
	}

	bvec4 gaag() const
	{
	    return bvec4(g, a, a, g);
	}

	bvec4 gaab() const
	{
	    return bvec4(g, a, a, b);
	}

	bvec4 gaaa() const
	{
	    return bvec4(g, a, a, a);
	}

	bvec4 brrr() const
	{
	    return bvec4(b, r, r, r);
	}

	bvec4 brrg() const
	{
	    return bvec4(b, r, r, g);
	}

	bvec4 brrb() const
	{
	    return bvec4(b, r, r, b);
	}

	bvec4 brra() const
	{
	    return bvec4(b, r, r, a);
	}

	bvec4 brgr() const
	{
	    return bvec4(b, r, g, r);
	}

	bvec4 brgg() const
	{
	    return bvec4(b, r, g, g);
	}

	bvec4 brgb() const
	{
	    return bvec4(b, r, g, b);
	}

	bvec4 brga() const
	{
	    return bvec4(b, r, g, a);
	}

	bvec4 brbr() const
	{
	    return bvec4(b, r, b, r);
	}

	bvec4 brbg() const
	{
	    return bvec4(b, r, b, g);
	}

	bvec4 brbb() const
	{
	    return bvec4(b, r, b, b);
	}

	bvec4 brba() const
	{
	    return bvec4(b, r, b, a);
	}

	bvec4 brar() const
	{
	    return bvec4(b, r, a, r);
	}

	bvec4 brag() const
	{
	    return bvec4(b, r, a, g);
	}

	bvec4 brab() const
	{
	    return bvec4(b, r, a, b);
	}

	bvec4 braa() const
	{
	    return bvec4(b, r, a, a);
	}

	bvec4 bgrr() const
	{
	    return bvec4(b, g, r, r);
	}

	bvec4 bgrg() const
	{
	    return bvec4(b, g, r, g);
	}

	bvec4 bgrb() const
	{
	    return bvec4(b, g, r, b);
	}

	bvec4 bgra() const
	{
	    return bvec4(b, g, r, a);
	}

	bvec4 bggr() const
	{
	    return bvec4(b, g, g, r);
	}

	bvec4 bggg() const
	{
	    return bvec4(b, g, g, g);
	}

	bvec4 bggb() const
	{
	    return bvec4(b, g, g, b);
	}

	bvec4 bgga() const
	{
	    return bvec4(b, g, g, a);
	}

	bvec4 bgbr() const
	{
	    return bvec4(b, g, b, r);
	}

	bvec4 bgbg() const
	{
	    return bvec4(b, g, b, g);
	}

	bvec4 bgbb() const
	{
	    return bvec4(b, g, b, b);
	}

	bvec4 bgba() const
	{
	    return bvec4(b, g, b, a);
	}

	bvec4 bgar() const
	{
	    return bvec4(b, g, a, r);
	}

	bvec4 bgag() const
	{
	    return bvec4(b, g, a, g);
	}

	bvec4 bgab() const
	{
	    return bvec4(b, g, a, b);
	}

	bvec4 bgaa() const
	{
	    return bvec4(b, g, a, a);
	}

	bvec4 bbrr() const
	{
	    return bvec4(b, b, r, r);
	}

	bvec4 bbrg() const
	{
	    return bvec4(b, b, r, g);
	}

	bvec4 bbrb() const
	{
	    return bvec4(b, b, r, b);
	}

	bvec4 bbra() const
	{
	    return bvec4(b, b, r, a);
	}

	bvec4 bbgr() const
	{
	    return bvec4(b, b, g, r);
	}

	bvec4 bbgg() const
	{
	    return bvec4(b, b, g, g);
	}

	bvec4 bbgb() const
	{
	    return bvec4(b, b, g, b);
	}

	bvec4 bbga() const
	{
	    return bvec4(b, b, g, a);
	}

	bvec4 bbbr() const
	{
	    return bvec4(b, b, b, r);
	}

	bvec4 bbbg() const
	{
	    return bvec4(b, b, b, g);
	}

	bvec4 bbbb() const
	{
	    return bvec4(b, b, b, b);
	}

	bvec4 bbba() const
	{
	    return bvec4(b, b, b, a);
	}

	bvec4 bbar() const
	{
	    return bvec4(b, b, a, r);
	}

	bvec4 bbag() const
	{
	    return bvec4(b, b, a, g);
	}

	bvec4 bbab() const
	{
	    return bvec4(b, b, a, b);
	}

	bvec4 bbaa() const
	{
	    return bvec4(b, b, a, a);
	}

	bvec4 barr() const
	{
	    return bvec4(b, a, r, r);
	}

	bvec4 barg() const
	{
	    return bvec4(b, a, r, g);
	}

	bvec4 barb() const
	{
	    return bvec4(b, a, r, b);
	}

	bvec4 bara() const
	{
	    return bvec4(b, a, r, a);
	}

	bvec4 bagr() const
	{
	    return bvec4(b, a, g, r);
	}

	bvec4 bagg() const
	{
	    return bvec4(b, a, g, g);
	}

	bvec4 bagb() const
	{
	    return bvec4(b, a, g, b);
	}

	bvec4 baga() const
	{
	    return bvec4(b, a, g, a);
	}

	bvec4 babr() const
	{
	    return bvec4(b, a, b, r);
	}

	bvec4 babg() const
	{
	    return bvec4(b, a, b, g);
	}

	bvec4 babb() const
	{
	    return bvec4(b, a, b, b);
	}

	bvec4 baba() const
	{
	    return bvec4(b, a, b, a);
	}

	bvec4 baar() const
	{
	    return bvec4(b, a, a, r);
	}

	bvec4 baag() const
	{
	    return bvec4(b, a, a, g);
	}

	bvec4 baab() const
	{
	    return bvec4(b, a, a, b);
	}

	bvec4 baaa() const
	{
	    return bvec4(b, a, a, a);
	}

	bvec4 arrr() const
	{
	    return bvec4(a, r, r, r);
	}

	bvec4 arrg() const
	{
	    return bvec4(a, r, r, g);
	}

	bvec4 arrb() const
	{
	    return bvec4(a, r, r, b);
	}

	bvec4 arra() const
	{
	    return bvec4(a, r, r, a);
	}

	bvec4 argr() const
	{
	    return bvec4(a, r, g, r);
	}

	bvec4 argg() const
	{
	    return bvec4(a, r, g, g);
	}

	bvec4 argb() const
	{
	    return bvec4(a, r, g, b);
	}

	bvec4 arga() const
	{
	    return bvec4(a, r, g, a);
	}

	bvec4 arbr() const
	{
	    return bvec4(a, r, b, r);
	}

	bvec4 arbg() const
	{
	    return bvec4(a, r, b, g);
	}

	bvec4 arbb() const
	{
	    return bvec4(a, r, b, b);
	}

	bvec4 arba() const
	{
	    return bvec4(a, r, b, a);
	}

	bvec4 arar() const
	{
	    return bvec4(a, r, a, r);
	}

	bvec4 arag() const
	{
	    return bvec4(a, r, a, g);
	}

	bvec4 arab() const
	{
	    return bvec4(a, r, a, b);
	}

	bvec4 araa() const
	{
	    return bvec4(a, r, a, a);
	}

	bvec4 agrr() const
	{
	    return bvec4(a, g, r, r);
	}

	bvec4 agrg() const
	{
	    return bvec4(a, g, r, g);
	}

	bvec4 agrb() const
	{
	    return bvec4(a, g, r, b);
	}

	bvec4 agra() const
	{
	    return bvec4(a, g, r, a);
	}

	bvec4 aggr() const
	{
	    return bvec4(a, g, g, r);
	}

	bvec4 aggg() const
	{
	    return bvec4(a, g, g, g);
	}

	bvec4 aggb() const
	{
	    return bvec4(a, g, g, b);
	}

	bvec4 agga() const
	{
	    return bvec4(a, g, g, a);
	}

	bvec4 agbr() const
	{
	    return bvec4(a, g, b, r);
	}

	bvec4 agbg() const
	{
	    return bvec4(a, g, b, g);
	}

	bvec4 agbb() const
	{
	    return bvec4(a, g, b, b);
	}

	bvec4 agba() const
	{
	    return bvec4(a, g, b, a);
	}

	bvec4 agar() const
	{
	    return bvec4(a, g, a, r);
	}

	bvec4 agag() const
	{
	    return bvec4(a, g, a, g);
	}

	bvec4 agab() const
	{
	    return bvec4(a, g, a, b);
	}

	bvec4 agaa() const
	{
	    return bvec4(a, g, a, a);
	}

	bvec4 abrr() const
	{
	    return bvec4(a, b, r, r);
	}

	bvec4 abrg() const
	{
	    return bvec4(a, b, r, g);
	}

	bvec4 abrb() const
	{
	    return bvec4(a, b, r, b);
	}

	bvec4 abra() const
	{
	    return bvec4(a, b, r, a);
	}

	bvec4 abgr() const
	{
	    return bvec4(a, b, g, r);
	}

	bvec4 abgg() const
	{
	    return bvec4(a, b, g, g);
	}

	bvec4 abgb() const
	{
	    return bvec4(a, b, g, b);
	}

	bvec4 abga() const
	{
	    return bvec4(a, b, g, a);
	}

	bvec4 abbr() const
	{
	    return bvec4(a, b, b, r);
	}

	bvec4 abbg() const
	{
	    return bvec4(a, b, b, g);
	}

	bvec4 abbb() const
	{
	    return bvec4(a, b, b, b);
	}

	bvec4 abba() const
	{
	    return bvec4(a, b, b, a);
	}

	bvec4 abar() const
	{
	    return bvec4(a, b, a, r);
	}

	bvec4 abag() const
	{
	    return bvec4(a, b, a, g);
	}

	bvec4 abab() const
	{
	    return bvec4(a, b, a, b);
	}

	bvec4 abaa() const
	{
	    return bvec4(a, b, a, a);
	}

	bvec4 aarr() const
	{
	    return bvec4(a, a, r, r);
	}

	bvec4 aarg() const
	{
	    return bvec4(a, a, r, g);
	}

	bvec4 aarb() const
	{
	    return bvec4(a, a, r, b);
	}

	bvec4 aara() const
	{
	    return bvec4(a, a, r, a);
	}

	bvec4 aagr() const
	{
	    return bvec4(a, a, g, r);
	}

	bvec4 aagg() const
	{
	    return bvec4(a, a, g, g);
	}

	bvec4 aagb() const
	{
	    return bvec4(a, a, g, b);
	}

	bvec4 aaga() const
	{
	    return bvec4(a, a, g, a);
	}

	bvec4 aabr() const
	{
	    return bvec4(a, a, b, r);
	}

	bvec4 aabg() const
	{
	    return bvec4(a, a, b, g);
	}

	bvec4 aabb() const
	{
	    return bvec4(a, a, b, b);
	}

	bvec4 aaba() const
	{
	    return bvec4(a, a, b, a);
	}

	bvec4 aaar() const
	{
	    return bvec4(a, a, a, r);
	}

	bvec4 aaag() const
	{
	    return bvec4(a, a, a, g);
	}

	bvec4 aaab() const
	{
	    return bvec4(a, a, a, b);
	}

	bvec4 aaaa() const
	{
	    return bvec4(a, a, a, a);
	}

	bvec2 ss() const
	{
	    return bvec2(s, s);
	}

	bvec2 st() const
	{
	    return bvec2(s, t);
	}

	bvec2 sp() const
	{
	    return bvec2(s, p);
	}

	bvec2 sq() const
	{
	    return bvec2(s, q);
	}

	bvec2 ts() const
	{
	    return bvec2(t, s);
	}

	bvec2 tt() const
	{
	    return bvec2(t, t);
	}

	bvec2 tp() const
	{
	    return bvec2(t, p);
	}

	bvec2 tq() const
	{
	    return bvec2(t, q);
	}

	bvec2 ps() const
	{
	    return bvec2(p, s);
	}

	bvec2 pt() const
	{
	    return bvec2(p, t);
	}

	bvec2 pp() const
	{
	    return bvec2(p, p);
	}

	bvec2 pq() const
	{
	    return bvec2(p, q);
	}

	bvec2 qs() const
	{
	    return bvec2(q, s);
	}

	bvec2 qt() const
	{
	    return bvec2(q, t);
	}

	bvec2 qp() const
	{
	    return bvec2(q, p);
	}

	bvec2 qq() const
	{
	    return bvec2(q, q);
	}

	bvec3 sss() const
	{
	    return bvec3(s, s, s);
	}

	bvec3 sst() const
	{
	    return bvec3(s, s, t);
	}

	bvec3 ssp() const
	{
	    return bvec3(s, s, p);
	}

	bvec3 ssq() const
	{
	    return bvec3(s, s, q);
	}

	bvec3 sts() const
	{
	    return bvec3(s, t, s);
	}

	bvec3 stt() const
	{
	    return bvec3(s, t, t);
	}

	bvec3 stp() const
	{
	    return bvec3(s, t, p);
	}

	bvec3 stq() const
	{
	    return bvec3(s, t, q);
	}

	bvec3 sps() const
	{
	    return bvec3(s, p, s);
	}

	bvec3 spt() const
	{
	    return bvec3(s, p, t);
	}

	bvec3 spp() const
	{
	    return bvec3(s, p, p);
	}

	bvec3 spq() const
	{
	    return bvec3(s, q, q);
	}

	bvec3 sqs() const
	{
	    return bvec3(s, q, s);
	}

	bvec3 sqt() const
	{
	    return bvec3(s, q, t);
	}

	bvec3 sqp() const
	{
	    return bvec3(s, q, p);
	}

	bvec3 sqq() const
	{
	    return bvec3(s, q, q);
	}

	bvec3 tss() const
	{
	    return bvec3(t, s, s);
	}

	bvec3 tst() const
	{
	    return bvec3(t, s, t);
	}

	bvec3 tsp() const
	{
	    return bvec3(t, s, p);
	}

	bvec3 tsq() const
	{
	    return bvec3(t, s, q);
	}

	bvec3 tts() const
	{
	    return bvec3(t, t, s);
	}

	bvec3 ttt() const
	{
	    return bvec3(t, t, t);
	}

	bvec3 ttp() const
	{
	    return bvec3(t, t, p);
	}

	bvec3 ttq() const
	{
	    return bvec3(t, t, q);
	}

	bvec3 tps() const
	{
	    return bvec3(t, p, s);
	}

	bvec3 tpt() const
	{
	    return bvec3(t, p, t);
	}

	bvec3 tpp() const
	{
	    return bvec3(t, p, p);
	}

	bvec3 tpq() const
	{
	    return bvec3(t, q, q);
	}

	bvec3 tqs() const
	{
	    return bvec3(t, q, s);
	}

	bvec3 tqt() const
	{
	    return bvec3(t, q, t);
	}

	bvec3 tqp() const
	{
	    return bvec3(t, q, p);
	}

	bvec3 tqq() const
	{
	    return bvec3(t, q, q);
	}

	bvec3 pss() const
	{
	    return bvec3(p, s, s);
	}

	bvec3 pst() const
	{
	    return bvec3(p, s, t);
	}

	bvec3 psp() const
	{
	    return bvec3(p, s, p);
	}

	bvec3 psq() const
	{
	    return bvec3(p, s, q);
	}

	bvec3 pts() const
	{
	    return bvec3(p, t, s);
	}

	bvec3 ptt() const
	{
	    return bvec3(p, t, t);
	}

	bvec3 ptp() const
	{
	    return bvec3(p, t, p);
	}

	bvec3 ptq() const
	{
	    return bvec3(p, t, q);
	}

	bvec3 pps() const
	{
	    return bvec3(p, p, s);
	}

	bvec3 ppt() const
	{
	    return bvec3(p, p, t);
	}

	bvec3 ppp() const
	{
	    return bvec3(p, p, p);
	}

	bvec3 ppq() const
	{
	    return bvec3(p, q, q);
	}

	bvec3 pqs() const
	{
	    return bvec3(p, q, s);
	}

	bvec3 pqt() const
	{
	    return bvec3(p, q, t);
	}

	bvec3 pqp() const
	{
	    return bvec3(p, q, p);
	}

	bvec3 pqq() const
	{
	    return bvec3(p, q, q);
	}

	bvec3 qss() const
	{
	    return bvec3(q, s, s);
	}

	bvec3 qst() const
	{
	    return bvec3(q, s, t);
	}

	bvec3 qsp() const
	{
	    return bvec3(q, s, p);
	}

	bvec3 qsq() const
	{
	    return bvec3(q, s, q);
	}

	bvec3 qts() const
	{
	    return bvec3(q, t, s);
	}

	bvec3 qtt() const
	{
	    return bvec3(q, t, t);
	}

	bvec3 qtp() const
	{
	    return bvec3(q, t, p);
	}

	bvec3 qtq() const
	{
	    return bvec3(q, t, q);
	}

	bvec3 qps() const
	{
	    return bvec3(q, p, s);
	}

	bvec3 qpt() const
	{
	    return bvec3(q, p, t);
	}

	bvec3 qpp() const
	{
	    return bvec3(q, p, p);
	}

	bvec3 qpq() const
	{
	    return bvec3(q, q, q);
	}

	bvec3 qqs() const
	{
	    return bvec3(q, q, s);
	}

	bvec3 qqt() const
	{
	    return bvec3(q, q, t);
	}

	bvec3 qqp() const
	{
	    return bvec3(q, q, p);
	}

	bvec3 qqq() const
	{
	    return bvec3(q, q, q);
	}

	bvec4 ssss() const
	{
	    return bvec4(s, s, s, s);
	}

	bvec4 ssst() const
	{
	    return bvec4(s, s, s, t);
	}

	bvec4 sssp() const
	{
	    return bvec4(s, s, s, p);
	}

	bvec4 sssq() const
	{
	    return bvec4(s, s, s, q);
	}

	bvec4 ssts() const
	{
	    return bvec4(s, s, t, s);
	}

	bvec4 sstt() const
	{
	    return bvec4(s, s, t, t);
	}

	bvec4 sstp() const
	{
	    return bvec4(s, s, t, p);
	}

	bvec4 sstq() const
	{
	    return bvec4(s, s, t, q);
	}

	bvec4 ssps() const
	{
	    return bvec4(s, s, p, s);
	}

	bvec4 sspt() const
	{
	    return bvec4(s, s, p, t);
	}

	bvec4 sspp() const
	{
	    return bvec4(s, s, p, p);
	}

	bvec4 sspq() const
	{
	    return bvec4(s, s, p, q);
	}

	bvec4 ssqs() const
	{
	    return bvec4(s, s, q, s);
	}

	bvec4 ssqt() const
	{
	    return bvec4(s, s, q, t);
	}

	bvec4 ssqp() const
	{
	    return bvec4(s, s, q, p);
	}

	bvec4 ssqq() const
	{
	    return bvec4(s, s, q, q);
	}

	bvec4 stss() const
	{
	    return bvec4(s, t, s, s);
	}

	bvec4 stst() const
	{
	    return bvec4(s, t, s, t);
	}

	bvec4 stsp() const
	{
	    return bvec4(s, t, s, p);
	}

	bvec4 stsq() const
	{
	    return bvec4(s, t, s, q);
	}

	bvec4 stts() const
	{
	    return bvec4(s, t, t, s);
	}

	bvec4 sttt() const
	{
	    return bvec4(s, t, t, t);
	}

	bvec4 sttp() const
	{
	    return bvec4(s, t, t, p);
	}

	bvec4 sttq() const
	{
	    return bvec4(s, t, t, q);
	}

	bvec4 stps() const
	{
	    return bvec4(s, t, p, s);
	}

	bvec4 stpt() const
	{
	    return bvec4(s, t, p, t);
	}

	bvec4 stpp() const
	{
	    return bvec4(s, t, p, p);
	}

	bvec4 stpq() const
	{
	    return bvec4(s, t, p, q);
	}

	bvec4 stqs() const
	{
	    return bvec4(s, t, q, s);
	}

	bvec4 stqt() const
	{
	    return bvec4(s, t, q, t);
	}

	bvec4 stqp() const
	{
	    return bvec4(s, t, q, p);
	}

	bvec4 stqq() const
	{
	    return bvec4(s, t, q, q);
	}

	bvec4 spss() const
	{
	    return bvec4(s, p, s, s);
	}

	bvec4 spst() const
	{
	    return bvec4(s, p, s, t);
	}

	bvec4 spsp() const
	{
	    return bvec4(s, p, s, p);
	}

	bvec4 spsq() const
	{
	    return bvec4(s, p, s, q);
	}

	bvec4 spts() const
	{
	    return bvec4(s, p, t, s);
	}

	bvec4 sptt() const
	{
	    return bvec4(s, p, t, t);
	}

	bvec4 sptp() const
	{
	    return bvec4(s, p, t, p);
	}

	bvec4 sptq() const
	{
	    return bvec4(s, p, t, q);
	}

	bvec4 spps() const
	{
	    return bvec4(s, p, p, s);
	}

	bvec4 sppt() const
	{
	    return bvec4(s, p, p, t);
	}

	bvec4 sppp() const
	{
	    return bvec4(s, p, p, p);
	}

	bvec4 sppq() const
	{
	    return bvec4(s, p, p, q);
	}

	bvec4 spqs() const
	{
	    return bvec4(s, p, q, s);
	}

	bvec4 spqt() const
	{
	    return bvec4(s, p, q, t);
	}

	bvec4 spqp() const
	{
	    return bvec4(s, p, q, p);
	}

	bvec4 spqq() const
	{
	    return bvec4(s, p, q, q);
	}

	bvec4 sqss() const
	{
	    return bvec4(s, q, s, s);
	}

	bvec4 sqst() const
	{
	    return bvec4(s, q, s, t);
	}

	bvec4 sqsp() const
	{
	    return bvec4(s, q, s, p);
	}

	bvec4 sqsq() const
	{
	    return bvec4(s, q, s, q);
	}

	bvec4 sqts() const
	{
	    return bvec4(s, q, t, s);
	}

	bvec4 sqtt() const
	{
	    return bvec4(s, q, t, t);
	}

	bvec4 sqtp() const
	{
	    return bvec4(s, q, t, p);
	}

	bvec4 sqtq() const
	{
	    return bvec4(s, q, t, q);
	}

	bvec4 sqps() const
	{
	    return bvec4(s, q, p, s);
	}

	bvec4 sqpt() const
	{
	    return bvec4(s, q, p, t);
	}

	bvec4 sqpp() const
	{
	    return bvec4(s, q, p, p);
	}

	bvec4 sqpq() const
	{
	    return bvec4(s, q, p, q);
	}

	bvec4 sqqs() const
	{
	    return bvec4(s, q, q, s);
	}

	bvec4 sqqt() const
	{
	    return bvec4(s, q, q, t);
	}

	bvec4 sqqp() const
	{
	    return bvec4(s, q, q, p);
	}

	bvec4 sqqq() const
	{
	    return bvec4(s, q, q, q);
	}

	bvec4 tsss() const
	{
	    return bvec4(t, s, s, s);
	}

	bvec4 tsst() const
	{
	    return bvec4(t, s, s, t);
	}

	bvec4 tssp() const
	{
	    return bvec4(t, s, s, p);
	}

	bvec4 tssq() const
	{
	    return bvec4(t, s, s, q);
	}

	bvec4 tsts() const
	{
	    return bvec4(t, s, t, s);
	}

	bvec4 tstt() const
	{
	    return bvec4(t, s, t, t);
	}

	bvec4 tstp() const
	{
	    return bvec4(t, s, t, p);
	}

	bvec4 tstq() const
	{
	    return bvec4(t, s, t, q);
	}

	bvec4 tsps() const
	{
	    return bvec4(t, s, p, s);
	}

	bvec4 tspt() const
	{
	    return bvec4(t, s, p, t);
	}

	bvec4 tspp() const
	{
	    return bvec4(t, s, p, p);
	}

	bvec4 tspq() const
	{
	    return bvec4(t, s, p, q);
	}

	bvec4 tsqs() const
	{
	    return bvec4(t, s, q, s);
	}

	bvec4 tsqt() const
	{
	    return bvec4(t, s, q, t);
	}

	bvec4 tsqp() const
	{
	    return bvec4(t, s, q, p);
	}

	bvec4 tsqq() const
	{
	    return bvec4(t, s, q, q);
	}

	bvec4 ttss() const
	{
	    return bvec4(t, t, s, s);
	}

	bvec4 ttst() const
	{
	    return bvec4(t, t, s, t);
	}

	bvec4 ttsp() const
	{
	    return bvec4(t, t, s, p);
	}

	bvec4 ttsq() const
	{
	    return bvec4(t, t, s, q);
	}

	bvec4 ttts() const
	{
	    return bvec4(t, t, t, s);
	}

	bvec4 tttt() const
	{
	    return bvec4(t, t, t, t);
	}

	bvec4 tttp() const
	{
	    return bvec4(t, t, t, p);
	}

	bvec4 tttq() const
	{
	    return bvec4(t, t, t, q);
	}

	bvec4 ttps() const
	{
	    return bvec4(t, t, p, s);
	}

	bvec4 ttpt() const
	{
	    return bvec4(t, t, p, t);
	}

	bvec4 ttpp() const
	{
	    return bvec4(t, t, p, p);
	}

	bvec4 ttpq() const
	{
	    return bvec4(t, t, p, q);
	}

	bvec4 ttqs() const
	{
	    return bvec4(t, t, q, s);
	}

	bvec4 ttqt() const
	{
	    return bvec4(t, t, q, t);
	}

	bvec4 ttqp() const
	{
	    return bvec4(t, t, q, p);
	}

	bvec4 ttqq() const
	{
	    return bvec4(t, t, q, q);
	}

	bvec4 tpss() const
	{
	    return bvec4(t, p, s, s);
	}

	bvec4 tpst() const
	{
	    return bvec4(t, p, s, t);
	}

	bvec4 tpsp() const
	{
	    return bvec4(t, p, s, p);
	}

	bvec4 tpsq() const
	{
	    return bvec4(t, p, s, q);
	}

	bvec4 tpts() const
	{
	    return bvec4(t, p, t, s);
	}

	bvec4 tptt() const
	{
	    return bvec4(t, p, t, t);
	}

	bvec4 tptp() const
	{
	    return bvec4(t, p, t, p);
	}

	bvec4 tptq() const
	{
	    return bvec4(t, p, t, q);
	}

	bvec4 tpps() const
	{
	    return bvec4(t, p, p, s);
	}

	bvec4 tppt() const
	{
	    return bvec4(t, p, p, t);
	}

	bvec4 tppp() const
	{
	    return bvec4(t, p, p, p);
	}

	bvec4 tppq() const
	{
	    return bvec4(t, p, p, q);
	}

	bvec4 tpqs() const
	{
	    return bvec4(t, p, q, s);
	}

	bvec4 tpqt() const
	{
	    return bvec4(t, p, q, t);
	}

	bvec4 tpqp() const
	{
	    return bvec4(t, p, q, p);
	}

	bvec4 tpqq() const
	{
	    return bvec4(t, p, q, q);
	}

	bvec4 tqss() const
	{
	    return bvec4(t, q, s, s);
	}

	bvec4 tqst() const
	{
	    return bvec4(t, q, s, t);
	}

	bvec4 tqsp() const
	{
	    return bvec4(t, q, s, p);
	}

	bvec4 tqsq() const
	{
	    return bvec4(t, q, s, q);
	}

	bvec4 tqts() const
	{
	    return bvec4(t, q, t, s);
	}

	bvec4 tqtt() const
	{
	    return bvec4(t, q, t, t);
	}

	bvec4 tqtp() const
	{
	    return bvec4(t, q, t, p);
	}

	bvec4 tqtq() const
	{
	    return bvec4(t, q, t, q);
	}

	bvec4 tqps() const
	{
	    return bvec4(t, q, p, s);
	}

	bvec4 tqpt() const
	{
	    return bvec4(t, q, p, t);
	}

	bvec4 tqpp() const
	{
	    return bvec4(t, q, p, p);
	}

	bvec4 tqpq() const
	{
	    return bvec4(t, q, p, q);
	}

	bvec4 tqqs() const
	{
	    return bvec4(t, q, q, s);
	}

	bvec4 tqqt() const
	{
	    return bvec4(t, q, q, t);
	}

	bvec4 tqqp() const
	{
	    return bvec4(t, q, q, p);
	}

	bvec4 tqqq() const
	{
	    return bvec4(t, q, q, q);
	}

	bvec4 psss() const
	{
	    return bvec4(p, s, s, s);
	}

	bvec4 psst() const
	{
	    return bvec4(p, s, s, t);
	}

	bvec4 pssp() const
	{
	    return bvec4(p, s, s, p);
	}

	bvec4 pssq() const
	{
	    return bvec4(p, s, s, q);
	}

	bvec4 psts() const
	{
	    return bvec4(p, s, t, s);
	}

	bvec4 pstt() const
	{
	    return bvec4(p, s, t, t);
	}

	bvec4 pstp() const
	{
	    return bvec4(p, s, t, p);
	}

	bvec4 pstq() const
	{
	    return bvec4(p, s, t, q);
	}

	bvec4 psps() const
	{
	    return bvec4(p, s, p, s);
	}

	bvec4 pspt() const
	{
	    return bvec4(p, s, p, t);
	}

	bvec4 pspp() const
	{
	    return bvec4(p, s, p, p);
	}

	bvec4 pspq() const
	{
	    return bvec4(p, s, p, q);
	}

	bvec4 psqs() const
	{
	    return bvec4(p, s, q, s);
	}

	bvec4 psqt() const
	{
	    return bvec4(p, s, q, t);
	}

	bvec4 psqp() const
	{
	    return bvec4(p, s, q, p);
	}

	bvec4 psqq() const
	{
	    return bvec4(p, s, q, q);
	}

	bvec4 ptss() const
	{
	    return bvec4(p, t, s, s);
	}

	bvec4 ptst() const
	{
	    return bvec4(p, t, s, t);
	}

	bvec4 ptsp() const
	{
	    return bvec4(p, t, s, p);
	}

	bvec4 ptsq() const
	{
	    return bvec4(p, t, s, q);
	}

	bvec4 ptts() const
	{
	    return bvec4(p, t, t, s);
	}

	bvec4 pttt() const
	{
	    return bvec4(p, t, t, t);
	}

	bvec4 pttp() const
	{
	    return bvec4(p, t, t, p);
	}

	bvec4 pttq() const
	{
	    return bvec4(p, t, t, q);
	}

	bvec4 ptps() const
	{
	    return bvec4(p, t, p, s);
	}

	bvec4 ptpt() const
	{
	    return bvec4(p, t, p, t);
	}

	bvec4 ptpp() const
	{
	    return bvec4(p, t, p, p);
	}

	bvec4 ptpq() const
	{
	    return bvec4(p, t, p, q);
	}

	bvec4 ptqs() const
	{
	    return bvec4(p, t, q, s);
	}

	bvec4 ptqt() const
	{
	    return bvec4(p, t, q, t);
	}

	bvec4 ptqp() const
	{
	    return bvec4(p, t, q, p);
	}

	bvec4 ptqq() const
	{
	    return bvec4(p, t, q, q);
	}

	bvec4 ppss() const
	{
	    return bvec4(p, p, s, s);
	}

	bvec4 ppst() const
	{
	    return bvec4(p, p, s, t);
	}

	bvec4 ppsp() const
	{
	    return bvec4(p, p, s, p);
	}

	bvec4 ppsq() const
	{
	    return bvec4(p, p, s, q);
	}

	bvec4 ppts() const
	{
	    return bvec4(p, p, t, s);
	}

	bvec4 pptt() const
	{
	    return bvec4(p, p, t, t);
	}

	bvec4 pptp() const
	{
	    return bvec4(p, p, t, p);
	}

	bvec4 pptq() const
	{
	    return bvec4(p, p, t, q);
	}

	bvec4 ppps() const
	{
	    return bvec4(p, p, p, s);
	}

	bvec4 pppt() const
	{
	    return bvec4(p, p, p, t);
	}

	bvec4 pppp() const
	{
	    return bvec4(p, p, p, p);
	}

	bvec4 pppq() const
	{
	    return bvec4(p, p, p, q);
	}

	bvec4 ppqs() const
	{
	    return bvec4(p, p, q, s);
	}

	bvec4 ppqt() const
	{
	    return bvec4(p, p, q, t);
	}

	bvec4 ppqp() const
	{
	    return bvec4(p, p, q, p);
	}

	bvec4 ppqq() const
	{
	    return bvec4(p, p, q, q);
	}

	bvec4 pqss() const
	{
	    return bvec4(p, q, s, s);
	}

	bvec4 pqst() const
	{
	    return bvec4(p, q, s, t);
	}

	bvec4 pqsp() const
	{
	    return bvec4(p, q, s, p);
	}

	bvec4 pqsq() const
	{
	    return bvec4(p, q, s, q);
	}

	bvec4 pqts() const
	{
	    return bvec4(p, q, t, s);
	}

	bvec4 pqtt() const
	{
	    return bvec4(p, q, t, t);
	}

	bvec4 pqtp() const
	{
	    return bvec4(p, q, t, p);
	}

	bvec4 pqtq() const
	{
	    return bvec4(p, q, t, q);
	}

	bvec4 pqps() const
	{
	    return bvec4(p, q, p, s);
	}

	bvec4 pqpt() const
	{
	    return bvec4(p, q, p, t);
	}

	bvec4 pqpp() const
	{
	    return bvec4(p, q, p, p);
	}

	bvec4 pqpq() const
	{
	    return bvec4(p, q, p, q);
	}

	bvec4 pqqs() const
	{
	    return bvec4(p, q, q, s);
	}

	bvec4 pqqt() const
	{
	    return bvec4(p, q, q, t);
	}

	bvec4 pqqp() const
	{
	    return bvec4(p, q, q, p);
	}

	bvec4 pqqq() const
	{
	    return bvec4(p, q, q, q);
	}

	bvec4 qsss() const
	{
	    return bvec4(q, s, s, s);
	}

	bvec4 qsst() const
	{
	    return bvec4(q, s, s, t);
	}

	bvec4 qssp() const
	{
	    return bvec4(q, s, s, p);
	}

	bvec4 qssq() const
	{
	    return bvec4(q, s, s, q);
	}

	bvec4 qsts() const
	{
	    return bvec4(q, s, t, s);
	}

	bvec4 qstt() const
	{
	    return bvec4(q, s, t, t);
	}

	bvec4 qstp() const
	{
	    return bvec4(q, s, t, p);
	}

	bvec4 qstq() const
	{
	    return bvec4(q, s, t, q);
	}

	bvec4 qsps() const
	{
	    return bvec4(q, s, p, s);
	}

	bvec4 qspt() const
	{
	    return bvec4(q, s, p, t);
	}

	bvec4 qspp() const
	{
	    return bvec4(q, s, p, p);
	}

	bvec4 qspq() const
	{
	    return bvec4(q, s, p, q);
	}

	bvec4 qsqs() const
	{
	    return bvec4(q, s, q, s);
	}

	bvec4 qsqt() const
	{
	    return bvec4(q, s, q, t);
	}

	bvec4 qsqp() const
	{
	    return bvec4(q, s, q, p);
	}

	bvec4 qsqq() const
	{
	    return bvec4(q, s, q, q);
	}

	bvec4 qtss() const
	{
	    return bvec4(q, t, s, s);
	}

	bvec4 qtst() const
	{
	    return bvec4(q, t, s, t);
	}

	bvec4 qtsp() const
	{
	    return bvec4(q, t, s, p);
	}

	bvec4 qtsq() const
	{
	    return bvec4(q, t, s, q);
	}

	bvec4 qtts() const
	{
	    return bvec4(q, t, t, s);
	}

	bvec4 qttt() const
	{
	    return bvec4(q, t, t, t);
	}

	bvec4 qttp() const
	{
	    return bvec4(q, t, t, p);
	}

	bvec4 qttq() const
	{
	    return bvec4(q, t, t, q);
	}

	bvec4 qtps() const
	{
	    return bvec4(q, t, p, s);
	}

	bvec4 qtpt() const
	{
	    return bvec4(q, t, p, t);
	}

	bvec4 qtpp() const
	{
	    return bvec4(q, t, p, p);
	}

	bvec4 qtpq() const
	{
	    return bvec4(q, t, p, q);
	}

	bvec4 qtqs() const
	{
	    return bvec4(q, t, q, s);
	}

	bvec4 qtqt() const
	{
	    return bvec4(q, t, q, t);
	}

	bvec4 qtqp() const
	{
	    return bvec4(q, t, q, p);
	}

	bvec4 qtqq() const
	{
	    return bvec4(q, t, q, q);
	}

	bvec4 qpss() const
	{
	    return bvec4(q, p, s, s);
	}

	bvec4 qpst() const
	{
	    return bvec4(q, p, s, t);
	}

	bvec4 qpsp() const
	{
	    return bvec4(q, p, s, p);
	}

	bvec4 qpsq() const
	{
	    return bvec4(q, p, s, q);
	}

	bvec4 qpts() const
	{
	    return bvec4(q, p, t, s);
	}

	bvec4 qptt() const
	{
	    return bvec4(q, p, t, t);
	}

	bvec4 qptp() const
	{
	    return bvec4(q, p, t, p);
	}

	bvec4 qptq() const
	{
	    return bvec4(q, p, t, q);
	}

	bvec4 qpps() const
	{
	    return bvec4(q, p, p, s);
	}

	bvec4 qppt() const
	{
	    return bvec4(q, p, p, t);
	}

	bvec4 qppp() const
	{
	    return bvec4(q, p, p, p);
	}

	bvec4 qppq() const
	{
	    return bvec4(q, p, p, q);
	}

	bvec4 qpqs() const
	{
	    return bvec4(q, p, q, s);
	}

	bvec4 qpqt() const
	{
	    return bvec4(q, p, q, t);
	}

	bvec4 qpqp() const
	{
	    return bvec4(q, p, q, p);
	}

	bvec4 qpqq() const
	{
	    return bvec4(q, p, q, q);
	}

	bvec4 qqss() const
	{
	    return bvec4(q, q, s, s);
	}

	bvec4 qqst() const
	{
	    return bvec4(q, q, s, t);
	}

	bvec4 qqsp() const
	{
	    return bvec4(q, q, s, p);
	}

	bvec4 qqsq() const
	{
	    return bvec4(q, q, s, q);
	}

	bvec4 qqts() const
	{
	    return bvec4(q, q, t, s);
	}

	bvec4 qqtt() const
	{
	    return bvec4(q, q, t, t);
	}

	bvec4 qqtp() const
	{
	    return bvec4(q, q, t, p);
	}

	bvec4 qqtq() const
	{
	    return bvec4(q, q, t, q);
	}

	bvec4 qqps() const
	{
	    return bvec4(q, q, p, s);
	}

	bvec4 qqpt() const
	{
	    return bvec4(q, q, p, t);
	}

	bvec4 qqpp() const
	{
	    return bvec4(q, q, p, p);
	}

	bvec4 qqpq() const
	{
	    return bvec4(q, q, p, q);
	}

	bvec4 qqqs() const
	{
	    return bvec4(q, q, q, s);
	}

	bvec4 qqqt() const
	{
	    return bvec4(q, q, q, t);
	}

	bvec4 qqqp() const
	{
	    return bvec4(q, q, q, p);
	}

	bvec4 qqqq() const
	{
	    return bvec4(q, q, q, q);
	}

	/* Functions */

	bool any() const
	{
	    return x || y || z || w;
	}

	bool all() const
	{
	    return x && y && z && w;
	}

	bvec4 negate() const
	{
	    return bvec4(!x, !y, !z, !w);
	}
    };

    class vec4
    {
	public:

	union
	{
	    struct
	    {
		float x, y, z, w;
	    };
	    struct
	    {
		float r, g, b, a;
	    };
	    struct
	    {
		float s, t, p, q;
	    };
	    float v[4];
	};

	/* Constructors, Destructor */

	vec4() {}

	vec4(float x, float y, float z, float w)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	    this->w = w;
	}

	vec4(const vec2 xy, float z, float w)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = z;
	    this->w = w;
	}

	vec4(const vec2 xy, const vec2 zw)
	{
	    this->x = xy.x;
	    this->y = xy.y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	vec4(float x, const vec2 yz, float w)
	{
	    this->x = x;
	    this->y = yz.x;
	    this->z = yz.y;
	    this->w = w;
	}

	vec4(float x, float y, const vec2 zw)
	{
	    this->x = x;
	    this->y = y;
	    this->z = zw.x;
	    this->w = zw.y;
	}

	vec4(const vec3 xyz, float w)
	{
	    this->x = xyz.x;
	    this->y = xyz.y;
	    this->z = xyz.z;
	    this->w = w;
	}

	vec4(float x, const vec3 yzw)
	{
	    this->x = x;
	    this->y = yzw.x;
	    this->z = yzw.y;
	    this->w = yzw.z;
	}

	vec4(const float *xyzw)
	{
	    this->x = xyzw[0];
	    this->y = xyzw[1];
	    this->z = xyzw[2];
	    this->w = xyzw[3];
	}

	~vec4() {}

	/* "Swizzling" (as far as possible in C++) */

	vec2 xx() const
	{
	    return vec2(x, x);
	}

	vec2 xy() const
	{
	    return vec2(x, y);
	}

	vec2 xz() const
	{
	    return vec2(x, z);
	}

	vec2 xw() const
	{
	    return vec2(x, w);
	}

	vec2 yx() const
	{
	    return vec2(y, x);
	}

	vec2 yy() const
	{
	    return vec2(y, y);
	}

	vec2 yz() const
	{
	    return vec2(y, z);
	}

	vec2 yw() const
	{
	    return vec2(y, w);
	}

	vec2 zx() const
	{
	    return vec2(z, x);
	}

	vec2 zy() const
	{
	    return vec2(z, y);
	}

	vec2 zz() const
	{
	    return vec2(z, z);
	}

	vec2 zw() const
	{
	    return vec2(w, w);
	}

	vec2 wx() const
	{
	    return vec2(w, x);
	}

	vec2 wy() const
	{
	    return vec2(w, y);
	}

	vec2 wz() const
	{
	    return vec2(w, z);
	}

	vec2 ww() const
	{
	    return vec2(w, w);
	}

	vec3 xxx() const
	{
	    return vec3(x, x, x);
	}

	vec3 xxy() const
	{
	    return vec3(x, x, y);
	}

	vec3 xxz() const
	{
	    return vec3(x, x, z);
	}

	vec3 xxw() const
	{
	    return vec3(x, x, w);
	}

	vec3 xyx() const
	{
	    return vec3(x, y, x);
	}

	vec3 xyy() const
	{
	    return vec3(x, y, y);
	}

	vec3 xyz() const
	{
	    return vec3(x, y, z);
	}

	vec3 xyw() const
	{
	    return vec3(x, y, w);
	}

	vec3 xzx() const
	{
	    return vec3(x, z, x);
	}

	vec3 xzy() const
	{
	    return vec3(x, z, y);
	}

	vec3 xzz() const
	{
	    return vec3(x, z, z);
	}

	vec3 xzw() const
	{
	    return vec3(x, w, w);
	}

	vec3 xwx() const
	{
	    return vec3(x, w, x);
	}

	vec3 xwy() const
	{
	    return vec3(x, w, y);
	}

	vec3 xwz() const
	{
	    return vec3(x, w, z);
	}

	vec3 xww() const
	{
	    return vec3(x, w, w);
	}

	vec3 yxx() const
	{
	    return vec3(y, x, x);
	}

	vec3 yxy() const
	{
	    return vec3(y, x, y);
	}

	vec3 yxz() const
	{
	    return vec3(y, x, z);
	}

	vec3 yxw() const
	{
	    return vec3(y, x, w);
	}

	vec3 yyx() const
	{
	    return vec3(y, y, x);
	}

	vec3 yyy() const
	{
	    return vec3(y, y, y);
	}

	vec3 yyz() const
	{
	    return vec3(y, y, z);
	}

	vec3 yyw() const
	{
	    return vec3(y, y, w);
	}

	vec3 yzx() const
	{
	    return vec3(y, z, x);
	}

	vec3 yzy() const
	{
	    return vec3(y, z, y);
	}

	vec3 yzz() const
	{
	    return vec3(y, z, z);
	}

	vec3 yzw() const
	{
	    return vec3(y, w, w);
	}

	vec3 ywx() const
	{
	    return vec3(y, w, x);
	}

	vec3 ywy() const
	{
	    return vec3(y, w, y);
	}

	vec3 ywz() const
	{
	    return vec3(y, w, z);
	}

	vec3 yww() const
	{
	    return vec3(y, w, w);
	}

	vec3 zxx() const
	{
	    return vec3(z, x, x);
	}

	vec3 zxy() const
	{
	    return vec3(z, x, y);
	}

	vec3 zxz() const
	{
	    return vec3(z, x, z);
	}

	vec3 zxw() const
	{
	    return vec3(z, x, w);
	}

	vec3 zyx() const
	{
	    return vec3(z, y, x);
	}

	vec3 zyy() const
	{
	    return vec3(z, y, y);
	}

	vec3 zyz() const
	{
	    return vec3(z, y, z);
	}

	vec3 zyw() const
	{
	    return vec3(z, y, w);
	}

	vec3 zzx() const
	{
	    return vec3(z, z, x);
	}

	vec3 zzy() const
	{
	    return vec3(z, z, y);
	}

	vec3 zzz() const
	{
	    return vec3(z, z, z);
	}

	vec3 zzw() const
	{
	    return vec3(z, w, w);
	}

	vec3 zwx() const
	{
	    return vec3(z, w, x);
	}

	vec3 zwy() const
	{
	    return vec3(z, w, y);
	}

	vec3 zwz() const
	{
	    return vec3(z, w, z);
	}

	vec3 zww() const
	{
	    return vec3(z, w, w);
	}

	vec3 wxx() const
	{
	    return vec3(w, x, x);
	}

	vec3 wxy() const
	{
	    return vec3(w, x, y);
	}

	vec3 wxz() const
	{
	    return vec3(w, x, z);
	}

	vec3 wxw() const
	{
	    return vec3(w, x, w);
	}

	vec3 wyx() const
	{
	    return vec3(w, y, x);
	}

	vec3 wyy() const
	{
	    return vec3(w, y, y);
	}

	vec3 wyz() const
	{
	    return vec3(w, y, z);
	}

	vec3 wyw() const
	{
	    return vec3(w, y, w);
	}

	vec3 wzx() const
	{
	    return vec3(w, z, x);
	}

	vec3 wzy() const
	{
	    return vec3(w, z, y);
	}

	vec3 wzz() const
	{
	    return vec3(w, z, z);
	}

	vec3 wzw() const
	{
	    return vec3(w, w, w);
	}

	vec3 wwx() const
	{
	    return vec3(w, w, x);
	}

	vec3 wwy() const
	{
	    return vec3(w, w, y);
	}

	vec3 wwz() const
	{
	    return vec3(w, w, z);
	}

	vec3 www() const
	{
	    return vec3(w, w, w);
	}

	vec4 xxxx() const
	{
	    return vec4(x, x, x, x);
	}

	vec4 xxxy() const
	{
	    return vec4(x, x, x, y);
	}

	vec4 xxxz() const
	{
	    return vec4(x, x, x, z);
	}

	vec4 xxxw() const
	{
	    return vec4(x, x, x, w);
	}

	vec4 xxyx() const
	{
	    return vec4(x, x, y, x);
	}

	vec4 xxyy() const
	{
	    return vec4(x, x, y, y);
	}

	vec4 xxyz() const
	{
	    return vec4(x, x, y, z);
	}

	vec4 xxyw() const
	{
	    return vec4(x, x, y, w);
	}

	vec4 xxzx() const
	{
	    return vec4(x, x, z, x);
	}

	vec4 xxzy() const
	{
	    return vec4(x, x, z, y);
	}

	vec4 xxzz() const
	{
	    return vec4(x, x, z, z);
	}

	vec4 xxzw() const
	{
	    return vec4(x, x, z, w);
	}

	vec4 xxwx() const
	{
	    return vec4(x, x, w, x);
	}

	vec4 xxwy() const
	{
	    return vec4(x, x, w, y);
	}

	vec4 xxwz() const
	{
	    return vec4(x, x, w, z);
	}

	vec4 xxww() const
	{
	    return vec4(x, x, w, w);
	}

	vec4 xyxx() const
	{
	    return vec4(x, y, x, x);
	}

	vec4 xyxy() const
	{
	    return vec4(x, y, x, y);
	}

	vec4 xyxz() const
	{
	    return vec4(x, y, x, z);
	}

	vec4 xyxw() const
	{
	    return vec4(x, y, x, w);
	}

	vec4 xyyx() const
	{
	    return vec4(x, y, y, x);
	}

	vec4 xyyy() const
	{
	    return vec4(x, y, y, y);
	}

	vec4 xyyz() const
	{
	    return vec4(x, y, y, z);
	}

	vec4 xyyw() const
	{
	    return vec4(x, y, y, w);
	}

	vec4 xyzx() const
	{
	    return vec4(x, y, z, x);
	}

	vec4 xyzy() const
	{
	    return vec4(x, y, z, y);
	}

	vec4 xyzz() const
	{
	    return vec4(x, y, z, z);
	}

	vec4 xyzw() const
	{
	    return vec4(x, y, z, w);
	}

	vec4 xywx() const
	{
	    return vec4(x, y, w, x);
	}

	vec4 xywy() const
	{
	    return vec4(x, y, w, y);
	}

	vec4 xywz() const
	{
	    return vec4(x, y, w, z);
	}

	vec4 xyww() const
	{
	    return vec4(x, y, w, w);
	}

	vec4 xzxx() const
	{
	    return vec4(x, z, x, x);
	}

	vec4 xzxy() const
	{
	    return vec4(x, z, x, y);
	}

	vec4 xzxz() const
	{
	    return vec4(x, z, x, z);
	}

	vec4 xzxw() const
	{
	    return vec4(x, z, x, w);
	}

	vec4 xzyx() const
	{
	    return vec4(x, z, y, x);
	}

	vec4 xzyy() const
	{
	    return vec4(x, z, y, y);
	}

	vec4 xzyz() const
	{
	    return vec4(x, z, y, z);
	}

	vec4 xzyw() const
	{
	    return vec4(x, z, y, w);
	}

	vec4 xzzx() const
	{
	    return vec4(x, z, z, x);
	}

	vec4 xzzy() const
	{
	    return vec4(x, z, z, y);
	}

	vec4 xzzz() const
	{
	    return vec4(x, z, z, z);
	}

	vec4 xzzw() const
	{
	    return vec4(x, z, z, w);
	}

	vec4 xzwx() const
	{
	    return vec4(x, z, w, x);
	}

	vec4 xzwy() const
	{
	    return vec4(x, z, w, y);
	}

	vec4 xzwz() const
	{
	    return vec4(x, z, w, z);
	}

	vec4 xzww() const
	{
	    return vec4(x, z, w, w);
	}

	vec4 xwxx() const
	{
	    return vec4(x, w, x, x);
	}

	vec4 xwxy() const
	{
	    return vec4(x, w, x, y);
	}

	vec4 xwxz() const
	{
	    return vec4(x, w, x, z);
	}

	vec4 xwxw() const
	{
	    return vec4(x, w, x, w);
	}

	vec4 xwyx() const
	{
	    return vec4(x, w, y, x);
	}

	vec4 xwyy() const
	{
	    return vec4(x, w, y, y);
	}

	vec4 xwyz() const
	{
	    return vec4(x, w, y, z);
	}

	vec4 xwyw() const
	{
	    return vec4(x, w, y, w);
	}

	vec4 xwzx() const
	{
	    return vec4(x, w, z, x);
	}

	vec4 xwzy() const
	{
	    return vec4(x, w, z, y);
	}

	vec4 xwzz() const
	{
	    return vec4(x, w, z, z);
	}

	vec4 xwzw() const
	{
	    return vec4(x, w, z, w);
	}

	vec4 xwwx() const
	{
	    return vec4(x, w, w, x);
	}

	vec4 xwwy() const
	{
	    return vec4(x, w, w, y);
	}

	vec4 xwwz() const
	{
	    return vec4(x, w, w, z);
	}

	vec4 xwww() const
	{
	    return vec4(x, w, w, w);
	}

	vec4 yxxx() const
	{
	    return vec4(y, x, x, x);
	}

	vec4 yxxy() const
	{
	    return vec4(y, x, x, y);
	}

	vec4 yxxz() const
	{
	    return vec4(y, x, x, z);
	}

	vec4 yxxw() const
	{
	    return vec4(y, x, x, w);
	}

	vec4 yxyx() const
	{
	    return vec4(y, x, y, x);
	}

	vec4 yxyy() const
	{
	    return vec4(y, x, y, y);
	}

	vec4 yxyz() const
	{
	    return vec4(y, x, y, z);
	}

	vec4 yxyw() const
	{
	    return vec4(y, x, y, w);
	}

	vec4 yxzx() const
	{
	    return vec4(y, x, z, x);
	}

	vec4 yxzy() const
	{
	    return vec4(y, x, z, y);
	}

	vec4 yxzz() const
	{
	    return vec4(y, x, z, z);
	}

	vec4 yxzw() const
	{
	    return vec4(y, x, z, w);
	}

	vec4 yxwx() const
	{
	    return vec4(y, x, w, x);
	}

	vec4 yxwy() const
	{
	    return vec4(y, x, w, y);
	}

	vec4 yxwz() const
	{
	    return vec4(y, x, w, z);
	}

	vec4 yxww() const
	{
	    return vec4(y, x, w, w);
	}

	vec4 yyxx() const
	{
	    return vec4(y, y, x, x);
	}

	vec4 yyxy() const
	{
	    return vec4(y, y, x, y);
	}

	vec4 yyxz() const
	{
	    return vec4(y, y, x, z);
	}

	vec4 yyxw() const
	{
	    return vec4(y, y, x, w);
	}

	vec4 yyyx() const
	{
	    return vec4(y, y, y, x);
	}

	vec4 yyyy() const
	{
	    return vec4(y, y, y, y);
	}

	vec4 yyyz() const
	{
	    return vec4(y, y, y, z);
	}

	vec4 yyyw() const
	{
	    return vec4(y, y, y, w);
	}

	vec4 yyzx() const
	{
	    return vec4(y, y, z, x);
	}

	vec4 yyzy() const
	{
	    return vec4(y, y, z, y);
	}

	vec4 yyzz() const
	{
	    return vec4(y, y, z, z);
	}

	vec4 yyzw() const
	{
	    return vec4(y, y, z, w);
	}

	vec4 yywx() const
	{
	    return vec4(y, y, w, x);
	}

	vec4 yywy() const
	{
	    return vec4(y, y, w, y);
	}

	vec4 yywz() const
	{
	    return vec4(y, y, w, z);
	}

	vec4 yyww() const
	{
	    return vec4(y, y, w, w);
	}

	vec4 yzxx() const
	{
	    return vec4(y, z, x, x);
	}

	vec4 yzxy() const
	{
	    return vec4(y, z, x, y);
	}

	vec4 yzxz() const
	{
	    return vec4(y, z, x, z);
	}

	vec4 yzxw() const
	{
	    return vec4(y, z, x, w);
	}

	vec4 yzyx() const
	{
	    return vec4(y, z, y, x);
	}

	vec4 yzyy() const
	{
	    return vec4(y, z, y, y);
	}

	vec4 yzyz() const
	{
	    return vec4(y, z, y, z);
	}

	vec4 yzyw() const
	{
	    return vec4(y, z, y, w);
	}

	vec4 yzzx() const
	{
	    return vec4(y, z, z, x);
	}

	vec4 yzzy() const
	{
	    return vec4(y, z, z, y);
	}

	vec4 yzzz() const
	{
	    return vec4(y, z, z, z);
	}

	vec4 yzzw() const
	{
	    return vec4(y, z, z, w);
	}

	vec4 yzwx() const
	{
	    return vec4(y, z, w, x);
	}

	vec4 yzwy() const
	{
	    return vec4(y, z, w, y);
	}

	vec4 yzwz() const
	{
	    return vec4(y, z, w, z);
	}

	vec4 yzww() const
	{
	    return vec4(y, z, w, w);
	}

	vec4 ywxx() const
	{
	    return vec4(y, w, x, x);
	}

	vec4 ywxy() const
	{
	    return vec4(y, w, x, y);
	}

	vec4 ywxz() const
	{
	    return vec4(y, w, x, z);
	}

	vec4 ywxw() const
	{
	    return vec4(y, w, x, w);
	}

	vec4 ywyx() const
	{
	    return vec4(y, w, y, x);
	}

	vec4 ywyy() const
	{
	    return vec4(y, w, y, y);
	}

	vec4 ywyz() const
	{
	    return vec4(y, w, y, z);
	}

	vec4 ywyw() const
	{
	    return vec4(y, w, y, w);
	}

	vec4 ywzx() const
	{
	    return vec4(y, w, z, x);
	}

	vec4 ywzy() const
	{
	    return vec4(y, w, z, y);
	}

	vec4 ywzz() const
	{
	    return vec4(y, w, z, z);
	}

	vec4 ywzw() const
	{
	    return vec4(y, w, z, w);
	}

	vec4 ywwx() const
	{
	    return vec4(y, w, w, x);
	}

	vec4 ywwy() const
	{
	    return vec4(y, w, w, y);
	}

	vec4 ywwz() const
	{
	    return vec4(y, w, w, z);
	}

	vec4 ywww() const
	{
	    return vec4(y, w, w, w);
	}

	vec4 zxxx() const
	{
	    return vec4(z, x, x, x);
	}

	vec4 zxxy() const
	{
	    return vec4(z, x, x, y);
	}

	vec4 zxxz() const
	{
	    return vec4(z, x, x, z);
	}

	vec4 zxxw() const
	{
	    return vec4(z, x, x, w);
	}

	vec4 zxyx() const
	{
	    return vec4(z, x, y, x);
	}

	vec4 zxyy() const
	{
	    return vec4(z, x, y, y);
	}

	vec4 zxyz() const
	{
	    return vec4(z, x, y, z);
	}

	vec4 zxyw() const
	{
	    return vec4(z, x, y, w);
	}

	vec4 zxzx() const
	{
	    return vec4(z, x, z, x);
	}

	vec4 zxzy() const
	{
	    return vec4(z, x, z, y);
	}

	vec4 zxzz() const
	{
	    return vec4(z, x, z, z);
	}

	vec4 zxzw() const
	{
	    return vec4(z, x, z, w);
	}

	vec4 zxwx() const
	{
	    return vec4(z, x, w, x);
	}

	vec4 zxwy() const
	{
	    return vec4(z, x, w, y);
	}

	vec4 zxwz() const
	{
	    return vec4(z, x, w, z);
	}

	vec4 zxww() const
	{
	    return vec4(z, x, w, w);
	}

	vec4 zyxx() const
	{
	    return vec4(z, y, x, x);
	}

	vec4 zyxy() const
	{
	    return vec4(z, y, x, y);
	}

	vec4 zyxz() const
	{
	    return vec4(z, y, x, z);
	}

	vec4 zyxw() const
	{
	    return vec4(z, y, x, w);
	}

	vec4 zyyx() const
	{
	    return vec4(z, y, y, x);
	}

	vec4 zyyy() const
	{
	    return vec4(z, y, y, y);
	}

	vec4 zyyz() const
	{
	    return vec4(z, y, y, z);
	}

	vec4 zyyw() const
	{
	    return vec4(z, y, y, w);
	}

	vec4 zyzx() const
	{
	    return vec4(z, y, z, x);
	}

	vec4 zyzy() const
	{
	    return vec4(z, y, z, y);
	}

	vec4 zyzz() const
	{
	    return vec4(z, y, z, z);
	}

	vec4 zyzw() const
	{
	    return vec4(z, y, z, w);
	}

	vec4 zywx() const
	{
	    return vec4(z, y, w, x);
	}

	vec4 zywy() const
	{
	    return vec4(z, y, w, y);
	}

	vec4 zywz() const
	{
	    return vec4(z, y, w, z);
	}

	vec4 zyww() const
	{
	    return vec4(z, y, w, w);
	}

	vec4 zzxx() const
	{
	    return vec4(z, z, x, x);
	}

	vec4 zzxy() const
	{
	    return vec4(z, z, x, y);
	}

	vec4 zzxz() const
	{
	    return vec4(z, z, x, z);
	}

	vec4 zzxw() const
	{
	    return vec4(z, z, x, w);
	}

	vec4 zzyx() const
	{
	    return vec4(z, z, y, x);
	}

	vec4 zzyy() const
	{
	    return vec4(z, z, y, y);
	}

	vec4 zzyz() const
	{
	    return vec4(z, z, y, z);
	}

	vec4 zzyw() const
	{
	    return vec4(z, z, y, w);
	}

	vec4 zzzx() const
	{
	    return vec4(z, z, z, x);
	}

	vec4 zzzy() const
	{
	    return vec4(z, z, z, y);
	}

	vec4 zzzz() const
	{
	    return vec4(z, z, z, z);
	}

	vec4 zzzw() const
	{
	    return vec4(z, z, z, w);
	}

	vec4 zzwx() const
	{
	    return vec4(z, z, w, x);
	}

	vec4 zzwy() const
	{
	    return vec4(z, z, w, y);
	}

	vec4 zzwz() const
	{
	    return vec4(z, z, w, z);
	}

	vec4 zzww() const
	{
	    return vec4(z, z, w, w);
	}

	vec4 zwxx() const
	{
	    return vec4(z, w, x, x);
	}

	vec4 zwxy() const
	{
	    return vec4(z, w, x, y);
	}

	vec4 zwxz() const
	{
	    return vec4(z, w, x, z);
	}

	vec4 zwxw() const
	{
	    return vec4(z, w, x, w);
	}

	vec4 zwyx() const
	{
	    return vec4(z, w, y, x);
	}

	vec4 zwyy() const
	{
	    return vec4(z, w, y, y);
	}

	vec4 zwyz() const
	{
	    return vec4(z, w, y, z);
	}

	vec4 zwyw() const
	{
	    return vec4(z, w, y, w);
	}

	vec4 zwzx() const
	{
	    return vec4(z, w, z, x);
	}

	vec4 zwzy() const
	{
	    return vec4(z, w, z, y);
	}

	vec4 zwzz() const
	{
	    return vec4(z, w, z, z);
	}

	vec4 zwzw() const
	{
	    return vec4(z, w, z, w);
	}

	vec4 zwwx() const
	{
	    return vec4(z, w, w, x);
	}

	vec4 zwwy() const
	{
	    return vec4(z, w, w, y);
	}

	vec4 zwwz() const
	{
	    return vec4(z, w, w, z);
	}

	vec4 zwww() const
	{
	    return vec4(z, w, w, w);
	}

	vec4 wxxx() const
	{
	    return vec4(w, x, x, x);
	}

	vec4 wxxy() const
	{
	    return vec4(w, x, x, y);
	}

	vec4 wxxz() const
	{
	    return vec4(w, x, x, z);
	}

	vec4 wxxw() const
	{
	    return vec4(w, x, x, w);
	}

	vec4 wxyx() const
	{
	    return vec4(w, x, y, x);
	}

	vec4 wxyy() const
	{
	    return vec4(w, x, y, y);
	}

	vec4 wxyz() const
	{
	    return vec4(w, x, y, z);
	}

	vec4 wxyw() const
	{
	    return vec4(w, x, y, w);
	}

	vec4 wxzx() const
	{
	    return vec4(w, x, z, x);
	}

	vec4 wxzy() const
	{
	    return vec4(w, x, z, y);
	}

	vec4 wxzz() const
	{
	    return vec4(w, x, z, z);
	}

	vec4 wxzw() const
	{
	    return vec4(w, x, z, w);
	}

	vec4 wxwx() const
	{
	    return vec4(w, x, w, x);
	}

	vec4 wxwy() const
	{
	    return vec4(w, x, w, y);
	}

	vec4 wxwz() const
	{
	    return vec4(w, x, w, z);
	}

	vec4 wxww() const
	{
	    return vec4(w, x, w, w);
	}

	vec4 wyxx() const
	{
	    return vec4(w, y, x, x);
	}

	vec4 wyxy() const
	{
	    return vec4(w, y, x, y);
	}

	vec4 wyxz() const
	{
	    return vec4(w, y, x, z);
	}

	vec4 wyxw() const
	{
	    return vec4(w, y, x, w);
	}

	vec4 wyyx() const
	{
	    return vec4(w, y, y, x);
	}

	vec4 wyyy() const
	{
	    return vec4(w, y, y, y);
	}

	vec4 wyyz() const
	{
	    return vec4(w, y, y, z);
	}

	vec4 wyyw() const
	{
	    return vec4(w, y, y, w);
	}

	vec4 wyzx() const
	{
	    return vec4(w, y, z, x);
	}

	vec4 wyzy() const
	{
	    return vec4(w, y, z, y);
	}

	vec4 wyzz() const
	{
	    return vec4(w, y, z, z);
	}

	vec4 wyzw() const
	{
	    return vec4(w, y, z, w);
	}

	vec4 wywx() const
	{
	    return vec4(w, y, w, x);
	}

	vec4 wywy() const
	{
	    return vec4(w, y, w, y);
	}

	vec4 wywz() const
	{
	    return vec4(w, y, w, z);
	}

	vec4 wyww() const
	{
	    return vec4(w, y, w, w);
	}

	vec4 wzxx() const
	{
	    return vec4(w, z, x, x);
	}

	vec4 wzxy() const
	{
	    return vec4(w, z, x, y);
	}

	vec4 wzxz() const
	{
	    return vec4(w, z, x, z);
	}

	vec4 wzxw() const
	{
	    return vec4(w, z, x, w);
	}

	vec4 wzyx() const
	{
	    return vec4(w, z, y, x);
	}

	vec4 wzyy() const
	{
	    return vec4(w, z, y, y);
	}

	vec4 wzyz() const
	{
	    return vec4(w, z, y, z);
	}

	vec4 wzyw() const
	{
	    return vec4(w, z, y, w);
	}

	vec4 wzzx() const
	{
	    return vec4(w, z, z, x);
	}

	vec4 wzzy() const
	{
	    return vec4(w, z, z, y);
	}

	vec4 wzzz() const
	{
	    return vec4(w, z, z, z);
	}

	vec4 wzzw() const
	{
	    return vec4(w, z, z, w);
	}

	vec4 wzwx() const
	{
	    return vec4(w, z, w, x);
	}

	vec4 wzwy() const
	{
	    return vec4(w, z, w, y);
	}

	vec4 wzwz() const
	{
	    return vec4(w, z, w, z);
	}

	vec4 wzww() const
	{
	    return vec4(w, z, w, w);
	}

	vec4 wwxx() const
	{
	    return vec4(w, w, x, x);
	}

	vec4 wwxy() const
	{
	    return vec4(w, w, x, y);
	}

	vec4 wwxz() const
	{
	    return vec4(w, w, x, z);
	}

	vec4 wwxw() const
	{
	    return vec4(w, w, x, w);
	}

	vec4 wwyx() const
	{
	    return vec4(w, w, y, x);
	}

	vec4 wwyy() const
	{
	    return vec4(w, w, y, y);
	}

	vec4 wwyz() const
	{
	    return vec4(w, w, y, z);
	}

	vec4 wwyw() const
	{
	    return vec4(w, w, y, w);
	}

	vec4 wwzx() const
	{
	    return vec4(w, w, z, x);
	}

	vec4 wwzy() const
	{
	    return vec4(w, w, z, y);
	}

	vec4 wwzz() const
	{
	    return vec4(w, w, z, z);
	}

	vec4 wwzw() const
	{
	    return vec4(w, w, z, w);
	}

	vec4 wwwx() const
	{
	    return vec4(w, w, w, x);
	}

	vec4 wwwy() const
	{
	    return vec4(w, w, w, y);
	}

	vec4 wwwz() const
	{
	    return vec4(w, w, w, z);
	}

	vec4 wwww() const
	{
	    return vec4(w, w, w, w);
	}

	vec2 rr() const
	{
	    return vec2(r, r);
	}

	vec2 rg() const
	{
	    return vec2(r, g);
	}

	vec2 rb() const
	{
	    return vec2(r, b);
	}

	vec2 ra() const
	{
	    return vec2(r, a);
	}

	vec2 gr() const
	{
	    return vec2(g, r);
	}

	vec2 gg() const
	{
	    return vec2(g, g);
	}

	vec2 gb() const
	{
	    return vec2(g, b);
	}

	vec2 ga() const
	{
	    return vec2(g, a);
	}

	vec2 br() const
	{
	    return vec2(b, r);
	}

	vec2 bg() const
	{
	    return vec2(b, g);
	}

	vec2 bb() const
	{
	    return vec2(b, b);
	}

	vec2 ba() const
	{
	    return vec2(b, a);
	}

	vec2 ar() const
	{
	    return vec2(a, r);
	}

	vec2 ag() const
	{
	    return vec2(a, g);
	}

	vec2 ab() const
	{
	    return vec2(a, b);
	}

	vec2 aa() const
	{
	    return vec2(a, a);
	}

	vec3 rrr() const
	{
	    return vec3(r, r, r);
	}

	vec3 rrg() const
	{
	    return vec3(r, r, g);
	}

	vec3 rrb() const
	{
	    return vec3(r, r, b);
	}

	vec3 rra() const
	{
	    return vec3(r, r, a);
	}

	vec3 rgr() const
	{
	    return vec3(r, g, r);
	}

	vec3 rgg() const
	{
	    return vec3(r, g, g);
	}

	vec3 rgb() const
	{
	    return vec3(r, g, b);
	}

	vec3 rga() const
	{
	    return vec3(r, g, a);
	}

	vec3 rbr() const
	{
	    return vec3(r, b, r);
	}

	vec3 rbg() const
	{
	    return vec3(r, b, g);
	}

	vec3 rbb() const
	{
	    return vec3(r, b, b);
	}

	vec3 rba() const
	{
	    return vec3(r, a, a);
	}

	vec3 rar() const
	{
	    return vec3(r, a, r);
	}

	vec3 rag() const
	{
	    return vec3(r, a, g);
	}

	vec3 rab() const
	{
	    return vec3(r, a, b);
	}

	vec3 raa() const
	{
	    return vec3(r, a, a);
	}

	vec3 grr() const
	{
	    return vec3(g, r, r);
	}

	vec3 grg() const
	{
	    return vec3(g, r, g);
	}

	vec3 grb() const
	{
	    return vec3(g, r, b);
	}

	vec3 gra() const
	{
	    return vec3(g, r, a);
	}

	vec3 ggr() const
	{
	    return vec3(g, g, r);
	}

	vec3 ggg() const
	{
	    return vec3(g, g, g);
	}

	vec3 ggb() const
	{
	    return vec3(g, g, b);
	}

	vec3 gga() const
	{
	    return vec3(g, g, a);
	}

	vec3 gbr() const
	{
	    return vec3(g, b, r);
	}

	vec3 gbg() const
	{
	    return vec3(g, b, g);
	}

	vec3 gbb() const
	{
	    return vec3(g, b, b);
	}

	vec3 gba() const
	{
	    return vec3(g, a, a);
	}

	vec3 gar() const
	{
	    return vec3(g, a, r);
	}

	vec3 gag() const
	{
	    return vec3(g, a, g);
	}

	vec3 gab() const
	{
	    return vec3(g, a, b);
	}

	vec3 gaa() const
	{
	    return vec3(g, a, a);
	}

	vec3 brr() const
	{
	    return vec3(b, r, r);
	}

	vec3 brg() const
	{
	    return vec3(b, r, g);
	}

	vec3 brb() const
	{
	    return vec3(b, r, b);
	}

	vec3 bra() const
	{
	    return vec3(b, r, a);
	}

	vec3 bgr() const
	{
	    return vec3(b, g, r);
	}

	vec3 bgg() const
	{
	    return vec3(b, g, g);
	}

	vec3 bgb() const
	{
	    return vec3(b, g, b);
	}

	vec3 bga() const
	{
	    return vec3(b, g, a);
	}

	vec3 bbr() const
	{
	    return vec3(b, b, r);
	}

	vec3 bbg() const
	{
	    return vec3(b, b, g);
	}

	vec3 bbb() const
	{
	    return vec3(b, b, b);
	}

	vec3 bba() const
	{
	    return vec3(b, a, a);
	}

	vec3 bar() const
	{
	    return vec3(b, a, r);
	}

	vec3 bag() const
	{
	    return vec3(b, a, g);
	}

	vec3 bab() const
	{
	    return vec3(b, a, b);
	}

	vec3 baa() const
	{
	    return vec3(b, a, a);
	}

	vec3 arr() const
	{
	    return vec3(a, r, r);
	}

	vec3 arg() const
	{
	    return vec3(a, r, g);
	}

	vec3 arb() const
	{
	    return vec3(a, r, b);
	}

	vec3 ara() const
	{
	    return vec3(a, r, a);
	}

	vec3 agr() const
	{
	    return vec3(a, g, r);
	}

	vec3 agg() const
	{
	    return vec3(a, g, g);
	}

	vec3 agb() const
	{
	    return vec3(a, g, b);
	}

	vec3 aga() const
	{
	    return vec3(a, g, a);
	}

	vec3 abr() const
	{
	    return vec3(a, b, r);
	}

	vec3 abg() const
	{
	    return vec3(a, b, g);
	}

	vec3 abb() const
	{
	    return vec3(a, b, b);
	}

	vec3 aba() const
	{
	    return vec3(a, a, a);
	}

	vec3 aar() const
	{
	    return vec3(a, a, r);
	}

	vec3 aag() const
	{
	    return vec3(a, a, g);
	}

	vec3 aab() const
	{
	    return vec3(a, a, b);
	}

	vec3 aaa() const
	{
	    return vec3(a, a, a);
	}

	vec4 rrrr() const
	{
	    return vec4(r, r, r, r);
	}

	vec4 rrrg() const
	{
	    return vec4(r, r, r, g);
	}

	vec4 rrrb() const
	{
	    return vec4(r, r, r, b);
	}

	vec4 rrra() const
	{
	    return vec4(r, r, r, a);
	}

	vec4 rrgr() const
	{
	    return vec4(r, r, g, r);
	}

	vec4 rrgg() const
	{
	    return vec4(r, r, g, g);
	}

	vec4 rrgb() const
	{
	    return vec4(r, r, g, b);
	}

	vec4 rrga() const
	{
	    return vec4(r, r, g, a);
	}

	vec4 rrbr() const
	{
	    return vec4(r, r, b, r);
	}

	vec4 rrbg() const
	{
	    return vec4(r, r, b, g);
	}

	vec4 rrbb() const
	{
	    return vec4(r, r, b, b);
	}

	vec4 rrba() const
	{
	    return vec4(r, r, b, a);
	}

	vec4 rrar() const
	{
	    return vec4(r, r, a, r);
	}

	vec4 rrag() const
	{
	    return vec4(r, r, a, g);
	}

	vec4 rrab() const
	{
	    return vec4(r, r, a, b);
	}

	vec4 rraa() const
	{
	    return vec4(r, r, a, a);
	}

	vec4 rgrr() const
	{
	    return vec4(r, g, r, r);
	}

	vec4 rgrg() const
	{
	    return vec4(r, g, r, g);
	}

	vec4 rgrb() const
	{
	    return vec4(r, g, r, b);
	}

	vec4 rgra() const
	{
	    return vec4(r, g, r, a);
	}

	vec4 rggr() const
	{
	    return vec4(r, g, g, r);
	}

	vec4 rggg() const
	{
	    return vec4(r, g, g, g);
	}

	vec4 rggb() const
	{
	    return vec4(r, g, g, b);
	}

	vec4 rgga() const
	{
	    return vec4(r, g, g, a);
	}

	vec4 rgbr() const
	{
	    return vec4(r, g, b, r);
	}

	vec4 rgbg() const
	{
	    return vec4(r, g, b, g);
	}

	vec4 rgbb() const
	{
	    return vec4(r, g, b, b);
	}

	vec4 rgba() const
	{
	    return vec4(r, g, b, a);
	}

	vec4 rgar() const
	{
	    return vec4(r, g, a, r);
	}

	vec4 rgag() const
	{
	    return vec4(r, g, a, g);
	}

	vec4 rgab() const
	{
	    return vec4(r, g, a, b);
	}

	vec4 rgaa() const
	{
	    return vec4(r, g, a, a);
	}

	vec4 rbrr() const
	{
	    return vec4(r, b, r, r);
	}

	vec4 rbrg() const
	{
	    return vec4(r, b, r, g);
	}

	vec4 rbrb() const
	{
	    return vec4(r, b, r, b);
	}

	vec4 rbra() const
	{
	    return vec4(r, b, r, a);
	}

	vec4 rbgr() const
	{
	    return vec4(r, b, g, r);
	}

	vec4 rbgg() const
	{
	    return vec4(r, b, g, g);
	}

	vec4 rbgb() const
	{
	    return vec4(r, b, g, b);
	}

	vec4 rbga() const
	{
	    return vec4(r, b, g, a);
	}

	vec4 rbbr() const
	{
	    return vec4(r, b, b, r);
	}

	vec4 rbbg() const
	{
	    return vec4(r, b, b, g);
	}

	vec4 rbbb() const
	{
	    return vec4(r, b, b, b);
	}

	vec4 rbba() const
	{
	    return vec4(r, b, b, a);
	}

	vec4 rbar() const
	{
	    return vec4(r, b, a, r);
	}

	vec4 rbag() const
	{
	    return vec4(r, b, a, g);
	}

	vec4 rbab() const
	{
	    return vec4(r, b, a, b);
	}

	vec4 rbaa() const
	{
	    return vec4(r, b, a, a);
	}

	vec4 rarr() const
	{
	    return vec4(r, a, r, r);
	}

	vec4 rarg() const
	{
	    return vec4(r, a, r, g);
	}

	vec4 rarb() const
	{
	    return vec4(r, a, r, b);
	}

	vec4 rara() const
	{
	    return vec4(r, a, r, a);
	}

	vec4 ragr() const
	{
	    return vec4(r, a, g, r);
	}

	vec4 ragg() const
	{
	    return vec4(r, a, g, g);
	}

	vec4 ragb() const
	{
	    return vec4(r, a, g, b);
	}

	vec4 raga() const
	{
	    return vec4(r, a, g, a);
	}

	vec4 rabr() const
	{
	    return vec4(r, a, b, r);
	}

	vec4 rabg() const
	{
	    return vec4(r, a, b, g);
	}

	vec4 rabb() const
	{
	    return vec4(r, a, b, b);
	}

	vec4 raba() const
	{
	    return vec4(r, a, b, a);
	}

	vec4 raar() const
	{
	    return vec4(r, a, a, r);
	}

	vec4 raag() const
	{
	    return vec4(r, a, a, g);
	}

	vec4 raab() const
	{
	    return vec4(r, a, a, b);
	}

	vec4 raaa() const
	{
	    return vec4(r, a, a, a);
	}

	vec4 grrr() const
	{
	    return vec4(g, r, r, r);
	}

	vec4 grrg() const
	{
	    return vec4(g, r, r, g);
	}

	vec4 grrb() const
	{
	    return vec4(g, r, r, b);
	}

	vec4 grra() const
	{
	    return vec4(g, r, r, a);
	}

	vec4 grgr() const
	{
	    return vec4(g, r, g, r);
	}

	vec4 grgg() const
	{
	    return vec4(g, r, g, g);
	}

	vec4 grgb() const
	{
	    return vec4(g, r, g, b);
	}

	vec4 grga() const
	{
	    return vec4(g, r, g, a);
	}

	vec4 grbr() const
	{
	    return vec4(g, r, b, r);
	}

	vec4 grbg() const
	{
	    return vec4(g, r, b, g);
	}

	vec4 grbb() const
	{
	    return vec4(g, r, b, b);
	}

	vec4 grba() const
	{
	    return vec4(g, r, b, a);
	}

	vec4 grar() const
	{
	    return vec4(g, r, a, r);
	}

	vec4 grag() const
	{
	    return vec4(g, r, a, g);
	}

	vec4 grab() const
	{
	    return vec4(g, r, a, b);
	}

	vec4 graa() const
	{
	    return vec4(g, r, a, a);
	}

	vec4 ggrr() const
	{
	    return vec4(g, g, r, r);
	}

	vec4 ggrg() const
	{
	    return vec4(g, g, r, g);
	}

	vec4 ggrb() const
	{
	    return vec4(g, g, r, b);
	}

	vec4 ggra() const
	{
	    return vec4(g, g, r, a);
	}

	vec4 gggr() const
	{
	    return vec4(g, g, g, r);
	}

	vec4 gggg() const
	{
	    return vec4(g, g, g, g);
	}

	vec4 gggb() const
	{
	    return vec4(g, g, g, b);
	}

	vec4 ggga() const
	{
	    return vec4(g, g, g, a);
	}

	vec4 ggbr() const
	{
	    return vec4(g, g, b, r);
	}

	vec4 ggbg() const
	{
	    return vec4(g, g, b, g);
	}

	vec4 ggbb() const
	{
	    return vec4(g, g, b, b);
	}

	vec4 ggba() const
	{
	    return vec4(g, g, b, a);
	}

	vec4 ggar() const
	{
	    return vec4(g, g, a, r);
	}

	vec4 ggag() const
	{
	    return vec4(g, g, a, g);
	}

	vec4 ggab() const
	{
	    return vec4(g, g, a, b);
	}

	vec4 ggaa() const
	{
	    return vec4(g, g, a, a);
	}

	vec4 gbrr() const
	{
	    return vec4(g, b, r, r);
	}

	vec4 gbrg() const
	{
	    return vec4(g, b, r, g);
	}

	vec4 gbrb() const
	{
	    return vec4(g, b, r, b);
	}

	vec4 gbra() const
	{
	    return vec4(g, b, r, a);
	}

	vec4 gbgr() const
	{
	    return vec4(g, b, g, r);
	}

	vec4 gbgg() const
	{
	    return vec4(g, b, g, g);
	}

	vec4 gbgb() const
	{
	    return vec4(g, b, g, b);
	}

	vec4 gbga() const
	{
	    return vec4(g, b, g, a);
	}

	vec4 gbbr() const
	{
	    return vec4(g, b, b, r);
	}

	vec4 gbbg() const
	{
	    return vec4(g, b, b, g);
	}

	vec4 gbbb() const
	{
	    return vec4(g, b, b, b);
	}

	vec4 gbba() const
	{
	    return vec4(g, b, b, a);
	}

	vec4 gbar() const
	{
	    return vec4(g, b, a, r);
	}

	vec4 gbag() const
	{
	    return vec4(g, b, a, g);
	}

	vec4 gbab() const
	{
	    return vec4(g, b, a, b);
	}

	vec4 gbaa() const
	{
	    return vec4(g, b, a, a);
	}

	vec4 garr() const
	{
	    return vec4(g, a, r, r);
	}

	vec4 garg() const
	{
	    return vec4(g, a, r, g);
	}

	vec4 garb() const
	{
	    return vec4(g, a, r, b);
	}

	vec4 gara() const
	{
	    return vec4(g, a, r, a);
	}

	vec4 gagr() const
	{
	    return vec4(g, a, g, r);
	}

	vec4 gagg() const
	{
	    return vec4(g, a, g, g);
	}

	vec4 gagb() const
	{
	    return vec4(g, a, g, b);
	}

	vec4 gaga() const
	{
	    return vec4(g, a, g, a);
	}

	vec4 gabr() const
	{
	    return vec4(g, a, b, r);
	}

	vec4 gabg() const
	{
	    return vec4(g, a, b, g);
	}

	vec4 gabb() const
	{
	    return vec4(g, a, b, b);
	}

	vec4 gaba() const
	{
	    return vec4(g, a, b, a);
	}

	vec4 gaar() const
	{
	    return vec4(g, a, a, r);
	}

	vec4 gaag() const
	{
	    return vec4(g, a, a, g);
	}

	vec4 gaab() const
	{
	    return vec4(g, a, a, b);
	}

	vec4 gaaa() const
	{
	    return vec4(g, a, a, a);
	}

	vec4 brrr() const
	{
	    return vec4(b, r, r, r);
	}

	vec4 brrg() const
	{
	    return vec4(b, r, r, g);
	}

	vec4 brrb() const
	{
	    return vec4(b, r, r, b);
	}

	vec4 brra() const
	{
	    return vec4(b, r, r, a);
	}

	vec4 brgr() const
	{
	    return vec4(b, r, g, r);
	}

	vec4 brgg() const
	{
	    return vec4(b, r, g, g);
	}

	vec4 brgb() const
	{
	    return vec4(b, r, g, b);
	}

	vec4 brga() const
	{
	    return vec4(b, r, g, a);
	}

	vec4 brbr() const
	{
	    return vec4(b, r, b, r);
	}

	vec4 brbg() const
	{
	    return vec4(b, r, b, g);
	}

	vec4 brbb() const
	{
	    return vec4(b, r, b, b);
	}

	vec4 brba() const
	{
	    return vec4(b, r, b, a);
	}

	vec4 brar() const
	{
	    return vec4(b, r, a, r);
	}

	vec4 brag() const
	{
	    return vec4(b, r, a, g);
	}

	vec4 brab() const
	{
	    return vec4(b, r, a, b);
	}

	vec4 braa() const
	{
	    return vec4(b, r, a, a);
	}

	vec4 bgrr() const
	{
	    return vec4(b, g, r, r);
	}

	vec4 bgrg() const
	{
	    return vec4(b, g, r, g);
	}

	vec4 bgrb() const
	{
	    return vec4(b, g, r, b);
	}

	vec4 bgra() const
	{
	    return vec4(b, g, r, a);
	}

	vec4 bggr() const
	{
	    return vec4(b, g, g, r);
	}

	vec4 bggg() const
	{
	    return vec4(b, g, g, g);
	}

	vec4 bggb() const
	{
	    return vec4(b, g, g, b);
	}

	vec4 bgga() const
	{
	    return vec4(b, g, g, a);
	}

	vec4 bgbr() const
	{
	    return vec4(b, g, b, r);
	}

	vec4 bgbg() const
	{
	    return vec4(b, g, b, g);
	}

	vec4 bgbb() const
	{
	    return vec4(b, g, b, b);
	}

	vec4 bgba() const
	{
	    return vec4(b, g, b, a);
	}

	vec4 bgar() const
	{
	    return vec4(b, g, a, r);
	}

	vec4 bgag() const
	{
	    return vec4(b, g, a, g);
	}

	vec4 bgab() const
	{
	    return vec4(b, g, a, b);
	}

	vec4 bgaa() const
	{
	    return vec4(b, g, a, a);
	}

	vec4 bbrr() const
	{
	    return vec4(b, b, r, r);
	}

	vec4 bbrg() const
	{
	    return vec4(b, b, r, g);
	}

	vec4 bbrb() const
	{
	    return vec4(b, b, r, b);
	}

	vec4 bbra() const
	{
	    return vec4(b, b, r, a);
	}

	vec4 bbgr() const
	{
	    return vec4(b, b, g, r);
	}

	vec4 bbgg() const
	{
	    return vec4(b, b, g, g);
	}

	vec4 bbgb() const
	{
	    return vec4(b, b, g, b);
	}

	vec4 bbga() const
	{
	    return vec4(b, b, g, a);
	}

	vec4 bbbr() const
	{
	    return vec4(b, b, b, r);
	}

	vec4 bbbg() const
	{
	    return vec4(b, b, b, g);
	}

	vec4 bbbb() const
	{
	    return vec4(b, b, b, b);
	}

	vec4 bbba() const
	{
	    return vec4(b, b, b, a);
	}

	vec4 bbar() const
	{
	    return vec4(b, b, a, r);
	}

	vec4 bbag() const
	{
	    return vec4(b, b, a, g);
	}

	vec4 bbab() const
	{
	    return vec4(b, b, a, b);
	}

	vec4 bbaa() const
	{
	    return vec4(b, b, a, a);
	}

	vec4 barr() const
	{
	    return vec4(b, a, r, r);
	}

	vec4 barg() const
	{
	    return vec4(b, a, r, g);
	}

	vec4 barb() const
	{
	    return vec4(b, a, r, b);
	}

	vec4 bara() const
	{
	    return vec4(b, a, r, a);
	}

	vec4 bagr() const
	{
	    return vec4(b, a, g, r);
	}

	vec4 bagg() const
	{
	    return vec4(b, a, g, g);
	}

	vec4 bagb() const
	{
	    return vec4(b, a, g, b);
	}

	vec4 baga() const
	{
	    return vec4(b, a, g, a);
	}

	vec4 babr() const
	{
	    return vec4(b, a, b, r);
	}

	vec4 babg() const
	{
	    return vec4(b, a, b, g);
	}

	vec4 babb() const
	{
	    return vec4(b, a, b, b);
	}

	vec4 baba() const
	{
	    return vec4(b, a, b, a);
	}

	vec4 baar() const
	{
	    return vec4(b, a, a, r);
	}

	vec4 baag() const
	{
	    return vec4(b, a, a, g);
	}

	vec4 baab() const
	{
	    return vec4(b, a, a, b);
	}

	vec4 baaa() const
	{
	    return vec4(b, a, a, a);
	}

	vec4 arrr() const
	{
	    return vec4(a, r, r, r);
	}

	vec4 arrg() const
	{
	    return vec4(a, r, r, g);
	}

	vec4 arrb() const
	{
	    return vec4(a, r, r, b);
	}

	vec4 arra() const
	{
	    return vec4(a, r, r, a);
	}

	vec4 argr() const
	{
	    return vec4(a, r, g, r);
	}

	vec4 argg() const
	{
	    return vec4(a, r, g, g);
	}

	vec4 argb() const
	{
	    return vec4(a, r, g, b);
	}

	vec4 arga() const
	{
	    return vec4(a, r, g, a);
	}

	vec4 arbr() const
	{
	    return vec4(a, r, b, r);
	}

	vec4 arbg() const
	{
	    return vec4(a, r, b, g);
	}

	vec4 arbb() const
	{
	    return vec4(a, r, b, b);
	}

	vec4 arba() const
	{
	    return vec4(a, r, b, a);
	}

	vec4 arar() const
	{
	    return vec4(a, r, a, r);
	}

	vec4 arag() const
	{
	    return vec4(a, r, a, g);
	}

	vec4 arab() const
	{
	    return vec4(a, r, a, b);
	}

	vec4 araa() const
	{
	    return vec4(a, r, a, a);
	}

	vec4 agrr() const
	{
	    return vec4(a, g, r, r);
	}

	vec4 agrg() const
	{
	    return vec4(a, g, r, g);
	}

	vec4 agrb() const
	{
	    return vec4(a, g, r, b);
	}

	vec4 agra() const
	{
	    return vec4(a, g, r, a);
	}

	vec4 aggr() const
	{
	    return vec4(a, g, g, r);
	}

	vec4 aggg() const
	{
	    return vec4(a, g, g, g);
	}

	vec4 aggb() const
	{
	    return vec4(a, g, g, b);
	}

	vec4 agga() const
	{
	    return vec4(a, g, g, a);
	}

	vec4 agbr() const
	{
	    return vec4(a, g, b, r);
	}

	vec4 agbg() const
	{
	    return vec4(a, g, b, g);
	}

	vec4 agbb() const
	{
	    return vec4(a, g, b, b);
	}

	vec4 agba() const
	{
	    return vec4(a, g, b, a);
	}

	vec4 agar() const
	{
	    return vec4(a, g, a, r);
	}

	vec4 agag() const
	{
	    return vec4(a, g, a, g);
	}

	vec4 agab() const
	{
	    return vec4(a, g, a, b);
	}

	vec4 agaa() const
	{
	    return vec4(a, g, a, a);
	}

	vec4 abrr() const
	{
	    return vec4(a, b, r, r);
	}

	vec4 abrg() const
	{
	    return vec4(a, b, r, g);
	}

	vec4 abrb() const
	{
	    return vec4(a, b, r, b);
	}

	vec4 abra() const
	{
	    return vec4(a, b, r, a);
	}

	vec4 abgr() const
	{
	    return vec4(a, b, g, r);
	}

	vec4 abgg() const
	{
	    return vec4(a, b, g, g);
	}

	vec4 abgb() const
	{
	    return vec4(a, b, g, b);
	}

	vec4 abga() const
	{
	    return vec4(a, b, g, a);
	}

	vec4 abbr() const
	{
	    return vec4(a, b, b, r);
	}

	vec4 abbg() const
	{
	    return vec4(a, b, b, g);
	}

	vec4 abbb() const
	{
	    return vec4(a, b, b, b);
	}

	vec4 abba() const
	{
	    return vec4(a, b, b, a);
	}

	vec4 abar() const
	{
	    return vec4(a, b, a, r);
	}

	vec4 abag() const
	{
	    return vec4(a, b, a, g);
	}

	vec4 abab() const
	{
	    return vec4(a, b, a, b);
	}

	vec4 abaa() const
	{
	    return vec4(a, b, a, a);
	}

	vec4 aarr() const
	{
	    return vec4(a, a, r, r);
	}

	vec4 aarg() const
	{
	    return vec4(a, a, r, g);
	}

	vec4 aarb() const
	{
	    return vec4(a, a, r, b);
	}

	vec4 aara() const
	{
	    return vec4(a, a, r, a);
	}

	vec4 aagr() const
	{
	    return vec4(a, a, g, r);
	}

	vec4 aagg() const
	{
	    return vec4(a, a, g, g);
	}

	vec4 aagb() const
	{
	    return vec4(a, a, g, b);
	}

	vec4 aaga() const
	{
	    return vec4(a, a, g, a);
	}

	vec4 aabr() const
	{
	    return vec4(a, a, b, r);
	}

	vec4 aabg() const
	{
	    return vec4(a, a, b, g);
	}

	vec4 aabb() const
	{
	    return vec4(a, a, b, b);
	}

	vec4 aaba() const
	{
	    return vec4(a, a, b, a);
	}

	vec4 aaar() const
	{
	    return vec4(a, a, a, r);
	}

	vec4 aaag() const
	{
	    return vec4(a, a, a, g);
	}

	vec4 aaab() const
	{
	    return vec4(a, a, a, b);
	}

	vec4 aaaa() const
	{
	    return vec4(a, a, a, a);
	}

	vec2 ss() const
	{
	    return vec2(s, s);
	}

	vec2 st() const
	{
	    return vec2(s, t);
	}

	vec2 sp() const
	{
	    return vec2(s, p);
	}

	vec2 sq() const
	{
	    return vec2(s, q);
	}

	vec2 ts() const
	{
	    return vec2(t, s);
	}

	vec2 tt() const
	{
	    return vec2(t, t);
	}

	vec2 tp() const
	{
	    return vec2(t, p);
	}

	vec2 tq() const
	{
	    return vec2(t, q);
	}

	vec2 ps() const
	{
	    return vec2(p, s);
	}

	vec2 pt() const
	{
	    return vec2(p, t);
	}

	vec2 pp() const
	{
	    return vec2(p, p);
	}

	vec2 pq() const
	{
	    return vec2(p, q);
	}

	vec2 qs() const
	{
	    return vec2(q, s);
	}

	vec2 qt() const
	{
	    return vec2(q, t);
	}

	vec2 qp() const
	{
	    return vec2(q, p);
	}

	vec2 qq() const
	{
	    return vec2(q, q);
	}

	vec3 sss() const
	{
	    return vec3(s, s, s);
	}

	vec3 sst() const
	{
	    return vec3(s, s, t);
	}

	vec3 ssp() const
	{
	    return vec3(s, s, p);
	}

	vec3 ssq() const
	{
	    return vec3(s, s, q);
	}

	vec3 sts() const
	{
	    return vec3(s, t, s);
	}

	vec3 stt() const
	{
	    return vec3(s, t, t);
	}

	vec3 stp() const
	{
	    return vec3(s, t, p);
	}

	vec3 stq() const
	{
	    return vec3(s, t, q);
	}

	vec3 sps() const
	{
	    return vec3(s, p, s);
	}

	vec3 spt() const
	{
	    return vec3(s, p, t);
	}

	vec3 spp() const
	{
	    return vec3(s, p, p);
	}

	vec3 spq() const
	{
	    return vec3(s, q, q);
	}

	vec3 sqs() const
	{
	    return vec3(s, q, s);
	}

	vec3 sqt() const
	{
	    return vec3(s, q, t);
	}

	vec3 sqp() const
	{
	    return vec3(s, q, p);
	}

	vec3 sqq() const
	{
	    return vec3(s, q, q);
	}

	vec3 tss() const
	{
	    return vec3(t, s, s);
	}

	vec3 tst() const
	{
	    return vec3(t, s, t);
	}

	vec3 tsp() const
	{
	    return vec3(t, s, p);
	}

	vec3 tsq() const
	{
	    return vec3(t, s, q);
	}

	vec3 tts() const
	{
	    return vec3(t, t, s);
	}

	vec3 ttt() const
	{
	    return vec3(t, t, t);
	}

	vec3 ttp() const
	{
	    return vec3(t, t, p);
	}

	vec3 ttq() const
	{
	    return vec3(t, t, q);
	}

	vec3 tps() const
	{
	    return vec3(t, p, s);
	}

	vec3 tpt() const
	{
	    return vec3(t, p, t);
	}

	vec3 tpp() const
	{
	    return vec3(t, p, p);
	}

	vec3 tpq() const
	{
	    return vec3(t, q, q);
	}

	vec3 tqs() const
	{
	    return vec3(t, q, s);
	}

	vec3 tqt() const
	{
	    return vec3(t, q, t);
	}

	vec3 tqp() const
	{
	    return vec3(t, q, p);
	}

	vec3 tqq() const
	{
	    return vec3(t, q, q);
	}

	vec3 pss() const
	{
	    return vec3(p, s, s);
	}

	vec3 pst() const
	{
	    return vec3(p, s, t);
	}

	vec3 psp() const
	{
	    return vec3(p, s, p);
	}

	vec3 psq() const
	{
	    return vec3(p, s, q);
	}

	vec3 pts() const
	{
	    return vec3(p, t, s);
	}

	vec3 ptt() const
	{
	    return vec3(p, t, t);
	}

	vec3 ptp() const
	{
	    return vec3(p, t, p);
	}

	vec3 ptq() const
	{
	    return vec3(p, t, q);
	}

	vec3 pps() const
	{
	    return vec3(p, p, s);
	}

	vec3 ppt() const
	{
	    return vec3(p, p, t);
	}

	vec3 ppp() const
	{
	    return vec3(p, p, p);
	}

	vec3 ppq() const
	{
	    return vec3(p, q, q);
	}

	vec3 pqs() const
	{
	    return vec3(p, q, s);
	}

	vec3 pqt() const
	{
	    return vec3(p, q, t);
	}

	vec3 pqp() const
	{
	    return vec3(p, q, p);
	}

	vec3 pqq() const
	{
	    return vec3(p, q, q);
	}

	vec3 qss() const
	{
	    return vec3(q, s, s);
	}

	vec3 qst() const
	{
	    return vec3(q, s, t);
	}

	vec3 qsp() const
	{
	    return vec3(q, s, p);
	}

	vec3 qsq() const
	{
	    return vec3(q, s, q);
	}

	vec3 qts() const
	{
	    return vec3(q, t, s);
	}

	vec3 qtt() const
	{
	    return vec3(q, t, t);
	}

	vec3 qtp() const
	{
	    return vec3(q, t, p);
	}

	vec3 qtq() const
	{
	    return vec3(q, t, q);
	}

	vec3 qps() const
	{
	    return vec3(q, p, s);
	}

	vec3 qpt() const
	{
	    return vec3(q, p, t);
	}

	vec3 qpp() const
	{
	    return vec3(q, p, p);
	}

	vec3 qpq() const
	{
	    return vec3(q, q, q);
	}

	vec3 qqs() const
	{
	    return vec3(q, q, s);
	}

	vec3 qqt() const
	{
	    return vec3(q, q, t);
	}

	vec3 qqp() const
	{
	    return vec3(q, q, p);
	}

	vec3 qqq() const
	{
	    return vec3(q, q, q);
	}

	vec4 ssss() const
	{
	    return vec4(s, s, s, s);
	}

	vec4 ssst() const
	{
	    return vec4(s, s, s, t);
	}

	vec4 sssp() const
	{
	    return vec4(s, s, s, p);
	}

	vec4 sssq() const
	{
	    return vec4(s, s, s, q);
	}

	vec4 ssts() const
	{
	    return vec4(s, s, t, s);
	}

	vec4 sstt() const
	{
	    return vec4(s, s, t, t);
	}

	vec4 sstp() const
	{
	    return vec4(s, s, t, p);
	}

	vec4 sstq() const
	{
	    return vec4(s, s, t, q);
	}

	vec4 ssps() const
	{
	    return vec4(s, s, p, s);
	}

	vec4 sspt() const
	{
	    return vec4(s, s, p, t);
	}

	vec4 sspp() const
	{
	    return vec4(s, s, p, p);
	}

	vec4 sspq() const
	{
	    return vec4(s, s, p, q);
	}

	vec4 ssqs() const
	{
	    return vec4(s, s, q, s);
	}

	vec4 ssqt() const
	{
	    return vec4(s, s, q, t);
	}

	vec4 ssqp() const
	{
	    return vec4(s, s, q, p);
	}

	vec4 ssqq() const
	{
	    return vec4(s, s, q, q);
	}

	vec4 stss() const
	{
	    return vec4(s, t, s, s);
	}

	vec4 stst() const
	{
	    return vec4(s, t, s, t);
	}

	vec4 stsp() const
	{
	    return vec4(s, t, s, p);
	}

	vec4 stsq() const
	{
	    return vec4(s, t, s, q);
	}

	vec4 stts() const
	{
	    return vec4(s, t, t, s);
	}

	vec4 sttt() const
	{
	    return vec4(s, t, t, t);
	}

	vec4 sttp() const
	{
	    return vec4(s, t, t, p);
	}

	vec4 sttq() const
	{
	    return vec4(s, t, t, q);
	}

	vec4 stps() const
	{
	    return vec4(s, t, p, s);
	}

	vec4 stpt() const
	{
	    return vec4(s, t, p, t);
	}

	vec4 stpp() const
	{
	    return vec4(s, t, p, p);
	}

	vec4 stpq() const
	{
	    return vec4(s, t, p, q);
	}

	vec4 stqs() const
	{
	    return vec4(s, t, q, s);
	}

	vec4 stqt() const
	{
	    return vec4(s, t, q, t);
	}

	vec4 stqp() const
	{
	    return vec4(s, t, q, p);
	}

	vec4 stqq() const
	{
	    return vec4(s, t, q, q);
	}

	vec4 spss() const
	{
	    return vec4(s, p, s, s);
	}

	vec4 spst() const
	{
	    return vec4(s, p, s, t);
	}

	vec4 spsp() const
	{
	    return vec4(s, p, s, p);
	}

	vec4 spsq() const
	{
	    return vec4(s, p, s, q);
	}

	vec4 spts() const
	{
	    return vec4(s, p, t, s);
	}

	vec4 sptt() const
	{
	    return vec4(s, p, t, t);
	}

	vec4 sptp() const
	{
	    return vec4(s, p, t, p);
	}

	vec4 sptq() const
	{
	    return vec4(s, p, t, q);
	}

	vec4 spps() const
	{
	    return vec4(s, p, p, s);
	}

	vec4 sppt() const
	{
	    return vec4(s, p, p, t);
	}

	vec4 sppp() const
	{
	    return vec4(s, p, p, p);
	}

	vec4 sppq() const
	{
	    return vec4(s, p, p, q);
	}

	vec4 spqs() const
	{
	    return vec4(s, p, q, s);
	}

	vec4 spqt() const
	{
	    return vec4(s, p, q, t);
	}

	vec4 spqp() const
	{
	    return vec4(s, p, q, p);
	}

	vec4 spqq() const
	{
	    return vec4(s, p, q, q);
	}

	vec4 sqss() const
	{
	    return vec4(s, q, s, s);
	}

	vec4 sqst() const
	{
	    return vec4(s, q, s, t);
	}

	vec4 sqsp() const
	{
	    return vec4(s, q, s, p);
	}

	vec4 sqsq() const
	{
	    return vec4(s, q, s, q);
	}

	vec4 sqts() const
	{
	    return vec4(s, q, t, s);
	}

	vec4 sqtt() const
	{
	    return vec4(s, q, t, t);
	}

	vec4 sqtp() const
	{
	    return vec4(s, q, t, p);
	}

	vec4 sqtq() const
	{
	    return vec4(s, q, t, q);
	}

	vec4 sqps() const
	{
	    return vec4(s, q, p, s);
	}

	vec4 sqpt() const
	{
	    return vec4(s, q, p, t);
	}

	vec4 sqpp() const
	{
	    return vec4(s, q, p, p);
	}

	vec4 sqpq() const
	{
	    return vec4(s, q, p, q);
	}

	vec4 sqqs() const
	{
	    return vec4(s, q, q, s);
	}

	vec4 sqqt() const
	{
	    return vec4(s, q, q, t);
	}

	vec4 sqqp() const
	{
	    return vec4(s, q, q, p);
	}

	vec4 sqqq() const
	{
	    return vec4(s, q, q, q);
	}

	vec4 tsss() const
	{
	    return vec4(t, s, s, s);
	}

	vec4 tsst() const
	{
	    return vec4(t, s, s, t);
	}

	vec4 tssp() const
	{
	    return vec4(t, s, s, p);
	}

	vec4 tssq() const
	{
	    return vec4(t, s, s, q);
	}

	vec4 tsts() const
	{
	    return vec4(t, s, t, s);
	}

	vec4 tstt() const
	{
	    return vec4(t, s, t, t);
	}

	vec4 tstp() const
	{
	    return vec4(t, s, t, p);
	}

	vec4 tstq() const
	{
	    return vec4(t, s, t, q);
	}

	vec4 tsps() const
	{
	    return vec4(t, s, p, s);
	}

	vec4 tspt() const
	{
	    return vec4(t, s, p, t);
	}

	vec4 tspp() const
	{
	    return vec4(t, s, p, p);
	}

	vec4 tspq() const
	{
	    return vec4(t, s, p, q);
	}

	vec4 tsqs() const
	{
	    return vec4(t, s, q, s);
	}

	vec4 tsqt() const
	{
	    return vec4(t, s, q, t);
	}

	vec4 tsqp() const
	{
	    return vec4(t, s, q, p);
	}

	vec4 tsqq() const
	{
	    return vec4(t, s, q, q);
	}

	vec4 ttss() const
	{
	    return vec4(t, t, s, s);
	}

	vec4 ttst() const
	{
	    return vec4(t, t, s, t);
	}

	vec4 ttsp() const
	{
	    return vec4(t, t, s, p);
	}

	vec4 ttsq() const
	{
	    return vec4(t, t, s, q);
	}

	vec4 ttts() const
	{
	    return vec4(t, t, t, s);
	}

	vec4 tttt() const
	{
	    return vec4(t, t, t, t);
	}

	vec4 tttp() const
	{
	    return vec4(t, t, t, p);
	}

	vec4 tttq() const
	{
	    return vec4(t, t, t, q);
	}

	vec4 ttps() const
	{
	    return vec4(t, t, p, s);
	}

	vec4 ttpt() const
	{
	    return vec4(t, t, p, t);
	}

	vec4 ttpp() const
	{
	    return vec4(t, t, p, p);
	}

	vec4 ttpq() const
	{
	    return vec4(t, t, p, q);
	}

	vec4 ttqs() const
	{
	    return vec4(t, t, q, s);
	}

	vec4 ttqt() const
	{
	    return vec4(t, t, q, t);
	}

	vec4 ttqp() const
	{
	    return vec4(t, t, q, p);
	}

	vec4 ttqq() const
	{
	    return vec4(t, t, q, q);
	}

	vec4 tpss() const
	{
	    return vec4(t, p, s, s);
	}

	vec4 tpst() const
	{
	    return vec4(t, p, s, t);
	}

	vec4 tpsp() const
	{
	    return vec4(t, p, s, p);
	}

	vec4 tpsq() const
	{
	    return vec4(t, p, s, q);
	}

	vec4 tpts() const
	{
	    return vec4(t, p, t, s);
	}

	vec4 tptt() const
	{
	    return vec4(t, p, t, t);
	}

	vec4 tptp() const
	{
	    return vec4(t, p, t, p);
	}

	vec4 tptq() const
	{
	    return vec4(t, p, t, q);
	}

	vec4 tpps() const
	{
	    return vec4(t, p, p, s);
	}

	vec4 tppt() const
	{
	    return vec4(t, p, p, t);
	}

	vec4 tppp() const
	{
	    return vec4(t, p, p, p);
	}

	vec4 tppq() const
	{
	    return vec4(t, p, p, q);
	}

	vec4 tpqs() const
	{
	    return vec4(t, p, q, s);
	}

	vec4 tpqt() const
	{
	    return vec4(t, p, q, t);
	}

	vec4 tpqp() const
	{
	    return vec4(t, p, q, p);
	}

	vec4 tpqq() const
	{
	    return vec4(t, p, q, q);
	}

	vec4 tqss() const
	{
	    return vec4(t, q, s, s);
	}

	vec4 tqst() const
	{
	    return vec4(t, q, s, t);
	}

	vec4 tqsp() const
	{
	    return vec4(t, q, s, p);
	}

	vec4 tqsq() const
	{
	    return vec4(t, q, s, q);
	}

	vec4 tqts() const
	{
	    return vec4(t, q, t, s);
	}

	vec4 tqtt() const
	{
	    return vec4(t, q, t, t);
	}

	vec4 tqtp() const
	{
	    return vec4(t, q, t, p);
	}

	vec4 tqtq() const
	{
	    return vec4(t, q, t, q);
	}

	vec4 tqps() const
	{
	    return vec4(t, q, p, s);
	}

	vec4 tqpt() const
	{
	    return vec4(t, q, p, t);
	}

	vec4 tqpp() const
	{
	    return vec4(t, q, p, p);
	}

	vec4 tqpq() const
	{
	    return vec4(t, q, p, q);
	}

	vec4 tqqs() const
	{
	    return vec4(t, q, q, s);
	}

	vec4 tqqt() const
	{
	    return vec4(t, q, q, t);
	}

	vec4 tqqp() const
	{
	    return vec4(t, q, q, p);
	}

	vec4 tqqq() const
	{
	    return vec4(t, q, q, q);
	}

	vec4 psss() const
	{
	    return vec4(p, s, s, s);
	}

	vec4 psst() const
	{
	    return vec4(p, s, s, t);
	}

	vec4 pssp() const
	{
	    return vec4(p, s, s, p);
	}

	vec4 pssq() const
	{
	    return vec4(p, s, s, q);
	}

	vec4 psts() const
	{
	    return vec4(p, s, t, s);
	}

	vec4 pstt() const
	{
	    return vec4(p, s, t, t);
	}

	vec4 pstp() const
	{
	    return vec4(p, s, t, p);
	}

	vec4 pstq() const
	{
	    return vec4(p, s, t, q);
	}

	vec4 psps() const
	{
	    return vec4(p, s, p, s);
	}

	vec4 pspt() const
	{
	    return vec4(p, s, p, t);
	}

	vec4 pspp() const
	{
	    return vec4(p, s, p, p);
	}

	vec4 pspq() const
	{
	    return vec4(p, s, p, q);
	}

	vec4 psqs() const
	{
	    return vec4(p, s, q, s);
	}

	vec4 psqt() const
	{
	    return vec4(p, s, q, t);
	}

	vec4 psqp() const
	{
	    return vec4(p, s, q, p);
	}

	vec4 psqq() const
	{
	    return vec4(p, s, q, q);
	}

	vec4 ptss() const
	{
	    return vec4(p, t, s, s);
	}

	vec4 ptst() const
	{
	    return vec4(p, t, s, t);
	}

	vec4 ptsp() const
	{
	    return vec4(p, t, s, p);
	}

	vec4 ptsq() const
	{
	    return vec4(p, t, s, q);
	}

	vec4 ptts() const
	{
	    return vec4(p, t, t, s);
	}

	vec4 pttt() const
	{
	    return vec4(p, t, t, t);
	}

	vec4 pttp() const
	{
	    return vec4(p, t, t, p);
	}

	vec4 pttq() const
	{
	    return vec4(p, t, t, q);
	}

	vec4 ptps() const
	{
	    return vec4(p, t, p, s);
	}

	vec4 ptpt() const
	{
	    return vec4(p, t, p, t);
	}

	vec4 ptpp() const
	{
	    return vec4(p, t, p, p);
	}

	vec4 ptpq() const
	{
	    return vec4(p, t, p, q);
	}

	vec4 ptqs() const
	{
	    return vec4(p, t, q, s);
	}

	vec4 ptqt() const
	{
	    return vec4(p, t, q, t);
	}

	vec4 ptqp() const
	{
	    return vec4(p, t, q, p);
	}

	vec4 ptqq() const
	{
	    return vec4(p, t, q, q);
	}

	vec4 ppss() const
	{
	    return vec4(p, p, s, s);
	}

	vec4 ppst() const
	{
	    return vec4(p, p, s, t);
	}

	vec4 ppsp() const
	{
	    return vec4(p, p, s, p);
	}

	vec4 ppsq() const
	{
	    return vec4(p, p, s, q);
	}

	vec4 ppts() const
	{
	    return vec4(p, p, t, s);
	}

	vec4 pptt() const
	{
	    return vec4(p, p, t, t);
	}

	vec4 pptp() const
	{
	    return vec4(p, p, t, p);
	}

	vec4 pptq() const
	{
	    return vec4(p, p, t, q);
	}

	vec4 ppps() const
	{
	    return vec4(p, p, p, s);
	}

	vec4 pppt() const
	{
	    return vec4(p, p, p, t);
	}

	vec4 pppp() const
	{
	    return vec4(p, p, p, p);
	}

	vec4 pppq() const
	{
	    return vec4(p, p, p, q);
	}

	vec4 ppqs() const
	{
	    return vec4(p, p, q, s);
	}

	vec4 ppqt() const
	{
	    return vec4(p, p, q, t);
	}

	vec4 ppqp() const
	{
	    return vec4(p, p, q, p);
	}

	vec4 ppqq() const
	{
	    return vec4(p, p, q, q);
	}

	vec4 pqss() const
	{
	    return vec4(p, q, s, s);
	}

	vec4 pqst() const
	{
	    return vec4(p, q, s, t);
	}

	vec4 pqsp() const
	{
	    return vec4(p, q, s, p);
	}

	vec4 pqsq() const
	{
	    return vec4(p, q, s, q);
	}

	vec4 pqts() const
	{
	    return vec4(p, q, t, s);
	}

	vec4 pqtt() const
	{
	    return vec4(p, q, t, t);
	}

	vec4 pqtp() const
	{
	    return vec4(p, q, t, p);
	}

	vec4 pqtq() const
	{
	    return vec4(p, q, t, q);
	}

	vec4 pqps() const
	{
	    return vec4(p, q, p, s);
	}

	vec4 pqpt() const
	{
	    return vec4(p, q, p, t);
	}

	vec4 pqpp() const
	{
	    return vec4(p, q, p, p);
	}

	vec4 pqpq() const
	{
	    return vec4(p, q, p, q);
	}

	vec4 pqqs() const
	{
	    return vec4(p, q, q, s);
	}

	vec4 pqqt() const
	{
	    return vec4(p, q, q, t);
	}

	vec4 pqqp() const
	{
	    return vec4(p, q, q, p);
	}

	vec4 pqqq() const
	{
	    return vec4(p, q, q, q);
	}

	vec4 qsss() const
	{
	    return vec4(q, s, s, s);
	}

	vec4 qsst() const
	{
	    return vec4(q, s, s, t);
	}

	vec4 qssp() const
	{
	    return vec4(q, s, s, p);
	}

	vec4 qssq() const
	{
	    return vec4(q, s, s, q);
	}

	vec4 qsts() const
	{
	    return vec4(q, s, t, s);
	}

	vec4 qstt() const
	{
	    return vec4(q, s, t, t);
	}

	vec4 qstp() const
	{
	    return vec4(q, s, t, p);
	}

	vec4 qstq() const
	{
	    return vec4(q, s, t, q);
	}

	vec4 qsps() const
	{
	    return vec4(q, s, p, s);
	}

	vec4 qspt() const
	{
	    return vec4(q, s, p, t);
	}

	vec4 qspp() const
	{
	    return vec4(q, s, p, p);
	}

	vec4 qspq() const
	{
	    return vec4(q, s, p, q);
	}

	vec4 qsqs() const
	{
	    return vec4(q, s, q, s);
	}

	vec4 qsqt() const
	{
	    return vec4(q, s, q, t);
	}

	vec4 qsqp() const
	{
	    return vec4(q, s, q, p);
	}

	vec4 qsqq() const
	{
	    return vec4(q, s, q, q);
	}

	vec4 qtss() const
	{
	    return vec4(q, t, s, s);
	}

	vec4 qtst() const
	{
	    return vec4(q, t, s, t);
	}

	vec4 qtsp() const
	{
	    return vec4(q, t, s, p);
	}

	vec4 qtsq() const
	{
	    return vec4(q, t, s, q);
	}

	vec4 qtts() const
	{
	    return vec4(q, t, t, s);
	}

	vec4 qttt() const
	{
	    return vec4(q, t, t, t);
	}

	vec4 qttp() const
	{
	    return vec4(q, t, t, p);
	}

	vec4 qttq() const
	{
	    return vec4(q, t, t, q);
	}

	vec4 qtps() const
	{
	    return vec4(q, t, p, s);
	}

	vec4 qtpt() const
	{
	    return vec4(q, t, p, t);
	}

	vec4 qtpp() const
	{
	    return vec4(q, t, p, p);
	}

	vec4 qtpq() const
	{
	    return vec4(q, t, p, q);
	}

	vec4 qtqs() const
	{
	    return vec4(q, t, q, s);
	}

	vec4 qtqt() const
	{
	    return vec4(q, t, q, t);
	}

	vec4 qtqp() const
	{
	    return vec4(q, t, q, p);
	}

	vec4 qtqq() const
	{
	    return vec4(q, t, q, q);
	}

	vec4 qpss() const
	{
	    return vec4(q, p, s, s);
	}

	vec4 qpst() const
	{
	    return vec4(q, p, s, t);
	}

	vec4 qpsp() const
	{
	    return vec4(q, p, s, p);
	}

	vec4 qpsq() const
	{
	    return vec4(q, p, s, q);
	}

	vec4 qpts() const
	{
	    return vec4(q, p, t, s);
	}

	vec4 qptt() const
	{
	    return vec4(q, p, t, t);
	}

	vec4 qptp() const
	{
	    return vec4(q, p, t, p);
	}

	vec4 qptq() const
	{
	    return vec4(q, p, t, q);
	}

	vec4 qpps() const
	{
	    return vec4(q, p, p, s);
	}

	vec4 qppt() const
	{
	    return vec4(q, p, p, t);
	}

	vec4 qppp() const
	{
	    return vec4(q, p, p, p);
	}

	vec4 qppq() const
	{
	    return vec4(q, p, p, q);
	}

	vec4 qpqs() const
	{
	    return vec4(q, p, q, s);
	}

	vec4 qpqt() const
	{
	    return vec4(q, p, q, t);
	}

	vec4 qpqp() const
	{
	    return vec4(q, p, q, p);
	}

	vec4 qpqq() const
	{
	    return vec4(q, p, q, q);
	}

	vec4 qqss() const
	{
	    return vec4(q, q, s, s);
	}

	vec4 qqst() const
	{
	    return vec4(q, q, s, t);
	}

	vec4 qqsp() const
	{
	    return vec4(q, q, s, p);
	}

	vec4 qqsq() const
	{
	    return vec4(q, q, s, q);
	}

	vec4 qqts() const
	{
	    return vec4(q, q, t, s);
	}

	vec4 qqtt() const
	{
	    return vec4(q, q, t, t);
	}

	vec4 qqtp() const
	{
	    return vec4(q, q, t, p);
	}

	vec4 qqtq() const
	{
	    return vec4(q, q, t, q);
	}

	vec4 qqps() const
	{
	    return vec4(q, q, p, s);
	}

	vec4 qqpt() const
	{
	    return vec4(q, q, p, t);
	}

	vec4 qqpp() const
	{
	    return vec4(q, q, p, p);
	}

	vec4 qqpq() const
	{
	    return vec4(q, q, p, q);
	}

	vec4 qqqs() const
	{
	    return vec4(q, q, q, s);
	}

	vec4 qqqt() const
	{
	    return vec4(q, q, q, t);
	}

	vec4 qqqp() const
	{
	    return vec4(q, q, q, p);
	}

	vec4 qqqq() const
	{
	    return vec4(q, q, q, q);
	}

	/* Operators */

	const vec4& operator=(const vec4& v)
	{
	    x = v.x;
	    y = v.y;
	    z = v.z;
	    w = v.w;
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return v[i];
	}

	const float &operator[](size_t i) const
	{
	    return v[i];
	}
	
	vec4 operator*(const float s) const
	{
	    return vec4(x * s, y * s, z * s, w * s);
	}

	vec4 operator/(const float s) const
	{
	    return vec4(x / s, y / s, z / s, w / s);
	}
     
	const vec4 &operator*=(float s)
	{
	    x *= s;
	    y *= s;
	    z *= s;
	    w *= s;
	    return *this;
	}
     
	const vec4 &operator/=(float s)
	{
	    x /= s;
	    y /= s;
	    z /= s;
	    w /= s;
	    return *this;
	}

	vec4 operator+(const vec4& v) const
	{
	    return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	vec4 operator-(const vec4& v) const
	{
	    return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	vec4 operator*(const vec4& v) const 
	{
	    return vec4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	vec4 operator/(const vec4& v) const 
	{
	    return vec4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	vec4 operator-() const
	{
	    return vec4(-x, -y, -z, -w);
	}

	const vec4& operator+=(const vec4& v)
	{
	    x += v.x;
	    y += v.y;
	    z += v.z;
	    w += v.w;
	    return *this;
	}

	const vec4& operator-=(const vec4& v)
	{
	    x -= v.x;
	    y -= v.y;
	    z -= v.z;
	    w -= v.w;
	    return *this;
	}

	const vec4& operator*=(const vec4& v)
	{
	    x *= v.x;
	    y *= v.y;
	    z *= v.z;
	    w *= v.w;
	    return *this;
	}

	const vec4& operator/=(const vec4& v)
	{
	    x /= v.x;
	    y /= v.y;
	    z /= v.z;
	    w /= v.w;
	    return *this;
	}

	/* Trigonometric functions */

	vec4 sin() const
	{
	    return vec4(sinf(x), sinf(y), sinf(z), sinf(w));
	}

	vec4 cos() const
	{
	    return vec4(cosf(x), cosf(y), cosf(z), cosf(w));
	}

	vec4 tan() const
	{
	    return vec4(tanf(x), tanf(y), tanf(z), tanf(w));
	}

	vec4 asin() const
	{
	    return vec4(asinf(x), asinf(y), asinf(z), asinf(w));
	}

	vec4 acos() const
	{
	    return vec4(acosf(x), acosf(y), acosf(z), acosf(w));
	}

	vec4 atan() const
	{
	    return vec4(atanf(x), atanf(y), atanf(z), atanf(w));
	}

	vec4 atan(const vec4 v) const
	{
	    return vec4(atan2f(x, v.x), atan2f(y, v.y), atan2f(z, v.z), atan2f(w, v.w));
	}

	vec4 radians() const
	{
	    return vec4(x * static_cast<float>(M_PI / 180.0f), y * static_cast<float>(M_PI / 180.0f), z * static_cast<float>(M_PI / 180.0f), w * static_cast<float>(M_PI / 180.0f));
	}

	vec4 degrees() const
	{
	    return vec4(x * 180.0f / static_cast<float>(M_PI), y * 180.0f / static_cast<float>(M_PI), z * 180.0f / static_cast<float>(M_PI), w * 180.0f / static_cast<float>(M_PI));
	}

	/* Exponential functions */

	vec4 pow(const float p) const
	{
	    return vec4(powf(x, p), powf(y, p), powf(z, p), powf(w, p));
	}

	vec4 exp() const
	{
	    return vec4(expf(x), expf(y), expf(z), expf(w));
	}

	vec4 log() const
	{
	    return vec4(logf(x), logf(y), logf(z), logf(w));
	}

	vec4 sqrt() const
	{
	    return vec4(sqrtf(x), sqrtf(y), sqrtf(z), sqrtf(w));
	}
	
	/* Common functions */

	vec4 abs() const
	{
	    return vec4(fabsf(x), fabsf(y), fabsf(z), fabsf(w));
	}

	vec4 sign() const
	{
	    return vec4(
		    x < 0.0f ? -1.0f : x > 0.0f ? +1.0f : 0.0f,
		    y < 0.0f ? -1.0f : y > 0.0f ? +1.0f : 0.0f,
		    z < 0.0f ? -1.0f : z > 0.0f ? +1.0f : 0.0f,
		    w < 0.0f ? -1.0f : w > 0.0f ? +1.0f : 0.0f);
	}

	vec4 floor() const
	{
	    return vec4(floorf(x), floorf(y), floorf(z), floorf(w));
	}

	vec4 ceil() const
	{
	    return vec4(ceilf(x), ceilf(y), ceilf(z), ceilf(w));
	}

	vec4 fract() const
	{
	    return *this - this->floor();
	}

	vec4 min(const float v) const
	{
	    return vec4(v < x ? v : x, v < y ? v : y, v < z ? v : z, v < w ? v : w);
	}

	vec4 min(const vec4 v) const
	{
	    return vec4(v.x < x ? v.x : x, v.y < y ? v.y : y, v.z < z ? v.z : z, v.w < w ? v.w : w);
	}

	vec4 max(const float v) const
	{
	    return vec4(v > x ? v : x, v > y ? v : y, v > z ? v : z, v > w ? v : w);
	}

	vec4 max(const vec4 v) const
	{
	    return vec4(v.x > x ? v.x : x, v.y > y ? v.y : y, v.z > z ? v.z : z, v.w > w ? v.w : w);
	}

	vec4 clamp(const float minval, const float maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec4 clamp(const vec4 minval, const vec4 maxval) const
	{
	    return this->max(minval).min(maxval);
	}

	vec4 mix(const vec4 v, const float alpha) const
	{
	    return *this * (1.0f - alpha) + v * alpha;
	}

	vec4 mix(const vec4 v, const vec4 alpha) const
	{
	    return (vec4(1.0f, 1.0f, 1.0f, 1.0f) - alpha) * *this + alpha * v;
	}

	vec4 step(const float edge) const
	{
	    return vec4(x < edge ? 0.0f : 1.0f, y < edge ? 0.0f : 1.0f, z < edge ? 0.0f : 1.0f, w < edge ? 0.0f : 1.0f);
	}

	vec4 step(const vec4 edge) const
	{
	    return vec4(x < edge.x ? 0.0f : 1.0f, y < edge.y ? 0.0f : 1.0f, z < edge.z ? 0.0f : 1.0f, w < edge.w ? 0.0f : 1.0f);
	}

	vec4 smoothstep(const float edge0, const float edge1) const
	{
	    vec4 t = (*this - vec4(edge0, edge0, edge0, edge0)) / (vec4(edge1, edge1, edge1, edge1) - vec4(edge0, edge0, edge0, edge0));
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec4(3.0f, 3.0f, 3.0f, 3.0f) - t * 2.0f);
	}

	vec4 smoothstep(const vec4 edge0, const vec4 edge1) const
	{
	    vec4 t = (*this - edge0) / (edge1 - edge0);
	    t.clamp(0.0f, 1.0f);
	    return t * t * (vec4(3.0f, 3.0f, 3.0f, 3.0f) - t * 2.0f);
	}
	
	vec4 mod(const float y) const
	{
	    vec4 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}
	
	vec4 mod(const vec4 y) const
	{
	    vec4 v = *this;
	    v /= y;
	    return *this - v.floor() * y;
	}

	/* Geometric functions */

	float length() const
	{
	    return sqrtf(x * x + y * y + z * z + w * w);
	}

	float distance(const vec4 v) const
	{
	    return vec4(*this - v).length();
	}

	float dot(const vec4 v) const
	{
	    return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	vec4 normalize() const
	{
	    return *this / this->length();
	}

	vec4 faceforward(const vec4 I, const vec4 Nref) const
	{
	    return Nref.dot(I) < 0.0f ? *this : - *this;
	}

	vec4 reflect(const vec4 N) const
	{
	    return *this - N * (N.dot(*this) * 2.0f);
	}

	vec4 refract(const vec4 N, float eta) const
	{
	    float d = N.dot(*this);
	    float k = 1.0f - eta * eta * (1.0f - d * d);
	    return k < 0.0f ? vec4(0.0f, 0.0f, 0.0f, 0.0f) : *this * eta - N * (eta * d + sqrtf(k));
	}
	
	/* Comparion functions */

	bvec4 equal(const vec4 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return bvec4(
		    fabsf(x - v.x) < epsilon,
		    fabsf(y - v.y) < epsilon,
		    fabsf(z - v.z) < epsilon,
		    fabsf(w - v.w) < epsilon);
	}

	bvec4 notEqual(const vec4 v, const float epsilon = std::numeric_limits<float>::epsilon()) const
	{
	    return this->equal(v, epsilon).negate();
	}

	bvec4 greaterThan(const vec4 v) const
	{
	    return bvec4(x > v.x, y > v.y, z > v.z, w > v.w);
	}

	bvec4 greaterThanEqual(const vec4 v) const
	{
	    return bvec4(x >= v.x, y >= v.y, z >= v.z, w >= v.w);
	}

	bvec4 lessThan(const vec4 v) const
	{
	    return bvec4(x < v.x, y < v.y, z < v.z, w < v.w);
	}

	bvec4 lessThanEqual(const vec4 v) const
	{
	    return bvec4(x <= v.x, y <= v.y, z <= v.z, w <= v.w);
	}
    };

    /* Trigonometric functions */

    vec4 sin(const vec4 v)
    {
	return v.sin();
    }

    vec4 cos(const vec4 v)
    {
	return v.cos();
    }

    vec4 tan(const vec4 v)
    {
	return v.tan();
    }

    vec4 asin(const vec4 v)
    {
	return v.asin();
    }

    vec4 acos(const vec4 v)
    {
	return v.acos();
    }

    vec4 atan(const vec4 v)
    {
	return v.atan();
    }

    vec4 atan(const vec4 v, const vec4 w)
    {

	return v.atan(w);
    }

    vec4 radians(const vec4 v)
    {
	return v.radians();
    }

    vec4 degrees(const vec4 v)
    {
	return v.degrees();
    }

    /* Exponential functions */

    vec4 pow(const vec4 v, const float p)
    {
	return v.pow(p);
    }

    vec4 exp(const vec4 v)
    {
	return v.exp();
    }

    vec4 log(const vec4 v)
    {
	return v.log();
    }

    vec4 sqrt(const vec4 v)
    {
	return v.sqrt();
    }

    /* Common functions */

    vec4 abs(const vec4 v)
    {
	return v.abs();
    }

    vec4 sign(const vec4 v)
    {
	return v.sign();
    }

    vec4 floor(const vec4 v)
    {
	return v.floor();
    }

    vec4 ceil(const vec4 v)
    {
	return v.ceil();
    }

    vec4 fract(const vec4 v)
    {
	return v.fract();
    }

    vec4 min(const vec4 v, const float w)
    {
	return v.min(w);
    }

    vec4 min(const vec4 v, const vec4 w)
    {
	return v.min(w);
    }

    vec4 max(const vec4 v, const float w)
    {
	return v.max(w);
    }

    vec4 max(const vec4 v, const vec4 w)
    {
	return v.max(w);
    }

    vec4 clamp(const vec4 v, const float minval, const float maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec4 clamp(const vec4 v, const vec4 minval, const vec4 maxval)
    {
	return v.clamp(minval, maxval);
    }

    vec4 mix(const vec4 v, const vec4 w, const float alpha)
    {
	return v.mix(w, alpha);
    }

    vec4 mix(const vec4 v, const vec4 w, const vec4 alpha)
    {
	return v.mix(w, alpha);
    }

    vec4 step(const vec4 v, const float edge)
    {
	return v.step(edge);
    }

    vec4 step(const vec4 v, const vec4 edge)
    {
	return v.step(edge);
    }

    vec4 smoothstep(const vec4 v, const float edge0, const float edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec4 smoothstep(const vec4 v, const vec4 edge0, const vec4 edge1)
    {
	return v.smoothstep(edge0, edge1);
    }

    vec4 mod(const vec4 v, const float y)
    {
	return v.mod(y);
    }

    vec4 mod(const vec4 v, const vec4 y)
    {
	return v.mod(y);
    }

    /* Geometric functions */

    float length(const vec4 v)
    {
	return v.length();
    }

    float distance(const vec4 v, const vec4 w)
    {
	return v.distance(w);
    }

    float dot(const vec4 v, const vec4 w)
    {
	return v.dot(w);
    }

    vec4 normalize(const vec4 v)
    {
	return v.normalize();
    }

    vec4 faceforward(const vec4 N, const vec4 I, const vec4 Nref)
    {
	return N.faceforward(I, Nref);
    }

    vec4 reflect(const vec4 I, const vec4 N)
    {
	return I.reflect(N);
    }

    vec4 refract(const vec4 I, const vec4 N, float eta)
    {
	return I.refract(N, eta);
    }

    /* Comparion functions */

    bvec4 equal(const vec4 v, const vec4 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.equal(w, epsilon);
    }

    bvec4 notEqual(const vec4 v, const vec4 w, const float epsilon = std::numeric_limits<float>::epsilon())
    {
	return v.notEqual(w, epsilon);
    }

    bvec4 greaterThan(const vec4 v, const vec4 w)
    {
	return v.greaterThan(w);
    }

    bvec4 greaterThanEqual(const vec4 v, const vec4 w)
    {
	return v.greaterThanEqual(w);
    }

    bvec4 lessThan(const vec4 v, const vec4 w)
    {
	return v.lessThan(w);
    }

    bvec4 lessThanEqual(const vec4 v, const vec4 w)
    {
	return v.lessThanEqual(w);
    }

    class mat2
    {
	public:

	union
	{
	    float v[2][2];
	    float vl[4];
	};

	/* Constructors, Destructor */

	mat2() {}

	mat2(float v00, float v01, float v10, float v11)
	{
	    this->v[0][0] = v00;
	    this->v[0][1] = v01;
	    this->v[1][0] = v10;
	    this->v[1][1] = v11;
	}

	mat2(const float *vl)
	{
	    memcpy(this->vl, vl, 4 * sizeof(float));
	}

	mat2(const double *vl)
	{
	    for (int i = 0; i < 4; i++)
		this->vl[0] = vl[0];
	}

	~mat2() {}

	/* Operators */

	const mat2& operator=(const mat2& v)
	{
	    memcpy(this->vl, v.vl, 4 * sizeof(float));
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return vl[i];
	}

	const float &operator[](size_t i) const
	{
	    return vl[i];
	}
	
	mat2 operator*(const float s) const
	{
	    mat2 m = *this;
	    for (int i = 0; i < 4; i++)
		m.vl[i] *= s;
	    return m;
	}

	mat2 operator/(const float s) const
	{
	    mat2 m = *this;
	    for (int i = 0; i < 4; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const mat2 &operator*=(float s)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const mat2 &operator/=(float s)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] /= s;
	    return *this;
	}

	mat2 operator+(const mat2& m) const
	{
	    mat2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	mat2 operator-(const mat2& m) const
	{
	    mat2 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	mat2 operator*(const mat2& n) const 
	{
	    mat2 r;
	    r.v[0][0] = v[0][0] * n.v[0][0] + v[0][1] * n.v[1][0];
	    r.v[1][0] = v[1][0] * n.v[0][0] + v[1][1] * n.v[1][0];
	    r.v[0][1] = v[0][0] * n.v[0][1] + v[0][1] * n.v[1][1];
	    r.v[1][1] = v[1][0] * n.v[0][1] + v[1][1] * n.v[1][1];
	    return r;
	}

	mat2 operator-() const
	{
	    mat2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const mat2& operator+=(const mat2& m)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const mat2& operator-=(const mat2& m)
	{
	    for (int i = 0; i < 4; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const mat2& operator*=(const mat2& m)
	{
	    mat2 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

	mat2 transpose() const
	{
	    mat2 r;
	    for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

	mat2 matrixCompMult(const mat2 m) const
	{
	    mat2 r;
	    for (int i = 0; i < 4; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}
    };

    mat2 outerProduct(const vec2 v, const vec2 w)
    {
	mat2 m;
	for (int i = 0; i < 2; i++)
	    for (int j = 0; j < 2; j++)
		m.v[i][j] = v.v[i] * w.v[j];
	return m;
    }

    mat2 transpose(const mat2 m)
    {
	return m.transpose();
    }

    mat2 matrixCompMult(const mat2 m, const mat2 n)
    {
	return m.matrixCompMult(n);
    }

    class mat3
    {
	public:

	union
	{
	    float v[3][3];
	    float vl[9];
	};

	/* Constructors, Destructor */

	mat3() {}

	mat3(float v00, float v01, float v02,
		float v10, float v11, float v12,
		float v20, float v21, float v22)
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

	mat3(const float *vl)
	{
	    memcpy(this->vl, vl, 9 * sizeof(float));
	}

	mat3(const double *vl)
	{
	    for (int i = 0; i < 9; i++)
		this->vl[0] = vl[0];
	}

	~mat3() {}

	/* Operators */

	const mat3& operator=(const mat3& v)
	{
	    memcpy(this->vl, v.vl, 9 * sizeof(float));
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return vl[i];
	}

	const float &operator[](size_t i) const
	{
	    return vl[i];
	}
	
	mat3 operator*(const float s) const
	{
	    mat3 m = *this;
	    for (int i = 0; i < 9; i++)
		m.vl[i] *= s;
	    return m;
	}

	mat3 operator/(const float s) const
	{
	    mat3 m = *this;
	    for (int i = 0; i < 9; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const mat3 &operator*=(float s)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const mat3 &operator/=(float s)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] /= s;
	    return *this;
	}

	mat3 operator+(const mat3& m) const
	{
	    mat3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	mat3 operator-(const mat3& m) const
	{
	    mat3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	mat3 operator*(const mat3& n) const 
	{
	    mat3 r;
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

	mat3 operator-() const
	{
	    mat3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const mat3& operator+=(const mat3& m)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const mat3& operator-=(const mat3& m)
	{
	    for (int i = 0; i < 9; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const mat3& operator*=(const mat3& m)
	{
	    mat3 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

	mat3 transpose() const
	{
	    mat3 r;
	    for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

	mat3 matrixCompMult(const mat3 m) const
	{
	    mat3 r;
	    for (int i = 0; i < 9; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}
    };

    mat3 outerProduct(const vec3 v, const vec3 w)
    {
	mat3 m;
	for (int i = 0; i < 3; i++)
	    for (int j = 0; j < 3; j++)
		m.v[i][j] = v.v[i] * w.v[j];
	return m;
    }

    mat3 transpose(const mat3 m)
    {
	return m.transpose();
    }

    mat3 matrixCompMult(const mat3 m, const mat3 n)
    {
	return m.matrixCompMult(n);
    }

    class mat4
    {
	public:

	union
	{
	    float v[4][4];
	    float vl[16];
	};

	/* Constructors, Destructor */

	mat4() {}

	mat4(float v00, float v01, float v02, float v03,
		float v10, float v11, float v12, float v13,
		float v20, float v21, float v22, float v23,
		float v30, float v31, float v32, float v33)
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

	mat4(const float *vl)
	{
	    memcpy(this->vl, vl, 16 * sizeof(float));
	}

	mat4(const double *vl)
	{
	    for (int i = 0; i < 16; i++)
		this->vl[0] = vl[0];
	}

	~mat4() {}

	/* Operators */

	const mat4& operator=(const mat4& v)
	{
	    memcpy(this->vl, v.vl, 16 * sizeof(float));
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return vl[i];
	}

	const float &operator[](size_t i) const
	{
	    return vl[i];
	}
	
	mat4 operator*(const float s) const
	{
	    mat4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.vl[i] *= s;
	    return m;
	}

	mat4 operator/(const float s) const
	{
	    mat4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.vl[i] /= s;
	    return m;
	}
     
	const mat4 &operator*=(float s)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const mat4 &operator/=(float s)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] /= s;
	    return *this;
	}

	mat4 operator+(const mat4& m) const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] + m.vl[i];
	    return m;
	}

	mat4 operator-(const mat4& m) const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] - m.vl[i];
	    return m;
	}

	mat4 operator*(const mat4& n) const 
	{
	    mat4 r;
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

	mat4 operator-() const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	const mat4& operator+=(const mat4& m)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] += m.vl[i];
	    return *this;
	}

	const mat4& operator-=(const mat4& m)
	{
	    for (int i = 0; i < 16; i++)
		vl[i] -= m.vl[i];
	    return *this;
	}

	const mat4& operator*=(const mat4& m)
	{
	    mat4 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

	mat4 transpose() const
	{
	    mat4 r;
	    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		    r.v[i][j] = v[j][i];
	    return r;
	}

	mat4 matrixCompMult(const mat4 m) const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.vl[i] = vl[i] * m.vl[i];
	    return r;
	}
    };

    mat4 outerProduct(const vec4 v, const vec4 w)
    {
	mat4 m;
	for (int i = 0; i < 4; i++)
	    for (int j = 0; j < 4; j++)
		m.v[i][j] = v.v[i] * w.v[j];
	return m;
    }

    mat4 transpose(const mat4 m)
    {
	return m.transpose();
    }

    mat4 matrixCompMult(const mat4 m, const mat4 n)
    {
	return m.matrixCompMult(n);
    }
}

#endif
