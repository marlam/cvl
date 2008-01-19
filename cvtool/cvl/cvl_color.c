/*
 * cvl_color.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
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

/** 
 * \file cvl_color.h
 * \brief Color handling. 
 *
 * Color handling.
 */

#include "config.h"

#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include <GL/glew.h>

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_taglist.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_basic.h"
#include "cvl/cvl_color.h"

#include "glsl/color/lum_to_rgb.glsl.h"
#include "glsl/color/lum_to_xyz.glsl.h"
#include "glsl/color/rgb_to_lum.glsl.h"
#include "glsl/color/rgb_to_xyz.glsl.h"
#include "glsl/color/xyz_to_rgb.glsl.h"
#include "glsl/color/rgb_to_hsl.glsl.h"
#include "glsl/color/hsl_to_rgb.glsl.h"
#include "glsl/color/channel_combine.glsl.h"
#include "glsl/color/channel_extract.glsl.h"
#include "glsl/color/invert.glsl.h"
#include "glsl/color/gamma_correct.glsl.h"
#include "glsl/color/color_adjust.glsl.h"
#include "glsl/color/transform_linear.glsl.h"
#include "glsl/color/transform_log.glsl.h"
#include "glsl/color/luminance_range.glsl.h"
#include "glsl/color/pseudo_color.glsl.h"
#include "glsl/color/threshold.glsl.h"


/**
 * \typedef cvl_color_t
 *
 * The representation of colors.
 * These color values correspond to the SVG color names defined by the W3c in
 * the SVG 1.1 specification. See <http://www.w3.org/TR/SVG11/types.html>.
 */
/** \var CVL_COLOR_ALICEBLUE
 *  aliceblue */
/** \var CVL_COLOR_ANTIQUEWHITE
 *  antiquewhite */
/** \var CVL_COLOR_AQUA
 *  aqua */
/** \var CVL_COLOR_AQUAMARINE
 *  aquamarine */
/** \var CVL_COLOR_AZURE
 *  azure */
/** \var CVL_COLOR_BEIGE
 *  beige */
/** \var CVL_COLOR_BISQUE
 *  bisque */
/** \var CVL_COLOR_BLACK
 *  black */
/** \var CVL_COLOR_BLANCHEDALMOND
 *  blanchedalmond */
/** \var CVL_COLOR_BLUE
 *  blue */
/** \var CVL_COLOR_BLUEVIOLET
 *  blueviolet */
/** \var CVL_COLOR_BROWN
 *  brown */
/** \var CVL_COLOR_BURLYWOOD
 *  burlywood */
/** \var CVL_COLOR_CADETBLUE
 *  cadetblue */
/** \var CVL_COLOR_CHARTREUSE
 *  chartreuse */
/** \var CVL_COLOR_CHOCOLATE
 *  chocolate */
/** \var CVL_COLOR_CORAL
 *  coral */
/** \var CVL_COLOR_CORNFLOWERBLUE
 *  cornflowerblue */
/** \var CVL_COLOR_CORNSILK
 *  cornsilk */
/** \var CVL_COLOR_CRIMSON
 *  crimson */
/** \var CVL_COLOR_CYAN
 *  cyan */
/** \var CVL_COLOR_DARKBLUE
 *  darkblue */
/** \var CVL_COLOR_DARKCYAN
 *  darkcyan */
/** \var CVL_COLOR_DARKGOLDENROD
 *  darkgoldenrod */
/** \var CVL_COLOR_DARKGRAY
 *  darkgray */
/** \var CVL_COLOR_DARKGREEN
 *  darkgreen */
/** \var CVL_COLOR_DARKGREY
 *  darkgrey */
/** \var CVL_COLOR_DARKKHAKI
 *  darkkhaki */
/** \var CVL_COLOR_DARKMAGENTA
 *  darkmagenta */
/** \var CVL_COLOR_DARKOLIVEGREEN
 *  darkolivegreen */
/** \var CVL_COLOR_DARKORANGE
 *  darkorange */
/** \var CVL_COLOR_DARKORCHID
 *  darkorchid */
/** \var CVL_COLOR_DARKRED
 *  darkred */
/** \var CVL_COLOR_DARKSALMON
 *  darksalmon */
/** \var CVL_COLOR_DARKSEAGREEN
 *  darkseagreen */
/** \var CVL_COLOR_DARKSLATEBLUE
 *  darkslateblue */
/** \var CVL_COLOR_DARKSLATEGRAY
 *  darkslategray */
/** \var CVL_COLOR_DARKSLATEGREY
 *  darkslategrey */
/** \var CVL_COLOR_DARKTURQUOISE
 *  darkturquoise */
/** \var CVL_COLOR_DARKVIOLET
 *  darkviolet */
/** \var CVL_COLOR_DEEPPINK
 *  deeppink */
/** \var CVL_COLOR_DEEPSKYBLUE
 *  deepskyblue */
/** \var CVL_COLOR_DIMGRAY
 *  dimgray */
/** \var CVL_COLOR_DIMGREY
 *  dimgrey */
/** \var CVL_COLOR_DODGERBLUE
 *  dodgerblue */
/** \var CVL_COLOR_FIREBRICK
 *  firebrick */
/** \var CVL_COLOR_FLORALWHITE
 *  floralwhite */
/** \var CVL_COLOR_FORESTGREEN
 *  forestgreen */
/** \var CVL_COLOR_FUCHSIA
 *  fuchsia */
/** \var CVL_COLOR_GAINSBORO
 *  gainsboro */
/** \var CVL_COLOR_GHOSTWHITE
 *  ghostwhite */
/** \var CVL_COLOR_GOLD
 *  gold */
/** \var CVL_COLOR_GOLDENROD
 *  goldenrod */
/** \var CVL_COLOR_GRAY
 *  gray */
/** \var CVL_COLOR_GREY
 *  grey */
/** \var CVL_COLOR_GREEN
 *  green */
/** \var CVL_COLOR_GREENYELLOW
 *  greenyellow */
/** \var CVL_COLOR_HONEYDEW
 *  honeydew */
/** \var CVL_COLOR_HOTPINK
 *  hotpink */
/** \var CVL_COLOR_INDIANRED
 *  indianred */
