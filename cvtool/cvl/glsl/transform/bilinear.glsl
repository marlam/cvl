/*
 * bilinear.glsl
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

#version 110

uniform sampler2D tex;
uniform float width;		// Texture width.
uniform float step_h;		// = 1.0 / width
uniform float step_h_2;		// = step_h / 2.0
uniform float height;		// Texture height.
uniform float step_v;		// = 1.0 / height
uniform float step_v_2;		// = step_v / 2.0

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    float x0 = floor((coord.x - step_h_2) * width) / width + step_h_2;
    float y0 = floor((coord.y - step_v_2) * height) / height + step_v_2;
    float x1 = x0 + step_h;
    float y1 = y0 + step_v;
    vec4 cx0y0 = texture2D(tex, vec2(x0, y0));
    vec4 cx1y0 = texture2D(tex, vec2(x1, y0));
    vec4 cx0y1 = texture2D(tex, vec2(x0, y1));
    vec4 cx1y1 = texture2D(tex, vec2(x1, y1));
    float x_diff = (coord.x - x0) * width;
    float y_diff = (coord.y - y0) * height;

    vec4 fp = cx0y0 + y_diff * (cx0y1 - cx0y0);
    vec4 fq = cx1y0 + y_diff * (cx1y1 - cx1y0);
    gl_FragColor = fp + x_diff * (fq - fp);
}
