/*
* Functions for applying the median filter to an array of bins using sliding 3x3 window
*/
#include <math.h>
#include "filter.h"
#include "helpers.h"
#include "cayula.h"

#define BIN_SORT(a,b) { if ((a)>(b)) BIN_SWAP((a),(b)); }
#define BIN_SWAP(a,b) { int temp=(a);(a)=(b);(b)=temp; }

/*
 * Function:  median9
 * --------------------
 * Uses a median network to obtain the median of array of 9 elements.
 *
 * args:
 *      int *a: pointer to the array from which to get the median. The array must be exactly 9 elements long.
 */
static int median9(int * p) {
    BIN_SORT(p[1], p[2]);  BIN_SORT(p[4], p[5]);  BIN_SORT(p[7], p[8]);
    BIN_SORT(p[0], p[1]);  BIN_SORT(p[3], p[4]);  BIN_SORT(p[6], p[7]);
    BIN_SORT(p[1], p[2]);  BIN_SORT(p[4], p[5]);  BIN_SORT(p[7], p[8]);
    BIN_SORT(p[0], p[3]);  BIN_SORT(p[5], p[8]);  BIN_SORT(p[4], p[7]);
    BIN_SORT(p[3], p[6]);  BIN_SORT(p[1], p[4]);  BIN_SORT(p[2], p[5]);
    BIN_SORT(p[4], p[7]);  BIN_SORT(p[4], p[2]);  BIN_SORT(p[6], p[4]);
    BIN_SORT(p[4], p[2]);  return(p[4]);
}


#define MIN(a,b) ((int)(fmin(a,b)))
#define MAX(a,b) ((int)(fmax(a,b)))
#define SWAP(i,j) do { int s = MIN(a##i,a##j); int t = MAX(a##i,a##j); a##i = s; a##j = t; } while (0)

/*
 * Function:  sort9
 * --------------------
 * Uses a sorting network to sort the given array of 9 elements in ascending order
 *
 * args:
 *      int *a: pointer to the array to sort. The array must be exactly 9 elements long. The array is sorted in place
 */
static void sort9 (int *a) {
    int a0, a1, a2, a3, a4, a5, a6, a7, a8;
    a0=a[0];a1=a[1];a2=a[2];a3=a[3];a4=a[4];a5=a[5];a6=a[6];a7=a[7];a8=a[8];

    SWAP (0, 1);   SWAP (3, 4);   SWAP (6, 7);   SWAP (1, 2);   SWAP (4, 5);
    SWAP (7, 8);   SWAP (0, 1);   SWAP (3, 4);   SWAP (6, 7);   SWAP (0, 3);
    SWAP (3, 6);   SWAP (0, 3);   SWAP (1, 4);   SWAP (4, 7);   SWAP (1, 4);
    SWAP (5, 8);   SWAP (2, 5);   SWAP (5, 8);   SWAP (2, 4);   SWAP (4, 6);
    SWAP (2, 4);   SWAP (1, 3);   SWAP (2, 3);   SWAP (5, 7);   SWAP (5, 6);

    a[0]=a0;a[1]=a1;a[2]=a2;a[3]=a3;a[4]=a4;a[5]=a5;a[6]=a6;a[7]=a7;a[8]=a8;
}


/*
 * Function:  medianN
 * --------------------
 * Determines the median of the given array containing fill values. Provided there is at least one valid value in the
 * array, the median is determined using all valid values in the array. If removing fill values results in an array of
 * even length, the median is the average of the two middle values rounded up to nearest integer.
 *
 * args:
 *      int *p: pointer to the input array
 *      int n_invalid: number of fill values contained in the array
 *
 * returns:
 *      int: the median of the array after excluding fill values
 */
static int medianN(int * p, int n_invalid) {
    if (n_invalid == 9) return FILL_VALUE;
    sort9(p);
    int n = 9 - n_invalid;
    int arr[n];
    for (int i = n_invalid; i < 9; i++) {
        arr[i - n_invalid] = p[i];
    }
    return n & 1 ? arr[(n - 1) >> 1] : (arr[n >> 1] + arr[(n >> 1) - 1] + 1) >> 1;
}

/*
 * Function:  median_filter
 * --------------------
 * Applies a median filter with a fixed 3x3 kernel with the median determined using a sorting network. For the case of
 * a 3x3 kernel and 256 possible values, this method is generally faster than a histogram based approach. If the kernel
 * contains fill values, the median is determined using however many valid values there are in the kernel.
 *
 * args:
 *      int *data: pointer to array containing the data to be filtered
 *      int *filtered_data: pointer to output array
 *      int nbins: the number of bins in the binning scheme
 *      int nrows: the number of rows in the binning scheme,
 *      int *nbins_in_row: pointer to an array the number of bins in each row
 *      int *basebins: pointer to an array containing the bin number of the first bin in each row
 */
void median_filter(int *data, int *filtered_data, int nbins, int nrows, int *nbins_in_row, int *basebins) {
    int window[9];
    int last_row = nrows - 1;
    /*
     * Fill in the first and last row with fill values. Then iterate through the remaining rows while filling the
     * first and last bin of each row with fill values and the rest with the result of the median filter.
     */
    for (int i = 0; i < basebins[0] + nbins_in_row[0]; i++) filtered_data[i] = FILL_VALUE;
    for (int i = basebins[last_row]; i < basebins[last_row] + nbins_in_row[last_row]; i++) filtered_data[i] = FILL_VALUE;
    for (int i = 1; i < nrows - 1; i++) {
        filtered_data[basebins[i]] = FILL_VALUE;
        filtered_data[basebins[i] + nbins_in_row[i] - 1] = FILL_VALUE;
        for (int j = basebins[i] + 1; j < basebins[i] + nbins_in_row[i] - 1; j++) {
            if (data[j] == FILL_VALUE) {
                filtered_data[j] = FILL_VALUE;
            } else {
                int n_invalid = get_window(j, i, 3, data, nbins_in_row, basebins, window);
                filtered_data[j] = n_invalid == 0 ? median9(window) : medianN(window, n_invalid);
            }
        }
    }
}