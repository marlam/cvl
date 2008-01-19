/*
 * sobel.glsl
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

#define CHANNEL $channel

uniform float step_h;
uniform float step_v;
uniform sampler2D tex;

void main()
{
    float a = texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, - step_v)).CHANNEL;
    float b = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0     , - step_v)).CHANNEL;
    float c = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, - step_v)).CHANNEL;
    float d = texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, 0.0     )).CHANNEL;
    float f = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, 0.0     )).CHANNEL;
    float g = texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, + step_v)).CHANNEL;
    float h = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0     , + step_v)).CHANNEL;
    float i = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, + step_v)).CHANNEL;

    float fx = (- 1.0 * a + 1.0 * c - 2.0 * d + 2.0 * f - 1.0 * g + 1.0 * i) / 4.0;
    float fy = (+ 1.0 * a + 2.0 * b + 1.0 * c - 1.0 * g - 2.0 * h - 1.0 * i) / 4.0;
   
    float edge_strength = sqrt(fx * fx + fy * fy);
    if (edge_strength < 0.0001)
	edge_strength = 0.0;
    float edge_direction = (edge_strength >= 0.0001 ? atan(fy, fx) : 0.0);

    gl_FragColor = vec4(edge_strength, edge_direction, 0.0, 0.0);
}
