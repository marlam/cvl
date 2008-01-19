/*
 * channel_extract.glsl
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
uniform int channel;

void main()
{
    vec4 c = texture2D(tex, gl_TexCoord[0].xy);
    float l;

    if (channel == 0)
	gl_FragColor = vec4(c.r, c.r, c.r, c.r);
    else if (channel == 1)
	gl_FragColor = vec4(c.g, c.g, c.g, c.g);
    else if (channel == 2)
	gl_FragColor = vec4(c.b, c.b, c.b, c.b);
    else
	gl_FragColor = vec4(c.a, c.a, c.a, c.a);
}
