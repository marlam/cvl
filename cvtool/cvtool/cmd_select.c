/*
 * cmd_select.c
 * 
 * This file is part of cvtool, a computer vision tool.
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

#include "config.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "xalloc.h"

#include "c99math.h"

#include <cvl/cvl.h>

#include "options.h"


void cmd_select_print_help(void)
{
    cvl_msg_fmt_req(
	    "select [-d|--drop] <range>...\n"
	    "\n"
	    "Selects frames from a stream. "
	    "By default, frames in the given ranges are kept and all others "
	    "dropped. With --drop, frames in the given ranges are dropped and all others kept. "
	    "A range must be of the following form: <l>-<h> (from <l> to <h>), -<h> (from "
	    "beginning to <h>), <l>- (from <l> to end), <l> (only <l>), or - (everything). "
	    "Each start and end point can be a frame number (counting from 0) or a time in "
	    "the format [hours:]minutes:seconds[.fraction]. In short: if it contains a colon, "
	    "it's a time. Time ranges can only be used for YUV4MPEG2 streams with known frame "
	    "rate. IMPORTANT: If you use frame number ranges, the high frame number is inclusive: "
	    "the frame with this number will be dropped/kept. If you use time ranges, the high "
	    "time is exclusive and marks the first frame that will not be dropped/kept.");
}


/* A range of frame numbers */

typedef struct
{
    long low;
    long high;
} framerange_t;

enum frameno_t { FRAMENO_LOW, FRAMENO_HIGH };


/*
 * my_strrnchr()
 * 
 * Returns a pointer to the last occurence of the character 'c' in the first 
 * 'l' characters of the string 's', or NULL.
 */

char *my_strrnchr(char *s, size_t l, char c)
{
    char *p = NULL;
    size_t i = 0;

    while (i < l)
    {
	if (s[i] == c)
	{
	    p = s + i;
	}
	i++;
    }
    return p;
}


/*
 * string2frameno()
 *
 * Converts the string 'str' to a frame number and returns this number.
 * If 'str' contains no colon ':', it must be a decimal integer greater than 
 * or equal to zero.
 * If it contains a colon, it must be a date in the format [H:]M:S[.fract]
 * (hours, minutes, seconds, and fraction of a second), which will be used to
 * compute a frame number with the given framerate (both framerate1 and
 * framerate2 must not be zero in this case, or else -1 is returned).
 * If it is neither of these, -2 is returned.
 * If 'frameno_type' is FRAMENO_HIGH and the relevant part of 'str' is a time
 * instead of an integer, the resulting frame number will be decreased by one
 * (if it was bigger than zero). 
 */

long string2frameno(char *str, int framerate1, int framerate2, int frameno_type)
{
    size_t l;
    char *p;
    long n;
    char *hp, *mp, *sp;
    int hours;
    int minutes;
    int seconds;
    double fraction;

    l = strlen(str);
    if (!(sp = my_strrnchr(str, l, ':')))
    {
	/* a frame number */
	p = str;
	while (p < str + l)
	{
	    if (!isdigit((unsigned char)*p))
	    {
		return -2;
	    }
	    p++;
	}
	n = atol(str);
    }
    else
    {
	if (framerate1 == 0 || framerate2 == 0)
	{
	    /* unknown framerate => we cannot compute frame numbers */
	    return -1;
	}
	/* a time string */
	hp = NULL;
	mp = NULL;
	sp = str;
	p = str;
	while (p < str + l)
	{
	    if (*p == ':')
	    {
		if (hp)
		{
		    /* too many colons */
		    return -2;
		}
		hp = mp;
		mp = sp;
		sp = p + 1;
	    }
	    p++;
	}	
	if ((hp && strspn(hp, "0123456789") != (size_t)(mp - hp - 1))
		|| strspn(mp, "0123456789") != (size_t)(sp - mp - 1)
		|| strspn(sp, "0123456789.") != (size_t)(l - (sp - str)))
	{
	    return -2;
	}
	hours = hp ? atoi(hp) : 0;
	minutes = atoi(mp);
	seconds = atoi(sp);
	if ((p = my_strrnchr(sp, l - (sp - str), '.')))
	{
	    if (strchr(p + 1, '.'))
	    {
		/* more than one dot */
		return -2;
	    }
	    fraction = strtod(p, NULL);
	}
	else
	{
	    fraction = 0.0;
	}

	n = lround(((double)(hours * 3600 + minutes * 60 + seconds) + fraction) 
		* (double)framerate1 / (double)framerate2);
	if (n > 0 && frameno_type == FRAMENO_HIGH)
	{
	    n--;
	}
    }
    return n;
}


