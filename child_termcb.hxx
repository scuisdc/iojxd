//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_CHILD_TERMCB_HXX
#define IOJXD_CHILD_TERMCB_HXX

#include <stddef.h>
#include <sys/types.h>

#include <functional>

struct ixc_termcb_args;

typedef void (*ixc_termcb_t)(struct ixc_termcb_args *);

struct ixc_termcb_args {
    void *data;
    ixc_termcb_t cb;
    pid_t pid;
    int status;
    int term_signal;
};

void ixc_add_termcb(pid_t pid, ixc_termcb_t cb, void *args);

ixc_termcb_args *ixc_get_termcb(pid_t pid);

size_t ixc_remove_termcb(pid_t pid);

void ixc_set_termcb(pid_t pid, int status, int term_signal);

ixc_termcb_args *ixc_call_n_remove_termcb(pid_t pid);

#endif //IOJXD_CHILD_TERMCB_HXX
