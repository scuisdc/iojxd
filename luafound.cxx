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

#include "foundation.hxx"

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

void ixlb_spawn_process(const char *file, LuaRef cb) {
    LuaRef *ref_cb = new LuaRef(cb);
    ixut_spawn_process(file, NULL, [] (ixc_termcb_args *args) {
        LuaRef *ref_cb_inner = (LuaRef *) (args->data);
        (*ref_cb_inner)(args->pid, args->status, args->term_signal);
        delete ref_cb_inner;
    }, (void *) ref_cb);
}

void ixlb_reg_interface(lua_State *state) {

    luabridge::getGlobalNamespace(state).
            addFunction("spawn_process", &ixlb_spawn_process);

}
