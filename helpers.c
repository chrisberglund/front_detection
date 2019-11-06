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

int max(const double *arr, int length) {
    int maxValue = 0;
    for (int i = 0; i < length; i++) {
        if (arr[i] > maxValue)
            maxValue = arr[i];
    }
    return maxValue;
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


/**
 * Finds the bin number north or south of given bin by given distance. This function determines
 * the neighboring bin number by using the ratio between the number of bins in a row difference between the bin numbers
 * of the first bin in a row and the bin of interest. Rounding to the nearest bin number is done.
 * @param bin bin number of the bin of interest
 * @param row row: row number of the bin
 * @param distance number of rows away from the bin of interest to look for neighbor with positive values for north and
 * negative numbers for south
 * @param nBinsInRow pointer to array containing the number of bins in each row
 * @param basebins pointer to array containing the bin number of the first bin of each row
 * @return
 */
int getNeighborBin(int bin, int row, int distance, const int *nBinsInRow, const int *basebins) {
    int neighbor;
    double ratio;
    ratio = (bin - basebins[row]) / (double) nBinsInRow[row];
    neighbor = ((int) round(ratio * nBinsInRow[row + distance]) + basebins[row + distance]);
    return neighbor;
}



/**
 * Creates a n*n subset of a set of bins centered around a specified bin.
 * @param bin bin to center window on
 * @param row row the center bin is in
 * @param nrows total number of rows in world
 * @param width dimension of window. it must be an odd number
 * @param data data to subset
 * @param nBinsInRow pointer to an array containing the number of bins in each row
 * @param basebins pointer to an array containing the bin number of the first bin of each row
 * @param window pointer to nxn 2-D array to write data values to
 */
bool getWindow(int bin, int row, int width, const int *data, const int *nBinsInRow,
               const int *basebins, int *window, int fillValue, bool fill) {
    int maxDistance = (int) round((width - 1.0) / 2);
    int nsNeighbor;
    for (int i = 0; i < width; i++) {
        nsNeighbor = getNeighborBin(bin, row, i - maxDistance, nBinsInRow, basebins);
        int neighborRow = row+i-maxDistance;
        for (int j = 0; j < width; j++) {
            if (nsNeighbor + (j - maxDistance) < basebins[neighborRow]) {
                window[i * width + j] = data[basebins[neighborRow] + nBinsInRow[neighborRow] + (j - maxDistance)];
            } else if (nsNeighbor + (j - maxDistance) - 1 >= basebins[neighborRow + 1]) {
                window[i * width + j] = data[basebins[neighborRow] + (j - maxDistance) - 1];
            } else {
                window[i * width + j] = data[nsNeighbor + (j - maxDistance) - 1];
            }
        }
    }

    for (int i = 0; i < width * width; i++) {
        if (window[i] == -999 && fill){
            return false;
        }
    }

    return true;
}