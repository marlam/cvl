/*
 * convolve3d_separable.glsl
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

/* The horizontal and vertical direction can be computed using
 * the 2D convolve_separable.glsl .*/

const int k_t = $k_t;
uniform float mask_t[2 * k_t + 1];
uniform float factor_t;
uniform sampler2D textures[2 * k_t + 1];

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int t = -k_t; t <= +k_t; t++)
    {
	color += mask_t[t + k_t] * texture2D(textures[t + k_t], gl_TexCoord[0].xy);
    }
    gl_FragColor = factor_t * color;
}
