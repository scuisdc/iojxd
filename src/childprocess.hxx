//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_CHILDPROCESS_HXX
#define IOJXD_CHILDPROCESS_HXX

#include <sys/types.h>

#include <ev++.h>

#include "common.hxx"

struct ixut_child_watcher;

typedef void (*forked_process_callback)(void *args);
typedef void (*ixut_child_callback)(ixut_child_watcher *watcher, void *args);

struct ixut_child_watcher {
    ev_child *event;
    ixut_child_callback cb;
    void *data;

    ixc_context *context;
};

struct ixut_child_exitstatus {
    int status_raw;
    int status;
    int exit_cause;
    int term_sig;
};

pid_t ixut_fork(forked_process_callback cb, void *args);

ixut_child_watcher *ixut_childw_create(ixc_context *context);
void ixut_childw_free(ixut_child_watcher *childw);

void ixut_childw_start(ixut_child_watcher *childw, pid_t pid,
                       ixut_child_callback cb, void *args);

pid_t ixut_childw_getpid(ixut_child_watcher *childw);
ixut_child_exitstatus ixut_childw_getstatus(ixut_child_watcher *childw);

#endif // IOJXD_CHILDPROCESS_HXX
