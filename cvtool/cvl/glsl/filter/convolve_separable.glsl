/*
 * convolve_separable.glsl
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

const int k = $k;
uniform vec2 step;
uniform float mask[2 * k + 1];
uniform float factor;
uniform sampler2D tex;

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -k; i <= +k; i++)
    {
	color += mask[i + k] * texture2D(tex, gl_TexCoord[0].xy + float(i) * step);
    }
    gl_FragColor = factor * color;
}
