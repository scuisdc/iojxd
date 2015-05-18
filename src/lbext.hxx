//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_LBEXT_HXX
#define IOJXD_LBEXT_HXX

#include "luafound.hxx"

struct ixlb_LightUserdataTrait;

using ixlb_StateAsUserdata = ixlb_LightUserdataTrait;

namespace luabridge {

template<>
struct Stack<ixlb_LightUserdataTrait *> {
    static inline void push(lua_State *L, ixlb_LightUserdataTrait *value) {
        lua_pushlightuserdata(L, (void *) value); }

    static inline ixlb_LightUserdataTrait *get(lua_State *L, int index) {
        return (ixlb_LightUserdataTrait *) lua_touserdata(L, index); }
};

namespace ixex {

LuaRef fromCFunction (lua_State *L, lua_CFunction func) {
    lua_pushcfunction(L, func);
    return LuaRef::fromStack(L, 1);
}

}

}

#endif //IOJXD_LBEXT_HXX
