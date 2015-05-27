//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/27.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "platformed.hxx"

#include <unistd.h>
#include <stdlib.h>

#include <string>

#include "platform.hxx"

#ifdef CUBE_PLATFORM_LINUX

#include <limits.h>

std::string ixut_execpath() {
    char buffer[PATH_MAX * 2];

    ssize_t n = readlink("/proc/self/exe", buffer, PATH_MAX * 2 - 1);

    if (n == -1)
        return "ixut_exepath: error in readlink.";
    else {
        buffer[n] = '\0';
        return buffer;
    }
}

#elif defined(CUBE_PLATFORM_MACH)

#include <sys/syslimits.h>
#include <mach-o/dyld.h>

std::string ixut_execpath() {
    constexpr size_t len_execpath = 2 * PATH_MAX;
    char execpath[len_execpath] = { '\0' };
    char abspath[len_execpath] = { '\0' };

    uint32_t darwin_exepath_len = len_execpath;
    if (_NSGetExecutablePath(execpath, &darwin_exepath_len) == 0 &&
        realpath(execpath, abspath) == abspath && strlen(abspath) > 0) {
        memcpy(execpath, abspath, strlen(abspath) + 1);
    } else return "ixut_exepath: unknown error.";

    return execpath;
}

#endif