//
// Created by Christopher Berglund on 11/1/19.
//
#include <stdlib.h>
#include "locator.h"


void locateEdgePixels(const int *window, int *outWindow, int width, int threshold) {
    int *bodies = malloc((width * width) * sizeof(int));
    for (int i = 0; i < width * width; i++) {
        bodies[i] = window[i] >= threshold;
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            int center = window[i * width + j];
            if (center == window[(i - 1) * width + j] && center == window[(i + 1) * width + j]
            && center == window[i * width + (j - 1)] && center == window[i * width + (j + 1)]) {
                outWindow[i * width + j] = threshold;
            }
        }
    }
}