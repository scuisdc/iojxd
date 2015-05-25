//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "childprocess.hxx"

#include <string.h>
#include <unistd.h>

#include <ev++.h>

#include "debug.hxx"
#include "common.hxx"

void ixut_childw_cb(struct ev_loop *loop, ev_child *w, int revent);

pid_t ixut_fork(forked_process_callback cb, void *args) {
    pid_t pid = fork();

    switch (pid) {
        case -1:
            return -1;
            break;

        case 0:
            cb(args);
            abort();
            ASSERT_FOUNDATION();
            break;

        default:
            return pid;
            break;
    }
}

void ixut_childw_cb(struct ev_loop *loop, ev_child *w, int revent) {
    assert(w->data != NULL);
    assert(((ixut_child_watcher *) w->data)->event == w);

    ixut_child_watcher *childw = (ixut_child_watcher *) w->data;
    ev_child_stop(loop, w);
    if (childw->cb != NULL) {
        (*childw->cb)(childw, childw->data); }
}

ixut_child_watcher *ixut_childw_create(ixc_context *context) {
    assert(context != NULL);

    ixut_child_watcher *ret = (ixut_child_watcher *) malloc(sizeof(*ret));
    memset(ret, 0, sizeof(*ret));
    ret->event = (ev_child *) malloc(sizeof(*ret->event));
    ret->context = context;

    ev_init(ret->event, ixut_childw_cb);
    ret->event->data = ret;

    return ret;
}

void ixut_childw_free(ixut_child_watcher *childw) {
    assert(childw != NULL);
    assert(childw->event != NULL);

    assert(!ev_is_active(childw->event));
    assert(!ev_is_pending(childw->event));

    free(childw->event);
    free(childw);
}

void ixut_childw_start(ixut_child_watcher *childw, pid_t pid,
                                      ixut_child_callback cb, void *args) {
    assert(childw != NULL);
    assert(childw->event != NULL);

    assert(!ev_is_active(childw->event));
    assert(!ev_is_pending(childw->event));

    ev_child_set(childw->event, pid, 0);
    childw->cb = cb; childw->data = args;

    ev_child_start(childw->context->evl, childw->event);

}

pid_t ixut_childw_getpid(ixut_child_watcher *childw) {
    assert(childw != NULL);
    return childw->event->rpid; }

ixut_child_exitstatus ixut_childw_getstatus(ixut_child_watcher *childw) {
    assert(childw != NULL);

    ixut_child_exitstatus status;
    memset(&status, 0, sizeof(status));

    int raw = childw->event->rstatus;

    status.status = WEXITSTATUS(raw);
    status.term_sig = WTERMSIG(raw);

    if (WIFEXITED(raw))
        status.exit_cause = 0;
    if (WIFSIGNALED(raw))
        status.exit_cause = 1;
    if (WIFSTOPPED(raw))
        status.exit_cause = 2;

    return status;
}
