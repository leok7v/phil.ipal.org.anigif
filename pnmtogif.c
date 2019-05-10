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
// version	0.9.1
// homepage	http://phil.ipal.org/freeware/angif
//-----------------------------------------------------------------------------
// author	Philip Howard
// email	phil@ipal.org
// homepage	http://phil.ipal.org/
//-----------------------------------------------------------------------------

#include <ctype.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pnm.h>

#include "angif.h"

#define MAX_WIDTH	32766
#define MAX_HEIGHT	32766


//-----------------------------------------------------------------------------
// program	pnmtogif
//
// purpose	Convert one or more Portable Pix Map image files to GIF with
//		support for animation and true-color.
//
// command	pnmtogif { [options] [filename] ... } > filename.gif
//
// options	-d nn.nn	delay time seconds, 0.01 resolution.
//		-t 0xRRGGBB	color code to use for transparent.
//
// note		To avoid a true-color GIF, quantize the input images to
//		256 or fewer colors.
//-----------------------------------------------------------------------------
int
main (
	int		argc
,
	char * *	argv
)
{

    struct file_info {
	const char *	filename	;
	long		trans		;// color code for transparent
	int		width		;
	int		height		;
	int		delay		;// in centiseconds
    };
    struct file_info *	fi_list		;
    struct file_info *	fi_ptr		;
    struct file_info *	fi_end		;

    FILE *		in_file		;
    FILE *		out_file	;

    angif_header	gif_header	;
    angif_stream	gif_stream	;
    angif_image		gif_image	;

    xel *		xel_row		;

    long *		pix_array	;
    long *		pix_stdin	;
    long *		gif_pix_p	;

    char * *		scanv		;
    char *		out_filename	;

    xel			pnm_pixel	;
    long		gif_pixel	;

    xelval		file_maxval	;
    xelval		stdin_maxval	;
    int			maxval_plus	;

    int			scanc		;
    int			abort		;
    int			cols		;
    int			rows		;
    int			width		;
    int			height		;
    int			delay_cs	;
    int			file_format	;
    int			stdin_format	;
    int			count		;

    int			x		;
    int			y		;


    //-- Initialize defaults.
    delay_cs = 200;
    abort = 0;
    width = -1;
    height = -1;
    out_filename = NULL;
    stdin_format = -1;
    count = 0;

    //-- Allocate list of files at least as big as we need.
    fi_list = malloc( sizeof (struct file_info) * argc );
    if ( ! fi_list ) {
	fprintf( stderr, "Out of memory\n" );
	return 1;
    }

    //-- Scan arguments for files.
    scanc = argc;
    scanv = argv;

    fi_ptr = fi_list;
    for (;;) {

	//-- Process next argument.
	if ( -- scanc < 1 || ! * ++ scanv ) break;

	//-- Check for -o option.
	if ( strcmp( * scanv, "-o" ) == 0 ) {
	    if ( -- scanc < 1 || ! * ++ scanv ) break;
	    if ( out_filename && strcmp( * scanv, out_filename ) != 0 ) {
		fprintf( stderr, "Redundant output file name \"%s\"\n", * scanv );
		++ abort;
		continue;
	    }
	    out_filename = * scanv;
	    continue;
	}

	//-- Check for -d option.
	if ( strcmp( * scanv, "-d" ) == 0 ) {
	    double num;
	    if ( -- scanc < 1 || ! * ++ scanv ) break;
	    num = strtod( * scanv, NULL );
	    delay_cs = (int) ( num * 100.0 );
	    fi_ptr->delay = delay_cs;
	    continue;
	}

	//-- Store the previous delay setting.
	fi_ptr->delay = delay_cs;

	//-- If stdin specified, handle differently since we can read it only once.
	if ( strcmp( * scanv, "-" ) == 0 || strcmp( * scanv, "/dev/stdin" ) == 0 ) {
	    if ( stdin_format == -1 ) {
		pnm_readpnminit( stdin, & cols, & rows, & stdin_maxval, & stdin_format );
		if ( ( width != -1 && width != cols ) || ( height != -1 && height != rows ) ) {
		    fprintf( stderr,
			     "File \"%s\" geometry (%dx%d) not the same as previous (%dx%d)\n",
			     * scanv, cols, rows, width, height
		    );
		    ++ abort;
		    continue;
		}
		width = cols;
		height = rows;
	    }
	    fi_ptr->filename = "-";
	    ++ fi_ptr;
	    ++ count;
	    continue;
	}


	//-- Open the file.
	in_file = fopen( * scanv, "rb" );
	if ( ! in_file ) {
	    fprintf( stderr, "File \"%s\" cannot be opened.\n", * scanv );
	    ++ abort;
	    continue;
	}

	//-- Read the header info.
	pnm_readpnminit( in_file, & cols, & rows, & file_maxval, & file_format );
	if ( ( width != -1 && width != cols ) || ( height != -1 && height != rows ) ) {
	    fprintf( stderr,
		     "File \"%s\" geometry (%dx%d) not the same as previous (%dx%d)\n",
		     * scanv, cols, rows, width, height
	    );
	    ++ abort;
	}
	width = cols;
	height = rows;

	//-- Store the file name.
	fi_ptr->filename = * scanv;
	++ fi_ptr;
	++ count;

	//-- Close it for now.
	fclose( in_file );

	continue;
    }

    //-- Check for abort.
    if ( abort ) {
	fprintf( stderr, "Aborting now\n" );
	return 1;
    }

    //-- Check if we got a file.
    if ( fi_ptr == fi_list ) {
	fi_ptr->filename = "-";
	fi_ptr->delay = delay_cs;
	pnm_readpnminit( stdin, & cols, & rows, & stdin_maxval, & stdin_format );
	width = cols;
	height = rows;
	++ fi_ptr;
    }
    fi_end = fi_ptr;

    //-- Allocate space to read a row of PNM xels.
    xel_row = pnm_allocrow( width );
    if ( ! xel_row ) {
	fprintf( stderr, "Out of memory by pnm_allocrow( %d )\n", cols );
	return 1;
    }

    //-- Setup output file.
    if ( out_filename ) {
	out_file = fopen( out_filename, "wb" );
    } else {
	out_file = stdout;
    }

    //-- Setup a stream for the GIF data.
    angif_init_stream( & gif_stream );
    angif_init_header( & gif_header );
    angif_init_image( & gif_image );
    angif_set_file( & gif_stream, stdout );

    //-- Initialize and output the GIF header.
    gif_header.width		= width;
    gif_header.height		= height;
    gif_header.color_res	= 7;
    angif_put_header( & gif_stream, & gif_header );

    //-- Fill in the image block.
    gif_image.disposal		= 1;
    gif_image.image_width	= width;
    gif_image.array_width	= width;
    gif_image.image_height	= height;
    gif_image.array_height	= height;

    //-- If more than one frame, do animation.
    if ( count > 1 ) {
	angif_put_animate( & gif_stream, 0 );
    }

    //-- Initialize this because gcc is bitchy.
    pix_stdin = NULL;

    //-- If stdin used, read it in first.
    if ( stdin_format != -1 ) {
	pix_stdin = malloc( sizeof (long) * width * height );
	if ( ! pix_stdin ) {
	    fprintf( stderr, "Out of memory\n" );
	    return 1;
	}
	gif_pix_p = pix_stdin;
	maxval_plus = stdin_maxval + 1;
	for ( y = 0; y < height; ++ y ) {
	    pnm_readpnmrow( stdin, xel_row, width, stdin_maxval, stdin_format );
	    for ( x = 0; x < width; ++ x ) {
		pnm_pixel = xel_row[ x ];
		if ( stdin_format == PPM_FORMAT || stdin_format == RPPM_FORMAT ) {
		    gif_pixel =
			( ( ( PPM_GETR( pnm_pixel ) << 8 ) / maxval_plus ) << 16 ) +
			( ( ( PPM_GETG( pnm_pixel ) << 8 ) / maxval_plus ) <<  8 ) +
			( ( ( PPM_GETB( pnm_pixel ) << 8 ) / maxval_plus )       );
		} else {
		    gif_pixel = ( PPM_GETR( pnm_pixel ) << 8 ) / maxval_plus;
		    gif_pixel = ( gif_pixel << 16 ) + ( gif_pixel << 8 ) + gif_pixel;
		}
		* gif_pix_p = gif_pixel;
		++ gif_pix_p;
	    }
	}
    }

    //-- Allocate the pixel array we use.
    pix_array = malloc( sizeof (long) * width * height );
    if ( ! pix_array ) {
	fprintf( stderr, "Out of memory\n" );
	return 1;
    }

    //-- Scan files in list.
    for ( fi_ptr = fi_list; fi_ptr < fi_end; ++ fi_ptr ) {

	//-- If it was stdin, use the stdin image, else read the image from the file.
	if ( strcmp( fi_ptr->filename, "-" ) == 0 ) {

	    //-- Point to the stdin array.
	    gif_image.data_rgb = pix_stdin;

	} else {

	    //-- Point to this array to use it.
	    gif_image.data_rgb = pix_array;

	    //-- Open the file.
	    in_file = fopen( fi_ptr->filename, "rb" );
	    if ( ! in_file ) {
		fprintf( stderr, "File \"%s\" cannot be opened.\n", fi_ptr->filename );
		continue;
	    }

	    //-- Read the header info again.
	    pnm_readpnminit( in_file, & cols, & rows, & file_maxval, & file_format );

	    //-- Convert pixels a row at a time into a large pix array.
	    gif_pix_p = pix_array;
	    maxval_plus = file_maxval + 1;
	    for ( y = 0; y < height; ++ y ) {
		pnm_readpnmrow( in_file, xel_row, width, file_maxval, file_format );
		for ( x = 0; x < width; ++ x ) {
		    pnm_pixel = xel_row[ x ];
		    if ( file_format == PPM_FORMAT || file_format == RPPM_FORMAT ) {
			gif_pixel =
			    ( ( ( PPM_GETR( pnm_pixel ) << 8 ) / maxval_plus ) << 16 ) +
			    ( ( ( PPM_GETG( pnm_pixel ) << 8 ) / maxval_plus ) <<  8 ) +
			    ( ( ( PPM_GETB( pnm_pixel ) << 8 ) / maxval_plus )       );
		    } else {
			gif_pixel = ( PNM_GET1( pnm_pixel ) << 8 ) / maxval_plus;
			gif_pixel = ( gif_pixel << 16 ) + ( gif_pixel << 8 ) + gif_pixel;
		    }
		    * gif_pix_p = gif_pixel;
		    ++ gif_pix_p;
		}
	    }

	    //-- Close the file.
	    fclose( in_file );
	}

	//-- Fill in the image block.
	gif_image.delay_time	= fi_ptr->delay;

	//-- Output the GIF stream.
	angif_put_rgb( & gif_stream, & gif_image );
    }

    //-- Finish the GIF stream properly.
    angif_put_trailer( & gif_stream );

    return 0;
}
