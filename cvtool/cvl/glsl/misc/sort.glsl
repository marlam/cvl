/*
 * sort.glsl
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#version 120
#extension GL_EXT_gpu_shader4 : enable

#define ALL_CHANNELS $all_channels
#define CHANNEL $channel

uniform sampler2D tex;	// The texture.
uniform int stageno;	// = 2 ^ stage
uniform int stepno;	// = 2 ^ step
uniform int offset;	// = 2 ^ (stage - 1)
uniform int width;	// Texture width.
uniform int height;	// Texture height.

int coord_to_index(vec2 coord)
{
    int x = int(coord.x * float(width));
    int y = int(coord.y * float(height));
    return y * width + x;
}

vec2 index_to_coord(int i)
{
    int y = i / width;
    int x = i - y * width;
    return vec2((float(x) + 0.5) / float(width), (float(y) + 0.5) / float(height));
}

// avoid the modulo operator since not all drivers support it (as of
// 20080702).
int mymodulo(int x, int y)
{
    return x - (x / y) * y;
}

void main()
{
    // The current index is i, the other index is j.
    vec2 icoord = gl_TexCoord[0].xy;
    int i = coord_to_index(icoord);
    int csign = (mymodulo(i, stageno) < offset) ? +1 : -1;
    int cdir = (mymodulo(i / stepno, 2) == 0) ? +1 : -1;
    vec2 jcoord = index_to_coord(i + csign * offset);

    // Get the values.
    vec4 ival = texture2D(tex, icoord);
    vec4 jval = texture2D(tex, jcoord);

    // Sort.
#if ALL_CHANNELS
    vec4 minval = min(ival, jval);
    vec4 maxval = max(ival, jval);
#else
    vec4 minval = (ival.CHANNEL < jval.CHANNEL ? ival : jval);
    vec4 maxval = (ival.CHANNEL > jval.CHANNEL ? ival : jval);
#endif
    gl_FragColor = (csign == cdir ? minval : maxval);
}
