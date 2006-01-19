/*
 * cvl_draw.c
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
 * \file cvl_draw.h
 * \brief Drawing.
 *
 * Drawing points, lines, polygons, and circles. Currently very limited.
 */

#include "config.h"

#include <stdbool.h>
#include <stdlib.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_draw.h"


/**
 * \param frame		The frame.
 * \param p		The pixel value of the point.
 * \param x		The x coordinate of the point.
 * \param y		The y coordinate of the point.
 * 
 * Draws a point. Checks if \a x and \a y are inside the frame, and does nothing if
 * they are not.
 */
inline void cvl_draw_point(cvl_frame_t *frame, cvl_pixel_t p, int x, int y)
{
    if (x >= 0 && y >= 0 && x < cvl_frame_width(frame) && y < cvl_frame_height(frame))
    {
	cvl_frame_set(frame, x, y, p);
    }
}


/**
 * \param frame		The frame.
 * \param p		The pixel value of the line.
 * \param x0		x coordinate of the start point.
 * \param y0		y coordinate of the start point.
 * \param x1		x coordinate of the end point.
 * \param y1		y coordinate of the end point.
 * 
 * Draws a line. Start and/or end point do not need to be inside the frame.
 */
void cvl_draw_line(cvl_frame_t *frame, cvl_pixel_t p, int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    // |dy| <= |dx|: Decide for every x which y to draw
    if (abs(dy) <= abs(dx))
    {
	// always from left to right
	if (x1 < x0)
	{
	    int tmp = x0;
	    x0 = x1;
	    x1 = tmp;
	    tmp = y0;
	    y0 = y1;
	    y1 = tmp;
	    dx = -dx;
	    dy = -dy;
	}	
	int d = 2 * abs(dy) - dx;
	int dE = 2 * abs(dy);
	int dNE = 2 * (abs(dy) - dx);
	int x = x0;
	int y = y0;
	int y_step = (dy > 0) ? +1 : -1;
	cvl_draw_point(frame, p, x, y);
	while (x < x1)
	{
	    if (d <= 0)
	    {
		d += dE;
		x++;
	    }
	    else
	    {
		d += dNE;
		x++;
		y += y_step;
	    }
	    cvl_draw_point(frame, p, x, y);
	}
    }
    // |dy| > |dx|: Decide for every y which x to draw
    else
    {
	// always from bottom to top
	if (y1 < y0)
	{
	    int tmp = x0;
	    x0 = x1;
	    x1 = tmp;
	    tmp = y0;
	    y0 = y1;
	    y1 = tmp;
	    dx = -dx;
	    dy = -dy;
	}	
	int d = 2 * abs(dx) - dy;
	int dE = 2 * abs(dx);
	int dNE = 2 * (abs(dx) - dy);
	int x = x0;
	int y = y0;
	int x_step = (dx > 0) ? +1 : -1;
    	cvl_draw_point(frame, p, x, y);
	while (y < y1)
	{
	    if (d <= 0)
	    {
		d += dE;
		y++;
	    }
	    else
	    {
		d += dNE;
		y++;
		x += x_step;
	    }
	    cvl_draw_point(frame, p, x, y);
	}
    }
}


/*
 * Fills an object in a temporary frame. The pixel value 0 is expected to mean
 * "no color yet", 1 is expected to mean "this is the object border". The inside
 * of the object will be filled with pixel value 2.
 */
static void cvl_draw_fill_object(cvl_frame_t *tmpframe)
{
    typedef enum { OUTSIDE, BORDER_INWARDS, BORDER_OUTWARDS, INSIDE } state_t;
    
    for (int y = 0; y < cvl_frame_height(tmpframe); y++)
    {
	state_t state = OUTSIDE;
	int object_start = -1;
	for (int x = 0; x < cvl_frame_width(tmpframe); x++)
	{
	    cvl_pixel_t p = cvl_frame_get(tmpframe, x, y);
	    switch (state)
	    {
		case OUTSIDE:
		    if (p == 1)
		    {
			state = BORDER_INWARDS;
		    }
		    break;

		case BORDER_INWARDS:
		    if (p == 0)
		    {
			state = INSIDE;
			object_start = x;
		    }
		    break;

		case INSIDE:
		    if (p == 1)
		    {
			state = BORDER_OUTWARDS;
			for (int o = object_start; o <= x - 1; o++)
			{
			    cvl_frame_set(tmpframe, o, y, 2);
			}
		    }
		    break;

		case BORDER_OUTWARDS:
		    if (p == 0)
		    {
			state = OUTSIDE;
		    }
		    break;
	    }
	}
    }
}

