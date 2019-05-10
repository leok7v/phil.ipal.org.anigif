//-----------------------------------------------------------------------------
// program	rawtogif
//
// purpose	Convert a raw format to GIF
//
// command	rawtogif width height
//
// arguments	1	width in pixels
//		2	height in pixels
//
// stdout	the image file stream in gif format
//
// stderr	error messages
//
// status	0 OK
//		1 OOPS
//-----------------------------------------------------------------------------
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "angif.h"

#define MAX_WIDTH	32766
#define MAX_HEIGHT	32766

int
main (
	int		argc
,
	char * *	argv
,
	char * *	envp
)
{
    angif_header	gif_header	;
    angif_stream	gif_stream	;
    angif_image		gif_image	;

    long *		array		;
    long *		array_p		;

    long		nread		;
    long		pixels		;

    int			width		;
    int			height		;
    int			rc		;

    rc = 0;

    //-- Make sure we have enough arguments.
    if ( argc < 3 ) {
	fprintf(stderr,
		"\n"
		"rawtogif needs 2 arguments:\n"
		"rawtogif width height\n"
		"\n"
	);
	return 1;
    }

    //-- Get width.
    width = strtol( argv[1], NULL, 0 );
    if ( width < 1 || width > MAX_WIDTH ) {
	fprintf(stderr,"Invalid width\n");
	++ rc;
    }

    //-- Get height.
    height = strtol( argv[2], NULL, 0 );
    if ( height < 1 || height > MAX_HEIGHT ) {
	fprintf(stderr,"Invalid height\n");
	++ rc;
    }

    //-- Check for errors.
    if ( rc != 0 ) return 1;

    //-- Calculate number of pixels.
    pixels = width * height;

    //-- Allocate a buffer.
    array = malloc( sizeof (long) * pixels );
    if ( ! array ) {
	fprintf( stderr, "Out of memory\n" );
	return 1;
    }

    //-- Read in the raw data.
    nread = 0;
    array_p = array;
    for (;;) {
	long red;
	long green;
	long blue;
	if ( ( red   = getc( stdin ) ) == EOF ) break;
	if ( ( green = getc( stdin ) ) == EOF ) break;
	if ( ( blue  = getc( stdin ) ) == EOF ) break;
	getc( stdin );
	* array_p = ( red << 16 ) + ( green << 8 ) + blue;
	++ array_p;
	++ nread;
    }
    if ( nread != pixels ) {
	fprintf( stderr,
		 "EOF on input, expected %lu bytes, only got %lu\n",
		 pixels,
		 nread
	);
	return 1;
    }

    //-- Setup a stream for the GIF data.
    angif_init_stream( & gif_stream );
    angif_set_file( & gif_stream, stdout );

    //-- Initialize the GIF header.
    angif_init_header( & gif_header );
    gif_header.width		= width;
    gif_header.height		= height;
    gif_header.color_res	= 7;

    //-- Initialize the GIF image block.
    angif_init_image( & gif_image );
    gif_image.data_rgb		= array;
    gif_image.image_width	= width;
    gif_image.image_height	= height;
    gif_image.array_width	= width;
    gif_image.array_height	= height;

    //-- Output the GIF stream.
    angif_put_header( & gif_stream, & gif_header );
    angif_put_rgb( & gif_stream, & gif_image );
    angif_put_trailer( & gif_stream );

    //-- Get rid of the array.
    free( gif_image.data_rgb );

    return 0;
}