/** \var CVL_COLOR_INDIGO
 *  indigo */
/** \var CVL_COLOR_IVORY
 *  ivory */
/** \var CVL_COLOR_KHAKI
 *  khaki */
/** \var CVL_COLOR_LAVENDER
 *  lavender */
/** \var CVL_COLOR_LAVENDERBLUSH
 *  lavenderblush */
/** \var CVL_COLOR_LAWNGREEN
 *  lawngreen */
/** \var CVL_COLOR_LEMONCHIFFON
 *  lemonchiffon */
/** \var CVL_COLOR_LIGHTBLUE
 *  lightblue */
/** \var CVL_COLOR_LIGHTCORAL
 *  lightcoral */
/** \var CVL_COLOR_LIGHTCYAN
 *  lightcyan */
/** \var CVL_COLOR_LIGHTGOLDENRODYELLOW
 *  lightgoldenrodyellow */
/** \var CVL_COLOR_LIGHTGRAY
 *  lightgray */
/** \var CVL_COLOR_LIGHTGREEN
 *  lightgreen */
/** \var CVL_COLOR_LIGHTGREY
 *  lightgrey */
/** \var CVL_COLOR_LIGHTPINK
 *  lightpink */
/** \var CVL_COLOR_LIGHTSALMON
 *  lightsalmon */
/** \var CVL_COLOR_LIGHTSEAGREEN
 *  lightseagreen */
/** \var CVL_COLOR_LIGHTSKYBLUE
 *  lightskyblue */
/** \var CVL_COLOR_LIGHTSLATEGRAY
 *  lightslategray */
/** \var CVL_COLOR_LIGHTSLATEGREY
 *  lightslategrey */
/** \var CVL_COLOR_LIGHTSTEELBLUE
 *  lightsteelblue */
/** \var CVL_COLOR_LIGHTYELLOW
 *  lightyellow */
/** \var CVL_COLOR_LIME
 *  lime */
/** \var CVL_COLOR_LIMEGREEN
 *  limegreen */
/** \var CVL_COLOR_LINEN
 *  linen */
/** \var CVL_COLOR_MAGENTA
 *  magenta */
/** \var CVL_COLOR_MAROON
 *  maroon */
/** \var CVL_COLOR_MEDIUMAQUAMARINE
 *  mediumaquamarine */
/** \var CVL_COLOR_MEDIUMBLUE
 *  mediumblue */
/** \var CVL_COLOR_MEDIUMORCHID
 *  mediumorchid */
/** \var CVL_COLOR_MEDIUMPURPLE
 *  mediumpurple */
/** \var CVL_COLOR_MEDIUMSEAGREEN
 *  mediumseagreen */
/** \var CVL_COLOR_MEDIUMSLATEBLUE
 *  mediumslateblue */
/** \var CVL_COLOR_MEDIUMSPRINGGREEN
 *  mediumspringgreen */
/** \var CVL_COLOR_MEDIUMTURQUOISE
 *  mediumturquoise */
/** \var CVL_COLOR_MEDIUMVIOLETRED
 *  mediumvioletred */
/** \var CVL_COLOR_MIDNIGHTBLUE
 *  midnightblue */
/** \var CVL_COLOR_MINTCREAM
 *  mintcream */
/** \var CVL_COLOR_MISTYROSE
 *  mistyrose */
/** \var CVL_COLOR_MOCCASIN
 *  moccasin */
/** \var CVL_COLOR_NAVAJOWHITE
 *  navajowhite */
/** \var CVL_COLOR_NAVY
 *  navy */
/** \var CVL_COLOR_OLDLACE
 *  oldlace */
/** \var CVL_COLOR_OLIVE
 *  olive */
/** \var CVL_COLOR_OLIVEDRAB
 *  olivedrab */
/** \var CVL_COLOR_ORANGE
 *  orange */
/** \var CVL_COLOR_ORANGERED
 *  orangered */
/** \var CVL_COLOR_ORCHID
 *  orchid */
/** \var CVL_COLOR_PALEGOLDENROD
 *  palegoldenrod */
/** \var CVL_COLOR_PALEGREEN
 *  palegreen */
/** \var CVL_COLOR_PALETURQUOISE
 *  paleturquoise */
/** \var CVL_COLOR_PALEVIOLETRED
 *  palevioletred */
/** \var CVL_COLOR_PAPAYAWHIP
 *  papayawhip */
/** \var CVL_COLOR_PEACHPUFF
 *  peachpuff */
/** \var CVL_COLOR_PERU
 *  peru */
/** \var CVL_COLOR_PINK
 *  pink */
/** \var CVL_COLOR_PLUM
 *  plum */
/** \var CVL_COLOR_POWDERBLUE
 *  powderblue */
/** \var CVL_COLOR_PURPLE
 *  purple */
/** \var CVL_COLOR_RED
 *  red */
/** \var CVL_COLOR_ROSYBROWN
 *  rosybrown */
/** \var CVL_COLOR_ROYALBLUE
 *  royalblue */
/** \var CVL_COLOR_SADDLEBROWN
 *  saddlebrown */
/** \var CVL_COLOR_SALMON
 *  salmon */
/** \var CVL_COLOR_SANDYBROWN
 *  sandybrown */
/** \var CVL_COLOR_SEAGREEN
 *  seagreen */
/** \var CVL_COLOR_SEASHELL
 *  seashell */
/** \var CVL_COLOR_SIENNA
 *  sienna */
/** \var CVL_COLOR_SILVER
 *  silver */
/** \var CVL_COLOR_SKYBLUE
 *  skyblue */
/** \var CVL_COLOR_SLATEBLUE
 *  slateblue */
/** \var CVL_COLOR_SLATEGRAY
 *  slategray */
/** \var CVL_COLOR_SLATEGREY
 *  slategrey */
/** \var CVL_COLOR_SNOW
 *  snow */
/** \var CVL_COLOR_SPRINGGREEN
 *  springgreen */
/** \var CVL_COLOR_STEELBLUE
 *  steelblue */
/** \var CVL_COLOR_TAN
 *  tan */
/** \var CVL_COLOR_TEAL
 *  teal */
/** \var CVL_COLOR_THISTLE
 *  thistle */
/** \var CVL_COLOR_TOMATO
 *  tomato */
