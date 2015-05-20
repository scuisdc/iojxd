//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/20.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_CHILDPROCESS_HXX
#define IOJXD_CHILDPROCESS_HXX

#include <sys/types.h>

typedef void (*forked_process_callback)(void *args);

pid_t ixut_fork(forked_process_callback cb, void *args);

#endif // IOJXD_CHILDPROCESS_HXX
