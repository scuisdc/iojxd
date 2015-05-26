//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/26.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include <stdio.h>

static void handle_sigsys(int const nr, siginfo_t * const info, void * const context) {
    exit(0);
}

static void install_handler() __attribute__((constructor));

void install_handler()
{
    struct sigaction sa;
    sa.sa_sigaction = handle_sigsys;
    sa.sa_flags = SA_SIGINFO;

    if (sigprocmask(0, 0, &sa.sa_mask) != 0)
        abort();

    if (sigaction(SIGSYS, &sa, 0) != 0)
        abort();
}

int main() {

    void *t = malloc(1024*1024*128);
    memset(t, 0, 1024*1024*128);

    printf("hahaha\n");

    for (size_t i = 0; i < 3; i++) {
        sleep(1);
    }

    return 0;
}