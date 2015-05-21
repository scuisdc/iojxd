//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "context.hxx"

#include <stdlib.h>

#include <ev++.h>

#include "lua_inc.hxx"
#include "debug.hxx"
#include "luafound.hxx"
#include "luafoundx.hxx"

ixc_context *ixc_create_context() {
    ixc_context *r = (ixc_context *) malloc(sizeof(*r));
    ixlu_initstate(&r->state);
    ixlb_reg_interface(r->state);
    ixlbx_reg_interface(r->state);

    r->evl = ev_default_loop(0);

    lua_pushstring(r->state, "_context");
    lua_pushlightuserdata(r->state, (void *) r);
    lua_settable(r->state, LUA_REGISTRYINDEX);

    return r;
}
