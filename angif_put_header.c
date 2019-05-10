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
// function	angif_put_header
//
// purpose	Output a GIF header to a GIF stream target.
//		The logical image descriptor is also included.
//
// arguments	1 (angif_stream *) GIF stream to write to
//		2 (angif_header *) GIF header data to be written
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_header (
	angif_stream *		stream
,
	angif_header *		header
)
{
    unsigned char	buffer		[16+3*256];
    long *		color_ptr	;
    unsigned char *	buf_ptr		;
    int			color_count	;
    int			ct_size		;
    int			color_size	;

    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! header ) return ANGIF_ERROR_ARG_HEADER;

    //-- Setup buffer pointer.
    buf_ptr = buffer;

    //-- Calculate the color table size indicator.
    if ( header->color_table && (color_count = header->color_count) > 1 ) {
	ct_size = 0;
	-- color_count;
	while ( (color_count >>= 1) ) {
	    ++ ct_size;
	}
    }
    else {
	ct_size = 1;
    }

    //-- Fill in GIF version.
    * (buf_ptr ++) = 'G';
    * (buf_ptr ++) = 'I';
    * (buf_ptr ++) = 'F';
    * (buf_ptr ++) = '8';
    * (buf_ptr ++) = '9';
    * (buf_ptr ++) = 'a';

    //-- Fill in screen description.
    * (buf_ptr ++) = header->width;
    * (buf_ptr ++) = header->width >> 8;
    * (buf_ptr ++) = header->height;
    * (buf_ptr ++) = header->height >> 8;
    * (buf_ptr ++) =
	  ( header->color_table ? 0x80 : 0x00 ) +	// color table flag
	  ( 0x70 & ( ct_size           << 4 ) ) +	// color resolution
	  ( 0x08 & ( header->ct_sort   << 3 ) ) +	// color table sort flag
	  ( 0x07 &   ct_size                  );	// color table size
    * (buf_ptr ++) = header->bgc_index;
    * (buf_ptr ++) = header->aspect_ratio;

    //-- If the global color table is provided, output it.
    if ( header->color_table ) {
        color_count = header->color_count;
        color_ptr   = header->color_table;
        color_size  = 2 << ct_size;

        //-- Output as many color table elements as exist.
        while ( color_count -- ) {
            * (buf_ptr ++) = * color_ptr >> 16;
            * (buf_ptr ++) = * color_ptr >> 8;
            * (buf_ptr ++) = * color_ptr;
            ++ color_ptr;
            -- color_size;
        }

        //-- Pad the output color table to a power of 2 elements.
        while ( color_size -- ) {
            * (buf_ptr ++) = 0;
            * (buf_ptr ++) = 0;
            * (buf_ptr ++) = 0;
        }
    }

    //-- Write the buffer out.
    return angif_write( stream, buffer, (size_t) ( buf_ptr - buffer ) );
}