/** \var CVL_COLOR_TURQUOISE
 *  turquoise */
/** \var CVL_COLOR_VIOLET
 *  violet */
/** \var CVL_COLOR_WHEAT
 *  wheat */
/** \var CVL_COLOR_WHITE
 *  white */
/** \var CVL_COLOR_WHITESMOKE
 *  whitesmoke */
/** \var CVL_COLOR_YELLOW
 *  yellow */
/** \var CVL_COLOR_YELLOWGREEN
 *  yellowgreen */
/** \var CVL_COLOR_INVALID
 *  Special value: not a valid color! */


/* Intern: color table entries */
typedef struct
{
    const char *name;
    cvl_color_t color;
} cvl_color_table_entry_t;

/* See color.h for the origin of these color names. This table must be in
 * ascending order according to strcasecmp(), because we do a binary search on it. */
static cvl_color_table_entry_t cvl_color_table[] = 
{
    { "aliceblue",	CVL_COLOR_ALICEBLUE },
    { "antiquewhite",	CVL_COLOR_ANTIQUEWHITE },
    { "aqua",		CVL_COLOR_AQUA },
    { "aquamarine",	CVL_COLOR_AQUAMARINE },
    { "azure",		CVL_COLOR_AZURE },
    { "beige",		CVL_COLOR_BEIGE },
    { "bisque",		CVL_COLOR_BISQUE },
    { "black",		CVL_COLOR_BLACK },
    { "blanchedalmond",	CVL_COLOR_BLANCHEDALMOND },
    { "blue",		CVL_COLOR_BLUE },
    { "blueviolet",	CVL_COLOR_BLUEVIOLET },
    { "brown",		CVL_COLOR_BROWN },
    { "burlywood",	CVL_COLOR_BURLYWOOD },
    { "cadetblue",	CVL_COLOR_CADETBLUE },
    { "chartreuse",	CVL_COLOR_CHARTREUSE },
    { "chocolate",	CVL_COLOR_CHOCOLATE },
    { "coral",		CVL_COLOR_CORAL },
    { "cornflowerblue",	CVL_COLOR_CORNFLOWERBLUE },
    { "cornsilk",	CVL_COLOR_CORNSILK },
    { "crimson",	CVL_COLOR_CRIMSON },
    { "cyan",		CVL_COLOR_CYAN },
    { "darkblue",	CVL_COLOR_DARKBLUE },
    { "darkcyan",	CVL_COLOR_DARKCYAN },
    { "darkgoldenrod",	CVL_COLOR_DARKGOLDENROD },
    { "darkgray",	CVL_COLOR_DARKGRAY },
    { "darkgreen",	CVL_COLOR_DARKGREEN },
    { "darkgrey",	CVL_COLOR_DARKGREY },
    { "darkkhaki",	CVL_COLOR_DARKKHAKI },
    { "darkmagenta",	CVL_COLOR_DARKMAGENTA },
    { "darkolivegreen",	CVL_COLOR_DARKOLIVEGREEN },
    { "darkorange",	CVL_COLOR_DARKORANGE },
    { "darkorchid",	CVL_COLOR_DARKORCHID },
    { "darkred",	CVL_COLOR_DARKRED },
    { "darksalmon",	CVL_COLOR_DARKSALMON },
    { "darkseagreen",	CVL_COLOR_DARKSEAGREEN },
    { "darkslateblue",	CVL_COLOR_DARKSLATEBLUE },
    { "darkslategray",	CVL_COLOR_DARKSLATEGRAY },
    { "darkslategrey",	CVL_COLOR_DARKSLATEGREY },
    { "darkturquoise",	CVL_COLOR_DARKTURQUOISE },
    { "darkviolet",	CVL_COLOR_DARKVIOLET },
    { "deeppink",	CVL_COLOR_DEEPPINK },
    { "deepskyblue",	CVL_COLOR_DEEPSKYBLUE },
    { "dimgray",	CVL_COLOR_DIMGRAY },
    { "dimgrey",	CVL_COLOR_DIMGREY },
    { "dodgerblue",	CVL_COLOR_DODGERBLUE },
    { "firebrick",	CVL_COLOR_FIREBRICK },
    { "floralwhite",	CVL_COLOR_FLORALWHITE },
    { "forestgreen",	CVL_COLOR_FORESTGREEN },
    { "fuchsia",	CVL_COLOR_FUCHSIA },
    { "gainsboro",	CVL_COLOR_GAINSBORO },
    { "ghostwhite",	CVL_COLOR_GHOSTWHITE },
    { "gold",		CVL_COLOR_GOLD },
    { "goldenrod",	CVL_COLOR_GOLDENROD },
    { "gray",		CVL_COLOR_GRAY },
    { "green",		CVL_COLOR_GREEN },
    { "greenyellow",	CVL_COLOR_GREENYELLOW },
    { "grey",		CVL_COLOR_GREY },
    { "honeydew",	CVL_COLOR_HONEYDEW },
    { "hotpink",	CVL_COLOR_HOTPINK },
    { "indianred",	CVL_COLOR_INDIANRED },
    { "indigo",		CVL_COLOR_INDIGO },
    { "ivory",		CVL_COLOR_IVORY },
    { "khaki",		CVL_COLOR_KHAKI },
    { "lavender",	CVL_COLOR_LAVENDER },
    { "lavenderblush",	CVL_COLOR_LAVENDERBLUSH },
    { "lawngreen",	CVL_COLOR_LAWNGREEN },
    { "lemonchiffon",	CVL_COLOR_LEMONCHIFFON },
    { "lightblue",	CVL_COLOR_LIGHTBLUE },
    { "lightcoral",	CVL_COLOR_LIGHTCORAL },
    { "lightcyan",	CVL_COLOR_LIGHTCYAN },
    { "lightgoldenrodyellow", CVL_COLOR_LIGHTGOLDENRODYELLOW },
    { "lightgray",	CVL_COLOR_LIGHTGRAY },
    { "lightgreen",	CVL_COLOR_LIGHTGREEN },
    { "lightgrey",	CVL_COLOR_LIGHTGREY },
    { "lightpink",	CVL_COLOR_LIGHTPINK },
    { "lightsalmon",	CVL_COLOR_LIGHTSALMON },
    { "lightseagreen",	CVL_COLOR_LIGHTSEAGREEN },
    { "lightskyblue",	CVL_COLOR_LIGHTSKYBLUE },
    { "lightslategray",	CVL_COLOR_LIGHTSLATEGRAY },
    { "lightslategrey",	CVL_COLOR_LIGHTSLATEGREY },
    { "lightsteelblue",	CVL_COLOR_LIGHTSTEELBLUE },
    { "lightyellow",	CVL_COLOR_LIGHTYELLOW },
    { "lime",		CVL_COLOR_LIME },
    { "limegreen",	CVL_COLOR_LIMEGREEN },
    { "linen",		CVL_COLOR_LINEN },
    { "magenta",	CVL_COLOR_MAGENTA },
    { "maroon",		CVL_COLOR_MAROON },
    { "mediumaquamarine", CVL_COLOR_MEDIUMAQUAMARINE },
    { "mediumblue",	CVL_COLOR_MEDIUMBLUE },
    { "mediumorchid",	CVL_COLOR_MEDIUMORCHID },
    { "mediumpurple",	CVL_COLOR_MEDIUMPURPLE },
    { "mediumseagreen",	CVL_COLOR_MEDIUMSEAGREEN },
    { "mediumslateblue", CVL_COLOR_MEDIUMSLATEBLUE },
    { "mediumspringgreen", CVL_COLOR_MEDIUMSPRINGGREEN },
    { "mediumturquoise", CVL_COLOR_MEDIUMTURQUOISE },
    { "mediumvioletred", CVL_COLOR_MEDIUMVIOLETRED },
    { "midnightblue",	CVL_COLOR_MIDNIGHTBLUE },
    { "mintcream",	CVL_COLOR_MINTCREAM },
    { "mistyrose",	CVL_COLOR_MISTYROSE },
    { "moccasin",	CVL_COLOR_MOCCASIN },
    { "navajowhite",	CVL_COLOR_NAVAJOWHITE },
    { "navy",		CVL_COLOR_NAVY },
    { "oldlace",	CVL_COLOR_OLDLACE },
    { "olive",		CVL_COLOR_OLIVE },
    { "olivedrab",	CVL_COLOR_OLIVEDRAB },
    { "orange",		CVL_COLOR_ORANGE },
    { "orangered",	CVL_COLOR_ORANGERED },
    { "orchid",		CVL_COLOR_ORCHID },
    { "palegoldenrod",	CVL_COLOR_PALEGOLDENROD },
    { "palegreen",	CVL_COLOR_PALEGREEN },
    { "paleturquoise",	CVL_COLOR_PALETURQUOISE },
    { "palevioletred",	CVL_COLOR_PALEVIOLETRED },
    { "papayawhip",	CVL_COLOR_PAPAYAWHIP },
    { "peachpuff",	CVL_COLOR_PEACHPUFF },
    { "peru",		CVL_COLOR_PERU },
    { "pink",		CVL_COLOR_PINK },
    { "plum",		CVL_COLOR_PLUM },
    { "powderblue",	CVL_COLOR_POWDERBLUE },
    { "purple",		CVL_COLOR_PURPLE },
    { "red",		CVL_COLOR_RED },
    { "rosybrown",	CVL_COLOR_ROSYBROWN },
    { "royalblue",	CVL_COLOR_ROYALBLUE },
    { "saddlebrown",	CVL_COLOR_SADDLEBROWN },
    { "salmon",		CVL_COLOR_SALMON },
    { "sandybrown",	CVL_COLOR_SANDYBROWN },
    { "seagreen",	CVL_COLOR_SEAGREEN },
    { "seashell",	CVL_COLOR_SEASHELL },
    { "sienna",		CVL_COLOR_SIENNA },
    { "silver",		CVL_COLOR_SILVER },
    { "skyblue",	CVL_COLOR_SKYBLUE },
    { "slateblue",	CVL_COLOR_SLATEBLUE },
    { "slategray",	CVL_COLOR_SLATEGRAY },
    { "slategrey",	CVL_COLOR_SLATEGREY },
    { "snow",		CVL_COLOR_SNOW },
    { "springgreen",	CVL_COLOR_SPRINGGREEN },
    { "steelblue",	CVL_COLOR_STEELBLUE },
    { "tan",		CVL_COLOR_TAN },
    { "teal",		CVL_COLOR_TEAL },
    { "thistle",	CVL_COLOR_THISTLE },
    { "tomato",		CVL_COLOR_TOMATO },
    { "turquoise",	CVL_COLOR_TURQUOISE },
    { "violet",		CVL_COLOR_VIOLET },
    { "wheat",		CVL_COLOR_WHEAT },
    { "white",		CVL_COLOR_WHITE },
    { "whitesmoke",	CVL_COLOR_WHITESMOKE },
    { "yellow",		CVL_COLOR_YELLOW },
    { "yellowgreen",	CVL_COLOR_YELLOWGREEN }
};

