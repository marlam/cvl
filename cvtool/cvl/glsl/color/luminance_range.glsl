/*
 * luminance_range.glsl
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

#version 120

uniform sampler2D tex;
uniform float lum_min, lum_max;

void main()
{
    vec3 XYZ = texture2D(tex, gl_TexCoord[0].xy).rgb;

    float X_old = XYZ.r;
    float Y_old = XYZ.g;
    float Z_old = XYZ.b;
    float Sum_old = X_old + Y_old + Z_old;

    float x_old = X_old / Sum_old;
    float y_old = Y_old / Sum_old;
    float z_old = Z_old / Sum_old;

    // Avoid zero luminance. Not allowed in XYZ!
    float Y_new = (clamp(Y_old, lum_min + 0.00001, lum_max) - lum_min) / (lum_max - lum_min);

    float X_new = (Y_new / y_old) * x_old;
    float Z_new = (Y_new / y_old) * z_old;

    gl_FragColor = vec4(X_new, Y_new, Z_new, 0.0);
}
