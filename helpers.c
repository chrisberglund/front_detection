//
// Created by Christopher Berglund on 11/1/19.
//

#include "helpers.h"
void swap(int *a, int *b) {
    double t = *a;
    *a = *b;
    *b = t;
}


int partition(int *array, int left, int right, int pivot) {
    double pivotValue = array[pivot];
    swap(&array[pivot], &array[right]);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
        if (array[i] <= pivotValue) {
            swap(&array[i], &array[storeIndex]);
            storeIndex++;
        }
    }
    swap(&array[storeIndex], &array[right]);
    return storeIndex;
}

/**
 * Serial Quicksort implementation.
 */
void sort(int *array, int left, int right) {
    if (right > left) {
        int pivotIndex = left + (right - left) / 2;
        pivotIndex = partition(array, left, right, pivotIndex);
        sort(array, left, pivotIndex - 1);
        sort(array, pivotIndex + 1, right);
    }
}