/*
 * string2framerange()
 *
 * Converts the following strings to frameranges (where <l> and <h> are 
 * strings that can be transformed to frame numbers, see string2frameno()):
 * <l>-<h>	from l to h
 * -<h>		from 0 to h
 * <l>-		from l to end
 * <l>		just l
 * -		everything
 *
 * 'framerate' is needed to convert time values to frame numbers.
 */

framerange_t string2framerange(char *s, int framerate1, int framerate2)
{
    char *p;
    framerange_t r;
    size_t l = strlen(s);
    
    if (s[0] == '-')
    {
	r.low = 0;
	if (l == 1)
	{
	    /* - */
	    r.high = LONG_MAX;
	}
	else
	{
	    /* -<h> */
	    r.high = string2frameno(s + 1, framerate1, framerate2, FRAMENO_HIGH);
	}
    }
    else
    {
	if (!(p = my_strrnchr(s, l, '-')))
	{
	    /* <l> */
	    r.low = r.high = string2frameno(s, framerate1, framerate2, FRAMENO_LOW);
	}
	else
	{
	    *p = '\0';
	    r.low = string2frameno(s, framerate1, framerate2, FRAMENO_LOW);
	    *p = '-';
	    if ((size_t)(p - s + 1) == l)
	    {
		/* <l>- */
		r.high = LONG_MAX;
	    }
	    else
	    {
		/* <l>-<h> */
		r.high = string2frameno(p + 1, framerate1, framerate2, FRAMENO_HIGH);
	    }
	}
    }
    return r;
}


/*
 * strings2framerangelist()
 *
 * Converts the strings in 's', which is of length 'len', to an array of frame ranges.
 *
 * The numbers in the array may be -1, which means the frame range was invalid.
 */

void strings2framerangelist(char *s[], int len, int framerate1, int framerate2, framerange_t **ranges)
{

    *ranges = xmalloc(len * sizeof(framerange_t));
    for (int i = 0; i < len; i++)
    {
	(*ranges)[i] = string2framerange(s[i], framerate1, framerate2);
    }
}
	

/*
 * cmp_frameranges()
 *
 * Compare to framerange_t variables. Needed for qsort().
 */

int cmp_frameranges(framerange_t *r1, framerange_t *r2)
{
    return (r1->low < r2->low ? -1 : (r1->low > r2->low) ? +1 : 0);
}


/*
 * normalize_frameranges()
 *
 * Merges overlapping frame ranges in the array of frame ranges and sort the
 * frame ranges in ascending order of their start values.
 * Returns the resulting number of frame ranges.
 */

int normalize_frameranges(framerange_t *ranges, int ranges_len)
{
    qsort(ranges, (size_t)ranges_len, sizeof(framerange_t), 
	    (int (*)(const void *, const void *))cmp_frameranges);
    
    for (int i = 0; i < ranges_len - 1; )
    {
	bool merged = 0;
	if (ranges[i + 1].low - 1 <= ranges[i].high)
	{
	    if (ranges[i + 1].high > ranges[i].high)
	    {
		ranges[i].high = ranges[i + 1].high;
	    }
	    for (int j = i + 1; j < ranges_len - 1; j++) 
	    {
		ranges[j] = ranges[j + 1];
	    }
	    ranges_len--;
	    merged = true;
	}
	if (!merged)
	{
	    i++;
	}
    }

    return ranges_len;
}


/*
 * frameno_in_range()
 *
 * Checks whether the frame number 'frameno' is in one of the frame ranges in
 * 'ranges'. The frame ranges in 'ranges' are expected to be sorted in ascending
 * order of their start values, and the values of 'frameno' are expected to 
 * increase with subsequent calls to this function. The value of 'ranges_index' 
 * must be initialized with zero. It points to the first frame range in 'ranges' 
 * that can contain 'frameno'. Since 'ranges' is sorted and 'frameno' is 
 * ascending, this index can be increased by this function with growing frame 
 * numbers.
 */

