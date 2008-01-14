/*
 * cvl_hdr.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

/** 
 * \file cvl_hdr.h
 * \brief High dynamic range frames.
 *
 * Handling High Dynamic Range (HDR) frames.\n
 * All tone mapping operators (TMOs) take normalized #CVL_XYZ frames as input. If you
 * have #CVL_XYZ frames with absolute values, you need to convert them with
 * cvl_luminance_range() first. TMOs that work on absolute luminance values have
 * an additional parameter called max_abs_lum. If the maximum absolute luminance 
 * is known, this parameter can be set accordingly. If the maximum absolute luminance
 * is not known, the parameter can be used to prescale the values to a suitable
 * range.
 */

#include "config.h"

#include <math.h>
#include <float.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_misc.h"
#include "cvl/cvl_hdr.h"

#include "glsl/hdr/log_avg_lum.glsl.h"
#include "glsl/hdr/tonemap_schlick94.glsl.h"
#include "glsl/hdr/tonemap_tumblin99.glsl.h"
#include "glsl/hdr/tonemap_drago03.glsl.h"
#include "glsl/hdr/tonemap_reinhard05.glsl.h"
#include "glsl/hdr/tonemap_durand02_step1.glsl.h"
#include "glsl/hdr/tonemap_durand02_step2.glsl.h"
#include "glsl/hdr/tonemap_reinhard02_step1.glsl.h"
#include "glsl/hdr/tonemap_reinhard02_step2.glsl.h"


/**
 * \param frame		The frame.
 * \param max_abs_lum	Maximum absolute luminance.
 * \return	The log average luminance.
 *
 * Computes the log average luminance of the given frame, with respect to the
 * given maximum absolute luminance.
 */
float cvl_log_avg_lum(cvl_frame_t *frame, float max_abs_lum)
{
    cvl_assert(frame != NULL);
    cvl_assert(cvl_frame_format(frame) == CVL_XYZ);
    cvl_assert(max_abs_lum > 0.0f);
    if (cvl_error())
	return 0.0f;

    float log_avg_lum;
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_log_avg_lum")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_log_avg_lum", NULL, 
		CVL_LOG_AVG_LUM_GLSL_STR);
	cvl_gl_program_cache_put("cvl_log_avg_lum", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "max_abs_lum"), max_abs_lum);
    cvl_frame_t *tmp = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame), 
	    1, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
    cvl_transform(tmp, frame);
    cvl_reduce(tmp, CVL_REDUCE_SUM, 0, &log_avg_lum);
    log_avg_lum = expf(log_avg_lum / (float)cvl_frame_size(frame));
    cvl_frame_free(tmp);
    return log_avg_lum;
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param p		Parameter.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * The parameter \a p must be greater than or equal to 1.0.\n
 * See also section 7.2.9 in 
 * E. Reinhard and G. Ward and S. Pattanaik and P. Debevec, 
 * High Dynamic Range Imaging: Acquisition, Display and Image-based Lighting,
 * Morgan Kaufmann, 2005, ISBN 0-12-585263-0
 */
void cvl_tonemap_schlick94(cvl_frame_t *dst, cvl_frame_t *src, float p)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(p >= 1.0f);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_schlick94")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_schlick94", NULL, 
		CVL_TONEMAP_SCHLICK94_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_schlick94", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "p"), p);
    cvl_transform(dst, src);

    cvl_check_errors();
}


// Helper function
static float cvl_tonemap_tr_gamma(float L)
{
    return (L > 100.0f) ? 2.655f : 1.855f + 0.4f * logf(L + 2.3e-5f) / logf(10.0f);
}

