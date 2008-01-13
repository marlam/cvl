/*
 * tonemap_reinhard05.glsl
 * 
 * This file is part of CVL, a computer vision library.
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

#version 110

uniform float f;
uniform float c;
uniform float l;
uniform float m;
uniform float min_lum;
uniform float max_lum;
uniform vec3 I_a_global;
uniform sampler2D tex;

float rgb_to_lum(vec3 rgb)
{
    // See rgb_to_lum.glsl.
    vec3 m = vec3(0.212656, 0.715158, 0.072186);
    if (all(lessThanEqual(rgb, vec3(0.04045, 0.04045, 0.04045))))
    {
	rgb /= 12.92;
    }
    else
    {
	rgb = pow((rgb + 0.055) / 1.055, vec3(2.4, 2.4, 2.4));
    }
    return dot(m, rgb);
}

void main()
{
    vec3 I, I_a_local, I_a;
    float lum;
    
    I = texture2D(tex, gl_TexCoord[0].xy).rgb;
    lum = rgb_to_lum(I);

    I_a_local = c * I + (1.0 - c) * vec3(lum, lum, lum);
    I_a = l * I_a_local + (1.0 - l) * I_a_global;

    I = I / (I + pow(f * I_a, vec3(m, m, m)));
    I = clamp((I - min_lum) / (max_lum - min_lum), 0.0, 1.0);

    gl_FragColor = vec4(I, 1.0);
}
