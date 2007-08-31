/*
 * convolve.glsl
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

const int hk = $hk;
const int vk = $vk;
uniform float hstep;
uniform float vstep;
uniform float kernel[(2 * hk + 1) * (2 * vk + 1)];
uniform float factor;
uniform sampler2D tex;

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int r = -vk; r <= +vk; r++)
    {
	for (int c = -hk; c <= hk; c++)
	{
	    color += kernel[(r + vk) * (2 * hk + 1) + (c + hk)] 
		* texture2D(tex, gl_TexCoord[0].xy 
			+ vec2(float(c) * hstep, float(r) * vstep));
	}
    }
    gl_FragColor = factor * color;
}
