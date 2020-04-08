//
// Created by Christopher Berglund on 11/1/19.
//
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"

void swap(int *a, int *b) {
    int t = *a;
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

/**
 * Finds the index of the closest value in a sorted array
 * @param arr sorted array
 * @param l index of the left most value of interest
 * @param r index of the right most value of interest
 * @param x value to search for
 * @return index of the closest value in the array
 */
int findClosestValue(int arr[], int l, int r, int x) {
    int mid = l + (r - l) / 2;
    if (r >= l) {
        if (arr[mid] == x)
            return mid;

        if (arr[mid] > x)
            return findClosestValue(arr, l, mid - 1, x);

        return findClosestValue(arr, mid + 1, r, x);
    }

    return mid;
}

int median(const int *arr, int length) {
    int *copy = (int *) malloc(length * sizeof(int));
    int median;
    bool isSorted = true;
    for (int i = 0; i < length; i++) {
        if (i > 0) {
            if (arr[i] < arr[i-1]) isSorted = false;
        }
        copy[i] = arr[i];
    }
    if (!isSorted)
        sort(copy, 0, length - 1);

    median = length % 2 == 0 ? (copy[((length - 1) / 2) - 1] + copy[(length - 1) / 2]) / 2 : copy[(int) round(
            (length - 1) / 2.0)];

    free(copy);
    return median;
}


/*
 * Function:  get_window
 * --------------------
 * Selects a window of data values centered on a given bin with a given width. Window must not cross the edge of the
 * boundaries of the original data map. This function selects the north and south adjacent bins using the ratio
 * between the position of the bin in its row and the total number of bins in the row.
 *
 * args:
 *      int bin: bin number of the center bin in the window. If the width of the window is even, then this is the upper
 *      left bin in the center. Bin numbers begin with 1.
 *      int row: the row number of the center bin. Row numbers begin with 0.
 *      int width: the width of the desired window. Width must be a positive number greater than 2.
 *      int *n_bins_in_row: pointer to an array containing the number of bins in each row
 *      int *basebins: pointer to an array containing the bin number for the first bin in each row
 *      int *window: pointer to output array for the window. The array should be of width * width length
 */
void get_window(int bin, int row, int width, const int *data, const int *n_bins_in_row,
                const int *basebins, int window[]) {
    int column_neighbor;
    double ratio = (bin - basebins[row]) / (double) n_bins_in_row[row];
    if (width % 2 == 0) {
        int max_distance = width >> 1;
        int currentRow = row - max_distance + 1;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = ((int) round(ratio * n_bins_in_row[currentRow]) + basebins[currentRow]);
            for (int j = 0; j < width; j++) {
                window[i + j] = data[column_neighbor + j - max_distance];
            }
            currentRow++;
        }
    } else {
        int max_distance = (width - 1) >> 1;
        int current_row = row - max_distance;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = ((int) round(ratio * n_bins_in_row[current_row]) + basebins[current_row]);
            for (int j = 0; j < width; j++) {
                window[i + j] = data[column_neighbor + j - max_distance - 1];
            }
            current_row++;
        }
    }
}