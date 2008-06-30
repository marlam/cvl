/*
 * rgb_to_lum.glsl
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
    // We use the sRGB D65 reference white for the RGB values.
    // The computation is exactly the same that is used by pfstools-1.5.
    vec3 m = vec3(0.212656, 0.715158, 0.072186);
    vec3 rgb = texture2D(tex, gl_TexCoord[0].xy).rgb;
    rgb.r = (rgb.r <= 0.04045 ? (rgb.r / 12.92) : (pow((rgb.r + 0.055) / 1.055, 2.4)));
    rgb.g = (rgb.g <= 0.04045 ? (rgb.g / 12.92) : (pow((rgb.g + 0.055) / 1.055, 2.4)));
    rgb.b = (rgb.b <= 0.04045 ? (rgb.b / 12.92) : (pow((rgb.b + 0.055) / 1.055, 2.4)));
    float lum = dot(m, rgb);
    gl_FragColor = vec4(lum, lum, lum, 0.0);
}
