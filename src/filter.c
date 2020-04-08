**
* Functions for applying the contextual median filter to an array of bins using sliding 5x5 and 3x3 windows
*/

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "filter.h"
#include "helpers.h"

#define BIN_SORT(a,b) { if ((a)>(b)) BIN_SWAP((a),(b)); }
#define BIN_SWAP(a,b) { int temp=(a);(a)=(b);(b)=temp; }

/*
 * Function: median9
 * Returns the median value of the given 9 element long array.
 * args:
 *      int *p: pointer to an array. Must be 9 elements long
 * returns:
 *      int: median of the provided array
 */
int median9(int * p) {
    BIN_SORT(p[1], p[2]) ; BIN_SORT(p[4], p[5]) ; BIN_SORT(p[7], p[8]) ;
    BIN_SORT(p[0], p[1]) ; BIN_SORT(p[3], p[4]) ; BIN_SORT(p[6], p[7]) ;
    BIN_SORT(p[1], p[2]) ; BIN_SORT(p[4], p[5]) ; BIN_SORT(p[7], p[8]) ;
    BIN_SORT(p[0], p[3]) ; BIN_SORT(p[5], p[8]) ; BIN_SORT(p[4], p[7]) ;
    BIN_SORT(p[3], p[6]) ; BIN_SORT(p[1], p[4]) ; BIN_SORT(p[2], p[5]) ;
    BIN_SORT(p[4], p[7]) ; BIN_SORT(p[4], p[2]) ; BIN_SORT(p[6], p[4]) ;
    BIN_SORT(p[4], p[2]) ; return(p[4]) ;
}

/*
 * Function:  median_filter
 * --------------------
 * Applies a median filter with a fixed 3x3 kernel with the median determined using a sorting network. For the case of
 * a 3x3 kernel and 256 possible values, this method is generally faster than a histogram based approach.
 *
 * args:
 *      int *data: pointer to array containing the data to be filtered
 *      int *filtered_data: pointer to output array
 *      int nbins: the number of bins in the binning scheme
 *      int nrows: the number of rows in the binning scheme,
 *      int *nbins_in_row: pointer to an array the number of bins in each row
 *      int *basebins: pointer to an array containing the bin number of the first bin in each row
 */
void median_filter(int *data, int *filtered_data, int nbins, int nrows,
                   int *nbins_in_row, int *basebins) {
    int *window[9];
    int row = 0;
    for (int i = 0; i < nbins; i++) {

        if (i == basebins[row] + nbins_in_row[row]) {
            row++;
        }

        if (row < 2 || row > nrows - 3) {
            continue;
        }
        get_window(i + 1, row, 3, data, nbins_in_row, basebins, window);
        filtered_data[i] = median9(window);
    }
}