//
// Created by Christopher Berglund on 11/1/19.
//
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"
#include "cayula.h"

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
int getNeighborBin(struct binLoc bin, int distance, struct binRows basebins) {
    int neighbor;
    double ratio;
    ratio = (bin.bin - basebins.basebins[bin.row]) / (double) basebins.nbins[bin.row];
    neighbor = ((int) round(ratio * basebins.nbins[bin.row + distance]) + basebins.basebins[bin.row + distance]);
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
 * @param rows pointer to an array containing the bin number of the first bin of each row
 * @param window pointer to nxn 2-D array to write data values to
 */

bool getWindow(struct binLoc bin, int width, const int *data, struct binRows rows, int *window) {
    int maxDistance;
    int nsNeighbor;
    int neighborRow;
    if (width % 2 == 0) {
        maxDistance = width / 2;
        for (int i = 0; i < width; i++) {
            if (i < width) {
                nsNeighbor = getNeighborBin(bin, i - maxDistance + 1, rows);
                neighborRow = bin.row + i - maxDistance + 1;
            } else {
                nsNeighbor = getNeighborBin(bin, i - maxDistance, rows);
                neighborRow = bin.row + i - maxDistance;
            }
            for (int j = 0; j < width; j++) {
                if (j < width) {
                    if (nsNeighbor + (j - maxDistance + 1) < rows.basebins[neighborRow]) {
                        window[i * width + j] = data[rows.basebins[neighborRow] + rows.nbins[neighborRow] + (j - maxDistance + 1)];
                    } else if (nsNeighbor + (j - maxDistance + 1) - 1 >= rows.basebins[neighborRow] + rows.nbins[neighborRow]) {
                        window[i * width + j] = data[rows.basebins[neighborRow] + (j - maxDistance + 1) - 1];
                    } else {
                        window[i * width + j] = data[nsNeighbor + (j - maxDistance + 1) - 1];
                    }
                } else {
                    if (nsNeighbor + (j - maxDistance) < rows.basebins[neighborRow]) {
                        window[i * width + j] = data[rows.basebins[neighborRow] + rows.nbins[neighborRow] + (j - maxDistance)];
                    } else if (nsNeighbor + (j - maxDistance) - 1 >= rows.basebins[neighborRow + 1]) {
                        window[i * width + j] = data[rows.basebins[neighborRow] + (j - maxDistance) - 1];
                    } else {
                        window[i * width + j] = data[nsNeighbor + (j - maxDistance) - 1];
                    }
                }
            }
        }
    } else {
        maxDistance = (int) round((width - 1.0) / 2);
        for (int i = 0; i < width; i++) {
            nsNeighbor = getNeighborBin(bin, i - maxDistance,  rows);
            neighborRow = bin.row + i - maxDistance;
            for (int j = 0; j < width; j++) {
                if (nsNeighbor + (j - maxDistance) < rows.basebins[neighborRow]) {
                    window[i * width + j] = data[rows.basebins[neighborRow] + rows.nbins[neighborRow] + (j - maxDistance)];
                } else if (nsNeighbor + (j - maxDistance) - 1 >= rows.basebins[neighborRow + 1]) {
                    window[i * width + j] = data[rows.basebins[neighborRow] + (j - maxDistance) - 1];
                } else {
                    window[i * width + j] = data[nsNeighbor + (j - maxDistance) - 1];
                }
            }
        }
    }

    for (int i = 0; i < width * width; i++) {
        if (window[i] == FILL_VALUE){
            return false;
        }
    }

    return true;
}