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
// function	angif_write
//
// purpose	Write data to a target stream object.
//
// arguments	1 (angif_stream *) pointer to stream object
//		2 (const unsigned char *) address of data to write
//		3 (size_t) length of data to write
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_write (
	angif_stream *		arg_stream
,
	const unsigned char *	arg_data
,
	size_t			arg_length
)
{
    //-----------------
    // Check arguments.
    //-----------------
    if ( ! arg_stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( arg_length && ! arg_data ) return ANGIF_ERROR_ARG_DATA;
    if ( arg_length < 0 ) return ANGIF_ERROR_ARG_LEN;

    //----------------------------------------------------------
    // If a function call is requested, call the function first.
    //----------------------------------------------------------
    if ( arg_stream->call_fun ) {
	( *(arg_stream->call_fun) )( arg_stream->call_arg, arg_data, arg_length );
    }

    //------------------------------------------------------
    // If file output requested, let stdio do the buffering.
    //------------------------------------------------------
    if ( arg_stream->out_file ) {
	//-- If the length is zero, that is special for flush.
	if ( arg_length == 0 ) {
	    fflush( arg_stream->out_file );
	} else {
	    if ( fwrite( arg_data, 1, arg_length, arg_stream->out_file ) < arg_length ) {
		return ANGIF_ERROR_FWRITE;
	    }
	}
    }

    //-------------------------------------------------------------
    // If memory is requested, expand if needed, and copy the data.
    //-------------------------------------------------------------
    if ( arg_length && arg_stream->memory_begin ) {
	size_t pos;
	size_t size;

	pos = arg_stream->memory_pos - arg_stream->memory_begin;
	size = arg_stream->memory_end - arg_stream->memory_begin;
	if ( ( size - pos ) < arg_length ) {
	    do { size *= 2; } while ( ( size - pos ) < arg_length );
	    arg_stream->memory_begin = realloc( arg_stream->memory_begin, size );
	    if ( ! arg_stream->memory_begin ) return ANGIF_ERROR_MEMORY;
	    arg_stream->memory_pos = arg_stream->memory_begin + pos;
	    arg_stream->memory_end = arg_stream->memory_begin + size;
	    memcpy( arg_stream->memory_pos, arg_data, arg_length );
	    arg_stream->memory_pos += arg_length;
	}
    }

    //-- Return with success.
    return 0;
}
