//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/18.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#ifndef IOJXD_UTIL_HXX
#define IOJXD_UTIL_HXX

#include <string>

struct ixut_cycqueue {
    void *data;
    ixut_cycqueue *next;
};

ixut_cycqueue *ixut_cycqueue_create();

void ixut_cycqueue_free(ixut_cycqueue *entry);

void ixut_cycqueue_enqueue(ixut_cycqueue *entry, void *data);

void *ixut_cycqueue_dequeue(ixut_cycqueue *entry);

void *ixut_cycqueue_front(ixut_cycqueue *entry);

int ixut_cycqueue_empty(ixut_cycqueue *entry);

std::string directory_name(const std::string& src, char slash = '/');

#endif //IOJXD_UTIL_HXX
