//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2013, nex-hack project
//
// This file "fdat_image.h" is part of fwtool (http://www.nex-hack.info)
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

#ifndef FDAT_IMAGE_H
#define FDAT_IMAGE_H

#define	FDAT_EXTRACT_IOBUF_SIZE		1048576

#define	FDAT_IMAGE_MAGIC		"UDTRFIRM"
#define	FDAT_IMAGE_MAGIC_LEN	(sizeof(FDAT_IMAGE_MAGIC)-1)
#define	FDAT_IMAGE_MAGIC_OFS	0

#define	MAX_FDAT_FS_IMAGES		28
#define	FDAT_HDR_VERSION_LEN	4

typedef struct tagFDAT_ENC_BLOCK_HDR {
	unsigned short	hdr_csum;
	unsigned short	hdr_len_and_flags;
} FDAT_ENC_BLOCK_HDR;

typedef struct tagFDAT_FS_IMAGE_DESC {
	unsigned char	ffid_ident;									// 0000
	unsigned char	ffid_unknown_01[3];							// 0001
	unsigned int	ffid_fs_offset;								// 0004 - offset to filesystem image
	unsigned int	ffid_fs_length;								// 0008 - length of filesystem image
	unsigned int	ffid_unknown_02;							// 000c
} FDAT_FS_IMAGE_DESC;

typedef struct tagFDAT_IMAGE_HEADER {
	unsigned char	fih_magic[FDAT_IMAGE_MAGIC_LEN];			// 0000
	unsigned int	fih_header_crc;								// 0008 - CRC32 of remainder of header (000c-01ff)
	unsigned char	fih_image_version[FDAT_HDR_VERSION_LEN];	// 000c - "0100" - I assume this is a version string
	unsigned char	fih_fw_mode_type;							// 0010 - ModeType: "P" (??? "U" in all fw ?)
	unsigned char	fih_fw_mode_type_unknown_pad[3];			// 0011
	unsigned char	fih_fw_luw_flag;							// 0014 - LUW flag: "N"
	unsigned char	fih_fw_luw_flag_unknown_pad[3];				// 0015
	unsigned int	fih_unknown_01;								// 0018 - unknown
	unsigned int	fih_unknown_02;								// 001c - unknown
	unsigned char	fih_version_minor;							// 0020 - minor version: FF is sign-extended on read
	unsigned char	fih_version_major;							// 0021 - major version: FF is sign-extended on read
	unsigned char	fih_version_unknown_pad[2];					// 0022
	unsigned int	fih_model;									// 0024 - body or lens model (see table)
	unsigned int	fih_unknown_03;								// 0028 - unknown u32
	unsigned int	fih_unknown_04;								// 002c
	unsigned int	fih_fw_offset;								// 0030 - offset to firmware image (.tar)
	unsigned int	fih_fw_length;								// 0034 - length of firmware image (.tar)
	unsigned int	fih_fs_image_count;							// 0038 - number of fs images starting at 0040
	unsigned int	fih_unknown_05;								// 003c

	FDAT_FS_IMAGE_DESC	fih_fs_image_info[MAX_FDAT_FS_IMAGES];

} FDAT_IMAGE_HEADER;


extern	int	fdat_extract_firmware_image(const char *fname_fdat, const char *fname_fw_image);	// the ".tar" file inside the FDAT image

extern	int	fdat_fs_image_count(const char *fname_fdat);
extern	int	fdat_fs_image_length(const char *fname_fdat, int image_idx);
extern	int	fdat_extract_fs_image(const char *fname_fdat, int image_idx, const char *fname_fs_image);
extern  int	fdat_header_tofile(const char *fname_fdat, const char *fname_fdat_header);
extern  int fdat_repack(const char *fname_fdat, const char *fname_fdat_repack, const char *dirname_parts);

// FDAT header model type (0xffffffff - value not known to date) // added by kenan
enum fih_model_type {
	fih_unknown		=0x00000000,
	fih_dslra560	=0xffffffff,
	fih_dslra580	=0xffffffff,
	fih_slta33		=0x10300202,	// 1.gen
	fih_slta35		=0xffffffff,	// 1.gen
	fih_slta37		=0x21030008,
	fih_slta55		=0x10300203,	// 1.gen
	fih_slta57		=0x21030006,
	fih_slta58		=0xffffffff,
	fih_slta65		=0x12030012,
	fih_slta77		=0x12030011,
	fih_slta99		=0x22030010,
	fih_nex3		=0x10300206,	// 1.gen
	fih_nexc3		=0x11300223,	// 1.gen
	fih_nexf3		=0x21030010,
	fih_nex3n		=0xffffffff,
	fih_nex5		=0x10300205,	// 1.gen
	fih_nex5n		=0x12030023,
	fih_nex5r		=0x22030014,
	fih_nex6		=0x22030018,
	fih_nex7		=0x12030024,
	fih_nexvg10		=0x10300211,	// 1.gen
	fih_nexvg20		=0xffffffff,
	fih_nexvg30		=0xffffffff,
	fih_nexvg900	=0xffffffff,
	fih_nexea50		=0xffffffff,
	fih_nexfs100	=0x11010009,
	fih_nexfs700	=0xffffffff,
	fih_sel16f28	=0xffffffff,
	fih_sel20f28	=0xffffffff,
	fih_sel24f18	=0x11A08015,
	fih_sel30m35	=0x11A08014,
	fih_sel50f18	=0x11A08016,
	fih_sel1650pz	=0xffffffff,
	fih_sel1855		=0x10A08011,
	fih_sel18200	=0x10A08013,
	fih_sel18200le	=0xffffffff,
	fih_sel18200pz	=0xffffffff,
	fih_sel55210	=0x11A08012,
	fih_laea1		=0x10A00001,	// 1.gen
	fih_laea2		=0xffffffff,
	fih_laea3		=0xffffffff,
	fih_dscrx100	=0xffffffff,
	fih_dscrx1		=0xffffffff
};

#endif // FDAT_IMAGE_H
