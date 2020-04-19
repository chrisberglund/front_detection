#include <stdlib.h>
#include <stdio.h>
#include "histogram.h"
#include "../include/initialize/prefilter.h"
#include "helpers.h"
#include "cohesion.h"
#include "locator.h"
#include "contour.h"
#include "filter.h"
#include "cayula.h"
#include <math.h>
#include "../include/initialize/initialize.h"

void define(double *lats, double *lons, int *outRows, int *outBins, int nrows, int totalBins) {
    getLatLon(lats, lons, outRows, outBins, nrows, totalBins);
}

void cayula(int *bins, int *data, int *out_data, int n_bins, int nrows, int *n_bins_in_row, int *basebins) {
    int *filtered_data = malloc(n_bins * sizeof(int));

    median_filter(data, filtered_data, n_bins, nrows, n_bins_in_row, basebins);
    int *edgePixels = (int *) malloc(n_bins * sizeof(int));
    for (int i = 0; i < n_bins; i++) {
        if (data[i] == FILL_VALUE) {
            out_data[i] = FILL_VALUE;
        } else {
            out_data[i] = 0;
        }
    }

    for (int i = WINDOW_WIDTH / 2 - 1; i < nrows - WINDOW_WIDTH / 2; i += WINDOW_WIDTH / 2) {
        for (int j = WINDOW_WIDTH / 2 - 1; j < n_bins_in_row[i] - WINDOW_WIDTH / 2; j += WINDOW_WIDTH / 2) {
            if (n_bins_in_row[i - WINDOW_WIDTH / 2] < WINDOW_WIDTH || n_bins_in_row[i + WINDOW_WIDTH / 2] < WINDOW_WIDTH)
                continue;
            int *window = (int *) malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
            int *binWindow = (int *) malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
            get_window(basebins[i] + j, i, WINDOW_WIDTH, filtered_data, n_bins_in_row, basebins, window);
            get_window(basebins[i] + j, i, WINDOW_WIDTH, bins, n_bins_in_row, basebins, binWindow);
            int threshold = histogram_analysis(window, WINDOW_WIDTH, 256);

            if (threshold > 0) {
                if (isCohesive(window, WINDOW_WIDTH, threshold)) {
                    int *edgeWindow = malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
                    locateEdgePixels(window, edgeWindow, WINDOW_WIDTH, threshold);
                    for (int k = 0; k < WINDOW_WIDTH; k++) {
                        for (int m = 0; m < WINDOW_WIDTH; m++) {
                            if (edgeWindow[k * WINDOW_WIDTH + m] == threshold) {
                                edgePixels[binWindow[k * WINDOW_WIDTH + m] - 1] = edgeWindow[k * WINDOW_WIDTH + m];
                            }
                        }
                    }
                    free(edgeWindow);
                }
            }
            free(window);
            free(binWindow);
        }
    }
    free(filtered_data);
}

/*
void cayula(int totalBins, int nDataBins, int nrows, int fillValue,
            int *dataBins, int *rows, double *inData, double *weights, double *lats, double *lons, int *outData,
            bool chlora) {
    int *bins = (int *) malloc(totalBins * sizeof(int));
    int *nBinsInRow = (int *) malloc(nrows * sizeof(int));
    int *basebins = (int *) malloc(nrows * sizeof(int));
    int *data = (int *) malloc(totalBins * sizeof(int));

    createFullBinArray(totalBins, nDataBins, nrows, dataBins, fillValue,
                       bins, inData, weights, lats, lons, nBinsInRow, basebins, data, chlora);

    int *filteredData = (int *) malloc(totalBins * sizeof(int));
    median_filter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);

    int *edgePixels = (int *) malloc(totalBins * sizeof(int));

    for (int i = 0; i < totalBins; i++) {
        if (data[i] == fillValue) {
            outData[i] = fillValue;
        } else {
            outData[i] = 0;
        }
    }
    free(data);
    for (int i = WINDOW_WIDTH / 2 - 1; i < nrows - WINDOW_WIDTH / 2; i += WINDOW_WIDTH / 2) {
        for (int j = WINDOW_WIDTH / 2 - 1; j < nBinsInRow[i] - WINDOW_WIDTH / 2; j += WINDOW_WIDTH / 2) {
            if (nBinsInRow[i - WINDOW_WIDTH / 2] < WINDOW_WIDTH || nBinsInRow[i + WINDOW_WIDTH / 2] < WINDOW_WIDTH)
                continue;
            int *window = (int *) malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
            int *binWindow = (int *) malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
            get_window(basebins[i] + j, i, WINDOW_WIDTH, filteredData, nBinsInRow, basebins, window, fillValue, false);
            get_window(basebins[i] + j, i, WINDOW_WIDTH, bins, nBinsInRow, basebins, binWindow, fillValue, false);
            int threshold = histogram_analysis(window, WINDOW_WIDTH, 256);

            if (threshold > 0) {
                if (isCohesive(window, WINDOW_WIDTH, threshold)) {
                    int *edgeWindow = malloc(WINDOW_WIDTH * WINDOW_WIDTH * sizeof(int));
                    locateEdgePixels(window, edgeWindow, WINDOW_WIDTH, threshold);
                    for (int k = 0; k < WINDOW_WIDTH; k++) {
                        for (int m = 0; m < WINDOW_WIDTH; m++) {
                            if (edgeWindow[k * WINDOW_WIDTH + m] == threshold) {
                                edgePixels[binWindow[k * WINDOW_WIDTH + m] - 1] = edgeWindow[k * WINDOW_WIDTH + m];
                            }
                        }
                    }
                    free(edgeWindow);
                }
            }
            free(window);
            free(binWindow);
        }
    }
    contour(bins, edgePixels, filteredData, outData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(nBinsInRow);
    free(basebins);
    free(filteredData);
    free(edgePixels);
    free(bins);
}*/

