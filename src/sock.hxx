//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/19.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_SOCK_HXX
#define IOJXD_SOCK_HXX

#include <ev++.h>

#include "common.hxx"
#include "util.hxx"
#include "timer.hxx"

#define IXFD_SOCK_UNKNOWN 0
#define IXFD_SOCK_TCP 1
#define IXFD_SOCK_UNIX 2
#define IXFD_SOCK_UDP 3

struct ixfd_sock;
struct ixfd_conn_ctx;

typedef int ixfd_socktype;
typedef void (*ixfd_data_read_callback)(ixfd_conn_ctx *ctx, const char *data, size_t len);
typedef void (*ixfd_connect_fail_callback)(ixfd_sock *sock, void *args);
typedef void (*ixfd_context_callback)(ixfd_conn_ctx *ctx, void *args);

struct ixfd_sock {
    int fd;
    ev_io *event_accept;
    ev_io *event_connect;

    ixfd_data_read_callback cb_read;
    // ixc_void_callback cb_close;

    ixfd_socktype type;
    bool active;
    bool connected;
    bool listening;

    ixc_context *context;
    ixfd_conn_ctx *default_ctx;
    void *data;
};

struct ixfd_conn_ctx {
    struct ixfd_sock *sock;

    int fd;
    ev_io *event_read;
    ev_io *event_write;

    ixut_cycqueue *buf_write;
    size_t size_bufread;
    char *buf_read;

    ixfd_data_read_callback cb_read;
    // ixc_void_callback cb_close;

    bool write_launched;
};

struct ixfd_sock_write_ctx {
    const char *data;
    size_t len_remain;
    ixfd_context_callback cb;
    const char *data_org;
    void *cb_args;
};

struct ixfd_sock *ixfd_commonsock_create(struct ixc_context *ctx);

void ixfd_commonsock_free(struct ixfd_sock *sock);

void ixfd_commonsock_tcp_createnbind(struct ixfd_sock *sock, const char *ip, unsigned short port);

void ixfd_commonsock_tcp_createnconnect(struct ixfd_sock *sock, const char *ip, unsigned short
    port, ixfd_context_callback cb_success, ixfd_connect_fail_callback cb_failed, void *args);

void ixfd_commonsock_tcp_listen(struct ixfd_sock *sock);

bool ixfd_commonsock_write(struct ixfd_conn_ctx *ctx, const char *data, size_t len,
                           ixfd_context_callback cb, void *args);

void ixfd_commonsock_set_bufread_len(struct ixfd_conn_ctx *conn, size_t len);

void ixfd_commonsock_close(struct ixfd_conn_ctx *ctx);

#endif //IOJXD_SOCK_HXX
