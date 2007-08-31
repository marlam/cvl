/*
 * cmd_draw.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006, 2007  Martin Lambers <marlam@marlam.de>
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

#include "config.h"

#if !HAVE_LIBCAIRO

#include "mh.h"

void cmd_draw_print_help(void)
{
    mh_msg_fmt_req("This version of " PACKAGE_NAME " was compiled without support for the draw command.");
}

int cmd_draw(int argc UNUSED, char *argv[] UNUSED)
{
    mh_msg_fmt_err("This version of " PACKAGE_NAME " was compiled without support for the draw command.");
    return 1;
}

#else // HAVE_LIBCAIRO

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <errno.h>

#include <cairo.h>

#include <cvl/cvl.h>

#include "mh.h"

#include "striconv.h"
#include "localcharset.h"


void cmd_draw_print_help(void)
{
    mh_msg_fmt_req(
	    "draw "
	    "[-w|--width=<w>] "
	    "[-d|--dash=<on0,off0,...>] "
	    "[-l|--line-cap=butt|round|square] "
	    "[-L|--line-join=miter|round|bevel] "
	    "[-s|--border-style=none|color|pattern|multipattern|linear-gradient|radial-gradient] "
	    "[-c|--border-color=<c>] "
	    "[-p|--border-pattern=<file>] "
	    "[-g|--border-gradient=<gradient>] "
	    "[-S|--fill-style=none|color|pattern|multipattern|linear-gradient|radial-gradient] "
	    "[-C|--fill-color=<C>] "
	    "[-P|--fill-pattern=<file>] "
	    "[-G|--fill-gradient=<gradient>] "
	    "[-f|--font-family=<font>] "
	    "[-t|--font-slant=normal|italic|oblique] "
	    "[-W|--font-weight=normal|bold] "
	    "[-F|--font-size=<size>|<size-x,size-y>] "
	    "[-j|--justify-x=left|right|center] "
	    "[-J|--justify-y=bottom|top|center] "
	    "[-a|--antialias] "
	    "[-u|--unit] "
	    "<command>...\n"
	    "\n"
	    "Draws simple geometric forms, lines and curves, and/or text using the given styles for "
	    "the border/line and the filling and text justification. See the main documentation for "
	    "a description of the options.\n"
	    "A drawing command consists of a command name and parameter sets that define one "
	    "or more instances of the command.\n"
	    "Simple geometric forms:\n"
	    "rectangle    <x,y,w,h> [...]\n"
	    "circle       <x,y,r> [...]\n"
	    "ellipse      <x,y,w,h> [...]\n"
	    "arc          <x,y,r,a,b> [...]\n"
	    "Text:\n"
	    "text         <string>\n"
	    "Lines and curves:\n"
	    "move_to      <x,y>\n"
	    "line_to      <x,y> [...]\n"
	    "curve_to     <x0,y0,x1,y1,x2,y2> [...]\n"
	    "rel_move_to  <dx,dy>\n"
	    "rel_line_to  <dx,dy> [...]\n"
	    "rel_curve_to <dx0,dy0,dx1,dy1,dx2,dy2> [...]\n"
	    "close\n"
	    "Lines and curves will implicitly be closed when drawing geometric forms or text.");
}


/* Converts a string from the character set of the users locale to UTF-8.
 * This function returns a pointer to an allocated string, or NULL if the
 * conversion fails and default_value is NULL. If the conversion fails and
 * default_value is not NULL, then the allocated string will be a duplicate of
 * the default_value string.
 * This function tries to determine the users character set regardless of the
 * current locale settings of this process, so that it works as expected for
 * programs that do not use setlocale() at all. */
char *string_to_utf8(const char *s, const char *default_value)
{
    char *locale_bak;
    char *result;
    
    locale_bak = setlocale(LC_CTYPE, NULL);
    if (locale_bak)
    {
	locale_bak = mh_strdup(locale_bak);
	setlocale(LC_CTYPE, "");
    }
    if (!(result = str_iconv(s, locale_charset(), "UTF-8")))
    {
	if (default_value)
	{
	    result = mh_strdup(default_value);
	}
    }
    if (locale_bak)
    {
	setlocale(LC_CTYPE, locale_bak);
	free(locale_bak);
    }
    return result;
}


/* Data structures and functions to parse the list of drawing commands from the
 * command line. Note that the "text" command is treated separately because it
 * does not take a list of doubles as argument. */

