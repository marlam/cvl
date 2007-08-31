/*
 * canny_hysterese2.glsl
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

void main()
{
    float edge_strength = texture2D(tex, gl_TexCoord[0].xy).r;

    if (edge_strength < 1.0)
    {
	gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else
    {
	gl_FragColor = vec4(1.0, texture2D(tex, gl_TexCoord[0].xy).g, 0.0, 0.0);
    }
}
