/*
 * layer.glsl
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

#define $layer_mode

const int n = $n;
uniform sampler2D layers[n];


#if defined mode_or || defined mode_and || defined mode_xor
/* This is necessary until bitwise OR, AND, XOR are available in GLSL. */
bool a_bits[8];
bool b_bits[8];
void bit_decompose(int a, int b)
{
    int s;
    s = 0;
    a_bits[7] = (a >= 128);
    if (a_bits[7]) s += 128;
    a_bits[6] = (a - s >= 64);
    if (a_bits[6]) s += 64;
    a_bits[5] = (a - s >= 32);
    if (a_bits[5]) s += 32;
    a_bits[4] = (a - s >= 16);
    if (a_bits[4]) s += 16;
    a_bits[3] = (a - s >= 8);
    if (a_bits[3]) s += 8;
    a_bits[2] = (a - s >= 4);
    if (a_bits[2]) s += 4;
    a_bits[1] = (a - s >= 2);
    if (a_bits[1]) s += 2;
    a_bits[0] = (a - s >= 1);
    if (a_bits[0]) s += 1;
    s = 0;
    b_bits[7] = (b >= 128);
    if (b_bits[7]) s += 128;
    b_bits[6] = (b - s >= 64);
    if (b_bits[6]) s += 64;
    b_bits[5] = (b - s >= 32);
    if (b_bits[5]) s += 32;
    b_bits[4] = (b - s >= 16);
    if (b_bits[4]) s += 16;
    b_bits[3] = (b - s >= 8);
    if (b_bits[3]) s += 8;
    b_bits[2] = (b - s >= 4);
    if (b_bits[2]) s += 4;
    b_bits[1] = (b - s >= 2);
    if (b_bits[1]) s += 2;
    b_bits[0] = (b - s >= 1);
    if (b_bits[0]) s += 1;
}
#endif

#if defined mode_or
int bitwise_or(int a, int b)
{
    bit_decompose(a, b);
    int x = 0;
    int f = 1;
    for (int i = 0; i < 8; i++)
    {
	if (a_bits[i] || b_bits[i])
	    x += f; 
	f *= 2;
    }
    return x;
}
#elif defined mode_and
int bitwise_and(int a, int b)
{
    bit_decompose(a, b);
    int x = 0;
    int f = 1;
    for (int i = 0; i < 8; i++)
    {
	if (a_bits[i] && b_bits[i])
	    x += f; 
	f *= 2;
    }
    return x;
}
#elif defined mode_xor
int bitwise_xor(int a, int b)
{
    bit_decompose(a, b);
    int x = 0;
    int f = 1;
    for (int i = 0; i < 8; i++)
    {
	if (a_bits[i] ^^ b_bits[i])
	    x += f; 
	f *= 2;
    }
    return x;
}
#endif

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
#if defined mode_min
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c = min(c, texture2D(layers[i], coord));
    gl_FragColor = c;
#elif defined mode_max
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c = max(c, texture2D(layers[i], coord));
    gl_FragColor = c;
#elif defined mode_median
    vec4 c[n];
    for (int i = 0; i < n; i++)
	c[i] = texture2D(layers[i], coord);
    for (int a = 0; a < n; a++)
    { 
	for (int b = 0; b < n - 1; b++)
	{ 
	    if (c[b].r > c[b + 1].r)
	    {
		float tmp = c[b + 1].r;
		c[b + 1].r = c[b].r;
		c[b].r = tmp;
	    }
	    if (c[b].g > c[b + 1].g)
	    {
		float tmp = c[b + 1].g;
		c[b + 1].g = c[b].g;
		c[b].g = tmp;
	    }
	    if (c[b].b > c[b + 1].b)
	    {
		float tmp = c[b + 1].b;
		c[b + 1].b = c[b].b;
		c[b].b = tmp;
	    }
	    if (c[b].a > c[b + 1].a)
	    {
		float tmp = c[b + 1].a;
		c[b + 1].a = c[b].a;
		c[b].a = tmp;
	    }
	} 
    } 
    gl_FragColor = c[n / 2];
#elif defined mode_or
    ivec4 c = ivec4(texture2D(layers[0], coord) * 255.0);
    for (int i = 1; i < n; i++)
    {
	ivec4 t = ivec4(texture2D(layers[i], coord) * 255.0);
	c.r = bitwise_or(c.r, t.r);
	c.g = bitwise_or(c.g, t.g);
	c.b = bitwise_or(c.b, t.b);
    }
    gl_FragColor = vec4(c);
#elif defined mode_and
    ivec4 c = ivec4(texture2D(layers[0], coord) * 255.0);
    for (int i = 1; i < n; i++)
    {
	ivec4 t = ivec4(texture2D(layers[i], coord) * 255.0);
	c.r = bitwise_and(c.r, t.r);
	c.g = bitwise_and(c.g, t.g);
	c.b = bitwise_and(c.b, t.b);
    }
    gl_FragColor = vec4(c);
#elif defined mode_xor
    ivec4 c = ivec4(texture2D(layers[0], coord) * 255.0);
    for (int i = 1; i < n; i++)
    {
	ivec4 t = ivec4(texture2D(layers[i], coord) * 255.0);
	c.r = bitwise_xor(c.r, t.r);
	c.g = bitwise_xor(c.g, t.g);
	c.b = bitwise_xor(c.b, t.b);
    }
    gl_FragColor = vec4(c);
#elif defined mode_diff
    vec4 cmin = texture2D(layers[0], coord);
    vec4 cmax = cmin;
    for (int i = 1; i < n; i++)
    {
	vec4 t = texture2D(layers[i], coord);
	cmin = min(t, cmin);
	cmax = max(t, cmax);
    }
    gl_FragColor = cmax - cmin;
#elif defined mode_add
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c += texture2D(layers[i], coord);
    gl_FragColor = c;
#elif defined mode_xadd
    vec4 c = texture2D(layers[0], coord) / n;
    for (int i = 1; i < n; i++)
	c += texture2D(layers[i], coord) / float(n);
    gl_FragColor = c;
#elif defined mode_sub
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c -= texture2D(layers[i], coord);
    gl_FragColor = c;
#elif defined mode_xsub
    vec4 c = texture2D(layers[0], coord) / n;
    for (int i = 1; i < n; i++)
	c = c + 1.0 / float(n) - texture2D(layers[i], coord) / float(n);
    gl_FragColor = c;
#elif defined mode_mul
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c *= texture2D(layers[i], coord);
    gl_FragColor = c;
#elif defined mode_div
    vec4 c = texture2D(layers[0], coord);
    for (int i = 1; i < n; i++)
	c /= texture2D(layers[i], coord);
    gl_FragColor = c;
#endif
}
