//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/19.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "sock.hxx"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include <ev++.h>

#include "common.hxx"
#include "foundation.hxx"
#include "util.hxx"

void ixut_set_socket_nonblock(int fd);

struct ixfd_conn_ctx *ixfd_commonsock_create_ctx(ixfd_sock *sock, int fd);

void ixfd_commonsock_accept_cb(struct ev_loop *loop, ev_io *w_, int revents);
void ixfd_commonsock_connect_cb(struct ev_loop *loop, ev_io *w_, int revents);
void ixfd_commonsock_read_cb(struct ev_loop *loop, ev_io *w_, int revents);
void ixfd_commonsock_write_cb(struct ev_loop *loop, ev_io *w_, int revents);

struct ixfd_sock *ixfd_commonsock_create(ixc_context *ctx) {
    assert(ctx != NULL);
    assert(ctx->evl != NULL);

    struct ixfd_sock *sock = (struct ixfd_sock *) malloc(sizeof(*sock));
    memset(sock, 0, sizeof(*sock));

    sock->type = IXFD_SOCK_UNKNOWN;
    sock->connected = sock->active = sock->listening = false;
    sock->cb_read = NULL; sock->cb_close = NULL;
    sock->default_ctx = NULL;

    sock->event_accept = sock->event_connect = NULL;

    sock->fd = 0;
    sock->context = ctx;

    return sock;
}

void ixfd_commonsock_free(struct ixfd_sock *sock) {
    assert(sock != NULL);
    assert(!sock->active);

    free(sock->event_accept);
    free(sock);
}

void ixfd_commonsock_tcp_createnbind(ixfd_sock *sock, const char *ip, unsigned short port) {
    assert(sock != NULL);
    assert(sock->type == IXFD_SOCK_UNKNOWN);

    sock->fd = socket(AF_INET, SOCK_STREAM, 0);
    ixut_set_socket_nonblock(sock->fd);

    sock->event_accept = (ev_io *) malloc(sizeof(*sock->event_accept));
    ev_init(sock->event_accept, ixfd_commonsock_accept_cb);
    sock->event_accept->data = (void *) sock;
    ev_io_set(sock->event_accept, sock->fd, EV_READ);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(port);

    if (bind(sock->fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        printf("ixfd_commonsocket_tcp_createnbind: failed when binding to %s:%u", ip, port);
        return;
    }

    sock->type = IXFD_SOCK_TCP;
}

void ixfd_commonsock_tcp_createnconnect(struct ixfd_sock *sock, const char *ip, unsigned short
        port) {
    assert(sock != NULL);
    assert(sock->type == IXFD_SOCK_UNKNOWN);

    sock->fd = socket(AF_INET, SOCK_STREAM, 0);
    ixut_set_socket_nonblock(sock->fd);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(port);

    sock->event_connect = (ev_io *) malloc(sizeof(*sock->event_connect));
    ev_init(sock->event_connect, ixfd_commonsock_connect_cb);
    sock->event_connect->data = (void *) sock;
    ev_io_set(sock->event_connect, sock->fd, EV_WRITE);

    int e = connect(sock->fd, (struct sockaddr *) &sin, sizeof(sin));
    if ((!e) || ((e == -1) && errno == EINPROGRESS)) {
        ev_io_start(sock->context->evl, sock->event_connect);
    } else {
        printf("ixfd_commonsocket_tcp_createnconnect: failed when connecting to %s:%u", ip, port);
        return;
    }
}

void ixfd_commonsock_tcp_listen(ixfd_sock *sock) {
    assert(sock != NULL);
    assert(sock->type == IXFD_SOCK_TCP);
    assert(sock->fd != 0);

    listen(sock->fd, 16);
    ev_io_start(sock->context->evl, sock->event_accept);
}

bool ixfd_commonsock_write(struct ixfd_conn_ctx *ctx, const char *data, size_t len,
                           ixfd_context_callback cb, void *args) {
    size_t n = write(ctx->fd, data, len);

    if (n >= len) {
        if (cb != NULL) {
            cb(ctx, args); }
        return true;
    } else {
        ixfd_sock_write_ctx *ctx_write = (struct ixfd_sock_write_ctx *) malloc(sizeof(*ctx));

        if (n > 0) {
            data = data + n;
            len = len - n;
        }

        char *cp = (char *) malloc(len * sizeof(char));
        memset((void *) cp, 0, len * sizeof(char));
        memcpy(cp, data, len);

        ctx_write->len_remain = len;
        ctx_write->data = ctx_write->data_org = cp;
        ctx_write->cb = cb;

        ixut_cycqueue_enqueue(ctx->buf_write, ctx_write);

        if (!ctx->write_launched) {
            ctx->write_launched = true;
            ev_io_start(ctx->sock->context->evl, ctx->event_write); }

        return false;
    }
}

void ixfd_commonsock_set_bufread_len(struct ixfd_conn_ctx *conn, size_t len) {
    assert(conn != NULL);
    assert(len > 0);

    char *realloced = (char *) malloc((len+1) * sizeof(char));

    if (realloced != NULL) {
        if (conn->buf_read != NULL) {
            free(conn->buf_read); }
        conn->buf_read = realloced;
        conn->size_bufread = len;
    }
}

struct ixfd_conn_ctx *ixfd_commonsock_create_ctx(ixfd_sock *sock, int fd) {
    ixfd_conn_ctx *ctx = (ixfd_conn_ctx *) malloc(sizeof *ctx);
    memset(ctx, 0, sizeof(*ctx));
    ctx->fd = fd;
    ctx->sock = sock;
    ctx->write_launched = false;

    ctx->cb_read = ctx->sock->cb_read;
    ctx->cb_close = ctx->sock->cb_close;

    ev_io *io_read = (ev_io *) malloc(sizeof(*io_read));
    ev_init(io_read, ixfd_commonsock_read_cb);
    ev_io_set(io_read, fd, EV_READ);

    ev_io *io_write = (ev_io *) malloc(sizeof(*io_write));
    ev_init(io_write, ixfd_commonsock_write_cb);
    ev_io_set(io_write, fd, EV_WRITE);
    ctx->event_read = io_read; ctx->event_write = io_write;
    io_read->data = io_write->data = (void *) ctx;

    ctx->buf_read = NULL; ixfd_commonsock_set_bufread_len(ctx, 4096);
    ctx->buf_write = ixut_cycqueue_create();

    return ctx;
}

void ixfd_commonsock_accept_cb(struct ev_loop *loop, ev_io *w_, int revents) {
    assert(w_->data != NULL);

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(w_->fd, (struct sockaddr *) &ss, &slen);
    ixut_set_socket_nonblock(fd);

    struct ixfd_conn_ctx *ctx = ixfd_commonsock_create_ctx((ixfd_sock *) w_->data, fd);

    ev_io_start(loop, ctx->event_read);
    // ev_io_start(loop, io_write);
}

void ixfd_commonsock_connect_cb(struct ev_loop *loop, ev_io *w_, int revents) {
    assert(w_->data != NULL);
    ixfd_sock *sock = (ixfd_sock *) w_->data;

    if (revents & EV_WRITE) {

        int optval = 0;
        socklen_t optlen = sizeof(optval);

        if ((getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &optval, &optlen)) == 1 ||
                (optval != 0)) {

        } else {
            sock->default_ctx = ixfd_commonsock_create_ctx(sock, sock->fd);
            sock->type = IXFD_SOCK_TCP;

            if (sock->cb_conn) {
                sock->cb_conn(sock->default_ctx); }
        }

        ev_io_stop(loop, w_);
    }
}

