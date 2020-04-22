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
        copy[i] = window[i] >= threshold;
    }
    double r1 = 0, t1 = 0, r2 = 0, t2 = 0;
    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_WIDTH; j++) {
            int sum = 0;
            int count = 0;
            for (int k = max(i - 1, 0); k < min(i + 2, WINDOW_WIDTH); k++) {
                for (int l = max(j - 1, 0); l < min(j + 2, WINDOW_WIDTH); l++) {
                    if (k != i && l != j) {
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
    double c = (r1 + r2)/(t1 + t2);
    return (r1/t1 >= CRIT_C1 && r2/t2 >= CRIT_C2 && c >= CRIT_C);
}