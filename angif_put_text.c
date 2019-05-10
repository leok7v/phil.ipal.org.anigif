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
// function	angif_put_text
//
// purpose	Output a text block array to a GIF stream target.
//
// arguments	1 (angif_stream *) pointer to GIF stream.
//		2 (angif_text *) pointer to text object.
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_text (
	angif_stream *	stream
,
	angif_text *	text
)
{
    unsigned char		buffer		[256];
    unsigned char *		buf_ptr		;

    const unsigned char *	text_ptr	;
    size_t			text_len	;

    int				rc		;

    rc = 0;

    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! text ) return ANGIF_ERROR_ARG_TEXT;

    //-- Get text data and check it.
    text_ptr = text->text_data;
    text_len = text->text_len;
    if ( text_len < 0 ) {
	text_len = strlen( (const char *) text_ptr );
	if ( text_len < 0 ) return ANGIF_ERROR_TEXT;
    }
    if ( text_len == 0 ) return 0;

    //-- Setup buffer pointer.
    buf_ptr = buffer;

    //-- Fill in text block.
    * (buf_ptr ++) = 0x21;	// extension introducer
    * (buf_ptr ++) = 0x01;	// plain text extension
    * (buf_ptr ++) = 12;	// block size

    * (buf_ptr ++) = text->pos_left;
    * (buf_ptr ++) = text->pos_left >> 8;

    * (buf_ptr ++) = text->pos_top;
    * (buf_ptr ++) = text->pos_top >> 8;

    * (buf_ptr ++) = text->text_width;
    * (buf_ptr ++) = text->text_width >> 8;

    * (buf_ptr ++) = text->text_height;
    * (buf_ptr ++) = text->text_height >> 8;

    * (buf_ptr ++) = text->cell_width;
    * (buf_ptr ++) = text->cell_height;
    * (buf_ptr ++) = text->fg_color;
    * (buf_ptr ++) = text->bg_color;

    //-- Flush the buffer.
    if ( (rc = angif_write( stream, buffer, (size_t) ( buf_ptr - buffer ) )) ) return rc;

    //-- Write all full blocks first.
    buffer[0] = 255;
    while ( text_len > 255 ) {
	if ( (rc = angif_write( stream, buffer, 1 )) ) return rc;
	if ( (rc = angif_write( stream, text_ptr, 255 )) ) return rc;
	text_ptr += 255;
	text_len -= 255;
    }

    //-- Write last short block.
    buffer[0] = text_len;
    if ( (rc = angif_write( stream, buffer, 1 )) ) return rc;
    if ( (rc = angif_write( stream, text_ptr, text_len )) ) return rc;

    //-- Write block terminator.
    buffer[0] = 0;
    rc = angif_write( stream, buffer, 1 );

    //-- All done.
    return rc;
}
