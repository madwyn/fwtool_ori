//
// Sony NEX camera firmware toolbox
//
// written (reverse-engineered) by Paul Bartholomew, released under the GPL
// (originally based on "pr.exe" from nex-hack.info, with much more since then)
//
// Copyright (C) 2012-2013, nex-hack project
//
// This file "fwt_util.c" is part of fwtool (http://www.nex-hack.info)
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

#include "config.h"
#include "fwt_names.h"
#include "fwt_util.h"


void
do_version(const char *tool_name)
{
	printf("%s " VERSION "\n", tool_name);
	printf("info at http://www.nex-hack.info\n\n" \
			"BE WARNED! This is unfinished and/or incomplete and/or untested code!\n" \
			"USE AT YOUR OWN RISK!\n\n");
}


void
do_usage(const char *tool_name)
{
	printf("usage : %s [-<mode>] [-<options>] file or directory\n\n" \
			"  mode :\n" \
			"    -x  eXtract firmware (default mode)\n" \
			"    -c  Create firmware (only reencrypt&rebuild fwdata to date)\n" \
			"    -m  Modify firmware (not implemented yet)\n" \
			"    -h  show extended Help\n" \
			"    -t  show Todo list\n" \
			"  options :\n" \
			"    -d  Destination directory (eXtract only, optional)\n" \
			"    -l  Level to extract to or create from (default 127)\n" \
			"    -v  be Verbose\n", tool_name);
}

void
do_help(const char *tool_name)
{
	printf("help (extended) :" \
			"usage : %s [-<mode>] [-<options>] file or directory\n\n" \
			"  mode :\n" \
			//23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
			"    -x  eXtract firmware (default mode, may be omitted):\n" \
			"        Argument: downloaded Windows Firmware Update *.exe file in current\n" \
			"        path, extension '.exe' may be omitted.\n" \
			"        Extraction to directory contained in zip inside *.exe to current path\n" \
			"        or destination directory if -d is given.\n" \
			"        Extract complete content, used as input to modify and create mode.\n" \

			"    -c  Create firmware (only reencrypt&rebuild fwdata to date):\n" \
			"        Argument: <UpdaterDirectory> created with eXtract mode, this may contain\n" \
			"        a relative or full path.\n" \
			"        <UpdaterDirectory>/nexhack/level3/*.mod.* must exist, this files will be\n" \
			"        repacked, reencrypted and repacked to a valid FirmwareUpdate*.dat,\n" \
			"        further versions of fwtool will repack lower levels not implemented yet.\n" \
			"        Run <UpdaterDirectory>/FirmwareUpdater.exe to update cam with modified\n" \
			"        firmware <UpdaterDirectory>/Resource/FirmwareData_NexHack.dat.\n" \
			"        Original firmware is saved to\n" \
 			"        <UpdaterDirectory>/Resource/FirmwareData_Original.dat.save.\n" \

			"    -m  Modify firmware (not implemented yet)\n" \
			"    -h  show extended Help (this list)\n" \
			"    -t  show Todo list\n" \

			"  options :\n" \
			"    -d  Destination directory (eXtract only, optional):\n" \
			"        Extracting complete firmware updater directory not to current path but\n" \
			"        to this directory.\n" \
			"        In create mode a destination makes no sense, so this option is ignored.\n" \
			"    -l  Level to extract to or create from (default 127):\n" \
			"        Used for modify mode to do only needed unpacking, not implemented yet.\n" \
			"    -v  be Verbose (not implemented yet)\n" \
			"  fwtool appends extended log output to fwtool_log.txt in current path.\n", tool_name);
}

void
do_todolist(const char *tool_name)
{
	printf("TODO list %s "VERSION" (in that order)\n\n" \
			"  v07: implement repacking of tar, lzpt\n" \
			"  v07: implement level logic for ease of repack (create mode)\n" \
			"  v07: fully integrate fwtoolGUI into main code\n" \
			"  v07: cleanup variables, variable names and code ;)\n" \
			"  v08: implement unpacking of cramfs, cramfs-lz77, axfs, vfat, ext2\n" \
			"  v08: implement repacking of vfat, ext2, cramfs(lz77), axfs\n" \
			"  v08: implement modify config&code package format\n" \
			"  v09: implement on the fly in memory modify for end users\n" \
			"  v09: implement modify of level6 (bin fw's for sub cpu's)\n" \
			"  v09: clear many minor TODO, UGLY; do heavy testing\n" \
			"  v1.0: document what this tools do ...\n\n", tool_name);
}


char *
convert_path_slashes(char *buf)
{
	char	*p;
	if (buf) {
		while((p = strrchr(buf, '\\'))) {
			*p = '/';
		}
	}
	return buf;
}


void
log_it(char *pinfo)
{
    FILE *plog;

    if((plog = fopen(LOGFILE,"a")))
    {
        fputs(pinfo,plog);
        fflush(plog);
        fclose(plog);
    }
}
