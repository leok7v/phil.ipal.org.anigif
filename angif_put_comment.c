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
// function	angif_put_comment
//
// purpose	Output a comment block to a GIF stream target.
//
// arguments	1 (angif_stream *) pointer to GIF stream object.
//		2 (char *) comment string to output
//		3 (long) length of comment string
//		  (long) ~0 or -1 to use strlen() to get length
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_comment (
	angif_stream *	stream
,
	const char *	comment_str
,
	long		comment_len
)
{
    unsigned char	buffer	[256]	;
    unsigned char *	buf_ptr		;

    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! comment_str ) return ANGIF_ERROR_ARG_COMMENT;

    //-- Get string length if not given.
    if ( comment_len < 0 ) comment_len = strlen( comment_str );

    //-- Start the comment extension.
    buf_ptr = buffer;
    * (buf_ptr ++) = 0x21;	// extension introducer
    * (buf_ptr ++) = 0xfe;	// comment label
    angif_write( stream, buffer, 2 );

    //-- hold a length in the buffer for large comments.
    buffer[0] = 255;

    //-- Do each large buffer full.
    while ( comment_len > 255 ) {
	angif_write( stream, buffer, 1 );
	angif_write( stream, comment_str, 255 );
	comment_str += 255;
	comment_len -= 255;
    }

    //-- Do whatever remains.
    buffer[0] = comment_len;
    angif_write( stream, buffer, 1 );
    angif_write( stream, comment_str, (size_t) comment_len );

    //-- Finish with an empty block.
    buffer[0] = 0;
    angif_write( stream, buffer, 1 );

    //-- The comment block is now done.
    return 0;
}
