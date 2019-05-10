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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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

//-----------------------------------------------------------------------------
// header	angif.h
//
// purpose	Declare and define resources for the ANGIF package.
//-----------------------------------------------------------------------------

#ifndef _ANGIF_H_
#define _ANGIF_H_


//-----------------------------------------------------------------------------
// struct	angif_stream
// purpose	Contain information that describes an output GIF stream to
//		which data is written.
//-----------------------------------------------------------------------------
struct angif_stream {

    //-----------------------------------------------------------------------------
    // If the GIF data is being written to a memory buffer,
    // these pointers will define the state of that buffer.
    //-----------------------------------------------------------------------------
    unsigned char *	memory_begin		;// beginning of allocated memory
    unsigned char *	memory_end		;// end of allocated memory
    unsigned char *	memory_pos		;// current position in memory

    //-----------------------------------------------------------------------------
    // If the GIF data is being given to a function call, then these pointers
    // define that function as well as a pointer argument to pass along.
    //-----------------------------------------------------------------------------
    int		(*	call_fun	)()	;// function to call with GIF data
    void *		call_arg		;// arbitrary void pointer to pass

    //-----------------------------------------------------------------------------
    // If the GIF data is being written to a stdio file,
    // then this file pointer is used.
    //-----------------------------------------------------------------------------
    FILE *		out_file		;// standard I/O file to write to

};
typedef struct angif_stream	angif_stream	;


//-----------------------------------------------------------------------------
// struct	angif_header
// purpose	Contain information that describes header values for the
//		output GIF stream.
//-----------------------------------------------------------------------------
struct angif_header {

    //-----------------------------------------------------------------------------
    // The global color table is optional.  Images must have either a global or
    // a local color table, else a local gray scale table will be substituted.
    //-----------------------------------------------------------------------------
    long *		color_table		;
    int			color_count		;

    //-----------------------------------------------------------------------------
    // Define the width and height of the entire (screen) image.
    //-----------------------------------------------------------------------------
    unsigned int	width			;
    unsigned int	height			;

    //-----------------------------------------------------------------------------
    // Define miscellaneous GIF info.
    //-----------------------------------------------------------------------------
    unsigned char	bgc_index		;
    unsigned char	aspect_ratio		;

    unsigned char	color_res		;// (0..7)
    unsigned char	ct_sort			;

};
typedef struct angif_header	angif_header	;


//-----------------------------------------------------------------------------
// struct	angif_image
// purpose	Contain information that describes an image to be converted
//		to a GIF stream.
//-----------------------------------------------------------------------------
struct angif_image {

    //-----------------------------------------------------------------------------
    // These three pointers will be filled in when the image is composed of
    // three separate arrays of floating point numbers that correspond to the
    // different color planes of the image.  The values range from 0.0 to 1.0
    // representing a linear intensity of that color.
    //-----------------------------------------------------------------------------
    float *		data_red		;
    float *		data_green		;
    float *		data_blue		;

    //-----------------------------------------------------------------------------
    // This pointer will be filled in when the image is composed of an array
    // of long values representing the gamma corrected RGB values for each
    // pixel.  The numeric format is 0x00RRGGBB.  A special value of -1
    // represents a transparent pixel.
    //-----------------------------------------------------------------------------
    long *		data_rgb		;

    //-----------------------------------------------------------------------------
    // This pointer will be filled in when the image is composed of integer
    // indexes into a color table, or if the table is absent, represents a
    // gray scale.  The values range from 0 to 255 and are octets.
    //-----------------------------------------------------------------------------
    unsigned char *	data_index		;

    //-----------------------------------------------------------------------------
    // This pointer will be filled in when the image is composed of data that
    // is already compressed using the LZW algorithm.  This function library
    // does not compress or uncompress using LZW.  This data would typically
    // originate from a GIF file that was read in.  The data is in the form
    // contained in the GIF image block, including the length prefix which is
    // found as the first octet of each segment.
    //-----------------------------------------------------------------------------
    unsigned char *	data_prelzw		;

