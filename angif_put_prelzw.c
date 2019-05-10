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
// function	angif_put_prelzw
//
// purpose	Output pre-compressed LZW data to a stream target.
//
// arguments	1 (angif_stream *)	stream target
//		2 (angif_image *)	image object
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
// notice
//
// Due to the LZW compression algorithm, specified in the GIF format,
// being subject to a patent held by Unisys, and the unavailability of
// licensing for this patent, this code does not contain compression
// logic, and only generates uncompressed data codes.
//-----------------------------------------------------------------------------
int
angif_put_prelzw (
	angif_stream *	stream
,
	angif_image *	image
)
{
    unsigned char	buffer		[288];

    long *		color_ptr	;

    unsigned char *	buf_ptr		;

    int			color_count	;// Count of actual colors in table
    int			color_size	;
    int			ct_size		;// Color table size indicator (0..7 for 2^(n+1))
    int			rc		;


    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! image  ) return ANGIF_ERROR_ARG_IMAGE;

    //-- Make sure we have LZW data.
    if ( ! image->data_prelzw ) return ANGIF_ERROR_NO_LZW;

    //-- Calculate the color table size indicator.
    ct_size = 0;
    if ( image->color_table ) {
	color_count = image->color_count - 1;
	if ( color_count < 1 || color_count > 255 ) return ANGIF_ERROR_COLOR_COUNT;
	while ( (color_count >>= 1) ) ++ ct_size;
    }

    //-- Setup buffer pointer.
    buf_ptr = buffer;

    //-- Output a graphic control extension if needed.
    if ( image->delay_time || image->disposal || image->user_input || image->trans_flag ) {
	* (buf_ptr ++) = 0x21;	// extension introducer
	* (buf_ptr ++) = 0xf9;	// graphic control label
	* (buf_ptr ++) = 0x04;	// block size
	* (buf_ptr ++) = 0 +
	    ( 0x1c & ( image->disposal   << 2 ) ) +
	    ( 0x02 & ( image->user_input << 1 ) ) +
	    ( 0x01 &   image->trans_flag        );
	* (buf_ptr ++) = image->delay_time;
	* (buf_ptr ++) = image->delay_time >> 8;
	* (buf_ptr ++) = image->trans_index;
	* (buf_ptr ++) = 0;	// block terminator
    }

    //-- Output the image descriptor.
    * (buf_ptr ++) = 0x2c;	// image separator
    * (buf_ptr ++) = image->image_left;
    * (buf_ptr ++) = image->image_left >> 8;
    * (buf_ptr ++) = image->image_top;
    * (buf_ptr ++) = image->image_top >> 8;
    * (buf_ptr ++) = image->image_width;
    * (buf_ptr ++) = image->image_width >> 8;
    * (buf_ptr ++) = image->image_height;
    * (buf_ptr ++) = image->image_height >> 8;
    * (buf_ptr ++) = 0 +
	  ( image->color_table ? 0x80 : 0x00 ) +
	  ( 0x40 & ( image->interlace << 6 ) ) +
	  ( 0x20 & ( image->ct_sort   << 5 ) ) +
	  ( 0x07 &   ct_size                 );

    //-- If the local color table is provided, output it.
    if ( image->color_table ) {
	color_count = image->color_count;
	color_ptr   = image->color_table;
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

    //-- Flush the buffer.
    if ( (rc = angif_write( stream, buffer, (size_t) ( buf_ptr - buffer ) )) < 0 ) return rc;

    buf_ptr = image->data_prelzw;

    //-- Loop through all segments of the data block.
    for (;;) {
	//-- Write one segment, with length prefix.
	if ( (rc = angif_write( stream, buf_ptr, (size_t) * buf_ptr )) < 0 ) return rc;

	//-- If this was the last segment, then break out.
	if ( 0 == * buf_ptr ) break;
    }

    //-- All done.
    return 0;
}
