/*
 * rgb_to_hsl.glsl
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
    vec3 rgb = texture2D(tex, gl_TexCoord[0].xy).rgb;
    vec3 hsl;

    float minval = min(min(rgb.r, rgb.g), rgb.b);
    float maxval = max(max(rgb.r, rgb.g), rgb.b);
    float delta = maxval - minval;

    hsl.z = (maxval + minval) / 2.0;
    if (maxval == minval)
    {
	hsl.x = 0.0;
	hsl.y = 0.0;
    }
    else
    {
	hsl.y = delta / ((hsl.z <= 0.5) ? (maxval + minval) : (2.0 - maxval - minval));
	if (maxval == rgb.r)
	{
	    hsl.x = (60.0 / 360.0) * (rgb.g - rgb.b) / (maxval - minval);
	    if (rgb.g < rgb.b)
		hsl.x += 360.0 / 360.0;
	}
	else if (maxval == rgb.g)
	{
	    hsl.x = (60.0 / 360.0) * (rgb.b - rgb.r) / (maxval - minval) + (120.0 / 360.0);
	}
	else
	{
	    hsl.x = (60.0 / 360.0) * (rgb.r - rgb.g) / (maxval - minval) + (240.0 / 360.0);
	}
    }
    gl_FragColor = vec4(hsl, 1.0);
}
