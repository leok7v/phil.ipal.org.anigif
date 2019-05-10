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
// function	angif_set_file
//
// purpose	Setup a stream for output to a stdio FILE.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//		2 (FILE *) already open FILE to use or NULL to cancel
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_set_file (
	angif_stream *	stream
,
	FILE *		file
)
{
    //-- If no struct pointer given, return with error.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;

    //-- Put in the FILE pointer.
    stream->out_file = file;

    //-- Return with success.
    return 0;
}
