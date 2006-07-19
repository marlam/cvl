/*
 * cvl_color.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
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
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** 
 * \file cvl_color.h
 * \brief Color handling. 
 *
 * Handling colors: Basic definitions, color related frame manipulation,
 * binarziation.
 */

#include "config.h"

#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
extern int errno;

#include "xalloc.h"

#include "cvl/cvl_math.h"
#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_color.h"


/**
 * \typedef cvl_color_t
 *
 * The representation of colors. Note that most functions expect pixel values;
 * you can convert a color into a pixel value using cvl_color_to_pixel().
 *
 * These color values correspond to X11 colors defined in rgb.txt. See also
 * cvl_color_from_string().
 */
/** \var CVL_COLOR_SNOW 
 *  snow */
/** \var CVL_COLOR_GHOSTWHITE 
 *  ghostwhite */
/** \var CVL_COLOR_WHITESMOKE 
 *  whitesmoke */
/** \var CVL_COLOR_GAINSBORO 
 *  gainsboro */
/** \var CVL_COLOR_FLORALWHITE 
 *  floralwhite */
/** \var CVL_COLOR_OLDLACE 
 *  oldlace */
/** \var CVL_COLOR_LINEN 
 *  linen */
/** \var CVL_COLOR_ANTIQUEWHITE 
 *  antiquewhite */
/** \var CVL_COLOR_PAPAYAWHIP 
 *  papayawhip */
/** \var CVL_COLOR_BLANCHEDALMOND 
 *  blanchedalmond */
/** \var CVL_COLOR_BISQUE 
 *  bisque */
/** \var CVL_COLOR_PEACHPUFF 
 *  peachpuff */
/** \var CVL_COLOR_NAVAJOWHITE 
 *  navajowhite */
/** \var CVL_COLOR_MOCCASIN 
 *  moccasin */
/** \var CVL_COLOR_CORNSILK 
 *  cornsilk */
/** \var CVL_COLOR_IVORY 
 *  ivory */
/** \var CVL_COLOR_LEMONCHIFFON 
 *  lemonchiffon */
/** \var CVL_COLOR_SEASHELL 
 *  seashell */
/** \var CVL_COLOR_HONEYDEW 
 *  honeydew */
/** \var CVL_COLOR_MINTCREAM 
 *  mintcream */
/** \var CVL_COLOR_AZURE 
 *  azure */
/** \var CVL_COLOR_ALICEBLUE 
 *  aliceblue */
/** \var CVL_COLOR_LAVENDER 
 *  lavender */
/** \var CVL_COLOR_LAVENDERBLUSH 
 *  lavenderblush */
/** \var CVL_COLOR_MISTYROSE 
 *  mistyrose */
/** \var CVL_COLOR_WHITE 
 *  white */
/** \var CVL_COLOR_BLACK 
 *  black */
/** \var CVL_COLOR_DARKSLATEGRAY 
 *  darkslategray */
/** \var CVL_COLOR_DARKSLATEGREY 
 *  darkslategrey */
/** \var CVL_COLOR_DIMGRAY 
 *  dimgray */
/** \var CVL_COLOR_DIMGREY 
 *  dimgrey */
/** \var CVL_COLOR_SLATEGRAY 
 *  slategray */
/** \var CVL_COLOR_SLATEGREY 
 *  slategrey */
/** \var CVL_COLOR_LIGHTSLATEGRAY 
 *  lightslategray */
/** \var CVL_COLOR_LIGHTSLATEGREY 
 *  lightslategrey */
/** \var CVL_COLOR_GRAY 
 *  gray */
/** \var CVL_COLOR_GREY 
 *  grey */
/** \var CVL_COLOR_LIGHTGREY 
 *  lightgrey */
/** \var CVL_COLOR_LIGHTGRAY 
 *  lightgray */
/** \var CVL_COLOR_MIDNIGHTBLUE 
 *  midnightblue */
/** \var CVL_COLOR_NAVY 
 *  navy */
/** \var CVL_COLOR_NAVYBLUE 
 *  navyblue */
/** \var CVL_COLOR_CORNFLOWERBLUE 
 *  cornflowerblue */
/** \var CVL_COLOR_DARKSLATEBLUE 
 *  darkslateblue */
/** \var CVL_COLOR_SLATEBLUE 
 *  slateblue */
/** \var CVL_COLOR_MEDIUMSLATEBLUE 
 *  mediumslateblue */
/** \var CVL_COLOR_LIGHTSLATEBLUE 
 *  lightslateblue */
/** \var CVL_COLOR_MEDIUMBLUE 
 *  mediumblue */
/** \var CVL_COLOR_ROYALBLUE 
 *  royalblue */
/** \var CVL_COLOR_BLUE 
 *  blue */
/** \var CVL_COLOR_DODGERBLUE 
 *  dodgerblue */
/** \var CVL_COLOR_DEEPSKYBLUE 
 *  deepskyblue */
/** \var CVL_COLOR_SKYBLUE 
 *  skyblue */
/** \var CVL_COLOR_LIGHTSKYBLUE 
 *  lightskyblue */
/** \var CVL_COLOR_STEELBLUE 
 *  steelblue */
/** \var CVL_COLOR_LIGHTSTEELBLUE 
 *  lightsteelblue */
/** \var CVL_COLOR_LIGHTBLUE 
 *  lightblue */
/** \var CVL_COLOR_POWDERBLUE 
 *  powderblue */
/** \var CVL_COLOR_PALETURQUOISE 
 *  paleturquoise */
/** \var CVL_COLOR_DARKTURQUOISE 
 *  darkturquoise */
/** \var CVL_COLOR_MEDIUMTURQUOISE 
 *  mediumturquoise */
/** \var CVL_COLOR_TURQUOISE 
 *  turquoise */
/** \var CVL_COLOR_CYAN 
 *  cyan */
/** \var CVL_COLOR_LIGHTCYAN 
 *  lightcyan */
/** \var CVL_COLOR_CADETBLUE 
 *  cadetblue */
/** \var CVL_COLOR_MEDIUMAQUAMARINE 
 *  mediumaquamarine */
/** \var CVL_COLOR_AQUAMARINE 
 *  aquamarine */
/** \var CVL_COLOR_DARKGREEN 
 *  darkgreen */
/** \var CVL_COLOR_DARKOLIVEGREEN 
 *  darkolivegreen */
/** \var CVL_COLOR_DARKSEAGREEN 
 *  darkseagreen */
/** \var CVL_COLOR_SEAGREEN 
 *  seagreen */
/** \var CVL_COLOR_MEDIUMSEAGREEN 
 *  mediumseagreen */
/** \var CVL_COLOR_LIGHTSEAGREEN 
 *  lightseagreen */
/** \var CVL_COLOR_PALEGREEN 
 *  palegreen */
/** \var CVL_COLOR_SPRINGGREEN 
 *  springgreen */
/** \var CVL_COLOR_LAWNGREEN 
 *  lawngreen */
/** \var CVL_COLOR_GREEN 
 *  green */
/** \var CVL_COLOR_CHARTREUSE 
 *  chartreuse */
/** \var CVL_COLOR_MEDIUMSPRINGGREEN 
 *  mediumspringgreen */
/** \var CVL_COLOR_GREENYELLOW 
 *  greenyellow */
/** \var CVL_COLOR_LIMEGREEN 
 *  limegreen */
/** \var CVL_COLOR_YELLOWGREEN 
 *  yellowgreen */
/** \var CVL_COLOR_FORESTGREEN 
 *  forestgreen */
/** \var CVL_COLOR_OLIVEDRAB 
 *  olivedrab */
/** \var CVL_COLOR_DARKKHAKI 
 *  darkkhaki */
/** \var CVL_COLOR_KHAKI 
 *  khaki */
/** \var CVL_COLOR_PALEGOLDENROD 
 *  palegoldenrod */
/** \var CVL_COLOR_LIGHTGOLDENRODYELLOW 
 *  lightgoldenrodyellow */
/** \var CVL_COLOR_LIGHTYELLOW 
 *  lightyellow */
/** \var CVL_COLOR_YELLOW 
 *  yellow */
/** \var CVL_COLOR_GOLD 
 *  gold */
/** \var CVL_COLOR_LIGHTGOLDENROD 
 *  lightgoldenrod */
/** \var CVL_COLOR_GOLDENROD 
 *  goldenrod */
/** \var CVL_COLOR_DARKGOLDENROD 
 *  darkgoldenrod */
/** \var CVL_COLOR_ROSYBROWN 
 *  rosybrown */
/** \var CVL_COLOR_INDIANRED 
 *  indianred */
/** \var CVL_COLOR_SADDLEBROWN 
 *  saddlebrown */
/** \var CVL_COLOR_SIENNA 
 *  sienna */
/** \var CVL_COLOR_PERU 
 *  peru */
/** \var CVL_COLOR_BURLYWOOD 
 *  burlywood */
/** \var CVL_COLOR_BEIGE 
 *  beige */
/** \var CVL_COLOR_WHEAT 
 *  wheat */
/** \var CVL_COLOR_SANDYBROWN 
 *  sandybrown */
/** \var CVL_COLOR_TAN 
 *  tan */
/** \var CVL_COLOR_CHOCOLATE 
 *  chocolate */
/** \var CVL_COLOR_FIREBRICK 
 *  firebrick */
/** \var CVL_COLOR_BROWN 
 *  brown */
/** \var CVL_COLOR_DARKSALMON 
 *  darksalmon */
/** \var CVL_COLOR_SALMON 
 *  salmon */
/** \var CVL_COLOR_LIGHTSALMON 
 *  lightsalmon */
/** \var CVL_COLOR_ORANGE 
 *  orange */
/** \var CVL_COLOR_DARKORANGE 
 *  darkorange */
/** \var CVL_COLOR_CORAL 
 *  coral */
/** \var CVL_COLOR_LIGHTCORAL 
 *  lightcoral */
/** \var CVL_COLOR_TOMATO 
 *  tomato */
/** \var CVL_COLOR_ORANGERED 
 *  orangered */
/** \var CVL_COLOR_RED 
 *  red */
/** \var CVL_COLOR_HOTPINK 
 *  hotpink */
/** \var CVL_COLOR_DEEPPINK 
 *  deeppink */
/** \var CVL_COLOR_PINK 
 *  pink */
/** \var CVL_COLOR_LIGHTPINK 
 *  lightpink */
/** \var CVL_COLOR_PALEVIOLETRED 
 *  palevioletred */
/** \var CVL_COLOR_MAROON 
 *  maroon */
/** \var CVL_COLOR_MEDIUMVIOLETRED 
 *  mediumvioletred */
/** \var CVL_COLOR_VIOLETRED 
 *  violetred */
/** \var CVL_COLOR_MAGENTA 
 *  magenta */
/** \var CVL_COLOR_VIOLET 
 *  violet */
/** \var CVL_COLOR_PLUM 
 *  plum */
/** \var CVL_COLOR_ORCHID 
 *  orchid */
/** \var CVL_COLOR_MEDIUMORCHID 
 *  mediumorchid */
/** \var CVL_COLOR_DARKORCHID 
 *  darkorchid */
/** \var CVL_COLOR_DARKVIOLET 
 *  darkviolet */
/** \var CVL_COLOR_BLUEVIOLET 
 *  blueviolet */
/** \var CVL_COLOR_PURPLE 
 *  purple */
/** \var CVL_COLOR_MEDIUMPURPLE 
 *  mediumpurple */
/** \var CVL_COLOR_THISTLE 
 *  thistle */
/** \var CVL_COLOR_SNOW1 
 *  snow1 */
/** \var CVL_COLOR_SNOW2 
 *  snow2 */
/** \var CVL_COLOR_SNOW3 
 *  snow3 */
/** \var CVL_COLOR_SNOW4 
 *  snow4 */
/** \var CVL_COLOR_SEASHELL1 
 *  seashell1 */
/** \var CVL_COLOR_SEASHELL2 
 *  seashell2 */
/** \var CVL_COLOR_SEASHELL3 
 *  seashell3 */
/** \var CVL_COLOR_SEASHELL4 
 *  seashell4 */
/** \var CVL_COLOR_ANTIQUEWHITE1 
 *  antiquewhite1 */
/** \var CVL_COLOR_ANTIQUEWHITE2 
 *  antiquewhite2 */
/** \var CVL_COLOR_ANTIQUEWHITE3 
 *  antiquewhite3 */
/** \var CVL_COLOR_ANTIQUEWHITE4 
 *  antiquewhite4 */
/** \var CVL_COLOR_BISQUE1 
 *  bisque1 */
/** \var CVL_COLOR_BISQUE2 
 *  bisque2 */
/** \var CVL_COLOR_BISQUE3 
 *  bisque3 */
/** \var CVL_COLOR_BISQUE4 
 *  bisque4 */
/** \var CVL_COLOR_PEACHPUFF1 
 *  peachpuff1 */
/** \var CVL_COLOR_PEACHPUFF2 
 *  peachpuff2 */
/** \var CVL_COLOR_PEACHPUFF3 
 *  peachpuff3 */
/** \var CVL_COLOR_PEACHPUFF4 
 *  peachpuff4 */
/** \var CVL_COLOR_NAVAJOWHITE1 
 *  navajowhite1 */
/** \var CVL_COLOR_NAVAJOWHITE2 
 *  navajowhite2 */
/** \var CVL_COLOR_NAVAJOWHITE3 
 *  navajowhite3 */
/** \var CVL_COLOR_NAVAJOWHITE4 
 *  navajowhite4 */
/** \var CVL_COLOR_LEMONCHIFFON1 
 *  lemonchiffon1 */
/** \var CVL_COLOR_LEMONCHIFFON2 
 *  lemonchiffon2 */
/** \var CVL_COLOR_LEMONCHIFFON3 
 *  lemonchiffon3 */
/** \var CVL_COLOR_LEMONCHIFFON4 
 *  lemonchiffon4 */
/** \var CVL_COLOR_CORNSILK1 
 *  cornsilk1 */
/** \var CVL_COLOR_CORNSILK2 
 *  cornsilk2 */
/** \var CVL_COLOR_CORNSILK3 
 *  cornsilk3 */
/** \var CVL_COLOR_CORNSILK4 
 *  cornsilk4 */
/** \var CVL_COLOR_IVORY1 
 *  ivory1 */
/** \var CVL_COLOR_IVORY2 
 *  ivory2 */
/** \var CVL_COLOR_IVORY3 
 *  ivory3 */
/** \var CVL_COLOR_IVORY4 
 *  ivory4 */
/** \var CVL_COLOR_HONEYDEW1 
 *  honeydew1 */
/** \var CVL_COLOR_HONEYDEW2 
 *  honeydew2 */
/** \var CVL_COLOR_HONEYDEW3 
 *  honeydew3 */
/** \var CVL_COLOR_HONEYDEW4 
 *  honeydew4 */
/** \var CVL_COLOR_LAVENDERBLUSH1 
 *  lavenderblush1 */
/** \var CVL_COLOR_LAVENDERBLUSH2 
 *  lavenderblush2 */
/** \var CVL_COLOR_LAVENDERBLUSH3 
 *  lavenderblush3 */
/** \var CVL_COLOR_LAVENDERBLUSH4 
 *  lavenderblush4 */
/** \var CVL_COLOR_MISTYROSE1 
 *  mistyrose1 */
/** \var CVL_COLOR_MISTYROSE2 
 *  mistyrose2 */
/** \var CVL_COLOR_MISTYROSE3 
 *  mistyrose3 */
/** \var CVL_COLOR_MISTYROSE4 
 *  mistyrose4 */
/** \var CVL_COLOR_AZURE1 
 *  azure1 */
/** \var CVL_COLOR_AZURE2 
 *  azure2 */
/** \var CVL_COLOR_AZURE3 
 *  azure3 */
/** \var CVL_COLOR_AZURE4 
 *  azure4 */
/** \var CVL_COLOR_SLATEBLUE1 
 *  slateblue1 */
/** \var CVL_COLOR_SLATEBLUE2 
 *  slateblue2 */
/** \var CVL_COLOR_SLATEBLUE3 
 *  slateblue3 */
/** \var CVL_COLOR_SLATEBLUE4 
 *  slateblue4 */
/** \var CVL_COLOR_ROYALBLUE1 
 *  royalblue1 */
/** \var CVL_COLOR_ROYALBLUE2 
 *  royalblue2 */
/** \var CVL_COLOR_ROYALBLUE3 
 *  royalblue3 */
/** \var CVL_COLOR_ROYALBLUE4 
 *  royalblue4 */
/** \var CVL_COLOR_BLUE1 
 *  blue1 */
/** \var CVL_COLOR_BLUE2 
 *  blue2 */
/** \var CVL_COLOR_BLUE3 
 *  blue3 */
/** \var CVL_COLOR_BLUE4 
 *  blue4 */
/** \var CVL_COLOR_DODGERBLUE1 
 *  dodgerblue1 */
/** \var CVL_COLOR_DODGERBLUE2 
 *  dodgerblue2 */
/** \var CVL_COLOR_DODGERBLUE3 
 *  dodgerblue3 */
/** \var CVL_COLOR_DODGERBLUE4 
 *  dodgerblue4 */
/** \var CVL_COLOR_STEELBLUE1 
 *  steelblue1 */
/** \var CVL_COLOR_STEELBLUE2 
 *  steelblue2 */
/** \var CVL_COLOR_STEELBLUE3 
 *  steelblue3 */
/** \var CVL_COLOR_STEELBLUE4 
 *  steelblue4 */