/**
 * \param dst				The destination frame.
 * \param src				The source frame.
 * \param max_abs_lum			Maximum absolute luminance.
 * \param display_adaptation_level	Display adaptation level.
 * \param max_displayable_contrast	Maximum displayable contrast.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * The \a display_adaptation_level parameter must be greater than zero.
 * The \a max_displayable_contrast parameter must be greater than zero.\n
 * See also section 7.2.2 in 
 * E. Reinhard and G. Ward and S. Pattanaik and P. Debevec, 
 * High Dynamic Range Imaging: Acquisition, Display and Image-based Lighting,
 * Morgan Kaufmann, 2005, ISBN 0-12-585263-0
 */
void cvl_tonemap_tumblin99(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, 
	float display_adaptation_level, float max_displayable_contrast)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(max_abs_lum > 0.0f);
    cvl_assert(display_adaptation_level > 0.0f);
    cvl_assert(max_displayable_contrast > 0.0f);
    if (cvl_error())
	return;

    float world_adaptation_level = cvl_log_avg_lum(src, max_abs_lum);
    float gamma_d = cvl_tonemap_tr_gamma(display_adaptation_level);
    float gamma_w = cvl_tonemap_tr_gamma(world_adaptation_level);
    float gamma_wd = gamma_w / (1.855f + 0.4f * logf(display_adaptation_level) / logf(10.0f));
    float m = powf(sqrtf(max_displayable_contrast), gamma_wd - 1.0f);
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_tumblin99")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_tumblin99", NULL, 
		CVL_TONEMAP_TUMBLIN99_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_tumblin99", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "max_abs_lum"), max_abs_lum);
    glUniform1f(glGetUniformLocation(prg, "Lwa"), world_adaptation_level);
    glUniform1f(glGetUniformLocation(prg, "Lda"), display_adaptation_level);
    glUniform1f(glGetUniformLocation(prg, "m"), m);
    glUniform1f(glGetUniformLocation(prg, "gamma_w"), gamma_w);
    glUniform1f(glGetUniformLocation(prg, "gamma_d"), gamma_d);
    cvl_transform(dst, src);

    cvl_check_errors();
}


/**
 * \param dst			The destination frame.
 * \param src			The source frame.
 * \param max_abs_lum		Maximum absolute luminance.
 * \param bias			Bias.
 * \param max_disp_lum		Maximum display luminance.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * The \a bias parameter must be from [0,1].
 * The \a max_disp_lum parameter must be greater than zero.\n
 * See also:
 * F. Drago, K. Myszkowski, T. Annen and N. Chiba,
 * Adaptive Logarithmic Mapping For Displaying High Contrast Scenes.
 * Proc. Eurographics 2003.
 */
void cvl_tonemap_drago03(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, float bias, float max_disp_lum)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(max_abs_lum > 0.0f);
    cvl_assert(bias >= 0.0f && bias <= 1.0f);
    cvl_assert(max_disp_lum > 0.0f);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_drago03")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_drago03", NULL, 
		CVL_TONEMAP_DRAGO03_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_drago03", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "max_abs_lum"), max_abs_lum);
    glUniform1f(glGetUniformLocation(prg, "factor"), (max_disp_lum / 100.0f) / logf(1.0f + max_abs_lum));
    glUniform1f(glGetUniformLocation(prg, "bias_cooked"), logf(bias) / logf(0.5f));
    cvl_transform(dst, src);

    cvl_check_errors();
}


/**
 * \param dst			The destination frame.
 * \param src			The source frame.
 * \param min_lum		Minimum luminance value in \a src.
 * \param avg_lum		Average luminance value in \a src.
 * \param log_avg_lum		Log average luminance value in \a src.
 * \param rgb			A RGB version of \a src.
 * \param channel_avg		Average channel values in \a rgb.
 * \param f			Brightness parameter.
 * \param c			Chromatic adaptation parameter.
 * \param l			Light adaptation parameter.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * This function needs some information that needs to be computed from the
 * source frame \a src: \a min_lum, \a avg_lum, \a log_avg_lum, a #CVL_RGB
 * version of \a src, and the average RGB channel values.\n
 * The \a f parameter must be from [-8,8] (default: 0).
 * The \a c parameter must be from [0,1] (default: 0).
 * The \a l parameter must be from [0,1] (default: 1).
 * * The \a max_disp_lum parameter must be greater than zero.\n
 * See also:
 * E. Reinhard and K. Devlin\n
 * Dynamic range reduction inspired by photoreceptor physiology\m
 * Transactions on Visualization and Computer Graphics, Volume 11, Issue 1,
 * Jan.-Feb. 2005, pp 13-24.
 */
