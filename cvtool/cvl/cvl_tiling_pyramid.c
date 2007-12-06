/*
 * cvl_tiling_pyramid.c
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

/**
 * \file cvl_tiling_pyramid.h
 * \brief Data management using tiling pyramids.
 *
 * Data management using tiling pyramids.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "mh.h"

#include "cvl/cvl_tiling_pyramid.h"


/**
 * \struct cvl_tp_t
 * \brief A tiling pyramid.
 *
 * A tiling pyramid.
 */


/**
 * \param data_width
 * \param data_height
 * \param data_element_size
 * \param tile_width
 * \param tile_height
 * \param tile_overlap_horizontal
 * \param tile_overlap_vertical
 * \param progress_indicator
 * \param f
 * \param offset
 *
 * Creates a new tiling pyramid 
 */
cvl_tp_t *cvl_tp_new(
	int data_width, int data_height, int data_element_size,
	int tile_width, int tile_height, 
	int tile_overlap_horizontal, int tile_overlap_vertical,
	void (*progress_indicator)(int percent),
	FILE *f, off_t offset)
{
}


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


TilingPyramid::TilingPyramid(TilingPyramidData *data,
	size_t tile_width, size_t tile_height, 
	size_t tile_overlap_horizontal, size_t tile_overlap_vertical,
	void (*progress_indicator)(int percent),
	FILE *f, off_t offset) throw(err)
{
    mh_assert(data != NULL);
    mh_assert(data->width() > 0);
    mh_assert(data->height() > 0);
    mh_assert(data->element_size() > 0);
    mh_assert(mh_pfits(data->width(), data->height()));
    mh_assert(tile_width > 0);
    mh_assert(tile_width % 2 == 0);
    mh_assert(tile_height > 0);
    mh_assert(tile_height % 2 == 0);
    mh_assert(mh_pfits(tile_width, tile_height));
    mh_assert(tile_overlap_horizontal < tile_width / 2);
    mh_assert(tile_overlap_horizontal % 2 == 0);
    mh_assert(tile_overlap_horizontal < static_cast<size_t>(data->width()));
    mh_assert(tile_overlap_vertical < tile_height / 2);
    mh_assert(tile_overlap_vertical % 2 == 0);
    mh_assert(tile_overlap_vertical < static_cast<size_t>(data->height()));
    mh_assert(mh_pfits(tile_width * tile_height, data->element_size()));
    mh_assert(tile_width * tile_height * data->element_size() % static_cast<size_t>(getpagesize()) == 0);
    mh_assert(f == NULL || offset % getpagesize() == 0);

    _data_width = data->width();
    _data_height = data->height();
    _data_element_size = data->element_size();
    _tile_width = tile_width;
    _tile_height = tile_height;
    _tile_overlap_horizontal = tile_overlap_horizontal;
    _tile_overlap_vertical = tile_overlap_vertical;

    mh_msg_dbg("Building tiling pyramid for %zux%zu data with elements of size %zu", 
	    _data_width, _data_height, _data_element_size);

    /* Compute derived values */

    _tile_size = _tile_width * _tile_height * _data_element_size;
    _tile_net_width = _tile_width - 2 * _tile_overlap_horizontal;
    _tile_net_height = _tile_height - 2 * _tile_overlap_vertical;

    // With <= 64bit size_t for data_width, data_height, data_element_size, 
    // one can never have more than 65 levels (probably much less).
    _level_width = new size_t[65];
    _level_height = new size_t[65];
    _tiles = 0;
    _levels = 0;
    do
    {
	mh_assert(_levels < 65);
	_levels++;
	size_t level_factor = mh_pows(2, _levels - 1);
	_level_width[_levels - 1] = mh_maxs(1, _data_width / (level_factor * _tile_net_width)
		+ (_data_width % (level_factor * _tile_net_width) == 0 ? 0 : 1));
	_level_height[_levels - 1] = mh_maxs(1, _data_height / (level_factor * _tile_net_height)
		+ (_data_height % (level_factor * _tile_net_height) == 0 ? 0 : 1));
	_tiles += _level_width[_levels - 1] * _level_height[_levels - 1];
	mh_msg_DBG("At level %zu: %zux%zu tiles", 
		_levels - 1, _level_width[_levels - 1], _level_height[_levels - 1]);
    }
    while (_level_width[_levels - 1] > 1 || _level_height[_levels - 1] > 1);

    if (progress_indicator)
    {
	progress_indicator(0);
    }

    if (f)
    {
	/* Use existing cache file */
	_f = f;
	_offset = offset;
	CIO::disable_buffering(_f);
    }
    else
    {
	/* Create cache file */
	_f = CIO::tempfile(PACKAGE_NAME "-tiling_pyramid-");
	_offset = 0;
	CIO::disable_buffering(_f);

	// Base level
	size_t tilecounter = 0;
	char *tile = static_cast<char *>(mh_alloc(_tile_size));
	char *data_buf = static_cast<char *>(mh_alloc(_tile_net_width * _data_element_size));
	for (size_t ty = 0; ty < _level_height[0]; ty++)
	{
	    int dy = ty * _tile_net_height - tile_overlap_vertical;
	    for (size_t tx = 0; tx < _level_width[0]; tx++)
	    {
		int dx = tx * _tile_net_width - tile_overlap_horizontal;
		if (dx >= 0 && dx + _tile_width < _data_width
			&& dy >= 0 && dy + _tile_height < _data_height)
		{
		    // Optimize the common case
		    data->get(dx, dy, _tile_width, _tile_height, tile);
		}
		else
		{
		    for (size_t y = 0; y < _tile_height; y++)
		    {
			// Coordinate reflection happens like this:
			// -2 -1 00 +1 +2 +3 ...  =>  +2 +1 00 +1 +2 +3 ...
			// I.e. the 00 entry is not repeated.
			// We assume (_data_height > _tile_overlap_vertical) and
			// (_data_width > _tile_overlap_horizontal)
			int ddy = dy + y;
			if (ddy < 0)
			{
			    ddy = min(static_cast<int>(_data_height) - 1, -ddy); 
			}
			else if (ddy >= static_cast<int>(_data_height))
			{
			    ddy = max(0, 2 * static_cast<int>(_data_height) - ddy - 2);
			}
			//mh_msg_dbg("L0Y%zuX%zu: line %zu: dy = %d, ddy = %d", ty, tx, y, dy, ddy);
			int cl = 0;
			// Left part
			if (dx < 0)
			{
			    data->get(1, ddy, -dx, 1, data_buf);
			    for (size_t x = 0; x < static_cast<size_t>(-dx); x++)
			    {
				memcpy(tile + (y * _tile_width + x) * _data_element_size,
				     	data_buf + (-dx - 1 - x) * _data_element_size,
					_data_element_size);
			    }
			    cl = -dx;
			}
			// Right part
			int cr = _tile_width - 1;
			if (dx + _tile_width > _data_width)
			{
			    int len = dx + _tile_width - _data_width;
			    data->get(_data_width - 1 - len, ddy, len, 1, data_buf);
			    for (size_t x = _tile_width - len; x < _tile_width; x++)
			    {
				memcpy(tile + (y * _tile_width + x) * _data_element_size,
				     	data_buf + (len - 1 - x) * _data_element_size,
					_data_element_size);
			    }
			    cr = _tile_width - len - 1;
			}
			// Center part: [cl,cr]
			data->get(dx + cl, ddy, cr - cl + 1, 1, tile + (y * _tile_width + cl) * _data_element_size);
		    }
		}
		mh_msg_dbg("Caching tile l=%zu, y=%zu, x=%zu", static_cast<size_t>(0), ty, tx);
		CIO::write(tile, _tile_size, 1, _f);
		tilecounter++;
		if (progress_indicator)
		{
		    progress_indicator(tilecounter * 100 / (_tiles - 1));
		}
	    }
	}
	free(data_buf);
	// Higher pyramid levels
	char *lower_tile = NULL;
	size_t lower_tile_mapped = SIZE_MAX;
	for (size_t tl = 1; tl < _levels; tl++)
	{
	    for (size_t ty = 0; ty < _level_height[tl]; ty++)
	    {
		for (size_t tx = 0; tx < _level_width[tl]; tx++)
		{
		    for (size_t y = 0; y < _tile_height; y++)
		    {
			size_t lower_ty, lower_tile_y;
			lower_level_coord(_level_height[tl - 1], _tile_height, _tile_overlap_vertical,
				ty, y, &lower_ty, &lower_tile_y);
			for (size_t x = 0; x < _tile_width; x++)
			{
			    size_t lower_tx, lower_tile_x;
			    lower_level_coord(_level_width[tl - 1], _tile_width, _tile_overlap_horizontal,
				    tx, x, &lower_tx, &lower_tile_x);

			    mh_assert(lower_tx < _level_width[tl - 1]);
			    mh_assert(lower_ty < _level_height[tl - 1]);
			    mh_assert(lower_tile_x < _tile_width);
			    mh_assert(lower_tile_y < _tile_height);

			    size_t lower_tile_tn = tile_index(lower_tx, lower_ty, tl - 1);
			    if (lower_tile_mapped != lower_tile_tn)
			    {
				if (lower_tile)
				{
				    unlock_tile(lower_tile);
				}
				lower_tile = static_cast<char *>(lock_tile(lower_tile_tn));
				lower_tile_mapped = lower_tile_tn;
			    }
			    char *elem00 = lower_tile + ((lower_tile_y    ) * _tile_width + (lower_tile_x    )) * _data_element_size;
			    char *elem01 = lower_tile + ((lower_tile_y    ) * _tile_width + (lower_tile_x + 1)) * _data_element_size;
			    char *elem10 = lower_tile + ((lower_tile_y + 1) * _tile_width + (lower_tile_x    )) * _data_element_size;
			    char *elem11 = lower_tile + ((lower_tile_y + 1) * _tile_width + (lower_tile_x + 1)) * _data_element_size;
			    data->mean(elem00, elem01, elem10, elem11, tile + (y * _tile_width + x) * _data_element_size);
			}
		    }
		    mh_msg_dbg("Caching tile l=%zu, y=%zu, x=%zu", tl, ty, tx);
		    CIO::write(tile, _tile_size, 1, _f);
		    tilecounter++;
		    if (progress_indicator)
		    {
			progress_indicator(tilecounter * 100 / (_tiles - 1));
		    }
		}
	    }
	}
	if (lower_tile)
	{
	    unlock_tile(lower_tile);
	}
	free(tile);
    }

    mh_msg_dbg("Caching done.");
    if (progress_indicator)
    {
	progress_indicator(100);
    }
}