/** \var CVL_COLOR_DEEPSKYBLUE1 
 *  deepskyblue1 */
/** \var CVL_COLOR_DEEPSKYBLUE2 
 *  deepskyblue2 */
/** \var CVL_COLOR_DEEPSKYBLUE3 
 *  deepskyblue3 */
/** \var CVL_COLOR_DEEPSKYBLUE4 
 *  deepskyblue4 */
/** \var CVL_COLOR_SKYBLUE1 
 *  skyblue1 */
/** \var CVL_COLOR_SKYBLUE2 
 *  skyblue2 */
/** \var CVL_COLOR_SKYBLUE3 
 *  skyblue3 */
/** \var CVL_COLOR_SKYBLUE4 
 *  skyblue4 */
/** \var CVL_COLOR_LIGHTSKYBLUE1 
 *  lightskyblue1 */
/** \var CVL_COLOR_LIGHTSKYBLUE2 
 *  lightskyblue2 */
/** \var CVL_COLOR_LIGHTSKYBLUE3 
 *  lightskyblue3 */
/** \var CVL_COLOR_LIGHTSKYBLUE4 
 *  lightskyblue4 */
/** \var CVL_COLOR_SLATEGRAY1 
 *  slategray1 */
/** \var CVL_COLOR_SLATEGRAY2 
 *  slategray2 */
/** \var CVL_COLOR_SLATEGRAY3 
 *  slategray3 */
/** \var CVL_COLOR_SLATEGRAY4 
 *  slategray4 */
/** \var CVL_COLOR_LIGHTSTEELBLUE1 
 *  lightsteelblue1 */
/** \var CVL_COLOR_LIGHTSTEELBLUE2 
 *  lightsteelblue2 */
/** \var CVL_COLOR_LIGHTSTEELBLUE3 
 *  lightsteelblue3 */
/** \var CVL_COLOR_LIGHTSTEELBLUE4 
 *  lightsteelblue4 */
/** \var CVL_COLOR_LIGHTBLUE1 
 *  lightblue1 */
/** \var CVL_COLOR_LIGHTBLUE2 
 *  lightblue2 */
/** \var CVL_COLOR_LIGHTBLUE3 
 *  lightblue3 */
/** \var CVL_COLOR_LIGHTBLUE4 
 *  lightblue4 */
/** \var CVL_COLOR_LIGHTCYAN1 
 *  lightcyan1 */
/** \var CVL_COLOR_LIGHTCYAN2 
 *  lightcyan2 */
/** \var CVL_COLOR_LIGHTCYAN3 
 *  lightcyan3 */
/** \var CVL_COLOR_LIGHTCYAN4 
 *  lightcyan4 */
/** \var CVL_COLOR_PALETURQUOISE1 
 *  paleturquoise1 */
/** \var CVL_COLOR_PALETURQUOISE2 
 *  paleturquoise2 */
/** \var CVL_COLOR_PALETURQUOISE3 
 *  paleturquoise3 */
/** \var CVL_COLOR_PALETURQUOISE4 
 *  paleturquoise4 */
/** \var CVL_COLOR_CADETBLUE1 
 *  cadetblue1 */
/** \var CVL_COLOR_CADETBLUE2 
 *  cadetblue2 */
/** \var CVL_COLOR_CADETBLUE3 
 *  cadetblue3 */
/** \var CVL_COLOR_CADETBLUE4 
 *  cadetblue4 */
/** \var CVL_COLOR_TURQUOISE1 
 *  turquoise1 */
/** \var CVL_COLOR_TURQUOISE2 
 *  turquoise2 */
/** \var CVL_COLOR_TURQUOISE3 
 *  turquoise3 */
/** \var CVL_COLOR_TURQUOISE4 
 *  turquoise4 */
/** \var CVL_COLOR_CYAN1 
 *  cyan1 */
/** \var CVL_COLOR_CYAN2 
 *  cyan2 */
/** \var CVL_COLOR_CYAN3 
 *  cyan3 */
/** \var CVL_COLOR_CYAN4 
 *  cyan4 */
/** \var CVL_COLOR_DARKSLATEGRAY1 
 *  darkslategray1 */
/** \var CVL_COLOR_DARKSLATEGRAY2 
 *  darkslategray2 */
/** \var CVL_COLOR_DARKSLATEGRAY3 
 *  darkslategray3 */
/** \var CVL_COLOR_DARKSLATEGRAY4 
 *  darkslategray4 */
/** \var CVL_COLOR_AQUAMARINE1 
 *  aquamarine1 */
/** \var CVL_COLOR_AQUAMARINE2 
 *  aquamarine2 */
/** \var CVL_COLOR_AQUAMARINE3 
 *  aquamarine3 */
/** \var CVL_COLOR_AQUAMARINE4 
 *  aquamarine4 */
/** \var CVL_COLOR_DARKSEAGREEN1 
 *  darkseagreen1 */
/** \var CVL_COLOR_DARKSEAGREEN2 
 *  darkseagreen2 */
/** \var CVL_COLOR_DARKSEAGREEN3 
 *  darkseagreen3 */
/** \var CVL_COLOR_DARKSEAGREEN4 
 *  darkseagreen4 */
/** \var CVL_COLOR_SEAGREEN1 
 *  seagreen1 */
/** \var CVL_COLOR_SEAGREEN2 
 *  seagreen2 */
/** \var CVL_COLOR_SEAGREEN3 
 *  seagreen3 */
/** \var CVL_COLOR_SEAGREEN4 
 *  seagreen4 */
/** \var CVL_COLOR_PALEGREEN1 
 *  palegreen1 */
/** \var CVL_COLOR_PALEGREEN2 
 *  palegreen2 */
/** \var CVL_COLOR_PALEGREEN3 
 *  palegreen3 */
/** \var CVL_COLOR_PALEGREEN4 
 *  palegreen4 */
/** \var CVL_COLOR_SPRINGGREEN1 
 *  springgreen1 */
/** \var CVL_COLOR_SPRINGGREEN2 
 *  springgreen2 */
/** \var CVL_COLOR_SPRINGGREEN3 
 *  springgreen3 */
/** \var CVL_COLOR_SPRINGGREEN4 
 *  springgreen4 */
/** \var CVL_COLOR_GREEN1 
 *  green1 */
/** \var CVL_COLOR_GREEN2 
 *  green2 */
/** \var CVL_COLOR_GREEN3 
 *  green3 */
/** \var CVL_COLOR_GREEN4 
 *  green4 */
/** \var CVL_COLOR_CHARTREUSE1 
 *  chartreuse1 */
/** \var CVL_COLOR_CHARTREUSE2 
 *  chartreuse2 */
/** \var CVL_COLOR_CHARTREUSE3 
 *  chartreuse3 */
/** \var CVL_COLOR_CHARTREUSE4 
 *  chartreuse4 */
/** \var CVL_COLOR_OLIVEDRAB1 
 *  olivedrab1 */
/** \var CVL_COLOR_OLIVEDRAB2 
 *  olivedrab2 */
/** \var CVL_COLOR_OLIVEDRAB3 
 *  olivedrab3 */
/** \var CVL_COLOR_OLIVEDRAB4 
 *  olivedrab4 */
/** \var CVL_COLOR_DARKOLIVEGREEN1 
 *  darkolivegreen1 */
/** \var CVL_COLOR_DARKOLIVEGREEN2 
 *  darkolivegreen2 */
/** \var CVL_COLOR_DARKOLIVEGREEN3 
 *  darkolivegreen3 */
/** \var CVL_COLOR_DARKOLIVEGREEN4 
 *  darkolivegreen4 */
/** \var CVL_COLOR_KHAKI1 
 *  khaki1 */
/** \var CVL_COLOR_KHAKI2 
 *  khaki2 */
/** \var CVL_COLOR_KHAKI3 
 *  khaki3 */
/** \var CVL_COLOR_KHAKI4 
 *  khaki4 */
/** \var CVL_COLOR_LIGHTGOLDENROD1 
 *  lightgoldenrod1 */
/** \var CVL_COLOR_LIGHTGOLDENROD2 
 *  lightgoldenrod2 */
/** \var CVL_COLOR_LIGHTGOLDENROD3 
 *  lightgoldenrod3 */
/** \var CVL_COLOR_LIGHTGOLDENROD4 
 *  lightgoldenrod4 */
/** \var CVL_COLOR_LIGHTYELLOW1 
 *  lightyellow1 */
/** \var CVL_COLOR_LIGHTYELLOW2 
 *  lightyellow2 */
/** \var CVL_COLOR_LIGHTYELLOW3 
 *  lightyellow3 */
/** \var CVL_COLOR_LIGHTYELLOW4 
 *  lightyellow4 */
/** \var CVL_COLOR_YELLOW1 
 *  yellow1 */
/** \var CVL_COLOR_YELLOW2 
 *  yellow2 */
/** \var CVL_COLOR_YELLOW3 
 *  yellow3 */
/** \var CVL_COLOR_YELLOW4 
 *  yellow4 */
/** \var CVL_COLOR_GOLD1 
 *  gold1 */
/** \var CVL_COLOR_GOLD2 
 *  gold2 */
/** \var CVL_COLOR_GOLD3 
 *  gold3 */
/** \var CVL_COLOR_GOLD4 
 *  gold4 */
/** \var CVL_COLOR_GOLDENROD1 
 *  goldenrod1 */
/** \var CVL_COLOR_GOLDENROD2 
 *  goldenrod2 */
/** \var CVL_COLOR_GOLDENROD3 
 *  goldenrod3 */
/** \var CVL_COLOR_GOLDENROD4 
 *  goldenrod4 */
/** \var CVL_COLOR_DARKGOLDENROD1 
 *  darkgoldenrod1 */
/** \var CVL_COLOR_DARKGOLDENROD2 
 *  darkgoldenrod2 */
/** \var CVL_COLOR_DARKGOLDENROD3 
 *  darkgoldenrod3 */
/** \var CVL_COLOR_DARKGOLDENROD4 
 *  darkgoldenrod4 */
/** \var CVL_COLOR_ROSYBROWN1 
 *  rosybrown1 */
/** \var CVL_COLOR_ROSYBROWN2 
 *  rosybrown2 */
/** \var CVL_COLOR_ROSYBROWN3 
 *  rosybrown3 */
/** \var CVL_COLOR_ROSYBROWN4 
 *  rosybrown4 */
/** \var CVL_COLOR_INDIANRED1 
 *  indianred1 */
/** \var CVL_COLOR_INDIANRED2 
 *  indianred2 */
/** \var CVL_COLOR_INDIANRED3 
 *  indianred3 */
/** \var CVL_COLOR_INDIANRED4 
 *  indianred4 */
/** \var CVL_COLOR_SIENNA1 
 *  sienna1 */
/** \var CVL_COLOR_SIENNA2 
 *  sienna2 */
/** \var CVL_COLOR_SIENNA3 
 *  sienna3 */
/** \var CVL_COLOR_SIENNA4 
 *  sienna4 */
/** \var CVL_COLOR_BURLYWOOD1 
 *  burlywood1 */
/** \var CVL_COLOR_BURLYWOOD2 
 *  burlywood2 */
/** \var CVL_COLOR_BURLYWOOD3 
 *  burlywood3 */
/** \var CVL_COLOR_BURLYWOOD4 
 *  burlywood4 */
/** \var CVL_COLOR_WHEAT1 
 *  wheat1 */
/** \var CVL_COLOR_WHEAT2 
 *  wheat2 */
/** \var CVL_COLOR_WHEAT3 
 *  wheat3 */
/** \var CVL_COLOR_WHEAT4 
 *  wheat4 */
/** \var CVL_COLOR_TAN1 
 *  tan1 */
/** \var CVL_COLOR_TAN2 
 *  tan2 */
/** \var CVL_COLOR_TAN3 
 *  tan3 */
/** \var CVL_COLOR_TAN4 
 *  tan4 */
/** \var CVL_COLOR_CHOCOLATE1 
 *  chocolate1 */
/** \var CVL_COLOR_CHOCOLATE2 
 *  chocolate2 */
/** \var CVL_COLOR_CHOCOLATE3 
 *  chocolate3 */
/** \var CVL_COLOR_CHOCOLATE4 
 *  chocolate4 */
/** \var CVL_COLOR_FIREBRICK1 
 *  firebrick1 */
/** \var CVL_COLOR_FIREBRICK2 
 *  firebrick2 */
/** \var CVL_COLOR_FIREBRICK3 
 *  firebrick3 */
/** \var CVL_COLOR_FIREBRICK4 
 *  firebrick4 */
/** \var CVL_COLOR_BROWN1 
 *  brown1 */
/** \var CVL_COLOR_BROWN2 
 *  brown2 */
/** \var CVL_COLOR_BROWN3 
 *  brown3 */
/** \var CVL_COLOR_BROWN4 
 *  brown4 */
/** \var CVL_COLOR_SALMON1 
 *  salmon1 */
/** \var CVL_COLOR_SALMON2 
 *  salmon2 */
/** \var CVL_COLOR_SALMON3 
 *  salmon3 */
/** \var CVL_COLOR_SALMON4 
 *  salmon4 */
/** \var CVL_COLOR_LIGHTSALMON1 
 *  lightsalmon1 */
/** \var CVL_COLOR_LIGHTSALMON2 
 *  lightsalmon2 */
/** \var CVL_COLOR_LIGHTSALMON3 
 *  lightsalmon3 */
/** \var CVL_COLOR_LIGHTSALMON4 
 *  lightsalmon4 */
/** \var CVL_COLOR_ORANGE1 
 *  orange1 */
/** \var CVL_COLOR_ORANGE2 
 *  orange2 */
/** \var CVL_COLOR_ORANGE3 
 *  orange3 */
/** \var CVL_COLOR_ORANGE4 
 *  orange4 */
/** \var CVL_COLOR_DARKORANGE1 
 *  darkorange1 */
/** \var CVL_COLOR_DARKORANGE2 
 *  darkorange2 */
/** \var CVL_COLOR_DARKORANGE3 
 *  darkorange3 */
/** \var CVL_COLOR_DARKORANGE4 
 *  darkorange4 */
/** \var CVL_COLOR_CORAL1 
 *  coral1 */
/** \var CVL_COLOR_CORAL2 
 *  coral2 */
/** \var CVL_COLOR_CORAL3 
 *  coral3 */
/** \var CVL_COLOR_CORAL4 
 *  coral4 */
/** \var CVL_COLOR_TOMATO1 
 *  tomato1 */
/** \var CVL_COLOR_TOMATO2 
 *  tomato2 */
/** \var CVL_COLOR_TOMATO3 
 *  tomato3 */
/** \var CVL_COLOR_TOMATO4 
 *  tomato4 */
/** \var CVL_COLOR_ORANGERED1 
 *  orangered1 */
/** \var CVL_COLOR_ORANGERED2 
 *  orangered2 */
/** \var CVL_COLOR_ORANGERED3 
 *  orangered3 */
/** \var CVL_COLOR_ORANGERED4 
 *  orangered4 */
/** \var CVL_COLOR_RED1 
 *  red1 */
/** \var CVL_COLOR_RED2 
 *  red2 */
/** \var CVL_COLOR_RED3 
 *  red3 */
/** \var CVL_COLOR_RED4 
 *  red4 */
/** \var CVL_COLOR_DEEPPINK1 
 *  deeppink1 */
/** \var CVL_COLOR_DEEPPINK2 
 *  deeppink2 */
/** \var CVL_COLOR_DEEPPINK3 
 *  deeppink3 */
/** \var CVL_COLOR_DEEPPINK4 
 *  deeppink4 */
/** \var CVL_COLOR_HOTPINK1 
 *  hotpink1 */
/** \var CVL_COLOR_HOTPINK2 
 *  hotpink2 */
/** \var CVL_COLOR_HOTPINK3 
 *  hotpink3 */
/** \var CVL_COLOR_HOTPINK4 
 *  hotpink4 */
/** \var CVL_COLOR_PINK1 
 *  pink1 */
/** \var CVL_COLOR_PINK2 
 *  pink2 */
/** \var CVL_COLOR_PINK3 
 *  pink3 */
/** \var CVL_COLOR_PINK4 
 *  pink4 */
/** \var CVL_COLOR_LIGHTPINK1 
 *  lightpink1 */
/** \var CVL_COLOR_LIGHTPINK2 
 *  lightpink2 */
/** \var CVL_COLOR_LIGHTPINK3 
 *  lightpink3 */
/** \var CVL_COLOR_LIGHTPINK4 
 *  lightpink4 */
/** \var CVL_COLOR_PALEVIOLETRED1 
 *  palevioletred1 */
/** \var CVL_COLOR_PALEVIOLETRED2 
 *  palevioletred2 */
/** \var CVL_COLOR_PALEVIOLETRED3 
 *  palevioletred3 */
/** \var CVL_COLOR_PALEVIOLETRED4 
 *  palevioletred4 */
/** \var CVL_COLOR_MAROON1 
 *  maroon1 */
/** \var CVL_COLOR_MAROON2 
 *  maroon2 */
/** \var CVL_COLOR_MAROON3 
 *  maroon3 */
/** \var CVL_COLOR_MAROON4 
 *  maroon4 */
/** \var CVL_COLOR_VIOLETRED1 
 *  violetred1 */
/** \var CVL_COLOR_VIOLETRED2 
 *  violetred2 */
/** \var CVL_COLOR_VIOLETRED3 
 *  violetred3 */
/** \var CVL_COLOR_VIOLETRED4 
 *  violetred4 */
/** \var CVL_COLOR_MAGENTA1 
 *  magenta1 */
