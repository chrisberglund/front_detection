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
 * Function:  getWindow
 * --------------------
 * Selects a window of data values centered on a given bin with a given width. 
 *
 * args:
 *      struct InData inData: contains the weighted sum and weight for each bin
 *      int *outData: pointer to the output array
 *      int nbins: number of bins in the area of interest
 *      int nDataBins: number of data containing bins
 *      int nTotalBins: total number of bins in the binning scheme
 *      int *dataBins: pointer to an array containing the bin number for each data containing bin
 *      int *bins: pointer to an array containing the bin number for all bins in the area of interest
 *      bool chlora: whether or not to use the log10 of the data values
 */
void getWindow(int bin, int row, int width, const int *data, const int *nBinsInRow,
               const int *basebins, int *window) {
    int nsNeighbor;
    double ratio = (bin - basebins[row]) / (double) nBinsInRow[row];
    if (width % 2 == 0) {
        int maxDistance = width >> 1;
        int currentRow = row - maxDistance + 1;
        for (int i = 0; i < width * width; i += width) {   //Iterate through the first bins of each row in window
            nsNeighbor = ((int) round(ratio * nBinsInRow[currentRow]) + basebins[currentRow]);
            for (int j = 0; j < width; j++) {
                window[i + j] = data[nsNeighbor + j - maxDistance];
            }
            currentRow++;
        }
    } else {
        int maxDistance = (width - 1) >> 1;
        int currentRow = row - maxDistance;
        for (int i = 0; i < width * width; i += width) {   //Iterate through the first bins of each row in window
            nsNeighbor = ((int) round(ratio * nBinsInRow[currentRow]) + basebins[currentRow]);
            for (int j = 0; j < width; j++) {
                window[i + j] = data[nsNeighbor + j - maxDistance - 1];
            }
            currentRow++;
        }
    }
}