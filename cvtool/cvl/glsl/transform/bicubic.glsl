/*
 * bicubic.glsl
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

/* 
 * This is an implementation of the Mitchell-Netravali family of bicubic
 * filters. See the original paper:
 * D. Mitchell, A. Netravali,  "Reconstruction Filters in Computer Graphics",
 * ACM SIGGRAPH Computer Graphics Volume 22, Issue 4, Aug. 1988
 */

#version 110

uniform sampler2D tex;
uniform float width;		// Texture width.
uniform float step_h;		// = 1.0 / width
uniform float step_h_2;		// = step_h / 2.0
uniform float height;		// Texture height.
uniform float step_v;		// = 1.0 / height
uniform float step_v_2;		// = step_v / 2.0

#define $mode

#if defined b_spline
const float B = 1.0;
const float C = 0.0;
#elif defined catmull_rom_spline
const float B = 0.0;
const float C = 0.5;
#else /* recommended: mitchell_netravali */
const float B = 1.0 / 3.0;
const float C = 1.0 / 3.0;
#endif

float mn_weight(float x)
{
    float ax = abs(x);
    if (ax < 1.0)
    {
	return ((12.0 - 9.0 * B - 6.0 * C) * ax * ax * ax
		+ (-18.0 + 12.0 * B + 6.0 * C) * ax * ax
		+ (6.0 - 2.0 * B)) / 6.0;
    }
    else if (ax < 2.0)
    {
	return ((-B - 6.0 * C) * ax * ax * ax 
		+ (6.0 * B + 30.0 * C) * ax * ax
		+ (-12.0 * B - 48.0 * C) * ax
		+ (8.0 * B + 24.0 * C)) / 6.0;
    }
    else
    {
	return 0.0;
    }
}

vec4 cubic_filter(float d, vec4 c0, vec4 c1, vec4 c2, vec4 c3)
{
    return mn_weight(d + 1.0) * c0
	 + mn_weight(d      ) * c1
	 + mn_weight(1.0 - d) * c2
	 + mn_weight(2.0 - d) * c3;
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    
    float x[4], y[4];

    x[1] = floor((coord.x - step_h_2) * width) / width + step_h_2;
    y[1] = floor((coord.y - step_v_2) * height) / height + step_v_2;
    x[0] = x[1] - step_h;
    y[0] = y[1] - step_v;
    x[2] = x[1] + step_h;
    y[2] = y[1] + step_v;
    x[3] = x[2] + step_h;
    y[3] = y[2] + step_v;

    float dx = (coord.x - x[1]) * width;
    float dy = (coord.y - y[1]) * height;

    vec4 c[4];
    for (int i = 0; i < 4; i++)
    {
	c[i] = cubic_filter(dy, 
		texture2D(tex, vec2(x[i], y[0])),
		texture2D(tex, vec2(x[i], y[1])),
		texture2D(tex, vec2(x[i], y[2])),
		texture2D(tex, vec2(x[i], y[3])));
    }
    gl_FragColor = cubic_filter(dx, c[0], c[1], c[2], c[3]);
}