void ixfd_commonsock_read_cb(struct ev_loop *loop, ev_io *w_, int revents) {
    assert(w_ != NULL);
    assert(w_->data != NULL);
    struct ixfd_conn_ctx *ctx = (struct ixfd_conn_ctx *) w_->data;

    memset(ctx->buf_read, 0, ctx->size_bufread+1);
    int n = read(ctx->fd, ctx->buf_read, ctx->size_bufread);
    if (n > 0) {
        if (ctx->cb_read != NULL) {
            ctx->cb_read(ctx, ctx->buf_read, n); }
    } else if (n < 0) {

    } else {
        ev_io_stop(loop, ctx->event_read);
        free(ctx->event_read); free(ctx->event_write);

        close(ctx->fd);
        if (ctx->cb_close != NULL) {
            ctx->cb_close(); }

        free(ctx->buf_read);
        ixut_cycqueue_free(ctx->buf_write);

        free(ctx);
    }
}

void ixfd_commonsock_write_cb(struct ev_loop *loop, ev_io *w_, int revents) {
    assert(w_ != NULL);
    assert(w_->data != NULL);
    struct ixfd_conn_ctx *ctx = (struct ixfd_conn_ctx *) w_->data;

    while (!ixut_cycqueue_empty(ctx->buf_write)) {
        struct ixfd_sock_write_ctx *ctx_write = (struct ixfd_sock_write_ctx *) ixut_cycqueue_front
                (ctx->buf_write);
        size_t n = write(ctx->fd, ctx_write->data, ctx_write->len_remain);

        if (n >= ctx_write->len_remain) {
            ixut_cycqueue_dequeue(ctx->buf_write);
            if (ctx_write->cb != NULL) {
                ctx_write->cb(ctx, ctx_write->cb_args); }

            free((void *) ctx_write->data_org);
            free(ctx);
        } else {
            if (n > 0) {
                ctx_write->len_remain -= n;
                ctx_write->data += n;
            }
        }
    }

    ctx->write_launched = false;
    ev_io_stop(loop, w_);
}

// Well, no support for Win32 IOCtl! Who cares!
void ixut_set_socket_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {

    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {

    }
}