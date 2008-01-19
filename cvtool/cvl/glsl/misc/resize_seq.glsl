/*
 * resize_seq.glsl
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
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D tex;		// The texture.
uniform vec4 fill_color;	// Fill the empty area with this data.
uniform int src_width;
uniform int src_height;
uniform int dst_width;
uniform int dst_height;


// Transform texture coordinates to an index.
int coord_to_index(vec2 coord, int w, int h)
{
    int x = int(round((coord.x - (0.5 / float(w))) * float(w)));
    int y = int(round((coord.y - (0.5 / float(h))) * float(h)));
    return y * w + x;
}

// Transform an index to texture coordinates.
vec2 index_to_coord(int i, int w, int h)
{
    int x = i % w;
    int y = i / w;
    return vec2(
	    float(x) / float(w) + (0.5 / float(w)),
	    float(y) / float(h) + (0.5 / float(h)));
}

void main()
{
    // Compute the data index in the destination that we write to.
    int dst_i = coord_to_index(gl_TexCoord[0].xy, dst_width, dst_height);
    if (dst_i >= src_width * src_height)
    {
	// Fill the additional space.
	gl_FragColor = fill_color;
    }
    else
    {
	// Compute the corresponding source coordinates.
	vec2 src_coord = index_to_coord(dst_i, src_width, src_height);
	// Copy.
	gl_FragColor = texture2D(tex, src_coord);
    }
}
