//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2014, nex-hack project
//
// This file "tarfile.c" is part of fwtool (http://www.nex-hack.info)
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

#define	TARFILE_C

#include "config.h"
#include "fwt_names.h"
#include "fwt_util.h"
#include <errno.h>
#include <stdlib.h>

#include "archive.h"
#include "archive_entry.h"

#include "tarfile.h"
#include "dir.h"

static tar_handle
_create_th(struct archive *a)
{
	tar_handle	retval=NULL;

	if ((retval = malloc(sizeof(tar_handle_struct)))) {
		memset((void *)retval, 0, sizeof(tar_handle_struct));
		retval->a = a;
	}

	return retval;
}


static void
_free_th(tar_handle th)
{
	if (th) {
		free((void *)th);
	}
}


tar_handle
_tarfile_open(const char *fname)
{
	struct archive *a=NULL;

	if ((a = archive_read_new())) {
		archive_read_support_format_tar(a);
		if (archive_read_open_filename(a, fname, TAR_IOBUF_SIZE) == 0) {
			return _create_th(a);
		} else {
			archive_read_free(a);
		}
	}
	return NULL;
}


void
_tarfile_close(tar_handle th)
{
	if (th) {
		if (th->a) {
			archive_read_close(th->a);
			archive_read_free(th->a);
		}
		_free_th(th);
	}

}


int
is_tarfile(const char *fname)
{
	tar_handle	th=NULL;

	if ((th = _tarfile_open(fname))) {
		_tarfile_close(th);
		return 1;
	} else {
		return 0;
	}
}


static int
_tarfile_extract_create_dir(char *path)
{
	int r=0;
	struct stat st;
	char *slash=NULL, *base=NULL;

	/* Check for special names and just skip them. */
	slash = strrchr(path, '/');
	if (slash == NULL)
		base = path;
	else
		base = slash + 1;

	if (base[0] == '\0' ||
	    (base[0] == '.' && base[1] == '\0') ||
	    (base[0] == '.' && base[1] == '.' && base[2] == '\0')) {
		/* Don't bother trying to create null path, '.', or '..'. */
		if (slash != NULL) {
			*slash = '\0';
			r = _tarfile_extract_create_dir(path);
			*slash = '/';
			return r;
		}
		return 0;
	}

	/*
	 * Yes, this should be stat() and not lstat().  Using lstat()
	 * here loses the ability to extract through symlinks.  Also note
	 * that this should not use the a->st cache.
	 */
	if (stat(path, &st) == 0) {
		if (S_ISDIR(st.st_mode))
			return 0;
		if (unlink(path) != 0) {
			return 1;
		}
	} else if (errno != ENOENT && errno != ENOTDIR) {
		/* Stat failed? */
		return 1;
	} else if (slash != NULL) {
		*slash = '\0';
		r = _tarfile_extract_create_dir(path);
		*slash = '/';
		if (r != 0)
			return r;
	}

	if (0 == MKDIR(path)) {
		return 0;
	}

	/*
	 * Without the following check, a/b/../b/c/d fails at the
	 * second visit to 'b', so 'd' can't be created.  Note that we
	 * don't add it to the fixup list here, as it's already been
	 * added.
	 */
	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
		return 0;

	return 1;
}


