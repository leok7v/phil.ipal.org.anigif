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
// function	angif_init_image
//
// purpose	Initialize an image info struct.
//
// arguments	1 (angif_image *) pointer to image info struct
//
// returns	(angif_image *) pointer to image info struct
//		(angif_image *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_image() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_image *
angif_init_image (
	angif_image *	image
)
{
    //-- If no struct pointer given, then allocate one.
    if ( ! image ) {
	image = malloc( sizeof ( angif_image ) );
	if ( ! image ) return NULL;
    }

    //-- Fill in all the fields to initial state.
    image->data_red	= NULL;
    image->data_green	= NULL;
    image->data_blue	= NULL;
    image->data_rgb	= NULL;
    image->data_index	= NULL;
    image->data_prelzw	= NULL;
    image->gamma_red	= 0.0;
    image->gamma_green	= 0.0;
    image->gamma_blue	= 0.0;
    image->color_table	= NULL;
    image->color_count	= 0;
    image->image_width	= 0;
    image->image_height	= 0;
    image->image_left	= 0;
    image->image_top	= 0;
    image->array_width	= 0;
    image->array_height	= 0;
    image->array_left	= 0;
    image->array_top	= 0;
    image->delay_time	= 0;
    image->interlace	= 0;
    image->ct_sort	= 0;
    image->disposal	= 1;
    image->user_input	= 0;
    image->trans_flag	= 0;
    image->trans_index	= 0;

    //-- Return the image struct pointer.
    return image;
}
