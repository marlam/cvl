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
 * vec4, mat2, mat3, mat4, mat2x3, mat3x2, mat2x4, mat4x2, mat3x4, mat4x3 (and 
 * the variants bvec, ivec, dvec, dmat).
 * Additionally, there's a quaternion class (quat and dquat).
 *
 * Vector elements are called (x,y,z,w) and (r,g,b,a) and (s,t,p,q). Swizzling
 * is fully supported, including assignments! Just remember that the swizzlers
 * are member functions (use parentheses).
 * Example: "v4.wz() = v3.rg() + v2;"
 *
 * All data elements are accessible via a two-dimensional array v or a linear
 * array vl. Both v and vl are row-major, unlike OpenGL, but like everyone else.
 * Use transpose() when reading OpenGL data (or use the functions from glvm-gl.h).
 *
 * Everything that is specified by GLSL 1.30 should work, unless it is
 * impossible to implement in C++.
 */

#ifndef GLVM_H
#define GLVM_H

#include <cmath>	// for sin(), sinf() etc.
#include <cstdlib>	// for abs()

#include <limits>
#include <string>
#include <iostream>
#include <sstream>


namespace glvm
{
    /* Define the GLSL functions for the base data types (bool, int, float,
     * double). */

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


    /* The array class is the base for both the vector and the matrix class. It
     * defines everything that is valid for both vectors and matrices, e.g. all
     * functions and operators that work per component. */

    template<typename T, unsigned int rows, unsigned int cols>
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

	// The following constructor are needed to construct vector and matrix
	// objects from a series of values. We need constructors for 1, 2, 3, 
	// 4, 6, 8, 9, 12, and 16 elements.