static const size_t cvl_color_table_len = sizeof(cvl_color_table) / sizeof(cvl_color_table[0]);

/* A helper for cvl_color_from_string() */
static int cvl_color_from_string_cmp(const cvl_color_table_entry_t *e1, const cvl_color_table_entry_t *e2)
{
    return strcasecmp(e1->name, e2->name);
}

/**
 * \param s		The string.
 * \return color	The color.
 *
 * Converts a color string into a color value.
 * Valid strings are SVG color names as defined in the W3C standard SVG 1.1 (see
 * <http://www.w3.org/TR/SVG11/types.html>), RGB values in the form 0xrrggbb,
 * and decimal values prepended with r, g, or b, for example r127g64b12.
 * If \a s is not a valid color string, the special value #CVL_COLOR_INVALID
 * is returned.
 */
cvl_color_t cvl_color_from_string(const char *s)
{
    size_t len = strlen(s);
    cvl_color_table_entry_t key = { s, 0 };
    cvl_color_table_entry_t *entry;

    // 0xrrggbb
    if (len == 8 && s[0] == '0' && s[1] == 'x')
    {
	char *ptr;
	long l;

	errno = 0;
	l = strtol(s, &ptr, 16);
	if (errno != ERANGE && *ptr == '\0')
	{
	    /* we know that the value is in [0x000000, 0xffffff] */
	    return l;
	}
	// if it has the form 0x......, it cannot be one of the other to 
	// possibilities
	return CVL_COLOR_INVALID;
    }
    // r<val>g<val>b<val>
    if (len >= 1 && strspn(s, "rgb0123456789") == len)
    {
	typedef enum { RGB_LETTER, RGB_VALUE_R, RGB_VALUE_G, RGB_VALUE_B } state_t;
	state_t state = RGB_LETTER;
	int r = 0;
	int g = 0;
	int b = 0;
	int i = 0;
	bool error = false;
	
	while (!error)
	{
	    if (state == RGB_LETTER)
	    {
    		if (s[i] == 'r')
		{
		    state = RGB_VALUE_R;
		    i++;
		}
		else if (s[i] == 'g')
		{
		    state = RGB_VALUE_G;
		    i++;
		}
		else if (s[i] == 'b')
		{
		    state = RGB_VALUE_B;
		    i++;
		}
		else if (s[i] == '\0')
		{
		    break;
		}
		else
		{
		    error = true;
		}
	    }
	    else
	    {
		char *ptr;
		long l;

    		errno = 0;
		l = strtol(s + i, &ptr, 10);
		if (ptr != s + i && errno != ERANGE && l >= 0 && l <= 255)
		{
		    if (state == RGB_VALUE_R)
			r = l;
		    else if (state == RGB_VALUE_G)
			g = l;
		    else // state == RGB_VALUE_B
			b = l;
		    state = RGB_LETTER;
		    i = ptr - s;
		}
		else
		{
		    error = true;
		}
	    }
	}
	if (!error)
	{
	    return (r << 16) | (g << 8) | b;
	}
	// no need to check for a SVG color, because for none of those is 
	// (strspn(s, "rgb0123456789") == len) true.
	return CVL_COLOR_INVALID;
    }
    // SVG color name
    if ((entry = bsearch(&key, cvl_color_table, cvl_color_table_len, sizeof(cvl_color_table_entry_t), 
	    (int (*)(const void *, const void *))cvl_color_from_string_cmp)))
    {
	return entry->color;
    }

    return CVL_COLOR_INVALID;
}