/* The type of drawing command. */
typedef enum { 
    DRAW_ARC, DRAW_CIRCLE, DRAW_CLOSE, DRAW_CURVE_TO, DRAW_ELLIPSE, DRAW_LINE_TO, 
    DRAW_MOVE_TO, DRAW_RECTANGLE, DRAW_REL_CURVE_TO, DRAW_REL_LINE_TO, DRAW_REL_MOVE_TO,
    DRAW_TEXT
} draw_cmd_type_t;

/* Specification of a drawing command. */
typedef struct
{
    const char *name;		// The command name    
    draw_cmd_type_t type;	// The command constant
    int params_len;		// The number of parameters
} draw_cmd_t;

/* List of all known drawing commands. */
static draw_cmd_t draw_cmd_table[] = 
{
    // Keep this table in ascending order according to strcmp(), because it is
    // used with bsearch()
    { "arc",          DRAW_ARC,          5 },
    { "circle",       DRAW_CIRCLE,       3 },
    { "close",        DRAW_CLOSE,        0 },
    { "curve_to",     DRAW_CURVE_TO,     6 },
    { "ellipse",      DRAW_ELLIPSE,      4 },
    { "line_to",      DRAW_LINE_TO,      2 },
    { "move_to",      DRAW_MOVE_TO,      2 },
    { "rectangle",    DRAW_RECTANGLE,    4 },
    { "rel_curve_to", DRAW_REL_CURVE_TO, 6 },
    { "rel_line_to",  DRAW_REL_LINE_TO,  2 },
    { "rel_move_to",  DRAW_REL_MOVE_TO,  2 },
    { "text",         DRAW_TEXT,         1 }
};

static const size_t draw_cmd_table_len = sizeof(draw_cmd_table) / sizeof(draw_cmd_table[0]);

static int draw_cmd_cmp(const draw_cmd_t *e1, const draw_cmd_t *e2)
{
    return strcmp(e1->name, e2->name);
}

/* Get parameters (= a list of zero or more comma separated double values) for
 * the command cmd from the string s. d is expected to have enough space. */
static bool get_params(const draw_cmd_t *cmd, const char *s, double *d)
{
    const char *p;
    char *q;
    double value;
    
    if (cmd->params_len > 0 && !s)
    {
	mh_msg_err("missing parameters for %s", cmd->name);
	return false;
    }
    p = s;
    for (int i = 0; i < cmd->params_len; i++)
    {
	errno = 0;
	value = strtod(p, &q);
	if (q == p || *q != (i == cmd->params_len - 1 ? '\0' : ',') || errno == ERANGE) 
	{
	    mh_msg_err("invalid parameters %s for %s", s, cmd->name);
	    return false;
	}
	d[i] = value;
	p = q + 1;
    }
    return true;
}

/* Parse the drawing ocmmand from the command line into a command list cmd_list, 
 * and a parameter list params_list. The command line must contain only drawing
 * commands. The number of commands will be stored in cmd_list_len. The arguments
 * to the "text" command are strings instead of doubles; they are stored in 
 * string_list. */
