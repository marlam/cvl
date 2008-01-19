/*
 * transform_linear.glsl
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

#version 110

#define CHANNEL $channel
#define ALL_CHANNELS $all_channels

uniform sampler2D tex;
uniform float xmin, xmax;

void main()
{
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);

#if ALL_CHANNELS
    c = clamp(c, xmin, xmax);
    c = (c - xmin) / (xmax - xmin);
#else
    c.CHANNEL = clamp(c.CHANNEL, xmin, xmax);
    c.CHANNEL = (c.CHANNEL - xmin) / (xmax - xmin);
#endif

    gl_FragColor = c;
}