	array(const T x)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
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
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] = a[i];
	}

	template<typename U>
	array(const array<U, rows, cols> &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] = a.vl[i];
	}

	array(const std::string &s)
	{
	    std::istringstream is(s);
	    for (unsigned int i = 0; i < rows * cols; i++)
		is >> vl[i];
	}

	~array() {}

	/* Get / set rows, columns, and sub-arrays */

	array<T, cols, 1> row(const int r) const
	{
	    array<T, cols, 1> row;
	    for (unsigned int i = 0; i < cols; i++)
		row.vl[i] = v[r][i];
	    return row;
	}

	void row(const int r, const array<T, cols, 1> &row)
	{
	    for (unsigned int i = 0; i < cols; i++)
		row.v[r][i] = row.vl[i];
	}

	array<T, rows, 1> col(const int c) const
	{
	    array<T, rows, 1> col;
	    for (unsigned int i = 0; i < rows; i++)
		col.vl[i] = vl[i][c];
	    return col;
	}

	void col(const int c, const array<T, rows, 1> &col)
	{
	    for (unsigned int i = 0; i < rows; i++)
		v[i][c] = col.vl[i];
	}

	array<T, rows - 1, cols - 1> strike(const int row, const int col) const
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

	friend array<T, rows - 1, cols - 1> strike(const array &m, const int row, const int col)
	{
	    return m.strike(row, col);
	}

	template<unsigned int subrows, unsigned int subcols>
	void setSubArray(const array<T, subrows, subcols> &m, const int row = 0, const int col = 0)
	{
	    for (unsigned int i = 0; i < subrows; i++)
		for (unsigned int j = 0; j < subcols; j++)
		    v[row + i][col + j] = m.v[i][j];
	}

	void set2x2(const array<T, 2, 2> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set3x3(const array<T, 3, 3> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set2x3(const array<T, 2, 3> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set3x2(const array<T, 3, 2> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set2x4(const array<T, 2, 4> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set4x2(const array<T, 4, 2> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set3x4(const array<T, 3, 4> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	void set4x3(const array<T, 4, 3> &m, const int row = 0, const int col = 0)
	{
	    setSubArray(m, row, col);
	}

	/* Operators */

	const array &operator=(const array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] = a.vl[i];
	    return *this;
	}
	
	array operator*(const T s) const
	{
	    array a;
	    for (unsigned int i = 0; i < rows * cols; i++)
		a.vl[i] = vl[i] * s;
	    return a;
	}

    	friend array operator*(const T s, const array &a)
	{
	    return a * s;
	}

	array operator/(const T s) const
	{
	    array a;
	    for (unsigned int i = 0; i < rows * cols; i++)
		a.vl[i] = vl[i] / s;
	    return a;
	}

	const array &operator*=(const T s)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] *= s;
	    return *this;
	}
     
	const array &operator/=(const T s)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] /= s;
	    return *this;
	}

	array operator+(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] + a.vl[i];
	    return r;
	}

	const array &operator+=(const array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] += a.vl[i];
	    return *this;
	}

	array operator-(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] - a.vl[i];
	    return r;
	}

	const array &operator-=(const array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] -= a.vl[i];
	    return *this;
	}

	array operator-() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = -vl[i];
	    return r;
	}

	array compMult(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] * a.vl[i];
	    return r;
	}

	const array &compMultAssign(const array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] *= a.vl[i];
	    return *this;
	}

	array compDiv(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] / a.vl[i];
	    return r;
	}

	const array &compDivAssign(const array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		vl[i] /= a.vl[i];
	    return *this;
	}

	std::string str() const
	{
	    std::ostringstream os;
	    os.precision(std::numeric_limits<T>::digits10);
	    for (unsigned int i = 0; i < rows * cols; i++)
		os << vl[i] << " ";
	    return os.str();
	}

	friend std::ostream &operator<<(std::ostream &os, const array &a)
	{
	    return os << a.str();
	}

	friend std::istream &operator>>(std::istream &is, array &a)
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		is >> a.vl[i];
	    return is;
	}

	/* Trigonometric functions */

	array sin() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sin(vl[i]);
	    return r;
	}

	friend array sin(const array &a)
	{
    	    return a.sin();
       	}

	array cos() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::cos(vl[i]);
	    return r;
	}

	friend array cos(const array &a)
	{
    	    return a.cos();
       	}

	array tan() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::tan(vl[i]);
	    return r;
	}

	array asin() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::asin(vl[i]);
	    return r;
	}

	friend array asin(const array &a)
	{
    	    return a.asin();
       	}

	array acos() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::acos(vl[i]);
	    return r;
	}

	friend array acos(const array &a)
	{
    	    return a.acos();
       	}

	array atan() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::atan(vl[i]);
	    return r;
	}

	friend array atan(const array &a)
	{
    	    return a.atan();
       	}

	array atan(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::atan(vl[i], a.vl[i]);
	    return r;
	}

	friend array atan(const array &a, const array &b)
	{
    	    return a.atan(b);
       	}

	array radians() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::radians(r.vl[i]);
	    return r;
	}

	friend array radians(const array &a)
	{
    	    return a.radians();
       	}

	array degrees() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::degrees(r.vl[i]);
	    return r;
	}

	friend array degrees(const array &a)
	{
    	    return a.degrees();
       	}

	/* Exponential functions */

	array pow(const T p) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::pow(vl[i], p);
	    return r;
	}

	friend array pow(const array &a, const T p)
	{
    	    return a.pow(p);
       	}

	array exp() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::exp(vl[i]);
	    return r;
	}

	friend array exp(const array &a)
	{
    	    return a.exp();
       	}

	array exp2() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::exp2(vl[i]);
	    return r;
	}

	friend array exp2(const array &a)
	{
    	    return a.exp2();
       	}

	array log() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log(vl[i]);
	    return r;
	}

	friend array log(const array &a)
	{
    	    return a.log();
       	}

	array log2() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log2(vl[i]);
	    return r;
	}

	friend array log2(const array &a)
	{
    	    return a.log2();
       	}

	array log10() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::log10(vl[i]);
	    return r;
	}

	friend array log10(const array &a)
	{
    	    return a.log10();
       	}

	array sqrt() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sqrt(vl[i]);
	    return r;
	}

	friend array sqrt(const array &a)
	{
    	    return a.sqrt();
       	}

	array inversesqrt() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::inversesqrt(vl[i]);
	    return r;
	}

	friend array inversesqrt(const array &a)
	{
    	    return a.inversesqrt();
       	}

	array cbrt() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::cbrt(vl[i]);
	    return r;
	}

	friend array cbrt(const array &a)
	{
    	    return a.cbrt();
       	}

	/* Common functions */

	array<bool, rows, cols> isfinite() const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isfinite(vl[i]);
	    return r;
	}

	friend array<bool, rows, cols> isfinite(const array &a)
	{
	    return a.isfinite();
	}

	array<bool, rows, cols> isinf() const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isinf(vl[i]);
	    return r;
	}

	friend array<bool, rows, cols> isinf(const array &a)
	{
	    return a.isinf();
	}

	array<bool, rows, cols> isnan() const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isnan(vl[i]);
	    return r;
	}

	friend array<bool, rows, cols> isnan(const array &a)
	{
	    return a.isnan();
	}

	array<bool, rows, cols> isnormal() const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::isnormal(vl[i]);
	    return r;
	}

	friend array<bool, rows, cols> isnormal(const array &a)
	{
	    return a.isnormal();
	}

	array abs() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::abs(vl[i]);
	    return r;
	}

	friend array abs(const array &a)
	{
    	    return a.abs();
       	}

	array sign() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::sign(vl[i]);
	    return r;
	}

	friend array sign(const array &a)
	{
    	    return a.sign();
       	}

	array floor() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::floor(vl[i]);
	    return r;
	}

	friend array floor(const array &a)
	{
    	    return a.floor();
       	}

	array ceil() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::ceil(vl[i]);
	    return r;
	}

	friend array ceil(const array &a)
	{
    	    return a.ceil();
       	}

	array round() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::round(vl[i]);
	    return r;
	}

	friend array round(const array &a)
	{
    	    return a.round();
       	}

	array fract() const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::fract(vl[i]);
	    return r;
	}

	friend array fract(const array &a)
	{
    	    return a.fract();
       	}

	array min(const T x) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::min(x, vl[i]);
	    return r;
	}

	friend array min(const array &a, const T w)
	{
    	    return a.min(w);
       	}

	array min(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::min(a.vl[i], vl[i]);
	    return r;
	}

	friend array min(const array &a, const array &b)
	{
    	    return a.min(b);
       	}

	array max(const T x) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::max(x, vl[i]);
	    return r;
	}

	friend array max(const array &a, const T w)
	{
    	    return a.max(w);
       	}

	array max(const array &a) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::max(a.vl[i], vl[i]);
	    return r;
	}

	friend array max(const array &a, const array &b)
	{
    	    return a.max(b);
       	}

	array clamp(const T minval, const T maxval) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::clamp(vl[i], minval, maxval);
	    return r;
	}

	friend array clamp(const array &a, const T minval, const T maxval)
	{
    	    return a.clamp(minval, maxval);
       	}

	array clamp(const array &minval, const array &maxval) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::clamp(vl[i], minval.vl[i], maxval.vl[i]);
	    return r;
	}

	friend array clamp(const array &a, const array &minval, const array &maxval)
	{
    	    return a.clamp(minval, maxval);
       	}

	array mix(const array &a, const T alpha) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mix(vl[i], a.vl[i], alpha);
	    return r;
	}

	friend array mix(const array &a, const array &b, const T alpha)
	{
    	    return a.mix(b, alpha);
       	}

	array mix(const array &a, const array &alpha) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mix(vl[i], a.vl[i], alpha.vl[i]);
	    return r;
	}

	friend array mix(const array &a, const array &b, const array &alpha)
	{
    	    return a.mix(b, alpha);
       	}

	array step(const T edge) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::step(vl[i], edge);
	    return r;
	}

	friend array step(const array &a, const T edge)
	{
    	    return a.step(edge);
       	}

	array step(const array &edge) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::step(vl[i], edge.vl[i]);
	    return r;
	}

	friend array step(const array &a, const array &edge)
	{
    	    return a.step(edge);
       	}

	array smoothstep(const T edge0, const T edge1) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::smoothstep(vl[i], edge0, edge1);
	    return r;
	}

	friend array smoothstep(const array &a, const T edge0, const T edge1)
	{
    	    return a.smoothstep(edge0, edge1);
       	}

	array smoothstep(const array &edge0, const array &edge1) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::smoothstep(vl[i], edge0.vl[i], edge1.vl[i]);
	    return r;
	}

	friend array smoothstep(const array &a, const array &edge0, const array &edge1)
	{
    	    return a.smoothstep(edge0, edge1);
       	}

	array mod(const T y) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mod(vl[i], y);
	    return r;
	}

	friend array mod(const array &a, const T y)
	{
    	    return a.mod(y);
       	}

	array mod(const array &y) const
	{
	    array r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::mod(vl[i], y.vl[i]);
	    return r;
	}

	friend array mod(const array &a, const array &y)
	{
    	    return a.mod(y);
       	}

	/* Comparison functions */

	array<bool, rows, cols> greaterThan(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] > a.vl[i];
	    return r;
	}

	friend array<bool, rows, cols> greaterThan(const array &a, const array &b)
	{
    	    return a.greaterThan(b);
       	}

	array<bool, rows, cols> greaterThanEqual(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] >= a.vl[i];
	    return r;
	}

	friend array<bool, rows, cols> greaterThanEqual(const array &a, const array &b)
	{
    	    return a.greaterThanEqual(b);
       	}

	array<bool, rows, cols> lessThan(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] < a.vl[i];
	    return r;
	}

	friend array<bool, rows, cols> lessThan(const array &a, const array &b)
	{
    	    return a.lessThan(b);
       	}

	array<bool, rows, cols> lessThanEqual(const array &a) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = vl[i] <= a.vl[i];
	    return r;
	}

	friend array<bool, rows, cols> lessThanEqual(const array &a, const array &b)
	{
    	    return a.lessThanEqual(b);
       	}

	array<bool, rows, cols> equal(const array &a, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::abs(vl[i] - a.vl[i]) <= epsilon;
	    return r;
	}

	friend array<bool, rows, cols> equal(const array &a, const array &b, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return a.equal(b, epsilon);
       	}

	array<bool, rows, cols> notEqual(const array &a, const T epsilon = std::numeric_limits<T>::epsilon()) const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = glvm::abs(vl[i] - a.vl[i]) > epsilon;
	    return r;
	}

	friend array<bool, rows, cols> notEqual(const array &a, const array &b, const T epsilon = std::numeric_limits<T>::epsilon())
	{
    	    return a.notEqual(b, epsilon);
       	}

	bool any() const
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		if (vl[i])
		    return true;
	    return false;
	}

	bool all() const
	{
	    for (unsigned int i = 0; i < rows * cols; i++)
		if (!vl[i])
		    return false;
	    return true;
	}

	array<bool, rows, cols> negate() const
	{
	    array<bool, rows, cols> r;
	    for (unsigned int i = 0; i < rows * cols; i++)
		r.vl[i] = !vl[i];
	    return r;
	}

	/* Geometric functions */

	T length() const
	{
	    T l = static_cast<T>(0);
	    for (unsigned int i = 0; i < rows; i++)
		l += vl[i] * vl[i];
	    return glvm::sqrt(l);
	}

	friend T length(const array &a)
	{
    	    return a.length();
       	}

	T magnitude() const
	{
	    return this->length();
	}

	friend T magnitude(const array &a)
	{
    	    return a.magnitude();
       	}

	T distance(const array &a) const
	{
	    return (*this - a).length();
	}

	friend T distance(const array &a, const array &b)
	{
    	    return a.distance(b);
       	}

	T dot(const array &a) const
	{
	    T d = static_cast<T>(0);
	    for (unsigned int i = 0; i < rows; i++)
		d += vl[i] * a.vl[i];
	    return d;
	}

	friend T dot(const array &a, const array &b)
	{
    	    return a.dot(b);
       	}

	array normalize() const
	{
	    return *this / this->length();
	}

	friend array normalize(const array &a)
	{
    	    return a.normalize();
       	}

	array faceforward(const array &I, const array &Nref) const
	{
	    return Nref.dot(I) < static_cast<T>(0) ? *this : - *this;
	}

	friend array faceforward(const array &N, const array &I, const array &Nref)
	{
    	    return N.faceforward(I, Nref);
       	}

	array reflect(const array &N) const
	{
	    return *this - N * (N.dot(*this) * static_cast<T>(2));
	}

	friend array reflect(const array &I, const array &N)
	{
    	    return I.reflect(N);
       	}

	array refract(const array &N, T eta) const
	{
	    const T d = N.dot(*this);
	    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);
	    return k < static_cast<T>(0) ? array<T, rows, 1>(static_cast<T>(0)) : *this * eta - N * (eta * d + glvm::sqrt(k));
	}

	friend array refract(const array &I, const array &N, T eta)
	{
    	    return I.refract(N, eta);
       	}

	array<T, cols, rows> transpose() const
	{
	    array<T, cols, rows> r;
	    for (unsigned int i = 0; i < rows; i++)
		for (unsigned int j = 0; j < cols; j++)
		    r.v[i][j] = array<T, rows, cols>::v[j][i];
	    return r;
	}

    	friend array<T, cols, rows> transpose(const array &a)
	{
	    return a.transpose();
	}
    };


    /* The following swizzler classes are necessary to allow assignments to 
     * swizzlers, e.g. "v.xyz() = vec3(1,2,3)". For this purpose, the swizzler
     * classes store sets of references. Since arrays of references are not 
     * allowed, the swizzler classes with 2, 3, and 4 elements are specified
     * explicitly. If you know of a more elegant solution, please let me know.
     * Note: In the operator=() function with swizzler arguments, we need to
     * copy the values first and then assign them, or else things like 
     * "v.rgb() = v.bgr()" will go wrong.
     * The compiler will optimize all of this overhead away (tested with
     * "g++-4.3 -O2"). */

    template<typename T>
    class swizzler2
    {
	public:
        T &x, &y;

    	swizzler2(T &s0, T &s1) : x(s0), y(s1) {}

    	const swizzler2 &operator=(const swizzler2 &s)
	{
	    T a = s.x;
	    T b = s.y;
	    x = a;
	    y = b;
	    return *this;
	}

    	const swizzler2 &operator=(const array<T, 2, 1> &v)
	{
	    x = v[0];
	    y = v[1];
	    return *this;
	}
    };

    template<typename T> 
    class swizzler3
    {
	public:
        T &x, &y, &z;

    	swizzler3(T &s0, T &s1, T &s2) : x(s0), y(s1), z(s2) {}

    	const swizzler3 &operator=(const swizzler3 &s)
	{
	    T a = s.x;
	    T b = s.y;
	    T c = s.z;
	    x = a;
	    y = b;
	    z = c;
	    return *this;
	}

    	const swizzler3 &operator=(const array<T, 3, 1> &v)
	{
	    x = v[0];
	    y = v[1];
	    z = v[2];
	    return *this;
	}
    };

    template<typename T>
    class swizzler4
    {
	public:
        T &x, &y, &z, &w;

    	swizzler4(T &s0, T &s1, T &s2, T &s3) : x(s0), y(s1), z(s2), w(s3) {}

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
	    return *this;
	}

    	const swizzler4 &operator=(const array<T, 4, 1> &v)
	{
	    x = v[0];
	    y = v[1];
	    z = v[2];
	    w = v[3];
	    return *this;
	}
    };

    template<typename T, unsigned int rows>
    class vector : public array<T, rows, 1>
    {
	public:

	/* Constructors, Destructor */

	vector() {}

	vector(const swizzler2<T> &s) : array<T, rows, 1>(s.x, s.y) {}

	vector(const swizzler3<T> &s) : array<T, rows, 1>(s.x, s.y, s.z) {}

	vector(const swizzler4<T> &s) : array<T, rows, 1>(s.x, s.y, s.z, s.w) {}

	vector(const array<T, rows, 1> &a) : array<T, rows, 1>(a) {}
	
	vector(const T v0, const T v1) : array<T, rows, 1>(v0, v1) {}

	vector(const T v0, const T v1, const T v2) : array<T, rows, 1>(v0, v1, v2) {}

	vector(const T v0, const T v1, const T v2, const T v3) : array<T, rows, 1>(v0, v1, v2, v3) {}

	vector(const T *v) : array<T, rows, 1>(v) {}

	template<typename U>
	vector(const vector<U, rows> &v) : array<T, rows, 1>(v) {}

	vector(const std::string &s) : array<T, rows, 1>(s) {}

	~vector() {}

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
	vector<T, 2> xx() const { return vector<T, 2>(x(), x()); }
	vector<T, 2> rr() const { return vector<T, 2>(r(), r()); }
	vector<T, 2> ss() const { return vector<T, 2>(s(), s()); }
	vector<T, 2> xy() const { return vector<T, 2>(x(), y()); }
	vector<T, 2> rg() const { return vector<T, 2>(r(), g()); }
	vector<T, 2> st() const { return vector<T, 2>(s(), t()); }
	vector<T, 2> xz() const { return vector<T, 2>(x(), z()); }
	vector<T, 2> rb() const { return vector<T, 2>(r(), b()); }
	vector<T, 2> sp() const { return vector<T, 2>(s(), p()); }
	vector<T, 2> xw() const { return vector<T, 2>(x(), w()); }
	vector<T, 2> ra() const { return vector<T, 2>(r(), a()); }
	vector<T, 2> sq() const { return vector<T, 2>(s(), q()); }
	vector<T, 2> yx() const { return vector<T, 2>(y(), x()); }
	vector<T, 2> gr() const { return vector<T, 2>(g(), r()); }
	vector<T, 2> ts() const { return vector<T, 2>(t(), s()); }
	vector<T, 2> yy() const { return vector<T, 2>(y(), y()); }
	vector<T, 2> gg() const { return vector<T, 2>(g(), g()); }
	vector<T, 2> tt() const { return vector<T, 2>(t(), t()); }
	vector<T, 2> yz() const { return vector<T, 2>(y(), z()); }
	vector<T, 2> gb() const { return vector<T, 2>(g(), b()); }
	vector<T, 2> tp() const { return vector<T, 2>(t(), p()); }
	vector<T, 2> yw() const { return vector<T, 2>(y(), w()); }
	vector<T, 2> ga() const { return vector<T, 2>(g(), a()); }
	vector<T, 2> tq() const { return vector<T, 2>(t(), q()); }
	vector<T, 2> zx() const { return vector<T, 2>(z(), x()); }
	vector<T, 2> br() const { return vector<T, 2>(b(), r()); }
	vector<T, 2> ps() const { return vector<T, 2>(p(), s()); }
	vector<T, 2> zy() const { return vector<T, 2>(z(), y()); }
	vector<T, 2> bg() const { return vector<T, 2>(b(), g()); }
	vector<T, 2> pt() const { return vector<T, 2>(p(), t()); }
	vector<T, 2> zz() const { return vector<T, 2>(z(), z()); }
	vector<T, 2> bb() const { return vector<T, 2>(b(), b()); }
	vector<T, 2> pp() const { return vector<T, 2>(p(), p()); }
	vector<T, 2> zw() const { return vector<T, 2>(z(), w()); }
	vector<T, 2> ba() const { return vector<T, 2>(b(), a()); }
	vector<T, 2> pq() const { return vector<T, 2>(p(), q()); }
	vector<T, 2> wx() const { return vector<T, 2>(w(), x()); }
	vector<T, 2> ar() const { return vector<T, 2>(a(), r()); }
	vector<T, 2> qs() const { return vector<T, 2>(q(), s()); }
	vector<T, 2> wy() const { return vector<T, 2>(w(), y()); }
	vector<T, 2> ag() const { return vector<T, 2>(a(), g()); }
	vector<T, 2> qt() const { return vector<T, 2>(q(), t()); }
	vector<T, 2> wz() const { return vector<T, 2>(w(), z()); }
	vector<T, 2> ab() const { return vector<T, 2>(a(), b()); }
	vector<T, 2> qp() const { return vector<T, 2>(q(), p()); }
	vector<T, 2> ww() const { return vector<T, 2>(w(), w()); }
	vector<T, 2> aa() const { return vector<T, 2>(a(), a()); }
	vector<T, 2> qq() const { return vector<T, 2>(q(), q()); }
	vector<T, 3> xxx() const { return vector<T, 3>(x(), x(), x()); }
	vector<T, 3> rrr() const { return vector<T, 3>(r(), r(), r()); }
	vector<T, 3> sss() const { return vector<T, 3>(s(), s(), s()); }
	vector<T, 3> xxy() const { return vector<T, 3>(x(), x(), y()); }
	vector<T, 3> rrg() const { return vector<T, 3>(r(), r(), g()); }
	vector<T, 3> sst() const { return vector<T, 3>(s(), s(), t()); }
	vector<T, 3> xxz() const { return vector<T, 3>(x(), x(), z()); }
	vector<T, 3> rrb() const { return vector<T, 3>(r(), r(), b()); }
	vector<T, 3> ssp() const { return vector<T, 3>(s(), s(), p()); }
	vector<T, 3> xxw() const { return vector<T, 3>(x(), x(), w()); }
	vector<T, 3> rra() const { return vector<T, 3>(r(), r(), a()); }
	vector<T, 3> ssq() const { return vector<T, 3>(s(), s(), q()); }
	vector<T, 3> xyx() const { return vector<T, 3>(x(), y(), x()); }
	vector<T, 3> rgr() const { return vector<T, 3>(r(), g(), r()); }
	vector<T, 3> sts() const { return vector<T, 3>(s(), t(), s()); }
	vector<T, 3> xyy() const { return vector<T, 3>(x(), y(), y()); }
	vector<T, 3> rgg() const { return vector<T, 3>(r(), g(), g()); }
	vector<T, 3> stt() const { return vector<T, 3>(s(), t(), t()); }
	vector<T, 3> xyz() const { return vector<T, 3>(x(), y(), z()); }
	vector<T, 3> rgb() const { return vector<T, 3>(r(), g(), b()); }
	vector<T, 3> stp() const { return vector<T, 3>(s(), t(), p()); }
	vector<T, 3> xyw() const { return vector<T, 3>(x(), y(), w()); }
	vector<T, 3> rga() const { return vector<T, 3>(r(), g(), a()); }
	vector<T, 3> stq() const { return vector<T, 3>(s(), t(), q()); }
	vector<T, 3> xzx() const { return vector<T, 3>(x(), z(), x()); }
	vector<T, 3> rbr() const { return vector<T, 3>(r(), b(), r()); }
	vector<T, 3> sps() const { return vector<T, 3>(s(), p(), s()); }
	vector<T, 3> xzy() const { return vector<T, 3>(x(), z(), y()); }
	vector<T, 3> rbg() const { return vector<T, 3>(r(), b(), g()); }
	vector<T, 3> spt() const { return vector<T, 3>(s(), p(), t()); }
	vector<T, 3> xzz() const { return vector<T, 3>(x(), z(), z()); }
	vector<T, 3> rbb() const { return vector<T, 3>(r(), b(), b()); }
	vector<T, 3> spp() const { return vector<T, 3>(s(), p(), p()); }
	vector<T, 3> xzw() const { return vector<T, 3>(x(), z(), w()); }
	vector<T, 3> rba() const { return vector<T, 3>(r(), b(), a()); }
	vector<T, 3> spq() const { return vector<T, 3>(s(), p(), q()); }
	vector<T, 3> xwx() const { return vector<T, 3>(x(), w(), x()); }
	vector<T, 3> rar() const { return vector<T, 3>(r(), a(), r()); }
	vector<T, 3> sqs() const { return vector<T, 3>(s(), q(), s()); }
	vector<T, 3> xwy() const { return vector<T, 3>(x(), w(), y()); }
	vector<T, 3> rag() const { return vector<T, 3>(r(), a(), g()); }
	vector<T, 3> sqt() const { return vector<T, 3>(s(), q(), t()); }
	vector<T, 3> xwz() const { return vector<T, 3>(x(), w(), z()); }
	vector<T, 3> rab() const { return vector<T, 3>(r(), a(), b()); }
	vector<T, 3> sqp() const { return vector<T, 3>(s(), q(), p()); }
	vector<T, 3> xww() const { return vector<T, 3>(x(), w(), w()); }
	vector<T, 3> raa() const { return vector<T, 3>(r(), a(), a()); }
	vector<T, 3> sqq() const { return vector<T, 3>(s(), q(), q()); }
	vector<T, 3> yxx() const { return vector<T, 3>(y(), x(), x()); }
	vector<T, 3> grr() const { return vector<T, 3>(g(), r(), r()); }
	vector<T, 3> tss() const { return vector<T, 3>(t(), s(), s()); }
	vector<T, 3> yxy() const { return vector<T, 3>(y(), x(), y()); }
	vector<T, 3> grg() const { return vector<T, 3>(g(), r(), g()); }
	vector<T, 3> tst() const { return vector<T, 3>(t(), s(), t()); }
	vector<T, 3> yxz() const { return vector<T, 3>(y(), x(), z()); }
	vector<T, 3> grb() const { return vector<T, 3>(g(), r(), b()); }
	vector<T, 3> tsp() const { return vector<T, 3>(t(), s(), p()); }
	vector<T, 3> yxw() const { return vector<T, 3>(y(), x(), w()); }
	vector<T, 3> gra() const { return vector<T, 3>(g(), r(), a()); }
	vector<T, 3> tsq() const { return vector<T, 3>(t(), s(), q()); }
	vector<T, 3> yyx() const { return vector<T, 3>(y(), y(), x()); }
	vector<T, 3> ggr() const { return vector<T, 3>(g(), g(), r()); }
	vector<T, 3> tts() const { return vector<T, 3>(t(), t(), s()); }
	vector<T, 3> yyy() const { return vector<T, 3>(y(), y(), y()); }
	vector<T, 3> ggg() const { return vector<T, 3>(g(), g(), g()); }
	vector<T, 3> ttt() const { return vector<T, 3>(t(), t(), t()); }
	vector<T, 3> yyz() const { return vector<T, 3>(y(), y(), z()); }
	vector<T, 3> ggb() const { return vector<T, 3>(g(), g(), b()); }
	vector<T, 3> ttp() const { return vector<T, 3>(t(), t(), p()); }
	vector<T, 3> yyw() const { return vector<T, 3>(y(), y(), w()); }
	vector<T, 3> gga() const { return vector<T, 3>(g(), g(), a()); }
	vector<T, 3> ttq() const { return vector<T, 3>(t(), t(), q()); }
	vector<T, 3> yzx() const { return vector<T, 3>(y(), z(), x()); }
	vector<T, 3> gbr() const { return vector<T, 3>(g(), b(), r()); }
	vector<T, 3> tps() const { return vector<T, 3>(t(), p(), s()); }
	vector<T, 3> yzy() const { return vector<T, 3>(y(), z(), y()); }
	vector<T, 3> gbg() const { return vector<T, 3>(g(), b(), g()); }
	vector<T, 3> tpt() const { return vector<T, 3>(t(), p(), t()); }
	vector<T, 3> yzz() const { return vector<T, 3>(y(), z(), z()); }
	vector<T, 3> gbb() const { return vector<T, 3>(g(), b(), b()); }
	vector<T, 3> tpp() const { return vector<T, 3>(t(), p(), p()); }
	vector<T, 3> yzw() const { return vector<T, 3>(y(), z(), w()); }
	vector<T, 3> gba() const { return vector<T, 3>(g(), b(), a()); }
	vector<T, 3> tpq() const { return vector<T, 3>(t(), p(), q()); }
	vector<T, 3> ywx() const { return vector<T, 3>(y(), w(), x()); }
	vector<T, 3> gar() const { return vector<T, 3>(g(), a(), r()); }
	vector<T, 3> tqs() const { return vector<T, 3>(t(), q(), s()); }
	vector<T, 3> ywy() const { return vector<T, 3>(y(), w(), y()); }
	vector<T, 3> gag() const { return vector<T, 3>(g(), a(), g()); }
	vector<T, 3> tqt() const { return vector<T, 3>(t(), q(), t()); }
	vector<T, 3> ywz() const { return vector<T, 3>(y(), w(), z()); }
	vector<T, 3> gab() const { return vector<T, 3>(g(), a(), b()); }
	vector<T, 3> tqp() const { return vector<T, 3>(t(), q(), p()); }
	vector<T, 3> yww() const { return vector<T, 3>(y(), w(), w()); }
	vector<T, 3> gaa() const { return vector<T, 3>(g(), a(), a()); }
	vector<T, 3> tqq() const { return vector<T, 3>(t(), q(), q()); }
	vector<T, 3> zxx() const { return vector<T, 3>(z(), x(), x()); }
	vector<T, 3> brr() const { return vector<T, 3>(b(), r(), r()); }
	vector<T, 3> pss() const { return vector<T, 3>(p(), s(), s()); }
	vector<T, 3> zxy() const { return vector<T, 3>(z(), x(), y()); }
	vector<T, 3> brg() const { return vector<T, 3>(b(), r(), g()); }
	vector<T, 3> pst() const { return vector<T, 3>(p(), s(), t()); }
	vector<T, 3> zxz() const { return vector<T, 3>(z(), x(), z()); }
	vector<T, 3> brb() const { return vector<T, 3>(b(), r(), b()); }
	vector<T, 3> psp() const { return vector<T, 3>(p(), s(), p()); }
	vector<T, 3> zxw() const { return vector<T, 3>(z(), x(), w()); }
	vector<T, 3> bra() const { return vector<T, 3>(b(), r(), a()); }
	vector<T, 3> psq() const { return vector<T, 3>(p(), s(), q()); }
	vector<T, 3> zyx() const { return vector<T, 3>(z(), y(), x()); }
	vector<T, 3> bgr() const { return vector<T, 3>(b(), g(), r()); }
	vector<T, 3> pts() const { return vector<T, 3>(p(), t(), s()); }
	vector<T, 3> zyy() const { return vector<T, 3>(z(), y(), y()); }
	vector<T, 3> bgg() const { return vector<T, 3>(b(), g(), g()); }
	vector<T, 3> ptt() const { return vector<T, 3>(p(), t(), t()); }
	vector<T, 3> zyz() const { return vector<T, 3>(z(), y(), z()); }
	vector<T, 3> bgb() const { return vector<T, 3>(b(), g(), b()); }
	vector<T, 3> ptp() const { return vector<T, 3>(p(), t(), p()); }
	vector<T, 3> zyw() const { return vector<T, 3>(z(), y(), w()); }
	vector<T, 3> bga() const { return vector<T, 3>(b(), g(), a()); }
	vector<T, 3> ptq() const { return vector<T, 3>(p(), t(), q()); }
	vector<T, 3> zzx() const { return vector<T, 3>(z(), z(), x()); }
	vector<T, 3> bbr() const { return vector<T, 3>(b(), b(), r()); }
	vector<T, 3> pps() const { return vector<T, 3>(p(), p(), s()); }
	vector<T, 3> zzy() const { return vector<T, 3>(z(), z(), y()); }
	vector<T, 3> bbg() const { return vector<T, 3>(b(), b(), g()); }
	vector<T, 3> ppt() const { return vector<T, 3>(p(), p(), t()); }
	vector<T, 3> zzz() const { return vector<T, 3>(z(), z(), z()); }
	vector<T, 3> bbb() const { return vector<T, 3>(b(), b(), b()); }
	vector<T, 3> ppp() const { return vector<T, 3>(p(), p(), p()); }
	vector<T, 3> zzw() const { return vector<T, 3>(z(), z(), w()); }
	vector<T, 3> bba() const { return vector<T, 3>(b(), b(), a()); }
	vector<T, 3> ppq() const { return vector<T, 3>(p(), p(), q()); }
	vector<T, 3> zwx() const { return vector<T, 3>(z(), w(), x()); }
	vector<T, 3> bar() const { return vector<T, 3>(b(), a(), r()); }
	vector<T, 3> pqs() const { return vector<T, 3>(p(), q(), s()); }
	vector<T, 3> zwy() const { return vector<T, 3>(z(), w(), y()); }
	vector<T, 3> bag() const { return vector<T, 3>(b(), a(), g()); }
	vector<T, 3> pqt() const { return vector<T, 3>(p(), q(), t()); }
	vector<T, 3> zwz() const { return vector<T, 3>(z(), w(), z()); }
	vector<T, 3> bab() const { return vector<T, 3>(b(), a(), b()); }
	vector<T, 3> pqp() const { return vector<T, 3>(p(), q(), p()); }
	vector<T, 3> zww() const { return vector<T, 3>(z(), w(), w()); }
	vector<T, 3> baa() const { return vector<T, 3>(b(), a(), a()); }
	vector<T, 3> pqq() const { return vector<T, 3>(p(), q(), q()); }
	vector<T, 3> wxx() const { return vector<T, 3>(w(), x(), x()); }
	vector<T, 3> arr() const { return vector<T, 3>(a(), r(), r()); }
	vector<T, 3> qss() const { return vector<T, 3>(q(), s(), s()); }
	vector<T, 3> wxy() const { return vector<T, 3>(w(), x(), y()); }
	vector<T, 3> arg() const { return vector<T, 3>(a(), r(), g()); }
	vector<T, 3> qst() const { return vector<T, 3>(q(), s(), t()); }
	vector<T, 3> wxz() const { return vector<T, 3>(w(), x(), z()); }
	vector<T, 3> arb() const { return vector<T, 3>(a(), r(), b()); }
	vector<T, 3> qsp() const { return vector<T, 3>(q(), s(), p()); }
	vector<T, 3> wxw() const { return vector<T, 3>(w(), x(), w()); }
	vector<T, 3> ara() const { return vector<T, 3>(a(), r(), a()); }
	vector<T, 3> qsq() const { return vector<T, 3>(q(), s(), q()); }
	vector<T, 3> wyx() const { return vector<T, 3>(w(), y(), x()); }
	vector<T, 3> agr() const { return vector<T, 3>(a(), g(), r()); }
	vector<T, 3> qts() const { return vector<T, 3>(q(), t(), s()); }
	vector<T, 3> wyy() const { return vector<T, 3>(w(), y(), y()); }
	vector<T, 3> agg() const { return vector<T, 3>(a(), g(), g()); }
	vector<T, 3> qtt() const { return vector<T, 3>(q(), t(), t()); }
	vector<T, 3> wyz() const { return vector<T, 3>(w(), y(), z()); }
	vector<T, 3> agb() const { return vector<T, 3>(a(), g(), b()); }
	vector<T, 3> qtp() const { return vector<T, 3>(q(), t(), p()); }
	vector<T, 3> wyw() const { return vector<T, 3>(w(), y(), w()); }
	vector<T, 3> aga() const { return vector<T, 3>(a(), g(), a()); }
	vector<T, 3> qtq() const { return vector<T, 3>(q(), t(), q()); }
	vector<T, 3> wzx() const { return vector<T, 3>(w(), z(), x()); }
	vector<T, 3> abr() const { return vector<T, 3>(a(), b(), r()); }
	vector<T, 3> qps() const { return vector<T, 3>(q(), p(), s()); }
	vector<T, 3> wzy() const { return vector<T, 3>(w(), z(), y()); }
	vector<T, 3> abg() const { return vector<T, 3>(a(), b(), g()); }
	vector<T, 3> qpt() const { return vector<T, 3>(q(), p(), t()); }
	vector<T, 3> wzz() const { return vector<T, 3>(w(), z(), z()); }
	vector<T, 3> abb() const { return vector<T, 3>(a(), b(), b()); }
	vector<T, 3> qpp() const { return vector<T, 3>(q(), p(), p()); }
	vector<T, 3> wzw() const { return vector<T, 3>(w(), z(), w()); }
	vector<T, 3> aba() const { return vector<T, 3>(a(), b(), a()); }
	vector<T, 3> qpq() const { return vector<T, 3>(q(), p(), q()); }
	vector<T, 3> wwx() const { return vector<T, 3>(w(), w(), x()); }
	vector<T, 3> aar() const { return vector<T, 3>(a(), a(), r()); }
	vector<T, 3> qqs() const { return vector<T, 3>(q(), q(), s()); }
	vector<T, 3> wwy() const { return vector<T, 3>(w(), w(), y()); }
	vector<T, 3> aag() const { return vector<T, 3>(a(), a(), g()); }
	vector<T, 3> qqt() const { return vector<T, 3>(q(), q(), t()); }
	vector<T, 3> wwz() const { return vector<T, 3>(w(), w(), z()); }
	vector<T, 3> aab() const { return vector<T, 3>(a(), a(), b()); }
	vector<T, 3> qqp() const { return vector<T, 3>(q(), q(), p()); }
	vector<T, 3> www() const { return vector<T, 3>(w(), w(), w()); }
	vector<T, 3> aaa() const { return vector<T, 3>(a(), a(), a()); }
	vector<T, 3> qqq() const { return vector<T, 3>(q(), q(), q()); }
	vector<T, 4> xxxx() const { return vector<T, 4>(x(), x(), x(), x()); }
	vector<T, 4> rrrr() const { return vector<T, 4>(r(), r(), r(), r()); }
	vector<T, 4> ssss() const { return vector<T, 4>(s(), s(), s(), s()); }
	vector<T, 4> xxxy() const { return vector<T, 4>(x(), x(), x(), y()); }
	vector<T, 4> rrrg() const { return vector<T, 4>(r(), r(), r(), g()); }
	vector<T, 4> ssst() const { return vector<T, 4>(s(), s(), s(), t()); }
	vector<T, 4> xxxz() const { return vector<T, 4>(x(), x(), x(), z()); }
	vector<T, 4> rrrb() const { return vector<T, 4>(r(), r(), r(), b()); }
	vector<T, 4> sssp() const { return vector<T, 4>(s(), s(), s(), p()); }
	vector<T, 4> xxxw() const { return vector<T, 4>(x(), x(), x(), w()); }
	vector<T, 4> rrra() const { return vector<T, 4>(r(), r(), r(), a()); }
	vector<T, 4> sssq() const { return vector<T, 4>(s(), s(), s(), q()); }
	vector<T, 4> xxyx() const { return vector<T, 4>(x(), x(), y(), x()); }
	vector<T, 4> rrgr() const { return vector<T, 4>(r(), r(), g(), r()); }
	vector<T, 4> ssts() const { return vector<T, 4>(s(), s(), t(), s()); }
	vector<T, 4> xxyy() const { return vector<T, 4>(x(), x(), y(), y()); }
	vector<T, 4> rrgg() const { return vector<T, 4>(r(), r(), g(), g()); }
	vector<T, 4> sstt() const { return vector<T, 4>(s(), s(), t(), t()); }
	vector<T, 4> xxyz() const { return vector<T, 4>(x(), x(), y(), z()); }
	vector<T, 4> rrgb() const { return vector<T, 4>(r(), r(), g(), b()); }
	vector<T, 4> sstp() const { return vector<T, 4>(s(), s(), t(), p()); }
	vector<T, 4> xxyw() const { return vector<T, 4>(x(), x(), y(), w()); }
	vector<T, 4> rrga() const { return vector<T, 4>(r(), r(), g(), a()); }
	vector<T, 4> sstq() const { return vector<T, 4>(s(), s(), t(), q()); }
	vector<T, 4> xxzx() const { return vector<T, 4>(x(), x(), z(), x()); }
	vector<T, 4> rrbr() const { return vector<T, 4>(r(), r(), b(), r()); }
	vector<T, 4> ssps() const { return vector<T, 4>(s(), s(), p(), s()); }
	vector<T, 4> xxzy() const { return vector<T, 4>(x(), x(), z(), y()); }
	vector<T, 4> rrbg() const { return vector<T, 4>(r(), r(), b(), g()); }
	vector<T, 4> sspt() const { return vector<T, 4>(s(), s(), p(), t()); }
	vector<T, 4> xxzz() const { return vector<T, 4>(x(), x(), z(), z()); }
	vector<T, 4> rrbb() const { return vector<T, 4>(r(), r(), b(), b()); }
	vector<T, 4> sspp() const { return vector<T, 4>(s(), s(), p(), p()); }
	vector<T, 4> xxzw() const { return vector<T, 4>(x(), x(), z(), w()); }
	vector<T, 4> rrba() const { return vector<T, 4>(r(), r(), b(), a()); }
	vector<T, 4> sspq() const { return vector<T, 4>(s(), s(), p(), q()); }
	vector<T, 4> xxwx() const { return vector<T, 4>(x(), x(), w(), x()); }
	vector<T, 4> rrar() const { return vector<T, 4>(r(), r(), a(), r()); }
	vector<T, 4> ssqs() const { return vector<T, 4>(s(), s(), q(), s()); }
	vector<T, 4> xxwy() const { return vector<T, 4>(x(), x(), w(), y()); }
	vector<T, 4> rrag() const { return vector<T, 4>(r(), r(), a(), g()); }
	vector<T, 4> ssqt() const { return vector<T, 4>(s(), s(), q(), t()); }
	vector<T, 4> xxwz() const { return vector<T, 4>(x(), x(), w(), z()); }
	vector<T, 4> rrab() const { return vector<T, 4>(r(), r(), a(), b()); }
	vector<T, 4> ssqp() const { return vector<T, 4>(s(), s(), q(), p()); }
	vector<T, 4> xxww() const { return vector<T, 4>(x(), x(), w(), w()); }
	vector<T, 4> rraa() const { return vector<T, 4>(r(), r(), a(), a()); }
	vector<T, 4> ssqq() const { return vector<T, 4>(s(), s(), q(), q()); }
	vector<T, 4> xyxx() const { return vector<T, 4>(x(), y(), x(), x()); }
	vector<T, 4> rgrr() const { return vector<T, 4>(r(), g(), r(), r()); }
	vector<T, 4> stss() const { return vector<T, 4>(s(), t(), s(), s()); }
	vector<T, 4> xyxy() const { return vector<T, 4>(x(), y(), x(), y()); }
	vector<T, 4> rgrg() const { return vector<T, 4>(r(), g(), r(), g()); }
	vector<T, 4> stst() const { return vector<T, 4>(s(), t(), s(), t()); }
	vector<T, 4> xyxz() const { return vector<T, 4>(x(), y(), x(), z()); }
	vector<T, 4> rgrb() const { return vector<T, 4>(r(), g(), r(), b()); }
	vector<T, 4> stsp() const { return vector<T, 4>(s(), t(), s(), p()); }
	vector<T, 4> xyxw() const { return vector<T, 4>(x(), y(), x(), w()); }
	vector<T, 4> rgra() const { return vector<T, 4>(r(), g(), r(), a()); }
	vector<T, 4> stsq() const { return vector<T, 4>(s(), t(), s(), q()); }
	vector<T, 4> xyyx() const { return vector<T, 4>(x(), y(), y(), x()); }
	vector<T, 4> rggr() const { return vector<T, 4>(r(), g(), g(), r()); }
	vector<T, 4> stts() const { return vector<T, 4>(s(), t(), t(), s()); }
	vector<T, 4> xyyy() const { return vector<T, 4>(x(), y(), y(), y()); }
	vector<T, 4> rggg() const { return vector<T, 4>(r(), g(), g(), g()); }
	vector<T, 4> sttt() const { return vector<T, 4>(s(), t(), t(), t()); }
	vector<T, 4> xyyz() const { return vector<T, 4>(x(), y(), y(), z()); }
	vector<T, 4> rggb() const { return vector<T, 4>(r(), g(), g(), b()); }
	vector<T, 4> sttp() const { return vector<T, 4>(s(), t(), t(), p()); }
	vector<T, 4> xyyw() const { return vector<T, 4>(x(), y(), y(), w()); }
	vector<T, 4> rgga() const { return vector<T, 4>(r(), g(), g(), a()); }
	vector<T, 4> sttq() const { return vector<T, 4>(s(), t(), t(), q()); }
	vector<T, 4> xyzx() const { return vector<T, 4>(x(), y(), z(), x()); }
	vector<T, 4> rgbr() const { return vector<T, 4>(r(), g(), b(), r()); }
	vector<T, 4> stps() const { return vector<T, 4>(s(), t(), p(), s()); }
	vector<T, 4> xyzy() const { return vector<T, 4>(x(), y(), z(), y()); }
	vector<T, 4> rgbg() const { return vector<T, 4>(r(), g(), b(), g()); }
	vector<T, 4> stpt() const { return vector<T, 4>(s(), t(), p(), t()); }
	vector<T, 4> xyzz() const { return vector<T, 4>(x(), y(), z(), z()); }
	vector<T, 4> rgbb() const { return vector<T, 4>(r(), g(), b(), b()); }
	vector<T, 4> stpp() const { return vector<T, 4>(s(), t(), p(), p()); }
	vector<T, 4> xyzw() const { return vector<T, 4>(x(), y(), z(), w()); }
	vector<T, 4> rgba() const { return vector<T, 4>(r(), g(), b(), a()); }
	vector<T, 4> stpq() const { return vector<T, 4>(s(), t(), p(), q()); }
	vector<T, 4> xywx() const { return vector<T, 4>(x(), y(), w(), x()); }
	vector<T, 4> rgar() const { return vector<T, 4>(r(), g(), a(), r()); }
	vector<T, 4> stqs() const { return vector<T, 4>(s(), t(), q(), s()); }
	vector<T, 4> xywy() const { return vector<T, 4>(x(), y(), w(), y()); }
	vector<T, 4> rgag() const { return vector<T, 4>(r(), g(), a(), g()); }
	vector<T, 4> stqt() const { return vector<T, 4>(s(), t(), q(), t()); }
	vector<T, 4> xywz() const { return vector<T, 4>(x(), y(), w(), z()); }
	vector<T, 4> rgab() const { return vector<T, 4>(r(), g(), a(), b()); }
	vector<T, 4> stqp() const { return vector<T, 4>(s(), t(), q(), p()); }
	vector<T, 4> xyww() const { return vector<T, 4>(x(), y(), w(), w()); }
	vector<T, 4> rgaa() const { return vector<T, 4>(r(), g(), a(), a()); }
	vector<T, 4> stqq() const { return vector<T, 4>(s(), t(), q(), q()); }
	vector<T, 4> xzxx() const { return vector<T, 4>(x(), z(), x(), x()); }
	vector<T, 4> rbrr() const { return vector<T, 4>(r(), b(), r(), r()); }
	vector<T, 4> spss() const { return vector<T, 4>(s(), p(), s(), s()); }
	vector<T, 4> xzxy() const { return vector<T, 4>(x(), z(), x(), y()); }
	vector<T, 4> rbrg() const { return vector<T, 4>(r(), b(), r(), g()); }
	vector<T, 4> spst() const { return vector<T, 4>(s(), p(), s(), t()); }
	vector<T, 4> xzxz() const { return vector<T, 4>(x(), z(), x(), z()); }
	vector<T, 4> rbrb() const { return vector<T, 4>(r(), b(), r(), b()); }
	vector<T, 4> spsp() const { return vector<T, 4>(s(), p(), s(), p()); }
	vector<T, 4> xzxw() const { return vector<T, 4>(x(), z(), x(), w()); }
	vector<T, 4> rbra() const { return vector<T, 4>(r(), b(), r(), a()); }
	vector<T, 4> spsq() const { return vector<T, 4>(s(), p(), s(), q()); }
	vector<T, 4> xzyx() const { return vector<T, 4>(x(), z(), y(), x()); }
	vector<T, 4> rbgr() const { return vector<T, 4>(r(), b(), g(), r()); }
	vector<T, 4> spts() const { return vector<T, 4>(s(), p(), t(), s()); }
	vector<T, 4> xzyy() const { return vector<T, 4>(x(), z(), y(), y()); }
	vector<T, 4> rbgg() const { return vector<T, 4>(r(), b(), g(), g()); }
	vector<T, 4> sptt() const { return vector<T, 4>(s(), p(), t(), t()); }
	vector<T, 4> xzyz() const { return vector<T, 4>(x(), z(), y(), z()); }
	vector<T, 4> rbgb() const { return vector<T, 4>(r(), b(), g(), b()); }
	vector<T, 4> sptp() const { return vector<T, 4>(s(), p(), t(), p()); }
	vector<T, 4> xzyw() const { return vector<T, 4>(x(), z(), y(), w()); }
	vector<T, 4> rbga() const { return vector<T, 4>(r(), b(), g(), a()); }
	vector<T, 4> sptq() const { return vector<T, 4>(s(), p(), t(), q()); }
	vector<T, 4> xzzx() const { return vector<T, 4>(x(), z(), z(), x()); }
	vector<T, 4> rbbr() const { return vector<T, 4>(r(), b(), b(), r()); }
	vector<T, 4> spps() const { return vector<T, 4>(s(), p(), p(), s()); }
	vector<T, 4> xzzy() const { return vector<T, 4>(x(), z(), z(), y()); }
	vector<T, 4> rbbg() const { return vector<T, 4>(r(), b(), b(), g()); }
	vector<T, 4> sppt() const { return vector<T, 4>(s(), p(), p(), t()); }
	vector<T, 4> xzzz() const { return vector<T, 4>(x(), z(), z(), z()); }
	vector<T, 4> rbbb() const { return vector<T, 4>(r(), b(), b(), b()); }
	vector<T, 4> sppp() const { return vector<T, 4>(s(), p(), p(), p()); }
	vector<T, 4> xzzw() const { return vector<T, 4>(x(), z(), z(), w()); }
	vector<T, 4> rbba() const { return vector<T, 4>(r(), b(), b(), a()); }
	vector<T, 4> sppq() const { return vector<T, 4>(s(), p(), p(), q()); }
	vector<T, 4> xzwx() const { return vector<T, 4>(x(), z(), w(), x()); }
	vector<T, 4> rbar() const { return vector<T, 4>(r(), b(), a(), r()); }
	vector<T, 4> spqs() const { return vector<T, 4>(s(), p(), q(), s()); }
	vector<T, 4> xzwy() const { return vector<T, 4>(x(), z(), w(), y()); }
	vector<T, 4> rbag() const { return vector<T, 4>(r(), b(), a(), g()); }
	vector<T, 4> spqt() const { return vector<T, 4>(s(), p(), q(), t()); }
	vector<T, 4> xzwz() const { return vector<T, 4>(x(), z(), w(), z()); }
	vector<T, 4> rbab() const { return vector<T, 4>(r(), b(), a(), b()); }
	vector<T, 4> spqp() const { return vector<T, 4>(s(), p(), q(), p()); }
	vector<T, 4> xzww() const { return vector<T, 4>(x(), z(), w(), w()); }
	vector<T, 4> rbaa() const { return vector<T, 4>(r(), b(), a(), a()); }
	vector<T, 4> spqq() const { return vector<T, 4>(s(), p(), q(), q()); }
	vector<T, 4> xwxx() const { return vector<T, 4>(x(), w(), x(), x()); }
	vector<T, 4> rarr() const { return vector<T, 4>(r(), a(), r(), r()); }
	vector<T, 4> sqss() const { return vector<T, 4>(s(), q(), s(), s()); }
	vector<T, 4> xwxy() const { return vector<T, 4>(x(), w(), x(), y()); }
	vector<T, 4> rarg() const { return vector<T, 4>(r(), a(), r(), g()); }
	vector<T, 4> sqst() const { return vector<T, 4>(s(), q(), s(), t()); }
	vector<T, 4> xwxz() const { return vector<T, 4>(x(), w(), x(), z()); }
	vector<T, 4> rarb() const { return vector<T, 4>(r(), a(), r(), b()); }
	vector<T, 4> sqsp() const { return vector<T, 4>(s(), q(), s(), p()); }
	vector<T, 4> xwxw() const { return vector<T, 4>(x(), w(), x(), w()); }
	vector<T, 4> rara() const { return vector<T, 4>(r(), a(), r(), a()); }
	vector<T, 4> sqsq() const { return vector<T, 4>(s(), q(), s(), q()); }
	vector<T, 4> xwyx() const { return vector<T, 4>(x(), w(), y(), x()); }
	vector<T, 4> ragr() const { return vector<T, 4>(r(), a(), g(), r()); }
	vector<T, 4> sqts() const { return vector<T, 4>(s(), q(), t(), s()); }
	vector<T, 4> xwyy() const { return vector<T, 4>(x(), w(), y(), y()); }
	vector<T, 4> ragg() const { return vector<T, 4>(r(), a(), g(), g()); }
	vector<T, 4> sqtt() const { return vector<T, 4>(s(), q(), t(), t()); }
	vector<T, 4> xwyz() const { return vector<T, 4>(x(), w(), y(), z()); }
	vector<T, 4> ragb() const { return vector<T, 4>(r(), a(), g(), b()); }
	vector<T, 4> sqtp() const { return vector<T, 4>(s(), q(), t(), p()); }
	vector<T, 4> xwyw() const { return vector<T, 4>(x(), w(), y(), w()); }
	vector<T, 4> raga() const { return vector<T, 4>(r(), a(), g(), a()); }
	vector<T, 4> sqtq() const { return vector<T, 4>(s(), q(), t(), q()); }
	vector<T, 4> xwzx() const { return vector<T, 4>(x(), w(), z(), x()); }
	vector<T, 4> rabr() const { return vector<T, 4>(r(), a(), b(), r()); }
	vector<T, 4> sqps() const { return vector<T, 4>(s(), q(), p(), s()); }
	vector<T, 4> xwzy() const { return vector<T, 4>(x(), w(), z(), y()); }
	vector<T, 4> rabg() const { return vector<T, 4>(r(), a(), b(), g()); }
	vector<T, 4> sqpt() const { return vector<T, 4>(s(), q(), p(), t()); }
	vector<T, 4> xwzz() const { return vector<T, 4>(x(), w(), z(), z()); }
	vector<T, 4> rabb() const { return vector<T, 4>(r(), a(), b(), b()); }
	vector<T, 4> sqpp() const { return vector<T, 4>(s(), q(), p(), p()); }
	vector<T, 4> xwzw() const { return vector<T, 4>(x(), w(), z(), w()); }
	vector<T, 4> raba() const { return vector<T, 4>(r(), a(), b(), a()); }
	vector<T, 4> sqpq() const { return vector<T, 4>(s(), q(), p(), q()); }
	vector<T, 4> xwwx() const { return vector<T, 4>(x(), w(), w(), x()); }
	vector<T, 4> raar() const { return vector<T, 4>(r(), a(), a(), r()); }
	vector<T, 4> sqqs() const { return vector<T, 4>(s(), q(), q(), s()); }
	vector<T, 4> xwwy() const { return vector<T, 4>(x(), w(), w(), y()); }
	vector<T, 4> raag() const { return vector<T, 4>(r(), a(), a(), g()); }
	vector<T, 4> sqqt() const { return vector<T, 4>(s(), q(), q(), t()); }
	vector<T, 4> xwwz() const { return vector<T, 4>(x(), w(), w(), z()); }
	vector<T, 4> raab() const { return vector<T, 4>(r(), a(), a(), b()); }
	vector<T, 4> sqqp() const { return vector<T, 4>(s(), q(), q(), p()); }
	vector<T, 4> xwww() const { return vector<T, 4>(x(), w(), w(), w()); }
	vector<T, 4> raaa() const { return vector<T, 4>(r(), a(), a(), a()); }
	vector<T, 4> sqqq() const { return vector<T, 4>(s(), q(), q(), q()); }
	vector<T, 4> yxxx() const { return vector<T, 4>(y(), x(), x(), x()); }
	vector<T, 4> grrr() const { return vector<T, 4>(g(), r(), r(), r()); }
	vector<T, 4> tsss() const { return vector<T, 4>(t(), s(), s(), s()); }
	vector<T, 4> yxxy() const { return vector<T, 4>(y(), x(), x(), y()); }
	vector<T, 4> grrg() const { return vector<T, 4>(g(), r(), r(), g()); }
	vector<T, 4> tsst() const { return vector<T, 4>(t(), s(), s(), t()); }
	vector<T, 4> yxxz() const { return vector<T, 4>(y(), x(), x(), z()); }
	vector<T, 4> grrb() const { return vector<T, 4>(g(), r(), r(), b()); }
	vector<T, 4> tssp() const { return vector<T, 4>(t(), s(), s(), p()); }
	vector<T, 4> yxxw() const { return vector<T, 4>(y(), x(), x(), w()); }
	vector<T, 4> grra() const { return vector<T, 4>(g(), r(), r(), a()); }
	vector<T, 4> tssq() const { return vector<T, 4>(t(), s(), s(), q()); }
	vector<T, 4> yxyx() const { return vector<T, 4>(y(), x(), y(), x()); }
	vector<T, 4> grgr() const { return vector<T, 4>(g(), r(), g(), r()); }
	vector<T, 4> tsts() const { return vector<T, 4>(t(), s(), t(), s()); }
	vector<T, 4> yxyy() const { return vector<T, 4>(y(), x(), y(), y()); }
	vector<T, 4> grgg() const { return vector<T, 4>(g(), r(), g(), g()); }
	vector<T, 4> tstt() const { return vector<T, 4>(t(), s(), t(), t()); }
	vector<T, 4> yxyz() const { return vector<T, 4>(y(), x(), y(), z()); }
	vector<T, 4> grgb() const { return vector<T, 4>(g(), r(), g(), b()); }
	vector<T, 4> tstp() const { return vector<T, 4>(t(), s(), t(), p()); }
	vector<T, 4> yxyw() const { return vector<T, 4>(y(), x(), y(), w()); }
	vector<T, 4> grga() const { return vector<T, 4>(g(), r(), g(), a()); }
	vector<T, 4> tstq() const { return vector<T, 4>(t(), s(), t(), q()); }
	vector<T, 4> yxzx() const { return vector<T, 4>(y(), x(), z(), x()); }
	vector<T, 4> grbr() const { return vector<T, 4>(g(), r(), b(), r()); }
	vector<T, 4> tsps() const { return vector<T, 4>(t(), s(), p(), s()); }
	vector<T, 4> yxzy() const { return vector<T, 4>(y(), x(), z(), y()); }
	vector<T, 4> grbg() const { return vector<T, 4>(g(), r(), b(), g()); }
	vector<T, 4> tspt() const { return vector<T, 4>(t(), s(), p(), t()); }
	vector<T, 4> yxzz() const { return vector<T, 4>(y(), x(), z(), z()); }
	vector<T, 4> grbb() const { return vector<T, 4>(g(), r(), b(), b()); }
	vector<T, 4> tspp() const { return vector<T, 4>(t(), s(), p(), p()); }
	vector<T, 4> yxzw() const { return vector<T, 4>(y(), x(), z(), w()); }
	vector<T, 4> grba() const { return vector<T, 4>(g(), r(), b(), a()); }
	vector<T, 4> tspq() const { return vector<T, 4>(t(), s(), p(), q()); }
	vector<T, 4> yxwx() const { return vector<T, 4>(y(), x(), w(), x()); }
	vector<T, 4> grar() const { return vector<T, 4>(g(), r(), a(), r()); }
	vector<T, 4> tsqs() const { return vector<T, 4>(t(), s(), q(), s()); }
	vector<T, 4> yxwy() const { return vector<T, 4>(y(), x(), w(), y()); }
	vector<T, 4> grag() const { return vector<T, 4>(g(), r(), a(), g()); }
	vector<T, 4> tsqt() const { return vector<T, 4>(t(), s(), q(), t()); }
	vector<T, 4> yxwz() const { return vector<T, 4>(y(), x(), w(), z()); }
	vector<T, 4> grab() const { return vector<T, 4>(g(), r(), a(), b()); }
	vector<T, 4> tsqp() const { return vector<T, 4>(t(), s(), q(), p()); }
	vector<T, 4> yxww() const { return vector<T, 4>(y(), x(), w(), w()); }
	vector<T, 4> graa() const { return vector<T, 4>(g(), r(), a(), a()); }
	vector<T, 4> tsqq() const { return vector<T, 4>(t(), s(), q(), q()); }
	vector<T, 4> yyxx() const { return vector<T, 4>(y(), y(), x(), x()); }
	vector<T, 4> ggrr() const { return vector<T, 4>(g(), g(), r(), r()); }
	vector<T, 4> ttss() const { return vector<T, 4>(t(), t(), s(), s()); }
	vector<T, 4> yyxy() const { return vector<T, 4>(y(), y(), x(), y()); }
	vector<T, 4> ggrg() const { return vector<T, 4>(g(), g(), r(), g()); }
	vector<T, 4> ttst() const { return vector<T, 4>(t(), t(), s(), t()); }
	vector<T, 4> yyxz() const { return vector<T, 4>(y(), y(), x(), z()); }
	vector<T, 4> ggrb() const { return vector<T, 4>(g(), g(), r(), b()); }
	vector<T, 4> ttsp() const { return vector<T, 4>(t(), t(), s(), p()); }
	vector<T, 4> yyxw() const { return vector<T, 4>(y(), y(), x(), w()); }
	vector<T, 4> ggra() const { return vector<T, 4>(g(), g(), r(), a()); }
	vector<T, 4> ttsq() const { return vector<T, 4>(t(), t(), s(), q()); }
	vector<T, 4> yyyx() const { return vector<T, 4>(y(), y(), y(), x()); }
	vector<T, 4> gggr() const { return vector<T, 4>(g(), g(), g(), r()); }
	vector<T, 4> ttts() const { return vector<T, 4>(t(), t(), t(), s()); }
	vector<T, 4> yyyy() const { return vector<T, 4>(y(), y(), y(), y()); }
	vector<T, 4> gggg() const { return vector<T, 4>(g(), g(), g(), g()); }
	vector<T, 4> tttt() const { return vector<T, 4>(t(), t(), t(), t()); }
	vector<T, 4> yyyz() const { return vector<T, 4>(y(), y(), y(), z()); }
	vector<T, 4> gggb() const { return vector<T, 4>(g(), g(), g(), b()); }
	vector<T, 4> tttp() const { return vector<T, 4>(t(), t(), t(), p()); }
	vector<T, 4> yyyw() const { return vector<T, 4>(y(), y(), y(), w()); }
	vector<T, 4> ggga() const { return vector<T, 4>(g(), g(), g(), a()); }
	vector<T, 4> tttq() const { return vector<T, 4>(t(), t(), t(), q()); }
	vector<T, 4> yyzx() const { return vector<T, 4>(y(), y(), z(), x()); }
	vector<T, 4> ggbr() const { return vector<T, 4>(g(), g(), b(), r()); }
	vector<T, 4> ttps() const { return vector<T, 4>(t(), t(), p(), s()); }
	vector<T, 4> yyzy() const { return vector<T, 4>(y(), y(), z(), y()); }
	vector<T, 4> ggbg() const { return vector<T, 4>(g(), g(), b(), g()); }
	vector<T, 4> ttpt() const { return vector<T, 4>(t(), t(), p(), t()); }
	vector<T, 4> yyzz() const { return vector<T, 4>(y(), y(), z(), z()); }
	vector<T, 4> ggbb() const { return vector<T, 4>(g(), g(), b(), b()); }
	vector<T, 4> ttpp() const { return vector<T, 4>(t(), t(), p(), p()); }
	vector<T, 4> yyzw() const { return vector<T, 4>(y(), y(), z(), w()); }
	vector<T, 4> ggba() const { return vector<T, 4>(g(), g(), b(), a()); }
	vector<T, 4> ttpq() const { return vector<T, 4>(t(), t(), p(), q()); }
	vector<T, 4> yywx() const { return vector<T, 4>(y(), y(), w(), x()); }
	vector<T, 4> ggar() const { return vector<T, 4>(g(), g(), a(), r()); }
	vector<T, 4> ttqs() const { return vector<T, 4>(t(), t(), q(), s()); }
	vector<T, 4> yywy() const { return vector<T, 4>(y(), y(), w(), y()); }
	vector<T, 4> ggag() const { return vector<T, 4>(g(), g(), a(), g()); }
	vector<T, 4> ttqt() const { return vector<T, 4>(t(), t(), q(), t()); }
	vector<T, 4> yywz() const { return vector<T, 4>(y(), y(), w(), z()); }
	vector<T, 4> ggab() const { return vector<T, 4>(g(), g(), a(), b()); }
	vector<T, 4> ttqp() const { return vector<T, 4>(t(), t(), q(), p()); }
	vector<T, 4> yyww() const { return vector<T, 4>(y(), y(), w(), w()); }
	vector<T, 4> ggaa() const { return vector<T, 4>(g(), g(), a(), a()); }
	vector<T, 4> ttqq() const { return vector<T, 4>(t(), t(), q(), q()); }
	vector<T, 4> yzxx() const { return vector<T, 4>(y(), z(), x(), x()); }
	vector<T, 4> gbrr() const { return vector<T, 4>(g(), b(), r(), r()); }
	vector<T, 4> tpss() const { return vector<T, 4>(t(), p(), s(), s()); }
	vector<T, 4> yzxy() const { return vector<T, 4>(y(), z(), x(), y()); }
	vector<T, 4> gbrg() const { return vector<T, 4>(g(), b(), r(), g()); }
	vector<T, 4> tpst() const { return vector<T, 4>(t(), p(), s(), t()); }
	vector<T, 4> yzxz() const { return vector<T, 4>(y(), z(), x(), z()); }
	vector<T, 4> gbrb() const { return vector<T, 4>(g(), b(), r(), b()); }
	vector<T, 4> tpsp() const { return vector<T, 4>(t(), p(), s(), p()); }
	vector<T, 4> yzxw() const { return vector<T, 4>(y(), z(), x(), w()); }
	vector<T, 4> gbra() const { return vector<T, 4>(g(), b(), r(), a()); }
	vector<T, 4> tpsq() const { return vector<T, 4>(t(), p(), s(), q()); }
	vector<T, 4> yzyx() const { return vector<T, 4>(y(), z(), y(), x()); }
	vector<T, 4> gbgr() const { return vector<T, 4>(g(), b(), g(), r()); }
	vector<T, 4> tpts() const { return vector<T, 4>(t(), p(), t(), s()); }
	vector<T, 4> yzyy() const { return vector<T, 4>(y(), z(), y(), y()); }
	vector<T, 4> gbgg() const { return vector<T, 4>(g(), b(), g(), g()); }
	vector<T, 4> tptt() const { return vector<T, 4>(t(), p(), t(), t()); }
	vector<T, 4> yzyz() const { return vector<T, 4>(y(), z(), y(), z()); }
	vector<T, 4> gbgb() const { return vector<T, 4>(g(), b(), g(), b()); }
	vector<T, 4> tptp() const { return vector<T, 4>(t(), p(), t(), p()); }
	vector<T, 4> yzyw() const { return vector<T, 4>(y(), z(), y(), w()); }
	vector<T, 4> gbga() const { return vector<T, 4>(g(), b(), g(), a()); }
	vector<T, 4> tptq() const { return vector<T, 4>(t(), p(), t(), q()); }
	vector<T, 4> yzzx() const { return vector<T, 4>(y(), z(), z(), x()); }
	vector<T, 4> gbbr() const { return vector<T, 4>(g(), b(), b(), r()); }
	vector<T, 4> tpps() const { return vector<T, 4>(t(), p(), p(), s()); }
	vector<T, 4> yzzy() const { return vector<T, 4>(y(), z(), z(), y()); }
	vector<T, 4> gbbg() const { return vector<T, 4>(g(), b(), b(), g()); }
	vector<T, 4> tppt() const { return vector<T, 4>(t(), p(), p(), t()); }
	vector<T, 4> yzzz() const { return vector<T, 4>(y(), z(), z(), z()); }
	vector<T, 4> gbbb() const { return vector<T, 4>(g(), b(), b(), b()); }
	vector<T, 4> tppp() const { return vector<T, 4>(t(), p(), p(), p()); }
	vector<T, 4> yzzw() const { return vector<T, 4>(y(), z(), z(), w()); }
	vector<T, 4> gbba() const { return vector<T, 4>(g(), b(), b(), a()); }
	vector<T, 4> tppq() const { return vector<T, 4>(t(), p(), p(), q()); }
	vector<T, 4> yzwx() const { return vector<T, 4>(y(), z(), w(), x()); }
	vector<T, 4> gbar() const { return vector<T, 4>(g(), b(), a(), r()); }
	vector<T, 4> tpqs() const { return vector<T, 4>(t(), p(), q(), s()); }
	vector<T, 4> yzwy() const { return vector<T, 4>(y(), z(), w(), y()); }
	vector<T, 4> gbag() const { return vector<T, 4>(g(), b(), a(), g()); }
	vector<T, 4> tpqt() const { return vector<T, 4>(t(), p(), q(), t()); }
	vector<T, 4> yzwz() const { return vector<T, 4>(y(), z(), w(), z()); }
	vector<T, 4> gbab() const { return vector<T, 4>(g(), b(), a(), b()); }
	vector<T, 4> tpqp() const { return vector<T, 4>(t(), p(), q(), p()); }
	vector<T, 4> yzww() const { return vector<T, 4>(y(), z(), w(), w()); }
	vector<T, 4> gbaa() const { return vector<T, 4>(g(), b(), a(), a()); }
	vector<T, 4> tpqq() const { return vector<T, 4>(t(), p(), q(), q()); }
	vector<T, 4> ywxx() const { return vector<T, 4>(y(), w(), x(), x()); }
	vector<T, 4> garr() const { return vector<T, 4>(g(), a(), r(), r()); }
	vector<T, 4> tqss() const { return vector<T, 4>(t(), q(), s(), s()); }
	vector<T, 4> ywxy() const { return vector<T, 4>(y(), w(), x(), y()); }
	vector<T, 4> garg() const { return vector<T, 4>(g(), a(), r(), g()); }
	vector<T, 4> tqst() const { return vector<T, 4>(t(), q(), s(), t()); }
	vector<T, 4> ywxz() const { return vector<T, 4>(y(), w(), x(), z()); }
	vector<T, 4> garb() const { return vector<T, 4>(g(), a(), r(), b()); }
	vector<T, 4> tqsp() const { return vector<T, 4>(t(), q(), s(), p()); }
	vector<T, 4> ywxw() const { return vector<T, 4>(y(), w(), x(), w()); }
	vector<T, 4> gara() const { return vector<T, 4>(g(), a(), r(), a()); }
	vector<T, 4> tqsq() const { return vector<T, 4>(t(), q(), s(), q()); }
	vector<T, 4> ywyx() const { return vector<T, 4>(y(), w(), y(), x()); }
	vector<T, 4> gagr() const { return vector<T, 4>(g(), a(), g(), r()); }
	vector<T, 4> tqts() const { return vector<T, 4>(t(), q(), t(), s()); }
	vector<T, 4> ywyy() const { return vector<T, 4>(y(), w(), y(), y()); }
	vector<T, 4> gagg() const { return vector<T, 4>(g(), a(), g(), g()); }
	vector<T, 4> tqtt() const { return vector<T, 4>(t(), q(), t(), t()); }
	vector<T, 4> ywyz() const { return vector<T, 4>(y(), w(), y(), z()); }
	vector<T, 4> gagb() const { return vector<T, 4>(g(), a(), g(), b()); }
	vector<T, 4> tqtp() const { return vector<T, 4>(t(), q(), t(), p()); }
	vector<T, 4> ywyw() const { return vector<T, 4>(y(), w(), y(), w()); }
	vector<T, 4> gaga() const { return vector<T, 4>(g(), a(), g(), a()); }
	vector<T, 4> tqtq() const { return vector<T, 4>(t(), q(), t(), q()); }
	vector<T, 4> ywzx() const { return vector<T, 4>(y(), w(), z(), x()); }
	vector<T, 4> gabr() const { return vector<T, 4>(g(), a(), b(), r()); }
	vector<T, 4> tqps() const { return vector<T, 4>(t(), q(), p(), s()); }
	vector<T, 4> ywzy() const { return vector<T, 4>(y(), w(), z(), y()); }
	vector<T, 4> gabg() const { return vector<T, 4>(g(), a(), b(), g()); }
	vector<T, 4> tqpt() const { return vector<T, 4>(t(), q(), p(), t()); }
	vector<T, 4> ywzz() const { return vector<T, 4>(y(), w(), z(), z()); }
	vector<T, 4> gabb() const { return vector<T, 4>(g(), a(), b(), b()); }
	vector<T, 4> tqpp() const { return vector<T, 4>(t(), q(), p(), p()); }
	vector<T, 4> ywzw() const { return vector<T, 4>(y(), w(), z(), w()); }
	vector<T, 4> gaba() const { return vector<T, 4>(g(), a(), b(), a()); }
	vector<T, 4> tqpq() const { return vector<T, 4>(t(), q(), p(), q()); }
	vector<T, 4> ywwx() const { return vector<T, 4>(y(), w(), w(), x()); }
	vector<T, 4> gaar() const { return vector<T, 4>(g(), a(), a(), r()); }
	vector<T, 4> tqqs() const { return vector<T, 4>(t(), q(), q(), s()); }
	vector<T, 4> ywwy() const { return vector<T, 4>(y(), w(), w(), y()); }
	vector<T, 4> gaag() const { return vector<T, 4>(g(), a(), a(), g()); }
	vector<T, 4> tqqt() const { return vector<T, 4>(t(), q(), q(), t()); }
	vector<T, 4> ywwz() const { return vector<T, 4>(y(), w(), w(), z()); }
	vector<T, 4> gaab() const { return vector<T, 4>(g(), a(), a(), b()); }
	vector<T, 4> tqqp() const { return vector<T, 4>(t(), q(), q(), p()); }
	vector<T, 4> ywww() const { return vector<T, 4>(y(), w(), w(), w()); }
	vector<T, 4> gaaa() const { return vector<T, 4>(g(), a(), a(), a()); }
	vector<T, 4> tqqq() const { return vector<T, 4>(t(), q(), q(), q()); }
	vector<T, 4> zxxx() const { return vector<T, 4>(z(), x(), x(), x()); }
	vector<T, 4> brrr() const { return vector<T, 4>(b(), r(), r(), r()); }
	vector<T, 4> psss() const { return vector<T, 4>(p(), s(), s(), s()); }
	vector<T, 4> zxxy() const { return vector<T, 4>(z(), x(), x(), y()); }
	vector<T, 4> brrg() const { return vector<T, 4>(b(), r(), r(), g()); }
	vector<T, 4> psst() const { return vector<T, 4>(p(), s(), s(), t()); }
	vector<T, 4> zxxz() const { return vector<T, 4>(z(), x(), x(), z()); }
	vector<T, 4> brrb() const { return vector<T, 4>(b(), r(), r(), b()); }
	vector<T, 4> pssp() const { return vector<T, 4>(p(), s(), s(), p()); }
	vector<T, 4> zxxw() const { return vector<T, 4>(z(), x(), x(), w()); }
	vector<T, 4> brra() const { return vector<T, 4>(b(), r(), r(), a()); }
	vector<T, 4> pssq() const { return vector<T, 4>(p(), s(), s(), q()); }
	vector<T, 4> zxyx() const { return vector<T, 4>(z(), x(), y(), x()); }
	vector<T, 4> brgr() const { return vector<T, 4>(b(), r(), g(), r()); }
	vector<T, 4> psts() const { return vector<T, 4>(p(), s(), t(), s()); }
	vector<T, 4> zxyy() const { return vector<T, 4>(z(), x(), y(), y()); }
	vector<T, 4> brgg() const { return vector<T, 4>(b(), r(), g(), g()); }
	vector<T, 4> pstt() const { return vector<T, 4>(p(), s(), t(), t()); }
	vector<T, 4> zxyz() const { return vector<T, 4>(z(), x(), y(), z()); }
	vector<T, 4> brgb() const { return vector<T, 4>(b(), r(), g(), b()); }
	vector<T, 4> pstp() const { return vector<T, 4>(p(), s(), t(), p()); }
	vector<T, 4> zxyw() const { return vector<T, 4>(z(), x(), y(), w()); }
	vector<T, 4> brga() const { return vector<T, 4>(b(), r(), g(), a()); }
	vector<T, 4> pstq() const { return vector<T, 4>(p(), s(), t(), q()); }
	vector<T, 4> zxzx() const { return vector<T, 4>(z(), x(), z(), x()); }
	vector<T, 4> brbr() const { return vector<T, 4>(b(), r(), b(), r()); }
	vector<T, 4> psps() const { return vector<T, 4>(p(), s(), p(), s()); }
	vector<T, 4> zxzy() const { return vector<T, 4>(z(), x(), z(), y()); }
	vector<T, 4> brbg() const { return vector<T, 4>(b(), r(), b(), g()); }
	vector<T, 4> pspt() const { return vector<T, 4>(p(), s(), p(), t()); }
	vector<T, 4> zxzz() const { return vector<T, 4>(z(), x(), z(), z()); }
	vector<T, 4> brbb() const { return vector<T, 4>(b(), r(), b(), b()); }
	vector<T, 4> pspp() const { return vector<T, 4>(p(), s(), p(), p()); }
	vector<T, 4> zxzw() const { return vector<T, 4>(z(), x(), z(), w()); }
	vector<T, 4> brba() const { return vector<T, 4>(b(), r(), b(), a()); }
	vector<T, 4> pspq() const { return vector<T, 4>(p(), s(), p(), q()); }
	vector<T, 4> zxwx() const { return vector<T, 4>(z(), x(), w(), x()); }
	vector<T, 4> brar() const { return vector<T, 4>(b(), r(), a(), r()); }
	vector<T, 4> psqs() const { return vector<T, 4>(p(), s(), q(), s()); }
	vector<T, 4> zxwy() const { return vector<T, 4>(z(), x(), w(), y()); }
	vector<T, 4> brag() const { return vector<T, 4>(b(), r(), a(), g()); }
	vector<T, 4> psqt() const { return vector<T, 4>(p(), s(), q(), t()); }
	vector<T, 4> zxwz() const { return vector<T, 4>(z(), x(), w(), z()); }
	vector<T, 4> brab() const { return vector<T, 4>(b(), r(), a(), b()); }
	vector<T, 4> psqp() const { return vector<T, 4>(p(), s(), q(), p()); }
	vector<T, 4> zxww() const { return vector<T, 4>(z(), x(), w(), w()); }
	vector<T, 4> braa() const { return vector<T, 4>(b(), r(), a(), a()); }
	vector<T, 4> psqq() const { return vector<T, 4>(p(), s(), q(), q()); }
	vector<T, 4> zyxx() const { return vector<T, 4>(z(), y(), x(), x()); }
	vector<T, 4> bgrr() const { return vector<T, 4>(b(), g(), r(), r()); }
	vector<T, 4> ptss() const { return vector<T, 4>(p(), t(), s(), s()); }
	vector<T, 4> zyxy() const { return vector<T, 4>(z(), y(), x(), y()); }
	vector<T, 4> bgrg() const { return vector<T, 4>(b(), g(), r(), g()); }
	vector<T, 4> ptst() const { return vector<T, 4>(p(), t(), s(), t()); }
	vector<T, 4> zyxz() const { return vector<T, 4>(z(), y(), x(), z()); }
	vector<T, 4> bgrb() const { return vector<T, 4>(b(), g(), r(), b()); }
	vector<T, 4> ptsp() const { return vector<T, 4>(p(), t(), s(), p()); }
	vector<T, 4> zyxw() const { return vector<T, 4>(z(), y(), x(), w()); }
	vector<T, 4> bgra() const { return vector<T, 4>(b(), g(), r(), a()); }
	vector<T, 4> ptsq() const { return vector<T, 4>(p(), t(), s(), q()); }
	vector<T, 4> zyyx() const { return vector<T, 4>(z(), y(), y(), x()); }
	vector<T, 4> bggr() const { return vector<T, 4>(b(), g(), g(), r()); }
	vector<T, 4> ptts() const { return vector<T, 4>(p(), t(), t(), s()); }
	vector<T, 4> zyyy() const { return vector<T, 4>(z(), y(), y(), y()); }
	vector<T, 4> bggg() const { return vector<T, 4>(b(), g(), g(), g()); }
	vector<T, 4> pttt() const { return vector<T, 4>(p(), t(), t(), t()); }
	vector<T, 4> zyyz() const { return vector<T, 4>(z(), y(), y(), z()); }
	vector<T, 4> bggb() const { return vector<T, 4>(b(), g(), g(), b()); }
	vector<T, 4> pttp() const { return vector<T, 4>(p(), t(), t(), p()); }
	vector<T, 4> zyyw() const { return vector<T, 4>(z(), y(), y(), w()); }
	vector<T, 4> bgga() const { return vector<T, 4>(b(), g(), g(), a()); }
	vector<T, 4> pttq() const { return vector<T, 4>(p(), t(), t(), q()); }
	vector<T, 4> zyzx() const { return vector<T, 4>(z(), y(), z(), x()); }
	vector<T, 4> bgbr() const { return vector<T, 4>(b(), g(), b(), r()); }
	vector<T, 4> ptps() const { return vector<T, 4>(p(), t(), p(), s()); }
	vector<T, 4> zyzy() const { return vector<T, 4>(z(), y(), z(), y()); }
	vector<T, 4> bgbg() const { return vector<T, 4>(b(), g(), b(), g()); }
	vector<T, 4> ptpt() const { return vector<T, 4>(p(), t(), p(), t()); }
	vector<T, 4> zyzz() const { return vector<T, 4>(z(), y(), z(), z()); }
	vector<T, 4> bgbb() const { return vector<T, 4>(b(), g(), b(), b()); }
	vector<T, 4> ptpp() const { return vector<T, 4>(p(), t(), p(), p()); }
	vector<T, 4> zyzw() const { return vector<T, 4>(z(), y(), z(), w()); }
	vector<T, 4> bgba() const { return vector<T, 4>(b(), g(), b(), a()); }
	vector<T, 4> ptpq() const { return vector<T, 4>(p(), t(), p(), q()); }
	vector<T, 4> zywx() const { return vector<T, 4>(z(), y(), w(), x()); }
	vector<T, 4> bgar() const { return vector<T, 4>(b(), g(), a(), r()); }
	vector<T, 4> ptqs() const { return vector<T, 4>(p(), t(), q(), s()); }
	vector<T, 4> zywy() const { return vector<T, 4>(z(), y(), w(), y()); }
	vector<T, 4> bgag() const { return vector<T, 4>(b(), g(), a(), g()); }
	vector<T, 4> ptqt() const { return vector<T, 4>(p(), t(), q(), t()); }
	vector<T, 4> zywz() const { return vector<T, 4>(z(), y(), w(), z()); }
	vector<T, 4> bgab() const { return vector<T, 4>(b(), g(), a(), b()); }
	vector<T, 4> ptqp() const { return vector<T, 4>(p(), t(), q(), p()); }
	vector<T, 4> zyww() const { return vector<T, 4>(z(), y(), w(), w()); }
	vector<T, 4> bgaa() const { return vector<T, 4>(b(), g(), a(), a()); }
	vector<T, 4> ptqq() const { return vector<T, 4>(p(), t(), q(), q()); }
	vector<T, 4> zzxx() const { return vector<T, 4>(z(), z(), x(), x()); }
	vector<T, 4> bbrr() const { return vector<T, 4>(b(), b(), r(), r()); }
	vector<T, 4> ppss() const { return vector<T, 4>(p(), p(), s(), s()); }
	vector<T, 4> zzxy() const { return vector<T, 4>(z(), z(), x(), y()); }
	vector<T, 4> bbrg() const { return vector<T, 4>(b(), b(), r(), g()); }
	vector<T, 4> ppst() const { return vector<T, 4>(p(), p(), s(), t()); }
	vector<T, 4> zzxz() const { return vector<T, 4>(z(), z(), x(), z()); }
	vector<T, 4> bbrb() const { return vector<T, 4>(b(), b(), r(), b()); }
	vector<T, 4> ppsp() const { return vector<T, 4>(p(), p(), s(), p()); }
	vector<T, 4> zzxw() const { return vector<T, 4>(z(), z(), x(), w()); }
	vector<T, 4> bbra() const { return vector<T, 4>(b(), b(), r(), a()); }
	vector<T, 4> ppsq() const { return vector<T, 4>(p(), p(), s(), q()); }
	vector<T, 4> zzyx() const { return vector<T, 4>(z(), z(), y(), x()); }
	vector<T, 4> bbgr() const { return vector<T, 4>(b(), b(), g(), r()); }
	vector<T, 4> ppts() const { return vector<T, 4>(p(), p(), t(), s()); }
	vector<T, 4> zzyy() const { return vector<T, 4>(z(), z(), y(), y()); }
	vector<T, 4> bbgg() const { return vector<T, 4>(b(), b(), g(), g()); }
	vector<T, 4> pptt() const { return vector<T, 4>(p(), p(), t(), t()); }
	vector<T, 4> zzyz() const { return vector<T, 4>(z(), z(), y(), z()); }
	vector<T, 4> bbgb() const { return vector<T, 4>(b(), b(), g(), b()); }
	vector<T, 4> pptp() const { return vector<T, 4>(p(), p(), t(), p()); }
	vector<T, 4> zzyw() const { return vector<T, 4>(z(), z(), y(), w()); }
	vector<T, 4> bbga() const { return vector<T, 4>(b(), b(), g(), a()); }
	vector<T, 4> pptq() const { return vector<T, 4>(p(), p(), t(), q()); }
	vector<T, 4> zzzx() const { return vector<T, 4>(z(), z(), z(), x()); }
	vector<T, 4> bbbr() const { return vector<T, 4>(b(), b(), b(), r()); }
	vector<T, 4> ppps() const { return vector<T, 4>(p(), p(), p(), s()); }
	vector<T, 4> zzzy() const { return vector<T, 4>(z(), z(), z(), y()); }
	vector<T, 4> bbbg() const { return vector<T, 4>(b(), b(), b(), g()); }
	vector<T, 4> pppt() const { return vector<T, 4>(p(), p(), p(), t()); }
	vector<T, 4> zzzz() const { return vector<T, 4>(z(), z(), z(), z()); }
	vector<T, 4> bbbb() const { return vector<T, 4>(b(), b(), b(), b()); }
	vector<T, 4> pppp() const { return vector<T, 4>(p(), p(), p(), p()); }
	vector<T, 4> zzzw() const { return vector<T, 4>(z(), z(), z(), w()); }
	vector<T, 4> bbba() const { return vector<T, 4>(b(), b(), b(), a()); }
	vector<T, 4> pppq() const { return vector<T, 4>(p(), p(), p(), q()); }
	vector<T, 4> zzwx() const { return vector<T, 4>(z(), z(), w(), x()); }
	vector<T, 4> bbar() const { return vector<T, 4>(b(), b(), a(), r()); }
	vector<T, 4> ppqs() const { return vector<T, 4>(p(), p(), q(), s()); }
	vector<T, 4> zzwy() const { return vector<T, 4>(z(), z(), w(), y()); }
	vector<T, 4> bbag() const { return vector<T, 4>(b(), b(), a(), g()); }
	vector<T, 4> ppqt() const { return vector<T, 4>(p(), p(), q(), t()); }
	vector<T, 4> zzwz() const { return vector<T, 4>(z(), z(), w(), z()); }
	vector<T, 4> bbab() const { return vector<T, 4>(b(), b(), a(), b()); }
	vector<T, 4> ppqp() const { return vector<T, 4>(p(), p(), q(), p()); }
	vector<T, 4> zzww() const { return vector<T, 4>(z(), z(), w(), w()); }
	vector<T, 4> bbaa() const { return vector<T, 4>(b(), b(), a(), a()); }
	vector<T, 4> ppqq() const { return vector<T, 4>(p(), p(), q(), q()); }
	vector<T, 4> zwxx() const { return vector<T, 4>(z(), w(), x(), x()); }
	vector<T, 4> barr() const { return vector<T, 4>(b(), a(), r(), r()); }
	vector<T, 4> pqss() const { return vector<T, 4>(p(), q(), s(), s()); }
	vector<T, 4> zwxy() const { return vector<T, 4>(z(), w(), x(), y()); }
	vector<T, 4> barg() const { return vector<T, 4>(b(), a(), r(), g()); }
	vector<T, 4> pqst() const { return vector<T, 4>(p(), q(), s(), t()); }
	vector<T, 4> zwxz() const { return vector<T, 4>(z(), w(), x(), z()); }
	vector<T, 4> barb() const { return vector<T, 4>(b(), a(), r(), b()); }
	vector<T, 4> pqsp() const { return vector<T, 4>(p(), q(), s(), p()); }
	vector<T, 4> zwxw() const { return vector<T, 4>(z(), w(), x(), w()); }
	vector<T, 4> bara() const { return vector<T, 4>(b(), a(), r(), a()); }
	vector<T, 4> pqsq() const { return vector<T, 4>(p(), q(), s(), q()); }
	vector<T, 4> zwyx() const { return vector<T, 4>(z(), w(), y(), x()); }
	vector<T, 4> bagr() const { return vector<T, 4>(b(), a(), g(), r()); }
	vector<T, 4> pqts() const { return vector<T, 4>(p(), q(), t(), s()); }
	vector<T, 4> zwyy() const { return vector<T, 4>(z(), w(), y(), y()); }
	vector<T, 4> bagg() const { return vector<T, 4>(b(), a(), g(), g()); }
	vector<T, 4> pqtt() const { return vector<T, 4>(p(), q(), t(), t()); }
	vector<T, 4> zwyz() const { return vector<T, 4>(z(), w(), y(), z()); }
	vector<T, 4> bagb() const { return vector<T, 4>(b(), a(), g(), b()); }
	vector<T, 4> pqtp() const { return vector<T, 4>(p(), q(), t(), p()); }
	vector<T, 4> zwyw() const { return vector<T, 4>(z(), w(), y(), w()); }
	vector<T, 4> baga() const { return vector<T, 4>(b(), a(), g(), a()); }
	vector<T, 4> pqtq() const { return vector<T, 4>(p(), q(), t(), q()); }
	vector<T, 4> zwzx() const { return vector<T, 4>(z(), w(), z(), x()); }
	vector<T, 4> babr() const { return vector<T, 4>(b(), a(), b(), r()); }
	vector<T, 4> pqps() const { return vector<T, 4>(p(), q(), p(), s()); }
	vector<T, 4> zwzy() const { return vector<T, 4>(z(), w(), z(), y()); }
	vector<T, 4> babg() const { return vector<T, 4>(b(), a(), b(), g()); }
	vector<T, 4> pqpt() const { return vector<T, 4>(p(), q(), p(), t()); }
	vector<T, 4> zwzz() const { return vector<T, 4>(z(), w(), z(), z()); }
	vector<T, 4> babb() const { return vector<T, 4>(b(), a(), b(), b()); }
	vector<T, 4> pqpp() const { return vector<T, 4>(p(), q(), p(), p()); }
	vector<T, 4> zwzw() const { return vector<T, 4>(z(), w(), z(), w()); }
	vector<T, 4> baba() const { return vector<T, 4>(b(), a(), b(), a()); }
	vector<T, 4> pqpq() const { return vector<T, 4>(p(), q(), p(), q()); }
	vector<T, 4> zwwx() const { return vector<T, 4>(z(), w(), w(), x()); }
	vector<T, 4> baar() const { return vector<T, 4>(b(), a(), a(), r()); }
	vector<T, 4> pqqs() const { return vector<T, 4>(p(), q(), q(), s()); }
	vector<T, 4> zwwy() const { return vector<T, 4>(z(), w(), w(), y()); }
	vector<T, 4> baag() const { return vector<T, 4>(b(), a(), a(), g()); }
	vector<T, 4> pqqt() const { return vector<T, 4>(p(), q(), q(), t()); }
	vector<T, 4> zwwz() const { return vector<T, 4>(z(), w(), w(), z()); }
	vector<T, 4> baab() const { return vector<T, 4>(b(), a(), a(), b()); }
	vector<T, 4> pqqp() const { return vector<T, 4>(p(), q(), q(), p()); }
	vector<T, 4> zwww() const { return vector<T, 4>(z(), w(), w(), w()); }
	vector<T, 4> baaa() const { return vector<T, 4>(b(), a(), a(), a()); }
	vector<T, 4> pqqq() const { return vector<T, 4>(p(), q(), q(), q()); }
	vector<T, 4> wxxx() const { return vector<T, 4>(w(), x(), x(), x()); }
	vector<T, 4> arrr() const { return vector<T, 4>(a(), r(), r(), r()); }
	vector<T, 4> qsss() const { return vector<T, 4>(q(), s(), s(), s()); }
	vector<T, 4> wxxy() const { return vector<T, 4>(w(), x(), x(), y()); }
	vector<T, 4> arrg() const { return vector<T, 4>(a(), r(), r(), g()); }
	vector<T, 4> qsst() const { return vector<T, 4>(q(), s(), s(), t()); }
	vector<T, 4> wxxz() const { return vector<T, 4>(w(), x(), x(), z()); }
	vector<T, 4> arrb() const { return vector<T, 4>(a(), r(), r(), b()); }
	vector<T, 4> qssp() const { return vector<T, 4>(q(), s(), s(), p()); }
	vector<T, 4> wxxw() const { return vector<T, 4>(w(), x(), x(), w()); }
	vector<T, 4> arra() const { return vector<T, 4>(a(), r(), r(), a()); }
	vector<T, 4> qssq() const { return vector<T, 4>(q(), s(), s(), q()); }
	vector<T, 4> wxyx() const { return vector<T, 4>(w(), x(), y(), x()); }
	vector<T, 4> argr() const { return vector<T, 4>(a(), r(), g(), r()); }
	vector<T, 4> qsts() const { return vector<T, 4>(q(), s(), t(), s()); }
	vector<T, 4> wxyy() const { return vector<T, 4>(w(), x(), y(), y()); }
	vector<T, 4> argg() const { return vector<T, 4>(a(), r(), g(), g()); }
	vector<T, 4> qstt() const { return vector<T, 4>(q(), s(), t(), t()); }
	vector<T, 4> wxyz() const { return vector<T, 4>(w(), x(), y(), z()); }
	vector<T, 4> argb() const { return vector<T, 4>(a(), r(), g(), b()); }
	vector<T, 4> qstp() const { return vector<T, 4>(q(), s(), t(), p()); }
	vector<T, 4> wxyw() const { return vector<T, 4>(w(), x(), y(), w()); }
	vector<T, 4> arga() const { return vector<T, 4>(a(), r(), g(), a()); }
	vector<T, 4> qstq() const { return vector<T, 4>(q(), s(), t(), q()); }
	vector<T, 4> wxzx() const { return vector<T, 4>(w(), x(), z(), x()); }
	vector<T, 4> arbr() const { return vector<T, 4>(a(), r(), b(), r()); }
	vector<T, 4> qsps() const { return vector<T, 4>(q(), s(), p(), s()); }
	vector<T, 4> wxzy() const { return vector<T, 4>(w(), x(), z(), y()); }
	vector<T, 4> arbg() const { return vector<T, 4>(a(), r(), b(), g()); }
	vector<T, 4> qspt() const { return vector<T, 4>(q(), s(), p(), t()); }
	vector<T, 4> wxzz() const { return vector<T, 4>(w(), x(), z(), z()); }
	vector<T, 4> arbb() const { return vector<T, 4>(a(), r(), b(), b()); }
	vector<T, 4> qspp() const { return vector<T, 4>(q(), s(), p(), p()); }
	vector<T, 4> wxzw() const { return vector<T, 4>(w(), x(), z(), w()); }
	vector<T, 4> arba() const { return vector<T, 4>(a(), r(), b(), a()); }
	vector<T, 4> qspq() const { return vector<T, 4>(q(), s(), p(), q()); }
	vector<T, 4> wxwx() const { return vector<T, 4>(w(), x(), w(), x()); }
	vector<T, 4> arar() const { return vector<T, 4>(a(), r(), a(), r()); }
	vector<T, 4> qsqs() const { return vector<T, 4>(q(), s(), q(), s()); }
	vector<T, 4> wxwy() const { return vector<T, 4>(w(), x(), w(), y()); }
	vector<T, 4> arag() const { return vector<T, 4>(a(), r(), a(), g()); }
	vector<T, 4> qsqt() const { return vector<T, 4>(q(), s(), q(), t()); }
	vector<T, 4> wxwz() const { return vector<T, 4>(w(), x(), w(), z()); }
	vector<T, 4> arab() const { return vector<T, 4>(a(), r(), a(), b()); }
	vector<T, 4> qsqp() const { return vector<T, 4>(q(), s(), q(), p()); }
	vector<T, 4> wxww() const { return vector<T, 4>(w(), x(), w(), w()); }
	vector<T, 4> araa() const { return vector<T, 4>(a(), r(), a(), a()); }
	vector<T, 4> qsqq() const { return vector<T, 4>(q(), s(), q(), q()); }
	vector<T, 4> wyxx() const { return vector<T, 4>(w(), y(), x(), x()); }
	vector<T, 4> agrr() const { return vector<T, 4>(a(), g(), r(), r()); }
	vector<T, 4> qtss() const { return vector<T, 4>(q(), t(), s(), s()); }
	vector<T, 4> wyxy() const { return vector<T, 4>(w(), y(), x(), y()); }
	vector<T, 4> agrg() const { return vector<T, 4>(a(), g(), r(), g()); }
	vector<T, 4> qtst() const { return vector<T, 4>(q(), t(), s(), t()); }
	vector<T, 4> wyxz() const { return vector<T, 4>(w(), y(), x(), z()); }
	vector<T, 4> agrb() const { return vector<T, 4>(a(), g(), r(), b()); }
	vector<T, 4> qtsp() const { return vector<T, 4>(q(), t(), s(), p()); }
	vector<T, 4> wyxw() const { return vector<T, 4>(w(), y(), x(), w()); }
	vector<T, 4> agra() const { return vector<T, 4>(a(), g(), r(), a()); }
	vector<T, 4> qtsq() const { return vector<T, 4>(q(), t(), s(), q()); }
	vector<T, 4> wyyx() const { return vector<T, 4>(w(), y(), y(), x()); }
	vector<T, 4> aggr() const { return vector<T, 4>(a(), g(), g(), r()); }
	vector<T, 4> qtts() const { return vector<T, 4>(q(), t(), t(), s()); }
	vector<T, 4> wyyy() const { return vector<T, 4>(w(), y(), y(), y()); }
	vector<T, 4> aggg() const { return vector<T, 4>(a(), g(), g(), g()); }
	vector<T, 4> qttt() const { return vector<T, 4>(q(), t(), t(), t()); }
	vector<T, 4> wyyz() const { return vector<T, 4>(w(), y(), y(), z()); }
	vector<T, 4> aggb() const { return vector<T, 4>(a(), g(), g(), b()); }
	vector<T, 4> qttp() const { return vector<T, 4>(q(), t(), t(), p()); }
	vector<T, 4> wyyw() const { return vector<T, 4>(w(), y(), y(), w()); }
	vector<T, 4> agga() const { return vector<T, 4>(a(), g(), g(), a()); }
	vector<T, 4> qttq() const { return vector<T, 4>(q(), t(), t(), q()); }
	vector<T, 4> wyzx() const { return vector<T, 4>(w(), y(), z(), x()); }
	vector<T, 4> agbr() const { return vector<T, 4>(a(), g(), b(), r()); }
	vector<T, 4> qtps() const { return vector<T, 4>(q(), t(), p(), s()); }
	vector<T, 4> wyzy() const { return vector<T, 4>(w(), y(), z(), y()); }
	vector<T, 4> agbg() const { return vector<T, 4>(a(), g(), b(), g()); }
	vector<T, 4> qtpt() const { return vector<T, 4>(q(), t(), p(), t()); }
	vector<T, 4> wyzz() const { return vector<T, 4>(w(), y(), z(), z()); }
	vector<T, 4> agbb() const { return vector<T, 4>(a(), g(), b(), b()); }
	vector<T, 4> qtpp() const { return vector<T, 4>(q(), t(), p(), p()); }
	vector<T, 4> wyzw() const { return vector<T, 4>(w(), y(), z(), w()); }
	vector<T, 4> agba() const { return vector<T, 4>(a(), g(), b(), a()); }
	vector<T, 4> qtpq() const { return vector<T, 4>(q(), t(), p(), q()); }
	vector<T, 4> wywx() const { return vector<T, 4>(w(), y(), w(), x()); }
	vector<T, 4> agar() const { return vector<T, 4>(a(), g(), a(), r()); }
	vector<T, 4> qtqs() const { return vector<T, 4>(q(), t(), q(), s()); }
	vector<T, 4> wywy() const { return vector<T, 4>(w(), y(), w(), y()); }
	vector<T, 4> agag() const { return vector<T, 4>(a(), g(), a(), g()); }
	vector<T, 4> qtqt() const { return vector<T, 4>(q(), t(), q(), t()); }
	vector<T, 4> wywz() const { return vector<T, 4>(w(), y(), w(), z()); }
	vector<T, 4> agab() const { return vector<T, 4>(a(), g(), a(), b()); }
	vector<T, 4> qtqp() const { return vector<T, 4>(q(), t(), q(), p()); }
	vector<T, 4> wyww() const { return vector<T, 4>(w(), y(), w(), w()); }
	vector<T, 4> agaa() const { return vector<T, 4>(a(), g(), a(), a()); }
	vector<T, 4> qtqq() const { return vector<T, 4>(q(), t(), q(), q()); }
	vector<T, 4> wzxx() const { return vector<T, 4>(w(), z(), x(), x()); }
	vector<T, 4> abrr() const { return vector<T, 4>(a(), b(), r(), r()); }
	vector<T, 4> qpss() const { return vector<T, 4>(q(), p(), s(), s()); }
	vector<T, 4> wzxy() const { return vector<T, 4>(w(), z(), x(), y()); }
	vector<T, 4> abrg() const { return vector<T, 4>(a(), b(), r(), g()); }
	vector<T, 4> qpst() const { return vector<T, 4>(q(), p(), s(), t()); }
	vector<T, 4> wzxz() const { return vector<T, 4>(w(), z(), x(), z()); }
	vector<T, 4> abrb() const { return vector<T, 4>(a(), b(), r(), b()); }
	vector<T, 4> qpsp() const { return vector<T, 4>(q(), p(), s(), p()); }
	vector<T, 4> wzxw() const { return vector<T, 4>(w(), z(), x(), w()); }
	vector<T, 4> abra() const { return vector<T, 4>(a(), b(), r(), a()); }
	vector<T, 4> qpsq() const { return vector<T, 4>(q(), p(), s(), q()); }
	vector<T, 4> wzyx() const { return vector<T, 4>(w(), z(), y(), x()); }
	vector<T, 4> abgr() const { return vector<T, 4>(a(), b(), g(), r()); }
	vector<T, 4> qpts() const { return vector<T, 4>(q(), p(), t(), s()); }
	vector<T, 4> wzyy() const { return vector<T, 4>(w(), z(), y(), y()); }
	vector<T, 4> abgg() const { return vector<T, 4>(a(), b(), g(), g()); }
	vector<T, 4> qptt() const { return vector<T, 4>(q(), p(), t(), t()); }
	vector<T, 4> wzyz() const { return vector<T, 4>(w(), z(), y(), z()); }
	vector<T, 4> abgb() const { return vector<T, 4>(a(), b(), g(), b()); }
	vector<T, 4> qptp() const { return vector<T, 4>(q(), p(), t(), p()); }
	vector<T, 4> wzyw() const { return vector<T, 4>(w(), z(), y(), w()); }
	vector<T, 4> abga() const { return vector<T, 4>(a(), b(), g(), a()); }
	vector<T, 4> qptq() const { return vector<T, 4>(q(), p(), t(), q()); }
	vector<T, 4> wzzx() const { return vector<T, 4>(w(), z(), z(), x()); }
	vector<T, 4> abbr() const { return vector<T, 4>(a(), b(), b(), r()); }
	vector<T, 4> qpps() const { return vector<T, 4>(q(), p(), p(), s()); }
	vector<T, 4> wzzy() const { return vector<T, 4>(w(), z(), z(), y()); }
	vector<T, 4> abbg() const { return vector<T, 4>(a(), b(), b(), g()); }
	vector<T, 4> qppt() const { return vector<T, 4>(q(), p(), p(), t()); }
	vector<T, 4> wzzz() const { return vector<T, 4>(w(), z(), z(), z()); }
	vector<T, 4> abbb() const { return vector<T, 4>(a(), b(), b(), b()); }
	vector<T, 4> qppp() const { return vector<T, 4>(q(), p(), p(), p()); }
	vector<T, 4> wzzw() const { return vector<T, 4>(w(), z(), z(), w()); }
	vector<T, 4> abba() const { return vector<T, 4>(a(), b(), b(), a()); }
	vector<T, 4> qppq() const { return vector<T, 4>(q(), p(), p(), q()); }
	vector<T, 4> wzwx() const { return vector<T, 4>(w(), z(), w(), x()); }
	vector<T, 4> abar() const { return vector<T, 4>(a(), b(), a(), r()); }
	vector<T, 4> qpqs() const { return vector<T, 4>(q(), p(), q(), s()); }
	vector<T, 4> wzwy() const { return vector<T, 4>(w(), z(), w(), y()); }
	vector<T, 4> abag() const { return vector<T, 4>(a(), b(), a(), g()); }
	vector<T, 4> qpqt() const { return vector<T, 4>(q(), p(), q(), t()); }
	vector<T, 4> wzwz() const { return vector<T, 4>(w(), z(), w(), z()); }
	vector<T, 4> abab() const { return vector<T, 4>(a(), b(), a(), b()); }
	vector<T, 4> qpqp() const { return vector<T, 4>(q(), p(), q(), p()); }
	vector<T, 4> wzww() const { return vector<T, 4>(w(), z(), w(), w()); }
	vector<T, 4> abaa() const { return vector<T, 4>(a(), b(), a(), a()); }
	vector<T, 4> qpqq() const { return vector<T, 4>(q(), p(), q(), q()); }
	vector<T, 4> wwxx() const { return vector<T, 4>(w(), w(), x(), x()); }
	vector<T, 4> aarr() const { return vector<T, 4>(a(), a(), r(), r()); }
	vector<T, 4> qqss() const { return vector<T, 4>(q(), q(), s(), s()); }
	vector<T, 4> wwxy() const { return vector<T, 4>(w(), w(), x(), y()); }
	vector<T, 4> aarg() const { return vector<T, 4>(a(), a(), r(), g()); }
	vector<T, 4> qqst() const { return vector<T, 4>(q(), q(), s(), t()); }
	vector<T, 4> wwxz() const { return vector<T, 4>(w(), w(), x(), z()); }
	vector<T, 4> aarb() const { return vector<T, 4>(a(), a(), r(), b()); }
	vector<T, 4> qqsp() const { return vector<T, 4>(q(), q(), s(), p()); }
	vector<T, 4> wwxw() const { return vector<T, 4>(w(), w(), x(), w()); }
	vector<T, 4> aara() const { return vector<T, 4>(a(), a(), r(), a()); }
	vector<T, 4> qqsq() const { return vector<T, 4>(q(), q(), s(), q()); }
	vector<T, 4> wwyx() const { return vector<T, 4>(w(), w(), y(), x()); }
	vector<T, 4> aagr() const { return vector<T, 4>(a(), a(), g(), r()); }
	vector<T, 4> qqts() const { return vector<T, 4>(q(), q(), t(), s()); }
	vector<T, 4> wwyy() const { return vector<T, 4>(w(), w(), y(), y()); }
	vector<T, 4> aagg() const { return vector<T, 4>(a(), a(), g(), g()); }
	vector<T, 4> qqtt() const { return vector<T, 4>(q(), q(), t(), t()); }
	vector<T, 4> wwyz() const { return vector<T, 4>(w(), w(), y(), z()); }
	vector<T, 4> aagb() const { return vector<T, 4>(a(), a(), g(), b()); }
	vector<T, 4> qqtp() const { return vector<T, 4>(q(), q(), t(), p()); }
	vector<T, 4> wwyw() const { return vector<T, 4>(w(), w(), y(), w()); }
	vector<T, 4> aaga() const { return vector<T, 4>(a(), a(), g(), a()); }
	vector<T, 4> qqtq() const { return vector<T, 4>(q(), q(), t(), q()); }
	vector<T, 4> wwzx() const { return vector<T, 4>(w(), w(), z(), x()); }
	vector<T, 4> aabr() const { return vector<T, 4>(a(), a(), b(), r()); }
	vector<T, 4> qqps() const { return vector<T, 4>(q(), q(), p(), s()); }
	vector<T, 4> wwzy() const { return vector<T, 4>(w(), w(), z(), y()); }
	vector<T, 4> aabg() const { return vector<T, 4>(a(), a(), b(), g()); }
	vector<T, 4> qqpt() const { return vector<T, 4>(q(), q(), p(), t()); }
	vector<T, 4> wwzz() const { return vector<T, 4>(w(), w(), z(), z()); }
	vector<T, 4> aabb() const { return vector<T, 4>(a(), a(), b(), b()); }
	vector<T, 4> qqpp() const { return vector<T, 4>(q(), q(), p(), p()); }
	vector<T, 4> wwzw() const { return vector<T, 4>(w(), w(), z(), w()); }
	vector<T, 4> aaba() const { return vector<T, 4>(a(), a(), b(), a()); }
	vector<T, 4> qqpq() const { return vector<T, 4>(q(), q(), p(), q()); }
	vector<T, 4> wwwx() const { return vector<T, 4>(w(), w(), w(), x()); }
	vector<T, 4> aaar() const { return vector<T, 4>(a(), a(), a(), r()); }
	vector<T, 4> qqqs() const { return vector<T, 4>(q(), q(), q(), s()); }
	vector<T, 4> wwwy() const { return vector<T, 4>(w(), w(), w(), y()); }
	vector<T, 4> aaag() const { return vector<T, 4>(a(), a(), a(), g()); }
	vector<T, 4> qqqt() const { return vector<T, 4>(q(), q(), q(), t()); }
	vector<T, 4> wwwz() const { return vector<T, 4>(w(), w(), w(), z()); }
	vector<T, 4> aaab() const { return vector<T, 4>(a(), a(), a(), b()); }
	vector<T, 4> qqqp() const { return vector<T, 4>(q(), q(), q(), p()); }
	vector<T, 4> wwww() const { return vector<T, 4>(w(), w(), w(), w()); }
	vector<T, 4> aaaa() const { return vector<T, 4>(a(), a(), a(), a()); }
	vector<T, 4> qqqq() const { return vector<T, 4>(q(), q(), q(), q()); }
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

	/* Operators */

	T &operator[](const unsigned int i)
	{
	    return array<T, rows, 1>::vl[i];
	}

	const T &operator[](const unsigned int i) const
	{
	    return array<T, rows, 1>::vl[i];
	}

	vector operator*(const T s) const 
	{
	    return static_cast<array<T, rows, 1> >(*this) * s;
	}

	const vector &operator*=(const T s)
	{
	    static_cast<array<T, rows, 1> >(*this) *= s;
	    return *this;
	}

	vector operator*(const vector &v) const 
	{
	    return compMult(v);
	}

	const vector &operator*=(const vector &v)
	{
	    this->compMultAssign(v);
	    return *this;
	}

	vector operator/(const T s) const 
	{
	    return static_cast<array<T, rows, 1> >(*this) / s;
	}

	const vector &operator/=(const T s)
	{
	    static_cast<array<T, rows, 1> >(*this) /= s;
	    return *this;
	}

	vector operator/(const vector &v) const 
	{
	    return compDiv(v);
	}

	const vector &operator/=(const vector &v)
	{
	    this->compDivAssign(v);
	    return *this;
	}

	template<unsigned int rows2>
	array<T, rows, rows2> outerProduct(const vector<T, rows2> &v) const
	{
	    array<T, rows, rows2> a;
	    for (unsigned int i = 0; i < rows; i++)
		for (unsigned int j = 0; j < rows2; j++)
		    a.v[i][j] = array<T, rows, 1>::vl[i] * v.vl[j];
	    return a;
	}

	template<unsigned int rows2>
    	friend array<T, rows, rows2> outerProduct(const vector &v, const vector<T, rows2> &w)
	{
	    return v.outerProduct(w);
	}
    };


    template<typename T>
    class vector2 : public vector<T, 2>
    {
	public:

	/* Constructors, Destructor */

	vector2() {}

	vector2(const swizzler2<T> &s) : vector<T, 2>(s) {}

	vector2(const array<T, 2, 1> &v) : vector<T, 2>(v) {}

	vector2(const vector<T, 2> &v) : vector<T, 2>(v) {}

	vector2(const T x, const T y) : vector<T, 2>(x, y) {}

	vector2(const T *v) : vector<T, 2>(v) {}

	template<typename U>
	vector2(const vector2<U> &v) : vector<T, 2>(v) {}

	vector2(const std::string &s) : vector<T, 2>(s) {}

	~vector2() {}
    };


    template<typename T>
    class vector3 : public vector<T, 3>
    {
	public:

	/* Constructors, Destructor */

	vector3() {}

	vector3(const swizzler3<T> &s) : vector<T, 3>(s) {}

	vector3(const array<T, 3, 1> &v) : vector<T, 3>(v) {}

	vector3(const vector<T, 3> &v) : vector<T, 3>(v) {}

	vector3(const T x, const T y, const T z) : vector<T, 3>(x, y, z) {}

	vector3(const vector2<T> xy, T z) : vector<T, 3>(xy[0], xy[1], z) {}

	vector3(const T x, const vector2<T> yz) : vector<T, 3>(x, yz[0], yz[1]) {}

	vector3(const T *v) : vector<T, 3>(v) {}

	template<typename U>
	vector3(const vector3<U> &v) : vector<T, 3>(v) {}

	vector3(const std::string &s) : vector<T, 3>(s) {}

	~vector3() {}

	/* Geometric functions */

	vector3 cross(const vector3 &v) const
	{
	    return vector3(
		    this->y() * v.z() - this->z() * v.y(), 
		    this->z() * v.x() - this->x() * v.z(), 
		    this->x() * v.y() - this->y() * v.x());
	}

	friend vector3 cross(const vector3 &v1, const vector3 &v2)
	{
	    return v1.cross(v2);
	}
    };


    template<typename T>
    class vector4 : public vector<T, 4>
    {
	public:

	/* Constructors, Destructor */

	vector4() {}

	vector4(const swizzler4<T> &s) : vector<T, 4>(s) {}

	vector4(const array<T, 4, 1> &v) : vector<T, 4>(v) {}

	vector4(const vector<T, 4> &v) : vector<T, 4>(v) {}

	vector4(const T x, const T y, const T z, const T w) : vector<T, 4>(x, y, z, w) {}

	vector4(const vector2<T> xy, const T z, const T w) : vector<T, 4>(xy[0], xy[1], z, w) {}

	vector4(const vector2<T> xy, const vector2<T> zw) : vector<T, 4>(xy[0], xy[1], zw[0], zw[1]) {}

	vector4(const T x, const vector2<T> yz, const T w) : vector<T, 4>(x, yz[0], yz[1], w) {}

	vector4(const T x, const T y, const vector2<T> zw) : vector<T, 4>(x, y, zw[0], zw[1]) {}

	vector4(const vector3<T> xyz, const T w) : vector<T, 4>(xyz[0], xyz[1], xyz[2], w) {}

	vector4(const T x, const vector3<T> yzw) : vector<T, 4>(x, yzw[0], yzw[1], yzw[2]) {}

	vector4(const T *v) : vector<T, 4>(v) {}

	template<typename U>
	vector4(const vector4<U> &v) : vector<T, 4>(v) {}

	vector4(const std::string &s) : vector<T, 4>(s) {}

	~vector4() {}
    };


    template<typename T, unsigned int rows, unsigned int cols>
    class matrix : public array<T, rows, cols>
    {
	public:

	/* Constructors, Destructor */

	matrix() {}

	matrix(const array<T, rows, cols> &a) : array<T, rows, cols>(a) {}

	matrix(const T x)
	{
	    for (unsigned int i = 0; i < rows; i++)
		for (unsigned int j = 0; i < cols; j++)
		    array<T, rows, cols>::v[i][j] = (i == j ? x : static_cast<T>(0));
	}

	matrix(const T v0, const T v1, const T v2, const T v3) : array<T, rows, cols>(v0, v1, v2, v3) {}

	matrix(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5) {}

	matrix(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7) {}

	matrix(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7, const T v8)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8) {}

	matrix(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5,
		const T v6, const T v7, const T v8, const T v9, const T v10, const T v11)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) {}

	matrix(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5, const T v6, const T v7,
		const T v8, const T v9, const T v10, const T v11, const T v12, const T v13, const T v14, const T v15)
	    : array<T, rows, cols>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) {}

	matrix(const T *a) : array<T, rows, cols>(a) {}

	template<typename U>
	matrix(const matrix<U, rows, cols> &a) : array<T, rows, cols>(a) {}

	matrix(const std::string &s) : array<T, rows, cols>(s) {}

	~matrix() {}

	/* Operators */

	T *operator[](const unsigned int row)
	{
	    return array<T, rows, cols>::v[row];
	}

	const T *operator[](const unsigned int row) const
	{
	    return array<T, rows, cols>::v[row];
	}
	
	matrix operator*(const T s) const
	{
	    return static_cast<array<T, rows, cols> >(*this) * s;
	}

    	friend matrix operator*(const T s, const matrix &m)
	{
	    return m * s;
	}

	vector<T, rows> operator*(const vector<T, cols> &w) const
	{
	    vector<T, rows> r;
	    for (unsigned int i = 0; i < rows; i++)
	    {
		r.vl[i] = static_cast<T>(0);
		for (unsigned int j = 0; j < cols; j++)
	       	{
    		    r.vl[i] += array<T, rows, cols>::v[i][j] * w.vl[j];
		}
	    }
	    return r;
	}

    	friend vector<T, cols> operator*(const vector<T, rows> &w, const matrix &m)
	{
	    vector<T, cols> r;
	    for (unsigned int i = 0; i < rows; i++)
	    {
		r.vl[i] = static_cast<T>(0);
		for (unsigned int j = 0; j < cols; j++)
	       	{
    		    r.vl[i] += m.v[i][j] * w.vl[j];
		}
	    }
	    return r;

	}

	matrix<T, rows, rows> operator*(const matrix<T, cols, rows> &n) const 
	{
	    matrix<T, rows, rows> r;
	    for (unsigned int i = 0; i < rows; i++)
	    {
		for (unsigned int j = 0; j < rows; j++)
		{
		    r.v[i][j] = static_cast<T>(0);
		    for (unsigned int k = 0; k < rows; k++)
		    {
			r.v[i][j] += array<T, rows, cols>::v[i][k] * n.v[k][j];
		    }
		}
	    }
	    return r;
	}

	/* Matrix operations */

	matrix matrixCompMult(const matrix &m) const
	{
	    return compMult(m);
	}

    	friend matrix matrixCompMult(const matrix &m, const matrix &n)
	{
	    return m.matrixCompMult(n);
	}
    };


    template<typename T>
    class matrix2 : public matrix<T, 2, 2>
    {
	public:

	/* Constructors, Destructor */

	matrix2() {}

	matrix2(const array<T, 2, 2> &a) : matrix<T, 2, 2>(a) {}

	matrix2(const matrix<T, 2, 2> &a) : matrix<T, 2, 2>(a) {}
	
	matrix2(const T x) : matrix<T, 2, 2>(x) {}

	matrix2(const T v0, const T v1, const T v2, const T v3) 
	    : matrix<T, 2, 2>(v0, v1, v2, v3) {}

	matrix2(const vector2<T> &col0, const vector2<T> &col1)
	    : matrix<T, 2, 2>(col0.x(), col1.x(), col0.y(), col1.y()) {}

	matrix2(const T *a) : matrix<T, 2, 2>(a) {}

	template<typename U>
	matrix2(const matrix2<U> &a) : matrix<T, 2, 2>(a) {}

	matrix2(const std::string &s) : matrix<T, 2, 2>(s) {}

	~matrix2() {}

	/* Operators */
	
	const matrix2 &operator*=(const matrix2 &m)
	{
	    matrix2 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

	T det() const
	{
	    return array<T, 2, 2>::v[0][0] * array<T, 2, 2>::v[1][1] 
		- array<T, 2, 2>::v[0][1] * array<T, 2, 2>::v[1][0];
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
	    return matrix2<T>(
		    array<T, 2, 2>::v[1][1], -array<T, 2, 2>::v[0][1], 
		    -array<T, 2, 2>::v[1][0], array<T, 2, 2>::v[0][0]) 
		/ det();
	}

	friend matrix2<T> inverse(const matrix2<T> &m)
	{
	    return m.inverse();
	}
    };


    template<typename T>
    class matrix3 : public matrix<T, 3, 3>
    {
	public:

	/* Constructors, Destructor */

	matrix3() {}

	matrix3(const array<T, 3, 3> &a) : matrix<T, 3, 3>(a) {}

	matrix3(const matrix<T, 3, 3> &a) : matrix<T, 3, 3>(a) {}
	
	matrix3(const T x) : matrix<T, 3, 3>(x) {}

	matrix3(const T v0, const T v1, const T v2, 
		const T v3, const T v4, const T v5, 
		const T v6, const T v7, const T v8) 
	    : matrix<T, 3, 3>(v0, v1, v2, v3, v4, v5, v6, v7, v8) {}

	matrix3(const vector3<T> &col0, const vector3<T> &col1, const vector3<T> &col2)
	    : matrix<T, 3, 3>(col0.x(), col1.x(), col2.x(),
		    col0.y(), col1.y(), col2.y(),
		    col0.z(), col1.z(), col2.z()) {}

	matrix3(const T angle, const vector3<T> &v)
	{
	    const vector3<T> nv = normalize(v);
	    const T x = nv.x();
	    const T y = nv.y();
	    const T z = nv.z();
	    const T c = glvm::cos(angle);
	    const T s = glvm::sin(angle);
	    const T mc = static_cast<T>(1) - c;
	    array<T, 3, 3>::v[0][0] = x * x * mc + c;
	    array<T, 3, 3>::v[0][1] = x * y * mc - z * s;
	    array<T, 3, 3>::v[0][2] = x * z * mc + y * s;
	    array<T, 3, 3>::v[1][0] = y * x * mc + z * s;
	    array<T, 3, 3>::v[1][1] = y * y * mc + c;
	    array<T, 3, 3>::v[1][2] = y * z * mc - x * s;
	    array<T, 3, 3>::v[2][0] = x * z * mc - y * s;
	    array<T, 3, 3>::v[2][1] = y * z * mc + x * s;
	    array<T, 3, 3>::v[2][2] = z * z * mc + c;
	}

	matrix3(const T *a) : matrix<T, 3, 3>(a) {}

	template<typename U>
	matrix3(const matrix3<U> &a) : matrix<T, 3, 3>(a) {}

	matrix3(const std::string &s) : matrix<T, 3, 3>(s) {}

	~matrix3() {}

	/* Operators */

	const matrix3 &operator*=(const matrix3 &m)
	{
	    matrix3 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

	T det() const
	{
	    return array<T, 3, 3>::v[0][0] * (array<T, 3, 3>::v[1][1] * array<T, 3, 3>::v[2][2] - array<T, 3, 3>::v[1][2] * array<T, 3, 3>::v[2][1]) 
		+ array<T, 3, 3>::v[0][1] * (array<T, 3, 3>::v[1][2] * array<T, 3, 3>::v[2][0] - array<T, 3, 3>::v[1][0] * array<T, 3, 3>::v[2][2]) 
		+ array<T, 3, 3>::v[0][2] * (array<T, 3, 3>::v[1][0] * array<T, 3, 3>::v[2][1] - array<T, 3, 3>::v[1][1] * array<T, 3, 3>::v[2][0]);
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
	    I.v[0][0] = array<T, 3, 3>::v[1][1] * array<T, 3, 3>::v[2][2] - array<T, 3, 3>::v[1][2] * array<T, 3, 3>::v[2][1];
	    I.v[0][1] = array<T, 3, 3>::v[0][2] * array<T, 3, 3>::v[2][1] - array<T, 3, 3>::v[0][1] * array<T, 3, 3>::v[2][2];
	    I.v[0][2] = array<T, 3, 3>::v[0][1] * array<T, 3, 3>::v[1][2] - array<T, 3, 3>::v[0][2] * array<T, 3, 3>::v[1][1];
	    I.v[1][0] = array<T, 3, 3>::v[1][2] * array<T, 3, 3>::v[2][0] - array<T, 3, 3>::v[1][0] * array<T, 3, 3>::v[2][2];
	    I.v[1][1] = array<T, 3, 3>::v[0][0] * array<T, 3, 3>::v[2][2] - array<T, 3, 3>::v[0][2] * array<T, 3, 3>::v[2][0];
	    I.v[1][2] = array<T, 3, 3>::v[0][2] * array<T, 3, 3>::v[1][0] - array<T, 3, 3>::v[0][0] * array<T, 3, 3>::v[1][2];
	    I.v[2][0] = array<T, 3, 3>::v[1][0] * array<T, 3, 3>::v[2][1] - array<T, 3, 3>::v[1][1] * array<T, 3, 3>::v[2][0];
	    I.v[2][1] = array<T, 3, 3>::v[0][1] * array<T, 3, 3>::v[2][0] - array<T, 3, 3>::v[0][0] * array<T, 3, 3>::v[2][1];
	    I.v[2][2] = array<T, 3, 3>::v[0][0] * array<T, 3, 3>::v[1][1] - array<T, 3, 3>::v[0][1] * array<T, 3, 3>::v[1][0];
	    const T invdet = static_cast<T>(1) / (array<T, 3, 3>::v[0][0] * I.v[0][0] 
		    + array<T, 3, 3>::v[0][1] * I.v[1][0] + array<T, 3, 3>::v[0][2] * I.v[2][0]);
	    std::cerr << "A" << std::endl;
	    return I * invdet;
	}

	friend matrix3<T> inverse(const matrix3<T> &m)
	{
	    return m.inverse();
	}
    };


    template<typename T>
    class matrix4 : public matrix<T, 4, 4>
    {
	public:

	/* Constructors, Destructor */

	matrix4() {}

	matrix4(const array<T, 4, 4> &a) : matrix<T, 4, 4>(a) {}

	matrix4(const matrix<T, 4, 4> &a) : matrix<T, 4, 4>(a) {}

	matrix4(const T x) : matrix<T, 4, 4>(x) {}

	matrix4(const T v0, const T v1, const T v2, const T v3,
		const T v4, const T v5, const T v6, const T v7,
		const T v8, const T v9, const T v10, const T v11,
		const T v12, const T v13, const T v14, const T v15) 
	    : matrix<T, 4, 4>(v0, v1, v2, v3, 
		    v4, v5, v6, v7, 
		    v8, v9, v10, v11,
		    v12, v13, v14, v15) {}

	matrix4(const vector4<T> &col0, const vector4<T> &col1, const vector4<T> &col2, const vector4<T> &col3)
	    : matrix<T, 4, 4>(
		    col0.x(), col1.x(), col2.x(), col3.x(),
		    col0.y(), col1.y(), col2.y(), col3.y(),
		    col0.z(), col1.z(), col2.z(), col3.z(),
		    col0.w(), col1.w(), col2.w(), col3.w()) {}

	matrix4(const T angle, const vector3<T> &v)
	{
	    set3x3(matrix3<T>(angle, v));
	    array<T, 4, 4>::v[0][3] = static_cast<T>(0);
	    array<T, 4, 4>::v[1][3] = static_cast<T>(0);
	    array<T, 4, 4>::v[2][3] = static_cast<T>(0);
	    array<T, 4, 4>::v[3][0] = static_cast<T>(0);
	    array<T, 4, 4>::v[3][1] = static_cast<T>(0);
	    array<T, 4, 4>::v[3][2] = static_cast<T>(0);
	    array<T, 4, 4>::v[3][3] = static_cast<T>(1);
	}

	matrix4(const T *a) : matrix<T, 4, 4>(a) {}

	template<typename U>
	matrix4(const matrix4<U> &a) : matrix<T, 4, 4>(a) {}

	matrix4(const std::string &s) : matrix<T, 4, 4>(s) {}

	~matrix4() {}

	/* Operators */

	const matrix4 &operator*=(const matrix4 &m)
	{
	    matrix4 r = *this * m;
	    *this = r;
	    return *this;
	}

	/* Matrix operations */

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
	    const T d0 = array<T, 4, 4>::v[1][1] * (array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][3] - array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][2]) 
		+ array<T, 4, 4>::v[1][2] * (array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][1] - array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][3]) 
		+ array<T, 4, 4>::v[1][3] * (array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][2] - array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][1]);
	    const T d1 = array<T, 4, 4>::v[1][0] * (array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][3] - array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][2]) 
		+ array<T, 4, 4>::v[1][2] * (array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][0] - array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][3]) 
		+ array<T, 4, 4>::v[1][3] * (array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][2] - array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][0]);
	    const T d2 = array<T, 4, 4>::v[1][0] * (array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][3] - array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][1]) 
		+ array<T, 4, 4>::v[1][1] * (array<T, 4, 4>::v[2][3] * array<T, 4, 4>::v[3][0] - array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][3]) 
		+ array<T, 4, 4>::v[1][3] * (array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][1] - array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][0]);
	    const T d3 = array<T, 4, 4>::v[1][0] * (array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][2] - array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][1]) 
		+ array<T, 4, 4>::v[1][1] * (array<T, 4, 4>::v[2][2] * array<T, 4, 4>::v[3][0] - array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][2]) 
		+ array<T, 4, 4>::v[1][2] * (array<T, 4, 4>::v[2][0] * array<T, 4, 4>::v[3][1] - array<T, 4, 4>::v[2][1] * array<T, 4, 4>::v[3][0]);
	    return array<T, 4, 4>::v[0][0] * d0 - array<T, 4, 4>::v[0][1] * d1 + array<T, 4, 4>::v[0][2] * d2 - array<T, 4, 4>::v[0][3] * d3;
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
		array<T, 4, 4>::vl[ 2] * array<T, 4, 4>::vl[ 7] - array<T, 4, 4>::vl[ 6] * array<T, 4, 4>::vl[ 3],
		array<T, 4, 4>::vl[ 2] * array<T, 4, 4>::vl[11] - array<T, 4, 4>::vl[10] * array<T, 4, 4>::vl[ 3],
		array<T, 4, 4>::vl[ 2] * array<T, 4, 4>::vl[15] - array<T, 4, 4>::vl[14] * array<T, 4, 4>::vl[ 3],
	    	array<T, 4, 4>::vl[ 6] * array<T, 4, 4>::vl[11] - array<T, 4, 4>::vl[10] * array<T, 4, 4>::vl[ 7],
      		array<T, 4, 4>::vl[ 6] * array<T, 4, 4>::vl[15] - array<T, 4, 4>::vl[14] * array<T, 4, 4>::vl[ 7],
		array<T, 4, 4>::vl[10] * array<T, 4, 4>::vl[15] - array<T, 4, 4>::vl[14] * array<T, 4, 4>::vl[11] 
	    };

	    /* first half of comatrix: 24 multiplications, 16 additions */
	    result.vl[0] = array<T, 4, 4>::vl[ 5] * t1[5] - array<T, 4, 4>::vl[ 9] * t1[4] + array<T, 4, 4>::vl[13] * t1[3];
	    result.vl[1] = array<T, 4, 4>::vl[ 9] * t1[2] - array<T, 4, 4>::vl[13] * t1[1] - array<T, 4, 4>::vl[ 1] * t1[5];
	    result.vl[2] = array<T, 4, 4>::vl[13] * t1[0] - array<T, 4, 4>::vl[ 5] * t1[2] + array<T, 4, 4>::vl[ 1] * t1[4];
	    result.vl[3] = array<T, 4, 4>::vl[ 5] * t1[1] - array<T, 4, 4>::vl[ 1] * t1[3] - array<T, 4, 4>::vl[ 9] * t1[0];
	    result.vl[4] = array<T, 4, 4>::vl[ 8] * t1[4] - array<T, 4, 4>::vl[ 4] * t1[5] - array<T, 4, 4>::vl[12] * t1[3];
	    result.vl[5] = array<T, 4, 4>::vl[ 0] * t1[5] - array<T, 4, 4>::vl[ 8] * t1[2] + array<T, 4, 4>::vl[12] * t1[1];
	    result.vl[6] = array<T, 4, 4>::vl[ 4] * t1[2] - array<T, 4, 4>::vl[12] * t1[0] - array<T, 4, 4>::vl[ 0] * t1[4];
	    result.vl[7] = array<T, 4, 4>::vl[ 0] * t1[3] - array<T, 4, 4>::vl[ 4] * t1[1] + array<T, 4, 4>::vl[ 8] * t1[0];

	    /* second set of 2x2 determinants: 12 multiplications, 6 additions */
	    const T t2[6] = 
	    {
		array<T, 4, 4>::vl[ 0] * array<T, 4, 4>::vl[ 5] - array<T, 4, 4>::vl[ 4] * array<T, 4, 4>::vl[ 1],
		array<T, 4, 4>::vl[ 0] * array<T, 4, 4>::vl[ 9] - array<T, 4, 4>::vl[ 8] * array<T, 4, 4>::vl[ 1],
		array<T, 4, 4>::vl[ 0] * array<T, 4, 4>::vl[13] - array<T, 4, 4>::vl[12] * array<T, 4, 4>::vl[ 1],
	   	array<T, 4, 4>::vl[ 4] * array<T, 4, 4>::vl[ 9] - array<T, 4, 4>::vl[ 8] * array<T, 4, 4>::vl[ 5],
     		array<T, 4, 4>::vl[ 4] * array<T, 4, 4>::vl[13] - array<T, 4, 4>::vl[12] * array<T, 4, 4>::vl[ 5],
		array<T, 4, 4>::vl[ 8] * array<T, 4, 4>::vl[13] - array<T, 4, 4>::vl[12] * array<T, 4, 4>::vl[ 9] 
	    };

	    /* second half of comatrix: 24 multiplications, 16 additions */
	    result.vl[ 8] = array<T, 4, 4>::vl[ 7] * t2[5] - array<T, 4, 4>::vl[11] * t2[4] + array<T, 4, 4>::vl[15] * t2[3];
	    result.vl[ 9] = array<T, 4, 4>::vl[11] * t2[2] - array<T, 4, 4>::vl[15] * t2[1] - array<T, 4, 4>::vl[ 3] * t2[5];
	    result.vl[10] = array<T, 4, 4>::vl[15] * t2[0] - array<T, 4, 4>::vl[ 7] * t2[2] + array<T, 4, 4>::vl[ 3] * t2[4];
	    result.vl[11] = array<T, 4, 4>::vl[ 7] * t2[1] - array<T, 4, 4>::vl[ 3] * t2[3] - array<T, 4, 4>::vl[11] * t2[0];
	    result.vl[12] = array<T, 4, 4>::vl[10] * t2[4] - array<T, 4, 4>::vl[ 6] * t2[5] - array<T, 4, 4>::vl[14] * t2[3];
	    result.vl[13] = array<T, 4, 4>::vl[ 2] * t2[5] - array<T, 4, 4>::vl[10] * t2[2] + array<T, 4, 4>::vl[14] * t2[1];
	    result.vl[14] = array<T, 4, 4>::vl[ 6] * t2[2] - array<T, 4, 4>::vl[14] * t2[0] - array<T, 4, 4>::vl[ 2] * t2[4];
	    result.vl[15] = array<T, 4, 4>::vl[ 2] * t2[3] - array<T, 4, 4>::vl[ 6] * t2[1] + array<T, 4, 4>::vl[10] * t2[0];

	    /* determinant: 4 multiplications, 3 additions */
	    const T determinant =
		array<T, 4, 4>::vl[0] * result.vl[0] + array<T, 4, 4>::vl[4] * result.vl[1] +
		array<T, 4, 4>::vl[8] * result.vl[2] + array<T, 4, 4>::vl[12] * result.vl[3];

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
	    return vector3<T>(array<T, 4, 4>::v[0][3], array<T, 4, 4>::v[1][3], array<T, 4, 4>::v[2][3]);
	}

	void setTranslation(const vector3<T> &t)
	{
	    array<T, 4, 4>::v[0][3] = t.x();
	    array<T, 4, 4>::v[1][3] = t.y();
	    array<T, 4, 4>::v[2][3] = t.z();
	}

	const matrix4 &translate(const vector3<T> &v)
	{
	    vector4<T> t(v, static_cast<T>(1));
	    array<T, 4, 4>::v[0][3] = dot(this->row(0), t);
	    array<T, 4, 4>::v[1][3] = dot(this->row(1), t);
	    array<T, 4, 4>::v[2][3] = dot(this->row(2), t);
	    array<T, 4, 4>::v[3][3] = dot(this->row(3), t);
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
		array<T, 4, 4>::v[row][0] *= v.x();
		array<T, 4, 4>::v[row][1] *= v.y();
		array<T, 4, 4>::v[row][2] *= v.z();
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
    class matrix2x3 : public matrix<T, 2, 3>
    {
	public:

	/* Constructors, Destructor */

	matrix2x3() {}

	matrix2x3(const array<T, 2, 3> &a) : matrix<T, 2, 3>(a) {}

	matrix2x3(const matrix<T, 2, 3> &a) : matrix<T, 2, 3>(a) {}

	matrix2x3(const T x) : matrix<T, 2, 3>(x) {}

	matrix2x3(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5)
	    : matrix<T, 2, 3>(v0, v1, v2, v3, v4, v5) {}

	matrix2x3(const vector2<T> &col0, const vector2<T> &col1, const vector2<T> &col2)
	    : matrix<T, 2, 3>(
		    col0.x(), col1.x(), col2.x(),
		    col0.y(), col1.y(), col2.y()) {}

	matrix2x3(const T *a) : matrix<T, 2, 3>(a) {}

	template<typename U>
	matrix2x3(const matrix2x3<U> &a) : matrix<T, 2, 3>(a) {}

	matrix2x3(const std::string &s) : matrix<T, 2, 3>(s) {}

	~matrix2x3() {}
    };


    template<typename T>
    class matrix3x2 : public matrix<T, 3, 2>
    {
	public:

	/* Constructors, Destructor */

	matrix3x2() {}

	matrix3x2(const array<T, 3, 2> &a) : matrix<T, 3, 2>(a) {}

	matrix3x2(const matrix<T, 3, 2> &a) : matrix<T, 3, 2>(a) {}

	matrix3x2(const T x) : matrix<T, 3, 2>(x) {}

	matrix3x2(const T v0, const T v1, const T v2, const T v3, const T v4, const T v5)
	    : matrix<T, 3, 2>(v0, v1, v2, v3, v4, v5) {}

	matrix3x2(const vector3<T> &col0, const vector3<T> &col1)
	    : matrix<T, 3, 2>(
		    col0.x(), col1.x(),
		    col0.y(), col1.y(),
		    col0.z(), col1.z()) {}

	matrix3x2(const T *a) : matrix<T, 3, 2>(a) {}

	template<typename U>
	matrix3x2(const matrix3x2<U> &a) : matrix<T, 3, 2>(a) {}

	matrix3x2(const std::string &s) : matrix<T, 3, 2>(s) {}

	~matrix3x2() {}
    };


    template<typename T>
    class matrix2x4 : public matrix<T, 2, 4>
    {
	public:

	/* Constructors, Destructor */

	matrix2x4() {}

	matrix2x4(const array<T, 2, 4> &a) : matrix<T, 2, 4>(a) {}

	matrix2x4(const matrix<T, 2, 4> &a) : matrix<T, 2, 4>(a) {}

	matrix2x4(const T v0, const T v1, const T v2, const T v3,
		const T v4, const T v5, const T v6, const T v7) 
	    : matrix<T, 2, 4>(v0, v1, v2, v3, v4, v5, v6, v7) {}

	matrix2x4(const T x) : matrix<T, 2, 4>(x) {}

	matrix2x4(const vector2<T> &col0, const vector2<T> &col1, const vector2<T> &col2, const vector2<T> &col3)
	    : matrix<T, 2, 4>(
		    col0.x(), col1.x(), col2.x(), col3.x(),
		    col0.y(), col1.y(), col2.y(), col3.y()) {}

	matrix2x4(const T *a) : matrix<T, 2, 4>(a) {}

	template<typename U>
	matrix2x4(const matrix2x4<U> &a) : matrix<T, 2, 4>(a) {}

	matrix2x4(const std::string &s) : matrix<T, 2, 4>(s) {}

	~matrix2x4() {}
    };


    template<typename T>
    class matrix4x2 : public matrix<T, 4, 2>
    {
	public:

	/* Constructors, Destructor */

	matrix4x2() {}

	matrix4x2(const array<T, 4, 2> &a) : matrix<T, 4, 2>(a) {}

	matrix4x2(const matrix<T, 4, 2> &a) : matrix<T, 4, 2>(a) {}

	matrix4x2(const T x) : matrix<T, 4, 2>(x) {}

	matrix4x2(const T v0, const T v1, const T v2, const T v3, 
		const T v4, const T v5, const T v6, const T v7) 
	    : matrix<T, 4, 2>(v0, v1, v2, v3, v4, v5, v6, v7) {}

	matrix4x2(const vector4<T> &col0, const vector4<T> &col1)
	    : matrix<T, 4, 3>(
		    col0.x(), col1.x(),
		    col0.y(), col1.y(),
		    col0.z(), col1.z(),
		    col0.w(), col1.w()) {}

	matrix4x2(const T *a) : matrix<T, 4, 2>(a) {}

	template<typename U>
	matrix4x2(const matrix4x2<U> &a) : matrix<T, 4, 2>(a) {}

	matrix4x2(const std::string &s) : matrix<T, 4, 2>(s) {}

	~matrix4x2() {}
    };


    template<typename T>
    class matrix3x4 : public matrix<T, 3, 4>
    {
	public:

	/* Constructors, Destructor */

	matrix3x4() {}

	matrix3x4(const array<T, 3, 4> &a) : matrix<T, 3, 4>(a) {}

	matrix3x4(const matrix<T, 3, 4> &a) : matrix<T, 3, 4>(a) {}

	matrix3x4(const T x) : matrix<T, 3, 4>(x) {}

	matrix3x4(const T v0, const T v1, const T v2, const T v3, 
		const T v4, const T v5, const T v6, const T v7, 
		const T v8, const T v9, const T v10, const T v11) 
	    : matrix<T, 3, 4>(v0, v1, v2, v3, 
		    v4, v5, v6, v7, 
		    v8, v9, v10, v11) {}

	matrix3x4(const vector3<T> &col0, const vector3<T> &col1, const vector3<T> &col2, const vector3<T> &col3)
	    : matrix<T, 3, 4>(
		    col0.x(), col1.x(), col2.x(), col3.x(),
		    col0.y(), col1.y(), col2.y(), col3.y(),
		    col0.z(), col1.z(), col2.z(), col3.z()) {}

	~matrix3x4() {}
    };


    template<typename T>
    class matrix4x3 : public matrix<T, 4, 3>
    {
	public:

	/* Constructors, Destructor */

	matrix4x3() {}

	matrix4x3(const array<T, 4, 3> &a) : matrix<T, 4, 3>(a) {}

	matrix4x3(const matrix<T, 4, 3> &a) : matrix<T, 4, 3>(a) {}

	matrix4x3(const T x) : matrix<T, 4, 3>(x) {}

	matrix4x3(const T v0, const T v1, const T v2, const T v3, 
		const T v4, const T v5, const T v6, const T v7, 
		const T v8, const T v9, const T v10, const T v11) 
	    : matrix<T, 4, 3>(v0, v1, v2, v3, 
		    v4, v5, v6, v7, 
		    v8, v9, v10, v11) {}

	matrix4x3(const vector4<T> &col0, const vector4<T> &col1, const vector4<T> &col2)
	    : matrix<T, 4, 3>(
		    col0.x(), col1.x(), col2.x(),
		    col0.y(), col1.y(), col2.y(),
		    col0.z(), col1.z(), col2.z(),
		    col0.w(), col1.w(), col2.w()) {}

	matrix4x3(const T *a) : matrix<T, 4, 3>(a) {}

	template<typename U>
	matrix4x3(const matrix4x3<U> &a) : matrix<T, 4, 3>(a) {}

	matrix4x3(const std::string &s) : matrix<T, 4, 3>(s) {}

	~matrix4x3() {}
    };


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
    typedef matrix2x3<float> mat2x3;
    typedef matrix2x3<double> dmat2x3;
    typedef matrix3x2<float> mat3x2;
    typedef matrix3x2<double> dmat3x2;
    typedef matrix2x4<float> mat2x4;
    typedef matrix2x4<double> dmat2x4;
    typedef matrix4x2<float> mat4x2;
    typedef matrix4x2<double> dmat4x2;
    typedef matrix3x4<float> mat3x4;
    typedef matrix3x4<double> dmat3x4;
    typedef matrix4x3<float> mat4x3;
    typedef matrix4x3<double> dmat4x3;


    template<typename T>
    class quaternion : public array<T, 4, 1>
    {
	public:

	/* Constructors, Destructor */

	quaternion() {}

	quaternion(const array<T, 4, 1> &a) : array<T, 4, 1>(a) {}

	quaternion(const vector4<T> &a) : array<T, 4, 1>(a) {}

	quaternion(const T *v) : array<T, 4, 1>(v) {}

	quaternion(const std::string &s) : array<T, 4, 1>(s) {}

	template<typename U>
	quaternion(const quaternion<U> &q) : array<T, 4, 1>(q) {}

	quaternion(const T w, const T x, const T y, const T z) : array<T, 4, 1>(w, x, y, z) {}

	quaternion(const T w) : array<T, 4, 1>(w, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)) {}

	quaternion(const T w, const vector3<T> &xyz) : array<T, 4, 1>(w, xyz[0], xyz[1], xyz[2]) {}

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

	/* Quaternion operations */

	quaternion conjugate() const
	{
	    return quaternion(w(), -x(), -y(), -z());
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

	/* Conversions */

	matrix3<T> to_matrix3() const
	{
	    matrix3<T> M;
	    const T xx = x() * x();
	    const T xy = x() * y();
	    const T xz = x() * z();
	    const T xw = x() * w();
	    const T yy = y() * y();
	    const T yz = y() * z();
	    const T yw = y() * w();
	    const T zz = z() * z();
	    const T zw = z() * w();
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
	    M.set3x3(to_matrix3());
	    M.vl[3]  = static_cast<T>(0);
	    M.vl[7]  = static_cast<T>(0);
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
	    T cos_a = nq.w();
	    *angle = glvm::acos(cos_a) * static_cast<T>(2);
	    T sin_a = glvm::sqrt(static_cast<T>(1) - cos_a * cos_a);
	    if (glvm::abs(sin_a) < static_cast<T>(0.0005))
	    {
		sin_a = static_cast<T>(1);
	    }
	    axis.x() = nq.x() / sin_a;
	    axis.y() = nq.y() / sin_a;
	    axis.z() = nq.z() / sin_a;
	}

	void from_axis_angle(const vector3<T> &axis, const T angle)
	{
	    vector3<T> naxis = axis.normalize();
	    T sin_a = glvm::sin(angle / static_cast<T>(2));
	    T cos_a = glvm::cos(angle / static_cast<T>(2));
	    w() = cos_a;
	    x() = naxis.x() * sin_a;
	    y() = naxis.y() * sin_a;
	    z() = naxis.z() * sin_a;
	    this->normalize();
	}

	void to_euler_angles(T *rx, T *ry, T *rz) const
	{
	    *rx = glvm::atan(static_cast<T>(2) * (w() * x() + y() * z()) / (static_cast<T>(1) - static_cast<T>(2) * (x() * x() + y() * y())));
	    *ry = glvm::asin(static_cast<T>(2) * (w() * y() - x() * z()));
	    *rz = glvm::atan(static_cast<T>(2) * (w() * z() + x() * y()) / (static_cast<T>(1) - static_cast<T>(2) * (y() * y() + z() * z())));
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


    typedef quaternion<float> quat;
    typedef quaternion<double> dquat;
}

#endif