void cvl_tonemap_reinhard05(cvl_frame_t *dst, cvl_frame_t *src, 
	float min_lum, float avg_lum, float log_avg_lum,
	cvl_frame_t *rgb, const float channel_avg[3],
	float f, float c, float l)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(f >= -8.0f && f <= 8.0f);
    cvl_assert(c >= 0.0f && c <= 1.0f);
    cvl_assert(l >= 0.0f && l <= 1.0f);
    if (cvl_error())
	return;

    GLuint prg;

    float max_lum = 1.0f;
    float m;
    float I_a_global[3];

    m = 0.3f + 0.7f * powf((logf(max_lum) - logf(log_avg_lum)) / (logf(max_lum) - logf(min_lum)), 1.4f);
    I_a_global[0] = c * channel_avg[0] + (1.0f - c) * avg_lum;
    I_a_global[1] = c * channel_avg[1] + (1.0f - c) * avg_lum;
    I_a_global[2] = c * channel_avg[2] + (1.0f - c) * avg_lum;

    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_reinhard05")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_reinhard05", NULL, 
		CVL_TONEMAP_REINHARD05_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_reinhard05", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "f"), expf(-f));
    glUniform1f(glGetUniformLocation(prg, "c"), c);
    glUniform1f(glGetUniformLocation(prg, "l"), l);
    glUniform1f(glGetUniformLocation(prg, "m"), m);
    glUniform1f(glGetUniformLocation(prg, "min_lum"), min_lum);
    glUniform1f(glGetUniformLocation(prg, "max_lum"), max_lum);
    glUniform3fv(glGetUniformLocation(prg, "I_a_global"), 1, I_a_global);
    cvl_transform(dst, rgb);
    cvl_frame_set_format(dst, CVL_RGB);
    cvl_convert_format_inplace(dst, CVL_XYZ);

    cvl_check_errors();
}


/**
 * \param dst			The destination frame.
 * \param src			The source frame.
 * \param max_abs_lum		Maximum absolute luminance.
 * \param k			Mask size 2*k+1 x 2*k+1.
 * \param sigma_spatial		Spatial sigma.
 * \param sigma_luminance	Luminance sigma.
 * \param base_contrast		Base contrast.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * The sigma values must be greater than zero, and the \a base_contrast
 * parameter must be grater than 1.\n
 * See also:
 * F. Durand and J. Dorsey, Fast Bilateral Filtering for the Display of 
 * High-Dynamic-Range Images, Proc. ACM SIGGRAPH 2002, pp. 257-266.
 */
