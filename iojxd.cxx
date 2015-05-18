//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include <unistd.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "child_termcb.hxx"
#include "luafound.hxx"

lua_State *state = NULL;

void ixcb_child_terminated(evutil_socket_t sigchld_, short evt, void *arg) {
    (void) sigchld_; (void) evt; (void) arg;

    while (1) {
        int status = -1;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0)
            break;

        int estatus = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
        int termsig = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
        ixc_set_termcb(pid, estatus, termsig);
        ixc_call_n_remove_termcb(pid);
    }
}

int main() {

    ixlu_initstate(&state);
    ixlb_reg_interface(state);
    ixlu_dofile(state, "init.lua");

    struct event_base *ixevbase = event_base_new();
    assert(ixevbase != NULL);

    struct event *ev_chldterm = event_new(ixevbase, SIGCHLD, EV_SIGNAL | EV_PERSIST,
                                          ixcb_child_terminated, NULL);
    event_add(ev_chldterm, NULL);

    event_base_dispatch(ixevbase);

    return 0;
}
