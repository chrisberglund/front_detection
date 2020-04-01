#include <stdbool.h>
#include <stdlib.h>
#include "cohesion.h"

#define CRIT_C1 0.90
#define CRIT_C2 0.90
#define CRIT_C 0.92

/**
 * Checks the cohesiveness of a window that has a detected threshold. Compares each pixel in the window with all of its
 * neighbors and counts the ratio between neighbors that are in the same group and the total number of neighbors
 * @param window pointer to the first element of an array containing the data values for each pixel
 * @param width the width of the window
 * @param threshold the threshold detected with the the histogram method
 * @return true if the window has sufficiently high cohesion coefficients
 */
bool isCohesive(const int *window, int width, int threshold) {
    int *copy = malloc(width * width * sizeof(int));
    for (int i = 0; i < width * width; i++) {
        copy[i] = window[i] >= threshold;
    }
    double r1 = 0;
    double t1 = 0;
    double r2 = 0;
    double t2 = 0;
    for (int i = 1; i < width - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (copy[i * width + j] == 0 ) {
                t1 += 8;
            } else if (copy[i * width + j] == 1) {
                t2 += 8;
            }
            for (int k = 0; k < 3; k ++) {
                for (int m = 0; m < 3; m ++) {
                    if (k == 1 && m == 1) {
                        continue;
                    }
                    if (copy[i * width + j] == 0 && copy[(i + k - 1) * width + (j + m - 1)] == 0)
                        r1++;
                    else if (copy[i * width + j] == 1 && copy[(i + k - 1) * width + (j + m - 1)] == 1)
                        r2++;
                }
            }
        }
    }
    double c1 = r1/t1;
    double c2 = r2/t2;
    double c = (r1 + r2)/(t1 + t2);
    free(copy);
    return (c1 >= CRIT_C1 && c2 >= CRIT_C2 && c >= CRIT_C);
}