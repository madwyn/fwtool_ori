//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2013, nex-hack project
//
// This file "endian.h" is part of fwtool (http://www.nex-hack.info)
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

#ifndef ENDIAN_H
#define ENDIAN_H

#ifndef CONFIG_H
	#include "config.h"
#endif

// big-/little-endian helpers
extern	u16	readBE16(u8 *ptr);
extern	u32	readBE32(u8 *ptr);
extern	u16	readLE16(u8 *ptr);
extern	u32	readLE32(u8 *ptr);

extern	void	BE16toHost(u8 *ptr);
extern	void	BE32toHost(u8 *ptr);
extern	void	LE16toHost(u8 *ptr);
extern	void	LE32toHost(u8 *ptr);

#endif // ENDIAN_H

