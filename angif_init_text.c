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
// function	angif_init_text
//
// purpose	Initialize a text info struct.
//
// arguments	1 (angif_text *) pointer to text info struct
//
// returns	(angif_text *) pointer to text info struct
//		(angif_text *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_text() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_text *
angif_init_text (
	angif_text *	text
)
{
    //-- If no struct pointer given, then allocate one.
    if ( ! text ) {
	text = malloc( sizeof ( angif_text ) );
	if ( ! text ) return NULL;
    }

    //-- Fill in all the fields to initial state.
    text->pos_left	= 0;
    text->pos_top	= 0;
    text->text_width	= 0;
    text->text_height	= 0;
    text->array_width	= 0;
    text->array_height	= 0;
    text->offset_left	= 0;
    text->offset_top	= 0;
    text->cell_width	= 0;
    text->cell_height	= 0;
    text->fg_color	= 0;
    text->bg_color	= 0;

    //-- Return the text struct pointer.
    return text;
}
