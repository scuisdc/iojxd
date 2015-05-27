//
// Created by secondwtq <lovejay-lovemusic@outlook.com> 2015/05/27.
// Copyright (c) 2015 SCU ISDC All rights reserved.
//
// This file is part of ISDCNext.
//
// We have always treaded the borderland.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE (1024*1024)

int main() {
    printf("sort_test started, initing rand ...\n");
    srand(time(0));
    int *data = (int *) malloc(sizeof(int) * DATA_SIZE);
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand();
    }
    printf("data generated, start sorting ...\n");
    qsort(data, DATA_SIZE, sizeof(int), [] (const void *x, const void *y) {
        int x_ = *(const int *) x, y_ = *(const int *) y;
        if (x_ < y_) return -1;
        else if (x_ > y_) return 1;
        else return 0;
    });
    printf("completed.\n");

    return 0;
}