/**
 * \param c		The color.
 * \param format	The format.
 * \param result	The result.
 *
 * Converts the CVL color \a c to four floating point values using the color
 * format \a format. If \a c is #CVL_COLOR_INVALID or \a format is #CVL_UNKNOWN,
 * then the resulting values will be 0.0 in each channel.
 */
void cvl_color_to_float(cvl_color_t c, cvl_format_t format, float *result)
{
    cvl_assert(result != NULL);
    if (cvl_error())
	return;

    if (c == CVL_COLOR_INVALID || format == CVL_UNKNOWN)
    {
	result[0] = 0.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
    }
    else if (format == CVL_LUM)
    {
	float r = (float)((c >> 16) & 0xff) / 255.0f;
	float g = (float)((c >> 8) & 0xff) / 255.0f;
	float b = (float)(c & 0xff) / 255.0f;
	float y = 0.212656f * (float)r + 0.715158f * (float)g + 0.072186f * (float)b;
	result[0] = y;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
    }
    else if (format == CVL_RGB)
    {
	result[0] = (float)((c >> 16) & 0xff) / 255.0f;
	result[1] = (float)((c >> 8) & 0xff) / 255.0f;
	result[2] = (float)(c & 0xff) / 255.0f;
	result[3] = 0.0f;
    }
    else if (format == CVL_XYZ)
    {
	float r = (float)((c >> 16) & 0xff) / 255.0f;
	float g = (float)((c >> 8) & 0xff) / 255.0f;
	float b = (float)(c & 0xff) / 255.0f;
	result[0] = 0.412424f * (float)r + 0.357579f * (float)g + 0.180464f * (float)b;
	result[1] = 0.212656f * (float)r + 0.715158f * (float)g + 0.072186f * (float)b;
	result[2] = 0.019332f * (float)r + 0.119193f * (float)g + 0.950444f * (float)b;
	result[3] = 0.0f;
    }
    else // CVL_HSL
    {
	uint8_t ri = (c >> 16) & 0xff;
	uint8_t gi = (c >> 8) & 0xff;
	uint8_t bi = c & 0xff;
	float r = (float)ri / 255.0f;
	float g = (float)gi / 255.0f;
	float b = (float)bi / 255.0f;
	uint8_t maxvali = mh_max3i(ri, gi, bi);
	uint8_t minvali = mh_min3i(ri, gi, bi);
	float minval = mh_min3f(r, g, b);
	float maxval = mh_max3f(r, g, b);
	float delta = maxval - minval;
	float h, s, l;
	l = (maxval + minval) / 2.0f;
	if (maxvali == minvali)
	{
	    h = 0.0f;
	    s = 0.0f;
	}
	else
	{
	    s = delta / ((l <= 0.5f) ? (maxval + minval) : (2.0f - maxval - minval));
	    if (maxvali == ri)
	    {
		h = (60.0f / 360.0f) * (g - b) / (maxval - minval);
		if (g < b)
		    h += 360.0f / 360.0f;
	    }
	    else if (maxvali == gi)
	    {
		h = (60.0f / 360.0f) * (b - r) / (maxval - minval) + (120.0f / 360.0f);
	    }
	    else
	    {
		h = (60.0f / 360.0f) * (r - g) / (maxval - minval) + (240.0f / 360.0f);
	    }
	}
	result[0] = h;
	result[1] = s;
	result[2] = l;
	result[3] = 0.0f;
    }
}


/**
 * \param c		The color.
 * \param format	The format.
 *
 * Sets the OpenGL color to the given CVL color \a c. 
 * Set the \a format parameter to the format of the frame you want to use the
 * color with.
 */
