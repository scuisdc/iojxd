//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "child_termcb.hxx"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <ev++.h>

#include "common.hxx"
#include "debug.hxx"

void ixcb_child_terminated_ev(EV_P_ struct ev_child *w, int revents);

void ixut_spawn_process(const char *file, char * const argv[], ixc_termcb_t cb, void *args) {
    pid_t pid = fork();

    switch (pid) {
        case -1:
            printf("%s: fork failed.", __func__);
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

void ixc_enable_termcb(ixc_context *ctx) {
    assert(ctx->evl != NULL);
}

void ixc_disable_termcb(ixc_context *ctx) {
    assert(ctx->evl != NULL);
}

void ixc_add_termcb(pid_t pid, ixc_termcb_t cb, void *args) {
    ixc_termcb_args *r = (ixc_termcb_args *) malloc(sizeof(*r));
    memset((void *) r, 0, sizeof(*r));
    r->pid = pid;
    r->cb = cb;
    r->data = args;

    ev_child *childt = (ev_child *) malloc(sizeof(*childt));
    ev_child_init(childt, ixcb_child_terminated_ev, pid, 0);
    childt->data = r;
    ev_child_start(EV_DEFAULT_ childt);
}

void ixcb_child_terminated_ev(EV_P_ struct ev_child *w, int revents) {
    ixc_termcb_args *args = (ixc_termcb_args *) w->data;
    args->pid = w->rpid;
    args->status = w->rstatus;
    (args->cb)(args);

    // 0519: forget switching off the event
    //  could lead to unexpected behaviour?
    ev_child_stop(loop, w);
    free(args);
    free(w);
}
