/*
 * tonemap_reinhard02_step2.glsl
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

const int k_0 = 3;
const int k_1 = 6;
const int k_2 = 9;
const int k_3 = 12;
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
uniform float log_avg_lum;
uniform float brightness;
uniform float white;
uniform float sharpness;
uniform float threshold;
uniform sampler2D textures[2];

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

    /* Linear prescaling */
    float scale_factor = brightness / log_avg_lum;
    float Lm = scale_factor * old_Y;
    g_0 *= scale_factor;
    g_1 *= scale_factor;
    g_2 *= scale_factor;
    g_3 *= scale_factor;

    /* Dodging and Burning */
    float v_0 = (old_Y - g_0) / (pow(2.0, sharpness) * brightness / (0.2 * 0.2) + old_Y);
    float v_1 = (g_0 - g_1) / (pow(2.0, sharpness) * brightness / (s_0 * s_0) + g_0);
    float v_2 = (g_1 - g_2) / (pow(2.0, sharpness) * brightness / (s_1 * s_1) + g_1);
    float v_3 = (g_2 - g_3) / (pow(2.0, sharpness) * brightness / (s_2 * s_2) + g_2);
    float Lsmax = (v_3 < threshold) ? g_3 : (v_2 < threshold) ? g_2 : (v_1 < threshold) ? g_1 : (v_0 < threshold) ? g_0 : old_Y;
    // global variant: Lsmax = old_Y;
    float Ld = Lm * (1.0 + Lm / (white * white)) / (1.0 + Lsmax);
    float new_Y = clamp(Ld, 0.00001, 1.0);
    // visualize the choice: new_Y = (v_3 < threshold) ? 1.0 : (v_2 < threshold) ? 0.75 : (v_1 < threshold) ? 0.5 : (v_0 < threshold) ? 0.25 : 0.0;

    /* Recompute color */
    float new_X = (new_Y / y) * x;
    float new_Z = (new_Y / y) * (1.0 - x - y);
    gl_FragColor = vec4(new_X, new_Y, new_Z, 0.0);
}
