//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "timer.hxx"

#include <string.h>
#include <stdlib.h>

#include <assert.h>

#include "common.hxx"

void ixut_timer_cb(struct ev_loop *loop, ev_timer *w, int revents);
void ixut_timeout_cb(struct ev_loop *loop, ev_timer *w, int revents);

ixut_timer *ixut_timer_create(ixc_context *context) {
    ixut_timer *ret = (ixut_timer *) malloc(sizeof(*ret));
    memset(ret, 0, sizeof(*ret));

    ret->_timer = (ev_timer *) malloc(sizeof(*ret->_timer));
    ev_init(ret->_timer, ixut_timer_cb);
    ret->_timer->data = (void *) ret;

    ret->context = context;
    ret->active = ret->is_timeout = false;
    ret->interval = 0.0;
    return ret;
}

void ixut_timer_free(ixut_timer *timer) {
    assert(timer != NULL);
    assert(!timer->active);

    free(timer->_timer);
    free(timer);
}

void ixut_timer_start(ixut_timer *timer, double interval, ixut_timer_callback cb, void *args) {
    assert(timer != NULL);

    timer->cb = cb;
    timer->args = args;

    ev_timer_set(timer->_timer, interval, 0.);
    ev_timer_start(timer->context->evl, timer->_timer);
    timer->active = true;
    timer->interval = interval;
}

void ixut_timer_stop(ixut_timer *timer) {
    assert(timer != NULL);
    assert(timer->active);

    ev_timer_stop(timer->context->evl, timer->_timer);
    timer->active = false;
}

void ixut_timer_setimeout(ixut_timer *timer) {
    assert(timer != NULL);

    timer->is_timeout = true;
    ev_init(timer->_timer, ixut_timeout_cb);
}

void ixut_timer_tick(ixut_timer *timer) {
    assert(timer != NULL);

    timer->_last_act = ev_now(timer->context->evl);
}

void ixut_timer_again(ixut_timer *timer) {
    assert(timer != NULL);

    ev_timer_again(timer->context->evl, timer->_timer);
    timer->active = true;
}

void ixut_timer_cb(struct ev_loop *loop, ev_timer *w, int revents) {
    assert(w->data != NULL);
    assert(((ixut_timer *) w->data)->_timer == w);

    ixut_timer *timer = (ixut_timer *) w->data;
    timer->active = false;
    if (timer->cb != NULL) {
        (*timer->cb)(timer, timer->args);
    }
}

void ixut_timeout_cb(struct ev_loop *loop, ev_timer *w, int revents) {
    assert(w->data != NULL);
    assert(((ixut_timer *) w->data)->_timer == w);
    assert(((ixut_timer *) w->data)->is_timeout);

    ixut_timer *timer = (ixut_timer *) w->data;
    ev_tstamp now = ev_now(loop);
    ev_tstamp timeout = timer->_last_act + timer->interval;
    if (timeout < now) {
        if (timer->cb != NULL) {
            timer->active = false;
            (*timer->cb)(timer, timer->args);
        }
    } else {
        w->repeat = timeout - now;
        ixut_timer_again(timer);
    }
}