/*
 * Transfers an object from a temporary frame into the real frame at position
 * (x0,y0). Border pixels are drawn with the border pixel value, the object's inside
 * is drawn in the fill pixel value, pixels outside of the object are not touched in
 * the real frame.
 */
static void cvl_draw_transfer_object(cvl_frame_t *frame, int x0, int y0, 
	cvl_pixel_t border, cvl_pixel_t fill, cvl_frame_t *tmpframe)
{
    for (int y = 0; y < cvl_frame_height(tmpframe); y++)
    {
	for (int x = 0; x < cvl_frame_width(tmpframe); x++)
	{
	    cvl_pixel_t p = cvl_frame_get(tmpframe, x, y);
	    if (p == 1)
	    {
		cvl_draw_point(frame, border, x + x0, y + y0);
	    }
	    else if (p == 2)
	    {
		cvl_draw_point(frame, fill, x + x0, y + y0);
	    }
	}
    }
}

/**
 * \param frame		The frame.
 * \param p		The pixel value of the polygon border.
 * \param points	The points in an integer array.
 * \param numpoints	The number of points in the integer array.
 *
 * Draws a polygon.
 * The points must be given in the following way:
 * points[0] = x0, points[1] = y0, points[2] = x1, points[3] = y1, ...
 * The array length is thus (2 * \a numpoints).
 * The points must describe a convex polygon.
 */
void cvl_draw_polygon(cvl_frame_t *frame, cvl_pixel_t p, int *points, int numpoints)
{
    for (int i = 0; i < numpoints - 1; i++)
    {
	cvl_draw_line(frame, p, points[2 * i], points[2 * i + 1], 
		points[2 * i + 2], points[2 * i + 3]);
    }
    cvl_draw_line(frame, p, points[2 * (numpoints - 1)], points[2 * (numpoints - 1) + 1],
	    points[0], points[1]);
}

/**
 * \param frame		The frame
 * \param border	The pixel value of the polygon border.
 * \param fill		The pixel value that the polygon will be filled with.
 * \param points	The points in an integer array.
 * \param numpoints	The number of points in the integer array.
 *
 * Draws a filled polygon.
 * The points must be given in the following way:
 * points[0] = x0, points[1] = y0, points[2] = x1, points[3] = y1, ...
 * The array length is thus (2 * \a numpoints).
 * The points must describe a convex polygon.
 */
void cvl_draw_polygon_filled(cvl_frame_t *frame, cvl_pixel_t border, cvl_pixel_t fill,
	int *points, int numpoints)
{
    int xmin = points[0];
    int xmax = points[0];
    int ymin = points[1];
    int ymax = points[1];

    /* Find min and max coordinates */
    for (int i = 1; i < numpoints; i++)
    {
	if (points[2 * i] < xmin)
	{
	    xmin = points[2 * i];
	}
	else if (points[2 * i] > xmax)
	{
	    xmax = points[2 * i];
	}
	if (points[2 * i + 1] < ymin)
	{
	    ymin = points[2 * i + 1];
	}
	else if (points[2 * i + 1] > ymax)
	{
	    ymax = points[2 * i + 1];
	}
    }

    /* Draw the polygon outlines into a temporary frame. The gray values of this
     * frame have the following meaning:
     * 0 - outside of polygon
     * 1 - border of polygon
     * 2 - inside of polygon
     */
    cvl_frame_t *tmpframe = cvl_frame_new(CVL_PIXEL_GRAY, xmax - xmin + 1, ymax - ymin + 1);
    cvl_frame_zero(tmpframe);
    for (int i = 0; i < numpoints - 1; i++)
    {
	cvl_draw_line(tmpframe, 1, points[2 * i] - xmin, points[2 * i + 1] - ymin, 
		points[2 * i + 2] - xmin, points[2 * i + 3] - ymin);
    }
    cvl_draw_line(tmpframe, 1,
	    points[2 * (numpoints - 1)] - xmin, points[2 * (numpoints - 1) + 1] - ymin, 
	    points[0] - xmin, points[1] - ymin);
    cvl_draw_fill_object(tmpframe);
    
    /* Now transfer the polygon to the real frame */
    cvl_draw_transfer_object(frame, xmin, ymin, border, fill, tmpframe);
    cvl_frame_free(tmpframe);
}

