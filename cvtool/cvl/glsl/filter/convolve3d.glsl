/*
 * convolve3d.glsl
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

const int k_h = $k_h;
const int k_v = $k_v;
const int k_t = $k_t;
const int h_len = 2 * k_h + 1;
const int v_len = 2 * k_v + 1;
const int t_len = 2 * k_t + 1;
uniform float kernel[h_len * v_len * t_len];
uniform float factor;
uniform float step_h;
uniform float step_v;
uniform sampler2D textures[t_len];

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int t = -k_t; t <= +k_t; t++)
    {
	for (int r = -k_v; r <= +k_v; r++)
	{
	    for (int c = -k_h; c <= +k_v; c++)
	    {
		color += kernel[(t + k_t) * (h_len * v_len) + (r + k_v) * h_len + (c + k_h)]
		    * texture2D(textures[t + k_t], gl_TexCoord[0].xy 
			    + vec2(float(c) * step_h, float(r) * step_v));
	    }
	}
    }
    gl_FragColor = factor * color;
}
