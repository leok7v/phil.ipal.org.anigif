//-----------------------------------------------------------------------------
// Copyright © 1999 - Philip Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//-----------------------------------------------------------------------------
// package	angif
// version	1.0.0beta3
// homepage	http://phil.ipal.org/freeware/angif
//-----------------------------------------------------------------------------
// author	Philip Howard
// email	phil@ipal.org
// homepage	http://phil.ipal.org/
//-----------------------------------------------------------------------------
// This library is "patent free" in that it contains no code to implement
// the LZW compression, which is covered by a patent owned by Unisys.
//
// GIF is a service mark, property of CompuServe, Inc.
//
// The GIF standard is obsolete and is being deprecated in
// favor of the PNG (Portable Network Graphics) standard.
//-----------------------------------------------------------------------------

#include "angif_lib.h"

//-----------------------------------------------------------------------------
// function	angif_init_header
//
// purpose	Initialize the header struct.
//
// arguments	1 (angif_header *) pointer to header struct to initialize.
//
// returns	(angif_header *) pointer to header struct
//		(angif_header *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_header() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_header *
angif_init_header (
	angif_header *	header
)
{
    //-- If no struct pointer given, then allocate one.
    if ( ! header ) {
	header = malloc( sizeof ( angif_header ) );
	if ( ! header ) return NULL;
    }

    //-- Fill in all the fields to initial state.
    header->color_table  = NULL;
    header->color_count  = 0;
    header->width        = 0;
    header->height       = 0;
    header->bgc_index    = 0;
    header->aspect_ratio = 0;
    header->color_res    = 1;
    header->ct_sort      = 0;

    //-- Return the header struct pointer.
    return header;
}
