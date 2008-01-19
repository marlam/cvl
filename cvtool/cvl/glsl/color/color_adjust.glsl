/*
 * color_adjust.glsl
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
uniform float hue;
uniform float saturation;
uniform float lightness;
uniform float contrast;

const float PI = 3.14159265358979323846;

void main()
{
    vec3 hsl = texture2D(tex, gl_TexCoord[0].xy).xyz;

    hsl.x = clamp(hsl.x + hue / (2.0 * PI), 0.0, 1.0);
    hsl.y = clamp(hsl.y + saturation * hsl.y, 0.0, 1.0);
    hsl.z = clamp(hsl.z + lightness * hsl.z, 0.0, 1.0);
    hsl.z = clamp((hsl.z - 0.5) * (contrast + 1.0) + 0.5, 0.0, 1.0);

    gl_FragColor = vec4(hsl, 1.0);
}
