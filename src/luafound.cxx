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

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include "common.hxx"
#include "child_termcb.hxx"
#include "foundation.hxx"
#include "lbext.hxx"

// using namespace luabridge;
using luabridge::LuaRef;

void ixlu_initstate(lua_State **state) {
    *state = luaL_newstate();
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
        (*ref_cb_inner)(args->pid, args->status, args->term_signal);
        delete ref_cb_inner;
    }, (void *) ref_cb);
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
            beginNamespace("co").
                addFunction("spawn_process", &ixlb_spawn_process_co).
            endNamespace().
        endNamespace();

}
