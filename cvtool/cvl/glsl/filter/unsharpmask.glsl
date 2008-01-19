/*
 * unsharpmask.glsl
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

uniform sampler2D textures[2];
uniform vec2 step;
uniform float c;
uniform bool clamping;

void main()
{
    vec4 O = texture2D(textures[0], gl_TexCoord[0].xy);
    vec4 S = texture2D(textures[1], gl_TexCoord[0].xy);

    gl_FragColor = (c / (2.0 * c - 1.0)) * O - ((1.0 - c) / (2.0 * c - 1.0)) * S;
    if (clamping)
    {
	gl_FragColor = clamp(gl_FragColor, 0.0, 1.0);
    }
}