    //-----------------------------------------------------------------------------
    // If the linear floating point image is present, then gamma correction
    // will be applied to convert the pixel values to coded integers to
    // compensate for the usual gamma characteristics of the CRT display.
    // The following values are used to apply this correction.	If no value
    // is provided, then the default value of 2.45 (or close to it) is used.
    //-----------------------------------------------------------------------------
    float		gamma_red		;
    float		gamma_green		;
    float		gamma_blue		;

    //-----------------------------------------------------------------------------
    // This pointer is filled in when a local color table is provided.	It is
    // used only if the data_index or data_prelzw pointers are given.  If it is
    // absent and there was no global color table in the header, then a gray
    // scale will be used in its place.	 The count of colors in the table is
    // required if the table is present.
    //-----------------------------------------------------------------------------
    long *		color_table		;
    int			color_count		;

    //-----------------------------------------------------------------------------
    // These values represent the actual size of the image.
    //-----------------------------------------------------------------------------
    unsigned int	image_width		;
    unsigned int	image_height		;

    //-----------------------------------------------------------------------------
    // These values represent where this image is placed within the larger
    // GIF image collage that multiple image blocks can construct.
    //-----------------------------------------------------------------------------
    unsigned int	image_left		;
    unsigned int	image_top		;

    //-----------------------------------------------------------------------------
    // These values describe the geometry of the image array in pixels.
    //-----------------------------------------------------------------------------
    unsigned int	array_width		;
    unsigned int	array_height		;

    //-----------------------------------------------------------------------------
    // These values represent the pixel offset within the image array where
    // the image to be converted to GIF is obtained.
    //-----------------------------------------------------------------------------
    unsigned int	array_left		;
    unsigned int	array_top		;

    //-----------------------------------------------------------------------------
    // Define other miscellaneous GIF attributes.
    //-----------------------------------------------------------------------------
    unsigned int	delay_time		;// delay in centiseconds

    unsigned char	interlace		;// interlace mode flag
    unsigned char	ct_sort			;// color table is sorted
    unsigned char	disposal		;// disposal method (usually 1)
    unsigned char	user_input		;// user input request flag
    unsigned char	trans_flag		;// one color is transparent
    unsigned char	trans_index		;// index to transparent color

};
typedef struct angif_image	angif_image	;


//-----------------------------------------------------------------------------
// struct	angif_text
// purpose	Contain information that describes text data to be converted
//		to a GIF stream.  Text information is data intended to be
//		displayed as part of the GIF image.
//-----------------------------------------------------------------------------
struct angif_text {

    //-----------------------------------------------------------------------------
    // This pointer and associated length reference the text data to be
    // converted into a GIF text block.	 If the length is negative, then
    // the length will be determined using strlen().
    //-----------------------------------------------------------------------------
    const unsigned char *	text_data	;// ascii text string
    long			text_len	;// length of text

    //-----------------------------------------------------------------------------
    // These values represent where this text is placed within the larger
    // GIF image collage that multiple image blocks can construct.
    //-----------------------------------------------------------------------------
    unsigned int	pos_left		;
    unsigned int	pos_top			;

    //-----------------------------------------------------------------------------
    // These values represent the actual size of the text to be displayed.
    //-----------------------------------------------------------------------------
    unsigned int	text_width		;
    unsigned int	text_height		;

    //-----------------------------------------------------------------------------
    // These values describe the geometry of the text array in cells.
    //-----------------------------------------------------------------------------
    unsigned int	array_width		;
    unsigned int	array_height		;

    //-----------------------------------------------------------------------------
    // These values represent the cell offset within the text array where
    // the text to be converted to GIF is obtained.
    //-----------------------------------------------------------------------------
    unsigned int	offset_left		;
    unsigned int	offset_top		;

    //-----------------------------------------------------------------------------
    // These values represent the size of a text cell in pixels.
    //-----------------------------------------------------------------------------
    unsigned char	cell_width		;
    unsigned char	cell_height		;

    //-----------------------------------------------------------------------------
    // These values specify the foreground and background colors for this
    // text block.  They are indexes in the color table.
    //-----------------------------------------------------------------------------
    unsigned char	fg_color		;
    unsigned char	bg_color		;

};
typedef struct angif_text	angif_text	;


//-----------------------------------------------------------------------------
// Define return code error symbols for those functions which return
// an integer code.  The value zero is always used for general success.
// Specialized success code will be positive.  Errors will be negative.
//-----------------------------------------------------------------------------
#define ANGIF_NO_ERROR		0
#define ANGIF_SUCCESS		0