void cvl_gl_set_color(cvl_color_t c, cvl_format_t format)
{
    cvl_assert(c != CVL_COLOR_INVALID);
    cvl_assert(format != CVL_UNKNOWN);
    if (cvl_error())
	return;

    float val[4];
    cvl_color_to_float(c, format, val);
    glColor4f(val[0], val[1], val[2], val[3]);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_LUM to #CVL_RGB, and returns it.
 */
static void cvl_lum_to_rgb(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_LUM);
    cvl_assert(cvl_frame_format(dst) == CVL_RGB);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_lum_to_rgb")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_lum_to_rgb", NULL, CVL_LUM_TO_RGB_GLSL_STR);
	cvl_gl_program_cache_put("cvl_lum_to_rgb", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_LUM to #CVL_XYZ, and returns it.
 */
static void cvl_lum_to_xyz(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_LUM);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_lum_to_xyz")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_lum_to_xyz", NULL, CVL_LUM_TO_XYZ_GLSL_STR);
	cvl_gl_program_cache_put("cvl_lum_to_xyz", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_RGB to #CVL_LUM, and returns it.
 */
static void cvl_rgb_to_lum(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_RGB);
    cvl_assert(cvl_frame_format(dst) == CVL_LUM);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_rgb_to_lum")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_rgb_to_lum", NULL, CVL_RGB_TO_LUM_GLSL_STR);
	cvl_gl_program_cache_put("cvl_rgb_to_lum", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_RGB to #CVL_XYZ, and returns it.
 */
static void cvl_rgb_to_xyz(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_RGB);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_rgb_to_xyz")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_rgb_to_xyz", NULL, CVL_RGB_TO_XYZ_GLSL_STR);
	cvl_gl_program_cache_put("cvl_rgb_to_xyz", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_XYZ to #CVL_RGB, and returns it.
 */
static void cvl_xyz_to_rgb(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(cvl_frame_format(dst) == CVL_RGB);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_xyz_to_rgb")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_xyz_to_rgb", NULL, CVL_XYZ_TO_RGB_GLSL_STR);
	cvl_gl_program_cache_put("cvl_xyz_to_rgb", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_RGB to #CVL_HSL, and returns it.
 */
static void cvl_rgb_to_hsl(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_RGB);
    cvl_assert(cvl_frame_format(dst) == CVL_HSL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_rgb_to_hsl")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_rgb_to_hsl", NULL, CVL_RGB_TO_HSL_GLSL_STR);
	cvl_gl_program_cache_put("cvl_rgb_to_hsl", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Creates a copy of the frame \a frame, converts its format from 
 * #CVL_HSL to #CVL_RGB, and returns it.
 */
static void cvl_hsl_to_rgb(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_HSL);
    cvl_assert(cvl_frame_format(dst) == CVL_RGB);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_hsl_to_rgb")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_hsl_to_rgb", NULL, CVL_HSL_TO_RGB_GLSL_STR);
	cvl_gl_program_cache_put("cvl_hsl_to_rgb", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param c0		The first channel.
 * \param c1		The second channel.
 * \param c2		The third channel.
 * \param c3		The fourth channel.
 *
 * Combines the channels \a c0, \a c1, \a c2, and \a c3 into the frame \a dst.
 * Channels that are NULL will be set to zero in the result. At least 
 * one channel must be given. The number of channels in the resulting frame must
 * match.
 */
void cvl_channel_combine(cvl_frame_t *dst, 
	cvl_frame_t *c0, cvl_frame_t *c1, cvl_frame_t *c2, cvl_frame_t *c3)
{
    cvl_assert(dst != NULL);
    cvl_assert(c0 != NULL || c1 != NULL || c2 != NULL || c3 != NULL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_channel_combine")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_channel_combine", NULL, CVL_CHANNEL_COMBINE_GLSL_STR);
	cvl_gl_program_cache_put("cvl_channel_combine", prg);
    }

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dst));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(dst), 0);
    glViewport(0, 0, cvl_frame_width(dst), cvl_frame_height(dst));
    cvl_frame_t *t = c0 ? c0 : c1 ? c1 : c2 ? c2: c3;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, c0 ? cvl_frame_texture(c0) : cvl_frame_texture(t));
    cvl_gl_set_texture_state();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, c1 ? cvl_frame_texture(c1) : cvl_frame_texture(t));
    cvl_gl_set_texture_state();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, c2 ? cvl_frame_texture(c2) : cvl_frame_texture(t));
    cvl_gl_set_texture_state();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, c3 ? cvl_frame_texture(c3) : cvl_frame_texture(t));
    cvl_gl_set_texture_state();
    
    glUseProgram(prg);
    glUniform1i(glGetUniformLocation(prg, "tex_c0"), 0);
    glUniform1i(glGetUniformLocation(prg, "tex_c1"), 1);
    glUniform1i(glGetUniformLocation(prg, "tex_c2"), 2);
    glUniform1i(glGetUniformLocation(prg, "tex_c3"), 3);
    glUniform1i(glGetUniformLocation(prg, "have_c0"), c0 ? 1 : 0);
    glUniform1i(glGetUniformLocation(prg, "have_c1"), c1 ? 1 : 0);
    glUniform1i(glGetUniformLocation(prg, "have_c2"), c2 ? 1 : 0);
    glUniform1i(glGetUniformLocation(prg, "have_c3"), c3 ? 1 : 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel number.
 *
 * Extracts the given channel \a channel (0, 1, 2, or 3) from the frame \a src
 * into the frame \a dst at channel 0.
 */
void cvl_channel_extract(cvl_frame_t *dst, cvl_frame_t *src, int channel)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= 0 && channel <= cvl_frame_channels(src));
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_channel_extract")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_channel_extract", NULL, CVL_CHANNEL_EXTRACT_GLSL_STR);
	cvl_gl_program_cache_put("cvl_channel_extract", prg);
    }
    glUseProgram(prg);
    glUniform1i(glGetUniformLocation(prg, "channel"), channel);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Converts the format of frame \a src into the format of frame \a dst while
 * transferring the contents of \a src to \a dst.
 */
