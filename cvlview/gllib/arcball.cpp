/*
 * arcball.cpp
 * 
 * An ArcBall implementation: translate 2D mouse movements to 3D rotations.
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


#include "glvm.h"
using namespace glvm;

#include "arcball.h"

#include "../mhlib/mh.h"


vec3 ArcBall::map(const int x, const int y)
{
    // bring v=(x,y) to [-1..1]^2
    vec2 v = vec2(x, _height - 1 - y);
    v /= vec2(_width - 1, _height - 1);
    v -= vec2(0.5f, 0.5f);
    v *= 2.0f;

    float ll = dot(v, v);
    if (ll > 1.0f)
    {
	// outside ArcBall
	return vec3(v / sqrtf(ll), 0.0f);
    }
    else
    {
	// inside ArcBall
	return vec3(v, sqrtf(1.0f - ll));
    }
}

ArcBall::ArcBall(const int width, const int height)
{
    resize(width, height);
}

ArcBall::~ArcBall()
{
}

void ArcBall::resize(const int width, const int height)
{
    stop();
    _width = (width >= 2 ? width : 2);
    _height = (height >= 2 ? height : 2);
}

void ArcBall::start(const int x, const int y)
{
    _last_sphere_point = map(x, y);
    _active = true;
}

void ArcBall::stop()
{
    _active = false;
}

quat ArcBall::rotation(const int x, const int y, const quat &last_rot)
{
    quat rot = last_rot;
    if (_active)
    {
	vec3 sphere_point = map(x, y);
	vec3 normal = cross(_last_sphere_point, sphere_point);
	if (length(normal) > 0.001f)
	{
	    const float angle = acos(dot(_last_sphere_point, sphere_point));
	    rot *= toQuat(angle, normal);
	}
	_last_sphere_point = sphere_point;
    }
    return rot;
}
