#include <stdlib.h>
#include <stdio.h>
#include "histogram.h"
#include "prefilter.h"
#include "helpers.h"
#include "cohesion.h"
#include "locator.h"
#include "contour.h"
#include "filter.h"
#include "cayula.h"
#include <math.h>

#define WINDOW_WIDTH 32

void cayula(int totalBins, int nDataBins, int nrows, int fillValue,
            int *dataBins, int *rows, double *inData, double *weights, double *lats, double *lons, int *outData,
            bool chlora) {
    int *bins = (int *) malloc(totalBins * sizeof(int));
    int *nBinsInRow = (int *) malloc(nrows * sizeof(int));
    int *basebins = (int *) malloc(nrows * sizeof(int));
    int *data = (int *) malloc(totalBins * sizeof(int));

    //printf("Start \n");
    createFullBinArray(totalBins, nDataBins, nrows, dataBins, fillValue,
                       bins, inData, weights, lats, lons, nBinsInRow, basebins, data, chlora);

    //printf("Initializing done \n");
    int *filteredData = (int *) malloc(totalBins * sizeof(int));
    medianFilter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);


    //printf("Median filter done \n");

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
            getWindow(basebins[i] + j, i, WINDOW_WIDTH, filteredData, nBinsInRow, basebins, window, fillValue, false);
            getWindow(basebins[i] + j, i, WINDOW_WIDTH, bins, nBinsInRow, basebins, binWindow, fillValue, false);
            int threshold = histogramAnalysis(window, WINDOW_WIDTH, 256);

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
    //printf("Edgeing done \n");
    contour(bins, edgePixels, filteredData, outData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(nBinsInRow);
    free(basebins);
    free(filteredData);
    free(edgePixels);
    //printf("Contouring done \n");
    free(bins);
}

