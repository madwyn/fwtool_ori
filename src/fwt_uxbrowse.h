//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2014, nex-hack project
//
// This file "fwt_uxbrowse.h" is part of fwtool (http://www.nex-hack.info)
//
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef FWT_UXBROWSE_H
#define FWT_UXBROWSE_H

#define	UX_MAGIC		"ux"
#define	UX_MAGIC_LEN	(sizeof(UX_MAGIC)-1)
#define	UX_MAGIC2		"*\007"
#define	UX_MAGIC2_LEN	(sizeof(UX_MAGIC)-1)

#define	UX_MODE_32B_PTR	0x09
#define	UX_MODE_16B_PTR	0x08

#define UX_PTRFLAG_MASK	0x1F
#define UX_FLAGPTR_MASK ~UX_PTRFLAG_MASK

// ux header struct
typedef struct tagUX_HEADER {
	unsigned char	ux_magic[UX_MAGIC_LEN];						// 0000 - ux
	unsigned char	ux_magic2[UX_MAGIC2_LEN];					// 0002 - a0x07, b0x07, c0x07, d0x07 ...
	unsigned int	ux_unknown_01;								// 0004 - 0x00000000 padding?
	unsigned char	ux_mode;									// 0008 - 0x08, 0x09
	unsigned char	ux_unknown_02;								// 0009 - 0x00 pad?
	unsigned char	ux_index;									// 000A -
	unsigned char	ux_unknown_03;								// 000B - 0x00 pad?
	unsigned char	ux_ptrlow_count;							// 000C - low byte of pointer counter
	unsigned char	ux_ptrflag_bitmap;							// 000D - bitmap: bit 0..4 high bits of ptr_count, bit 5..7 flags
	unsigned char	ux_flag2;									// 000E - flags ?
	unsigned char	ux_flag3;									// 000F - flags ?
} UX_HEADER;


extern	int	ux_read_file(const char *fname_uxf, const char *dirname);


#endif // FWT_UXBROWSE_H
