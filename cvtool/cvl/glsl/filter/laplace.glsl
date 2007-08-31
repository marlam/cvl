/*
 * laplace.glsl
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
uniform vec2 step;
uniform float c;
uniform bool clamping;

void main()
{
    vec4 A = texture2D(tex, gl_TexCoord[0].xy + vec2(- step[0], - step[1]));
    vec4 B = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0      , - step[1]));
    vec4 C = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step[0], - step[1]));
    vec4 D = texture2D(tex, gl_TexCoord[0].xy + vec2(- step[0], 0.0      ));
    vec4 E = texture2D(tex, gl_TexCoord[0].xy                             );
    vec4 F = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step[0], 0.0      ));
    vec4 G = texture2D(tex, gl_TexCoord[0].xy + vec2(- step[0], + step[1]));
    vec4 H = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0      , + step[1]));
    vec4 I = texture2D(tex, gl_TexCoord[0].xy + vec2(+ step[0], + step[1]));

    vec4 L = (- 2.0 * A + 1.0 * B - 2.0 * C 
	      + 1.0 * D + 4.0 * E + 1.0 * F 
	      - 2.0 * G + 1.0 * H - 2.0 * I);

    gl_FragColor = E + c * L;
    if (clamping)
    {
	gl_FragColor = clamp(gl_FragColor, 0.0, 1.0);
    }
}
