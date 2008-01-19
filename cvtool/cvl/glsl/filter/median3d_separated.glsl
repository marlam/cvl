/*
 * median3d_separated.glsl
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
 * the 2D median_separated.glsl .*/

const int k_t = $k_t;
const int mask_size = 2 * k_t + 1;
vec4 mask[mask_size];
uniform sampler2D textures[mask_size];

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
	mask[t + k_t] = texture2D(textures[t + k_t], gl_TexCoord[0].xy);
    }
    bubblesort();
    gl_FragColor = mask[mask_size / 2];
}
