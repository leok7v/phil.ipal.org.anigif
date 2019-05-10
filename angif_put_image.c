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
// function	angif_put_image
//
// purpose	Output an image array to a GIF stream target.
//
// arguments	1 (angif_stream *) pointer to stream object.
//		2 (angif_image *) pointer to image object.
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_image (
	angif_stream *	stream
,
	angif_image *	image
)
{
    //-- Check arguments.
    if ( ! image ) return ANGIF_ERROR_ARG_IMAGE;

    //-- If there is LZW data present, use that.
    if ( image->data_prelzw ) {
	return angif_put_prelzw( stream, image );
    }

    //-- If there is index data present, use that.
    if ( image->data_index ) {
	return angif_put_index( stream, image );
    }

    //-- If there is coded RGB data present, use that.
    if ( image->data_rgb ) {
	return angif_put_rgb( stream, image );
    }

    //-- If there is linear RGB data present, use that.
    if ( image->data_red && image->data_green && image->data_blue ) {
	return angif_put_linear( stream, image );
    }

    return ANGIF_ERROR_NO_DATA;
}
