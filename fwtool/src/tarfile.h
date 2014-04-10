//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2014, nex-hack project
//
// This file "tarfile.h" is part of fwtool (http://www.nex-hack.info)
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

#ifndef TARFILE_H
#define TARFILE_H

#ifdef	TARFILE_C	// internal to tarfile.c
	#define	TAR_IOBUF_SIZE	16384

	typedef struct t_tar_handle_struct {
		struct archive *a;
	} tar_handle_struct;
#endif	// TARFILE_C


typedef struct t_tar_handle_struct *tar_handle;

// read
extern	int	is_tarfile(const char *fname);
extern	int	tarfile_extract_all(const char *fname_tar, const char *dirname_out, const char *fname_tar_attr);

// write
extern	int	tarfile_create_all(const char *fname_tar, const char *dirname_in);


#endif // TARFILE_H
