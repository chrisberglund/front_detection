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
    for (int i = 1; i < width - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int center = bodies[i * width + j];
            if (center != bodies[(i - 1) * width + j] || center != bodies[(i + 1) * width + j]
            || center != bodies[i * width + (j - 1)] || center != bodies[i * width + (j + 1)]) {
                outWindow[i * width + j] = threshold;
            }
        }
    }
}