//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2014, nex-hack project
//
// This file "endian.c" is part of fwtool (http://www.nex-hack.info)
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

#include "config.h"
#include "_endian.h"

// read a big-endian 16-bit value from buffer
uint16_t
readBE16(uint8_t *ptr)
{
	return (((uint16_t)(ptr[0]) << 8) | ptr[1]);
}

// read a big-endian 32-bit value from buffer
uint32_t
readBE32(uint8_t *ptr)
{
	return (((uint32_t)readBE16(&ptr[0]) << 16) | readBE16(&ptr[2]));
}

// read a little-endian 16-bit value from buffer
uint16_t
readLE16(uint8_t *ptr)
{
	return (((uint16_t)(ptr[1]) << 8) | ptr[0]);
}

// read a little-endian 32-bit value from buffer
uint32_t
readLE32(uint8_t *ptr)
{
	return (((uint32_t)readLE16(&ptr[2]) << 16) | readLE16(&ptr[0]));
}

void
BE16toHost(uint8_t *ptr)
{
	uint16_t	val;

	val = readBE16(ptr);
	*(uint16_t *)ptr = val;
}

void
BE32toHost(uint8_t *ptr)
{
	uint32_t	val;

	val = readBE32(ptr);
	*(uint32_t *)ptr = val;
}

void
LE16toHost(uint8_t *ptr)
{
	uint16_t	val;

	val = readLE16(ptr);
	*(uint16_t *)ptr = val;
}

void
LE32toHost(uint8_t *ptr)
{
	uint32_t	val;

	val = readLE32(ptr);
	*(uint32_t *)ptr = val;
}

/*
void
HosttoBE16 (uint16_t val, uint8_t *ptr)
{
	ptr[1] = (val & 0x00ff);
	ptr[0] = (val & 0xff00) >> 8;
}

void
HosttoBE32 (uint32_t val, uint8_t *ptr)
{
	ptr[3] = (val & 0xff);
	ptr[2] = (val & 0xff00) >> 8;
	ptr[1] = (val & 0xff0000) >> 16;
	ptr[0] = (val & 0xff000000) >> 24;
}

void
HosttoLE16(uint16_t val, uint8_t *ptr)
{
	ptr[0] = (val & 0x00ff);
	ptr[1] = (val & 0xff00) >> 8;
}

void
HosttoLE32(uint32_t val, uint8_t *ptr)
{
	ptr[0] = (uint8_t)((val & 0x000000ffL));
	ptr[1] = (uint8_t)((val & 0x0000ff00L) >> 8);
	ptr[2] = (uint8_t)((val & 0x00ff0000L) >> 16);
	ptr[3] = (uint8_t)((val & 0xff000000L) >> 24);
}
*/
