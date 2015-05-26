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

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {

    if (argc < 2) {
        printf("Usage: iojxd <script file>\n");
        exit(0);
    }

    ixc_context *ctx = ixc_create_context();

    ixlu_dofile(ctx->state, argv[1]);

    ev_run(ctx->evl, 0);

    ixc_destroy_context(ctx);
    ctx = NULL;
    return 0;
}