/** \var CVL_COLOR_MAGENTA2 
 *  magenta2 */
/** \var CVL_COLOR_MAGENTA3 
 *  magenta3 */
/** \var CVL_COLOR_MAGENTA4 
 *  magenta4 */
/** \var CVL_COLOR_ORCHID1 
 *  orchid1 */
/** \var CVL_COLOR_ORCHID2 
 *  orchid2 */
/** \var CVL_COLOR_ORCHID3 
 *  orchid3 */
/** \var CVL_COLOR_ORCHID4 
 *  orchid4 */
/** \var CVL_COLOR_PLUM1 
 *  plum1 */
/** \var CVL_COLOR_PLUM2 
 *  plum2 */
/** \var CVL_COLOR_PLUM3 
 *  plum3 */
/** \var CVL_COLOR_PLUM4 
 *  plum4 */
/** \var CVL_COLOR_MEDIUMORCHID1 
 *  mediumorchid1 */
/** \var CVL_COLOR_MEDIUMORCHID2 
 *  mediumorchid2 */
/** \var CVL_COLOR_MEDIUMORCHID3 
 *  mediumorchid3 */
/** \var CVL_COLOR_MEDIUMORCHID4 
 *  mediumorchid4 */
/** \var CVL_COLOR_DARKORCHID1 
 *  darkorchid1 */
/** \var CVL_COLOR_DARKORCHID2 
 *  darkorchid2 */
/** \var CVL_COLOR_DARKORCHID3 
 *  darkorchid3 */
/** \var CVL_COLOR_DARKORCHID4 
 *  darkorchid4 */
/** \var CVL_COLOR_PURPLE1 
 *  purple1 */
/** \var CVL_COLOR_PURPLE2 
 *  purple2 */
/** \var CVL_COLOR_PURPLE3 
 *  purple3 */
/** \var CVL_COLOR_PURPLE4 
 *  purple4 */
/** \var CVL_COLOR_MEDIUMPURPLE1 
 *  mediumpurple1 */
/** \var CVL_COLOR_MEDIUMPURPLE2 
 *  mediumpurple2 */
/** \var CVL_COLOR_MEDIUMPURPLE3 
 *  mediumpurple3 */
/** \var CVL_COLOR_MEDIUMPURPLE4 
 *  mediumpurple4 */
/** \var CVL_COLOR_THISTLE1 
 *  thistle1 */
/** \var CVL_COLOR_THISTLE2 
 *  thistle2 */
/** \var CVL_COLOR_THISTLE3 
 *  thistle3 */
/** \var CVL_COLOR_THISTLE4 
 *  thistle4 */
/** \var CVL_COLOR_GRAY0 
 *  gray0 */
/** \var CVL_COLOR_GREY0 
 *  grey0 */
/** \var CVL_COLOR_GRAY1 
 *  gray1 */
/** \var CVL_COLOR_GREY1 
 *  grey1 */
/** \var CVL_COLOR_GRAY2 
 *  gray2 */
/** \var CVL_COLOR_GREY2 
 *  grey2 */
/** \var CVL_COLOR_GRAY3 
 *  gray3 */
/** \var CVL_COLOR_GREY3 
 *  grey3 */
/** \var CVL_COLOR_GRAY4 
 *  gray4 */
/** \var CVL_COLOR_GREY4 
 *  grey4 */
/** \var CVL_COLOR_GRAY5 
 *  gray5 */
/** \var CVL_COLOR_GREY5 
 *  grey5 */
/** \var CVL_COLOR_GRAY6 
 *  gray6 */
/** \var CVL_COLOR_GREY6 
 *  grey6 */
/** \var CVL_COLOR_GRAY7 
 *  gray7 */
/** \var CVL_COLOR_GREY7 
 *  grey7 */
/** \var CVL_COLOR_GRAY8 
 *  gray8 */
/** \var CVL_COLOR_GREY8 
 *  grey8 */
/** \var CVL_COLOR_GRAY9 
 *  gray9 */
/** \var CVL_COLOR_GREY9 
 *  grey9 */
/** \var CVL_COLOR_GRAY10 
 *  gray10 */
/** \var CVL_COLOR_GREY10 
 *  grey10 */
/** \var CVL_COLOR_GRAY11 
 *  gray11 */
/** \var CVL_COLOR_GREY11 
 *  grey11 */
/** \var CVL_COLOR_GRAY12 
 *  gray12 */
/** \var CVL_COLOR_GREY12 
 *  grey12 */
/** \var CVL_COLOR_GRAY13 
 *  gray13 */
/** \var CVL_COLOR_GREY13 
 *  grey13 */
/** \var CVL_COLOR_GRAY14 
 *  gray14 */
/** \var CVL_COLOR_GREY14 
 *  grey14 */
/** \var CVL_COLOR_GRAY15 
 *  gray15 */
/** \var CVL_COLOR_GREY15 
 *  grey15 */
/** \var CVL_COLOR_GRAY16 
 *  gray16 */
/** \var CVL_COLOR_GREY16 
 *  grey16 */
/** \var CVL_COLOR_GRAY17 
 *  gray17 */
/** \var CVL_COLOR_GREY17 
 *  grey17 */
/** \var CVL_COLOR_GRAY18 
 *  gray18 */
/** \var CVL_COLOR_GREY18 
 *  grey18 */
/** \var CVL_COLOR_GRAY19 
 *  gray19 */
/** \var CVL_COLOR_GREY19 
 *  grey19 */
/** \var CVL_COLOR_GRAY20 
 *  gray20 */
/** \var CVL_COLOR_GREY20 
 *  grey20 */
/** \var CVL_COLOR_GRAY21 
 *  gray21 */
/** \var CVL_COLOR_GREY21 
 *  grey21 */
/** \var CVL_COLOR_GRAY22 
 *  gray22 */
/** \var CVL_COLOR_GREY22 
 *  grey22 */
/** \var CVL_COLOR_GRAY23 
 *  gray23 */
/** \var CVL_COLOR_GREY23 
 *  grey23 */
/** \var CVL_COLOR_GRAY24 
 *  gray24 */
/** \var CVL_COLOR_GREY24 
 *  grey24 */
/** \var CVL_COLOR_GRAY25 
 *  gray25 */
/** \var CVL_COLOR_GREY25 
 *  grey25 */
/** \var CVL_COLOR_GRAY26 
 *  gray26 */
/** \var CVL_COLOR_GREY26 
 *  grey26 */
/** \var CVL_COLOR_GRAY27 
 *  gray27 */
/** \var CVL_COLOR_GREY27 
 *  grey27 */
/** \var CVL_COLOR_GRAY28 
 *  gray28 */
/** \var CVL_COLOR_GREY28 
 *  grey28 */
/** \var CVL_COLOR_GRAY29 
 *  gray29 */
/** \var CVL_COLOR_GREY29 
 *  grey29 */
/** \var CVL_COLOR_GRAY30 
 *  gray30 */
/** \var CVL_COLOR_GREY30 
 *  grey30 */
/** \var CVL_COLOR_GRAY31 
 *  gray31 */
/** \var CVL_COLOR_GREY31 
 *  grey31 */
/** \var CVL_COLOR_GRAY32 
 *  gray32 */
/** \var CVL_COLOR_GREY32 
 *  grey32 */
/** \var CVL_COLOR_GRAY33 
 *  gray33 */
/** \var CVL_COLOR_GREY33 
 *  grey33 */
/** \var CVL_COLOR_GRAY34 
 *  gray34 */
/** \var CVL_COLOR_GREY34 
 *  grey34 */
/** \var CVL_COLOR_GRAY35 
 *  gray35 */
/** \var CVL_COLOR_GREY35 
 *  grey35 */
/** \var CVL_COLOR_GRAY36 
 *  gray36 */
/** \var CVL_COLOR_GREY36 
 *  grey36 */
/** \var CVL_COLOR_GRAY37 
 *  gray37 */
/** \var CVL_COLOR_GREY37 
 *  grey37 */
/** \var CVL_COLOR_GRAY38 
 *  gray38 */
/** \var CVL_COLOR_GREY38 
 *  grey38 */
/** \var CVL_COLOR_GRAY39 
 *  gray39 */
/** \var CVL_COLOR_GREY39 
 *  grey39 */
/** \var CVL_COLOR_GRAY40 
 *  gray40 */
/** \var CVL_COLOR_GREY40 
 *  grey40 */
/** \var CVL_COLOR_GRAY41 
 *  gray41 */
/** \var CVL_COLOR_GREY41 
 *  grey41 */
/** \var CVL_COLOR_GRAY42 
 *  gray42 */
/** \var CVL_COLOR_GREY42 
 *  grey42 */
/** \var CVL_COLOR_GRAY43 
 *  gray43 */
/** \var CVL_COLOR_GREY43 
 *  grey43 */
/** \var CVL_COLOR_GRAY44 
 *  gray44 */
/** \var CVL_COLOR_GREY44 
 *  grey44 */
/** \var CVL_COLOR_GRAY45 
 *  gray45 */
/** \var CVL_COLOR_GREY45 
 *  grey45 */
/** \var CVL_COLOR_GRAY46 
 *  gray46 */
/** \var CVL_COLOR_GREY46 
 *  grey46 */
/** \var CVL_COLOR_GRAY47 
 *  gray47 */
/** \var CVL_COLOR_GREY47 
 *  grey47 */
/** \var CVL_COLOR_GRAY48 
 *  gray48 */
/** \var CVL_COLOR_GREY48 
 *  grey48 */
/** \var CVL_COLOR_GRAY49 
 *  gray49 */
/** \var CVL_COLOR_GREY49 
 *  grey49 */
/** \var CVL_COLOR_GRAY50 
 *  gray50 */
/** \var CVL_COLOR_GREY50 
 *  grey50 */
/** \var CVL_COLOR_GRAY51 
 *  gray51 */
/** \var CVL_COLOR_GREY51 
 *  grey51 */
/** \var CVL_COLOR_GRAY52 
 *  gray52 */
/** \var CVL_COLOR_GREY52 
 *  grey52 */
/** \var CVL_COLOR_GRAY53 
 *  gray53 */
/** \var CVL_COLOR_GREY53 
 *  grey53 */
/** \var CVL_COLOR_GRAY54 
 *  gray54 */
/** \var CVL_COLOR_GREY54 
 *  grey54 */
/** \var CVL_COLOR_GRAY55 
 *  gray55 */
/** \var CVL_COLOR_GREY55 
 *  grey55 */
/** \var CVL_COLOR_GRAY56 
 *  gray56 */
/** \var CVL_COLOR_GREY56 
 *  grey56 */
/** \var CVL_COLOR_GRAY57 
 *  gray57 */
/** \var CVL_COLOR_GREY57 
 *  grey57 */
/** \var CVL_COLOR_GRAY58 
 *  gray58 */
/** \var CVL_COLOR_GREY58 
 *  grey58 */
/** \var CVL_COLOR_GRAY59 
 *  gray59 */
/** \var CVL_COLOR_GREY59 
 *  grey59 */
/** \var CVL_COLOR_GRAY60 
 *  gray60 */
/** \var CVL_COLOR_GREY60 
 *  grey60 */
/** \var CVL_COLOR_GRAY61 
 *  gray61 */
/** \var CVL_COLOR_GREY61 
 *  grey61 */
/** \var CVL_COLOR_GRAY62 
 *  gray62 */
/** \var CVL_COLOR_GREY62 
 *  grey62 */
/** \var CVL_COLOR_GRAY63 
 *  gray63 */
/** \var CVL_COLOR_GREY63 
 *  grey63 */
/** \var CVL_COLOR_GRAY64 
 *  gray64 */
/** \var CVL_COLOR_GREY64 
 *  grey64 */
/** \var CVL_COLOR_GRAY65 
 *  gray65 */
/** \var CVL_COLOR_GREY65 
 *  grey65 */
/** \var CVL_COLOR_GRAY66 
 *  gray66 */
/** \var CVL_COLOR_GREY66 
 *  grey66 */
/** \var CVL_COLOR_GRAY67 
 *  gray67 */
/** \var CVL_COLOR_GREY67 
 *  grey67 */
/** \var CVL_COLOR_GRAY68 
 *  gray68 */
/** \var CVL_COLOR_GREY68 
 *  grey68 */
/** \var CVL_COLOR_GRAY69 
 *  gray69 */
/** \var CVL_COLOR_GREY69 
 *  grey69 */
/** \var CVL_COLOR_GRAY70 
 *  gray70 */
/** \var CVL_COLOR_GREY70 
 *  grey70 */
/** \var CVL_COLOR_GRAY71 
 *  gray71 */
/** \var CVL_COLOR_GREY71 
 *  grey71 */
/** \var CVL_COLOR_GRAY72 
 *  gray72 */
/** \var CVL_COLOR_GREY72 
 *  grey72 */
/** \var CVL_COLOR_GRAY73 
 *  gray73 */
/** \var CVL_COLOR_GREY73 
 *  grey73 */
/** \var CVL_COLOR_GRAY74 
 *  gray74 */
/** \var CVL_COLOR_GREY74 
 *  grey74 */
/** \var CVL_COLOR_GRAY75 
 *  gray75 */
/** \var CVL_COLOR_GREY75 
 *  grey75 */
/** \var CVL_COLOR_GRAY76 
 *  gray76 */
/** \var CVL_COLOR_GREY76 
 *  grey76 */
/** \var CVL_COLOR_GRAY77 
 *  gray77 */
/** \var CVL_COLOR_GREY77 
 *  grey77 */
/** \var CVL_COLOR_GRAY78 
 *  gray78 */
/** \var CVL_COLOR_GREY78 
 *  grey78 */
/** \var CVL_COLOR_GRAY79 
 *  gray79 */
/** \var CVL_COLOR_GREY79 
 *  grey79 */
/** \var CVL_COLOR_GRAY80 
 *  gray80 */
/** \var CVL_COLOR_GREY80 
 *  grey80 */
/** \var CVL_COLOR_GRAY81 
 *  gray81 */
/** \var CVL_COLOR_GREY81 
 *  grey81 */
/** \var CVL_COLOR_GRAY82 
 *  gray82 */
/** \var CVL_COLOR_GREY82 
 *  grey82 */
/** \var CVL_COLOR_GRAY83 
 *  gray83 */
/** \var CVL_COLOR_GREY83 
 *  grey83 */
/** \var CVL_COLOR_GRAY84 
 *  gray84 */
/** \var CVL_COLOR_GREY84 
 *  grey84 */
/** \var CVL_COLOR_GRAY85 
 *  gray85 */
/** \var CVL_COLOR_GREY85 
 *  grey85 */
/** \var CVL_COLOR_GRAY86 
 *  gray86 */
/** \var CVL_COLOR_GREY86 
 *  grey86 */
/** \var CVL_COLOR_GRAY87 
 *  gray87 */
/** \var CVL_COLOR_GREY87 
 *  grey87 */
/** \var CVL_COLOR_GRAY88 
 *  gray88 */
/** \var CVL_COLOR_GREY88 
 *  grey88 */
/** \var CVL_COLOR_GRAY89 
 *  gray89 */
/** \var CVL_COLOR_GREY89 
 *  grey89 */
/** \var CVL_COLOR_GRAY90 
 *  gray90 */
/** \var CVL_COLOR_GREY90 
 *  grey90 */
/** \var CVL_COLOR_GRAY91 
 *  gray91 */
/** \var CVL_COLOR_GREY91 
 *  grey91 */
/** \var CVL_COLOR_GRAY92 
 *  gray92 */
/** \var CVL_COLOR_GREY92 
 *  grey92 */
/** \var CVL_COLOR_GRAY93 
 *  gray93 */
/** \var CVL_COLOR_GREY93 
 *  grey93 */
/** \var CVL_COLOR_GRAY94 
 *  gray94 */
/** \var CVL_COLOR_GREY94 
 *  grey94 */
/** \var CVL_COLOR_GRAY95 
 *  gray95 */
/** \var CVL_COLOR_GREY95 
 *  grey95 */
/** \var CVL_COLOR_GRAY96 
 *  gray96 */
/** \var CVL_COLOR_GREY96 
 *  grey96 */
/** \var CVL_COLOR_GRAY97 
 *  gray97 */
/** \var CVL_COLOR_GREY97 
 *  grey97 */
/** \var CVL_COLOR_GRAY98 
 *  gray98 */
/** \var CVL_COLOR_GREY98 
 *  grey98 */
/** \var CVL_COLOR_GRAY99 
 *  gray99 */
/** \var CVL_COLOR_GREY99 
 *  grey99 */
/** \var CVL_COLOR_GRAY100 
 *  gray100 */
/** \var CVL_COLOR_GREY100 
 *  grey100 */
/** \var CVL_COLOR_DARKGREY 
 *  darkgrey */
/** \var CVL_COLOR_DARKGRAY 
 *  darkgray */
/** \var CVL_COLOR_DARKBLUE 
 *  darkblue */
/** \var CVL_COLOR_DARKCYAN 
 *  darkcyan */
/** \var CVL_COLOR_DARKMAGENTA 
 *  darkmagenta */
/** \var CVL_COLOR_DARKRED 
 *  darkred */
/** \var CVL_COLOR_LIGHTGREEN 
 *  lightgreen */

/**
 * \enum cvl_binarize_local_threshold_type_t
 *
 * Chooses how to compute the threshold T from a neighborhood.
 */
/** \var CVL_BINARIZE_LOCAL_THRESHOLD_MEAN 
 *  Use the mean. */
/** \var CVL_BINARIZE_LOCAL_THRESHOLD_MEDIAN
 *  Use the median. */
