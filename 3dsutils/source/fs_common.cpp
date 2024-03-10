// Copyright 2013 Dolphin Emulator Project / 2014-2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "fs_common.h"

#include <sys/stat.h>
#include <errno.h>
#include <cstdio>

void SanitizeSeparators(std::string* path)
{
    size_t position = 0;
    while ((position = path->find("//", 0)) != path->npos) {
        path->erase(position, 1);
    }
}

bool CreateFullPath(const std::string& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode) == 0) {
            // Not a directory
            return false;
        }
        // Dir already exists
        return true;
    }

    for (size_t position = 0; ; position++) {
        // Find next sub path
        position = path.find('/', position);
        if (position == path.npos)
            return true;

        // Include the '/' so the first call is CreateDir("/") rather than CreateDir("")
        std::string const subPath(path.substr(0, position + 1));

        if (stat(subPath.c_str(), &st) != 0) {
            if (mkdir(subPath.c_str(), 0755) != 0 && errno != EEXIST)
                return false;
        } else if (!S_ISDIR(st.st_mode)) {
            return false;
        }
    }

    return true;
}
