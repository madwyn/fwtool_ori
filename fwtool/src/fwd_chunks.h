//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2013, nex-hack project
//
// This file "fwd_chunks.h" is part of fwtool (http://www.nex-hack.info)
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

#ifndef FWD_CHUNKS_H
#define FWD_CHUNKS_H

//#define	FWDATA_REPACK_IOBUF_SIZE		1048576
#define	FWDATA_MAXHEADLEN	16384

extern	int	fwdata_unpack_chunks(const char *fname_fwdata_in, const char *dirname_out);
extern	int	fwdata_extract_chunk_to_file(const char *fname_fwdata_in, const char *chunk_id, const char *fname_out);

extern	int	fwdata_repack_chunks(const char *fname_fwdata_in, const char *dirname_out);

#endif // FWD_CHUNKS_H

