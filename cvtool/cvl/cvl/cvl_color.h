/*
 * cvl_color.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
 * Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_COLOR_H
#define CVL_COLOR_H

#include <stdbool.h>

typedef enum
{
    CVL_COLOR_ALICEBLUE		= ((240 << 16) + (248 << 8) + 255),
    CVL_COLOR_ANTIQUEWHITE	= ((250 << 16) + (235 << 8) + 215),
    CVL_COLOR_AQUA		= ((  0 << 16) + (255 << 8) + 255),
    CVL_COLOR_AQUAMARINE	= ((127 << 16) + (255 << 8) + 212),
    CVL_COLOR_AZURE		= ((240 << 16) + (255 << 8) + 255),
    CVL_COLOR_BEIGE		= ((245 << 16) + (245 << 8) + 220),
    CVL_COLOR_BISQUE		= ((255 << 16) + (228 << 8) + 196),
    CVL_COLOR_BLACK		= ((  0 << 16) + (  0 << 8) +   0),
    CVL_COLOR_BLANCHEDALMOND	= ((255 << 16) + (235 << 8) + 205),
    CVL_COLOR_BLUE		= ((  0 << 16) + (  0 << 8) + 255),
    CVL_COLOR_BLUEVIOLET	= ((138 << 16) + ( 43 << 8) + 226),
    CVL_COLOR_BROWN		= ((165 << 16) + ( 42 << 8) +  42),
    CVL_COLOR_BURLYWOOD		= ((222 << 16) + (184 << 8) + 135),
    CVL_COLOR_CADETBLUE		= (( 95 << 16) + (158 << 8) + 160),
    CVL_COLOR_CHARTREUSE	= ((127 << 16) + (255 << 8) +   0),
    CVL_COLOR_CHOCOLATE		= ((210 << 16) + (105 << 8) +  30),
    CVL_COLOR_CORAL		= ((255 << 16) + (127 << 8) +  80),
    CVL_COLOR_CORNFLOWERBLUE	= ((100 << 16) + (149 << 8) + 237),
    CVL_COLOR_CORNSILK		= ((255 << 16) + (248 << 8) + 220),
    CVL_COLOR_CRIMSON		= ((220 << 16) + ( 20 << 8) +  60),
    CVL_COLOR_CYAN		= ((  0 << 16) + (255 << 8) + 255),
    CVL_COLOR_DARKBLUE		= ((  0 << 16) + (  0 << 8) + 139),
    CVL_COLOR_DARKCYAN		= ((  0 << 16) + (139 << 8) + 139),
    CVL_COLOR_DARKGOLDENROD	= ((184 << 16) + (134 << 8) +  11),
    CVL_COLOR_DARKGRAY		= ((169 << 16) + (169 << 8) + 169),
    CVL_COLOR_DARKGREEN		= ((  0 << 16) + (100 << 8) +   0),
    CVL_COLOR_DARKGREY		= ((169 << 16) + (169 << 8) + 169),
    CVL_COLOR_DARKKHAKI		= ((189 << 16) + (183 << 8) + 107),
    CVL_COLOR_DARKMAGENTA	= ((139 << 16) + (  0 << 8) + 139),
    CVL_COLOR_DARKOLIVEGREEN	= (( 85 << 16) + (107 << 8) + 47),
    CVL_COLOR_DARKORANGE	= ((255 << 16) + (140 << 8) +   0),
    CVL_COLOR_DARKORCHID	= ((153 << 16) + ( 50 << 8) + 204),
    CVL_COLOR_DARKRED		= ((139 << 16) + (  0 << 8) +   0),
    CVL_COLOR_DARKSALMON	= ((233 << 16) + (150 << 8) + 122),
    CVL_COLOR_DARKSEAGREEN	= ((143 << 16) + (188 << 8) + 143),
    CVL_COLOR_DARKSLATEBLUE	= (( 72 << 16) + ( 61 << 8) + 139),
    CVL_COLOR_DARKSLATEGRAY	= (( 47 << 16) + ( 79 << 8) +  79),
    CVL_COLOR_DARKSLATEGREY	= (( 47 << 16) + ( 79 << 8) +  79),
    CVL_COLOR_DARKTURQUOISE	= ((  0 << 16) + (206 << 8) + 209),
    CVL_COLOR_DARKVIOLET	= ((148 << 16) + (  0 << 8) + 211),
    CVL_COLOR_DEEPPINK		= ((255 << 16) + ( 20 << 8) + 147),
    CVL_COLOR_DEEPSKYBLUE	= ((  0 << 16) + (191 << 8) + 255),
    CVL_COLOR_DIMGRAY		= ((105 << 16) + (105 << 8) + 105),
    CVL_COLOR_DIMGREY		= ((105 << 16) + (105 << 8) + 105),
    CVL_COLOR_DODGERBLUE	= (( 30 << 16) + (144 << 8) + 255),
    CVL_COLOR_FIREBRICK		= ((178 << 16) + ( 34 << 8) +  34),
    CVL_COLOR_FLORALWHITE	= ((255 << 16) + (250 << 8) + 240),
    CVL_COLOR_FORESTGREEN	= (( 34 << 16) + (139 << 8) +  34),
    CVL_COLOR_FUCHSIA		= ((255 << 16) + (  0 << 8) + 255),
    CVL_COLOR_GAINSBORO		= ((220 << 16) + (220 << 8) + 220),
    CVL_COLOR_GHOSTWHITE	= ((248 << 16) + (248 << 8) + 255),
    CVL_COLOR_GOLD		= ((255 << 16) + (215 << 8) +   0),
    CVL_COLOR_GOLDENROD		= ((218 << 16) + (165 << 8) +  32),
    CVL_COLOR_GRAY		= ((128 << 16) + (128 << 8) + 128),
    CVL_COLOR_GREY		= ((128 << 16) + (128 << 8) + 128),
    CVL_COLOR_GREEN		= ((  0 << 16) + (128 << 8) +   0),
    CVL_COLOR_GREENYELLOW	= ((173 << 16) + (255 << 8) +  47),
    CVL_COLOR_HONEYDEW		= ((240 << 16) + (255 << 8) + 240),
    CVL_COLOR_HOTPINK		= ((255 << 16) + (105 << 8) + 180),
    CVL_COLOR_INDIANRED		= ((205 << 16) + ( 92 << 8) +  92),
    CVL_COLOR_INDIGO		= (( 75 << 16) + (  0 << 8) + 130),
    CVL_COLOR_IVORY		= ((255 << 16) + (255 << 8) + 240),
    CVL_COLOR_KHAKI		= ((240 << 16) + (230 << 8) + 140),
    CVL_COLOR_LAVENDER		= ((230 << 16) + (230 << 8) + 250),
    CVL_COLOR_LAVENDERBLUSH	= ((255 << 16) + (240 << 8) + 245),
    CVL_COLOR_LAWNGREEN		= ((124 << 16) + (252 << 8) +   0),
    CVL_COLOR_LEMONCHIFFON	= ((255 << 16) + (250 << 8) + 205),
    CVL_COLOR_LIGHTBLUE		= ((173 << 16) + (216 << 8) + 230),
    CVL_COLOR_LIGHTCORAL	= ((240 << 16) + (128 << 8) + 128),
    CVL_COLOR_LIGHTCYAN		= ((224 << 16) + (255 << 8) + 255),
    CVL_COLOR_LIGHTGOLDENRODYELLOW = ((250 << 16) + (250 << 8) + 210),
    CVL_COLOR_LIGHTGRAY		= ((211 << 16) + (211 << 8) + 211),
    CVL_COLOR_LIGHTGREEN	= ((144 << 16) + (238 << 8) + 144),
    CVL_COLOR_LIGHTGREY		= ((211 << 16) + (211 << 8) + 211),
    CVL_COLOR_LIGHTPINK		= ((255 << 16) + (182 << 8) + 193),
    CVL_COLOR_LIGHTSALMON	= ((255 << 16) + (160 << 8) + 122),
    CVL_COLOR_LIGHTSEAGREEN	= (( 32 << 16) + (178 << 8) + 170),
    CVL_COLOR_LIGHTSKYBLUE	= ((135 << 16) + (206 << 8) + 250),
    CVL_COLOR_LIGHTSLATEGRAY	= ((119 << 16) + (136 << 8) + 153),
    CVL_COLOR_LIGHTSLATEGREY	= ((119 << 16) + (136 << 8) + 153),
    CVL_COLOR_LIGHTSTEELBLUE	= ((176 << 16) + (196 << 8) + 222),
    CVL_COLOR_LIGHTYELLOW	= ((255 << 16) + (255 << 8) + 224),
    CVL_COLOR_LIME		= ((  0 << 16) + (255 << 8) +   0),
    CVL_COLOR_LIMEGREEN		= (( 50 << 16) + (205 << 8) +  50),
    CVL_COLOR_LINEN		= ((250 << 16) + (240 << 8) + 230),
    CVL_COLOR_MAGENTA		= ((255 << 16) + (  0 << 8) + 255),
    CVL_COLOR_MAROON		= ((128 << 16) + (  0 << 8) +  0),
    CVL_COLOR_MEDIUMAQUAMARINE	= ((102 << 16) + (205 << 8) + 170),
    CVL_COLOR_MEDIUMBLUE	= ((  0 << 16) + (  0 << 8) + 205),
    CVL_COLOR_MEDIUMORCHID	= ((186 << 16) + ( 85 << 8) + 211),
    CVL_COLOR_MEDIUMPURPLE	= ((147 << 16) + (112 << 8) + 219),
    CVL_COLOR_MEDIUMSEAGREEN	= (( 60 << 16) + (179 << 8) + 113),
    CVL_COLOR_MEDIUMSLATEBLUE	= ((123 << 16) + (104 << 8) + 238),
    CVL_COLOR_MEDIUMSPRINGGREEN = ((  0 << 16) + (250 << 8) + 154),
    CVL_COLOR_MEDIUMTURQUOISE	= (( 72 << 16) + (209 << 8) + 204),
    CVL_COLOR_MEDIUMVIOLETRED	= ((199 << 16) + ( 21 << 8) + 133),
    CVL_COLOR_MIDNIGHTBLUE	= (( 25 << 16) + ( 25 << 8) + 112),
    CVL_COLOR_MINTCREAM		= ((245 << 16) + (255 << 8) + 250),
    CVL_COLOR_MISTYROSE		= ((255 << 16) + (228 << 8) + 225),
    CVL_COLOR_MOCCASIN		= ((255 << 16) + (228 << 8) + 181),
    CVL_COLOR_NAVAJOWHITE	= ((255 << 16) + (222 << 8) + 173),
    CVL_COLOR_NAVY		= ((  0 << 16) + (  0 << 8) + 128),
    CVL_COLOR_OLDLACE		= ((253 << 16) + (245 << 8) + 230),
    CVL_COLOR_OLIVE		= ((128 << 16) + (128 << 8) +   0),
    CVL_COLOR_OLIVEDRAB		= ((107 << 16) + (142 << 8) +  35),
    CVL_COLOR_ORANGE		= ((255 << 16) + (165 << 8) +   0),
    CVL_COLOR_ORANGERED		= ((255 << 16) + ( 69 << 8) +   0),
    CVL_COLOR_ORCHID		= ((218 << 16) + (112 << 8) + 214),
    CVL_COLOR_PALEGOLDENROD	= ((238 << 16) + (232 << 8) + 170),
    CVL_COLOR_PALEGREEN		= ((152 << 16) + (251 << 8) + 152),
    CVL_COLOR_PALETURQUOISE	= ((175 << 16) + (238 << 8) + 238),
    CVL_COLOR_PALEVIOLETRED	= ((219 << 16) + (112 << 8) + 147),
    CVL_COLOR_PAPAYAWHIP	= ((255 << 16) + (239 << 8) + 213),
    CVL_COLOR_PEACHPUFF		= ((255 << 16) + (218 << 8) + 185),
    CVL_COLOR_PERU		= ((205 << 16) + (133 << 8) +  63),
    CVL_COLOR_PINK		= ((255 << 16) + (192 << 8) + 203),
    CVL_COLOR_PLUM		= ((221 << 16) + (160 << 8) + 221),
    CVL_COLOR_POWDERBLUE	= ((176 << 16) + (224 << 8) + 230),
    CVL_COLOR_PURPLE		= ((128 << 16) + (  0 << 8) + 128),
    CVL_COLOR_RED		= ((255 << 16) + (  0 << 8) +   0),
    CVL_COLOR_ROSYBROWN		= ((188 << 16) + (143 << 8) + 143),
    CVL_COLOR_ROYALBLUE		= (( 65 << 16) + (105 << 8) + 225),
    CVL_COLOR_SADDLEBROWN	= ((139 << 16) + ( 69 << 8) +  19),
    CVL_COLOR_SALMON		= ((250 << 16) + (128 << 8) + 114),
    CVL_COLOR_SANDYBROWN	= ((244 << 16) + (164 << 8) +  96),
    CVL_COLOR_SEAGREEN		= (( 46 << 16) + (139 << 8) +  87),
    CVL_COLOR_SEASHELL		= ((255 << 16) + (245 << 8) + 238),
    CVL_COLOR_SIENNA		= ((160 << 16) + ( 82 << 8) +  45),
    CVL_COLOR_SILVER		= ((192 << 16) + (192 << 8) + 192),
    CVL_COLOR_SKYBLUE		= ((135 << 16) + (206 << 8) + 235),
    CVL_COLOR_SLATEBLUE		= ((106 << 16) + ( 90 << 8) + 205),
    CVL_COLOR_SLATEGRAY		= ((112 << 16) + (128 << 8) + 144),
    CVL_COLOR_SLATEGREY		= ((112 << 16) + (128 << 8) + 144),
    CVL_COLOR_SNOW		= ((255 << 16) + (250 << 8) + 250),
    CVL_COLOR_SPRINGGREEN	= ((  0 << 16) + (255 << 8) + 127),
    CVL_COLOR_STEELBLUE		= (( 70 << 16) + (130 << 8) + 180),
    CVL_COLOR_TAN		= ((210 << 16) + (180 << 8) + 140),
    CVL_COLOR_TEAL		= ((  0 << 16) + (128 << 8) + 128),
    CVL_COLOR_THISTLE		= ((216 << 16) + (191 << 8) + 216),
    CVL_COLOR_TOMATO		= ((255 << 16) + ( 99 << 8) +  71),
    CVL_COLOR_TURQUOISE		= (( 64 << 16) + (224 << 8) + 208),
    CVL_COLOR_VIOLET		= ((238 << 16) + (130 << 8) + 238),
    CVL_COLOR_WHEAT		= ((245 << 16) + (222 << 8) + 179),
    CVL_COLOR_WHITE		= ((255 << 16) + (255 << 8) + 255),
    CVL_COLOR_WHITESMOKE	= ((245 << 16) + (245 << 8) + 245),
    CVL_COLOR_YELLOW		= ((255 << 16) + (255 << 8) +   0),
    CVL_COLOR_YELLOWGREEN	= ((154 << 16) + (205 << 8) +  50),
    CVL_COLOR_INVALID		= ((255 << 16) + (255 << 8) + 255) + 1
} cvl_color_t;

extern CVL_EXPORT cvl_color_t cvl_color_from_string(const char *s);
extern CVL_EXPORT void cvl_color_to_float(cvl_color_t c, cvl_format_t format, float *result);
extern CVL_EXPORT void cvl_gl_set_color(cvl_color_t c, cvl_format_t format);

extern CVL_EXPORT void cvl_channel_combine(cvl_frame_t *dst, 
	cvl_frame_t *c0, cvl_frame_t *c1, cvl_frame_t *c2, cvl_frame_t *c3);
extern CVL_EXPORT void cvl_channel_extract(cvl_frame_t *dst, cvl_frame_t *src, int channel);

extern CVL_EXPORT void cvl_convert_format(cvl_frame_t *dst, cvl_frame_t *src);
extern CVL_EXPORT void cvl_convert_format_forced(cvl_frame_t *dst, cvl_frame_t *src, cvl_format_t src_format);
extern CVL_EXPORT void cvl_convert_format_inplace(cvl_frame_t *frame, cvl_format_t format);

extern CVL_EXPORT void cvl_invert(cvl_frame_t *dst, cvl_frame_t *src);

extern CVL_EXPORT void cvl_gamma_correct(cvl_frame_t *dst, cvl_frame_t *src, float gamma);
extern CVL_EXPORT void cvl_color_adjust(cvl_frame_t *dst, cvl_frame_t *src, 
	float hue, float saturation, float lightness, float contrast);
extern CVL_EXPORT void cvl_transform_linear(cvl_frame_t *dst, cvl_frame_t *src, 
	int channel, float min, float max);
extern CVL_EXPORT void cvl_transform_log(cvl_frame_t *dst, cvl_frame_t *src, 
	int channel, float min, float max, float base);
extern CVL_EXPORT void cvl_luminance_range(cvl_frame_t *dst, cvl_frame_t *src, float lum_min, float lum_max);
extern CVL_EXPORT void cvl_pseudo_color(cvl_frame_t *dst, cvl_frame_t *src,
	int channel, float min, float max, 
	float startcolor, float lightness, bool invert, bool cyclic);
extern CVL_EXPORT void cvl_threshold(cvl_frame_t *dst, cvl_frame_t *src, int channel, float threshold);

#endif
