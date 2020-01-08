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

void cayula(int totalBins, int nDataBins, int nInputRows, int fillValue,
            int *dataBins, double *inData, double *weights, double **lats, double **lons, int **outData,
            bool chlora, double minLat, double maxLat, double minLon, double maxLon) {
    int *basebins;
    int *bins;
    int *data;
    int *nBinsInRow;
    int nrows;
    printf("test");
    int nbins = createFullBinArray(totalBins, nDataBins, nInputRows, dataBins, fillValue,
                           &bins, inData, weights,
                           lats, lons, &nBinsInRow, &basebins,
                           &data, &nrows, chlora, minLat,maxLat, minLon,maxLon);

    //printf("Initializing done \n");
    int *filteredData = (int *) malloc(nbins * sizeof(int));
    medianFilter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);


    //printf("Median filter done \n");

    int *edgePixels = (int *) malloc(nbins * sizeof(int));
    int *finalData = (int *) malloc(nbins * sizeof(int));

    for (int i = 0; i < nbins; i++) {
        if (data[i] == fillValue) {
            finalData[i] = fillValue;
        } else {
            finalData[i] = 0;
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
    contour(bins, edgePixels, filteredData, finalData, nbins, nrows, nBinsInRow, basebins, fillValue);
    *outData = finalData;
    free(filteredData);
    free(edgePixels);
    free(basebins);
    free(nBinsInRow);
    //printf("Contouring done \n");
    free(bins);
}

