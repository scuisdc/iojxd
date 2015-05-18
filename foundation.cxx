//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "foundation.hxx"

#include <sys/types.h>
#include <unistd.h>

#include "debug.hxx"
#include "child_termcb.hxx"

void ixut_spawn_process(const char *file, char * const argv[], ixc_termcb_t cb, void *args) {
    pid_t pid = fork();

    switch (pid) {
        case -1:

            break;
        case 0:
            execvp(file, argv);
            ASSERT_FOUNDATION();
            break;
        default:
            ixc_add_termcb(pid, cb, args);
            break;
    }
}

