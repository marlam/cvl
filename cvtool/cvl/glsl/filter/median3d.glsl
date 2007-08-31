/*
 * median3d.glsl
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
const int len_h = 2 * k_h + 1;
const int len_v = 2 * k_v + 1;
const int len_t = 2 * k_t + 1;
const int mask_size = len_h * len_v * len_t;
uniform float step_h;
uniform float step_v;
uniform sampler2D textures[len_t];
vec4 mask[mask_size];

void swap_r(int j)
{
    float tmp = mask[j + 1].r;
    mask[j + 1].r = mask[j].r;
    mask[j].r = tmp;
}

void swap_g(int j)
{
    float tmp = mask[j + 1].g;
    mask[j + 1].g = mask[j].g;
    mask[j].g = tmp;
}

void swap_b(int j)
{
    float tmp = mask[j + 1].b;
    mask[j + 1].b = mask[j].b;
    mask[j].b = tmp;
}

void swap_a(int j)
{
    float tmp = mask[j + 1].a;
    mask[j + 1].a = mask[j].a;
    mask[j].a = tmp;
}

void bubblesort() 
{ 
    for (int i = 0; i < mask_size; i++)
    { 
	for (int j = 0; j < mask_size - 1; j++)
	{ 
	    if (mask[j].r > mask[j + 1].r)
		swap_r(j);
	    if (mask[j].g > mask[j + 1].g)
		swap_g(j);
	    if (mask[j].b > mask[j + 1].b)
		swap_b(j);
	    if (mask[j].a > mask[j + 1].a)
		swap_a(j);
	} 
    } 
} 

void main()
{
    for (int t = -k_t; t <= +k_t; t++)
    {
	for (int r = -k_v; r <= +k_v; r++)
	{
	    for (int c = -k_h; c <= +k_v; c++)
	    {
		mask[(t + k_t) * (len_h * len_v) + (r + k_v) * len_h + (c + k_h)]
		    = texture2D(textures[t + k_t], gl_TexCoord[0].xy 
			    + vec2(float(c) * step_h, float(r) * step_v));
	    }
	}
    }
    bubblesort();
    gl_FragColor = mask[mask_size / 2];
}
