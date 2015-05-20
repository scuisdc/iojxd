//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "luafound.hxx"

#include <stdio.h>

#include "lua_inc.hxx"
#include "common.hxx"
#include "child_termcb.hxx"
#include "foundation.hxx"
#include "lbext.hxx"

#include "debug.hxx"
#include "sock.hxx"
#include "timer.hxx"
#include "childprocess.hxx"

#include <unistd.h>

#include <ev++.h>

// using namespace luabridge;
using luabridge::LuaRef;

void ixlu_initstate(lua_State **state) {
    *state = luaL_newstate();
    assert((*state) != NULL);
    luaL_openlibs(*state);
}

void ixlu_dofile(lua_State *state, const char *path) {
    if (luaL_dofile(state, path)) {
        printf("error in script!\n");
    }
}

ixc_context *ixlb_get_cur_ctx(lua_State *state) {
    lua_pushstring(state, "_context");
    lua_gettable(state, LUA_REGISTRYINDEX);
    ixc_context *r = (ixc_context *) lua_touserdata(state, -1);

    return r;
}

void ixlb_spawn_process(const char *file, LuaRef cb, lua_State *L) {
    LuaRef *ref_cb = new LuaRef(cb);

    ixut_spawn_process(file, NULL, [] (ixc_termcb_args *args) {
        LuaRef *ref_cb_inner = (LuaRef *) (args->data);
        if (!ref_cb_inner->isNil()) {
            (*ref_cb_inner)(args->pid, args->status, args->term_signal); }
        // delete ref_cb_inner;
    }, (void *) ref_cb);
}

void ixlb_sock_write(struct ixfd_conn_ctx *ctx, const char *data, size_t len, LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);

    ixfd_commonsock_write(ctx, data, len, [] (ixfd_conn_ctx *ctx, void *args) {
        LuaRef *ref_cb_inner = (LuaRef *) args;
        if (!ref_cb_inner->isNil()) {
            (*ref_cb_inner)(ctx); }
        // delete ref_cb_inner;
    }, (void *) ref_cb);
}

struct ixlb_connect_data {
    LuaRef *cb_suc;
    LuaRef *cb_fail;
};

void ixlb_sock_connect(struct ixfd_sock *sock, const char *ip, unsigned short port, LuaRef
        cb_success, LuaRef cb_failed) {
    ixlb_connect_data *data = (ixlb_connect_data *) malloc(sizeof(ixlb_connect_data));
    data->cb_suc = new LuaRef(cb_success);
    data->cb_fail = new LuaRef(cb_failed);

    ixfd_commonsock_tcp_createnconnect(sock, ip, port, [] (ixfd_conn_ctx *ctx, void *args) {
        ixlb_connect_data *d = (ixlb_connect_data *) args;
        if (!d->cb_suc->isNil()) {
            (*d->cb_suc)(ctx); }

        delete d->cb_suc;
        delete d->cb_fail;
        free(d);
    }, [] (ixfd_sock *sock, void *args) {
        ixlb_connect_data *d = (ixlb_connect_data *) args;
        if (!d->cb_fail->isNil()) {
            (*d->cb_fail)(sock); }

        delete d->cb_suc;
        delete d->cb_fail;
        free(d);
    }, (void *) data);
}

struct ixlb_sock_data {
    LuaRef *cb_read;
};

ixfd_sock *ixlb_sock_create(struct ixc_context *ctx) {
    ixfd_sock *ret = ixfd_commonsock_create(ctx);
    ixlb_sock_data *data = (ixlb_sock_data *) malloc(sizeof(*data));

    data->cb_read = NULL;
    ret->data = (void *) data;
    return ret;
}

void ixlb_sock_set_read_callback(struct ixfd_sock *sock, LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);

    ixlb_sock_data *data = (ixlb_sock_data *) sock->data;
    if (data->cb_read != NULL) {
        delete data->cb_read; }
    data->cb_read = ref_cb;

    sock->cb_read = [] (ixfd_conn_ctx *ctx, const char *data, size_t len) {
        LuaRef *cb = ((ixlb_sock_data *) (ctx->sock->data))->cb_read;
        if (cb && !cb->isNil()) {
            (*cb)(ctx, data, len); }
    };
}

void ixlb_timer_start(struct ixut_timer *timer, double interval, LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);

    ixut_timer_start(timer, interval, [] (ixut_timer *timer, void *args) {
        LuaRef *cb_inner = (LuaRef *) args;
        if (!cb_inner->isNil()) {
            (*cb_inner)(timer); }
    }, (void *) ref_cb);
}

int ixlb_fork(LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);

    return ixut_fork([] (void *args) {
        LuaRef *cb_inner = (LuaRef *) args;
        (*cb_inner)();
    }, (void *) ref_cb);
}

