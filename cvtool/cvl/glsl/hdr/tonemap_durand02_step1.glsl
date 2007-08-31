/*
 * tonemap_durand02_step1.glsl
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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

const int k = $k;
const float PI = 3.14159265358979323846;
uniform float max_abs_lum;
uniform float step_h;
uniform float step_v;
uniform float mask[2 * k + 1];
uniform float sigma_luminance;
uniform sampler2D tex;

void main()
{
    // Assume XYZ color space.
    vec3 color = texture2D(tex, gl_TexCoord[0].xy).rgb;
    float log_input_intensity = log(max_abs_lum * color.g + 1.0);
    float log_base = 0.0;
    float weight_sum = 0.0;
    for (int r = -k; r <= +k; r++)
    {
	for (int c = -k; c <= +k; c++)
	{
	    float log_current_intensity = log(max_abs_lum 
		    * texture2D(tex, gl_TexCoord[0].xy + vec2(float(c) * step_h, float(r) * step_v)).g + 1.0);
	    float factor_gauss_spatial = mask[c + k] * mask[r + k];
	    float luminance_diff = log_input_intensity - log_current_intensity;
	    float factor_gauss_luminance = exp(- (luminance_diff * luminance_diff) 
		    / (2.0 * sigma_luminance * sigma_luminance)) 
		/ (sqrt(2.0 * PI) * sigma_luminance);
	    float weight = factor_gauss_spatial * factor_gauss_luminance;
	    weight_sum += weight;
	    log_base += weight * log_current_intensity;
	}
    }
    log_base /= weight_sum;

    float x = color.r / (color.r + color.g + color.b);
    float y = color.g / (color.r + color.g + color.b);
    gl_FragColor = vec4(log_input_intensity, log_base, x, y);
}
