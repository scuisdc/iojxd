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

#include <unordered_map>
#include <functional>

// because we cannot have foundation
//  so this is the ASSERTNOTREACHED
#define ASSERT_FOUNDATION() assert(0);

using IXChildTermCallbackT = std::function<void (pid_t, int)>;

void ixlb_initstate(lua_State **state) {
    *state = luaL_newstate();
    luaL_openlibs(*state);
}

using namespace luabridge;

void ixlb_reg_interface(lua_State *state) {

    getGlobalNamespace(state);

}

void ixlb_dofile(lua_State *state, const char *path) {
    if (luaL_dofile(state, path)) {
        printf("error in script!\n");
    }
}

lua_State *state = NULL;

void ixut_spawn_process(const char *file, char * const argv[], IXChildTermCallbackT cb) {
    pid_t pid = fork();

    switch (pid) {
        case -1:
            break;
        case 0:
            break;
        default:
            execvp(file, argv);
            ASSERT_FOUNDATION();
    }
}

void ixcb_child_terminated(evutil_socket_t sigchld_, short evt, void *arg) {
    (void) sigchld_; (void) evt; (void) arg;

    while (1) {
        int status = -1;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if (pid <= 0)
            break;
    }
}

int main() {

    ixlb_initstate(&state);
    ixlb_reg_interface(state);
    ixlb_dofile(state, "init.lua");

    struct event_base *ixevbase = event_base_new();
    assert(ixevbase != NULL);

    struct event *ev_chldterm = event_new(ixevbase, SIGCHLD, EV_SIGNAL | EV_PERSIST,
                                          ixcb_child_terminated, NULL);
    event_add(ev_chldterm, NULL);

    event_base_dispatch(ixevbase);

    return 0;
}