void cvl_convert_format(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    if (cvl_frame_format(src) == cvl_frame_format(dst)
	    || cvl_frame_format(src) == CVL_UNKNOWN
	    || cvl_frame_format(dst) == CVL_UNKNOWN)
    {
	cvl_copy(dst, src);
    }
    else
    {
	if (cvl_frame_format(src) == CVL_LUM)
	{
	    if (cvl_frame_format(dst) == CVL_RGB)
	    {
		cvl_lum_to_rgb(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_XYZ)
	    {
		cvl_lum_to_xyz(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_HSL)
	    {
		cvl_channel_combine(dst, NULL, NULL, src, NULL);
	    }
	}
	else if (cvl_frame_format(src) == CVL_RGB)
	{
	    if (cvl_frame_format(dst) == CVL_LUM)
	    {
		cvl_rgb_to_lum(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_XYZ)
	    {
		cvl_rgb_to_xyz(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_HSL)
	    {
		cvl_rgb_to_hsl(dst, src);
	    }
	}
	else if (cvl_frame_format(src) == CVL_XYZ)
	{
	    if (cvl_frame_format(dst) == CVL_LUM)
	    {
		cvl_channel_extract(dst, src, 1);
	    }
	    else if (cvl_frame_format(dst) == CVL_RGB)
	    {
		cvl_xyz_to_rgb(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_HSL)
	    {
		cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
		cvl_frame_set_format(tmpframe, CVL_RGB);
		cvl_xyz_to_rgb(tmpframe, src);
		cvl_rgb_to_hsl(dst, tmpframe);
		cvl_frame_free(tmpframe);
	    }
	}
	else if (cvl_frame_format(src) == CVL_HSL)
	{
	    if (cvl_frame_format(dst) == CVL_LUM)
	    {
		cvl_channel_extract(dst, src, 2);
	    }
	    else if (cvl_frame_format(dst) == CVL_RGB)
	    {
		cvl_hsl_to_rgb(dst, src);
	    }
	    else if (cvl_frame_format(dst) == CVL_XYZ)
	    {
		cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
		cvl_frame_set_format(tmpframe, CVL_RGB);
		cvl_hsl_to_rgb(tmpframe, src);
		cvl_rgb_to_xyz(dst, tmpframe);
		cvl_frame_free(tmpframe);
	    }
	}
    }
}


/**
 * \param frame		The frame.
 * \param format	The format.
 *
 * Converts the format of frame \a frame to \a format.
 */
void cvl_convert_format_inplace(cvl_frame_t *frame, cvl_format_t format)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    if (cvl_frame_format(frame) == format)
    {
	return;
    }
    if (format == CVL_UNKNOWN)
    {
	cvl_frame_set_format(frame, CVL_UNKNOWN);
	return;
    }
    if (cvl_frame_format(frame) == CVL_UNKNOWN)
    {
	cvl_frame_set_format(frame, format);
	return;
    }
    cvl_frame_t *tmpframe = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame),
	    format == CVL_LUM ? 1 : 3, format, cvl_frame_type(frame), CVL_TEXTURE);
    cvl_convert_format(tmpframe, frame);
    glDeleteTextures(1, &(frame->tex));
    frame->tex = tmpframe->tex;
    frame->channels = tmpframe->channels;
    frame->format = tmpframe->format;
    tmpframe->tex = 0;
    cvl_frame_free(tmpframe);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Inverts the frame \a src into \a dst.\n
 * The format of the frames must be #CVL_LUM or #CVL_RGB.
 */
void cvl_invert(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_LUM || cvl_frame_format(src) == CVL_RGB);
    cvl_assert(cvl_frame_format(src) == cvl_frame_format(dst));
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_invert")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_invert", NULL, CVL_INVERT_GLSL_STR);
	cvl_gl_program_cache_put("cvl_invert", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param gamma		The gamma value.
 * 
 * Applies gamma correction to the frame \a src and writes the result to \a dst. 
 * The function does the right thing for known color formats. For unknown formats,
 * it applies gamma correction to all channels.
 */
void cvl_gamma_correct(cvl_frame_t *dst, cvl_frame_t *src, float gamma)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == cvl_frame_format(dst));
    cvl_assert(gamma > 0.0f);
    if (cvl_error())
	return;

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_gamma_correct_hsl=%d_xyz=%d", 
	    cvl_frame_format(src) == CVL_HSL ? 1 : 0,
	    cvl_frame_format(src) == CVL_XYZ ? 1 : 0);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *prgsrc = cvl_gl_srcprep(cvl_strdup(CVL_GAMMA_CORRECT_GLSL_STR),
		"$format_hsl=%d, $format_xyz=%d",
		cvl_frame_format(src) == CVL_HSL ? 1 : 0,
		cvl_frame_format(src) == CVL_XYZ ? 1 : 0);
	prg = cvl_gl_program_new_src(prgname, NULL, prgsrc);
	cvl_gl_program_cache_put(prgname, prg);
	free(prgsrc);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "g"), 1.0f / gamma);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param hue		Additive constant for the hue value.
 * \param saturation	Modification of saturation.
 * \param lightness	Modification of lightness.
 * \param contrast	Modification of contrast.
 * 
 * Manipulates the colors of the frame \a src in the #CVL_HSL color space, and
 * writes the result to \a dst.\n
 * The parameter \a hue is an angle from [-PI,PI] that will be added to the hue angle.
 * The new saturation, lightness, and contrast values of the frame will each be 
 * computed with the following formula: new_x = old_x + factor * old_x, where
 * factor is the corresponding parameter. Thus, a parameter \a saturation = -1
 * will set the color saturation to zero for the frame, which results in a gray
 * level frame.\n
 * The format of the input frame must be #CVL_HSL.
 */
void cvl_color_adjust(cvl_frame_t *dst, cvl_frame_t *src,
	float hue, float saturation, float lightness, float contrast)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(src) == CVL_HSL);
    cvl_assert(cvl_frame_format(dst) == CVL_HSL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_color_adjust")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_color_adjust", NULL, CVL_COLOR_ADJUST_GLSL_STR);
	cvl_gl_program_cache_put("cvl_color_adjust", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "hue"), hue);
    glUniform1f(glGetUniformLocation(prg, "saturation"), saturation);
    glUniform1f(glGetUniformLocation(prg, "lightness"), lightness);
    glUniform1f(glGetUniformLocation(prg, "contrast"), contrast);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 * \param min		The minimum value.
 * \param max		The maximum value.
 * 
 * Does a linear transform of the interval [\a min, \a max] to [0,1] on the
 * given \a channel (0-3) of the frame \a src and write the result to \a dst.
 * If \a channel is -1, the transformation will be done for all channels. 
 * Values outside of the given interval will be clipped.
 */
