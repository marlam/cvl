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

#include "glsl/hdr/tonemap_schlick94.glsl.h"
#include "glsl/hdr/tonemap_tumblin99_step1.glsl.h"
#include "glsl/hdr/tonemap_tumblin99_step2.glsl.h"
#include "glsl/hdr/tonemap_drago03.glsl.h"
#include "glsl/hdr/tonemap_durand02_step1.glsl.h"
#include "glsl/hdr/tonemap_durand02_step2.glsl.h"


/**
 * \param dst			The destination frame.
 * \param src			The source frame.
 * \param p			Parameter.
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

    GLuint prg;
    float world_adaptation_level;

    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_tumblin99_step1")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_tumblin99_step1", NULL, 
		CVL_TONEMAP_TUMBLIN99_STEP1_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_tumblin99_step1", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "max_abs_lum"), max_abs_lum);
    cvl_frame_t *tmp = cvl_frame_new(cvl_frame_width(src), cvl_frame_height(src), 
	    1, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
    cvl_transform(tmp, src);
    cvl_reduce(tmp, CVL_REDUCE_SUM, 0, &world_adaptation_level);
    world_adaptation_level = expf(world_adaptation_level / (float)cvl_frame_size(src));
    cvl_frame_free(tmp);

    float gamma_d = cvl_tonemap_tr_gamma(display_adaptation_level);
    float gamma_w = cvl_tonemap_tr_gamma(world_adaptation_level);
    float gamma_wd = gamma_w / (1.855f + 0.4f * logf(display_adaptation_level) / logf(10.0f));
    float m = powf(sqrtf(max_displayable_contrast), gamma_wd - 1.0f);
    if ((prg = cvl_gl_program_cache_get("cvl_tonemap_tumblin99_step2")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_tonemap_tumblin99_step2", NULL, 
		CVL_TONEMAP_TUMBLIN99_STEP2_GLSL_STR);
	cvl_gl_program_cache_put("cvl_tonemap_tumblin99_step2", prg);
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
