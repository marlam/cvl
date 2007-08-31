/*
 * tonemap_durand02_step2.glsl
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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

uniform float compression_factor;
uniform float log_absolute_scale;
uniform sampler2D tex;

void main()
{
    vec4 color = texture2D(tex, gl_TexCoord[0].xy);
    float log_input_intensity = color.r;
    float log_base = color.g;
    float log_details = log_input_intensity - log_base;
    float log_output_intensity = log_base * compression_factor + log_details - log_absolute_scale;

    // Recompute XYZ color. Avoid Y == 0.0 (out of range).
    float x = color.b;
    float y = color.a;
    float Y = clamp(exp(log_output_intensity) - 1.0, 0.00001, 1.0);
    float X = (Y / y) * x;
    float Z = (Y / y) * (1.0 - x - y);
    gl_FragColor = vec4(X, Y, Z, 0.0);
}