void ixlb_childw_start(ixut_child_watcher *childw, pid_t pid, LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);

    ixut_childw_start(childw, pid, [] (ixut_child_watcher *watcher, void *args) {
        LuaRef *cb_inner = (LuaRef *) args;
        if (!cb_inner->isNil()) {
            (*cb_inner)(watcher); }
    }, (void *) ref_cb);
}

pid_t ixlb_getpid() {
    return getpid(); }

void ixlb_init_loop(lua_State *L) {
    ev_loop_fork(EV_DEFAULT);
    ixlb_get_cur_ctx(L)->evl = ev_default_loop(0);
}

int ixlb_exec(lua_State *L) {
    int l = lua_gettop(L);

    const char **argv = (const char **) malloc((l+1) * sizeof(char *));
    for (size_t i = 0; i < l; i++) {
        const char *arg = lua_tostring(L, i+1);
        argv[i] = arg;
    }
    argv[l] = NULL;

    int e = execvp(argv[0], (char *const *) argv);
    ASSERT_FOUNDATION();
    return 0;
}

int ixlu_resume(lua_State *L) {
    int v = lua_gettop(L);
    lua_State *Lco = (lua_State *) lua_touserdata(L, 1);
    lua_xmove(L, Lco, v-1);
    lua_resume(Lco, v-1);
    return 0;
}

// TODO: this is the only way to 'resume' coroutines
//      inside callbacks, currently
void ixlb_spawn_process_co(const char *file, lua_State *L) {
    printf("%s\n", file);
    ixut_spawn_process(file, NULL, [] (ixc_termcb_args *args) {
        lua_State *Lco = (lua_State *) (args->data);
        lua_State *Lmain = ixlb_get_cur_ctx(Lco)->state;
        LuaRef r = luabridge::ixex::fromCFunction(Lmain, ixlu_resume);
        r((ixlb_StateAsUserdata *) Lco, args->pid, args->status, args->term_signal);
    }, (void *) L);
}

void ixlb_reg_interface(lua_State *state) {

    luabridge::getGlobalNamespace(state).
        beginNamespace("iojx").
            beginClass<ixc_context>("ixc_context").
            endClass().
            addFunction("current_context", &ixlb_get_cur_ctx).
            addFunction("enable_termcb", &ixc_enable_termcb).
            addFunction("disable_termcb", &ixc_disable_termcb).
            addFunction("spawn_process", &ixlb_spawn_process).
            beginNamespace("sock").
                beginClass<ixfd_sock>("socket").endClass().
                beginClass<ixfd_conn_ctx>("context").
                    addData("fd", &ixfd_conn_ctx::fd).
                endClass().
                addFunction("create", &ixlb_sock_create).
                addFunction("free", &ixfd_commonsock_free).
                beginNamespace("tcp").
                    addFunction("bind_ip", &ixfd_commonsock_tcp_createnbind).
                    addFunction("connect", &ixlb_sock_connect).
                    addFunction("listen", &ixfd_commonsock_tcp_listen).
                endNamespace().
                addFunction("setreadbuflen", &ixfd_commonsock_set_bufread_len).
                addFunction("write", &ixlb_sock_write).
                addFunction("set_read_callback", &ixlb_sock_set_read_callback).
            endNamespace().
            beginNamespace("timer").
                beginClass<ixut_timer>("timer").endClass().
                addFunction("create", &ixut_timer_create).
                addFunction("free", &ixut_timer_free).
                addFunction("start", &ixlb_timer_start).
                addFunction("stop", &ixut_timer_stop).
                addFunction("again", &ixut_timer_again).
                addFunction("setimeout", &ixut_timer_setimeout).
                addFunction("tick", &ixut_timer_tick).
            endNamespace().
            beginNamespace("util").
                addFunction("fork", &ixlb_fork).
                addFunction("getpid", &ixlb_getpid).
                addFunction("init_loop", &ixlb_init_loop).
                addCFunction("exec", &ixlb_exec).
            endNamespace().
            beginNamespace("child_process").
                beginClass<ixut_child_watcher>("watcher").endClass().
                beginClass<ixut_child_exitstatus>("status").
                    addData("status", &ixut_child_exitstatus::status).
                    addData("exit_cause", &ixut_child_exitstatus::exit_cause).
                    addData("termsig", &ixut_child_exitstatus::term_sig).
                endClass().
                addFunction("create", &ixut_childw_create).
                addFunction("free", &ixut_childw_free).
                addFunction("start", &ixlb_childw_start).
                addFunction("getpid", &ixut_childw_getpid).
                addFunction("getstatus", &ixut_childw_getstatus).
            endNamespace().
            beginNamespace("co").
                addFunction("spawn_process", &ixlb_spawn_process_co).
            endNamespace().
        endNamespace();

}
