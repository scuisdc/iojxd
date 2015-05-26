//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/26.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_SANDBOX_HXX
#define IOJXD_SANDBOX_HXX

#include <stdint.h>
#include <unistd.h>

void ixut_reslimit(int resource, long val);
void ixut_reslimit_d(int resource, long soft, long hard);

int ixut_setuid(uid_t uid);
int ixut_setgid(uid_t uid);

#endif // IOJXD_SANDBOX_HXX
