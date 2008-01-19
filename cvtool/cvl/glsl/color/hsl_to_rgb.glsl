/*
 * hsl_to_rgb.glsl
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

float helper(float tmp2, float tmp1, float H)
{
    float ret;
    
    if (H < 0.0)
	H += 1.0;
    else if (H > 1.0)
	H -= 1.0;

    if (H < 1.0 / 6.0)
	ret = (tmp2 + (tmp1 - tmp2) * (360.0 / 60.0) * H);
    else if (H < 1.0 / 2.0)
	ret = tmp1;
    else if (H < 2.0 / 3.0)
	ret = (tmp2 + (tmp1 - tmp2) * ((2.0 / 3.0) - H) * (360.0 / 60.0));
    else
	ret = tmp2;
    return ret;
}

void main()
{
    vec3 hsl = texture2D(tex, gl_TexCoord[0].xy).xyz;
    vec3 rgb;
    float tmp1, tmp2;

    if (hsl.z < 0.5) 
	tmp1 = hsl.z * (1.0 + hsl.y);
    else
	tmp1 = (hsl.z + hsl.y) - (hsl.z * hsl.y);
    tmp2 = 2.0 * hsl.z - tmp1;
    rgb.r = helper(tmp2, tmp1, hsl.x + (1.0 / 3.0));
    rgb.g = helper(tmp2, tmp1, hsl.x);
    rgb.b = helper(tmp2, tmp1, hsl.x - (1.0 / 3.0));
    gl_FragColor = vec4(rgb, 1.0);
}
