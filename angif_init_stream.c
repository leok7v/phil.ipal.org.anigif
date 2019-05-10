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
// function	angif_init_stream
//
// purpose	Initialize a stream object to which a GIF file will be written.
//
// arguments	1 (angif_stream *) pointer to stream object to initialize.
//
// returns	(angif_stream *) pointer to stream object struct
//		(angif_stream *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_stream() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_stream *
angif_init_stream (
	angif_stream *	stream
)
{
    //-- If no struct pointer given, then allocate one.
    if ( ! stream ) {
	stream = malloc( sizeof ( angif_stream ) );
	if ( ! stream ) return NULL;
    }

    //-- Fill in all the fields to initial state.
    stream->memory_begin	= NULL;
    stream->memory_end		= NULL;
    stream->memory_pos		= NULL;
    stream->call_fun		= NULL;
    stream->call_arg		= NULL;
    stream->out_file		= NULL;

    //-- Return the stream struct pointer.
    return stream;
}
