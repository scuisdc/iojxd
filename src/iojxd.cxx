//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include <ev++.h>

#include "common.hxx"
#include "luafound.hxx"
#include "timer.hxx"

#include "lua_inc.hxx"

int main() {

    ixc_context *ctx = ixc_create_context();

//    ixfd_sock *sock_echo = ixfd_commonsock_create(ctx);
//    ixfd_commonsocket_tcp_createnbind(sock_echo, "127.0.0.1", 6666);
//    ixfd_commonsocket_tcp_listen(sock_echo);
//
//    sock_echo->cb_read = [] (ixfd_conn_ctx *ctx, const char *data, size_t len) {
//        ixfd_commonsock_write(ctx, data, len, NULL);
//    };

//    ixfd_sock *sock_client = ixfd_commonsock_create(ctx);
//    ixfd_commonsock_tcp_createnconnect(sock_client, "127.0.0.1", 6666,
//                                       [] (ixfd_conn_ctx *ctx, void *args) {
//                                           ixfd_commonsock_write(ctx, "ooolive", 8, NULL, NULL);
//                                       }, NULL, NULL);

    ixlu_dofile(ctx->state, "init.lua");

    ev_run(ctx->evl, 0);

    lua_close(ctx->state);
    ev_loop_destroy(ctx->evl);
    free(ctx);

    return 0;
}
