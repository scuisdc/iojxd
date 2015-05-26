//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_CONTEXT_HXX
#define IOJXD_CONTEXT_HXX

struct lua_State;

// struct event_base;

struct ev_loop;

struct ixc_context {
    lua_State *state;
    // event_base *evb;
    struct ev_loop *evl;
};

ixc_context *ixc_create_context();

void ixc_destroy_context(ixc_context *ctx);

#endif //IOJXD_CONTEXT_HXX
