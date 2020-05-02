/*
 * Functions for calculating the integer data values to be used in the single image edge detection algorithm
 */

#include "initialize.h"
#include <stdlib.h>
#include <math.h>
#include "cayula.h"


/*
 * Function:  initialize
 * --------------------
 * Fills an array with integer values of the input data ranging from 0 to 255 with missing data filled with the fill value.
 *
 * args:
 *      double *in_data: pointer to an array containing the input data
 *      int *out_data: pointer to the output array
 *      int nbins: number of bins in the area of interest
 *      int ndata_bins: number of data containing bins
 *      int ntotal_bins: total number of bins in the binning scheme
 *      int *data_bins: pointer to an array containing the bin number for each data containing bin
 *      int *bins: pointer to an array containing the bin number for all bins in the area of interest
 */

void initialize(double *in_data, int *out_data, int nbins, int ndata_bins, int ntotal_bins, int *data_bins, const int *bins) {

    int i = 0, j = 0;
    double min_value = 999.;
    double max_value = -999.;
    for (int i = 0; i < ndata_bins; i++) {
        if (in_data[i] < min_value) {
            min_value = in_data[i];
        } else if (in_data[i] > max_value) {
            max_value = in_data[i]
        }
    }

    while (i < ndata_bins || j < nbins) {
        if (a[i] == b[j]) {
            double ratio = (in_data[i] + fabs(min_value)) / fabs(max_value - min_value);
            out_data[bins[j] - 1] = (int) (ratio * 255);
            if (i < ndata_bins) ++i;
            if (j < nbins) ++j;
        } else if (a[i] > b[j] && j < 12) {
            out_data[bins[j] - 1] = FILL_VALUE;
            ++j;
        } else {
            ++i;
        }
    }
}