static bool parse_draw_cmds(char *argv[], 
	draw_cmd_t ***cmd_list, int *cmd_list_len, 
	double **params_list, char ***string_list)
{
    typedef enum { CMD, PARAMS } state_t;
    state_t state = CMD;
    int arg = 0;
    draw_cmd_t key;
    draw_cmd_t *current_cmd = NULL;
    *cmd_list_len = 0;
    int params_list_len = 0;
    int string_list_len = 0;
    bool error = false;
    
    *cmd_list = NULL;
    *params_list = NULL;
    *string_list = NULL;
    while (!error)
    {
	if (state == CMD)
	{
    	    key.name = argv[arg];
	    current_cmd = bsearch(&key, draw_cmd_table, draw_cmd_table_len, sizeof(draw_cmd_t),
		    (int (*)(const void *, const void *))draw_cmd_cmp);
    	    if (!current_cmd)
	    {
		mh_msg_err("invalid drawing command %s", argv[arg]);
		error = true;
		break;
	    }
	    else
	    {
		state = PARAMS;
		if (current_cmd->params_len > 0)
		{
		    arg++;
		}
	    }
	}
	else // state == PARAMS
	{
	    *cmd_list = mh_realloc(*cmd_list, mh_alloc_mul(++(*cmd_list_len), sizeof(draw_cmd_t *)));
	    (*cmd_list)[*cmd_list_len - 1] = current_cmd;
	    if (current_cmd->type == DRAW_TEXT)
	    {
		// Special handling of the text command
		string_list_len += 1;
		*string_list = mh_realloc(*string_list, mh_alloc_mul(string_list_len, sizeof(char *)));
		(*string_list)[string_list_len - 1] = argv[arg];
	    }
	    else
	    {
		// All other commands take lists of doubles as arguments
		params_list_len += current_cmd->params_len;
		*params_list = mh_realloc(*params_list, mh_alloc_mul(params_list_len, sizeof(double)));
		error = !get_params(current_cmd, argv[arg], 
			*params_list + params_list_len - current_cmd->params_len);
	    }
	    arg++;
	    if (!argv[arg])
	    {
		break;
	    }
	    else if (isalpha((unsigned char)argv[arg][0]))
	    {
		state = CMD;
	    }
	}
    }
    if (error)
    {
	free(*cmd_list);
	*cmd_list = NULL;
	free(*params_list);
	*params_list = NULL;
	free(*string_list);
	*string_list = NULL;
	return false;
    }
    else
    {
	return true;
    }
}

/* Parse a gradient description from string s and create a CAIRO pattern from it */
static bool parse_gradient(const char *s, bool radial, cairo_pattern_t **gradient)
{
    double x[2], y[2], r[2];
    int stops;
    double *stop_offsets;
    cvl_color_t *stop_colors;
    const char *p;
    char *q;
    char *tmpstr;
    bool error;
    
    error = false;
    stops = 2;
    stop_offsets = mh_alloc(stops * sizeof(double));
    stop_colors = mh_alloc(stops * sizeof(cvl_color_t));
    p = s;
    for (int i = 0; i < 2 && !error; i++)
    {
	errno = 0;
	x[i] = strtod(p, &q);
	if (q == p || *q != ',' || errno == ERANGE) 
	{
	    error = true;
	    break;
	}
	p = q + 1;
	y[i] = strtod(p, &q);
	if (q == p || *q != ',' || errno == ERANGE) 
	{
	    error = true;
	    break;
	}
	p = q + 1;
	if (radial)
	{
	    r[i] = strtod(p, &q);
	    if (q == p || *q != ',' || errno == ERANGE || r[i] < 0.0)
	    {
		error = true;
		break;
	    }
	    p = q + 1;
	}
	if (!(q = strchr(p, ',')) && !(q = strchr(p, '\0')))
	{
	    error = true;
	    break;
	}
	tmpstr = mh_strndup(p, q - p);
	if ((stop_colors[i] = cvl_color_from_string(tmpstr)) == CVL_COLOR_INVALID)
	{
	    error = true;	    
	}
	free(tmpstr);
	p = (*q == '\0' ? q : q + 1);
    }
    stop_offsets[0] = 0.0;
    stop_offsets[1] = 1.0;
    while (*p != '\0' && !error)
    {
	stops++;
	stop_offsets = mh_realloc(stop_offsets, stops * sizeof(double));
	stop_colors = mh_realloc(stop_colors, stops * sizeof(cvl_color_t));
	stop_offsets[stops - 1] = strtod(p, &q);
	if (q == p || *q != ',' || errno == ERANGE 
		|| stop_offsets[stops - 1] < 0.0 || stop_offsets[stops - 1] > 1.0)
	{
	    error = true;
	    break;
	}
	p = q + 1;
	if (!(q = strchr(p, ',')) && !(q = strchr(p, '\0')))
	{
	    error = true;
	    break;
	}
	tmpstr = mh_strndup(p, q - p);
	if ((stop_colors[stops - 1] = cvl_color_from_string(tmpstr)) == CVL_COLOR_INVALID)
	{
	    error = true;	    
	}
	free(tmpstr);
	p = (*q == '\0' ? q : q + 1);
    }
    if (error)
    {
	mh_msg_err("invalid %s gradient %s", radial ? "radial" : "linear", s);
    }
    else
    {
	if (radial)
	{
	    *gradient = cairo_pattern_create_radial(x[0], y[0], r[0], x[1], y[1], r[1]);
	}
	else
	{
	    *gradient = cairo_pattern_create_linear(x[0], y[0], x[1], y[1]);
	}
	for (int i = 0; i < stops; i++)
	{
	    uint8_t r = (stop_colors[i] >> 16) & 0xff;
	    uint8_t g = (stop_colors[i] >> 8) & 0xff;
	    uint8_t b = stop_colors[i] & 0xff;
	    cairo_pattern_add_color_stop_rgb(*gradient, stop_offsets[i], 
		    (double)r / 255.0, (double)g / 255.0, (double)b / 255.0);
	}
    }
    free(stop_offsets);
    free(stop_colors);
    return !error;
}


