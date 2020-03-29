#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "prefilter.h"
#include "helpers.h"

typedef struct coordinates {
    double latitude;
    double longitude;
} coordinates;

/**
 * Determines the latitude and longitude values for the specified bin
 * @param bin bin number for the bin of interest
 * @param nBinsInRow pointer to an array containing the number of bins in the row
 * @param latrows pointer to an array containing the latitudes of each row
 * @param basebins pointer to an array containing the bin number for the first bin of each row
 * @param nrows the number of rows in the binning scheme
 * @param coords struct to write latitude and longitude values to
 */
void bin2latlon(int bin, const int *nBinsInRow, const double *latrows, int *basebins, int nrows,
                coordinates *coords) {
    if (bin < 1) {
        bin = 1;
    }
    int row = findClosestValue(basebins, 0, nrows, bin) - 1;
    double clat = latrows[row];
    double clon;
    clon = 360.0 * (bin - basebins[row] + 0.5) / nBinsInRow[row] - 180.0;
    coords->latitude = clat;
    coords->longitude = clon;
}

/*
 * Function:  getLatLon
 * --------------------
 * Calculates the latitude and longitude values for each bin
 *
 * args:
 *      double *lats: pointer to an output array for latitude values
 *      double *lons: pointer to an output array for longitude values
 *      int *outRows: pointer to an output array for the row number for each bin
 *      int *outBins: pointer to an output array for the bin number for each bin
 *      int nrows: the number of rows in the binning scheme
 *      int *dataBins: pointer to an array containing the bin number for each data containing bin
 *      int nbins: number of bins in the binning scheme
 */
void getLatLon(double *lats, double *lons, int *outRows, int *outBins, int nrows, int nbins) {
    double *latrows = (double *) malloc(sizeof(double) * nrows);
    int *nBinsInRow = (int *) malloc(nrows * sizeof(int));
    int *basebins = (int *) malloc(nrows * sizeof(int));
    /*
     * Get the latitude value for each row and the bin number of the first bin in each row
     */
    for (int i = 0; i < nrows; ++i) {
        latrows[i] = ((i + 0.5) * 180.0 / nrows) - 90;
        nBinsInRow[i] = (int) (2 * nrows * cos(latrows[i] * M_PI / 180.0) + 0.5);
        if (i == 0) {
            basebins[i] = 1;
        } else {
            basebins[i] = basebins[i - 1] + nBinsInRow[i - 1];
        }
    }
    coordinates *coords;
    coords = (coordinates *) malloc(sizeof(coordinates));
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        if (row + 1 < nrows && i >= basebins[row + 1]) {
            row++;
        }
        outBins[i] = i + 1;
        outRows[i] = row;
        bin2latlon(outBins[i], nBinsInRow, latrows, basebins, nrows, coords);
        lats[i] = coords->latitude;
        lons[i] = coords->longitude;
    }

    free(coords);
    free(latrows);
}

/**
 * Creates an array containing all bins in the binning scheme including empty bins and assigns a latitude and longitude
 * value to each bin. Empty bins will be assigned
 * a fill value.
 * @param totalBins the total number of bins in the binning scheme
 * @param nDataBins the number of data containing bins
 * @param nrows the number of rows in the binning scheme
 * @param dataBins pointer an array containing the bin number of each data containing bin
 * @param fillValue value to fill empty bins with
 * @param outBins pointer to an empty array of length totalBins to write all bin numbers to from 1 to totalBins
 * @param inData pointer to an array containing the weighted sum values for each bin
 * @param weights pointer to an array containing the weights for each bin
 * @param lats pointer to an empty array of totalBins length to write latitude values to
 * @param lons pointer to an empty array of totalBins length to write longitude values to
 * @param nBinsInRow pointer to an empty array of nrows length to write the number of bins in each row to
 * @param basebins pointer to an empty array of nrows length to write the bin number of the first bin of each row to
 * @param outData pointer to an empty array of totalBins length to write the arithmetic mean of data values to. Data
 * values for empty bins will be the given fill value
 * @param chlora if the provided data is chlorophyll concentration, the output data value will be the natural
 * lograithm of the mean data value for each bin
 */
void createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, int fillValue,
                        int *outBins, const double *inData, const double *weights,
                        double *lats, double *lons, int *nBinsInRow, int *basebins,
                        int *outData, bool chlora) {

    double *meanData = (double *) malloc(sizeof(double) * nDataBins);


    double maxValue = -999.;
    double minValue = 999;

    for (int i = 0; i < nDataBins; i++) {
        if (inData[i] == fillValue) {
            meanData[i] = fillValue;
            continue;
        }
        meanData[i] = chlora ? log10(inData[i] / weights[i]) : inData[i] / weights[i];
        if (meanData[i] < minValue)
            minValue = meanData[i];
        if (meanData[i] > maxValue)
            maxValue = meanData[i];
    }


    for (int i = 0; i < nDataBins; i++) {
        if (meanData[i] == fillValue)
            outData[dataBins[i] - 1] = fillValue;
        else {
            double ratio = (meanData[i] + fabs(minValue)) / fabs(maxValue - minValue);
            outData[dataBins[i] - 1] = (int) floor(ratio * 255);
        }
    }
    free(meanData);
}