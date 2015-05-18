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

#include <ev++.h>
#include <netinet/in.h>
#include <sys/fcntl.h>

#include "common.hxx"
#include "luafound.hxx"

#include <list>

struct ixfd_sock {
    ev_io *io;
};

// Well, no support for Win32 IOCtl! Who cares!
void ixut_set_socket_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {

    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {

    }
}

// EV_P_
void listen_cb(struct ev_loop *loop, ev_io *w_, int revents) {

    if (revents & EV_READ) {
        char buf[2048] = { '\0' };
        size_t n = read(w_->fd, buf, sizeof(buf)-1);
        if (n > 0) printf("listen_cb: %s\n", buf);
        else {
            // EV_A_
            ev_io_stop(loop, w_);
            close(w_->fd);
            printf("closed\n");
        }
    }
}

void ixfd_commonsock_write_cb(struct ev_loop *loop, ev_io *w_, int revents) {

}

void accept_cb(struct ev_loop *loop, ev_io *w_, int revents) {
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(w_->fd, (struct sockaddr *) &ss, &slen);
    ixut_set_socket_nonblock(fd);

    ev_io *io = (ev_io *) malloc(sizeof(*io));
    ev_init(io, listen_cb);
    ev_io_set(io, fd, EV_READ);
    ev_io_start(loop, io);
}

int main() {

    ixc_context *ctx = ixc_create_context();

    ixlu_dofile(ctx->state, "init.lua");

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(7777);

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    ixut_set_socket_nonblock(fd);

    bind(fd, (struct sockaddr *) &sin, sizeof(sin));
    listen(fd, 16);
    ev_io *ev_mainsock = (ev_io *) malloc(sizeof(*ev_mainsock));
    ev_io_init(ev_mainsock, accept_cb, fd, EV_READ);
    ev_io_start(ctx->evl, ev_mainsock);

    ev_run(ctx->evl, 0);

    return 0;
}
