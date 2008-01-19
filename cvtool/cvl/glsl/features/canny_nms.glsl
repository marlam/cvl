/*
 * canny_nms.glsl
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

uniform float step_h;
uniform float step_v;
uniform sampler2D tex;
const float PI = 3.14159265358979323846;

void main()
{
    float edge_strength = texture2D(tex, gl_TexCoord[0].xy).r;
    float edge_direction = texture2D(tex, gl_TexCoord[0].xy).g;

    if (edge_strength <= 0.0)
    {
	edge_strength = 0.0;
	edge_direction = 0.0;
    }
    else 
    {
	vec2 p, q;
	if ((edge_direction >= (-1.0 / 8.0) * PI && edge_direction < (1.0 / 8.0) * PI)
		|| (edge_direction >= (7.0 / 8.0) * PI || edge_direction < (-7.0 / 8.0) * PI))
	{
	    p = vec2(-1.0, 0.0);
	    q = vec2(+1.0, 0.0);
	}
	else if ((edge_direction >= (3.0 / 8.0) * PI && edge_direction < (5.0 / 8.0) * PI)
		|| (edge_direction >= (-5.0 / 8.0) * PI && edge_direction < (-3.0 / 8.0) * PI))
	{
	    p = vec2(0.0, -1.0);
	    q = vec2(0.0, +1.0);
	}
	else if ((edge_direction >= (1.0 / 8.0) * PI && edge_direction < (3.0 / 8.0) * PI)
		|| (edge_direction >= (-7.0 / 8.0) * PI && edge_direction < (-5.0 / 8.0) * PI))
	{
	    p = vec2(-1.0, -1.0);
	    q = vec2(+1.0, +1.0);
	}
	else 
	{
	    p = vec2(-1.0, +1.0);
	    q = vec2(+1.0, -1.0);
	}
	float sp = texture2D(tex, gl_TexCoord[0].xy + vec2(p.x * step_h, p.y * -step_v)).r;
	float sq = texture2D(tex, gl_TexCoord[0].xy + vec2(q.x * step_h, q.y * -step_v)).r;
	if (sp > edge_strength || sq > edge_strength)
	{
	    edge_strength = 0.0;
	    edge_direction = 0.0;
	}
    }
    gl_FragColor = vec4(edge_strength, edge_direction, 0.0, 0.0);
}