/* Helper function for circle drawing. */
static void cvl_draw_circle_pixels(cvl_frame_t *frame, int x, int y, int r)
{
    cvl_frame_set(frame,  x + r,  y + r, 1);
    cvl_frame_set(frame, -x + r,  y + r, 1);
    cvl_frame_set(frame,  x + r, -y + r, 1);
    cvl_frame_set(frame, -x + r, -y + r, 1);
    cvl_frame_set(frame,  y + r,  x + r, 1);
    cvl_frame_set(frame, -y + r,  x + r, 1);
    cvl_frame_set(frame,  y + r, -x + r, 1);
    cvl_frame_set(frame, -y + r, -x + r, 1);
}

/* Helper function for cirlce drawing. */
static void cvl_draw_circle_common(bool fill, cvl_frame_t *frame, cvl_pixel_t border_p, cvl_pixel_t fill_p, 
	int x0, int y0, int r)
{
    /* Use a temporary frame and copy it into the real frame later */    
    cvl_frame_t *tmpframe = cvl_frame_new(CVL_PIXEL_GRAY, 2 * r + 1, 2 * r + 1);
    cvl_frame_zero(tmpframe);
    /* Draw the circle outline into the temporary frame with pixel value 1 */
    int x = 0;
    int y = r;
    int d = 1 - r;
    int ddE = 3;
    int ddSE = 5 - 2 * r;
    cvl_draw_circle_pixels(tmpframe, x, y, r);
    while (y > x)
    {
	if (d < 0)
	{
	    d += ddE;
	    ddE += 2;
	    ddSE += 2;
	    x++;
	}
	else
	{
	    d += ddSE;
	    ddE += 2;
	    ddSE += 4;
	    x++;
	    y--;
	}
	cvl_draw_circle_pixels(tmpframe, x, y, r);
    }
    /* Fill the circle in the temporary frame with pixel value 2 */
    if (fill)
    {
	cvl_draw_fill_object(tmpframe);
    }
    /* Transfer the circle from the tmpframe into the real frame */
    cvl_draw_transfer_object(frame, x0 - r, y0 - r, border_p, fill_p, tmpframe);
    cvl_frame_free(tmpframe);
}

/**
 * \param frame		The frame.
 * \param p		The pixel value of the circle border.
 * \param x0		The x coordinate of the circle center.
 * \param y0		The y coordinate of the circle center.
 * \param r		The radius of the circle in pixels.
 *
 * Draws a circle.
 */
void cvl_draw_circle(cvl_frame_t *frame, cvl_pixel_t p, int x0, int y0, int r)
{
    cvl_draw_circle_common(false, frame, p, p, x0, y0, r);
}

/**
 * \param frame		The frame.
 * \param border	The pixel value of the circle border.
 * \param fill		The pixel value that the circle will be filled with.
 * \param x0		The x coordinate of the circle center.
 * \param y0		The y coordinate of the circle center.
 * \param r		The radius of the circle in pixels.
 *
 * Draws a filled circle.
 */
void cvl_draw_circle_filled(cvl_frame_t *frame, cvl_pixel_t border, cvl_pixel_t fill, int x0, int y0, int r)
{
    cvl_draw_circle_common(true, frame, border, fill, x0, y0, r);
}
