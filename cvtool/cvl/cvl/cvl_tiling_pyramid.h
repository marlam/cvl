/*
 * cvl_tiling_pyramid.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_TILING_PYRAMID_H
#define CVL_TILING_PYRAMID_H

#include <stdio.h>


typedef struct
{
    int data_width;
    int data_height;
    int data_element_size;
    int tile_width;
    int tile_height;
    int tile_overlap_horizontal;
    int tile_overlap_vertical;

    int tiles;
    int tile_size;
    int tile_net_width;
    int tile_net_height;
    int levels;
    int *level_width;
    int *level_height;

    FILE *f;
    off_t _offset;
} cvl_tp_t;

cvl_tp_t *cvl_tp_new(
	int data_width, int data_height, int data_element_size,
	int tile_width, int tile_height, 
	int tile_overlap_horizontal, int tile_overlap_vertical,
	void (*progress_indicator)(int percent),
	FILE *f, off_t offset);

void cvl_tp_free(cvl_tp_t *tp);

int cvl_tp_data_width(cvl_tp_t *tp);
int cvl_tp_data_height(cvl_tp_t *tp);
int cvl_tp_data_element_size(cvl_tp_t *tp);

int cvl_tp_tile_width(cvl_tp_t *tp);
int cvl_tp_tile_height(cvl_tp_t *tp);
int cvl_tp_tile_overlap_horizontal(cvl_tp_t *tp);
int cvl_tp_tile_overlap_vertical(cvl_tp_t *tp);
int cvl_tp_tile_net_width(cvl_tp_t *tp);
int cvl_tp_tile_net_height(cvl_tp_t *tp);

int cvl_tp_tiles(cvl_tp_t *tp);
int cvl_tp_levels(cvl_tp_t *tp);
int cvl_tp_level_width(cvl_tp_t *tp, int l);
int cvl_tp_level_height(cvl_tp_t *tp, int l);

int cvl_tp_tile_index(cvl_tp_t *tp, int lx, int ly, int ll);
void cvl_tp_tile_coordinates(cvl_tp_t *tp, int tn, int *lx, int *ly, int *ll);

void *cvl_tp_lock_tile(cvl_tp_t *tp, int tn);
void cvl_tp_unlock_tile(cvl_tp_t *tp, void *tile);

void cvl_tp_get(int x, int y, int level, void *dst);

#endif
