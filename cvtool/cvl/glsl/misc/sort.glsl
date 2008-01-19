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
uniform float hstep;	// = 1.0 / float(w)
uniform float vstep;	// = 1.0 / float(h)

int coord_to_index(vec2 coord)
{
    int x = int(round((coord.x - hstep / 2.0) * float(width)));
    int y = int(round((coord.y - vstep / 2.0) * float(height)));
    return y * width + x;
}

vec2 index_to_coord(int i)
{
    int x = i % width;
    int y = i / width;
    return vec2(
	    float(x) / float(width) + hstep / 2.0,
	    float(y) / float(height) + vstep / 2.0);
}

void main()
{
    // The current index is i, the other index is j.
    vec2 icoord = gl_TexCoord[0].xy;
    int i = coord_to_index(icoord);
    int csign = (i % stageno < offset) ? +1 : -1;
    int cdir = ((i / stepno) % 2 == 0) ? +1 : -1;
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