TilingPyramid::~TilingPyramid()
{
    delete[] _level_width;
    delete[] _level_height;
    CIO::close(_f);
}


// Helper function
// This function can be used for horizontal parameters (x) and for vertical
// parameters (y).
// For the tile coordinate tc and the coordinate c within this tile, it computes
// the coordinates lower_tc of the corresponding tile on the lower pyramid level
// and the coordinate lower_tile_c within this lower tile.
// It needs the width/height of the lower pyramid level (lower_level_dim), the
// tile width/height (tile_dim), and the overlap (tile_overlap) as parameters. 
void TilingPyramid::lower_level_coord(size_t lower_level_dim, size_t tile_dim, size_t tile_overlap, 
	size_t tc, size_t c, size_t *lower_tc, size_t *lower_tile_c)
{
    if (c < tile_overlap)
    {
	if (tc == 0)
	{
	    *lower_tc = 0;
	    *lower_tile_c = 0;
	}
	else
	{
	    *lower_tc = tc * 2 - 1;
	    *lower_tile_c = tile_dim - tile_overlap - 2 * (tile_overlap - c);
	}
    }
    else if (c < tile_dim / 2)
    {
	*lower_tc = tc * 2;
	*lower_tile_c = (c - tile_overlap) * 2 + tile_overlap;
    }
    else if (c < tile_dim - tile_overlap)
    {
	*lower_tc = tc * 2 + 1;
	if (*lower_tc >= lower_level_dim)
	{
	    *lower_tc = tc * 2;
	    *lower_tile_c = tile_dim - 2;
	}
	else
	{
	    *lower_tile_c = (c - tile_dim / 2) * 2 + tile_overlap;
	}
    }
    else
    {
	*lower_tc = tc * 2 + 2;
	if (*lower_tc >= lower_level_dim)
	{
	    *lower_tc = tc * 2;
	    *lower_tile_c = tile_dim - 2;
	}
	else
	{
	    *lower_tile_c = (c - (tile_dim - tile_overlap)) * 2 + tile_overlap;
	}
    }
}


