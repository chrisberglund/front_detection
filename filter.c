/**
 * Functions for applying the contextual median filter to an array of bins using sliding 5x5 and 3x3 windows
 */

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "filter.h"
#include "helpers.h"

/**
 * Checks if element at given position in an array is either the maximum or minimum value
 * @param idx index value of the element check
 * @param arr array to check for max or min values
 * @param length length of the array
 * @return 1 if element is max or min, 0 if not
 */
int isExtrema(int idx, const int *arr, int length) {
    bool isMax = false;
    bool isMin = false;
    bool isEqual = false;
    double x = arr[idx];

    for (int i = 0; i < length; i++) {
        if (i == idx) {
            continue;
        }
        if (x > arr[i]) {
            isMax = true;
        } else if (x < arr[i]) {
            isMin = true;
        } else if (x == arr[i]) {
            isEqual = true;
        }
    }
    return (isMax != isMin) && (!isEqual);
}

/**
 * Determines if the center of a square 2-D window is a maximum or minimum along NW-SE, NE-SW, N-S, and E-W axes
 * @param width width of the 2-D array
 * @param window pointer to 2-D array containing data values
 * @return  1 if center is an extrema and 0 if it is not
 */
int isWindowExtrema(int width, const int *window) {
    int *slice = (int *) malloc(sizeof(int) * width);
    bool extrema = false;
    int center = (int) (width - 1) / 2;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < width; j++) {
            switch (i) {
                //Northwest to Southeast slice
                case 0:
                    slice[j] = window[j * width + j];
                    break;
                    //North to South Slice
                case 1:
                    slice[j] = window[j*width + center];
                    break;

                    //Northeast to Southwest Slice
                case 2:
                    slice[j] = window[j * width + width - j -1];
                    break;

                    //West to East Slice
                case 3:
                    slice[j] = window[center * width + j];
                    break;
                default:
                    break;
            }
        }
        if (i == 0)
            extrema = isExtrema(center, slice, width);
        extrema = extrema && isExtrema(center, slice, width);
    }
    free(slice);
    return extrema;
}


/**
 * Takes a nxn window and applies a median filter to the center value if it is the maximum or minimum value
 * in a n-2xn-2 window, but not a maximum or minimum value in 4 linear slices through the 5x5 window.
 * @param window pointer to a 2-D array containing the 5x5 window
 * @param width width of the window
 * @return center pixel value resulting from the contextual median filter
 */
int applyMedianFilter(int *window, int width) {
    int mdn = median(window, (width * width));
    return mdn;
}

/**
 * Applies a contextual 3x3 median filter to all bin values with sufficient padding. The function iterates through the
 * bins with a 5x5 moving window. If the center pixel in a window is a maximum or minimum value in a 3x3 subwindow but
 * not in 4 linear slices of the 5x5 window, it is replaces with the median value of the 3x3 window.
 * @param bins pointer to array of nbins length containing the bin numbers for all the bins in the world
 * @param data pointer to an array of nbins length containing bin data values to use in median filter
 * @param filteredData pointer to an array of nbins length to write filtered values to
 * @param nbins number of bins in the world
 * @param nrows number of bin containing rows
 * @param nBinsInRow pointer to an array of nrows length containing the number of bins in each row
 * @param basebins pointer to an array of nrows length containing the bin number of the first bin in each row
 */
void medianFilter(int *bins, int *data, int *filteredData, int nbins, int nrows,
                            int *nBinsInRow, int *basebins, int fillValue) {
    int *threeWindow = (int *) malloc(sizeof(int) * 9);
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        int value = data[i];

        if (i == basebins[row] + nBinsInRow[row]) {
            row++;
        }

        if (row < 2 || row > nrows - 3) {
            filteredData[i] = fillValue;
            continue;
        }
        getWindow(bins[i], row, 3, data, nBinsInRow, basebins, threeWindow, fillValue, false);
        int isThreePeak = isWindowExtrema(3, threeWindow);
        if (isThreePeak) {
            value = applyMedianFilter(threeWindow, 3);
        }
        filteredData[i] = value;
        if (i == basebins[row] + nBinsInRow[row] - 1) {
            row++;
        }
    }
    free(threeWindow);
}