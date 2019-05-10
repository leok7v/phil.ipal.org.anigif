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

#include <math.h>

//-----------------------------------------------------------------------------
// function	angif_linear_to_coded
//
// purpose	Convert a floating point linear luminance value to a gamma
//		corrected integer coded value.
//
// note		This function is only needed by angif_put_linear and is
//		inline to optimize compilation.
//
// arguments	1 (double) the linear value to be converted
//		2 (double) the inverted gamma value to do corrections with
//
// returns	(long) coded pixel value
//-----------------------------------------------------------------------------
static inline
int
angif_linear_to_code (
	double		linear_pixel
,
	double		inv_gamma
)
{
    long	coded	;

    if ( linear_pixel < 0.0 ) linear_pixel = 0.0;
    else if ( linear_pixel > 1.0 ) linear_pixel = 1.0;
    coded = (long) 256.0 * pow( linear_pixel, inv_gamma );
    if ( coded < 0 ) coded = 0;
    else if ( coded > 255 ) coded = 255;
    return coded;
}


//-----------------------------------------------------------------------------
// function	angif_put_linear
//
// purpose	Output a linear RGB mode image array to a stream target.
//
// arguments	1 (angif_stream *)	stream target
//		2 (angif_image *)	image object
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_linear (
	angif_stream *	stream
,
	angif_image *	image
)
{
    angif_image		image_rgb		;

    double		inv_gamma_red		;
    double		inv_gamma_green		;
    double		inv_gamma_blue		;

    float *		linear_pix_red		;
    float *		linear_pix_green	;
    float *		linear_pix_blue		;
    float *		linear_row_red		;
    float *		linear_row_green	;
    float *		linear_row_blue		;

    long *		rgb_array		;
    long *		rgb_pix_ptr		;
    long *		rgb_pix_end		;
    long *		rgb_row_ptr		;
    long *		rgb_row_end		;

    long		array_left		;
    long		array_top		;
    long		array_width		;
    long		array_height		;

    long		image_width		;
    long		image_height		;

    int			rc			;


    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! image  ) return ANGIF_ERROR_ARG_IMAGE;

    //-- Make sure we have a linear image.
    if ( ! image->data_red    ) return ANGIF_ERROR_DATA;
    if ( ! image->data_green  ) return ANGIF_ERROR_DATA;
    if ( ! image->data_blue   ) return ANGIF_ERROR_DATA;

    //-- Get frequently used info.
    image_width  = image->image_width;
    image_height = image->image_height;
    array_width  = image->array_width;
    array_height = image->array_height;
    array_left   = image->array_left;
    array_top    = image->array_top;

    //-- Verify image geometry.
    if ( image_width  > ( array_width  - array_left ) ||
	 image_height > ( array_height - array_top  ) ) {
	return ANGIF_ERROR_GEOMETRY;
    }

    //-- Allocate an RGB image array.
    rgb_array = malloc( sizeof (long) * image_width * image_height );
    if ( ! rgb_array ) return ANGIF_ERROR_ALLOC;

    //-- Prepare the gamma correction.
    inv_gamma_red   = 1.0 / ( ( image->gamma_red   <= 0.0 ) ? image->gamma_red   : ANGIF_DEFAULT_GAMMA );
    inv_gamma_green = 1.0 / ( ( image->gamma_green <= 0.0 ) ? image->gamma_green : ANGIF_DEFAULT_GAMMA );
    inv_gamma_blue  = 1.0 / ( ( image->gamma_blue  <= 0.0 ) ? image->gamma_blue  : ANGIF_DEFAULT_GAMMA );

    //-- Setup the row starting and ending pointers.
    linear_row_red   = image->data_red   + array_top * array_width + array_left;
    linear_row_green = image->data_green + array_top * array_width + array_left;
    linear_row_blue  = image->data_blue  + array_top * array_width + array_left;
    rgb_row_ptr = rgb_array;
    rgb_row_end = rgb_array + image_height * image_width;

    //-- Go through all rows of the image.
    while ( rgb_row_ptr < rgb_row_end ) {

	//-- Setup the pixel starting and ending pointers from the rows.
	linear_pix_red   = linear_row_red;
	linear_pix_green = linear_row_green;
	linear_pix_blue  = linear_row_blue;
	rgb_pix_ptr = rgb_row_ptr;
	rgb_pix_end = rgb_row_ptr + image_width;

	//-- Go through all the pixels of the image row.
	while ( rgb_pix_ptr < rgb_pix_end ) {
	    * (rgb_pix_ptr ++) =
		  ( angif_linear_to_code( * (linear_pix_red   ++), inv_gamma_red   ) << 16 ) +
	          ( angif_linear_to_code( * (linear_pix_green ++), inv_gamma_green ) <<  8 ) +
		  ( angif_linear_to_code( * (linear_pix_blue  ++), inv_gamma_blue  )       );
	}

	//-- Increment the row pointers to the next row.
	linear_row_red   += array_width;
	linear_row_green += array_width;
	linear_row_blue  += array_width;
	rgb_row_ptr += image_width;
    }

    //-----------------------------------------------------------------------------
    // We now have a complete coded RGB image.
    //-----------------------------------------------------------------------------

    //-- Build a new image struct for the coded RGB image.
    image_rgb.data_red		= NULL;
    image_rgb.data_green	= NULL;
    image_rgb.data_blue		= NULL;
    image_rgb.data_rgb		= rgb_array;
    image_rgb.data_index	= NULL;
    image_rgb.data_prelzw	= NULL;
    image_rgb.gamma_red		= 0.0;
    image_rgb.gamma_green	= 0.0;
    image_rgb.gamma_blue	= 0.0;
    image_rgb.color_table	= NULL;
    image_rgb.color_count	= 0;
    image_rgb.image_width	= image_width;
    image_rgb.image_height	= image_height;
    image_rgb.image_left	= image->image_left;
    image_rgb.image_top		= image->image_top;
    image_rgb.array_width	= image_width;
    image_rgb.array_height	= image_height;
    image_rgb.array_left	= 0;
    image_rgb.array_top		= 0;
    image_rgb.delay_time	= image->delay_time;
    image_rgb.interlace		= image->interlace;
    image_rgb.ct_sort		= 0;
    image_rgb.disposal		= image->disposal;
    image_rgb.user_input	= image->user_input;
    image_rgb.trans_flag	= 0;
    image_rgb.trans_index	= 0;

    //-- Put this image as a coded RGB image.
    rc = angif_put_rgb( stream, & image_rgb );

    //-- Free the coded RGB image array.
    free( rgb_array );

    //-- Return the result we got above.
    return rc;

}
