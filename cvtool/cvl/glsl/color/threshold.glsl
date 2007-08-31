/*
 * threshold.glsl
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
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
uniform float threshold;

void main()
{
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);

#if ALL_CHANNELS
    c.r = (c.r >= threshold ? 1.0 : 0.0);
    c.g = (c.g >= threshold ? 1.0 : 0.0);
    c.b = (c.b >= threshold ? 1.0 : 0.0);
    c.a = (c.a >= threshold ? 1.0 : 0.0);
#else
    c.CHANNEL = (c.CHANNEL >= threshold ? 1.0 : 0.0);
#endif

    gl_FragColor = c;
}
