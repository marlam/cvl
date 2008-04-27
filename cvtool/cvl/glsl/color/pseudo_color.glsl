/*
 * pseudo_color.glsl
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

uniform sampler2D tex;
uniform float factor;
uniform float startcolor;
uniform float lightness;
uniform float xmin, xmax;
uniform int invert;

void main()
{
    float x = texture2D(tex, gl_TexCoord[0].xy).CHANNEL;
    x = (clamp(x, xmin, xmax) - xmin) / (xmax - xmin);
    if (bool(invert))
    {
	x = 1.0 - x;
    }

    float H = (2.0 / 3.0) - startcolor - factor * x;
    if (H < -1.0)
	H += 2.0;
    else if (H < 0.0)
	H += 1.0;
    float S = 1.0;
    float L = clamp(((1.0 - lightness) * 0.5) + lightness * x * 0.5, 0.0, 0.5);
    gl_FragColor = vec4(H, S, L, 0.0);
}
