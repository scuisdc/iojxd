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

struct event_base;

struct ixc_context {
    lua_State *state;
    event_base *evb;
};

ixc_context *ixc_create_context();

#endif //IOJXD_CONTEXT_HXX