void cvl_tonemap_durand02(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, 
	int k, float sigma_spatial, float sigma_luminance, float base_contrast)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(max_abs_lum > 0.0f);
    cvl_assert(k >= 0);
    cvl_assert(sigma_spatial > 0.0f);
    cvl_assert(sigma_luminance > 0.0f);
    cvl_assert(base_contrast > 1.0f);
    if (cvl_error())
	return;

    // Bilateral filtering is not separable. We implement it directly, which
    // limits us to mask sizes of up to 9x9. Larger masks cannot be handled by
    // current graphics hardware. But it turns out that we only need 3x3 or 5x5
    // masks for this method, so this should be tolerable.
    //
    // The following paper suggests that separating the bilateral filter is
    // good enough as an approximation in many cases:
    // "T. Q. Pham and L. J. van Vliet, Separable Bilateral Filtering for Fast
    //  Video Preprocessing, Proc. ICME 2005"
    // I tried it, but it gives very bad results in this context. The effects 
    // of forced separation are clearly visible.
    
    GLuint prg;
    char *prg_name;

    float *mask;
    if (!(mask = malloc((2 * k + 1) * sizeof(float))))
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
	return;
    }
    mh_gauss_mask(k, sigma_spatial, mask, NULL);

    prg_name = cvl_asprintf("cvl_tonemap_durand02_step1_k=%d", k);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_TONEMAP_DURAND02_STEP1_GLSL_STR), "$k=%d", k);
	prg = cvl_gl_program_new_src(prg_name, NULL, src);
	cvl_gl_program_cache_put(prg_name, prg);
	free(src);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)cvl_frame_height(src));
    glUniform1fv(glGetUniformLocation(prg, "mask"), 2 * k + 1, mask);
    glUniform1f(glGetUniformLocation(prg, "max_abs_lum"), max_abs_lum);
    glUniform1f(glGetUniformLocation(prg, "sigma_luminance"), sigma_luminance);
    cvl_frame_t *tmp = cvl_frame_new(cvl_frame_width(src), cvl_frame_height(src), 
	    4, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
    cvl_transform(tmp, src);

    float min_log_base, max_log_base;
    cvl_reduce(tmp, CVL_REDUCE_MIN, 1, &min_log_base);
    cvl_reduce(tmp, CVL_REDUCE_MAX, 1, &max_log_base);
    float compression_factor = logf(base_contrast) / (max_log_base - min_log_base);
    if (!isfinite(compression_factor))
    {
	compression_factor = FLT_MAX;
    }
    float log_absolute_scale = min_log_base * compression_factor;

    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_durand02_step2")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_durand02_step2", NULL, 
		CVL_TONEMAP_DURAND02_STEP2_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_durand02_step2", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "compression_factor"), compression_factor);
    glUniform1f(glGetUniformLocation(prg, "log_absolute_scale"), log_absolute_scale);
    cvl_transform(dst, tmp);
    cvl_frame_free(tmp);

    cvl_check_errors();
}


/**
 * \param dst			The destination frame.
 * \param src			The source frame.
 * \param tmp			A frame for temporary results.
 * \param log_avg_lum		The log-average luminance of \a src.
 * \param brightness		Brightness parameter.
 * \param white			Lowest value that will be mapped to white.
 * \param sharpness		Sharpness parameter.
 * \param threshold		Threshold parameter.
 *
 * Applies tone mapping to the high dynamic range frame \a src and writes the
 * result to \a dst. Input and output must be in #CVL_XYZ format.\n
 * The temporary frame \a tmp must have four channels of type #CVL_FLOAT.
 * The \a log_avg_lum parameter must be the log-average luminance of \a src
 * (unscaled).
 * The \a brightness parameter must be from [0,1].
 * The \a white parameter must be from [0,100).
 * The \a sharpness parameter must be from [0,100).
 * The \a threshold parameter must be from [0,1].\n
 * See also:
 * E. Reinhard and M. Stark and P. Shirley and J. Ferwerda.
 * Photographic Tone Reproduction for Digital Images.
 * Proc. ACM SIGGRAPH 2002, pp. 267-276.
 */
