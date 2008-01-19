/*
 * gamma_correct.glsl
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

#define FORMAT_HSL $format_hsl
#define FORMAT_XYZ $format_xyz

uniform sampler2D tex;
uniform float g;

void main()
{
    vec4 x = texture2D(tex, gl_TexCoord[0].xy);
    vec4 gx;

#if FORMAT_HSL
    float gl = clamp(pow(x.b, g), 0.0, 1.0);
    gx = vec4(x.r, x.g, gl, 0.0);
#elif FORMAT_XYZ
    float X_old = x.r;
    float Y_old = x.g;
    float Z_old = x.b;
    float Sum_old = X_old + Y_old + Z_old;
    float x_old = X_old / Sum_old;
    float y_old = Y_old / Sum_old;
    float z_old = Z_old / Sum_old;
    float Y_new = pow(Y_old, g);
    float X_new = (Y_new / y_old) * x_old;
    float Z_new = (Y_new / y_old) * z_old;
    gx = vec4(X_new, Y_new, Z_new, 0.0);
#else
    gx = clamp(pow(x, vec4(g, g, g, g)), 0.0, 1.0);
#endif

    gl_FragColor = gx;
}
