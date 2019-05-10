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
// homepage	http://phil.ipal.org/freeware/angif/
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

#ifndef __ANGIF_LIB_H__
#define __ANGIF_LIB_H__

//-----------------------------------------------------------------------------
// Include other headers needed for the library.
//-----------------------------------------------------------------------------
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Include our own header.
//-----------------------------------------------------------------------------
#include "angif.h"

//-----------------------------------------------------------------------------
// Define general configuration parameters.
//
// ANGIF_DEFAULT_GAMMA is the default gamma correction value.
// ANGIF_COLOR_MODULUS is used for hashing the color table lookups.
//-----------------------------------------------------------------------------
#define ANGIF_DEFAULT_GAMMA		2.45
#define ANGIF_COLOR_MODULUS		1999
#define ANGIF_MINIMUM_MEMORY_SIZE	256
#define ANGIF_INITAL_MEMORY_SIZE	256

#endif /* __ANGIF_LIB_H__ */