static int
_tarfile_extract_current_entry(tar_handle th, const char *dirname_out, struct archive_entry *entry, const char *fname_tar_attr)
{
	int	ret=0;
	const char	*entry_fname = archive_entry_pathname(entry);
	unsigned long	entry_uid = (unsigned long)archive_entry_uid(entry);
	unsigned long	entry_gid = (unsigned long)archive_entry_gid(entry);
	unsigned long	entry_mode = (unsigned long)archive_entry_mode(entry);
	const char	*entry_modestr = archive_entry_strmode(entry);
	long	entry_atime = (long)archive_entry_atime(entry);
	long	entry_ctime = (long)archive_entry_ctime(entry);
	long	entry_mtime = (long)archive_entry_mtime(entry);
	long	entry_btime = (long)archive_entry_birthtime(entry);
	char	entry_fullname[_TMP_FNAME_BUFLEN] = {0};
	char	parent_dirname[_TMP_FNAME_BUFLEN] = {0};
	char	entry_attr_string[_TMP_FNAME_BUFLEN] = {0};
	char	entry_specialinfo_str[_TMP_FNAME_BUFLEN] = {0};
	char	*p=NULL;
	char	*p_entry_basename=NULL;
	//char	*p_entry_dir = NULL;
	int	len=0, is_dir=0, is_special=0;
	FILE	*fh_out=NULL, *fh_attr=NULL;

	//
	// NOTE: we *assume* here that "entry_fname" has forward-slashes delimiting the directory
	// (basename of files can have '\' (esp in /dev/), so don't translate)
	//
	if (dirname_out) {
		sprintf(entry_fullname, "%s/%s", dirname_out, entry_fname);
	} else {
		if ((entry_fname[0] == '/')) {
			strcpy(entry_fullname, entry_fname+1);
		} else {
			strcpy(entry_fullname, entry_fname);
		}
	}

	is_dir = 0;
	if (entry_modestr[0] == 'd') {
		is_dir = 1;
		p = strchr(entry_fullname, '\0');
		// remove possible trailing slash
		if ((p > entry_fullname) && (p[-1] == '/')) {
			p[-1] = '\0';
		}
	} else {
		is_dir = 0;
	}
	if ((p_entry_basename = strrchr(entry_fullname, '/'))) {
		++p_entry_basename;
		len = (p_entry_basename - entry_fullname - 1);
		strncpy(parent_dirname, entry_fullname, len);
		parent_dirname[len] = '\0';
	} else {
		p_entry_basename = entry_fullname;
		parent_dirname[0] = '\0';
	}

	entry_specialinfo_str[0] = '\0';
	switch(entry_modestr[0]) {
	default:
		break;
	case 'd':	// directory
	case '-':	// regular file
		entry_specialinfo_str[0] = '\0';
		break;
	case 'c':	// character device
	case 'b':	// block device
		is_special = 1;
		sprintf(entry_specialinfo_str, "(%u,%u)", archive_entry_rdevmajor(entry), archive_entry_rdevminor(entry));
		break;
	case 's':	// socket
		is_special = 1;
		break;
	case 'p':	// fifo
		is_special = 1;
		break;
	case 'h':	// hardlink
		is_special = 1;
		sprintf(entry_specialinfo_str, "%s", archive_entry_hardlink(entry));
		break;
	case 'l':	// symlink
		is_special = 1;
		sprintf(entry_specialinfo_str, "%s", archive_entry_symlink(entry));
		break;
	}

	sprintf(entry_attr_string, "|%s|UID:%lu|GID:%lu|MODE:%lu|ATIME:%li|CTIME:%li|MTIME:%li|BTIME:%li|%s|%s|%s|%s|", \
			entry_modestr, entry_uid, entry_gid, entry_mode, entry_atime, entry_ctime, entry_mtime, entry_btime, \
			entry_specialinfo_str, entry_fname, parent_dirname, p_entry_basename);

	if (parent_dirname[0]) {
		if (_tarfile_extract_create_dir(parent_dirname) != 0) {
			fprintf(stderr, "Error creating parent dir: '%s'!\n", parent_dirname);
			goto exit_err;
		}
		if (!(fh_attr = fopen(fname_tar_attr, "a"))) {
			fprintf(stderr, "Error storing entry attributes for file '%s'\n", entry_fname);
			goto exit_err;
		}
		fprintf(fh_attr, "%s\n", entry_attr_string);
		fclose(fh_attr);
	}
	if (fwt_verbose_global) {
		sprintf(plog_global, "Extracting %s\n", entry_fullname); log_it(plog_global);
	}
	if (is_dir) {
		// if this entry is a directory, go ahead and create that directory
		// (with an empty attributes file)
		if (_tarfile_extract_create_dir(entry_fullname) != 0) {
			fprintf(stderr, "Error creating parent dir: '%s'!\n", parent_dirname);
			goto exit_err;
		}
	} else if (!is_special) {
		// it's a normal file that we can extract from the tar
		const void *block_buf;
		size_t block_size;
		long long	offset;	// int64_t

		if (!(fh_out = fopen(entry_fullname, "wb"))) {
			fprintf(stderr, "Error creating tar extract file '%s'!\n", entry_fullname);
			goto exit_err;
		}
		while(1) {
			ret = archive_read_data_block(th->a, &block_buf, &block_size, &offset);
			if (ret == ARCHIVE_EOF) {
				break;
			}
			if (ret != ARCHIVE_OK) {
				fprintf(stderr, "Error reading from tar file!\n");
				goto exit_err;
			}
			if (fwrite(block_buf, 1, block_size, fh_out) != block_size) {
				fprintf(stderr, "Error writing tar extract file '%s'!\n", entry_fullname);
				goto exit_err;
			}
		}
		fclose(fh_out);
	} else {
		// special file: do I want to create a dummy file with info inside it
		// (like "symlink: '%s')??
#if	0
		if (!(fh_out = fopen(entry_fullname, "wb"))) {
			fprintf(stderr, "Error creating tar extract 'special' file '%s'!\n", entry_fullname);
			goto exit_err;
		}
		fprintf(fh_out, "SPECIAL: '%s/%s'\n", entry_modestr, entry_specialinfo_str);
		fclose(fh_out);
#endif
	}
	goto exit_ok;

exit_err:
	if (fh_out) fclose(fh_out);
	if (entry_fullname[0]) unlink(entry_fullname);
	ret = 1;
	goto exit_common;

exit_ok:
	ret = 0;
	// fallthru
exit_common:
	return ret;
}


