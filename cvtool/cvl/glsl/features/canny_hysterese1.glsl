/*
 * canny_hysterese1.glsl
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
uniform float tl, th;
uniform sampler2D tex;

void main()
{
    float edge_strength = texture2D(tex, gl_TexCoord[0].xy).r;

    if (edge_strength <= 0.0 || edge_strength >= 1.0)
    {
	discard;
    }
    else if (edge_strength < tl)
    {
	gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else if (edge_strength >= th)
    {
	gl_FragColor = vec4(1.0, texture2D(tex, gl_TexCoord[0].xy).g, 0.0, 0.0);
    }
    else if (  texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, - step_v)).r >= 1.0
    	    || texture2D(tex, gl_TexCoord[0].xy + vec2(0.0     , - step_v)).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, - step_v)).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, 0.0     )).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, 0.0     )).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(- step_h, + step_v)).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(0.0     , + step_v)).r >= 1.0
	    || texture2D(tex, gl_TexCoord[0].xy + vec2(+ step_h, + step_v)).r >= 1.0)
    {
	gl_FragColor = vec4(1.0, texture2D(tex, gl_TexCoord[0].xy).g, 0.0, 0.0);
    }
    else
    {
	discard;
    }
}
