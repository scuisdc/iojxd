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

#include "common.hxx"
#include "luafound.hxx"

int main() {

    ixc_context *ctx = ixc_create_context();

    ixlu_dofile(ctx->state, "init.lua");

    event_base_dispatch(ctx->evb);

    return 0;
}