int
tarfile_extract_all(const char *fname_tar, const char *dirname_out, const char *fname_tar_attr)
{
	int	ret=0;
	tar_handle	th=NULL;
	FILE	*fh_attr=NULL;
	struct archive_entry *entry=NULL;
	const char	*p_entry_fname=NULL;

	if (!(th = _tarfile_open(fname_tar))) {
		fprintf(stderr, "Error opening tar file '%s'!\n", fname_tar);
		goto exit_err;
	}
	if (!(fh_attr = fopen(fname_tar_attr, "w"))) {
		fprintf(stderr, "tarfile_create_entry_attributes() returned error!\n");
		goto exit_err;
	}
	else fclose(fh_attr);

	MKDIR(dirname_out);

	while(1) {
		ret = archive_read_next_header(th->a, &entry);
		if (ret == ARCHIVE_EOF) {
			break;
		}
		if (ret != ARCHIVE_OK) {
			fprintf(stderr, "Error reading next tar file header!\n");
			goto exit_err;
		}
		p_entry_fname = archive_entry_pathname(entry);
		if (_tarfile_extract_current_entry(th, dirname_out, entry, fname_tar_attr) != 0) {
			fprintf(stderr, "Error extracting file '%s' from tar!\n", p_entry_fname);
			goto exit_err;
		}
	}
	_tarfile_close(th);
	goto exit_ok;

exit_err:
	if (th) _tarfile_close(th);
	ret = 1;
	goto exit_common;

exit_ok:
	ret = 0;
	// fallthru
exit_common:
	return ret;
}


// from libarchive/archiv.h:
/*-
 * To create an archive:
 *   1) Ask archive_write_new for an archive writer object.
 *   2) Set any global properties.  In particular, you should set
 *      the compression and format to use.
 *   3) Call archive_write_open to open the file (most people
 *       will use archive_write_open_file or archive_write_open_fd,
 *       which provide convenient canned I/O callbacks for you).
 *   4) For each entry:
 *      - construct an appropriate struct archive_entry structure
 *      - archive_write_header to write the header
 *      - archive_write_data to write the entry data
 *   5) archive_write_close to close the output
 *   6) archive_write_free to cleanup the writer and release resources
 */

// from minitar.c, modified
int
tarfile_create_all(const char *fname_tar, const char *dirname_in)
{
	int	ret=0;
	char buff[TAR_IOBUF_SIZE]="";
	struct archive *a=NULL;
	struct archive_entry *entry=NULL;
	ssize_t len=0;
	FILE	*fd=NULL;

	a = archive_write_new();
	archive_write_add_filter_none(a);
	archive_write_set_format_ustar(a);
	archive_write_open_filename(a, fname_tar);

	if (dirname_in != NULL) {
		struct archive *disk = archive_read_disk_new();
		int r;
		size_t dirlen = strlen(dirname_in)+1;

		r = archive_read_disk_open(disk, dirname_in);
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "tarfile_create: %s\n", archive_error_string(disk));
			goto exit_err;
		}

		for (;;) {
			const char *filename;

			entry = archive_entry_new();
			r = archive_read_next_header2(disk, entry);
			if (r == ARCHIVE_EOF)
				break;
			if (r != ARCHIVE_OK) {
				fprintf(stderr, "tarfile_create: %s\n", archive_error_string(disk));
				goto exit_err;
			}
			archive_read_disk_descend(disk);

			//TODO set attributes
			// dont put full pathname into tar
			filename = archive_entry_pathname(entry);
			archive_entry_set_pathname(entry, filename + dirlen);
			if (fwt_verbose_global) {
				sprintf(plog_global, "a %s\n", archive_entry_pathname(entry)); log_it(plog_global);
			}

			r = archive_write_header(a, entry);
			if (r < ARCHIVE_OK) {
				fprintf(stderr, "tarfile_create: %s\n", archive_error_string(a));
				sprintf(plog_global, "tarfile_create: %s\n", archive_error_string(a)); log_it(plog_global);
			}
			if (r == ARCHIVE_FATAL) {
				fprintf(stderr, "tarfile_create: Fatal error creating tar!\n");
				goto exit_err;
			}
			if (r > ARCHIVE_FAILED) {
				fd = fopen(archive_entry_sourcepath(entry), "rb");
				len = fread(buff, 1, sizeof(buff), fd);
				while (len > 0) {
					archive_write_data(a, buff, len);
					len = fread(buff, 1, sizeof(buff), fd);
				}
				fclose(fd);
			}
			archive_entry_free(entry);
		}
		archive_read_close(disk);
		archive_read_free(disk);
	}
	goto exit_ok;

exit_err:
// TODO: handle archive_read_*(disk) ?
	ret = 1;
	goto exit_common;

exit_ok:
	ret = 0;
	// fallthru
exit_common:
	archive_write_close(a);
	archive_write_free(a);
	if (fwt_verbose_global) {
		sprintf(plog_global, "\n"); log_it(plog_global);
	}
	return ret;
}