/** \var CVL_BINARIZE_LOCAL_THRESHOLD_MINMAX
 *  Use the mean of the minimum and maximum. */



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
    { "alice blue", 		CVL_COLOR_ALICEBLUE }, 
    { "AliceBlue", 		CVL_COLOR_ALICEBLUE }, 
    { "antique white", 		CVL_COLOR_ANTIQUEWHITE }, 
    { "AntiqueWhite", 		CVL_COLOR_ANTIQUEWHITE }, 
    { "AntiqueWhite1", 		CVL_COLOR_ANTIQUEWHITE1 }, 
    { "AntiqueWhite2", 		CVL_COLOR_ANTIQUEWHITE2 }, 
    { "AntiqueWhite3", 		CVL_COLOR_ANTIQUEWHITE3 }, 
    { "AntiqueWhite4", 		CVL_COLOR_ANTIQUEWHITE4 }, 
    { "aquamarine", 		CVL_COLOR_AQUAMARINE }, 
    { "aquamarine1", 		CVL_COLOR_AQUAMARINE1 }, 
    { "aquamarine2", 		CVL_COLOR_AQUAMARINE2 }, 
    { "aquamarine3", 		CVL_COLOR_AQUAMARINE3 }, 
    { "aquamarine4", 		CVL_COLOR_AQUAMARINE4 }, 
    { "azure", 			CVL_COLOR_AZURE }, 
    { "azure1", 		CVL_COLOR_AZURE1 }, 
    { "azure2", 		CVL_COLOR_AZURE2 }, 
    { "azure3", 		CVL_COLOR_AZURE3 }, 
    { "azure4", 		CVL_COLOR_AZURE4 }, 
    { "beige", 			CVL_COLOR_BEIGE }, 
    { "bisque", 		CVL_COLOR_BISQUE }, 
    { "bisque1", 		CVL_COLOR_BISQUE1 }, 
    { "bisque2", 		CVL_COLOR_BISQUE2 }, 
    { "bisque3", 		CVL_COLOR_BISQUE3 }, 
    { "bisque4", 		CVL_COLOR_BISQUE4 }, 
    { "black", 			CVL_COLOR_BLACK }, 
    { "blanched almond", 	CVL_COLOR_BLANCHEDALMOND }, 
    { "BlanchedAlmond", 	CVL_COLOR_BLANCHEDALMOND }, 
    { "blue", 			CVL_COLOR_BLUE }, 
    { "blue violet", 		CVL_COLOR_BLUEVIOLET }, 
    { "blue1", 			CVL_COLOR_BLUE1 }, 
    { "blue2", 			CVL_COLOR_BLUE2 }, 
    { "blue3", 			CVL_COLOR_BLUE3 }, 
    { "blue4", 			CVL_COLOR_BLUE4 }, 
    { "BlueViolet", 		CVL_COLOR_BLUEVIOLET }, 
    { "brown", 			CVL_COLOR_BROWN }, 
    { "brown1", 		CVL_COLOR_BROWN1 }, 
    { "brown2", 		CVL_COLOR_BROWN2 }, 
    { "brown3", 		CVL_COLOR_BROWN3 }, 
    { "brown4", 		CVL_COLOR_BROWN4 }, 
    { "burlywood", 		CVL_COLOR_BURLYWOOD }, 
    { "burlywood1", 		CVL_COLOR_BURLYWOOD1 }, 
    { "burlywood2", 		CVL_COLOR_BURLYWOOD2 }, 
    { "burlywood3", 		CVL_COLOR_BURLYWOOD3 }, 
    { "burlywood4", 		CVL_COLOR_BURLYWOOD4 }, 
    { "cadet blue", 		CVL_COLOR_CADETBLUE }, 
    { "CadetBlue", 		CVL_COLOR_CADETBLUE }, 
    { "CadetBlue1", 		CVL_COLOR_CADETBLUE1 }, 
    { "CadetBlue2", 		CVL_COLOR_CADETBLUE2 }, 
    { "CadetBlue3", 		CVL_COLOR_CADETBLUE3 }, 
    { "CadetBlue4", 		CVL_COLOR_CADETBLUE4 }, 
    { "chartreuse", 		CVL_COLOR_CHARTREUSE }, 
    { "chartreuse1", 		CVL_COLOR_CHARTREUSE1 }, 
    { "chartreuse2", 		CVL_COLOR_CHARTREUSE2 }, 
    { "chartreuse3", 		CVL_COLOR_CHARTREUSE3 }, 
    { "chartreuse4", 		CVL_COLOR_CHARTREUSE4 }, 
    { "chocolate", 		CVL_COLOR_CHOCOLATE }, 
    { "chocolate1", 		CVL_COLOR_CHOCOLATE1 }, 
    { "chocolate2", 		CVL_COLOR_CHOCOLATE2 }, 
    { "chocolate3", 		CVL_COLOR_CHOCOLATE3 }, 
    { "chocolate4", 		CVL_COLOR_CHOCOLATE4 }, 
    { "coral", 			CVL_COLOR_CORAL }, 
    { "coral1", 		CVL_COLOR_CORAL1 }, 
    { "coral2", 		CVL_COLOR_CORAL2 }, 
    { "coral3", 		CVL_COLOR_CORAL3 }, 
    { "coral4", 		CVL_COLOR_CORAL4 }, 
    { "cornflower blue", 	CVL_COLOR_CORNFLOWERBLUE }, 
    { "CornflowerBlue", 	CVL_COLOR_CORNFLOWERBLUE }, 
    { "cornsilk", 		CVL_COLOR_CORNSILK }, 
    { "cornsilk1", 		CVL_COLOR_CORNSILK1 }, 
    { "cornsilk2", 		CVL_COLOR_CORNSILK2 }, 
    { "cornsilk3", 		CVL_COLOR_CORNSILK3 }, 
    { "cornsilk4", 		CVL_COLOR_CORNSILK4 }, 
    { "cyan", 			CVL_COLOR_CYAN }, 
    { "cyan1", 			CVL_COLOR_CYAN1 }, 
    { "cyan2", 			CVL_COLOR_CYAN2 }, 
    { "cyan3", 			CVL_COLOR_CYAN3 }, 
    { "cyan4", 			CVL_COLOR_CYAN4 }, 
    { "dark blue", 		CVL_COLOR_DARKBLUE }, 
    { "dark cyan", 		CVL_COLOR_DARKCYAN }, 
    { "dark goldenrod", 	CVL_COLOR_DARKGOLDENROD }, 
    { "dark gray", 		CVL_COLOR_DARKGRAY }, 
    { "dark green", 		CVL_COLOR_DARKGREEN }, 
    { "dark grey", 		CVL_COLOR_DARKGREY }, 
    { "dark khaki", 		CVL_COLOR_DARKKHAKI }, 
    { "dark magenta", 		CVL_COLOR_DARKMAGENTA }, 
    { "dark olive green", 	CVL_COLOR_DARKOLIVEGREEN }, 
    { "dark orange", 		CVL_COLOR_DARKORANGE }, 
    { "dark orchid", 		CVL_COLOR_DARKORCHID }, 
    { "dark red", 		CVL_COLOR_DARKRED }, 
    { "dark salmon", 		CVL_COLOR_DARKSALMON }, 
    { "dark sea green", 	CVL_COLOR_DARKSEAGREEN }, 
    { "dark slate blue", 	CVL_COLOR_DARKSLATEBLUE }, 
    { "dark slate gray", 	CVL_COLOR_DARKSLATEGRAY }, 
    { "dark slate grey", 	CVL_COLOR_DARKSLATEGREY }, 
    { "dark turquoise", 	CVL_COLOR_DARKTURQUOISE }, 
    { "dark violet", 		CVL_COLOR_DARKVIOLET }, 
    { "DarkBlue", 		CVL_COLOR_DARKBLUE }, 
    { "DarkCyan", 		CVL_COLOR_DARKCYAN }, 
    { "DarkGoldenrod", 		CVL_COLOR_DARKGOLDENROD }, 
    { "DarkGoldenrod1", 	CVL_COLOR_DARKGOLDENROD1 }, 
    { "DarkGoldenrod2", 	CVL_COLOR_DARKGOLDENROD2 }, 
    { "DarkGoldenrod3", 	CVL_COLOR_DARKGOLDENROD3 }, 
    { "DarkGoldenrod4", 	CVL_COLOR_DARKGOLDENROD4 }, 
    { "DarkGray", 		CVL_COLOR_DARKGRAY }, 
    { "DarkGreen", 		CVL_COLOR_DARKGREEN }, 
    { "DarkGrey", 		CVL_COLOR_DARKGREY }, 
    { "DarkKhaki", 		CVL_COLOR_DARKKHAKI }, 
    { "DarkMagenta", 		CVL_COLOR_DARKMAGENTA }, 
    { "DarkOliveGreen", 	CVL_COLOR_DARKOLIVEGREEN }, 
    { "DarkOliveGreen1", 	CVL_COLOR_DARKOLIVEGREEN1 }, 
    { "DarkOliveGreen2", 	CVL_COLOR_DARKOLIVEGREEN2 }, 
    { "DarkOliveGreen3", 	CVL_COLOR_DARKOLIVEGREEN3 }, 
    { "DarkOliveGreen4", 	CVL_COLOR_DARKOLIVEGREEN4 }, 
    { "DarkOrange", 		CVL_COLOR_DARKORANGE }, 
    { "DarkOrange1", 		CVL_COLOR_DARKORANGE1 }, 
    { "DarkOrange2", 		CVL_COLOR_DARKORANGE2 }, 
    { "DarkOrange3", 		CVL_COLOR_DARKORANGE3 }, 
    { "DarkOrange4", 		CVL_COLOR_DARKORANGE4 }, 
    { "DarkOrchid", 		CVL_COLOR_DARKORCHID }, 
    { "DarkOrchid1", 		CVL_COLOR_DARKORCHID1 }, 
    { "DarkOrchid2", 		CVL_COLOR_DARKORCHID2 }, 
    { "DarkOrchid3", 		CVL_COLOR_DARKORCHID3 }, 
    { "DarkOrchid4", 		CVL_COLOR_DARKORCHID4 }, 
    { "DarkRed", 		CVL_COLOR_DARKRED }, 
    { "DarkSalmon", 		CVL_COLOR_DARKSALMON }, 
    { "DarkSeaGreen", 		CVL_COLOR_DARKSEAGREEN }, 
    { "DarkSeaGreen1", 		CVL_COLOR_DARKSEAGREEN1 }, 
    { "DarkSeaGreen2", 		CVL_COLOR_DARKSEAGREEN2 }, 
    { "DarkSeaGreen3", 		CVL_COLOR_DARKSEAGREEN3 }, 
    { "DarkSeaGreen4", 		CVL_COLOR_DARKSEAGREEN4 }, 
    { "DarkSlateBlue", 		CVL_COLOR_DARKSLATEBLUE }, 
    { "DarkSlateGray", 		CVL_COLOR_DARKSLATEGRAY }, 
    { "DarkSlateGray1", 	CVL_COLOR_DARKSLATEGRAY1 }, 
    { "DarkSlateGray2", 	CVL_COLOR_DARKSLATEGRAY2 }, 
    { "DarkSlateGray3", 	CVL_COLOR_DARKSLATEGRAY3 }, 
    { "DarkSlateGray4", 	CVL_COLOR_DARKSLATEGRAY4 }, 
    { "DarkSlateGrey", 		CVL_COLOR_DARKSLATEGREY }, 
    { "DarkTurquoise", 		CVL_COLOR_DARKTURQUOISE }, 
    { "DarkViolet", 		CVL_COLOR_DARKVIOLET }, 
    { "deep pink", 		CVL_COLOR_DEEPPINK }, 
    { "deep sky blue", 		CVL_COLOR_DEEPSKYBLUE }, 
    { "DeepPink", 		CVL_COLOR_DEEPPINK }, 
    { "DeepPink1", 		CVL_COLOR_DEEPPINK1 }, 
    { "DeepPink2", 		CVL_COLOR_DEEPPINK2 }, 
    { "DeepPink3", 		CVL_COLOR_DEEPPINK3 }, 
    { "DeepPink4", 		CVL_COLOR_DEEPPINK4 }, 
    { "DeepSkyBlue", 		CVL_COLOR_DEEPSKYBLUE }, 
    { "DeepSkyBlue1", 		CVL_COLOR_DEEPSKYBLUE1 }, 
    { "DeepSkyBlue2", 		CVL_COLOR_DEEPSKYBLUE2 }, 
    { "DeepSkyBlue3", 		CVL_COLOR_DEEPSKYBLUE3 }, 
    { "DeepSkyBlue4", 		CVL_COLOR_DEEPSKYBLUE4 }, 
    { "dim gray", 		CVL_COLOR_DIMGRAY }, 
    { "dim grey", 		CVL_COLOR_DIMGREY }, 
    { "DimGray", 		CVL_COLOR_DIMGRAY }, 
    { "DimGrey", 		CVL_COLOR_DIMGREY }, 
    { "dodger blue", 		CVL_COLOR_DODGERBLUE }, 
    { "DodgerBlue", 		CVL_COLOR_DODGERBLUE }, 
    { "DodgerBlue1", 		CVL_COLOR_DODGERBLUE1 }, 
    { "DodgerBlue2", 		CVL_COLOR_DODGERBLUE2 }, 
    { "DodgerBlue3", 		CVL_COLOR_DODGERBLUE3 }, 
    { "DodgerBlue4", 		CVL_COLOR_DODGERBLUE4 }, 
    { "firebrick", 		CVL_COLOR_FIREBRICK }, 
    { "firebrick1", 		CVL_COLOR_FIREBRICK1 }, 
    { "firebrick2", 		CVL_COLOR_FIREBRICK2 }, 
    { "firebrick3", 		CVL_COLOR_FIREBRICK3 }, 
    { "firebrick4", 		CVL_COLOR_FIREBRICK4 }, 
    { "floral white", 		CVL_COLOR_FLORALWHITE }, 
    { "FloralWhite", 		CVL_COLOR_FLORALWHITE }, 
    { "forest green", 		CVL_COLOR_FORESTGREEN }, 
    { "ForestGreen", 		CVL_COLOR_FORESTGREEN }, 
    { "gainsboro", 		CVL_COLOR_GAINSBORO }, 
    { "ghost white", 		CVL_COLOR_GHOSTWHITE }, 
    { "GhostWhite", 		CVL_COLOR_GHOSTWHITE }, 
    { "gold", 			CVL_COLOR_GOLD }, 
    { "gold1", 			CVL_COLOR_GOLD1 }, 
    { "gold2", 			CVL_COLOR_GOLD2 }, 
    { "gold3", 			CVL_COLOR_GOLD3 }, 
    { "gold4", 			CVL_COLOR_GOLD4 }, 
    { "goldenrod", 		CVL_COLOR_GOLDENROD }, 
    { "goldenrod1", 		CVL_COLOR_GOLDENROD1 }, 
    { "goldenrod2", 		CVL_COLOR_GOLDENROD2 }, 
    { "goldenrod3", 		CVL_COLOR_GOLDENROD3 }, 
    { "goldenrod4", 		CVL_COLOR_GOLDENROD4 }, 
    { "gray", 			CVL_COLOR_GRAY }, 
    { "gray0", 			CVL_COLOR_GRAY0 }, 
    { "gray1", 			CVL_COLOR_GRAY1 }, 
    { "gray10", 		CVL_COLOR_GRAY10 }, 
    { "gray100", 		CVL_COLOR_GRAY100 }, 
    { "gray11", 		CVL_COLOR_GRAY11 }, 
    { "gray12", 		CVL_COLOR_GRAY12 }, 
    { "gray13", 		CVL_COLOR_GRAY13 }, 
    { "gray14", 		CVL_COLOR_GRAY14 }, 
    { "gray15", 		CVL_COLOR_GRAY15 }, 
    { "gray16", 		CVL_COLOR_GRAY16 }, 
    { "gray17", 		CVL_COLOR_GRAY17 }, 
    { "gray18", 		CVL_COLOR_GRAY18 }, 
    { "gray19", 		CVL_COLOR_GRAY19 }, 
    { "gray2", 			CVL_COLOR_GRAY2 }, 
    { "gray20", 		CVL_COLOR_GRAY20 }, 
    { "gray21", 		CVL_COLOR_GRAY21 }, 
    { "gray22", 		CVL_COLOR_GRAY22 }, 
    { "gray23", 		CVL_COLOR_GRAY23 }, 
    { "gray24", 		CVL_COLOR_GRAY24 }, 
    { "gray25", 		CVL_COLOR_GRAY25 }, 
    { "gray26", 		CVL_COLOR_GRAY26 }, 
    { "gray27", 		CVL_COLOR_GRAY27 }, 
    { "gray28", 		CVL_COLOR_GRAY28 }, 
    { "gray29", 		CVL_COLOR_GRAY29 }, 
    { "gray3", 			CVL_COLOR_GRAY3 }, 
    { "gray30", 		CVL_COLOR_GRAY30 }, 
    { "gray31", 		CVL_COLOR_GRAY31 }, 
    { "gray32", 		CVL_COLOR_GRAY32 }, 
    { "gray33", 		CVL_COLOR_GRAY33 }, 
    { "gray34", 		CVL_COLOR_GRAY34 }, 
    { "gray35", 		CVL_COLOR_GRAY35 }, 
    { "gray36", 		CVL_COLOR_GRAY36 }, 
    { "gray37", 		CVL_COLOR_GRAY37 }, 
    { "gray38", 		CVL_COLOR_GRAY38 }, 
    { "gray39", 		CVL_COLOR_GRAY39 }, 
    { "gray4", 			CVL_COLOR_GRAY4 }, 
    { "gray40", 		CVL_COLOR_GRAY40 }, 
    { "gray41", 		CVL_COLOR_GRAY41 }, 
    { "gray42", 		CVL_COLOR_GRAY42 }, 
    { "gray43", 		CVL_COLOR_GRAY43 }, 
    { "gray44", 		CVL_COLOR_GRAY44 }, 
    { "gray45", 		CVL_COLOR_GRAY45 }, 
    { "gray46", 		CVL_COLOR_GRAY46 }, 
    { "gray47", 		CVL_COLOR_GRAY47 }, 
    { "gray48", 		CVL_COLOR_GRAY48 }, 
    { "gray49", 		CVL_COLOR_GRAY49 }, 
    { "gray5", 			CVL_COLOR_GRAY5 }, 
    { "gray50", 		CVL_COLOR_GRAY50 }, 
    { "gray51", 		CVL_COLOR_GRAY51 }, 
    { "gray52", 		CVL_COLOR_GRAY52 }, 
    { "gray53", 		CVL_COLOR_GRAY53 }, 
    { "gray54", 		CVL_COLOR_GRAY54 }, 
    { "gray55", 		CVL_COLOR_GRAY55 }, 
    { "gray56", 		CVL_COLOR_GRAY56 }, 
    { "gray57", 		CVL_COLOR_GRAY57 }, 
    { "gray58", 		CVL_COLOR_GRAY58 }, 
    { "gray59", 		CVL_COLOR_GRAY59 }, 
    { "gray6", 			CVL_COLOR_GRAY6 }, 
    { "gray60", 		CVL_COLOR_GRAY60 }, 
    { "gray61", 		CVL_COLOR_GRAY61 }, 
    { "gray62", 		CVL_COLOR_GRAY62 }, 
    { "gray63", 		CVL_COLOR_GRAY63 }, 
    { "gray64", 		CVL_COLOR_GRAY64 }, 
    { "gray65", 		CVL_COLOR_GRAY65 }, 
    { "gray66", 		CVL_COLOR_GRAY66 }, 
    { "gray67", 		CVL_COLOR_GRAY67 }, 
    { "gray68", 		CVL_COLOR_GRAY68 }, 
    { "gray69", 		CVL_COLOR_GRAY69 }, 
    { "gray7", 			CVL_COLOR_GRAY7 }, 
    { "gray70", 		CVL_COLOR_GRAY70 }, 
    { "gray71", 		CVL_COLOR_GRAY71 }, 
    { "gray72", 		CVL_COLOR_GRAY72 }, 
    { "gray73", 		CVL_COLOR_GRAY73 }, 
    { "gray74", 		CVL_COLOR_GRAY74 }, 
    { "gray75", 		CVL_COLOR_GRAY75 }, 
    { "gray76", 		CVL_COLOR_GRAY76 }, 
    { "gray77", 		CVL_COLOR_GRAY77 }, 
    { "gray78", 		CVL_COLOR_GRAY78 }, 
    { "gray79", 		CVL_COLOR_GRAY79 }, 
    { "gray8", 			CVL_COLOR_GRAY8 }, 
    { "gray80", 		CVL_COLOR_GRAY80 }, 
    { "gray81", 		CVL_COLOR_GRAY81 }, 
    { "gray82", 		CVL_COLOR_GRAY82 }, 
    { "gray83", 		CVL_COLOR_GRAY83 }, 
    { "gray84", 		CVL_COLOR_GRAY84 }, 
    { "gray85", 		CVL_COLOR_GRAY85 }, 
    { "gray86", 		CVL_COLOR_GRAY86 }, 
    { "gray87", 		CVL_COLOR_GRAY87 }, 
    { "gray88", 		CVL_COLOR_GRAY88 }, 
    { "gray89", 		CVL_COLOR_GRAY89 }, 
    { "gray9", 			CVL_COLOR_GRAY9 }, 
    { "gray90", 		CVL_COLOR_GRAY90 }, 
    { "gray91", 		CVL_COLOR_GRAY91 }, 
    { "gray92", 		CVL_COLOR_GRAY92 }, 
    { "gray93", 		CVL_COLOR_GRAY93 }, 
    { "gray94", 		CVL_COLOR_GRAY94 }, 
    { "gray95", 		CVL_COLOR_GRAY95 }, 
    { "gray96", 		CVL_COLOR_GRAY96 }, 
    { "gray97", 		CVL_COLOR_GRAY97 }, 
    { "gray98", 		CVL_COLOR_GRAY98 }, 
    { "gray99", 		CVL_COLOR_GRAY99 }, 
    { "green", 			CVL_COLOR_GREEN }, 
    { "green yellow", 		CVL_COLOR_GREENYELLOW }, 
    { "green1", 		CVL_COLOR_GREEN1 }, 
    { "green2", 		CVL_COLOR_GREEN2 }, 
    { "green3", 		CVL_COLOR_GREEN3 }, 
    { "green4", 		CVL_COLOR_GREEN4 }, 
    { "GreenYellow", 		CVL_COLOR_GREENYELLOW }, 
    { "grey", 			CVL_COLOR_GREY }, 
    { "grey0", 			CVL_COLOR_GREY0 }, 
    { "grey1", 			CVL_COLOR_GREY1 }, 
    { "grey10", 		CVL_COLOR_GREY10 }, 
    { "grey100", 		CVL_COLOR_GREY100 }, 
    { "grey11", 		CVL_COLOR_GREY11 }, 
    { "grey12", 		CVL_COLOR_GREY12 }, 
    { "grey13", 		CVL_COLOR_GREY13 }, 
    { "grey14", 		CVL_COLOR_GREY14 }, 
    { "grey15", 		CVL_COLOR_GREY15 }, 
    { "grey16", 		CVL_COLOR_GREY16 }, 
    { "grey17", 		CVL_COLOR_GREY17 }, 
    { "grey18", 		CVL_COLOR_GREY18 }, 
    { "grey19", 		CVL_COLOR_GREY19 }, 
    { "grey2", 			CVL_COLOR_GREY2 }, 
    { "grey20", 		CVL_COLOR_GREY20 }, 
    { "grey21", 		CVL_COLOR_GREY21 }, 
    { "grey22", 		CVL_COLOR_GREY22 }, 
    { "grey23", 		CVL_COLOR_GREY23 }, 
    { "grey24", 		CVL_COLOR_GREY24 }, 
    { "grey25", 		CVL_COLOR_GREY25 }, 
    { "grey26", 		CVL_COLOR_GREY26 }, 
    { "grey27", 		CVL_COLOR_GREY27 }, 
    { "grey28", 		CVL_COLOR_GREY28 }, 
    { "grey29", 		CVL_COLOR_GREY29 }, 
    { "grey3", 			CVL_COLOR_GREY3 }, 
    { "grey30", 		CVL_COLOR_GREY30 }, 
    { "grey31", 		CVL_COLOR_GREY31 }, 
    { "grey32", 		CVL_COLOR_GREY32 }, 
    { "grey33", 		CVL_COLOR_GREY33 }, 
    { "grey34", 		CVL_COLOR_GREY34 }, 
    { "grey35", 		CVL_COLOR_GREY35 }, 
    { "grey36", 		CVL_COLOR_GREY36 }, 
    { "grey37", 		CVL_COLOR_GREY37 }, 
    { "grey38", 		CVL_COLOR_GREY38 }, 
    { "grey39", 		CVL_COLOR_GREY39 }, 
    { "grey4", 			CVL_COLOR_GREY4 }, 
    { "grey40", 		CVL_COLOR_GREY40 }, 
    { "grey41", 		CVL_COLOR_GREY41 }, 
    { "grey42", 		CVL_COLOR_GREY42 }, 
    { "grey43", 		CVL_COLOR_GREY43 }, 
    { "grey44", 		CVL_COLOR_GREY44 }, 
    { "grey45", 		CVL_COLOR_GREY45 }, 
    { "grey46", 		CVL_COLOR_GREY46 }, 
    { "grey47", 		CVL_COLOR_GREY47 }, 
    { "grey48", 		CVL_COLOR_GREY48 }, 
    { "grey49", 		CVL_COLOR_GREY49 }, 
    { "grey5", 			CVL_COLOR_GREY5 }, 
    { "grey50", 		CVL_COLOR_GREY50 }, 
    { "grey51", 		CVL_COLOR_GREY51 }, 
    { "grey52", 		CVL_COLOR_GREY52 }, 
    { "grey53", 		CVL_COLOR_GREY53 }, 
    { "grey54", 		CVL_COLOR_GREY54 }, 
    { "grey55", 		CVL_COLOR_GREY55 }, 
    { "grey56", 		CVL_COLOR_GREY56 }, 
    { "grey57", 		CVL_COLOR_GREY57 }, 
    { "grey58", 		CVL_COLOR_GREY58 }, 
    { "grey59", 		CVL_COLOR_GREY59 }, 
    { "grey6", 			CVL_COLOR_GREY6 }, 
    { "grey60", 		CVL_COLOR_GREY60 }, 
    { "grey61", 		CVL_COLOR_GREY61 }, 
    { "grey62", 		CVL_COLOR_GREY62 }, 
    { "grey63", 		CVL_COLOR_GREY63 }, 
    { "grey64", 		CVL_COLOR_GREY64 }, 
    { "grey65", 		CVL_COLOR_GREY65 }, 
    { "grey66", 		CVL_COLOR_GREY66 }, 
    { "grey67", 		CVL_COLOR_GREY67 }, 
    { "grey68", 		CVL_COLOR_GREY68 }, 
    { "grey69", 		CVL_COLOR_GREY69 }, 
    { "grey7", 			CVL_COLOR_GREY7 }, 
    { "grey70", 		CVL_COLOR_GREY70 }, 
    { "grey71", 		CVL_COLOR_GREY71 }, 
    { "grey72", 		CVL_COLOR_GREY72 }, 
    { "grey73", 		CVL_COLOR_GREY73 }, 
    { "grey74", 		CVL_COLOR_GREY74 }, 
    { "grey75", 		CVL_COLOR_GREY75 }, 
    { "grey76", 		CVL_COLOR_GREY76 }, 
    { "grey77", 		CVL_COLOR_GREY77 }, 
    { "grey78", 		CVL_COLOR_GREY78 }, 
    { "grey79", 		CVL_COLOR_GREY79 }, 
    { "grey8", 			CVL_COLOR_GREY8 }, 
    { "grey80", 		CVL_COLOR_GREY80 }, 
    { "grey81", 		CVL_COLOR_GREY81 }, 
    { "grey82", 		CVL_COLOR_GREY82 }, 
    { "grey83", 		CVL_COLOR_GREY83 }, 
    { "grey84", 		CVL_COLOR_GREY84 }, 
    { "grey85", 		CVL_COLOR_GREY85 }, 
    { "grey86", 		CVL_COLOR_GREY86 }, 
    { "grey87", 		CVL_COLOR_GREY87 }, 
    { "grey88", 		CVL_COLOR_GREY88 }, 
    { "grey89", 		CVL_COLOR_GREY89 }, 
    { "grey9", 			CVL_COLOR_GREY9 }, 
    { "grey90", 		CVL_COLOR_GREY90 }, 
    { "grey91", 		CVL_COLOR_GREY91 }, 
    { "grey92", 		CVL_COLOR_GREY92 }, 
    { "grey93", 		CVL_COLOR_GREY93 }, 
    { "grey94", 		CVL_COLOR_GREY94 }, 
    { "grey95", 		CVL_COLOR_GREY95 }, 
    { "grey96", 		CVL_COLOR_GREY96 }, 
    { "grey97", 		CVL_COLOR_GREY97 }, 
    { "grey98", 		CVL_COLOR_GREY98 }, 
    { "grey99", 		CVL_COLOR_GREY99 }, 
    { "honeydew", 		CVL_COLOR_HONEYDEW }, 
    { "honeydew1", 		CVL_COLOR_HONEYDEW1 }, 
    { "honeydew2", 		CVL_COLOR_HONEYDEW2 }, 
    { "honeydew3", 		CVL_COLOR_HONEYDEW3 }, 
    { "honeydew4", 		CVL_COLOR_HONEYDEW4 }, 
    { "hot pink", 		CVL_COLOR_HOTPINK }, 
    { "HotPink", 		CVL_COLOR_HOTPINK }, 
    { "HotPink1", 		CVL_COLOR_HOTPINK1 }, 
    { "HotPink2", 		CVL_COLOR_HOTPINK2 }, 
    { "HotPink3", 		CVL_COLOR_HOTPINK3 }, 
    { "HotPink4", 		CVL_COLOR_HOTPINK4 }, 
    { "indian red", 		CVL_COLOR_INDIANRED }, 
    { "IndianRed", 		CVL_COLOR_INDIANRED }, 
    { "IndianRed1", 		CVL_COLOR_INDIANRED1 }, 
    { "IndianRed2", 		CVL_COLOR_INDIANRED2 }, 
    { "IndianRed3", 		CVL_COLOR_INDIANRED3 }, 
    { "IndianRed4", 		CVL_COLOR_INDIANRED4 }, 
    { "ivory", 			CVL_COLOR_IVORY }, 
    { "ivory1", 		CVL_COLOR_IVORY1 }, 
    { "ivory2", 		CVL_COLOR_IVORY2 }, 
    { "ivory3", 		CVL_COLOR_IVORY3 }, 
    { "ivory4", 		CVL_COLOR_IVORY4 }, 
    { "khaki", 			CVL_COLOR_KHAKI }, 
    { "khaki1", 		CVL_COLOR_KHAKI1 }, 
    { "khaki2", 		CVL_COLOR_KHAKI2 }, 
    { "khaki3", 		CVL_COLOR_KHAKI3 }, 
    { "khaki4", 		CVL_COLOR_KHAKI4 }, 
    { "lavender", 		CVL_COLOR_LAVENDER }, 
    { "lavender blush", 	CVL_COLOR_LAVENDERBLUSH }, 
    { "LavenderBlush", 		CVL_COLOR_LAVENDERBLUSH }, 
    { "LavenderBlush1", 	CVL_COLOR_LAVENDERBLUSH1 }, 
    { "LavenderBlush2", 	CVL_COLOR_LAVENDERBLUSH2 }, 
    { "LavenderBlush3", 	CVL_COLOR_LAVENDERBLUSH3 }, 
    { "LavenderBlush4", 	CVL_COLOR_LAVENDERBLUSH4 }, 
    { "lawn green", 		CVL_COLOR_LAWNGREEN }, 
    { "LawnGreen", 		CVL_COLOR_LAWNGREEN }, 
    { "lemon chiffon", 		CVL_COLOR_LEMONCHIFFON }, 
    { "LemonChiffon", 		CVL_COLOR_LEMONCHIFFON }, 
    { "LemonChiffon1", 		CVL_COLOR_LEMONCHIFFON1 }, 
    { "LemonChiffon2", 		CVL_COLOR_LEMONCHIFFON2 }, 
    { "LemonChiffon3", 		CVL_COLOR_LEMONCHIFFON3 }, 
    { "LemonChiffon4", 		CVL_COLOR_LEMONCHIFFON4 }, 
    { "light blue", 		CVL_COLOR_LIGHTBLUE }, 
    { "light coral", 		CVL_COLOR_LIGHTCORAL }, 
    { "light cyan", 		CVL_COLOR_LIGHTCYAN }, 
    { "light goldenrod yellow", CVL_COLOR_LIGHTGOLDENRODYELLOW }, 
    { "light goldenrod", 	CVL_COLOR_LIGHTGOLDENROD }, 
    { "light gray", 		CVL_COLOR_LIGHTGRAY }, 
    { "light green", 		CVL_COLOR_LIGHTGREEN }, 
    { "light grey", 		CVL_COLOR_LIGHTGREY }, 
    { "light pink", 		CVL_COLOR_LIGHTPINK }, 
    { "light salmon", 		CVL_COLOR_LIGHTSALMON }, 
    { "light sea green", 	CVL_COLOR_LIGHTSEAGREEN }, 
    { "light sky blue", 	CVL_COLOR_LIGHTSKYBLUE }, 
    { "light slate blue", 	CVL_COLOR_LIGHTSLATEBLUE }, 
    { "light slate gray", 	CVL_COLOR_LIGHTSLATEGRAY }, 
    { "light slate grey", 	CVL_COLOR_LIGHTSLATEGREY }, 
    { "light steel blue", 	CVL_COLOR_LIGHTSTEELBLUE }, 
    { "light yellow", 		CVL_COLOR_LIGHTYELLOW }, 
    { "LightBlue", 		CVL_COLOR_LIGHTBLUE }, 
    { "LightBlue1", 		CVL_COLOR_LIGHTBLUE1 }, 
    { "LightBlue2", 		CVL_COLOR_LIGHTBLUE2 }, 
    { "LightBlue3", 		CVL_COLOR_LIGHTBLUE3 }, 
    { "LightBlue4", 		CVL_COLOR_LIGHTBLUE4 }, 
    { "LightCoral", 		CVL_COLOR_LIGHTCORAL }, 
    { "LightCyan", 		CVL_COLOR_LIGHTCYAN }, 
    { "LightCyan1", 		CVL_COLOR_LIGHTCYAN1 }, 
    { "LightCyan2", 		CVL_COLOR_LIGHTCYAN2 }, 
    { "LightCyan3", 		CVL_COLOR_LIGHTCYAN3 }, 
    { "LightCyan4", 		CVL_COLOR_LIGHTCYAN4 }, 
    { "LightGoldenrod", 	CVL_COLOR_LIGHTGOLDENROD }, 
    { "LightGoldenrod1", 	CVL_COLOR_LIGHTGOLDENROD1 }, 
    { "LightGoldenrod2", 	CVL_COLOR_LIGHTGOLDENROD2 }, 
    { "LightGoldenrod3", 	CVL_COLOR_LIGHTGOLDENROD3 }, 
    { "LightGoldenrod4", 	CVL_COLOR_LIGHTGOLDENROD4 }, 
    { "LightGoldenrodYellow", 	CVL_COLOR_LIGHTGOLDENRODYELLOW }, 
    { "LightGray", 		CVL_COLOR_LIGHTGRAY }, 
    { "LightGreen", 		CVL_COLOR_LIGHTGREEN }, 
    { "LightGrey", 		CVL_COLOR_LIGHTGREY }, 
    { "LightPink", 		CVL_COLOR_LIGHTPINK }, 
    { "LightPink1", 		CVL_COLOR_LIGHTPINK1 }, 
    { "LightPink2", 		CVL_COLOR_LIGHTPINK2 }, 
    { "LightPink3", 		CVL_COLOR_LIGHTPINK3 }, 
    { "LightPink4", 		CVL_COLOR_LIGHTPINK4 }, 
    { "LightSalmon", 		CVL_COLOR_LIGHTSALMON }, 
    { "LightSalmon1", 		CVL_COLOR_LIGHTSALMON1 }, 
    { "LightSalmon2", 		CVL_COLOR_LIGHTSALMON2 }, 
    { "LightSalmon3", 		CVL_COLOR_LIGHTSALMON3 }, 
    { "LightSalmon4", 		CVL_COLOR_LIGHTSALMON4 }, 
    { "LightSeaGreen", 		CVL_COLOR_LIGHTSEAGREEN }, 
    { "LightSkyBlue", 		CVL_COLOR_LIGHTSKYBLUE }, 
    { "LightSkyBlue1", 		CVL_COLOR_LIGHTSKYBLUE1 }, 
    { "LightSkyBlue2", 		CVL_COLOR_LIGHTSKYBLUE2 }, 
    { "LightSkyBlue3", 		CVL_COLOR_LIGHTSKYBLUE3 }, 
    { "LightSkyBlue4", 		CVL_COLOR_LIGHTSKYBLUE4 }, 
    { "LightSlateBlue", 	CVL_COLOR_LIGHTSLATEBLUE }, 
    { "LightSlateGray", 	CVL_COLOR_LIGHTSLATEGRAY }, 
    { "LightSlateGrey", 	CVL_COLOR_LIGHTSLATEGREY }, 
    { "LightSteelBlue", 	CVL_COLOR_LIGHTSTEELBLUE }, 
    { "LightSteelBlue1", 	CVL_COLOR_LIGHTSTEELBLUE1 }, 
    { "LightSteelBlue2", 	CVL_COLOR_LIGHTSTEELBLUE2 }, 
    { "LightSteelBlue3", 	CVL_COLOR_LIGHTSTEELBLUE3 }, 
    { "LightSteelBlue4", 	CVL_COLOR_LIGHTSTEELBLUE4 }, 
    { "LightYellow", 		CVL_COLOR_LIGHTYELLOW }, 
    { "LightYellow1", 		CVL_COLOR_LIGHTYELLOW1 }, 
    { "LightYellow2", 		CVL_COLOR_LIGHTYELLOW2 }, 
    { "LightYellow3", 		CVL_COLOR_LIGHTYELLOW3 }, 
    { "LightYellow4", 		CVL_COLOR_LIGHTYELLOW4 }, 
    { "lime green", 		CVL_COLOR_LIMEGREEN }, 
    { "LimeGreen", 		CVL_COLOR_LIMEGREEN }, 
    { "linen", 			CVL_COLOR_LINEN }, 
    { "magenta", 		CVL_COLOR_MAGENTA }, 
    { "magenta1", 		CVL_COLOR_MAGENTA1 }, 
    { "magenta2", 		CVL_COLOR_MAGENTA2 }, 
    { "magenta3", 		CVL_COLOR_MAGENTA3 }, 
    { "magenta4", 		CVL_COLOR_MAGENTA4 }, 
    { "maroon", 		CVL_COLOR_MAROON }, 
    { "maroon1", 		CVL_COLOR_MAROON1 }, 
    { "maroon2", 		CVL_COLOR_MAROON2 }, 
    { "maroon3", 		CVL_COLOR_MAROON3 }, 
    { "maroon4", 		CVL_COLOR_MAROON4 }, 
    { "medium aquamarine", 	CVL_COLOR_MEDIUMAQUAMARINE }, 
    { "medium blue", 		CVL_COLOR_MEDIUMBLUE }, 
    { "medium orchid", 		CVL_COLOR_MEDIUMORCHID }, 
    { "medium purple", 		CVL_COLOR_MEDIUMPURPLE }, 
    { "medium sea green", 	CVL_COLOR_MEDIUMSEAGREEN }, 
    { "medium slate blue", 	CVL_COLOR_MEDIUMSLATEBLUE }, 
    { "medium spring green", 	CVL_COLOR_MEDIUMSPRINGGREEN }, 
    { "medium turquoise", 	CVL_COLOR_MEDIUMTURQUOISE }, 
    { "medium violet red", 	CVL_COLOR_MEDIUMVIOLETRED }, 
    { "MediumAquamarine", 	CVL_COLOR_MEDIUMAQUAMARINE }, 
    { "MediumBlue", 		CVL_COLOR_MEDIUMBLUE }, 
    { "MediumOrchid", 		CVL_COLOR_MEDIUMORCHID }, 
    { "MediumOrchid1", 		CVL_COLOR_MEDIUMORCHID1 }, 
    { "MediumOrchid2", 		CVL_COLOR_MEDIUMORCHID2 }, 
    { "MediumOrchid3", 		CVL_COLOR_MEDIUMORCHID3 }, 
    { "MediumOrchid4", 		CVL_COLOR_MEDIUMORCHID4 }, 
    { "MediumPurple", 		CVL_COLOR_MEDIUMPURPLE }, 
    { "MediumPurple1", 		CVL_COLOR_MEDIUMPURPLE1 }, 
    { "MediumPurple2", 		CVL_COLOR_MEDIUMPURPLE2 }, 
    { "MediumPurple3", 		CVL_COLOR_MEDIUMPURPLE3 }, 
    { "MediumPurple4", 		CVL_COLOR_MEDIUMPURPLE4 }, 
    { "MediumSeaGreen", 	CVL_COLOR_MEDIUMSEAGREEN }, 
    { "MediumSlateBlue", 	CVL_COLOR_MEDIUMSLATEBLUE }, 
    { "MediumSpringGreen", 	CVL_COLOR_MEDIUMSPRINGGREEN }, 
    { "MediumTurquoise", 	CVL_COLOR_MEDIUMTURQUOISE }, 
    { "MediumVioletRed", 	CVL_COLOR_MEDIUMVIOLETRED }, 
    { "midnight blue", 		CVL_COLOR_MIDNIGHTBLUE }, 
    { "MidnightBlue", 		CVL_COLOR_MIDNIGHTBLUE }, 
    { "mint cream", 		CVL_COLOR_MINTCREAM }, 
    { "MintCream", 		CVL_COLOR_MINTCREAM }, 
    { "misty rose", 		CVL_COLOR_MISTYROSE }, 
    { "MistyRose", 		CVL_COLOR_MISTYROSE }, 
    { "MistyRose1", 		CVL_COLOR_MISTYROSE1 }, 
    { "MistyRose2", 		CVL_COLOR_MISTYROSE2 }, 
    { "MistyRose3", 		CVL_COLOR_MISTYROSE3 }, 
    { "MistyRose4", 		CVL_COLOR_MISTYROSE4 }, 
    { "moccasin", 		CVL_COLOR_MOCCASIN }, 
    { "navajo white", 		CVL_COLOR_NAVAJOWHITE }, 
    { "NavajoWhite", 		CVL_COLOR_NAVAJOWHITE }, 
    { "NavajoWhite1", 		CVL_COLOR_NAVAJOWHITE1 }, 
    { "NavajoWhite2", 		CVL_COLOR_NAVAJOWHITE2 }, 
    { "NavajoWhite3", 		CVL_COLOR_NAVAJOWHITE3 }, 
    { "NavajoWhite4", 		CVL_COLOR_NAVAJOWHITE4 }, 
    { "navy", 			CVL_COLOR_NAVY }, 
    { "navy blue", 		CVL_COLOR_NAVYBLUE }, 
    { "NavyBlue", 		CVL_COLOR_NAVYBLUE }, 
    { "old lace", 		CVL_COLOR_OLDLACE }, 
    { "OldLace", 		CVL_COLOR_OLDLACE }, 
    { "olive drab", 		CVL_COLOR_OLIVEDRAB }, 
    { "OliveDrab", 		CVL_COLOR_OLIVEDRAB }, 
    { "OliveDrab1", 		CVL_COLOR_OLIVEDRAB1 }, 
    { "OliveDrab2", 		CVL_COLOR_OLIVEDRAB2 }, 
    { "OliveDrab3", 		CVL_COLOR_OLIVEDRAB3 }, 
    { "OliveDrab4", 		CVL_COLOR_OLIVEDRAB4 }, 
    { "orange", 		CVL_COLOR_ORANGE }, 
    { "orange red", 		CVL_COLOR_ORANGERED }, 
    { "orange1", 		CVL_COLOR_ORANGE1 }, 
    { "orange2", 		CVL_COLOR_ORANGE2 }, 
    { "orange3", 		CVL_COLOR_ORANGE3 }, 
    { "orange4", 		CVL_COLOR_ORANGE4 }, 
    { "OrangeRed", 		CVL_COLOR_ORANGERED }, 
    { "OrangeRed1", 		CVL_COLOR_ORANGERED1 }, 
    { "OrangeRed2", 		CVL_COLOR_ORANGERED2 }, 
    { "OrangeRed3", 		CVL_COLOR_ORANGERED3 }, 
    { "OrangeRed4", 		CVL_COLOR_ORANGERED4 }, 
    { "orchid", 		CVL_COLOR_ORCHID }, 
    { "orchid1", 		CVL_COLOR_ORCHID1 }, 
    { "orchid2", 		CVL_COLOR_ORCHID2 }, 
    { "orchid3", 		CVL_COLOR_ORCHID3 }, 
    { "orchid4", 		CVL_COLOR_ORCHID4 }, 
    { "pale goldenrod", 	CVL_COLOR_PALEGOLDENROD }, 
    { "pale green", 		CVL_COLOR_PALEGREEN }, 
    { "pale turquoise", 	CVL_COLOR_PALETURQUOISE }, 
    { "pale violet red", 	CVL_COLOR_PALEVIOLETRED }, 
    { "PaleGoldenrod", 		CVL_COLOR_PALEGOLDENROD }, 
    { "PaleGreen", 		CVL_COLOR_PALEGREEN }, 
    { "PaleGreen1", 		CVL_COLOR_PALEGREEN1 }, 
    { "PaleGreen2", 		CVL_COLOR_PALEGREEN2 }, 
    { "PaleGreen3", 		CVL_COLOR_PALEGREEN3 }, 
    { "PaleGreen4", 		CVL_COLOR_PALEGREEN4 }, 
    { "PaleTurquoise", 		CVL_COLOR_PALETURQUOISE }, 
    { "PaleTurquoise1", 	CVL_COLOR_PALETURQUOISE1 }, 
    { "PaleTurquoise2", 	CVL_COLOR_PALETURQUOISE2 }, 
    { "PaleTurquoise3", 	CVL_COLOR_PALETURQUOISE3 }, 
    { "PaleTurquoise4", 	CVL_COLOR_PALETURQUOISE4 }, 
    { "PaleVioletRed", 		CVL_COLOR_PALEVIOLETRED }, 
    { "PaleVioletRed1", 	CVL_COLOR_PALEVIOLETRED1 }, 
    { "PaleVioletRed2", 	CVL_COLOR_PALEVIOLETRED2 }, 
    { "PaleVioletRed3", 	CVL_COLOR_PALEVIOLETRED3 }, 
    { "PaleVioletRed4", 	CVL_COLOR_PALEVIOLETRED4 }, 
    { "papaya whip", 		CVL_COLOR_PAPAYAWHIP }, 
    { "PapayaWhip", 		CVL_COLOR_PAPAYAWHIP }, 
    { "peach puff", 		CVL_COLOR_PEACHPUFF }, 
    { "PeachPuff", 		CVL_COLOR_PEACHPUFF }, 
    { "PeachPuff1", 		CVL_COLOR_PEACHPUFF1 }, 
    { "PeachPuff2", 		CVL_COLOR_PEACHPUFF2 }, 
    { "PeachPuff3", 		CVL_COLOR_PEACHPUFF3 }, 
    { "PeachPuff4", 		CVL_COLOR_PEACHPUFF4 }, 
    { "peru", 			CVL_COLOR_PERU }, 
    { "pink", 			CVL_COLOR_PINK }, 
    { "pink1", 			CVL_COLOR_PINK1 }, 
    { "pink2", 			CVL_COLOR_PINK2 }, 
    { "pink3", 			CVL_COLOR_PINK3 }, 
    { "pink4", 			CVL_COLOR_PINK4 }, 
    { "plum", 			CVL_COLOR_PLUM }, 
    { "plum1", 			CVL_COLOR_PLUM1 }, 
    { "plum2", 			CVL_COLOR_PLUM2 }, 
    { "plum3", 			CVL_COLOR_PLUM3 }, 
    { "plum4", 			CVL_COLOR_PLUM4 }, 
    { "powder blue", 		CVL_COLOR_POWDERBLUE }, 
    { "PowderBlue", 		CVL_COLOR_POWDERBLUE }, 
    { "purple", 		CVL_COLOR_PURPLE }, 
    { "purple1", 		CVL_COLOR_PURPLE1 }, 
    { "purple2", 		CVL_COLOR_PURPLE2 }, 
    { "purple3", 		CVL_COLOR_PURPLE3 }, 
    { "purple4", 		CVL_COLOR_PURPLE4 }, 
    { "red", 			CVL_COLOR_RED }, 
    { "red1", 			CVL_COLOR_RED1 }, 
    { "red2", 			CVL_COLOR_RED2 }, 
    { "red3", 			CVL_COLOR_RED3 }, 
    { "red4", 			CVL_COLOR_RED4 }, 
    { "rosy brown", 		CVL_COLOR_ROSYBROWN }, 
    { "RosyBrown", 		CVL_COLOR_ROSYBROWN }, 
    { "RosyBrown1", 		CVL_COLOR_ROSYBROWN1 }, 
    { "RosyBrown2", 		CVL_COLOR_ROSYBROWN2 }, 
    { "RosyBrown3", 		CVL_COLOR_ROSYBROWN3 }, 
    { "RosyBrown4", 		CVL_COLOR_ROSYBROWN4 }, 
    { "royal blue", 		CVL_COLOR_ROYALBLUE }, 
    { "RoyalBlue", 		CVL_COLOR_ROYALBLUE }, 
    { "RoyalBlue1", 		CVL_COLOR_ROYALBLUE1 }, 
    { "RoyalBlue2", 		CVL_COLOR_ROYALBLUE2 }, 
    { "RoyalBlue3", 		CVL_COLOR_ROYALBLUE3 }, 
    { "RoyalBlue4", 		CVL_COLOR_ROYALBLUE4 }, 
    { "saddle brown", 		CVL_COLOR_SADDLEBROWN }, 
    { "SaddleBrown", 		CVL_COLOR_SADDLEBROWN }, 
    { "salmon", 		CVL_COLOR_SALMON }, 
    { "salmon1", 		CVL_COLOR_SALMON1 }, 
    { "salmon2", 		CVL_COLOR_SALMON2 }, 
    { "salmon3", 		CVL_COLOR_SALMON3 }, 
    { "salmon4", 		CVL_COLOR_SALMON4 }, 
    { "sandy brown", 		CVL_COLOR_SANDYBROWN }, 
    { "SandyBrown", 		CVL_COLOR_SANDYBROWN }, 
    { "sea green", 		CVL_COLOR_SEAGREEN }, 
    { "SeaGreen", 		CVL_COLOR_SEAGREEN }, 
    { "SeaGreen1", 		CVL_COLOR_SEAGREEN1 }, 
    { "SeaGreen2", 		CVL_COLOR_SEAGREEN2 }, 
    { "SeaGreen3", 		CVL_COLOR_SEAGREEN3 }, 
    { "SeaGreen4", 		CVL_COLOR_SEAGREEN4 }, 
    { "seashell", 		CVL_COLOR_SEASHELL }, 
    { "seashell1", 		CVL_COLOR_SEASHELL1 }, 
    { "seashell2", 		CVL_COLOR_SEASHELL2 }, 
    { "seashell3", 		CVL_COLOR_SEASHELL3 }, 
    { "seashell4", 		CVL_COLOR_SEASHELL4 }, 
    { "sienna", 		CVL_COLOR_SIENNA }, 
    { "sienna1", 		CVL_COLOR_SIENNA1 }, 
    { "sienna2", 		CVL_COLOR_SIENNA2 }, 
    { "sienna3", 		CVL_COLOR_SIENNA3 }, 
    { "sienna4", 		CVL_COLOR_SIENNA4 }, 
    { "sky blue", 		CVL_COLOR_SKYBLUE }, 
    { "SkyBlue", 		CVL_COLOR_SKYBLUE }, 
    { "SkyBlue1", 		CVL_COLOR_SKYBLUE1 }, 
    { "SkyBlue2", 		CVL_COLOR_SKYBLUE2 }, 
    { "SkyBlue3", 		CVL_COLOR_SKYBLUE3 }, 
    { "SkyBlue4", 		CVL_COLOR_SKYBLUE4 }, 
    { "slate blue", 		CVL_COLOR_SLATEBLUE }, 
    { "slate gray", 		CVL_COLOR_SLATEGRAY }, 
    { "slate grey", 		CVL_COLOR_SLATEGREY }, 
    { "SlateBlue", 		CVL_COLOR_SLATEBLUE }, 
    { "SlateBlue1", 		CVL_COLOR_SLATEBLUE1 }, 
    { "SlateBlue2", 		CVL_COLOR_SLATEBLUE2 }, 
    { "SlateBlue3", 		CVL_COLOR_SLATEBLUE3 }, 
    { "SlateBlue4", 		CVL_COLOR_SLATEBLUE4 }, 
    { "SlateGray", 		CVL_COLOR_SLATEGRAY }, 
    { "SlateGray1", 		CVL_COLOR_SLATEGRAY1 }, 
    { "SlateGray2", 		CVL_COLOR_SLATEGRAY2 }, 
    { "SlateGray3", 		CVL_COLOR_SLATEGRAY3 }, 
    { "SlateGray4", 		CVL_COLOR_SLATEGRAY4 }, 
    { "SlateGrey", 		CVL_COLOR_SLATEGREY }, 
    { "snow", 			CVL_COLOR_SNOW }, 
    { "snow1", 			CVL_COLOR_SNOW1 }, 
    { "snow2", 			CVL_COLOR_SNOW2 }, 
    { "snow3", 			CVL_COLOR_SNOW3 }, 
    { "snow4", 			CVL_COLOR_SNOW4 }, 
    { "spring green", 		CVL_COLOR_SPRINGGREEN }, 
    { "SpringGreen", 		CVL_COLOR_SPRINGGREEN }, 
    { "SpringGreen1", 		CVL_COLOR_SPRINGGREEN1 }, 
    { "SpringGreen2", 		CVL_COLOR_SPRINGGREEN2 }, 
    { "SpringGreen3", 		CVL_COLOR_SPRINGGREEN3 }, 
    { "SpringGreen4", 		CVL_COLOR_SPRINGGREEN4 }, 
    { "steel blue", 		CVL_COLOR_STEELBLUE }, 
    { "SteelBlue", 		CVL_COLOR_STEELBLUE }, 
    { "SteelBlue1", 		CVL_COLOR_STEELBLUE1 }, 
    { "SteelBlue2", 		CVL_COLOR_STEELBLUE2 }, 
    { "SteelBlue3", 		CVL_COLOR_STEELBLUE3 }, 
    { "SteelBlue4", 		CVL_COLOR_STEELBLUE4 }, 
    { "tan", 			CVL_COLOR_TAN }, 
    { "tan1", 			CVL_COLOR_TAN1 }, 
    { "tan2", 			CVL_COLOR_TAN2 }, 
    { "tan3", 			CVL_COLOR_TAN3 }, 
    { "tan4", 			CVL_COLOR_TAN4 }, 
    { "thistle", 		CVL_COLOR_THISTLE }, 
    { "thistle1", 		CVL_COLOR_THISTLE1 }, 
    { "thistle2", 		CVL_COLOR_THISTLE2 }, 
    { "thistle3", 		CVL_COLOR_THISTLE3 }, 
    { "thistle4", 		CVL_COLOR_THISTLE4 }, 
    { "tomato", 		CVL_COLOR_TOMATO }, 
    { "tomato1", 		CVL_COLOR_TOMATO1 }, 
    { "tomato2", 		CVL_COLOR_TOMATO2 }, 
    { "tomato3", 		CVL_COLOR_TOMATO3 }, 
    { "tomato4", 		CVL_COLOR_TOMATO4 }, 
    { "turquoise", 		CVL_COLOR_TURQUOISE }, 
    { "turquoise1", 		CVL_COLOR_TURQUOISE1 }, 
    { "turquoise2", 		CVL_COLOR_TURQUOISE2 }, 
    { "turquoise3", 		CVL_COLOR_TURQUOISE3 }, 
    { "turquoise4", 		CVL_COLOR_TURQUOISE4 }, 
    { "violet", 		CVL_COLOR_VIOLET }, 
    { "violet red", 		CVL_COLOR_VIOLETRED }, 
    { "VioletRed", 		CVL_COLOR_VIOLETRED }, 
    { "VioletRed1", 		CVL_COLOR_VIOLETRED1 }, 
    { "VioletRed2", 		CVL_COLOR_VIOLETRED2 }, 
    { "VioletRed3", 		CVL_COLOR_VIOLETRED3 }, 
    { "VioletRed4", 		CVL_COLOR_VIOLETRED4 }, 
    { "wheat", 			CVL_COLOR_WHEAT }, 
    { "wheat1", 		CVL_COLOR_WHEAT1 }, 
    { "wheat2", 		CVL_COLOR_WHEAT2 }, 
    { "wheat3", 		CVL_COLOR_WHEAT3 }, 
    { "wheat4", 		CVL_COLOR_WHEAT4 }, 
    { "white", 			CVL_COLOR_WHITE }, 
    { "white smoke", 		CVL_COLOR_WHITESMOKE }, 
    { "WhiteSmoke", 		CVL_COLOR_WHITESMOKE }, 
    { "yellow", 		CVL_COLOR_YELLOW }, 
    { "yellow green", 		CVL_COLOR_YELLOWGREEN }, 
    { "yellow1", 		CVL_COLOR_YELLOW1 }, 
    { "yellow2", 		CVL_COLOR_YELLOW2 }, 
    { "yellow3", 		CVL_COLOR_YELLOW3 }, 
    { "yellow4", 		CVL_COLOR_YELLOW4 },
    { "YellowGreen", 		CVL_COLOR_YELLOWGREEN }
};

