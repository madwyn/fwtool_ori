#ifndef FWTOOL_PROJECT_CMD_H
#define FWTOOL_PROJECT_CMD_H


#include "fwt_names.h"


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define MSG_DESCRIPTION     "fwtool " VERSION " Copyright 2012-2014 http://www.nex-hack.info"

#define OPT_GRP_MODE        "grp_mode"
#define OPT_MODE_EXTRACT    "--extract"
#define OPT_MODE_CREATE     "--create"
#define OPT_MODE_BROWSE     "--browse"
#define OPT_MODE_ADV_HELP   "--advanced_help"

#define OPT_GRP_IO          "grp_io"
#define OPT_IN              "--input"
#define OPT_OUT             "--ouput"

#define OPT_GRP_OPTION      "grp_option"
#define OPT_VER_MINOR       "--minor"
#define OPT_VER_MAJOR       "--major"
#define OPT_LEVEL           "--level"
#define OPT_VERBOSE         "--verbose"

#define MSG_MODE_EXTRACT "eXtract firmware.\n" \
            "Extract complete content, the content can be used as input to modify and create mode." \
			"Related arguments:\n" \
			"--input, required\n" \
			"--output, optional\n"

#define MSG_MODE_CREATE "Create firmware.\n" \
            "Only rebuild FDAT, reencrypt & rebuild fwdata to date.\n" \
			"Related arguments:\n" \
			"--input, required\n" \
			"--output, optional"

#define MSG_MODE_BROWSER "eXtract firmware (default mode, may be omitted).\n" \
			"Argument: downloaded Windows Firmware Update *.exe file in current path, extension '.exe' may be omitted.\n" \
			"Extraction to directory contained in zip inside *.exe to current path or destination directory if -d is given.\n" \
			"Extract complete content, used as input to modify and create mode." \

#define MSG_MODE_ADV_HELP  "Show advanced help with all possible options and flags with detialed usage."

#define MSG_IN "For --extract: input file\n" \
			"Downloaded Windows Firmware Update *.exe file in current path, extension '.exe' may be omitted.\n" \
			"Extraction to directory contained in zip inside *.exe\n" \
			"For --create: input directory\n" \
			"Created with eXtract mode, this may contain a relative or full path.\n" \
			"<UpdaterDirectory>/nexhack/level3/*.mod.* must exist, this files will be repacked, reencrypted and repacked to a valid FirmwareUpdate*.dat." \
            "Further versions of fwtool will repack lower levels not implemented yet." \
            "Run <INPUT_DIR>/FirmwareUpdater.exe to update with modified firmware <UpdaterDirectory>/Resource/FirmwareData_NexHack.dat.\n" \
			"Original firmware is saved to <UpdaterDirectory>/Resource/FirmwareData_Original.dat.save."

#define MSG_OUT	"For --extract: output directory\n" \
			"The directory for extracting the complete firmware updater. Default will output to current path.\n" \
			"For --create: output file\n" \
			"If not provided, the default output is <INPUT_DIR>/FirmwareUpdater.exe."

#define MSG_VER_MINOR   "set mInor version number in create or modify mode (default increment):\n" \
			"Minor version number may be in the range from 0 to " STR(FWT_MAXMINORVER) "."

#define MSG_VER_MAJOR   "set maJor version number in create or modify mode (default leave):\n" \
            "Major version number may be in the range from 1 to " STR(FWT_MAXMAJORVER) "."

#define MSG_LEVEL       "Level to extract to or create from (default 127):\n" \
			"Used for modify mode to do only needed unpacking (not implemented yet)."

#define MSG_VERBOSE    "Be Verbose (not implemented yet)."
#endif //FWTOOL_PROJECT_CMD_H
