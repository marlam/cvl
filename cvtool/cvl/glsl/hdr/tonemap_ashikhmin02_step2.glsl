/*
 * tonemap_ashikhmin02_step2.glsl
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

const int k_0 = 1;
const int k_1 = 3;
const int k_2 = 5;
const int k_3 = 7;
uniform float mask_0[2 * k_0 + 1];
uniform float mask_1[2 * k_1 + 1];
uniform float mask_2[2 * k_2 + 1];
uniform float mask_3[2 * k_3 + 1];
uniform float factor_0;
uniform float factor_1;
uniform float factor_2;
uniform float factor_3;
uniform float ystep;
uniform float s_0;
uniform float s_1;
uniform float s_2;
uniform float s_3;
uniform float min_abs_lum;
uniform float max_abs_lum;
uniform float t;
uniform sampler2D textures[2];


float C(float L)
{
    if (L < 0.0034)
	return L / 0.0014;
    else if (L < 1.0)
	return 2.4483 + log(L / 0.0034) / 0.4027;
    else if (L < 7.2444)
	return 16.5630 + (L - 1.0) / 0.4027;
    else
	return 32.0693 + log(L / 7.2444) / 0.0556;
}

float F(float Lwa)
{
    return (C(Lwa * max_abs_lum) - C(min_abs_lum)) / (C(max_abs_lum) - C(min_abs_lum));
}

void main()
{
    vec4 Y[2 * k_3 + 1];
    for (int r = -k_3; r <= +k_3; r++)
    {
	Y[r + k_3] = texture2D(textures[1], gl_TexCoord[0].xy + vec2(0.0, float(r) * ystep));
    }

    float g_0 = 0.0;
    for (int r = -k_0; r <= +k_0; r++)
    {
	g_0 += mask_0[r + k_0] * Y[r + k_3].r;
    }
    g_0 *= factor_0;
    float g_1 = 0.0;
    for (int r = -k_1; r <= +k_1; r++)
    {
	g_1 += mask_1[r + k_1] * Y[r + k_3].g;
    }
    g_1 *= factor_1;
    float g_2 = 0.0;
    for (int r = -k_2; r <= +k_2; r++)
    {
	g_2 += mask_2[r + k_2] * Y[r + k_3].b;
    }
    g_2 *= factor_2;
    float g_3 = 0.0;
    for (int r = -k_3; r <= +k_3; r++)
    {
	g_3 += mask_3[r + k_3] * Y[r + k_3].a;
    }
    g_3 *= factor_3;

    /* Tone Mapping */
    vec3 XYZ = texture2D(textures[0], gl_TexCoord[0].xy).rgb;
    float x = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);
    float y = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);
    float old_Y = XYZ.g;

    float v_0 = abs((old_Y - g_1) / old_Y);
    float v_1 = abs((g_0 - g_1) / g_0);
    float v_2 = abs((g_1 - g_2) / g_1);
    float v_3 = abs((g_2 - g_3) / g_2);
    float Lwa = (v_3 <= t) ? g_3 : (v_2 <= t) ? g_2 : (v_1 <= t) ? g_1 : (v_0 <= t) ? g_0 : old_Y;
    float new_Y = clamp(F(Lwa) * old_Y / Lwa, 0.00001, 1.0);
    // visualize the choice: new_Y = (v_3 <= t) ? 1.0 : (v_2 <= t) ? 0.75 : (v_1 <= t) ? 0.5 : (v_0 <= t) ? 0.25 : 0.0;

    /* Recompute color */
    float new_X = (new_Y / y) * x;
    float new_Z = (new_Y / y) * (1.0 - x - y);
    gl_FragColor = vec4(new_X, new_Y, new_Z, 1.0);
}