#define ANGIF_ERROR_ALLOC	-2
#define ANGIF_ERROR_ARG_COMMENT	-3
#define ANGIF_ERROR_ARG_DATA	-4
#define ANGIF_ERROR_ARG_HEADER	-5
#define ANGIF_ERROR_ARG_IMAGE	-6
#define ANGIF_ERROR_ARG_LEN	-7
#define ANGIF_ERROR_ARG_STREAM	-8
#define ANGIF_ERROR_ARG_TEXT	-9
#define ANGIF_ERROR_COLOR_COUNT	-10
#define ANGIF_ERROR_CT_SIZE	-11
#define ANGIF_ERROR_DATA	-12
#define ANGIF_ERROR_FREE	-13
#define ANGIF_ERROR_FWRITE	-14
#define ANGIF_ERROR_GEOMETRY	-15
#define ANGIF_ERROR_LOOP_COUNT	-16
#define ANGIF_ERROR_MEMORY	-17
#define ANGIF_ERROR_NO_DATA	-18
#define ANGIF_ERROR_NO_INDEX	-19
#define ANGIF_ERROR_NO_LZW	-20
#define ANGIF_ERROR_NO_MEMORY	-21
#define ANGIF_ERROR_TEXT	-22

#define ANGIF_ERRNO_LOGIC	1


//-----------------------------------------------------------------------------
// Declare functions and function-like macros in the angif library.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// function	angif_init_header
//
// purpose	Initialize the header struct.
//
// arguments	1 (angif_header *) pointer to header struct to initialize.
//
// returns	(angif_header *) pointer to header struct
//		(angif_header *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_header() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_header *
angif_init_header (
	angif_header *	header
);

//-----------------------------------------------------------------------------
// macro	angif_new_header
//
// purpose	Allocate a new header struct.
//
// arguments	-none-
//
// returns	(angif_header *) pointer to header struct
//		(angif_header *) NULL if allocate failed.
//-----------------------------------------------------------------------------
#define angif_new_header()	angif_init_header(NULL)

//-----------------------------------------------------------------------------
// function	angif_init_image
//
// purpose	Initialize an image info struct.
//
// arguments	1 (angif_image *) pointer to image info struct
//
// returns	(angif_image *) pointer to image info struct
//		(angif_image *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_image() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_image *
angif_init_image (
	angif_image *	image
);

//-----------------------------------------------------------------------------
// macro	angif_new_image
//
// purpose	Allocate a new image info struct.
//
// arguments	-none-
//
// returns	(angif_image *) pointer to image info struct
//		(angif_image *) NULL if allocate failed.
//-----------------------------------------------------------------------------
#define angif_new_image()	angif_init_image(NULL)

//-----------------------------------------------------------------------------
// function	angif_init_stream
//
// purpose	Initialize a stream object to which a GIF file will be written.
//
// arguments	1 (angif_stream *) pointer to stream object to initialize.
//
// returns	(angif_stream *) pointer to stream object struct
//		(angif_stream *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_stream() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_stream *
angif_init_stream (
	angif_stream *	stream
);

//-----------------------------------------------------------------------------
// function	angif_new_stream
//
// purpose	Allocate a new stream object to which a GIF file will be written.
//
// arguments	1 (angif_stream *) pointer to stream object to initialize.
//
// returns	(angif_stream *) pointer to stream object struct
//		(angif_stream *) NULL if allocate failed.
//-----------------------------------------------------------------------------
#define angif_new_stream()	angif_init_stream(NULL)

//-----------------------------------------------------------------------------
// function	angif_init_text
//
// purpose	Initialize a text info struct.
//
// arguments	1 (angif_text *) pointer to text info struct
//
// returns	(angif_text *) pointer to text info struct
//		(angif_text *) NULL if no struct given and allocate failed.
//
// warning	Calling angif_init_text() with a struct that contains
//		pointers to allocated memory could result in that memory
//		being lost (memory leak).
//-----------------------------------------------------------------------------
angif_text *
angif_init_text (
	angif_text *	text
);