bool frameno_in_range(framerange_t *ranges, int ranges_len, int *ranges_index, long frameno)
{
    /* test all frame ranges beginning at ranges_index */
    for (int i = *ranges_index; i < ranges_len; i++)
    {
	if (frameno > ranges[i].high)
	{
	    /* this frame range cannot be reached anymore */
	    *ranges_index = i + 1;
	    continue;
	}
	else if (frameno >= ranges[i].low)
	{
	    /* frameno is in the frame range i */
	    return true;
	}
	else
	{
	    /* frameno < ranges[i].low. Since the following frame ranges have start
	     * values >= ranges[i].low, we don't need to test them */
	    break;
	}
    }
    return false;
}



int cmd_select(int argc, char *argv[])
{
    option_bool_t drop = { false, true };
    option_t options[] =
    {
	{ "drop", 'd', OPTION_BOOL, &drop, false },
	null_option
    };
    int first_argument;		/* index in argv where the first range string is stored */
    bool ranges_parsed;
    framerange_t *ranges;	/* array of frame ranges */
    int ranges_len;		/* length of this array */
    int ranges_index;		/* index to this array */
    long frameno;		/* the current frame number */
    long dropcounter;		/* number of dropped frames so far */
    cvl_io_info_t *input_info;    
    int framerate1, framerate2;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    bool keep_frame;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvtool_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }
    ranges_len = argc - first_argument;
    cvl_msg_dbg("%d ranges on the command line", ranges_len);
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    ranges = NULL;
    ranges_parsed = NULL;
    error = false;
    frameno = 0;
    dropcounter = 0;
    ranges_index = 0;
    while (!cvl_io_eof(stdin, input_info))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	
	if (!ranges_parsed)
	{
	    cvl_io_info_framerate(input_info, &framerate1, &framerate2);
	    if (framerate1 != 0 && framerate2 != 0)
	    {
		cvl_msg_dbg("frame rate is %d/%d fps (~%.2f)",
		     	framerate1, framerate2, (double)framerate1 / (double)framerate2);
	    }
	    else
	    {
		cvl_msg_dbg("frame rate is unknown");
	    }
	    strings2framerangelist(argv + first_argument, ranges_len, 
		    framerate1, framerate2, &ranges);
	    for (int i = 0; i < ranges_len; i++)
	    {
		if (ranges[i].low == -1 || ranges[i].high == -1)
		{
		    cvl_msg_err("cannot use time ranges when frame rate is unknown");
		    error = true;
		    break;
		}
	     	else if (ranges[i].low < 0 || ranges[i].high < 0 || ranges[i].low > ranges[i].high)
		{
		    cvl_msg_err("frame range %d is invalid", i + 1);
		    error = true;
		    break;
		}
	    }
	    if (error)
	    {
		cvl_frame_free(frame);
		break;
	    }
	    ranges_len = normalize_frameranges(ranges, ranges_len);
	    cvl_msg_dbg("%d ranges left after merging overlapping ranges", ranges_len);
	    for (int i = 0; i < ranges_len; i++)
	    {
		if (ranges[i].high == LONG_MAX)
		{
		    cvl_msg_dbg("frame range %d: %ld - end", i, ranges[i].low);
		}
		else
		{
		    cvl_msg_dbg("frame range %d: %ld - %ld", i, ranges[i].low, ranges[i].high);
		}
	    }
	    ranges_parsed = true;
	}

	if (frameno_in_range(ranges, ranges_len, &ranges_index, frameno))
	{
	    keep_frame = !drop.value;
	}
	else
	{
	    keep_frame = drop.value;
	}

	if (keep_frame)
	{
	    //cvl_msg_dbg("keeping frame %ld", frameno);
	    if (!cvl_io_write(stdout, output_info, frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	}
	else
	{
	    //cvl_msg_dbg("dropping frame %ld", frameno);
	    dropcounter++;
	}
	cvl_frame_free(frame);

	frameno++;
    }
    cvl_msg_dbg("%ld frames processed, %ld kept, %ld dropped", 
	    frameno, frameno - dropcounter, dropcounter);

    free(ranges);
    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
