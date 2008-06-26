/*
 * arcball.h
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

#ifndef ARC_BALL_H
#define ARC_BALL_H


#include "glvm.h"
using namespace glvm;


class ArcBall
{
    private:
	int _width, _height;
	bool _active;
	vec3 _last_sphere_point;

	vec3 map(const int x, const int y);

    public:
	ArcBall(const int width, const int height);

	~ArcBall();

	void resize(const int width, const int height);

	void start(const int x, const int y);

	void stop();

	quat rotation(const int x, const int y, const quat &last_rot);
};

#endif
