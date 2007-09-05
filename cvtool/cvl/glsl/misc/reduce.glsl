/*
 * reduce.glsl
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

#version 120

#define MODE_MIN $mode_min
#define MODE_MIN_GREATER_ZERO $mode_mingz
#define MODE_ABSMIN $mode_absmin
#define MODE_ABSMIN_GREATER_ZERO $mode_absmingz
#define MODE_MAX $mode_max
#define MODE_ABSMAX $mode_absmax
#define MODE_SUM $mode_sum
#define ALL_CHANNELS $all_channels
#define CHANNEL $channel

uniform sampler2D tex;
uniform float step_h;
uniform float step_v;


vec4 reduce_2(vec4 c1, vec4 c2)
{
#if MODE_MIN
# if ALL_CHANNELS
    return min(c1, c2);
# else
    return (c1.CHANNEL < c2.CHANNEL ? c1 : c2);
# endif
#elif MODE_ABSMIN
# if ALL_CHANNELS
    return min(abs(c1), abs(c2));
# else
    vec4 r;
    r = (abs(c1.CHANNEL) < abs(c2.CHANNEL) ? c1 : c2);
    r.CHANNEL = (abs(c1.CHANNEL) < abs(c2.CHANNEL) ? abs(c1.CHANNEL) : abs(c2.CHANNEL));
    return r;
# endif
#elif MODE_MIN_GREATER_ZERO
# if ALL_CHANNELS
    vec4 r;
    r.r = (c1.r <= 0.0 ? c2.r : (c2.r <= 0.0) ? c1.r : (c1.r < c2.r) ? c1.r : c2.r);
    r.g = (c1.g <= 0.0 ? c2.g : (c2.g <= 0.0) ? c1.g : (c1.g < c2.g) ? c1.g : c2.g);
    r.b = (c1.b <= 0.0 ? c2.b : (c2.b <= 0.0) ? c1.b : (c1.b < c2.b) ? c1.b : c2.b);
    r.a = (c1.a <= 0.0 ? c2.a : (c2.a <= 0.0) ? c1.a : (c1.a < c2.a) ? c1.a : c2.a);
    return r;
# else
    return (c1.CHANNEL <= 0.0 ? c2.CHANNEL : (c2.CHANNEL <= 0.0) ? c1.CHANNEL : (c1.CHANNEL < c2.CHANNEL) ? c1.CHANNEL : c2.CHANNEL);
# endif
#elif MODE_ABSMIN_GREATER_ZERO
# if ALL_CHANNELS
    vec4 r;
    r.r (abs(c1.r) <= 0.0 ? abs(c2.r) : (abs(c2.r) <= 0.0) ? abs(c1.r) : (abs(c1.r) < abs(c2.r)) ? abs(c1.r) : abs(c2.r));
    r.g (abs(c1.g) <= 0.0 ? abs(c2.g) : (abs(c2.g) <= 0.0) ? abs(c1.g) : (abs(c1.g) < abs(c2.g)) ? abs(c1.g) : abs(c2.g));
    r.b (abs(c1.b) <= 0.0 ? abs(c2.b) : (abs(c2.b) <= 0.0) ? abs(c1.b) : (abs(c1.b) < abs(c2.b)) ? abs(c1.b) : abs(c2.b));
    r.a (abs(c1.a) <= 0.0 ? abs(c2.a) : (abs(c2.a) <= 0.0) ? abs(c1.a) : (abs(c1.a) < abs(c2.a)) ? abs(c1.a) : abs(c2.a));
    return r;
# else
    bool c1_wins = (abs(c1.CHANNEL) <= 0.0 ? false : (abs(c2.CHANNEL) <= 0.0) ? true 
	    : (abs(c1.CHANNEL) < abs(c2.CHANNEL)) ? true : false);
    vec4 r = c1_wins ? c1 : c2;
    r.CHANNEL = c1_wins ? abs(c1.CHANNEL) : abs(c2.CHANNEL);
    return r;
# endif
#elif MODE_MAX
# if ALL_CHANNELS
    return max(c1, c2);
# else
    return (c1.CHANNEL > c2.CHANNEL ? c1 : c2);
# endif
#elif MODE_ABSMAX
# if ALL_CHANNELS
    return max(abs(c1), abs(c2));
# else
    vec4 r;
    r = (abs(c1.CHANNEL) > abs(c2.CHANNEL) ? c1 : c2);
    r.CHANNEL = (abs(c1.CHANNEL) > abs(c2.CHANNEL) ? abs(c1.CHANNEL) : abs(c2.CHANNEL));
    return r;
# endif
#elif MODE_SUM
    return c1 + c2;
#endif
}

vec4 reduce_4(vec4 c1, vec4 c2, vec4 c3, vec4 c4)
{
    return reduce_2(reduce_2(c1, c2), reduce_2(c3, c4));
}


void main()
{
    float x0 = gl_TexCoord[0].x - step_h / 2.0;
    float y0 = gl_TexCoord[0].y - step_v / 2.0;
    float x1 = x0 + step_h;
    float y1 = y0 + step_v;

    vec4 c;
    if (x1 < 1.0 && y1 < 1.0)
    {
	c = reduce_4(
		texture2D(tex, vec2(x0, y0)),
		texture2D(tex, vec2(x0, y1)),
		texture2D(tex, vec2(x1, y0)),
		texture2D(tex, vec2(x1, y1)));
    }
    else if (x1 < 1.0 && y1 >= 1.0)
    {
	c = reduce_2(
		texture2D(tex, vec2(x0, y0)),
		texture2D(tex, vec2(x1, y0)));
    }
    else if (x1 >= 1.0 && y1 < 1.0)
    {
	c = reduce_2(
		texture2D(tex, vec2(x0, y0)),
		texture2D(tex, vec2(x0, y1)));
    }
    else
    {
	c = texture2D(tex, vec2(x0, y0));
    }

    gl_FragColor = c;
}