size_t TilingPyramid::tile_index(size_t tx, size_t ty, size_t tl)
{
    mh_assert(tl < _levels);
    mh_assert(tx < _level_width[tl]);
    mh_assert(ty < _level_height[tl]);

    size_t tn = 0;

    for (size_t l = 0; l < tl; l++)
    {
	tn += _level_width[l] * _level_height[l];
    }
    tn += ty * _level_width[tl] + tx;
    mh_assert(tn < _tiles);

    return tn;
}


void TilingPyramid::tile_coordinates(size_t tn, size_t *tx, size_t *ty, size_t *tl)
{
    mh_assert(tn < _tiles);
    mh_assert(tx != NULL);
    mh_assert(ty != NULL);
    mh_assert(tl != NULL);

    *tl = 0;
    while (tn >= _level_width[*tl] * _level_height[*tl])
    {
	tn -= _level_width[*tl] * _level_height[*tl];
	(*tl)++;
    }
    *ty = tn / _level_width[*tl];
    *tx = tn % _level_height[*tl];
}


void *TilingPyramid::lock_tile(size_t tn) throw(err)
{
    mh_assert(tn < _tiles);

    return CIO::map(_f, _offset + static_cast<off_t>(tn) * static_cast<off_t>(_tile_size), _tile_size);
}


