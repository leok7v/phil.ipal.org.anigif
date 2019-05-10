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
// function	angif_put_index
//
// purpose	Output an index mode image array to a stream target.
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
angif_put_index (
	angif_stream *	stream
,
	angif_image *	image
)
{
    //-----------------------------------------------------------------------------
    // This array defines the interlacing starting point and row increment.
    // Starting points are accessed from offset 3 downward to 1.
    // Row increments are accessed from offset 5 downward to 2.
    //-----------------------------------------------------------------------------
    static const unsigned char	interlace_table	[6]	= { 1, 1, 2, 4, 8, 8 };

    unsigned char	buffer	[1024]	;// The buffer where data blocks are built.

    long *		color_ptr	;

    unsigned char *	buf_ptr		;// Points to the current position in the data block buffer.
    unsigned char *	buf_end		;// Points to the end of the data block buffer for comparison.
    unsigned char *	img_end		;// Points to the end of the image data for comparison.
    unsigned char *	row_ptr		;// Points to the beginning of the current row.
    unsigned char *	row_end		;// Points to the end of the current row for comparison.
    unsigned char *	pix_ptr		;// Points to the current pixel being output.

    unsigned long	acc_data	;// Accumulate bits of the output data codes here.

    unsigned int	reset_code	;// The special code assigned to clear the decoder hash.
    unsigned int	end_code	;// The special code assigned to indicate the end of data.
    unsigned int	code_bits	;// The number of bits for codes.
    unsigned int	interlace	;// The current interlace group (4..1) or 0 for non-interlace.
    unsigned int	pixel_mask	;// Bit mask for pixel indexes to limit range to given bits.
    unsigned int	code_limit	;// ??
    unsigned int	code_count	;// ??

    int			acc_bits	;// How many bits are currently in the accum variable.
    int			color_count	;// Count of actual colors in table
    int			color_size	;
    int			ct_size		;// Color table size indicator (0..7 for 2^(n+1))
    int			rc		;


    //-- Check arguments.
    if ( ! stream ) return ANGIF_ERROR_ARG_STREAM;
    if ( ! image  ) return ANGIF_ERROR_ARG_IMAGE;

    //-- Make sure we have index data.
    if ( ! image->data_index ) return ANGIF_ERROR_NO_INDEX;

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
    * (buf_ptr ++) =
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

    //-----------------------------------------------------------------------------
    // Now we output literal, uncompressed, pixel data codes.
    //-----------------------------------------------------------------------------

    //-- Get number of bits in each code
    code_bits = ct_size + 1;
    if ( code_bits < 2 ) code_bits = 2;

    //-- Calculate the reset code (256 for 8 bits, 64 for 6).
    reset_code = 1 << code_bits;

    //-- Calculate the end code (257 for 8 bits, 65 for 6).
    end_code = reset_code + 1;

    //-- Calculate a mask to extract pixel bits (255 for 8 bits, 63 for 6).
    pixel_mask = reset_code - 1;

    //-- Calculate the limit on number of codes (254 for 8 bits, 62 for 6).
    code_limit = pixel_mask - 1;

    //-- Setup pointers for indexing the image data.
    row_ptr = image->data_index;
    pix_ptr = row_ptr;
    img_end = row_ptr + ( image->image_width * image->image_height );
    row_end = row_ptr + image->image_width;

    //-- Start the interlacing index if interlacing is enabled.
    interlace = image->interlace ? 4 : 0;

    //-- Initialize the code counter to force an initial clear code.
    code_count = 1;

    //-- Output the minimum code length.
    buffer[0] = code_bits;
    angif_write( stream, buffer, 1 );

    //-- Need an extra bit for reset code and end code.
    ++ code_bits;

    //-- Ready the data accumulator empty.
    acc_data = 0;
    acc_bits = 0;

    //-- Initialize the output buffer to empty, leaving space for the length code.
    buf_ptr = buffer + 1;
    buf_end = buffer + 256;


    //-----------------------------------------------------------------------------
    // Do main loop through all pixels and rows.
    //-----------------------------------------------------------------------------
    for (;;) {
	//-- Count codes and check for a needed reset.
	if ( (-- code_count) == 0 ) {
	    //-- Append reset code to accumulator.
	    acc_data |= reset_code << acc_bits;
	    acc_bits += code_bits;
	    code_count = code_limit;
	}

	//-- Append current pixel into accumulator.
	acc_data |= ( pixel_mask & (unsigned long) (* pix_ptr) ) << acc_bits;
	acc_bits += code_bits;

	//-- Flush accumulator of whole octets.
	while ( acc_bits >= 8 ) {
	    //-- Store one octet from acculator and remove it.
	    * (buf_ptr ++) = acc_data;
	    acc_data >>= 8;
	    acc_bits -= 8;

	    //-- If buffer is full, output it.
	    if ( buf_ptr == buf_end ) {
		* buffer = (unsigned int) 255;
		if ( (rc = angif_write( stream, buffer, (size_t) 256 )) < 0 ) return rc;
		buf_ptr = buffer + 1;
	    }
	}

	//-- Check for end of row.
	if ( (++ pix_ptr) == row_end ) {
	    //-- Get next row, considering possible interlacing.
	    row_ptr += image->image_width * interlace_table[ 1 + interlace ];

	    //-- Check for end of image.
	    if ( row_ptr >= img_end ) {
		//-- If no interlacing or end of interlacing, break out.
		if ( interlace < 2 ) break;

		//-- Restart image with different interlacing.
		-- interlace;
		row_ptr = image->data_index + interlace_table[ interlace ];
	    }

	    //-- Recalculate end of row pointer.
	    row_end = row_ptr + image->image_width;
	}
    }


    //-----------------------------------------------------------------------------
    // Finish with ending code and ending segment.
    //-----------------------------------------------------------------------------

    //-- Count codes and check for a needed reset.
    if ( (-- code_count) == 0 ) {
	//-- Append reset code to accumulator.
	acc_data |= reset_code << acc_bits;
	acc_bits += code_bits;
	code_count = code_limit;
    }

    //-- Append the end code into accumulator.
    acc_data |= end_code << acc_bits;
    acc_bits += code_bits;

    //-- Pad accumulator to get the last octet out.
    acc_bits += 7;

    //-- Flush accumulator of whole octets.
    while ( acc_bits >= 8 ) {
	//-- Store one octet from acculator and remove it.
	* (buf_ptr ++) = acc_data;
	acc_data >>= 8;
	acc_bits -= 8;

	//-- If buffer is full, output it.
	if ( buf_ptr == buf_end ) {
	    * buffer = (unsigned int) 255;
	    if ( (rc = angif_write( stream, buffer, (size_t) 256 )) < 0 ) return rc;
	    buf_ptr = buffer + 1;
	}
    }

    //-- If buffer has any data, finish it.
    if ( ( buf_ptr - buffer ) > 1 ) {
	* buffer = (unsigned int) ( ( buf_ptr - buffer ) - 1 );
	if ( (rc = angif_write( stream, buffer, (size_t) ( buf_ptr - buffer ) )) < 0 ) return rc;
    }

    //-- End the image data with a null data segment.
    buffer[0] = 0;
    if ( (rc = angif_write( stream, buffer, (size_t) 1 )) < 0 ) return rc;

    //-- Return with successful completion.
    return 0;
}