/*
 * \param frame		The frame.
 * \return		The CAIRO drawing context.
 *
 * Initializes a CAIRO drawing context for the given frame. The frame must be
 * in #CVL_RGB format.
 * The idea is to call this function, then use CAIRO to work on the frame, and then call
 * stop_cairo() when finished.
 * Do not use CVL functions on the frame while in CAIRO mode.
 */
cairo_t *start_cairo(cvl_frame_t *frame)
{
    mh_assert(frame != NULL);

    cairo_surface_t *surface;
    cairo_t *cr;

    void *p = mh_alloc(mh_alloc_mul3(cvl_frame_width(frame), cvl_frame_height(frame), 
		4 * sizeof(unsigned char)));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, p);

    surface = cairo_image_surface_create_for_data(
	    p, CAIRO_FORMAT_RGB24, 
	    cvl_frame_width(frame), cvl_frame_height(frame),
	    cvl_frame_width(frame) * 4 * sizeof(unsigned char));
    cr = cairo_create(surface);
    return cr;
}

/*
 * \param frame 	The frame.
 * \param cr		The CAIRO drawing context.
 *
 * Destroys \a cr and stores CAIROs changes in \a frame.
 * See also start_cairo().
 */
void stop_cairo(cvl_frame_t *frame, cairo_t *cr)
{
    mh_assert(frame != NULL);
    mh_assert(cr != NULL);

    cairo_surface_t *surface = cairo_get_target(cr);
    void *p = cairo_image_surface_get_data(surface);

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
	    cvl_frame_width(frame), cvl_frame_height(frame),
	    0, GL_BGRA, GL_UNSIGNED_BYTE, p);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    free(p);
}


/* 
 * The draw command 
 */

static bool check_color(const char *s)
{
    return (cvl_color_from_string(s) != CVL_COLOR_INVALID);
}

static bool has_more_data(FILE *f)
{
    int c;
    return ((c = fgetc(f)) != EOF && ungetc(c, f) != EOF);
}

