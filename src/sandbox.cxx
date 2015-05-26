//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/26.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "sandbox.hxx"

#include <stdint.h>
#include <sys/resource.h>
#include <unistd.h>

#include <stdio.h>
#include <stdexcept>

void ixut_reslimit_d(int resource, long soft, long hard) {
    rlimit rl = { 0, 0 };
    rl.rlim_cur = soft, rl.rlim_max = hard;
    if (setrlimit(resource, &rl) != 0)
        printf("setrlimit() failed: %d %ld %ld.\n", resource, soft, hard);
}

void ixut_reslimit(int resource, long val) {
    return ixut_reslimit_d(resource, val, val);
}

int ixut_setuid(uid_t uid) {
    return setuid(uid); }

int ixut_setgid(uid_t uid) {
    return setgid(uid); }
