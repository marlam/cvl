/*
 * biquadratic.glsl
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
    
    float x0, x1, x2;
    float y0, y1, y2;
    float xt = floor((coord.x - step_h_2) * width) / width + step_h_2;
    float yt = floor((coord.y - step_v_2) * height) / height + step_v_2;
    if (coord.x - xt < step_h_2)
    {
	x0 = xt - step_h;
    	x1 = xt;
	x2 = xt + step_h;
    }
    else
    {
	x0 = xt;
	x1 = xt + step_h;
	x2 = xt + 2.0 * step_h;
    }
    if (coord.y - yt < step_v_2)
    {
	y0 = yt - step_v;
	y1 = yt;
	y2 = yt + step_v;
    }
    else
    {
	y0 = yt;
	y1 = yt + step_v;
	y2 = yt + 2.0 * step_v;
    }

    vec4 cx0y0 = texture2D(tex, vec2(x0, y0));
    vec4 cx0y1 = texture2D(tex, vec2(x0, y1));
    vec4 cx0y2 = texture2D(tex, vec2(x0, y2));
    vec4 cx1y0 = texture2D(tex, vec2(x1, y0));
    vec4 cx1y1 = texture2D(tex, vec2(x1, y1));
    vec4 cx1y2 = texture2D(tex, vec2(x1, y2));
    vec4 cx2y0 = texture2D(tex, vec2(x2, y0));
    vec4 cx2y1 = texture2D(tex, vec2(x2, y1));
    vec4 cx2y2 = texture2D(tex, vec2(x2, y2));

    float dx = (coord.x - x1) * width;
    float dy = (coord.y - y1) * height;
    float qx = 0.5 * dx * dx;
    float qy = 0.5 * dy * dy;
    float rx = 0.5 * dx;
    float ry = 0.5 * dy;

    vec4 ca = cx1y0 + (cx2y0 - cx0y0) * rx + (cx0y0 - 2.0 * cx1y0 + cx2y0) * qx;
    vec4 cb = cx1y1 + (cx2y1 - cx0y1) * rx + (cx0y1 - 2.0 * cx1y1 + cx2y1) * qx;
    vec4 cc = cx1y2 + (cx2y2 - cx0y2) * rx + (cx0y2 - 2.0 * cx1y2 + cx2y2) * qx;
    gl_FragColor = cb + (cc - ca) * ry + (ca - 2.0 * cb + cc) * qy;
}
