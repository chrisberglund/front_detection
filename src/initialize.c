/*
 * Functions for calculating the integer data values to be used in the single image edge detection algorithm
 */

#include "../include/initialize/initialize.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cayula.h"

/*
 * Function:  get_int_values
 * --------------------
 * Computes the mean data value for each bin and converts it to an integer value between 0 and 255.
 *
 * args:
 *      struct InData inData: contains the weighted sum and weight for each bin
 *      int *outData: pointer to the output array
 *      int *dataBins: pointer to an array containing the bin number for each data containing bin
 *      int nbins: number of data containing bins
 *      bool chlora: whether or not to use the log10 of the data values
 */
static void get_int_values(InData inData, int *outData, const int *dataBins, int nbins, bool chlora) {
    double *meanData = (double *) malloc(sizeof(double) * nbins);
    double maxValue = -999.;
    double minValue = 999.;

    for (int i = 0; i < nbins; i++) {
        if (inData.values[i] == FILL_VALUE) {
            meanData[i] = FILL_VALUE;
            outData[dataBins[i] - 1] = FILL_VALUE;
        } else {
            meanData[i] = chlora ? log10(inData.values[i] / inData.weights[i]) : inData.values[i] / inData.weights[i];
            if (meanData[i] < minValue)
                minValue = meanData[i];
            else if (meanData[i] > maxValue)
                maxValue = meanData[i];
        }
    }
    /*
     * Converts the mean data value to integers on a scale from 0 to 255
     */
    for (int i = 0; i < nbins; i++) {
        if (meanData[i] != FILL_VALUE) {
            double ratio = (meanData[i] + fabs(minValue)) / fabs(maxValue - minValue);
            outData[dataBins[i] - 1] = (int) floor(ratio * 255);
        }
    }
    free(meanData);
}


/*
 * Function:  initialize
 * --------------------
 * Fills an array with integer values of the input data with missing data filled with the fill value. Only bins in the
 * provided bin numbers are copied to the output array.
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
void initialize(InData inData, int *outData, int nbins, int nDataBins, int nTotalBins, int *dataBins, const int *bins,
               bool chlora) {
    int *data = (int *) malloc(sizeof(int) * nTotalBins);
    for (int i = 0; i < nTotalBins; i++) {
        data[i] = FILL_VALUE;
    }

    get_int_values(inData, data, dataBins, nDataBins, chlora);
    for (int i = 0; i < nbins; i++) {
        outData[i] = data[bins[i] - 1];
    }
    free(data);
}