void cvl_transform_linear(cvl_frame_t *dst, cvl_frame_t *src,
	int channel, float min, float max)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= -1 && channel < cvl_frame_channels(src));
    if (cvl_error())
	return;

    const char *channel_names[] = { "r", "g", "b", "a" };

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_transform_linear_channel=%s",
	    channel == -1 ? "all" : channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *prgsrc = cvl_gl_srcprep(cvl_strdup(CVL_TRANSFORM_LINEAR_GLSL_STR),
		"$all_channels=%d, $channel=%s", 
		channel == -1 ? 1 : 0,
		channel == -1 ? "all" : channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, prgsrc);
	cvl_gl_program_cache_put(prg_name, prg);
	free(prgsrc);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "xmin"), min);
    glUniform1f(glGetUniformLocation(prg, "xmax"), max);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 * \param min		The minimum value.
 * \param max		The maximum value.
 * \param base		The base of the logarithm.
 * 
 * Does a logarithmic transform of the interval [\a min, \a max] to [0,1] on the
 * given \a channel (0-3) of the frame \a src and writes the result to \a dst. 
 * If \a channel is -1, the transformation will be done for all channels. 
 * Values outside of the given interval will be clipped.
 */
void cvl_transform_log(cvl_frame_t *dst, cvl_frame_t *src,
	int channel, float min, float max, float base)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= -1 && channel < cvl_frame_channels(src));
    cvl_assert(base > 0.0f);
    if (cvl_error())
	return;

    const char *channel_names[] = { "r", "g", "b", "a" };

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_transform_log_channel=%s",
	    channel == -1 ? "all" : channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *prgsrc = cvl_gl_srcprep(cvl_strdup(CVL_TRANSFORM_LOG_GLSL_STR),
		"$all_channels=%d, $channel=%s", 
		channel == -1 ? 1 : 0,
		channel == -1 ? "all" : channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, prgsrc);
	cvl_gl_program_cache_put(prg_name, prg);
	free(prgsrc);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "xmin"), min);
    glUniform1f(glGetUniformLocation(prg, "xmax"), max);
    glUniform1f(glGetUniformLocation(prg, "base"), base);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param lum_min	The minimum value.
 * \param lum_max	The maximum value.
 * 
 * Selects the luminance range from \a lum_min to \a lum_max from the input
 * frame \a src, transforms the result to (0,1] and writes it to the frame \a
 * dst. This function needs #CVL_XYZ frames for input and output. The color
 * information will be preserved. 
 */
void cvl_luminance_range(cvl_frame_t *dst, cvl_frame_t *src, float lum_min, float lum_max)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_XYZ);
    cvl_assert(cvl_frame_format(src) == CVL_XYZ);
    cvl_assert(lum_min >= 0.0f);
    cvl_assert(lum_min < lum_max);
    if (cvl_error())
	return;
    
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_luminance_range")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_luminance_range", NULL, CVL_LUMINANCE_RANGE_GLSL_STR);
	cvl_gl_program_cache_put("cvl_luminance_range", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "lum_min"), lum_min);
    glUniform1f(glGetUniformLocation(prg, "lum_max"), lum_max);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 * \param cyclic	Use cyclic pseudo colors?
 * \param startcolor    The color to map to the minimum value.
 * \param lightness	The usage of lightness variability.
 * \param min		The minimum value.
 * \param max		The maximum value.
 * 
 * Transforms the values in channel \a channel (0-3) of the frame \a src to pseudo
 * colors and writes the result to \a dst. 
 * The \a startcolor parameter sets the color that is mapped to the
 * minimum value. It must be from [0,1] and is usually 0 for blue.
 * The parameter \a lightness specifies the amount of lightness
 * variability in the pseudo color frame. It must be from [0,1] and is usually
 * 0. The channel values are expected to be in the range from \a min to \a max.
 * The \a dst frame must be in #CVL_HSL format.
 */
void cvl_pseudo_color(cvl_frame_t *dst, cvl_frame_t *src,
	int channel, bool cyclic, float startcolor, float lightness, float min, float max)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_HSL);
    cvl_assert(channel >= 0 && channel <= cvl_frame_channels(src));
    cvl_assert(startcolor >= 0.0f && startcolor <= 1.0f);
    cvl_assert(lightness >= 0.0f && lightness <= 1.0f);
    cvl_assert(min < max);
    if (cvl_error())
	return;

    const char *channel_names[] = { "r", "g", "b", "a" };

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_pseudo_color_channel=%s", channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *prgsrc = cvl_gl_srcprep(cvl_strdup(CVL_PSEUDO_COLOR_GLSL_STR),
		"$channel=%s", channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, prgsrc);
	cvl_gl_program_cache_put(prg_name, prg);
	free(prgsrc);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "factor"), cyclic ? 1.0f : (2.0f / 3.0f));
    glUniform1f(glGetUniformLocation(prg, "startcolor"), startcolor);
    glUniform1f(glGetUniformLocation(prg, "lightness"), lightness);
    glUniform1f(glGetUniformLocation(prg, "xmin"), min);
    glUniform1f(glGetUniformLocation(prg, "xmax"), max);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel(s).
 * \param threshold	The threshold.
 *
 * Sets all pixels in the given \a channel (0-3 or -1 for all channels) of the
 * frame \a src to either 1.0 (if its value was >= \a threshold) or 0.0 (if its
 * value was < \a threshold). The result is written to \a dst.
 */
void cvl_threshold(cvl_frame_t *dst, cvl_frame_t *src, int channel, float threshold)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= 0 && channel <= cvl_frame_channels(src));
    if (cvl_error())
	return;

    const char *channel_names[] = { "r", "g", "b", "a" };

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_threshold_channel=%s",
	    channel == -1 ? "all" : channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *prgsrc = cvl_gl_srcprep(cvl_strdup(CVL_THRESHOLD_GLSL_STR),
		"$all_channels=%d, $channel=%s", 
		channel == -1 ? 1 : 0,
		channel == -1 ? "all" : channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, prgsrc);
	cvl_gl_program_cache_put(prg_name, prg);
	free(prgsrc);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "threshold"), threshold);
    cvl_transform(dst, src);
    cvl_check_errors();
}
