/*
 * glv.h
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

#include <cmath>
#include <cstring>

namespace glv
{
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
	    bool xyz[3];
	    bool rgb[3];
	};

	/* Constructors, Destructor */

	bvec3() {}

	bvec3(bool x, bool y, bool z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
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
	    return xyz[i];
	}

	const bool &operator[](size_t i) const
	{
	    return xyz[i];
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

	bvec3 negated() const
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
	    float xyz[3];
	    float rgb[3];
	};

	/* Constructors, Destructor */

	vec3() {}

	vec3(float x, float y, float z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}

	vec3(const float *xyz)
	{
	    this->x = xyz[0];
	    this->y = xyz[1];
	    this->z = xyz[2];
	}

	~vec3() {}

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
	    return xyz[i];
	}

	const float &operator[](size_t i) const
	{
	    return xyz[i];
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
	    return this->equal(v, epsilon).negated();
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


    class mat4
    {
	public:

	union
	{
	    float m[4][4];
	    float ml[16];
	};

	/* Constructors, Destructor */

	mat4() {}

	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
	    this->m[0][0] = m00;
	    this->m[0][1] = m01;
	    this->m[0][2] = m02;
	    this->m[0][3] = m03;
	    this->m[1][0] = m10;
	    this->m[1][1] = m11;
	    this->m[1][2] = m12;
	    this->m[1][3] = m13;
	    this->m[2][0] = m20;
	    this->m[2][1] = m21;
	    this->m[2][2] = m22;
	    this->m[2][3] = m23;
	    this->m[3][0] = m30;
	    this->m[3][1] = m31;
	    this->m[3][2] = m32;
	    this->m[3][3] = m33;
	}

	mat4(const float *ml)
	{
	    memcpy(this->ml, ml, 16 * sizeof(float));
	}

	mat4(const double *ml)
	{
	    for (int i = 0; i < 16; i++)
		this->ml[0] = ml[0];
	}

	~mat4() {}

	/* Matrix operations */

	mat4 transposed() const
	{
	    mat4 r;
	    for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		    r.m[i][j] = m[j][i];
	    return r;
	}

	const mat4 &transpose()
	{
	    mat4 r = this->transposed();
	    *this = r;
	    return *this;
	}

	/* Operators */

	const mat4& operator=(const mat4& v)
	{
	    memcpy(this->ml, v.ml, 16 * sizeof(float));
	    return *this;
	}

	float &operator[](size_t i)
	{
	    return ml[i];
	}

	const float &operator[](size_t i) const
	{
	    return ml[i];
	}
	
	mat4 operator*(const float s) const
	{
	    mat4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.ml[i] *= s;
	    return m;
	}

	mat4 operator/(const float s) const
	{
	    mat4 m = *this;
	    for (int i = 0; i < 16; i++)
		m.ml[i] /= s;
	    return m;
	}
     
	const mat4 &operator*=(float s)
	{
	    for (int i = 0; i < 16; i++)
		ml[i] *= s;
	    return *this;
	}
     
	const mat4 &operator/=(float s)
	{
	    for (int i = 0; i < 16; i++)
		ml[i] /= s;
	    return *this;
	}

	mat4 operator+(const mat4& m) const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.ml[i] = ml[i] + m.ml[i];
	    return m;
	}

	mat4 operator-(const mat4& m) const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.ml[i] = ml[i] - m.ml[i];
	    return m;
	}

	mat4 operator*(const mat4& n) const 
	{
	    mat4 r;
	    r.m[0][0] = m[0][0] * n.m[0][0] + m[0][1] * n.m[1][0] + m[0][2] * n.m[2][0] + m[0][3] * n.m[3][0];
	    r.m[1][0] = m[1][0] * n.m[0][0] + m[1][1] * n.m[1][0] + m[1][2] * n.m[2][0] + m[1][3] * n.m[3][0];
	    r.m[2][0] = m[2][0] * n.m[0][0] + m[2][1] * n.m[1][0] + m[2][2] * n.m[2][0] + m[2][3] * n.m[3][0];
	    r.m[3][0] = m[3][0] * n.m[0][0] + m[3][1] * n.m[1][0] + m[3][2] * n.m[2][0] + m[3][3] * n.m[3][0];
	    r.m[0][1] = m[0][0] * n.m[0][1] + m[0][1] * n.m[1][1] + m[0][2] * n.m[2][1] + m[0][3] * n.m[3][1];
	    r.m[1][1] = m[1][0] * n.m[0][1] + m[1][1] * n.m[1][1] + m[1][2] * n.m[2][1] + m[1][3] * n.m[3][1];
	    r.m[2][1] = m[2][0] * n.m[0][1] + m[2][1] * n.m[1][1] + m[2][2] * n.m[2][1] + m[2][3] * n.m[3][1];
	    r.m[3][1] = m[3][0] * n.m[0][1] + m[3][1] * n.m[1][1] + m[3][2] * n.m[2][1] + m[3][3] * n.m[3][1];
	    r.m[0][2] = m[0][0] * n.m[0][2] + m[0][1] * n.m[1][2] + m[0][2] * n.m[2][2] + m[0][3] * n.m[3][2];
	    r.m[1][2] = m[1][0] * n.m[0][2] + m[1][1] * n.m[1][2] + m[1][2] * n.m[2][2] + m[1][3] * n.m[3][2];
	    r.m[2][2] = m[2][0] * n.m[0][2] + m[2][1] * n.m[1][2] + m[2][2] * n.m[2][2] + m[2][3] * n.m[3][2];
	    r.m[3][2] = m[3][0] * n.m[0][2] + m[3][1] * n.m[1][2] + m[3][2] * n.m[2][2] + m[3][3] * n.m[3][2];
	    r.m[0][3] = m[0][0] * n.m[0][3] + m[0][1] * n.m[1][3] + m[0][2] * n.m[2][3] + m[0][3] * n.m[3][3];
	    r.m[1][3] = m[1][0] * n.m[0][3] + m[1][1] * n.m[1][3] + m[1][2] * n.m[2][3] + m[1][3] * n.m[3][3];
	    r.m[2][3] = m[2][0] * n.m[0][3] + m[2][1] * n.m[1][3] + m[2][2] * n.m[2][3] + m[2][3] * n.m[3][3];
	    r.m[3][3] = m[3][0] * n.m[0][3] + m[3][1] * n.m[1][3] + m[3][2] * n.m[2][3] + m[3][3] * n.m[3][3];
	    return r;
	}

	mat4 operator-() const
	{
	    mat4 r;
	    for (int i = 0; i < 16; i++)
		r.ml[i] = -ml[i];
	    return r;
	}

	const mat4& operator+=(const mat4& m)
	{
	    for (int i = 0; i < 16; i++)
		ml[i] += m.ml[i];
	    return *this;
	}

	const mat4& operator-=(const mat4& m)
	{
	    for (int i = 0; i < 16; i++)
		ml[i] -= m.ml[i];
	    return *this;
	}

	const mat4& operator*=(const mat4& m)
	{
	    mat4 r = *this * m;
	    *this = r;
	    return *this;
	}
    };
}
