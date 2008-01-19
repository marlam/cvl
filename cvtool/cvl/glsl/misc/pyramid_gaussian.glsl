/*
 * pyramid_gaussian.glsl
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

uniform sampler2D tex;
uniform float width;
uniform float height;

void main()
{
    const int k = 1;
    float kernel[9] = { 1.0, 2.0, 1.0,
			2.0, 4.0, 2.0,
			1.0, 2.0, 1.0 };
    const float kernel_sum = 16.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int r = -k; r <= +k; r++)
    {
	for (int c = -k; c <= +k; c++)
	{
	    color += kernel[(r + k) * (2 * k + 1) + (c + k)]
		* texture2D(tex, gl_TexCoord[0].xy 
			+ vec2(float(c) / width, float(r) / height));
	}
    }
    color /= kernel_sum;
    gl_FragColor = color;
}
