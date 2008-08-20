/*
 * soft_thresholding.glsl
 * 
 * This file is part of CVL, a computer vision library.
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

#version 110

uniform float upper_bound;
uniform float lower_bound;
uniform vec4 T;
uniform sampler2D tex;

float mysign(float x)
{
    return (x >= 0.0 ? +1.0 : -1.0);
}

void main()
{
    float x = gl_TexCoord[0].x;
    float y = gl_TexCoord[0].y;
    vec4 oldval = texture2D(tex, vec2(x, y));
    vec4 newval;

    // Only process the Details of the right level.
    if (x < upper_bound && y < upper_bound
	    && (x >= lower_bound || y >= lower_bound))
    {
	// Soft Thresholding
	newval.r = (abs(oldval.r) >= T.r ? mysign(oldval.r) * (abs(oldval.r) - T.r) : 0.0);
	newval.g = (abs(oldval.g) >= T.g ? mysign(oldval.g) * (abs(oldval.g) - T.g) : 0.0);
	newval.b = (abs(oldval.b) >= T.b ? mysign(oldval.b) * (abs(oldval.b) - T.b) : 0.0);
	newval.a = (abs(oldval.a) >= T.a ? mysign(oldval.a) * (abs(oldval.a) - T.a) : 0.0);
    }
    else
    {
	newval = oldval;
    }

    gl_FragColor = newval;
}
