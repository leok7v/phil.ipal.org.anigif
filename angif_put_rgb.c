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
// function	angif_put_rgb
//
// purpose	Output a coded RGB mode image array to a stream target.
//
// arguments	1 (angif_stream *)	stream target
//		2 (angif_image *)	image object
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_rgb (
	angif_stream *	stream
,
	angif_image *	image
)
{
    angif_image		new_image	;

    long		color_table	[256];

    int			color_hash	[ANGIF_COLOR_MODULUS];

    long *		rgb_row_ptr	;
    long *		rgb_pix_ptr	;

    unsigned char *	index_row_ptr	;
    unsigned char *	index_row_end	;
    unsigned char *	index_pix_ptr	;
    unsigned char *	index_pix_end	;

    unsigned char *	index_array	;

    long		array_left	;
    long		array_top	;
    long		array_width	;
    long		array_height	;

    long		image_width	;
    long		image_height	;

    unsigned int	rgb_pix		;

    int			color_count	;// Count of actual colors in table
    int			table_vec	;
    int			hash_vec	;
    int			trans_flag	;
    int			trans_index	;
    int			split		;
    int			rc		;


    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! image  ) return ANGIF_ERROR_ARG_IMAGE;

    //-- Make sure we have an RGB image.
    if ( ! image->data_rgb ) return ANGIF_ERROR_DATA;

    //-- Get frequently used info.
    image_width  = image->image_width;
    image_height = image->image_height;
    array_width  = image->array_width;
    array_height = image->array_height;
    array_left   = image->array_left;
    array_top 	 = image->array_top;

    //-- Verify image geometry.
    if ( image_width  > ( array_width  - array_left ) ||
	 image_height > ( array_height - array_top  ) ) {
	return ANGIF_ERROR_GEOMETRY;
    }

    //-- Allocate an index image array.
    index_array = malloc( (size_t) ( image_width * image_height ) );
    if ( ! index_array ) return ANGIF_ERROR_ALLOC;

    //-- Initialize the new image struct.
    angif_init_image( & new_image );

    //-- Initialize the color table to empty.
    for ( color_count = 0; color_count < 256; ++ color_count ) {
	color_table[color_count] = -2;
    }
    color_count = 0;

    //-- Initialize the color hash;
    for ( hash_vec = 0; hash_vec < ANGIF_COLOR_MODULUS; ++ hash_vec ) {
	color_hash[hash_vec] = -1;
    }

    //-- Initialize with no transparency.
    trans_flag	= 0;
    trans_index = 0;

    //-- Setup the row starting and ending pointers.
    rgb_row_ptr = image->data_rgb + array_top * array_width + array_left;
    index_row_ptr = index_array;
    index_row_end = index_array + image_height * image_width;

    //-- Go through all rows of the image.
    while ( index_row_ptr < index_row_end ) {

        //-- Setup the pixel starting and ending pointers from the rows.
	rgb_pix_ptr = rgb_row_ptr;
	index_pix_ptr = index_row_ptr;
	index_pix_end = index_row_ptr + image_width;

	//-- Go through all the pixels of the image row.
	while ( index_pix_ptr < index_pix_end ) {

	    //-- Get a pixel and look for it in the color table.
	    rgb_pix = * (rgb_pix_ptr ++);
	    hash_vec = ( (unsigned int) ( rgb_pix + 2 ) ) % ANGIF_COLOR_MODULUS;
	    //-- This loop must exit because color_hash cannot fill up
	    //-- as long as ANGIF_COLOR_MODULUS > 256.
	    for (;;) {
		if ( (table_vec = color_hash[hash_vec]) < 0 ) break;
		if ( color_table[table_vec] == rgb_pix ) break;
		if ( (++ hash_vec) >= ANGIF_COLOR_MODULUS ) hash_vec = 0;
	    }

	    //-- If the color is new, count it and put it in the table;
	    if ( table_vec < 0 ) {

		//-- Assign the color and count it.
		table_vec = color_count;
		++ color_count;

		//-- If the color table overflows, the image needs to be split.
		if ( color_count > 256 ) break;

		//-- Put the new color in the hash and table.
		color_hash[hash_vec] = table_vec;
		color_table[table_vec] = rgb_pix;

		//-- If the color is transparent, then handle it.
		//-- Leave the -1 in the color table to avoid false lookups.
		if ( rgb_pix == -1 ) {
		    trans_flag = 1;
		    trans_index = table_vec;
		}
	    }

	    //-- Put the color table index in the image index.
	    * (index_pix_ptr ++) = table_vec;
	}

	//-- If the color table overflows, the image needs to be split.
	if ( color_count > 256 ) break;

	//-- Point to next row.
	rgb_row_ptr += array_width;
	index_row_ptr += image_width;
    }

    //-- Sanity check color count.
    if ( color_count < 2 ) color_count = 2;

    //-- Prepare common aspects of new image.
    new_image.image_left	= image->image_left;
    new_image.image_top		= image->image_top;
    new_image.interlace		= image->interlace;
    new_image.disposal		= image->disposal;

    //-- The compiler is picky about this.
    rc = 0;

    //-- If we have a complete index image:
    if ( color_count <= 256 ) {

	//-- Build a new image struct for the index image.
	new_image.data_index	= index_array;
	new_image.color_table	= color_table;
	new_image.color_count	= color_count;
	new_image.image_width	= image_width;
	new_image.image_height	= image_height;
	new_image.array_width	= image_width;
	new_image.array_height	= image_height;
	new_image.delay_time	= image->delay_time;
	new_image.user_input	= image->user_input;
	new_image.trans_flag	= trans_flag;
	new_image.trans_index	= trans_index;

	//-- Put this image as an index image.
	rc = angif_put_index( stream, & new_image );
    }
	
    //-- Free the index image array.
    free( index_array );

    //-- If the color table overflowed:
    if ( color_count > 256 ) {

	//-- Initialize the new image for splitting.
	new_image.data_rgb	= image->data_rgb;
	new_image.array_width	= array_width;
	new_image.array_height	= array_height;
	new_image.array_left	= image->array_left;
	new_image.array_top	= image->array_top;
	new_image.delay_time	= 0; // no delay between halves
	new_image.user_input	= 0; // no user input between halves

	//-- Split along a vertical line between left and right halves.
	if ( image_width > image_height ) {
	    //-- Calculate split with alignment to multiple of 16.
	    split = ( ( image_width / 2 ) + 15 ) & ~15;

	    //-- Put left half.
	    new_image.image_width	= split;
	    new_image.image_height	= image_height;
	    rc = angif_put_rgb( stream, & new_image );
	    if ( rc != 0 ) return rc;

	    //-- Put right half.
	    new_image.image_width	= image_width - split;
	    new_image.image_left	= image->image_left + split;
	    new_image.array_left	= image->array_left + split;
	    new_image.delay_time	= image->delay_time;
	    new_image.user_input	= image->user_input;
	    rc = angif_put_rgb( stream, & new_image );
	}

	//-- Split along a horizontal line between top and bottom halves.
	else {
	    //-- Calculate split with alignment to multiple of 16.
	    split = ( ( image_height / 2 ) + 15 ) & ~15;

	    //-- Put top half.
	    new_image.image_width	= image_width;
	    new_image.image_height	= split;
	    rc = angif_put_rgb( stream, & new_image );
	    if ( rc != 0 ) return rc;

	    //-- Put bottom half.
	    new_image.image_height	= image_height - split;
	    new_image.image_top		= image->image_top + split;
	    new_image.array_top		= image->array_top + split;
	    new_image.delay_time	= image->delay_time;
	    new_image.user_input	= image->user_input;
	    rc = angif_put_rgb( stream, & new_image );
	}

    }

    //-- Return the final result.
    return rc;
}
