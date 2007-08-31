/*
 * channel_combine.glsl
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

uniform sampler2D tex_c0;
uniform sampler2D tex_c1;
uniform sampler2D tex_c2;
uniform sampler2D tex_c3;
uniform bool have_c0;
uniform bool have_c1;
uniform bool have_c2;
uniform bool have_c3;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    float c0 = (have_c0 ? texture2D(tex_c0, coord).r : 0.0);
    float c1 = (have_c1 ? texture2D(tex_c1, coord).r : 0.0);
    float c2 = (have_c2 ? texture2D(tex_c2, coord).r : 0.0);
    float c3 = (have_c3 ? texture2D(tex_c3, coord).r : 0.0);
    gl_FragColor = vec4(c0, c1, c2, c3);
}