//-----------------------------------------------------------------------------
// function	angif_new_text
//
// purpose	Allocate a new text info struct.
//
// arguments	1 (angif_text *) pointer to text info struct
//
// returns	(angif_text *) pointer to text info struct
//		(angif_text *) NULL if allocate failed.
//-----------------------------------------------------------------------------
#define angif_new_text()	angif_init_text(NULL)

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
);

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
);

//-----------------------------------------------------------------------------
// function	angif_put_header
//
// purpose	Output a GIF header to a GIF stream target.
//		The logical image descriptor is also included.
//
// arguments	1 (angif_stream *) GIF stream to write to
//		2 (angif_header *) GIF header data to be written
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_header (
	angif_stream *		stream
,
	angif_header *		header
);

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
);

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
);

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
);

//-----------------------------------------------------------------------------
// function	angif_put_prelzw
//
// purpose	Output pre-compressed LZW data to a stream target.
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
angif_put_prelzw (
	angif_stream *	stream
,
	angif_image *	image
);

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
);

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
);

//-----------------------------------------------------------------------------
// function	angif_put_trailer
//
// purpose	Output a GIF trailer block to a stream target.
//
// arguments	1 (angif_stream *) pointer to stream target.
//
// returns	(int) 0 successful completion
//		(int) <0 error indicated by return value and errno
//-----------------------------------------------------------------------------
int
angif_put_trailer (
	angif_stream *	stream
);

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
);

//-----------------------------------------------------------------------------
// function	angif_unset_file
//
// purpose	Unset a stdio FILE in a stream.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
#define angif_unset_file(s) angif_set_file((s),NULL)

//-----------------------------------------------------------------------------
// function	angif_set_function
//
// purpose	Setup a stream for output to a called function.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//		2 (int (*fun)()) address of function to call or NULL to stop
//		3 (void *) arbitrary argument to pass along to the function
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_set_function (
	angif_stream *	stream
,
	int		(*fun)()
,
	void *		arg
);

//-----------------------------------------------------------------------------
// macro	angif_unset_function
//
// purpose	Unset called function in a stream.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
#define angif_unset_function(s) angif_set_function((s),NULL,NULL)

//-----------------------------------------------------------------------------
// function	angif_set_memory
//
// purpose	Setup a stream for memory writing.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//		2 (size_t) initial size to use or 0 to cancel
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_set_memory (
	angif_stream *	stream
,
	size_t		size
);

//-----------------------------------------------------------------------------
// macro	angif_unset_memory
//
// purpose	Unset memory writing in a stream.
//
// arguments	1 (angif_stream *) pointer to stream to setup
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
#define angif_unset_memory(s) angif_set_memory((s),0)

//-----------------------------------------------------------------------------
// function	angif_take_memory
//
// purpose	Take the allocated memory from the stream object and give
//		it to the calling program.  The memory is no longer part
//		of the stream object.  The caller must free() it when done.
//		Call angif_set_memory() to establish a replacement if more
//		GIF writing to memory is desired for the same stream.
//
// arguments	1 (angif_stream *) pointer to stream to getup
//		2 (unsigned char * *) where to store memory pointer
//		3 (size_t *) where to store length of actual GIF data
//		4 (size_t *) where to store allocated size of buffer
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_take_memory (
	angif_stream *		stream
,
	unsigned char * *	memory_p
,
	size_t *		len_p
,
	size_t *		size_p
);

//-----------------------------------------------------------------------------
// function	angif_write
//
// purpose	Write data to a target stream object.
//
// arguments	1 (angif_stream *) pointer to stream object
//		2 (const unsigned char *) address of data to write
//		3 (size_t) length of data to write
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
int
angif_write (
	angif_stream *		stream
,
	const unsigned char *	data
,
	size_t			len
);

//-----------------------------------------------------------------------------
// function	angif_flush
//
// purpose	Flush data buffered in a target stream object.
//
// arguments	1 (angif_stream *) pointer to stream object
//
// returns	(int) 0 = successful
//		(int) -1 = failure
//-----------------------------------------------------------------------------
#define angif_flush(s) angif_write((s),NULL,0)

//-----------------------------------------------------------------------------
// End of angif.h header.
//-----------------------------------------------------------------------------
#endif /* _ANGIF_H_ */
