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
// function	angif_set_memory
//
// purpose	Setup a stream for memory writing.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//		2 (size_t) initial size to use or 0 to cancel
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//
// note		In memory GIF streams will dynamically expand as needed.
//		If controls or limits need to be applied to the stream by
//		the calling application, consider using angif_set_function.
//-----------------------------------------------------------------------------
int
angif_set_memory (
	angif_stream *	stream
,
	size_t		size
)
{
    unsigned char *	memory_base	;

    //-- If no struct pointer given, return with error.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;

    //-- Clear out any previous memory.
    if ( stream->memory_begin ) {
	free( stream->memory_begin );
    }
    stream->memory_pos = NULL;
    stream->memory_end = NULL;

    //-- If size is 0 then just cancel the memory writing.
    if ( size == 0 ) return 0;

    //-- Make sure the size is reasonable.
    if ( size < ANGIF_MINIMUM_MEMORY_SIZE ) size = ANGIF_INITAL_MEMORY_SIZE;

    //-- Allocate target memory.
    if ( ! ( memory_base = malloc( size ) ) ) return ANGIF_ERROR_MEMORY;

    //-- Set up this memory.
    stream->memory_begin = memory_base;
    stream->memory_pos = memory_base;
    stream->memory_end = memory_base + size;

    //-- Return with success.
    return 0;
}