int cmd_draw(int argc, char *argv[])
{
    typedef enum { STYLE_NONE = 0, STYLE_COLOR = 1, STYLE_PATTERN = 2, STYLE_MULTIPATTERN = 3, 
	STYLE_LINEAR_GRADIENT = 4, STYLE_RADIAL_GRADIENT = 5} style_t;
    const char *style_names[] = { "none", "color", "pattern", "multipattern", 
	"linear-gradient", "radial-gradient", NULL };
    mh_option_name_t s = { STYLE_COLOR, style_names };
    mh_option_string_t c = { (char *)"black", check_color };
    mh_option_file_t p = { NULL, "r", false };
    mh_option_string_t g = { NULL, NULL };
    mh_option_name_t S = { STYLE_NONE, style_names };
    mh_option_string_t C = { (char *)"black", check_color };
    mh_option_file_t P = { NULL, "r", false };
    mh_option_string_t G = { NULL, NULL };
    mh_option_double_t w = { 2.0, 0.0, true, DBL_MAX, true };
    mh_option_double_array_t d = { NULL, 0, NULL, 1, NULL };
    const char *l_names[] = { "butt", "round", "square", NULL };
    mh_option_name_t l = { 0, l_names };
    const char *L_names[] = { "miter", "round", "bevel", NULL };
    mh_option_name_t L = { 0, L_names };
    mh_option_bool_t a = { true, true };
    mh_option_bool_t u = { false, true };
    mh_option_string_t f = { (char *)"Serif", NULL };
    typedef enum { SLANT_NORMAL = 0, SLANT_ITALIC = 1, SLANT_OBLIQUE = 2 } slant_t;
    const char *slant_names[] = { "normal", "italic", "oblique", NULL };
    mh_option_name_t t = { SLANT_NORMAL, slant_names };
    typedef enum { WEIGHT_NORMAL = 0, WEIGHT_BOLD } weight_t;
    const char *weight_names[] = { "normal", "bold", NULL };
    mh_option_name_t W = { WEIGHT_NORMAL, weight_names };
    mh_option_double_array_t F = { NULL, 0, NULL, 1, NULL };
    typedef enum { JUSTIFY_X_LEFT = 0, JUSTIFY_X_RIGHT = 1, JUSTIFY_X_CENTER = 2 } justify_x_t;
    const char *justify_x_names[] = { "left", "right", "center", NULL };
    mh_option_name_t j = { JUSTIFY_X_LEFT, justify_x_names };
    typedef enum { JUSTIFY_Y_BOTTOM = 0, JUSTIFY_Y_TOP = 1, JUSTIFY_Y_CENTER = 2 } justify_y_t;
    const char *justify_y_names[] = { "bottom", "top", "center", NULL };
    mh_option_name_t J = { JUSTIFY_Y_BOTTOM, justify_y_names };
    mh_option_t options[] = 
    { 
	{ "border-style",    's', MH_OPTION_NAME,         &s, false },
	{ "border-color",    'c', MH_OPTION_STRING,       &c, false },
	{ "border-pattern",  'p', MH_OPTION_FILE,         &p, false },
	{ "border-gradient", 'g', MH_OPTION_STRING,       &g, false },
	{ "fill-style",      'S', MH_OPTION_NAME,         &S, false },
	{ "fill-color",      'C', MH_OPTION_STRING,       &C, false },
	{ "fill-pattern",    'P', MH_OPTION_FILE,         &P, false },
	{ "fill-gradient",   'G', MH_OPTION_STRING,       &G, false },
	{ "width",           'w', MH_OPTION_DOUBLE,       &w, false },
	{ "dash",            'd', MH_OPTION_DOUBLE_ARRAY, &d, false },
	{ "line-cap",        'l', MH_OPTION_NAME,         &l, false },
	{ "line-join",       'L', MH_OPTION_NAME,         &L, false },
	{ "font-family",     'f', MH_OPTION_STRING,       &f, false },
	{ "font-slant",      't', MH_OPTION_NAME,         &t, false },
	{ "font-weight",     'W', MH_OPTION_NAME,         &W, false },
	{ "font-size",       'F', MH_OPTION_DOUBLE_ARRAY, &F, false },
	{ "justify-x",       'j', MH_OPTION_NAME,         &j, false },
	{ "justify-y",       'J', MH_OPTION_NAME,         &J, false },
	{ "antialias",       'a', MH_OPTION_BOOL,         &a, false },
	{ "unit",            'u', MH_OPTION_BOOL,         &u, false },
	mh_option_null 
    };
    int first_argument;
    draw_cmd_t **cmd_list = NULL;			// initialize to shut up compiler warning
    int cmd_list_len = 0;				// initialize to shut up compiler warning
    double *params_list = NULL;				// initialize to shut up compiler warning
    char **string_list = NULL;				// initialize to shut up compiler warning
    cvl_frame_t *frame;
    cvl_frame_t *border_pattern_frame;
    cairo_t *border_pattern_cr = NULL;			// initialize to shut up compiler warning
    cairo_pattern_t *border_gradient;
    cvl_frame_t *fill_pattern_frame;
    cairo_t *fill_pattern_cr = NULL;			// initialize to shut up compiler warning
    cairo_pattern_t *fill_gradient;
    cairo_matrix_t font_size_matrix;
    char *font_family_utf8;
    cvl_stream_type_t stream_type;
    cvl_format_t format;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);    
    error = !mh_getopt(argc, argv, options, 1, -1, &first_argument);
    if (!error && d.value)
    {
	bool found_val_greater_zero = false;
	for (int i = 0; i < d.value_sizes[0]; i++)
	{
	    if (d.value[i] > 0.0)
	    {
		found_val_greater_zero = true;
	    }
	    else if (d.value[i] < 0.0)
	    {
		error = true;
		break;
	    }
	}
	if (error || !found_val_greater_zero)
	{
	    mh_msg_err("Invalid dash specification");
	    free(d.value);
	    error = true;
	}
    }
    if (!error && (s.value == STYLE_PATTERN || s.value == STYLE_MULTIPATTERN) && !p.value)
    {
	mh_msg_err("No border pattern file specified");
	error = true;
    }
    if (!error && (s.value == STYLE_LINEAR_GRADIENT || s.value == STYLE_RADIAL_GRADIENT))
    {
	if (!g.value)
	{
	    mh_msg_err("No border gradient specified");
	    error = true;
	}
	else
	{
	    error = !parse_gradient(g.value, s.value == STYLE_RADIAL_GRADIENT, &border_gradient);
	}
    }
    if (!error && (S.value == STYLE_PATTERN || S.value == STYLE_MULTIPATTERN) && !P.value)
    {
	mh_msg_err("No fill pattern file specified");
	error = true;
    }
    if (!error && (S.value == STYLE_LINEAR_GRADIENT || S.value == STYLE_RADIAL_GRADIENT))
    {
	if (!G.value)
	{
	    mh_msg_err("No fill gradient specified");
	    error = true;
	}
	else
	{
	    error = !parse_gradient(G.value, S.value == STYLE_RADIAL_GRADIENT, &fill_gradient);
	}
    }
    if (!error)
    {
	if (F.value)
	{
	    if ((F.value_sizes[0] != 1 && F.value_sizes[0] != 2)
		    || F.value[0] <= 0.0
		    || (F.value_sizes[0] == 2 && F.value[1] <= 0.0))
	    {
		mh_msg_err("Invalid font size");
		error = true;		    
	    }
	    cairo_matrix_init_scale(&font_size_matrix, F.value[0], F.value[F.value_sizes[0] == 1 ? 0 : 1]);
	}
	else
	{
	    cairo_matrix_init_scale(&font_size_matrix, 12.0, 12.0);
	}
    }
    if (!error)
    {
	font_family_utf8 = string_to_utf8(f.value, "Serif");
    }
    if (!error)
    {
	error = !parse_draw_cmds(&(argv[first_argument]), 
		&cmd_list, &cmd_list_len, &params_list, &string_list);
    }
    if (error)
    {
	return 1;
    }

    if (s.value == STYLE_PATTERN || s.value == STYLE_MULTIPATTERN)
    {
	if (s.value == STYLE_PATTERN)
	{
	    cvl_read(p.value, NULL, &border_pattern_frame);
	    if (!border_pattern_frame)
	    {
		mh_msg_err("Cannot read border pattern");
		error = true;
	    }
	    else
	    {
		cvl_convert_format_inplace(border_pattern_frame, CVL_RGB);
		border_pattern_cr = start_cairo(border_pattern_frame);
	    }
	}
    }
    if (S.value == STYLE_PATTERN || S.value == STYLE_MULTIPATTERN)
    {
	if (S.value == STYLE_PATTERN)
	{
	    cvl_read(P.value, NULL, &fill_pattern_frame);
	    if (!fill_pattern_frame)
	    {
		mh_msg_err("Cannot read fill pattern");
		error = true;
	    }
	    else
	    {
		cvl_convert_format_inplace(fill_pattern_frame, CVL_RGB);
		fill_pattern_cr = start_cairo(fill_pattern_frame);
	    }
	}
    }

    while (!error && !cvl_error()
	    && has_more_data(stdin) 
	    && !(s.value == STYLE_MULTIPATTERN && !has_more_data(p.value))
	    && !(S.value == STYLE_MULTIPATTERN && !has_more_data(P.value)))
    {
	/* Input */
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	format = cvl_frame_format(frame);
	cvl_convert_format_inplace(frame, CVL_RGB);

	/* Draw with CAIRO */
	cairo_t *cr;
	cr = start_cairo(frame);
	double *params = NULL;		// initialize to shut up compiler warning
	int params_index = 0;
	char *string;
	int string_index = 0;
	cairo_text_extents_t text_extents;

	cairo_move_to(cr, 0.0, 0.0);
	if (u.value)
	{
	    cairo_scale(cr, cvl_frame_width(frame), cvl_frame_height(frame));
	}
	cairo_set_line_width(cr, w.value);
	cairo_set_antialias(cr, a.value ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
	if (d.value)
	{
	    cairo_set_dash(cr, d.value, d.value_sizes[0], 0.0);
	}
	cairo_set_line_cap(cr, l.value == 0 ? CAIRO_LINE_CAP_BUTT :
    		l.value == 1 ? CAIRO_LINE_CAP_ROUND : CAIRO_LINE_CAP_SQUARE);
	cairo_set_line_join(cr, L.value == 0 ? CAIRO_LINE_JOIN_MITER :
		L.value == 1 ? CAIRO_LINE_JOIN_ROUND : CAIRO_LINE_JOIN_BEVEL);
	cairo_select_font_face(cr, font_family_utf8, 
		t.value == SLANT_NORMAL ? CAIRO_FONT_SLANT_NORMAL :
		t.value == SLANT_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_OBLIQUE,
		W.value == WEIGHT_NORMAL ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_matrix(cr, &font_size_matrix);
	for (int i = 0; i < cmd_list_len; i++)
	{
	    if (cmd_list[i]->type != DRAW_TEXT)		// "text" does not use double parameters
	    {
		params = params_list + params_index;
		params_index += cmd_list[i]->params_len;
	    }
	    switch (cmd_list[i]->type)
	    {
		case DRAW_CLOSE:
		    cairo_close_path(cr);
		    break;

		case DRAW_RECTANGLE:
		    cairo_close_path(cr);
		    cairo_rectangle(cr, params[0], params[1], params[2], params[3]);
		    break;

		case DRAW_CIRCLE:
		    cairo_close_path(cr);
		    cairo_arc(cr, params[0], params[1], params[2], 0.0, 2.0 * M_PI);
		    cairo_close_path(cr);
		    break;

		case DRAW_ELLIPSE:
		    cairo_close_path(cr);
		    cairo_save(cr);
		    cairo_translate(cr, params[0] + (params[2] / 2.0), params[1] + (params[3] / 2.0));
		    cairo_scale(cr, params[2] / 2.0, params[3] / 2.0);
		    cairo_arc(cr, 0.0, 0.0, 1.0, 0.0, 2.0 * M_PI);
		    cairo_restore(cr);
		    cairo_close_path(cr);
		    break;

		case DRAW_ARC:
		    cairo_close_path(cr);
		    cairo_arc_negative(cr, params[0], params[1], params[2],
			    - mh_deg_to_rad(params[3]), - mh_deg_to_rad(params[4]));
		    cairo_close_path(cr);
		    break;

		case DRAW_MOVE_TO:
		    cairo_move_to(cr, params[0], params[1]);
		    break;

		case DRAW_LINE_TO:
		    cairo_line_to(cr, params[0], params[1]);
		    break;

		case DRAW_CURVE_TO:
		    cairo_curve_to(cr, params[0], params[1], params[2], params[3], params[4], params[5]);
		    break;

		case DRAW_REL_MOVE_TO:
		    cairo_rel_move_to(cr, params[0], params[1]);
		    break;

		case DRAW_REL_LINE_TO:
		    cairo_rel_line_to(cr, params[0], params[1]);
		    break;

		case DRAW_REL_CURVE_TO:
		    cairo_rel_curve_to(cr, params[0], params[1], params[2], params[3], params[4], params[5]);
		    break;
		    
		case DRAW_TEXT:
		    cairo_close_path(cr);
		    string = string_to_utf8(string_list[string_index], "ERROR");
		    string_index++;
		    if (j.value != JUSTIFY_X_LEFT || J.value != JUSTIFY_Y_BOTTOM)
		    {
			cairo_text_extents(cr, string, &text_extents);
		    }
		    if (j.value == JUSTIFY_X_RIGHT)
		    {
			cairo_rel_move_to(cr, - text_extents.width, 0.0);
		    }
	    	    else if (j.value == JUSTIFY_X_CENTER)
		    {
			cairo_rel_move_to(cr, - (text_extents.width / 2.0 + text_extents.x_bearing), 0.0);
		    }
		    if (J.value == JUSTIFY_Y_TOP)
		    {
			cairo_rel_move_to(cr, 0.0, - text_extents.y_bearing);
		    }
	    	    else if (J.value == JUSTIFY_Y_CENTER)
		    {
			cairo_rel_move_to(cr, 0.0, - text_extents.height / 2.0 - text_extents.y_bearing);
		    }		    
	    	    cairo_text_path(cr, string);
    		    free(string);
		    cairo_close_path(cr);
		    break;
	    }

	}
	if (S.value == STYLE_COLOR)
	{
	    cvl_color_t color = cvl_color_from_string(C.value);
	    uint8_t r = (color >> 16) & 0xff;
	    uint8_t g = (color >> 8) & 0xff;
	    uint8_t b = color & 0xff;
	    cairo_set_source_rgb(cr, (double)r / 255.0, (double)g / 255.0, (double)b / 255.0);
	    cairo_fill_preserve(cr);	    
	}
	else if (S.value == STYLE_PATTERN)
	{
	    cairo_set_source_surface(cr, cairo_get_target(fill_pattern_cr), 0.0, 0.0);
	    cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	    cairo_fill_preserve(cr);
	}
	else if (S.value == STYLE_MULTIPATTERN)
	{
	    cvl_read(P.value, NULL, &fill_pattern_frame);
	    if (!fill_pattern_frame)
	    {
		mh_msg_err("Cannot read fill pattern");
		error = true;
	    }
	    else
	    {
		fill_pattern_cr = start_cairo(fill_pattern_frame);
		cairo_set_source_surface(cr, cairo_get_target(fill_pattern_cr), 0.0, 0.0);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
		cairo_fill_preserve(cr);
		stop_cairo(fill_pattern_frame, fill_pattern_cr);
		cvl_frame_free(fill_pattern_frame);
	    }
	}
	else if (S.value == STYLE_LINEAR_GRADIENT || S.value == STYLE_RADIAL_GRADIENT)
	{
	    cairo_set_source(cr, fill_gradient);
	    cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	    cairo_fill_preserve(cr);
	}
	if (s.value == STYLE_COLOR)
	{
	    cvl_color_t color = cvl_color_from_string(c.value);
	    uint8_t r = (color >> 16) & 0xff;
	    uint8_t g = (color >> 8) & 0xff;
	    uint8_t b = color & 0xff;
	    cairo_set_source_rgb(cr, (double)r / 255.0, (double)g / 255.0, (double)b / 255.0);
	    cairo_stroke_preserve(cr);
	}	
	else if (s.value == STYLE_PATTERN)
	{
	    cairo_set_source_surface(cr, cairo_get_target(border_pattern_cr), 0.0, 0.0);
	    cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	    cairo_stroke_preserve(cr);
	}
	else if (s.value == STYLE_MULTIPATTERN)
	{
	    cvl_read(p.value, NULL, &border_pattern_frame);
	    if (!border_pattern_frame)
	    {
		mh_msg_err("Cannot read border pattern");
		error = true;
	    }
	    else
	    {
		border_pattern_cr = start_cairo(border_pattern_frame);
		cairo_set_source_surface(cr, cairo_get_target(border_pattern_cr), 0.0, 0.0);
		cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
		cairo_stroke_preserve(cr);
		stop_cairo(border_pattern_frame, border_pattern_cr);
		cvl_frame_free(border_pattern_frame);
	    }
	}
	else if (s.value == STYLE_LINEAR_GRADIENT || s.value == STYLE_RADIAL_GRADIENT)
	{
	    cairo_set_source(cr, border_gradient);
	    cairo_pattern_set_extend(cairo_get_source(cr), CAIRO_EXTEND_REPEAT);
	    cairo_stroke_preserve(cr);
	}
	if (cairo_status(cr))
	{
	    mh_msg_err("CAIRO error: %s", cairo_status_to_string(cairo_status(cr)));
	    error = true;
	}
	stop_cairo(frame, cr);

	/* Output */
	if (!error)
	{
	    cvl_convert_format_inplace(frame, format);
	    cvl_write(stdout, stream_type, frame);
	}
	cvl_frame_free(frame);
    }
    error = error || cvl_error();

    free(cmd_list);
    free(params_list);
    if (s.value == STYLE_PATTERN || s.value == STYLE_MULTIPATTERN)
    {
	if (s.value == STYLE_PATTERN)
	{
	    stop_cairo(border_pattern_frame, border_pattern_cr);
	    cvl_frame_free(border_pattern_frame);
	}
	fclose(p.value);
    }
    if (S.value == STYLE_PATTERN || S.value == STYLE_MULTIPATTERN)
    {
	if (S.value == STYLE_PATTERN)
	{
	    stop_cairo(fill_pattern_frame, fill_pattern_cr);
	    cvl_frame_free(fill_pattern_frame);
	}
	fclose(P.value);
    }
    free(font_family_utf8);
    free(d.value);
    free(d.value_sizes);
    free(F.value);
    free(F.value_sizes);
    return error ? 1 : 0;
}

#endif // HAVE_LIBCAIRO
