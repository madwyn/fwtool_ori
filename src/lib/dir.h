#ifndef FWT_DIR_H
#define FWT_DIR_H

#if defined(_MSC_VER)
#include <direct.h>
#define MKDIR(dirname) _mkdir(dirname)
#else
#include <sys/stat.h>
#define MKDIR(dirname) mkdir(dirname, 0775)
#endif

#endif //FWT_DIR_H