void cvl_tonemap_reinhard02(cvl_frame_t *dst, cvl_frame_t *src, 
	cvl_frame_t *tmp, float log_avg_lum,
	float brightness, float white, float sharpness, float threshold)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(brightness >= 0.0f && brightness <= 1.0f);
    cvl_assert(white >= 0.0f && white < 100.0f);
    cvl_assert(sharpness >= 0.0f && sharpness < 100.0f);
    cvl_assert(threshold >= 0.0f && threshold <= 1.0f);
    if (cvl_error())
	return;

    GLuint prg;
    const int k[4] = { 3, 6, 9, 12 };
    const float sigma[4] = { 2.4f, 4.8f, 7.2f, 9.6f };
    float mask0[2 * k[0] + 1];
    float mask0_weightsum;
    float mask1[2 * k[1] + 1];
    float mask1_weightsum;
    float mask2[2 * k[2] + 1];
    float mask2_weightsum;
    float mask3[2 * k[3] + 1];
    float mask3_weightsum;

    mh_gauss_mask(k[0], sigma[0], mask0, &mask0_weightsum);
    mh_gauss_mask(k[1], sigma[1], mask1, &mask1_weightsum);
    mh_gauss_mask(k[2], sigma[2], mask2, &mask2_weightsum);
    mh_gauss_mask(k[3], sigma[3], mask3, &mask3_weightsum);
    
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_reinhard02_step1")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_reinhard02_step1", NULL, 
		CVL_TONEMAP_REINHARD02_STEP1_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_reinhard02_step1", prg);
    }
    glUseProgram(prg);
    glUniform1fv(glGetUniformLocation(prg, "mask_0"), 2 * k[0] + 1, mask0);
    glUniform1fv(glGetUniformLocation(prg, "mask_1"), 2 * k[1] + 1, mask1);
    glUniform1fv(glGetUniformLocation(prg, "mask_2"), 2 * k[2] + 1, mask2);
    glUniform1fv(glGetUniformLocation(prg, "mask_3"), 2 * k[3] + 1, mask3);
    glUniform1f(glGetUniformLocation(prg, "factor_0"), 1.0f / mask0_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_1"), 1.0f / mask1_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_2"), 1.0f / mask2_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_3"), 1.0f / mask3_weightsum);
    glUniform1f(glGetUniformLocation(prg, "xstep"), 1.0f / (float)cvl_frame_width(src));
    cvl_transform(tmp, src);
    
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_reinhard02_step2")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_reinhard02_step2", NULL, 
		CVL_TONEMAP_REINHARD02_STEP2_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_reinhard02_step2", prg);
    }
    glUseProgram(prg);
    glUniform1fv(glGetUniformLocation(prg, "mask_0"), 2 * k[0] + 1, mask0);
    glUniform1fv(glGetUniformLocation(prg, "mask_1"), 2 * k[1] + 1, mask1);
    glUniform1fv(glGetUniformLocation(prg, "mask_2"), 2 * k[2] + 1, mask2);
    glUniform1fv(glGetUniformLocation(prg, "mask_3"), 2 * k[3] + 1, mask3);
    glUniform1f(glGetUniformLocation(prg, "factor_0"), 1.0f / mask0_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_1"), 1.0f / mask1_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_2"), 1.0f / mask2_weightsum);
    glUniform1f(glGetUniformLocation(prg, "factor_3"), 1.0f / mask3_weightsum);
    glUniform1f(glGetUniformLocation(prg, "s_0"), sigma[0]);
    glUniform1f(glGetUniformLocation(prg, "s_1"), sigma[1]);
    glUniform1f(glGetUniformLocation(prg, "s_2"), sigma[2]);
    glUniform1f(glGetUniformLocation(prg, "s_3"), sigma[3]);
    glUniform1f(glGetUniformLocation(prg, "ystep"), 1.0f / (float)cvl_frame_height(tmp));
    glUniform1f(glGetUniformLocation(prg, "log_avg_lum"), log_avg_lum);
    glUniform1f(glGetUniformLocation(prg, "brightness"), brightness);
    glUniform1f(glGetUniformLocation(prg, "white"), white);
    glUniform1f(glGetUniformLocation(prg, "sharpness"), sharpness);
    glUniform1f(glGetUniformLocation(prg, "threshold"), threshold);
    cvl_frame_t *srcs[2] = { src, tmp };
    cvl_transform_multi(&dst, 1, srcs, 2, "textures");

    cvl_check_errors();
}
