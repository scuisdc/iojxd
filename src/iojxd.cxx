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

#include <ev++.h>

#include "common.hxx"
#include "luafound.hxx"
#include "sock.hxx"

int main() {

    ixc_context *ctx = ixc_create_context();

    ixfd_sock *sock_echo = ixfd_commonsock_create();
    ixfd_commonsocket_tcp_createnbind(sock_echo, "127.0.0.1", 6666);
    ixfd_commonsocket_tcp_listen(ctx, sock_echo);

    sock_echo->cb_read = [] (ixfd_conn_ctx *ctx, const char *data, size_t len) {
        ixfd_commonsock_write(ctx, data, len, NULL);
    };

    ixlu_dofile(ctx->state, "init.lua");

    ev_run(ctx->evl, 0);

    return 0;
}
