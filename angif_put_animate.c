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
// function	angif_put_animate
//
// purpose	Output an animation loop block to a GIF stream.
//
// arguments	1 (angif_stream *) pointer to GIF stream object
//		2 (unsigned int) animation loop count, 0 = infinite
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_animate (
	angif_stream *	stream
,
	unsigned int	loop_count
)
{
    static const unsigned char	prefix	[17] = "\041\377\013NETSCAPE2.0\003\001";
    unsigned char		buffer	[24];

    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( loop_count > 65535U ) return ANGIF_ERROR_LOOP_COUNT;

    //-- Load most of the animation extension.
    memcpy( buffer, prefix, 16 );

    //-- Put in the loop count and finish.
    buffer[16] = loop_count;
    buffer[17] = loop_count >> 8;
    buffer[18] = 0;

    //-- Output the block.
    return angif_write( stream, buffer, 19 );
}
