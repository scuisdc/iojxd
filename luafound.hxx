//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_LUAFOUND_HXX
#define IOJXD_LUAFOUND_HXX

struct lua_State;

void ixlu_initstate(lua_State **state);

void ixlu_dofile(lua_State *state, const char *path);

void ixlb_reg_interface(lua_State *state);

#endif //IOJXD_LUAFOUND_HXX
