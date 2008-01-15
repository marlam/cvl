/*
 * tonemap_ashikhmin02_step1.glsl
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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

const int k_0 = 2;
const int k_1 = 4;
const int k_2 = 8;
const int k_3 = 8;
uniform float mask_0[2 * k_0 + 1];
uniform float mask_1[2 * k_1 + 1];
uniform float mask_2[2 * k_2 + 1];
uniform float mask_3[2 * k_3 + 1];
uniform float factor_0;
uniform float factor_1;
uniform float factor_2;
uniform float factor_3;
uniform float xstep;
uniform sampler2D tex;

void main()
{
    float Y[2 * k_3 + 1];
    for (int c = -k_3; c <= +k_3; c++)
    {
	Y[c + k_3] = texture2D(tex, gl_TexCoord[0].xy + vec2(float(c) * xstep, 0.0)).g;
    }

    float g_0 = 0.0;
    for (int c = -k_0; c <= +k_0; c++)
    {
	g_0 += mask_0[c + k_0] * Y[c + k_3];
    }
    g_0 *= factor_0;
    float g_1 = 0.0;
    for (int c = -k_1; c <= +k_1; c++)
    {
	g_1 += mask_1[c + k_1] * Y[c + k_3];
    }
    g_1 *= factor_1;
    float g_2 = 0.0;
    for (int c = -k_2; c <= +k_2; c++)
    {
	g_2 += mask_2[c + k_2] * Y[c + k_3];
    }
    g_2 *= factor_2;
    float g_3 = 0.0;
    for (int c = -k_3; c <= +k_3; c++)
    {
	g_3 += mask_3[c + k_3] * Y[c + k_3];
    }
    g_3 *= factor_3;

    gl_FragColor = vec4(g_0, g_1, g_2, g_3);
}
