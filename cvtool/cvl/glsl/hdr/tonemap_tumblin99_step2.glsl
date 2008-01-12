/*
 * tonemap_tumblin_step2.glsl
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

uniform float max_abs_lum;
uniform float Lwa;
uniform float Lda;
uniform float m;
uniform float gamma_w;
uniform float gamma_d;
uniform sampler2D tex;

void main()
{
    vec3 XYZ = texture2D(tex, gl_TexCoord[0].xy).rgb;
    float x = XYZ.r / (XYZ.r + XYZ.g + XYZ.b);
    float y = XYZ.g / (XYZ.r + XYZ.g + XYZ.b);

    float old_Y = max_abs_lum * XYZ.g;
    float new_Y = m * Lda * pow(old_Y / Lwa, gamma_w / gamma_d) / 1000.0;
    new_Y = clamp(new_Y, 0.00001, 1.0);

    float new_X = (new_Y / y) * x;
    float new_Z = (new_Y / y) * (1.0 - x - y);
    gl_FragColor = vec4(new_X, new_Y, new_Z, 0.0);
}