void TilingPyramid::unlock_tile(void *data) throw(err)
{
    CIO::unmap(data, _tile_size);
}


void TilingPyramid::get(int x, int y, int level, void *dst) throw(err)
{
    mh_assert(x >= 0);
    mh_assert(y >= 0);
    mh_assert(x < static_cast<int>(_data_width));
    mh_assert(y < static_cast<int>(_data_height));
    mh_assert(level >= 0);
    mh_assert(level < static_cast<int>(_levels));

    /* Get the tiling pyramid coordinates lx,ly (for the tile in
     * the given level) and tx,ty (for the coordinates within
     * the tile) for the data element x,y.
     */
    size_t factor = mh_pows(2, level);
    size_t ltnw = factor * _tile_net_width;
    size_t ltnh = factor * _tile_net_height;
    size_t lx = x / ltnw;
    size_t ly = y / ltnh;
    size_t tx = (x - lx * ltnw) / factor + _tile_overlap_horizontal;
    size_t ty = (y - ly * ltnh) / factor + _tile_overlap_vertical;

    char *tile = static_cast<char *>(lock_tile(lx, ly, level));
    memcpy(dst, tile + (ty * _tile_width + tx) * _data_element_size, _data_element_size);
    unlock_tile(tile);
}
	size_t tile_width()
	{
	    return _tile_width;
	}

	size_t tile_height()
	{
	    return _tile_height;
	}

	size_t tile_overlap_horizontal()
	{
	    return _tile_overlap_horizontal;
	}

	size_t tile_overlap_vertical()
	{
	    return _tile_overlap_vertical;
	}

	size_t tiles()
	{
	    return _tiles;
	}

	size_t tile_net_width()
	{
	    return _tile_net_width;
	}

	size_t tile_net_height()
	{
	    return _tile_net_height;
	}

	size_t levels()
	{
	    return _levels;
	}

	size_t level_width(size_t l)
	{
	    mh_assert(l < _levels);

	    return _level_width[l];
	}
	
	size_t level_height(size_t l)
	{
	    mh_assert(l < _levels);

	    return _level_height[l];
	}

