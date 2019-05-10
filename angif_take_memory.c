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
// function	angif_take_memory
//
// purpose	Take the allocated memory from the stream object and give
//		it to the calling program.  The memory is no longer part
//		of the stream object.  The caller must free() it when done.
//		Call angif_set_memory() to establish a replacement if more
//		GIF writing to memory is desired for the same stream.
//
// arguments	1 (angif_stream *) pointer to stream to getup
//		2 (unsigned char * *) where to store memory pointer
//		3 (size_t *) where to store length of actual GIF data
//		4 (size_t *) where to store allocated size of buffer
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_take_memory (
	angif_stream *		stream
,
	unsigned char * *	memory_p
,
	size_t *		len_p
,
	size_t *		size_p
)
{
    //-- If no struct pointer given, return with error.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;

    //-- If there is no memory set up, return an error.
    if ( ! stream->memory_begin ) {
	if ( memory_p ) * memory_p = NULL;
	if ( len_p    ) * len_p    = 0;
	if ( size_p   ) * size_p   = 0;
	return ANGIF_ERROR_NO_MEMORY;
    }

    //-- Store the values where the caller wants.
    if ( memory_p ) * memory_p = stream->memory_begin;
    if ( len_p    ) * len_p    = stream->memory_pos - stream->memory_begin;
    if ( size_p   ) * size_p   = stream->memory_end - stream->memory_begin;

    //-- Clear things out.
    stream->memory_begin = NULL;
    stream->memory_pos   = NULL;
    stream->memory_end   = NULL;

    //-- Return with success.
    return 0;
}