static const size_t cvl_color_table_len = sizeof(cvl_color_table) / sizeof(cvl_color_table[0]);


/**
 * \param color		The color.
 * \param pixel_type	The pixel type.
 * \return		The pixel value.
 *
 * Converts a color to a pixel value that represents the color in the given
 * pixel type.
 */
inline cvl_pixel_t cvl_color_to_pixel(cvl_color_t color, cvl_pixel_type_t pixel_type)
{
    return (pixel_type == CVL_PIXEL_RGB) ? color : 
	((pixel_type == CVL_PIXEL_GRAY) ? cvl_pixel_rgb_to_gray(color) : cvl_pixel_rgb_to_yuv(color));
}

/* A helper for cvl_color_from_string() */
static int cvl_color_from_string_cmp(const cvl_color_table_entry_t *e1, const cvl_color_table_entry_t *e2)
{
    return strcasecmp(e1->name, e2->name);
}

/**
 * \param s		The string.
 * \param color		The returned color.
 * \return		Success (\a s is valid) or error (\a s is invalid).
 *
 * Converts a color string into a color value.
 * Valid strings are X11 color names, RGB values in the form 0xrrggbb,
 * and decimal values prepended with r, g, or b, for example r127g64b12.
 */
bool cvl_color_from_string(const char *s, cvl_color_t *color)
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
	    *color = (cvl_color_t)l;
	    return true;
	}
	// if it has the form 0x......, it cannot be one of the other to 
	// possibilities
	return false;
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
		    {
			r = l;
		    }
		    else if (state == RGB_VALUE_G)
		    {
			g = l;
		    }
		    else // state == RGB_VALUE_B
		    {
			b = l;
		    }
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
	    *color = cvl_pixel_rgb(r, g, b);
	    return true;
	}
	// no need to check for a X11 color, because for none of those is 
	// (strspn(s, "rgb0123456789") == len) true.
	return !error;
    }
    // X11 color name
    if ((entry = bsearch(&key, cvl_color_table, cvl_color_table_len, sizeof(cvl_color_table_entry_t), 
	    (int (*)(const void *, const void *))cvl_color_from_string_cmp)))
    {
	*color = entry->color;
	return true;
    }

    return false;
}


