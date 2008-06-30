/*
 * heightmap_quads_vs.glsl
 *
 * This file is part of cvlview, an image viewer using the CVL library.
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

#version 120

uniform int channel;
uniform float factor;
uniform float channel_min;
uniform float channel_max;
uniform float gamma;
uniform int invert;
uniform sampler2D heightmap;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vec4 data = texture2D(heightmap, gl_MultiTexCoord1.xy);
    float height = (channel == 0 ? data.r : channel == 1 ? data.g : channel == 2 ? data.b : data.a);
    height = clamp((height - channel_min) / (channel_max - channel_min), 0.0, 1.0);
    height = 1.0 - clamp(pow(1.0 - height, gamma), 0.0, 1.0);
    if (invert == 1)
    {
	height = 1.0 - height;
    }
    height = factor * (height - 0.5);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.x, gl_Vertex.y, height, gl_Vertex.w);
}
