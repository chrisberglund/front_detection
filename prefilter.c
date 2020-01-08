#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "prefilter.h"
#include "helpers.h"

struct coordinates {
    double latitude;
    double longitude;
};

struct node {
    struct node *next;
    int bin;
    int firstBin;
    int lastBin;
};

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
                struct coordinates *coords) {
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

/**
 * Takes a set of latitudes and longitudes and finds the bins that are inside provided minimum and maximum latitude
 * and longitude values.
 * @param lats pointer to an array containing all latitude values in dataset
 * @param lons pointer to an array containing all longitude values in dataset
 * @param data pointer to an array containing all data
 * @param nbins number of bins in dataset
 * @param outBins pointer to a pointer to be assigned to an array containing all the bins inside the minimum and max values
 * @param outLats pointer to a pointer to be assigned to an array containing all the lats inside the minimum and max values
 * @param outLons pointer to a pointer to be assigned to an array containing all the lons inside the minimum and max values
 * @param outData pointer to a pointer to be assigned to an array containing all the data inside the min and max values
 * @param minLat minimum latitude value
 * @param maxLat maximum latitude value
 * @param minLon minimum longitude value
 * @param maxLon maximum longitude value
 * @return the number of bins inside the given area
 */
int subset(const double *lats, const double *lons, const int *data, const int *basebins, const int *nBinsInRow,
           int nbins, int nrows, int **outBins, double **outLats, double **outLons, int **outData, int **outBasebins,
           int **outBinsInRow,
           int *outRows,
           double minLat, double maxLat, double minLon,
           double maxLon) {
    int currentRow = 0;
    int nsubsetBins = 0;
    struct node *head = NULL;
    struct node *tail = NULL;
    for (int i = 0; i < nbins; i++) {
        if (lats[i] <= maxLat && lats[i] >= minLat && lons[i] <= maxLon && lons[i] >= minLon) {
            if (head == NULL) {
                head = (struct node *) malloc(sizeof(struct node));
                head->next = NULL;
                head->bin = i + 1;
                head->firstBin = 0;
                head->lastBin = -1;
                tail = head;
            } else {
                struct node *tmp = (struct node *) malloc(sizeof(struct node));
                tmp->bin = i + 1;
                tmp->next = NULL;
                tmp->firstBin = tail->bin < basebins[currentRow] + nBinsInRow[currentRow] && tmp->bin >= basebins[currentRow] + nBinsInRow[currentRow] ? ++currentRow : -1;
                tmp->lastBin = tmp->bin >= basebins[currentRow] + nBinsInRow[currentRow] - 1 ? currentRow : -1;

                tail->next = tmp;
                tail = tmp;
            }
            nsubsetBins++;
        } else {
            if (currentRow + 1 < nrows) {
                if (i >= basebins[currentRow + 1])
                    currentRow++;
            }
        }
    }

    int prevBin = 0;
    int *subsetBins = (int *) malloc(nsubsetBins * sizeof(int));
    double *subsetLats = (double *) malloc(nsubsetBins * sizeof(double));
    double *subsetLons = (double *) malloc(nsubsetBins * sizeof(double));
    int *subsetData = (int *) malloc(nsubsetBins * sizeof(int));
    int *subsetBasebins = (int *) malloc((currentRow + 1) * sizeof(int));
    int *subsetBinsInRow = (int *) malloc((currentRow + 1) * sizeof(int));
    int binsInRow = 0;
    currentRow = -1;
    for (int i = 0; i < nsubsetBins; i++) {
        if (head != NULL) {
            subsetBins[i] = ++prevBin;
            subsetLats[i] = lats[head->bin - 1];
            subsetLons[i] = lons[head->bin - 1];
            subsetData[i] = data[head->bin - 1];
            binsInRow++;
            if (head->firstBin != -1) {
                currentRow++;
                subsetBasebins[currentRow] = subsetBins[i];
            } else if (head->lastBin != -1) {
                subsetBinsInRow[currentRow] = binsInRow;
                binsInRow = 0;
            }
            struct node *tmp = head->next;
            free(head);
            head = tmp;
        }
    }

    *outRows = currentRow + 1;
    *outBins =  subsetBins;
    *outLats = subsetLats;
    *outLons = subsetLons;
    *outData =  subsetData;
    *outBasebins =  subsetBasebins;
    *outBinsInRow = subsetBinsInRow;
    printf("%d \n", currentRow);
    return nsubsetBins;
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
 * @return the number of bins in the subset
 */
int createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, int fillValue,
                       int **outBins, const double *inData, const double *weights,
                       double **outLats, double **outLons, int **outBinsInRow, int **outBasebins,
                       int **outData, int *outRows, bool chlora, double minLat, double maxLat, double minLon, double maxLon) {
    double *latrows = (double *) malloc(sizeof(double) * nrows);
    int *basebins = (int *) malloc(sizeof(int) * nrows);
    int *nBinsInRow = (int *) malloc(sizeof(int) * nrows);
    for (int i = 0; i < nrows; ++i) {
        latrows[i] = ((i + 0.5) * 180.0 / nrows) - 90;
        nBinsInRow[i] = (int) (2 * nrows * cos(latrows[i] * M_PI / 180.0) + 0.5);
        if (i == 0) {
            basebins[i] = 1;
        } else {
            basebins[i] = basebins[i - 1] + nBinsInRow[i - 1];
        }
    }
    double *meanData = (double *) malloc(sizeof(double) * nDataBins);
    double *lats = (double *) malloc(sizeof(double) * totalBins);
    double *lons = (double *) malloc(sizeof(double) * totalBins);
    int *bins = (int *) malloc(sizeof(int) * totalBins);
    int *data = (int *) malloc(sizeof(int) * totalBins);
    struct coordinates *coords;
    coords = (struct coordinates *) malloc(sizeof(struct coordinates));
    for (int i = 0; i < totalBins; i++) {
        bins[i] = i + 1;
        data[i] = fillValue;
        bin2latlon(bins[i], nBinsInRow, latrows, basebins, nrows, coords);
        lats[i] = coords->latitude;
        lons[i] = coords->longitude;
    }
    free(coords);
    free(latrows);

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
        if (meanData[i] != fillValue) {
            double ratio = (meanData[i] + fabs(minValue)) / fabs(maxValue - minValue);
            data[dataBins[i] - 1] = (int) floor(ratio * 255);
        }
    }
    free(meanData);
    int nbins = subset(lats, lons, data, basebins, nBinsInRow, totalBins, nrows, outBins, outLats, outLons, outData, outBasebins,
                       outBinsInRow, outRows, minLat, maxLat, minLon, maxLon);

    printf("Finish subset");
    free(lats);
    free(lons);
    free(bins);
    free(data);
    free(basebins);
    free(nBinsInRow);
    return nbins;
}