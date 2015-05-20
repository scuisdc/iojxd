//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/19.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include "util.hxx"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

ixut_cycqueue *ixut_cycqueue_create() {
    ixut_cycqueue *entry = (ixut_cycqueue *) malloc(sizeof(*entry));
    ixut_cycqueue *place_holder = (ixut_cycqueue *) malloc(sizeof(*place_holder));

    memset(&entry->data, 0xF, sizeof(entry->data));
    memset(&place_holder->data, 0xF, sizeof(place_holder->data));

    entry->next = place_holder;
    place_holder->next = place_holder;

    return entry;
}

void ixut_cycqueue_free(ixut_cycqueue *entry) {
    assert(entry != NULL);
    assert(entry->next != NULL);

    while (!ixut_cycqueue_empty(entry)) {
        ixut_cycqueue_dequeue(entry);
    }
    free(entry->next);
    free(entry);
}

void ixut_cycqueue_enqueue(ixut_cycqueue *entry, void *data) {
    assert(entry != NULL);
    assert(entry->next != NULL);

    ixut_cycqueue *new_node = (ixut_cycqueue *) malloc(sizeof(*new_node));
    new_node->data = data;

    new_node->next = entry->next->next;
    entry->next->next = new_node;
    entry->next = new_node;
}

void *ixut_cycqueue_dequeue(ixut_cycqueue *entry) {
    assert(entry != NULL);
    assert(entry->next != NULL);

    ixut_cycqueue *ret_node = entry->next->next->next;
    entry->next->next->next = ret_node->next;

    if (ret_node == entry->next) {
        entry->next = ret_node->next; }

    void *ret = ret_node->data;
    free(ret_node);

    return ret;
}

void *ixut_cycqueue_front(ixut_cycqueue *entry) {
    assert(entry != NULL);
    assert(entry->next != NULL);
    assert(entry->next->next != entry->next);

    return entry->next->next->next->data;
}

int ixut_cycqueue_empty(ixut_cycqueue *entry) {
    assert(entry != NULL);
    assert(entry->next != NULL);

    return (entry->next == entry->next->next);
}
