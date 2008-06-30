/*
 * glvm-color.h
 * 
 * C++ vector and matrix classes that resemble GLSL style.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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

/*
 * This file provides functions to convert between color spaces using GLVM
 * data types.
 *
 * We use the D65 reference white for the RGB values.
 */

#ifndef GLVM_COLOR_H
#define GLVM_COLOR_H

#include "glvm.h"

namespace glvm
{
    /* Luminance */

    inline vec3 lum_to_xyz(const float &lum)
    {
	const float d65_x = 0.31271f;
	const float d65_y = 0.32902f;
	vec3 xyz;
	xyz.y = lum;
	xyz.x = xyz.y * (d65_x / d65_y);
	xyz.z = min(1.0f, xyz.y * (1.0f - d65_x - d65_y) / d65_y);
	return xyz;
    }

    inline float xyz_to_lum(const vec3 &xyz)
    {
	return xyz.y;
    }

    inline vec3 lum_to_rgb(const float &lum)
    {
	const mat3 M( 3.240708f, -1.537259f, -0.498570f,
   		     -0.969257f,  1.875995f,  0.041555f,
		      0.055636f, -0.203996f,  1.057069f);
	vec3 xyz = lum_to_xyz(lum);
	vec3 rgb = xyz * M;
	for (int i = 0; i < 3; i++)
	{
	    rgb[i] = (rgb[i] <= 0.0031308f ? rgb[i] * 12.92f : 1.055f * powf(rgb[i], 1.0f / 2.4f) - 0.055f);
	}
	return rgb;
    }

    inline float rgb_to_lum(const vec3 &rgb)
    {
	vec3 xrgb;
	for (int i = 0; i < 3; i++)
	{
	    xrgb[i] = (rgb[i] <= 0.04045f ? rgb[i] / 12.92f : powf((rgb[i] + 0.055f) / 1.055f, 2.4f));
	}
    	return dot(vec3(0.212656f, 0.715158f, 0.072186f), xrgb);
    }

    /* XYZ */

    inline vec3 xyz_to_rgb(const vec3 &xyz)
    {
	const mat3 M( 3.240708f, -1.537259f, -0.498570f,
   		     -0.969257f,  1.875995f,  0.041555f,
		      0.055636f, -0.203996f,  1.057069f);
	vec3 rgb = xyz * M;
	for (int i = 0; i < 3; i++)
	{
	    rgb[i] = (rgb[i] <= 0.0031308f ? rgb[i] * 12.92f : 1.055f * powf(rgb[i], 1.0f / 2.4f) - 0.055f);
	}
	return rgb;
    }

    inline vec3 rgb_to_xyz(const vec3 &rgb)
    {
	const mat3 M(0.412424f, 0.357579f, 0.180464f,
      		     0.212656f, 0.715158f, 0.072186f,
		     0.019332f, 0.119193f, 0.950444f);
	vec3 xrgb;
	for (int i = 0; i < 3; i++)
	{
	    xrgb[i] = (rgb[i] <= 0.04045f ? rgb[i] / 12.92f : powf((rgb[i] + 0.055f) / 1.055f, 2.4f));
	}
	return xrgb * M;
    }

    /* HSL */

    inline float _hsl_to_rgb_helper(const float tmp2, const float tmp1, const float h)
    {
	float H = h;
	float ret;

	if (H < 0.0f)
	    H += 1.0f;
	else if (H > 1.0f)
	    H -= 1.0f;

	if (H < 1.0f / 6.0f)
	    ret = (tmp2 + (tmp1 - tmp2) * (360.0f / 60.0f) * H);
	else if (H < 1.0f / 2.0f)
	    ret = tmp1;
	else if (H < 2.0f / 3.0f)
	    ret = (tmp2 + (tmp1 - tmp2) * ((2.0f / 3.0f) - H) * (360.0f / 60.0f));
	else
	    ret = tmp2;
	return ret;
    }

    inline vec3 hsl_to_rgb(const vec3 &hsl)
    {
	vec3 rgb;
	float tmp1, tmp2;

	if (hsl[2] < 0.5f)
	    tmp1 = hsl[2] * (1.0f + hsl[1]);
	else
	    tmp1 = (hsl[2] + hsl[1]) - (hsl[2] * hsl[1]);
	tmp2 = 2.0f * hsl[2] - tmp1;
	rgb.r = _hsl_to_rgb_helper(tmp2, tmp1, hsl[0] + (1.0f / 3.0f));
	rgb.g = _hsl_to_rgb_helper(tmp2, tmp1, hsl[0]);
	rgb.b = _hsl_to_rgb_helper(tmp2, tmp1, hsl[0] - (1.0f / 3.0f));
	return rgb;
    }

    inline vec3 rgb_to_hsl(const vec3 &rgb)
    {
	vec3 hsl;
	const float minval = min(min(rgb.r, rgb.g), rgb.b);
	const float maxval = max(max(rgb.r, rgb.g), rgb.b);
	const float delta = maxval - minval;

	hsl.z = (maxval + minval) / 2.0f;
	if (abs(maxval - minval) < FLT_EPSILON)
	{
	    hsl.x = 0.0f;
	    hsl.y = 0.0f;
	}
	else
	{
	    hsl.y = delta / ((hsl.z <= 0.5f) ? (maxval + minval) : (2.0f - maxval - minval));
	    if (abs(maxval - rgb.r) < FLT_EPSILON)
	    {
		hsl.x = (60.0f / 360.0f) * (rgb.g - rgb.b) / (maxval - minval);
		if (rgb.g < rgb.b)
		    hsl.x += 360.0f / 360.0f;
	    }
	    else if (abs(maxval - rgb.g) < FLT_EPSILON)
	    {
		hsl.x = (60.0f / 360.0f) * (rgb.b - rgb.r) / (maxval - minval) + (120.0f / 360.0f);
	    }
	    else
	    {
		hsl.x = (60.0f / 360.0f) * (rgb.r - rgb.g) / (maxval - minval) + (240.0f / 360.0f);
	    }
	}
	return hsl;
    }
}

#endif