/**
 * \param rgb	The pixel value in RGB.
 * \param H	The Hue component of the pixel value in HSL.
 * \param S	The Saturation component of the pixel value in HSL.
 * \param L	The Lightness component of the pixel value in HSL.
 *
 * Converts an RGB pixel into HSL values. All HSL values are from [0,1].
 */
inline void cvl_rgb_to_hsl(cvl_pixel_t rgb, float *H, float *S, float *L)
{
    int ri = cvl_pixel_rgb_to_r(rgb);
    int gi = cvl_pixel_rgb_to_g(rgb);
    int bi = cvl_pixel_rgb_to_b(rgb);
    int mini = cvl_min3i(ri, gi, bi);
    int maxi = cvl_max3i(ri, gi, bi);
    float r = (float)ri / 255.0f;
    float g = (float)gi / 255.0f;
    float b = (float)bi / 255.0f;
    float min = cvl_min3(r, g, b);
    float max = cvl_max3(r, g, b);
    float delta = max - min;
    
    *L = (max + min) / 2.0f;
    if (maxi == mini)
    {
	*H = 0.0f;
	*S = 0.0f;
    }
    else
    {
	*S = delta / ((*L < 0.5f) ? (max + min) : (2.0f - max - min));
	float delta_r = (((max - r) / 6.0f) + (delta / 2.0f)) / delta;
	float delta_g = (((max - g) / 6.0f) + (delta / 2.0f)) / delta;
	float delta_b = (((max - b) / 6.0f) + (delta / 2.0f)) / delta;
	if (ri == maxi)
	{
	    *H = delta_b - delta_g;
	}
	else if (gi == maxi)
	{
	    *H = (1.0f / 3.0f) + delta_r - delta_b;
	}
	else
	{
	    *H = (2.0f / 3.0f) + delta_g - delta_r;
	}
	if (*H < 0.0f)
	{
	    *H += 1.0f;
	}
	else if (*H > 1.0f)
	{
	    *H -= 1.0f;
	}
    }
}

