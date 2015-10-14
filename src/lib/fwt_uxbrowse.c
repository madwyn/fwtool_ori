//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2014, nex-hack project
//
// This file "fwt_uxbrowse.c" is part of fwtool (http://www.nex-hack.info)
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

// fwt_util contains global logfile, slash conversion, console output

#include <stdlib.h>
#include "config.h"
#include "fwt_names.h"
#include "fwt_uxbrowse.h"


int
ux_read_file(const char *fname_uxf, const char *dirname)
{
	int	ret = 0;
	FILE	*fh_in=NULL, *fh_out=NULL;
	UX_HEADER	*ux_hdr=NULL;
	char	fname_out[MAXPATH] = "";
	unsigned char	*p_iobuf=NULL;
	void	*ptr=NULL;
	int	i=0, ptr_count=0, ptr_mult=0, ptr_len=0, ptr_lst_len=0, ptr_offset=sizeof(*ux_hdr);
	int data_offset=0, ptr_mask=0, filesize=0;


	if (!(fh_in = fopen(fname_uxf, "rb"))) {
		fprintf(stderr, "Failed to open UX file '%s'!\n", fname_uxf);
		goto exit_err;
	}
	fseek(fh_in, 0L, SEEK_END); filesize = ftell(fh_in); fseek(fh_in, 0L, SEEK_SET);
	if (!(p_iobuf = malloc(filesize))) {
		fprintf(stderr, "Failed to allocate buffer for UX file '%s'!\n", fname_uxf);
		goto exit_err;
	}
	if (fread(p_iobuf, 1, filesize, fh_in) != filesize) {
		fprintf(stderr, "Failed to read UX file '%s'!\n", fname_uxf);
		goto exit_err;
	}
	fclose(fh_in);

	ux_hdr = (UX_HEADER *) p_iobuf;
	if (memcmp(ux_hdr->ux_magic, UX_MAGIC, sizeof(ux_hdr->ux_magic))) {
		fprintf(stderr, "No UX file header!\n");
		goto exit_err;
	}
	ptr_count = ux_hdr->ux_ptrlow_count + ((ux_hdr->ux_ptrflag_bitmap & UX_PTRFLAG_MASK) << 8);

	switch (ux_hdr->ux_mode) {
		case UX_MODE_32B_PTR:
				ptr_len = 4;
				ptr_mult = 1;
				ptr_mask=0xFFFFFFFF;	// 32bit value
				ptr_lst_len = 4*ptr_count;
				data_offset = ptr_offset + ptr_lst_len;
				break;
		case UX_MODE_16B_PTR:
				ptr_len = 2;
				ptr_mult = 4;	// ptr offsets 32bit int, not char in 16bit ptr mode
				ptr_mask=0x0000FFFF;	// 16bit value
				ptr_lst_len = 2*ptr_count;
				data_offset = ptr_offset + ptr_lst_len + (ptr_count % 2)*2; // ptr list padded to 32bit boundary
				break;
		default:
				fprintf(stderr, "Unknown UX mode '0x%02x'\n", ux_hdr->ux_mode);
				goto exit_err;
	}

	strcpy(fname_out, fname_uxf);
	strcat(fname_out, ".data");
	if (!(fh_out = fopen(fname_out, "wb"))) {
		fprintf(stderr, "Failed to open UX data output file '%s'!\n", fname_out);
		goto exit_err;
	}

	fprintf(stderr, "file: '%s' cat 'ux%c' type '0x%02x' index '0x%02x'\r\n", fname_uxf, ux_hdr->ux_magic2[0], ux_hdr->ux_mode, ux_hdr->ux_index);
	fprintf(fh_out, "file: '%s' cat 'ux%c' type '0x%02x' index '0x%02x'\r\n", fname_uxf, ux_hdr->ux_magic2[0], ux_hdr->ux_mode, ux_hdr->ux_index);
	fprintf(fh_out, "flags: 0x0d-bit7..5='0x%02x' 0x0e='0x%02x' 0x0f='0x%02x'\r\n", ux_hdr->ux_ptrflag_bitmap&UX_FLAGPTR_MASK, ux_hdr->ux_flag2, ux_hdr->ux_flag3);
	fprintf(fh_out, "pad  : 0x04='0x%08x' 0x09='0x%02x' 0x0b='0x%02x'\r\n", ux_hdr->ux_unknown_01, ux_hdr->ux_unknown_02, ux_hdr->ux_unknown_03);
	fprintf(fh_out, "ptr_offset: '0x%08x', ptr_len    : '0x%08x', ptr_mult   : '0x%08x'\r\n", ptr_offset, ptr_len, ptr_mult);
	fprintf(fh_out, "ptr_count : '0x%08x', ptr_lst_len: '0x%08x', data_offset: '0x%08x'\r\n\r\n", ptr_count, ptr_lst_len, data_offset);

	int last_ptr=0, data_len=0, j=0, k=0;
	unsigned char	c=0;

	for(i=0;i<ptr_count+1;i++){
		ptr = p_iobuf + ptr_offset + (i * ptr_len);
		if((*(int*)ptr&ptr_mask)==ptr_mask) {
			fprintf(fh_out, "Ptr %03d: offset '0x%08x' eff '0x%08x' data_len '0x00' (0xff-pointer)\r\n", i-1, last_ptr, last_ptr+data_offset);
			continue;
		}
		data_len = (*(int*)ptr&ptr_mask)*ptr_mult - last_ptr;
		if(i==0 && (*(int*)ptr&ptr_mask) != 0) {
			fprintf(stderr, "First data pointer offset not Null: 0x%08x\n", (*(int*)ptr&ptr_mask));
			goto exit_err;
		}
		if(i==ptr_count) {
			data_len = filesize-data_offset-last_ptr;
		}
		if(i>0) {
			j=0;
			fprintf(fh_out, "Ptr %03d: offset '0x%08x' eff '0x%08x' data_len '0x%08x'\r\n", i-1, last_ptr, last_ptr+data_offset, data_len);
			if(data_len>15) {
				for(j=0;j<data_len/16;j++) {
					fprintf(fh_out, "0x%08x: ", data_offset+last_ptr+j*16);
					for(k=0;k<16;k++) {
						fprintf(fh_out, "%02x ", *(unsigned char*)(p_iobuf+data_offset+last_ptr+j*16+k));
					}
					fprintf(fh_out, " - ");
					for(k=0;k<16;k++) {
						c=*(unsigned char*)(p_iobuf+data_offset+last_ptr+j*16+k);
						if((c>0x1f && c<0x81) || c>0x9f) fprintf(fh_out, "%c", c);
						else fprintf(fh_out, "\007");
					}
					fprintf(fh_out, "\r\n");
				}
			}
			if(data_len%16 != 0) {
				fprintf(fh_out, "0x%08x: ", data_offset+last_ptr+j*16);
				for(k=0;k<data_len%16;k++) {
					fprintf(fh_out, "%02x ", *(unsigned char*)(p_iobuf+data_offset+last_ptr+j*16+k));
				}
				for(k=data_len%16;k<16;k++) {
					fprintf(fh_out, "   ");
				}
				fprintf(fh_out, " - ");
				for(k=0;k<data_len%16;k++) {
					c=*(unsigned char*)(p_iobuf+data_offset+last_ptr+j*16+k);
					if((c>0x1f && c<0x81) || c>0x9f) fprintf(fh_out, "%c", c);
					else fprintf(fh_out, "\007");
				}
				fprintf(fh_out, "\r\n");
			}
		}
		last_ptr = (*(int*)ptr&ptr_mask)*ptr_mult;
	}

	fclose(fh_out);
	goto exit_ok;

exit_err:
	if (fh_in) fclose(fh_in);
	if (fh_out) fclose(fh_out);
	ret = EXIT_FAILURE;
	goto exit_common;
exit_ok:
	ret = EXIT_SUCCESS;
	// fallthru
exit_common:
	if (p_iobuf) {
		free((void *)p_iobuf);
	}
	return ret;
}
