/*
 * rgb_to_lum.glsl
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

void main()
{
    // We use the sRGB D65 reference white for the RGB values.
    // The computation is exactly the same that is used by pfstools-1.5.
    vec3 m = vec3(0.212656, 0.715158, 0.072186);
    vec3 rgb = texture2D(tex, gl_TexCoord[0].xy).rgb;
    if (all(lessThanEqual(rgb, vec3(0.04045, 0.04045, 0.04045))))
	rgb /= 12.92;
    else
	rgb = pow((rgb + 0.055) / 1.055, vec3(2.4, 2.4, 2.4));
    float lum = dot(m, rgb);
    gl_FragColor = vec4(lum, lum, lum, lum);
}