/* Helper for cvl_hsl_to_rgb() */
static inline int cvl_hsl_to_rgb_helper(float tmp2, float tmp1, float H)
{
    float ret;
    
    if (H < 0.0f)
    {
	H += 1.0f;
    }
    else if (H > 1.0f)
    {
	H -= 1.0f;
    }
    if ((6.0f * H) < 1.0f)
    {
	ret = (tmp2 + (tmp1 - tmp2) * 6.0f * H);
    }
    else if ((2.0f * H) < 1.0f)
    {
	ret = tmp1;
    }
    else if ((3.0f * H) < 2.0f)
    {
	ret = (tmp2 + (tmp1 - tmp2) * ((2.0f / 3.0f) - H) * 6.0f);
    }
    else
    {   
	ret = tmp2;
    }
    return cvl_iroundf(255.0f * ret);
}
    
/**
 * \param H	The Hue component of the pixel value in HSL.
 * \param S	The Saturation component of the pixel value in HSL.
 * \param L	The Lightness component of the pixel value in HSL.
 * \return	The pixel value in RGB.
 *
 * Converts a HSL value to a RGB pixel value.
 */
inline cvl_pixel_t cvl_hsl_to_rgb(float H, float S, float L)
{
    int r, g, b;
    float tmp1, tmp2;
    
    if (fabsf(S) < 0.0001f)
    {
	r = L * 255.0f;
	g = L * 255.0f;
	b = L * 255.0f;
    }
    else
    {
	if (L < 0.5f) 
	{
	    tmp1 = L * (1.0f + S);
	}
	else
	{
	    tmp1 = (L + S) - (S * L);
	}
	tmp2 = 2.0f * L - tmp1;
	r = cvl_hsl_to_rgb_helper(tmp2, tmp1, H + (1.0f / 3.0f));
	g = cvl_hsl_to_rgb_helper(tmp2, tmp1, H);
	b = cvl_hsl_to_rgb_helper(tmp2, tmp1, H - (1.0f / 3.0f));
    }
    return cvl_pixel_rgb(r, g, b);
}

/* Helpers for sRGB -> CIE L*a*b* conversion */
static inline float cvl_srgb_to_cielab_g(float K)
{
    const float a = 0.055f;
    const float gamma = 2.4f;
    return (K > 0.04045f) ? powf((K + a) / (1.0f + a), gamma) : (K / 12.92f);    
}
static inline float cvl_srgb_to_cielab_f(float t)
{
    return (t > 0.008856f) ? cbrtf(t) : (7.787f * t + 16.0f / 116.0f);
}

/**
 * \param srgb	The pixel value in sRGB.
 * \param L	The L* component of the pixel value in CIE L*a*b*.
 * \param a	The a* component of the pixel value in CIE L*a*b*.
 * \param b	The b* component of the pixel value in CIE L*a*b*.
 *
 * Converts an RGB pixel into CIE L*a*b* values. The RGB values are interpreted
 * as sRGB values because an absolute color space is needed.
 */
inline void cvl_srgb_to_cielab(cvl_pixel_t srgb, float *L, float *a, float *b)
{
    // The CIE XYZ tristimulus values of D65, which is the reference white point of sRGB:
    const float Xn = 0.3127f;
    const float Yn = 0.3290f;
    const float Zn = 1.0f - Xn - Yn;

    float gR = cvl_srgb_to_cielab_g((float)cvl_pixel_rgb_to_r(srgb) / 255.0f);
    float gG = cvl_srgb_to_cielab_g((float)cvl_pixel_rgb_to_g(srgb) / 255.0f);
    float gB = cvl_srgb_to_cielab_g((float)cvl_pixel_rgb_to_b(srgb) / 255.0f);
    float fXXn = cvl_srgb_to_cielab_f((0.412424f * gR + 0.357579f * gG + 0.180464f * gB) / Xn);
    float fYYn = cvl_srgb_to_cielab_f((0.212656f * gR + 0.715158f * gG + 0.072186f * gB) / Yn);
    float fZZn = cvl_srgb_to_cielab_f((0.019332f * gR + 0.119193f * gG + 0.950444f * gB) / Zn);
    *L = 116.0f * fYYn - 16.0f;
    *a = 500.0f * (fXXn - fYYn);
    *b = 200.0f * (fYYn - fZZn);
}


/**
 * \param frame		The frame.
 *
 * Inverts a frame, regardless of pixel type: GRAY and RGB images are inverted in
 * the obvious way, YUV images are converted to RGB first, then inverted, then
 * converted back to YUV.
 */
void cvl_invert(cvl_frame_t *frame)
{
    bool yuv = false;
    cvl_pixel_t mask = (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY) ? 0xff : 0xffffff;
    
    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	cvl_frame_to_rgb(frame);
	yuv = true;
    }
    
    for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
    {
	cvl_frame_set_i(frame, i, (~cvl_frame_get_i(frame, i) & mask));
    }
    
    if (yuv)
    {
	cvl_frame_to_yuv(frame);
    }
}


/**
 * \param frame		The frame.
 * \param hue		Additive constant for the hue value from -PI to PI.
 * \param saturation	Modification of saturation, from -1 to 1.
 * \param lightness	Modification of lightness, from -1 to 1.
 * \param contrast	Modification of contrast, from -1 to 1.
 * 
 * Manipulates the colors in an image. The HSL color space is used to do the
 * manipulations; the image is converted back to its original pixel type
 * afterwards.
 */
void cvl_color_adjust(cvl_frame_t *frame, 
	float hue, float saturation, float lightness, float contrast)
{
    int size = cvl_frame_width(frame) * cvl_frame_height(frame);
    bool do_hue = (fabsf(hue) > 0.0001f);
    bool do_saturation = (fabsf(saturation) > 0.0001f);
    bool do_lightness = (fabsf(lightness) > 0.0001f);
    bool do_contrast = (fabsf(contrast) > 0.0001f);

    for (int i = 0; i < size; i++)
    {
	float H, S, L;
	
     	cvl_rgb_to_hsl(cvl_pixel_to_rgb(cvl_frame_get_i(frame, i), cvl_frame_pixel_type(frame)), 
		&H, &S, &L);

	if (do_hue)
	{
	    H += hue / (2.0f * (float)M_PI);
	    if (H > 1.0f)
	    {
		H -= 1.0f;
	    }
	    else if (H < 0.0f)
	    {
		H += 1.0f;
	    }
	}
	if (do_saturation)
	{
	    S += saturation * S;
	    if (S > 1.0f)
	    {
		S = 1.0f;
	    }
	    else if (S < 0.0f)
	    {
		S = 0.0f;
	    }
	}
	if (do_lightness)
	{
	    L += lightness * L;
	    if (L > 1.0f)
	    {
		L = 1.0f;
	    }
	    else if (L < 0.0f)
	    {
		L = 0.0f;
	    }
	}
	if (do_contrast)
	{
	    L = (L - 0.5f) * (contrast + 1.0f) + 0.5f;
	    if (L > 1.0f)
	    {
		L = 1.0f;
	    }
	    else if (L < 0.0f)
	    {
		L = 0.0f;
	    }
	}

	cvl_frame_set_i(frame, i, 
		cvl_pixel_convert(cvl_frame_pixel_type(frame),
		    cvl_hsl_to_rgb(H, S, L), CVL_PIXEL_RGB));	
    }
}

