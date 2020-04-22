#include "cohesion.h"
#include "cayula.h"

#define CRIT_C1 0.90
#define CRIT_C2 0.90
#define CRIT_C 0.92

static inline int squarei(int a) {
    return a * a;
}

static inline int min(int a, int b) {
    return a < b ? a : b;
}

static inline int max(int a, int b) {
    return a > b ? a : b;
}

/*
 * Function:  cohesive
 * --------------------
 * Determines if the two groups in the window divided separated by the given threshold is sufficiently cohesive.
 * Compares each bin to its neighbors to see if members of each group are near other members.
 *
 * args:
 *      int *window: pointer to an array containing the data window
 *      int threshold: the threshold to separate the two groups by
 *
 * returns:
 *      int: 1 if the threshold results in cohesive groups 0 if it does not
 */
int cohesive(const int *window, int threshold) {
    int area = squarei(WINDOW_WIDTH);
    int copy[area];
    for (int i = 0; i < area; i++) {
        copy[i] = window[i] == FILL_VALUE ? FILL_VALUE : window[i] >= threshold;
    }
    double r1 = 0, t1 = 0, r2 = 0, t2 = 0;
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_WIDTH; j++) {
            int sum = 0;
            int count = 0;
            if (copy[i * WINDOW_WIDTH + j] != FILL_VALUE) {
                for (int k = max(i - 1, 0); k < min(i + 2, WINDOW_WIDTH); k++) {
                    for (int l = max(j - 1, 0); l < min(j + 2, WINDOW_WIDTH); l++) {
                        if (k != i && l != j && copy[k * WINDOW_WIDTH + l] != FILL_VALUE) {
                            sum += copy[k * WINDOW_WIDTH + l];
                            count++;
                        }
                    }
                }
                if (copy[i * WINDOW_WIDTH + j] == 0) {
                    r1 += count - sum;
                    t1 += count;
                } else {
                    r2 += sum;
                    t2 += count;
                }
            }
        }
    }
    double c = (r1 + r2)/(t1 + t2);
    return (r1/t1 >= CRIT_C1 && r2/t2 >= CRIT_C2 && c >= CRIT_C);
}

/*
 * Function:  neighbor_is_different
 * --------------------
 * Determines if any of the neighbors of the given element are of a different group than the element. For example,
 * if the element identified by its indices is below the threshold, this function checks to see if any of its neighbors
 * are above the threshold. This function ignores any neighbors that contain a fill value.
 *
 * args:
 *      int *window: pointer to an array containing the data. Elements should be 0 if they are below the threshold
 *      and 1 if they are above the threshold
 *      int row: the row the element of interest can be found in
 *      int col: the column the element of interest can be found in
 *
 * returns:
 *      int: 1 if at least one of the neighbors is different and 0 if all of its valid neighbors are the same
 */
int neighbor_is_different(const int *window, int row, int col) {
    int center = window[row * WINDOW_WIDTH + col];
    for (int i = max(row - 1, 0); i < min(row + 2, WINDOW_WIDTH); i++) {
        for (int j = max(col - 1, 0); j < min(col + 2, WINDOW_WIDTH); j++) {
            if (window[i * WINDOW_WIDTH + j] != FILL_VALUE && center != window[i * WINDOW_WIDTH + j]) {
                return 1;
            }
        }
    }
    return 0;
}


/*
 * Function:  find_edge
 * --------------------
 * Implementation of the window-level processing portion of the "Location of Edge Pixels" step of the algorithm.
 * This function looks for any pixel that has a neighbor that is on the opposite side of the threshold from itself.
 * If a pixel has a different neighbor, it is designated as an edge pixel. Any pixels containing a fill value are
 * ignored and the fill value is passed on to the output array.
 *
 * args:
 *      int *window: pointer to an array containing the data window to perform the detection on. Should be of
 *      WINDOW_WIDTH^2 length
 *      int *out: pointer to an array of same size as the input array to which to write the output values. Edge
 *      pixels will have a value of 1 while non-edge pixels will have a value of 0.
 *      int threshold: threshold value of the window determined by earlier steps of the algorithm
 *
 */
void find_edge(const int *window, int *out, int threshold) {
    int bodies[squarei(WINDOW_WIDTH)];
    for (int i = 0; i < squarei(WINDOW_WIDTH); i++) {
        bodies[i] = window[i] == FILL_VALUE ? FILL_VALUE : window[i] >= threshold;
    }

    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_WIDTH; j++) {
            if (bodies[i * WINDOW_WIDTH + j] != FILL_VALUE) {
                out[i * WINDOW_WIDTH + j] = neighbor_is_different(bodies, i, j);
            } else {
                out[i * WINDOW_WIDTH + j] = FILL_VALUE;
            }
        }
    }
}