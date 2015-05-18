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

#include <unordered_map>

static std::unordered_map<pid_t, ixc_termcb_args *> child_cb_map;

void ixc_add_termcb(pid_t pid, ixc_termcb_t cb, void *args) {
    ixc_termcb_args *r = (ixc_termcb_args *) malloc(sizeof(*r));
    memset((void *) r, 0, sizeof(*r));
    r->pid = pid;
    r->cb = cb;
    r->data = args;
    child_cb_map.insert({ pid, r });
}

ixc_termcb_args *ixc_get_termcb(pid_t pid) {
    return child_cb_map.at(pid); }

size_t ixc_remove_termcb(pid_t pid) {
    ixc_termcb_args *r = child_cb_map.at(pid);
    size_t ret = child_cb_map.erase(pid);
    free(r);
    return ret;
}

void ixc_set_termcb(pid_t pid, int status, int term_signal) {
    ixc_termcb_args *args = ixc_get_termcb(pid);
    args->status = status;
    args->term_signal = term_signal;
}

ixc_termcb_args *ixc_call_n_remove_termcb(pid_t pid) {
    ixc_termcb_args *ret = ixc_get_termcb(pid);
    ret->cb(ret);
    ixc_remove_termcb(pid);
    return ret;
}