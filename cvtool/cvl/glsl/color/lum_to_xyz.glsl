/*
 * lum_to_xyz.glsl
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

uniform sampler2D tex;

void main()
{
    const float d65_x = 0.31271;
    const float d65_y = 0.32902;
    float Y = texture2D(tex, gl_TexCoord[0].xy).g;
    float X = Y * (d65_x / d65_y);
    float Z = min(1.0, Y * (1.0 - d65_x - d65_y) / d65_y);
    gl_FragColor = vec4(X, Y, Z, 0.0);
}
