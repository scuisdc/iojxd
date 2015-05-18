//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/19.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include <assert.h>

#include <util.hxx>

void test_cycqueue() {
    ixut_cycqueue *queue = ixut_cycqueue_create();
    assert(queue != NULL);
    assert(ixut_cycqueue_empty(queue));

    ixut_cycqueue_enqueue(queue, (void *) 1);
    assert(!ixut_cycqueue_empty(queue));

    assert(ixut_cycqueue_dequeue(queue) == (void *)1);
    assert(ixut_cycqueue_empty(queue));

    ixut_cycqueue_enqueue(queue, (void *) 2);
    ixut_cycqueue_enqueue(queue, (void *) 3);
    ixut_cycqueue_enqueue(queue, (void *) 4);
    ixut_cycqueue_enqueue(queue, (void *) 5);

    assert(ixut_cycqueue_dequeue(queue) == (void *) 2);
    assert(ixut_cycqueue_dequeue(queue) == (void *) 3);
    assert(ixut_cycqueue_dequeue(queue) == (void *) 4);

    ixut_cycqueue_enqueue(queue, (void *) 6);

    assert(ixut_cycqueue_dequeue(queue) == (void *) 5);
    assert(ixut_cycqueue_dequeue(queue) == (void *) 6);
    assert(ixut_cycqueue_empty(queue));

    ixut_cycqueue_free(queue);
    queue = NULL;
}

int main() {

    test_cycqueue();

    return 0;
}