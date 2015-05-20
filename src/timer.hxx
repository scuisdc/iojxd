//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_TIMER_HXX
#define IOJXD_TIMER_HXX

#include <ev++.h>

#include "common.hxx"

struct ixut_timer;

typedef void (*ixut_timer_callback)(struct ixut_timer *, void *);

struct ixut_timer {
    ixut_timer_callback cb;
    void *args;

    bool active;
    bool is_timeout;
    double interval;
    ev_timer *_timer;
    ev_tstamp _last_act;
    ixc_context *context;
};

ixut_timer *ixut_timer_create(ixc_context *context);
void ixut_timer_free(ixut_timer *timer);

void ixut_timer_start(ixut_timer *timer, double interval, ixut_timer_callback cb, void *args);
void ixut_timer_stop(ixut_timer *timer);

void ixut_timer_again(ixut_timer *timer);

void ixut_timer_setimeout(ixut_timer *timer);
void ixut_timer_tick(ixut_timer *timer);

#endif // IOJXD_TIMER_HXX
