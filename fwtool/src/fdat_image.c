//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2013, nex-hack project
//
// This file "fdat_image.c" is part of fwtool (http://www.nex-hack.info)
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
#include "fwt_util.h"

#include "endian.h"
#include "csum.h"
#include "fdat_cipher.h"

#include "fdat_image.h"


int
fdat_read_image_header(const char *fname_fdat, FDAT_IMAGE_HEADER *p_image_hdr)
{
	FILE	*fh;
	FDAT_IMAGE_HEADER	l_hdr;
	int	i;
	FDAT_FS_IMAGE_DESC	*p_desc;

	if (!(fh = fopen(fname_fdat, "rb"))) {
		fprintf(stderr, "Failed to open FDAT image '%s'!\n", fname_fdat);
		return 1;
	}
	if (fread((void *)&l_hdr, 1, sizeof(l_hdr), fh) != sizeof(l_hdr)) {
			fprintf(stderr, "Error reading FDAT image header!\n");
			fclose(fh);
			return 1;
	}
	fclose(fh);
	if (memcmp(l_hdr.fih_magic, FDAT_IMAGE_MAGIC, sizeof(l_hdr.fih_magic))) {
			fprintf(stderr, "Invalid FDAT image header!\n");
			return 1;
	}
	// endian-convert header

	LE32toHost((u8 *)&l_hdr.fih_header_crc);
	LE32toHost((u8 *)&l_hdr.fih_fw_offset);
	LE32toHost((u8 *)&l_hdr.fih_fw_length);
	LE32toHost((u8 *)&l_hdr.fih_fs_image_count);

	for (i = 0; i < l_hdr.fih_fs_image_count; i++) {
		p_desc = &l_hdr.fih_fs_image_info[i];
		LE32toHost((u8 *)&p_desc->ffid_fs_length);
		LE32toHost((u8 *)&p_desc->ffid_fs_offset);
	}

	memcpy((void *)p_image_hdr, (void *)&l_hdr, sizeof(l_hdr));

	return 0;
}


static int
_fdat_extract_image(const char *fname_in, unsigned int f_offset, unsigned int f_len, const char *fname_out)
{
	int	ret;
	FILE	*fh_in = NULL, *fh_out = NULL;
	unsigned char	*p_iobuf = NULL;
	unsigned int	this_nbytes;

	if (!(fh_in = fopen(fname_in, "rb"))) {
		goto exit_err;
	}
	fseek(fh_in, f_offset, SEEK_SET);
	if (!(fh_out = fopen(fname_out, "wb"))) {
		goto exit_err;
	}
	if (!(p_iobuf = malloc(FDAT_EXTRACT_IOBUF_SIZE))) {
		goto exit_err;
	}
	while(f_len) {
		if (f_len > FDAT_EXTRACT_IOBUF_SIZE) {
			this_nbytes = FDAT_EXTRACT_IOBUF_SIZE;
		} else {
			this_nbytes = f_len;
		}
		if (fread(p_iobuf, 1, this_nbytes, fh_in) != this_nbytes) {
			goto exit_err;
		}
		if (fwrite(p_iobuf, 1, this_nbytes, fh_out) != this_nbytes) {
			goto exit_err;
		}

		f_len -= this_nbytes;
	}
	fclose(fh_in);
	fclose(fh_out);
	goto exit_ok;

exit_err:
	ret = 1;
	if (fh_in) fclose(fh_out);
	if (fh_out) fclose(fh_out);
	unlink(fname_out);
	goto exit_common;
exit_ok:
	ret = 0;
	// fallthru
exit_common:
	if (p_iobuf) free((void *)p_iobuf);
	return ret;
}


int
fdat_extract_firmware_image(const char *fname_fdat, const char *fname_fw_image)
{
	FDAT_IMAGE_HEADER	fdat_hdr;
	int	ret = 0;

	if (fdat_read_image_header(fname_fdat, &fdat_hdr)) {
		goto exit_err;
	}

	if (!fdat_hdr.fih_fw_length || (_fdat_extract_image(fname_fdat, fdat_hdr.fih_fw_offset, fdat_hdr.fih_fw_length, fname_fw_image) != 0)) {
		fprintf(stderr, "Error extracting firmware image from '%s'!\n", fname_fdat);
		goto exit_err;
	}
	goto exit_ok;

exit_err:
	ret = 1;
	unlink(fname_fw_image);
	goto exit_common;
exit_ok:
	ret = 0;
	// fallthru
exit_common:
	return ret;
}


int
fdat_fs_image_count(const char *fname_fdat)
{
	FDAT_IMAGE_HEADER	fdat_hdr;

	if (fdat_read_image_header(fname_fdat, &fdat_hdr)) {
		return -1;
	}
	return fdat_hdr.fih_fs_image_count;
}


int
fdat_fs_image_length(const char *fname_fdat, int image_idx)
{
	FDAT_IMAGE_HEADER	fdat_hdr;
	FDAT_FS_IMAGE_DESC	*p_desc;

	if (fdat_read_image_header(fname_fdat, &fdat_hdr)) {
		return -1;
	}
	if (image_idx >= fdat_hdr.fih_fs_image_count) {
		return -1;
	}
	p_desc = &fdat_hdr.fih_fs_image_info[image_idx];
	return (int)p_desc->ffid_fs_length;
}


int
fdat_extract_fs_image(const char *fname_fdat, int image_idx, const char *fname_fs_image)
{
	FDAT_IMAGE_HEADER	fdat_hdr;
	int	ret = 0;
	FDAT_FS_IMAGE_DESC	*p_desc;

	if (fdat_read_image_header(fname_fdat, &fdat_hdr)) {
		goto exit_err;
	}
	if (image_idx >= fdat_hdr.fih_fs_image_count) {
		goto exit_err;
	}

	p_desc = &fdat_hdr.fih_fs_image_info[image_idx];

	// some firmware images seem to have a zero-length second fw image
	// (as opposed to setting # images to '1'), so return "ok"
	// if length is zero (and don't create any output file)
	if (!p_desc->ffid_fs_length) {
		goto exit_ok;
	}
	if (_fdat_extract_image(fname_fdat, p_desc->ffid_fs_offset, p_desc->ffid_fs_length, fname_fs_image) != 0) {
		fprintf(stderr, "Error extracting filesystem image #%d from '%s'!\n", image_idx, fname_fdat);
		goto exit_err;
	}
	goto exit_ok;

exit_err:
	ret = 1;
	unlink(fname_fs_image);
	goto exit_common;
exit_ok:
	ret = 0;
	// fallthru
exit_common:
	return ret;
}