/**
 * \param frame		The frame.
 * \param gamma		The gamma value.
 * 
 * Applies gamma correction to the frame.\n
 * For \a CVL_PIXEL_TYPE_GRAY, gamma correction is applied to the gray channel.
 * For \a CVL_PIXEL_TYPE_RGB, gamma correction is applied to each of the R,G,B channels.
 * For \a CVL_PIXEL_TYPE_YUV, gamma correction is applied to the Y channel.
 */
void cvl_gamma_correct(cvl_frame_t *frame, float gamma)
{
    cvl_assert(frame != NULL);
    cvl_assert(gamma > 0.0f);

    float exponent = 1.0f / gamma;
    uint8_t table[256];
    
    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	for (int i = 0; i < 256; i++)
	{
	    if (i <= 16)
	    {
		table[i] = 16;
	    }
	    else if (i >= 235)
	    {
		table[i] = 235;
	    }
	    else
	    {
		table[i] = 16 + cvl_iroundf(219.0f * powf((float)(i - 16) / 219.0f, exponent));
	    }
	}
    }
    else
    {
	for (int i = 0; i < 256; i++)
	{
	    table[i] = cvl_iroundf(255.0f * powf((float)i / 255.0f, exponent));
	}
    }
    
    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
	{
	    cvl_frame_set_i(frame, i, table[cvl_pixel_gray(cvl_frame_get_i(frame, i))]);
	}
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
    {
	for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
	{
	    cvl_pixel_t old = cvl_frame_get_i(frame, i);
	    cvl_frame_set_i(frame, i, cvl_pixel_rgb(
			table[cvl_pixel_rgb_to_r(old)], 
			table[cvl_pixel_rgb_to_g(old)], 
			table[cvl_pixel_rgb_to_b(old)]));
	}
    }
    else // (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
	{
	    cvl_pixel_t old = cvl_frame_get_i(frame, i);
	    cvl_frame_set_i(frame, i, cvl_pixel_yuv(
			table[cvl_pixel_yuv_to_y(old)], 
			cvl_pixel_yuv_to_u(old), 
			cvl_pixel_yuv_to_v(old)));
	}
    }
}

/**
 * \param frame		The frame.
 * \param gamma_r	The gamma value for the R channel.
 * \param gamma_g	The gamma value for the G channel.
 * \param gamma_b	The gamma value for the B channel.
 * 
 * Applies gamma correction in the RGB color space, for each channel separately.
 * If the frame is not of type \a CVL_PIXEL_TYPE_RGB, it is temporarily
 * converted to RGB. After the correction, the original pixel type is restored.
 */
void cvl_gamma_correct_rgb(cvl_frame_t *frame, float gamma_r, float gamma_g, float gamma_b)
{
    cvl_assert(frame != NULL);
    cvl_assert(gamma_r > 0.0f);
    cvl_assert(gamma_g > 0.0f);
    cvl_assert(gamma_b > 0.0f);

    float exponent_r = 1.0f / gamma_r;
    float exponent_g = 1.0f / gamma_g;
    float exponent_b = 1.0f / gamma_b;
    uint8_t table_r[256];
    uint8_t table_g[256];
    uint8_t table_b[256];
    
    for (int i = 0; i < 256; i++)
    {
	table_r[i] = cvl_iroundf(255.0f * powf((float)i / 255.0, exponent_r));
	table_g[i] = cvl_iroundf(255.0f * powf((float)i / 255.0, exponent_g));
	table_b[i] = cvl_iroundf(255.0f * powf((float)i / 255.0, exponent_b));
    }
    
    cvl_pixel_type_t original_pixel_type = cvl_frame_pixel_type(frame);
    cvl_frame_to_rgb(frame);
    for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
    {
	cvl_pixel_t old = cvl_frame_get_i(frame, i);
	cvl_frame_set_i(frame, i, cvl_pixel_rgb(
    		    table_r[cvl_pixel_rgb_to_r(old)], 
		    table_g[cvl_pixel_rgb_to_g(old)], 
		    table_b[cvl_pixel_rgb_to_b(old)]));
    }
    cvl_frame_convert(frame, original_pixel_type);
}

/**
 * \param frame		The frame.
 * \param channel	Number of color channel.
 * \param count		Array of 256 ints to store the histogram in.
 *
 * Stores the absolute histogram of \a frame in \a count, which must be an array of
 * 256 ints. The \a channel is ignored for #CVL_PIXEL_GRAY frames (there is only
 * one channel). For #CVL_PIXEL_RGB frames it means 0 for red, 1 for green, and 2
 * for blue. For #CVL_PIXEL_YUV frames it means 0 for Y, 1 for U, and 2 for V.
 */
void cvl_abs_histogram(cvl_frame_t *frame, int channel, int *count)
{
    cvl_assert(frame != NULL);
    cvl_assert(count != NULL);

    memset(count, 0, 256 * sizeof(int));
    for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
    {
	cvl_pixel_t p = cvl_frame_get_i(frame, i);
	if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
	{
	    p = cvl_pixel_gray(p);
	}
	else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
	{
	    p = (channel == 0 ? cvl_pixel_rgb_to_r(p) : 
		    (channel == 1 ? cvl_pixel_rgb_to_g(p) : cvl_pixel_rgb_to_b(p)));
	}
	else
	{
	    p = (channel == 0 ? cvl_pixel_yuv_to_y(p) : 
		    (channel == 1 ? cvl_pixel_yuv_to_u(p) : cvl_pixel_yuv_to_v(p)));
	}
	count[p]++;
    }
}

/**
 * \param count		Array of 256 ints that contains an absolute histogram.
 * \param sum		Array of 256 ints to store the sums in.
 *
 * Sums the histogram values so that
 * sum[0] = count[0],
 * sum[1] = count[0] + count[1],
 * ...
 */
void cvl_histogram_sum(int *count, int *sum)
{
    sum[0] = count[0];
    for (int i = 1; i < 256; i++)
    {
	sum[i] = sum[i - 1] + count[i];
    }
}

/**
 * \param frame		The frame.
 * \param abs_hist	Array of 256 ints that contains the absolute histogram of the frame.
 * \param rel_hist	Array of 256 floats to store the relative histogram in.
 *
 * Computes the relative histogram of \a frame from its absolute histogram
 * \a abs_hist.
 */
void cvl_rel_histogram(cvl_frame_t *frame, int *abs_hist, float *rel_hist)
{
    for (int i = 0; i < 256; i++)
    {
	rel_hist[i] = (float)abs_hist[i] / (float)(cvl_frame_size(frame));
    }
}

/* A helper for cvl_equalize_histogram() */
static cvl_pixel_t cvl_equalize_histogram_clip_y(int y)
{
    if (y < 16)
    {
	return 16;
    }
    else if (y > 235)
    {
	return 235;
    }
    else
    {
	return y;
    }
}

/**
 * \param frame		The frame.
 *
 * For color frames (#CVL_PIXEL_RGB or #CVL_PIXEL_YUV), the histogram of the Y
 * part of the YUV color space qill be equalized. In case of #CVL_PIXEL_RGB, the
 * conversion to YUV and back will be done automatically.
 */
void cvl_equalize_histogram(cvl_frame_t *frame)
{
    bool rgb = false;
    int abs_hist[256];
    int sum[256];
    int rpl_tab[256];    

    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
    {
	rgb = true;
	cvl_frame_to_yuv(frame);
    }
    cvl_abs_histogram(frame, 0, abs_hist);
    cvl_histogram_sum(abs_hist, sum);
    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	for (int i = 0; i < 256; i++)
	{
    	    rpl_tab[i] = cvl_iroundf(255.0f * (float)sum[i] / (float)(cvl_frame_size(frame)));
	}
	for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
	{
	    cvl_frame_set_i(frame, i, rpl_tab[cvl_frame_get_i(frame, i)]);
	}
    }
    else
    {
	for (int i = 0; i < 220; i++)
	{
	    rpl_tab[i] = cvl_iround(219.0f * (float)sum[i + 16] / (float)(cvl_frame_size(frame)));
	}
	for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
	{
    	    cvl_pixel_t yuv = cvl_frame_get_i(frame, i);
	    cvl_frame_set_i(frame, i, cvl_pixel_yuv(
			rpl_tab[cvl_equalize_histogram_clip_y(cvl_pixel_yuv_to_y(yuv)) - 16] + 16, 
			cvl_pixel_yuv_to_u(yuv), 
			cvl_pixel_yuv_to_v(yuv)));
	}	
    }
    if (rgb)
    {
	cvl_frame_to_rgb(frame);
    }
}

/**
 * \param frame		The frame.
 * \param threshold	The threshold.
 *
 * The frame will be converted to #CVL_PIXEL_GRAY before binarization, if
 * necessary. Pixels will be set to 0x00 if their value is less than the
 * threshold, and to 0xff otherwise.
 */
void cvl_binarize_global_threshold(cvl_frame_t *frame, int threshold)
{
    cvl_frame_to_gray(frame);
    for (int i = 0; i < cvl_frame_width(frame) * cvl_frame_height(frame); i++)
    {
	cvl_pixel_t p = cvl_frame_get_i(frame, i);
	cvl_frame_set_i(frame, i, (int)p < threshold ? 0x00 : 0xff);
    }
}

/**
 * \param frame		The frame.
 * \return		The threshold.
 *
 * The frame will be converted to #CVL_PIXEL_GRAY if necessary.
 */
int cvl_binarize_get_threshold_iterative(cvl_frame_t *frame)
{
    int s = 128;
    int s_old = 0;
    int skh, sh;
    float m1, m2;
    int count[256];

    cvl_frame_to_gray(frame);
    cvl_abs_histogram(frame, 0, count);

    while (s_old != s)
    {
	
	skh = 0;
	sh = 0;
    	for (int k = 0; k <= s; k++)
	{
	    skh += k * count[k];
	    sh += count[k];
	}
	m1 = (sh > 0 ? (float)skh / (float)sh : 255.0f);
	skh = 0;
	sh = 0;
	for (int k = s + 1; k < 256; k++)
	{
	    skh += k * count[k];
	    sh += count[k];
	}
	m2 = (sh > 0 ? (float)skh / (float)sh : 255.0f);
	s_old = s;
	s = (int)(m1 + m2) / 2;
	if (s < 0)
	{
	    s = 0;
	}
	else if (s > 255)
	{
	    s = 255;
	}
    }
    return s;
}

/**
 * \param frame		The frame.
 * \return		The threshold.
 *
 * The frame will be converted to #CVL_PIXEL_GRAY if necessary.
 */
int cvl_binarize_get_threshold_otsu(cvl_frame_t *frame)
{
    int abs_hist[256];
    float rel_hist[256];
    int s_opt = 0;
    float sqb_max = 0.0f;
    
    cvl_frame_to_gray(frame);
    cvl_abs_histogram(frame, 0, abs_hist);
    cvl_rel_histogram(frame, abs_hist, rel_hist);
    for (int s = 1; s <= 254; s++)
    {
	float p1 = 0.0f;
	for (int i = 0; i <= s; i++)
	{
	    p1 += rel_hist[i];
	}
	float m1 = 0.0f;
	for (int i = 1; i <= s; i++)
	{
	    m1 += rel_hist[i] * (float)i;
	}
	float m = m1;
	m1 /= p1;
	for (int i = s + 1; i < 256; i++)
	{
	    m += rel_hist[i] * (float)i;
	}
	float sqb = (p1 / (1.0f - p1)) * (m - m1) * (m - m1);
	if (sqb > sqb_max)
	{
	    sqb_max = sqb;
	    s_opt = s;
	}
    }
    return s_opt;
}

/**
 * \param frame		The frame.
 * \param type		The threshold type.
 * \param k		The size of the neighborhood will be 2k+1 x 2k+1.
 * \param C		An additive constant.
 *
 * The frame will be converted to #CVL_PIXEL_GRAY before binarization, if
 * necessary.
 * The local method uses a local threshold (T - C) for a neighborhood of size 
 * (2k+1 x 2k+1), where T is the mean of all pixel values in the neighborhood 
 * (type #CVL_BINARIZE_LOCAL_THRESHOLD_MEAN), the median of the pixel values
 * (type #CVL_BINARIZE_LOCAL_THRESHOLD_MEDIAN), or the mean of the minimum and 
 * maximum of the pixel values (type #CVL_BINARIZE_LOCAL_THRESHOLD_MINMAX). C 
 * can be negative. The mean type is much faster than median and minmax.
 */
void cvl_binarize_local_threshold(cvl_frame_t *frame, cvl_binarize_local_threshold_type_t type, int k, int C)
{
    int T;
    cvl_frame_to_gray(frame);
    cvl_frame_t *tmpframe = cvl_frame_clone(frame);

    if (type == CVL_BINARIZE_LOCAL_THRESHOLD_MEAN)
    {
	for (int y = 0; y < cvl_frame_height(frame); y++)
	{
	    int sum = 0;
	    int sum_border = 0;
	    for (int x = 0; x < cvl_frame_width(frame); x++)
	    {
		if (x == 0)
		{
		    /* x == 0: Compute the sum of the whole neighborhood */
		    for (int r = - k / 2; r <= k / 2; r++)
		    {
			sum_border += cvl_frame_get_r(tmpframe, - (k / 2), y + r);
			for (int c = - k / 2; c <= k / 2; c++)
			{
			    sum += cvl_frame_get_r(tmpframe, c, y + r);
			}
		    }
		}
		else
		{
		    /* x >= 1: Compute only the necessary parts */
		    sum -= sum_border;
		    sum_border = 0;
		    for (int r = - k / 2; r <= k / 2; r++)
		    {
			sum += cvl_frame_get_r(tmpframe, x + (k / 2), y + r);
			sum_border += cvl_frame_get_r(tmpframe, x - (k / 2), y + r);
		    }
		}
		T = cvl_iroundf((float)sum / (float)((2 * k + 1) * (2 * k + 1)));
		cvl_pixel_t p = cvl_frame_get(tmpframe, x, y);
		cvl_frame_set(frame, x, y, (int)p < (T - C) ? 0x00 : 0xff);
	    }
	}
    }
    else
    {
	int matrix_len = (2 * k + 1) * (2 * k + 1);
	int *matrix = xmalloc(matrix_len * sizeof(int));
	for (int y = 0; y < cvl_frame_height(frame); y++)
	{
	    for (int x = 0; x < cvl_frame_width(frame); x++)
	    {
		int matrix_index = 0;
		for (int r = - k / 2; r <= k / 2; r++)
		{
		    for (int c = - k / 2; c <= k / 2; c++)
		    {
			matrix[matrix_index++] = cvl_frame_get_r(tmpframe, x + c, y + r);
		    }
		}
		cvl_sort_array_int(matrix, (size_t)matrix_len);
		if (type == CVL_BINARIZE_LOCAL_THRESHOLD_MEDIAN)
		{
		    T = matrix[matrix_len / 2];
		}
		else
		{
		    T = cvl_iroundf((float)(matrix[0] + matrix[matrix_len - 1]) / 2.0f);
		}
		cvl_pixel_t p = cvl_frame_get(tmpframe, x, y);
		cvl_frame_set(frame, x, y, (int)p < T - C ? 0x00 : 0xff);
	    }
	}
	free(matrix);
    }
    cvl_frame_free(tmpframe);
}


/**
 * \param frame		The frame.
 * \param tl		The low threshold parameter, from [0,255].
 * \param th		The high threshold parameter, from [0,255].
 *
 * The frame will be converted to #CVL_PIXEL_GRAY before binarization, if
 * necessary. Choose \a tl and \a th so that 2tl <= th <= 4tl.
 * The hysterese method is not adequate for general images; it is mainly used 
 * by the Canny edge detector.
 */
void cvl_binarize_hysterese(cvl_frame_t *frame, int tl, int th)
{
    bool stable;

    cvl_frame_to_gray(frame);
    cvl_frame_t *tmpframe = cvl_frame_new(CVL_PIXEL_GRAY, cvl_frame_width(frame), cvl_frame_height(frame));

    // step 1
    cvl_frame_zero(tmpframe);

    // step 2:
    for (int y = 1; y < cvl_frame_height(frame) - 1; y++)
    {
	for (int x = 1; x < cvl_frame_width(frame) - 1; x++)
	{
	    if ((int)cvl_frame_get(frame, x, y) >= th)
	    {
		cvl_frame_set(tmpframe, x, y, 0xff);
	    }
	}
    }
    // steps 3, 4:
    stable = false;
    while (!stable)
    {
	stable = true;
	for (int y = 1; y < cvl_frame_height(frame) - 1; y++)
	{
	    for (int x = 1; x < cvl_frame_width(frame) - 1; x++)
	    {
		if ((int)cvl_frame_get(tmpframe, x, y) >= tl && (int)cvl_frame_get(tmpframe, x, y) < th)
		{
		    // Are there neighbors with 0xff?
		    if (cvl_frame_get(tmpframe, x + 1, y) == 0xff
			|| cvl_frame_get(tmpframe, x + 1, y - 1) == 0xff
			|| cvl_frame_get(tmpframe, x, y - 1) == 0xff
			|| cvl_frame_get(tmpframe, x - 1, y - 1) == 0xff
			|| cvl_frame_get(tmpframe, x - 1, y) == 0xff 
			|| cvl_frame_get(tmpframe, x - 1, y + 1) == 0xff
			|| cvl_frame_get(tmpframe, x, y + 1) == 0xff 
			|| cvl_frame_get(tmpframe, x + 1, y + 1) == 0xff)
		    {
			stable = false;
			cvl_frame_set(tmpframe, x, y, 0xff);
		    }
		}
	    }
	}
    }

    cvl_frame_copy(frame, tmpframe);
    cvl_frame_free(tmpframe);
}
