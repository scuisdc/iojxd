//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "childprocess.hxx"

#include <unistd.h>

#include <ev++.h>

#include "debug.hxx"
#include "context.hxx"
#include "sock.hxx"

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

struct ixut_child_watcher;

typedef void (*ixut_child_callback)(ixut_child_watcher *watcher, void *args);

struct ixut_child_watcher {
    ev_child *event;
    ixut_child_callback cb;
    void *data;

    ixc_context *context;
};

void ixut_childw_cb(struct ev_loop *loop, ev_child *w, int revent) {
    assert(w->data != NULL);
    assert(((ixut_child_watcher *) w->data)->event == w);

    ixut_child_watcher *childw = (ixut_child_watcher *) w->data;
    if (childw->cb != NULL) {
        (*childw->cb)(childw, childw->data);
    }
}

ixut_child_watcher *ixut_childw_create(ixc_context *context) {
    assert(context != NULL);

    ixut_child_watcher *ret = (ixut_child_watcher *) malloc(sizeof(*ret));
    memset(ret, 0, sizeof(*ret));
    ret->event = (ev_child *) malloc(sizeof(*ret->event));

    ev_init(ret->event, ixut_childw_cb);
    ret->event->data = ret;

    return ret;
}

void ixut_childw_start(ixut_child_watcher *childw, pid_t pid,
                                      ixut_child_callback cb, void *args) {
    assert(childw != NULL);

    ev_child_set(childw->event, pid, 0);
    childw->cb = cb; childw->data = args;

    ev_child_start(childw->context->evl, childw->event);

}