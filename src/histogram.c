/*
 * Functions for the implementation of the histogram analysis step of the single image edge detection algorithm.
 */
#include <string.h>
#include "histogram.h"
#include "cayula.h"


#define CRIT_VALUE 0.7

static inline double square(double a) {
    return a * a;
}

static inline int squarei(int a) {
    return a * a;
}

/*
 * Function: get_histogram
 * --------------------
 * Creates a histogram of the values in the window assuming the window contains integer values ranging from
 * 0 to 255.
 *
 * args:
 *      int *data: the data contained within the window. Ranges from 0 to 255.
 *      int *histogram: pointer to a 256 element array for the output of the histogram
 */
void get_histogram(const int *data, int *histogram) {
    memset(histogram, 0, 256 * sizeof(int));
    int nvalid = 0;
    int area = squarei(WINDOW_WIDTH);
    for (int i = 0; i < area; i++) {
        if (data[i] != FILL_VALUE) {
            histogram[data[i]]++;
            nvalid++;
        }
    }
}

/*
 * Function:  within_group_variance
 * --------------------
 * Calculates the sum of the variance within the groups resulting from segmenting by the given threshold
 *
 * args:
 *      int *histogram: pointer to an array containing the histogram of the window
 *      double mu_low: mean of the group below the threshold
 *      double mu_high: mean of the group above the threshold
 *      int n_low: sum of the histogram of the group below the threshold
 *      int n_high: sum of the histogram of the group above the threshold
 *      int tau: the threshold
 * returns:
 *      double: the sum of the variance within the groups
 */
double within_group_variance(const int *histogram, double mu_low, double mu_high, int n_low, int n_high, int tau) {
    double num = 0;
    for (int i = 0; i < tau; i++) {
        num += square(i - mu_low) * histogram[i];
    }
    double s_low = num / n_low;
    num = 0;
    for (int i = tau; i < 256; i++) {
        num += square(i - mu_high) * histogram[i];
    }
    double s_high = num / n_high;

    return (s_low * n_low / (n_low + n_high)) + (s_high * n_high / (n_low + n_high));
}


/*
 * Function:  too_large
 * --------------------
 * Check to see if segmenting the window by the given threshold results in too small of a segment.
 *
 * args:
 *      int *histogram: pointer to an array containing a histogram of the values in the window
 *      int tau: the threshold by which to segment the window
 * returns:
 *      int: 1 if one of the segments is too large and 0 if both segments are of adequate size
 */
int too_large(const int *histogram, int tau) {
    int sum = 0;
    double area = (double) (WINDOW_WIDTH * WINDOW_WIDTH);
    for (int i = 0; i < tau; i++) {
        sum += histogram[i];
    }
    double ratio = sum / area;
    return (ratio < 0.25 ||  ratio > 0.75);
}

/*
 * Function:  histogram_analysis
 * --------------------
 * Performs the histogram step of the single image edge detection algorithm. Looks for a value which divides the given
 * window into two distinct values and compares the relationship between the within group variance and between group
 * variance to determine if there is likely to be a front in the window.
 *
 * args:
 *      int *window pointer to an array containing the data values to perform the histogram analysis on
 * returns:
 *      int: the threshold value that best divides the window
 */
int histogram_analysis(int *window) {
    int histogram[256];
    get_histogram(window, histogram);
    int n_low = 0, num_low = 0, n_high = 0, num_high = 0;


    double max_between = 0;
    for (int i = 0; i < 256; i++) {
        n_high += histogram[i];
        num_high += i * histogram[i];
    }
    int tau = -1;
    double n_low_max, n_high_max, mu_high, mu_low_max, mu_low, mu_high_max;

    for (int i = 0; i  < 254; i++) {
        n_high -= histogram[i];
        num_high -= i * histogram[i];
        n_low += histogram[i];
        num_low += (i) * histogram[i];
        if (n_low != 0 && n_high != 0) {
            mu_low = (double) num_low / n_low;
            mu_high = (double) num_high / n_high;
            double between = (square(mu_low - mu_high) * n_low * n_high) / squarei(n_low + n_high);

            if (between > max_between) {
                tau = i + 1;
                max_between = between;
                n_low_max = n_low;
                n_high_max = n_high;
                mu_low_max = mu_low;
                mu_high_max = mu_high;
            }
        }

    }
    double theta = 0;
    if (!too_large(histogram, tau)) {
        double within = within_group_variance(histogram, mu_low_max, mu_high_max, n_low_max, n_high_max, tau);
        theta = max_between / (max_between + within);
    }

    return theta >= CRIT_VALUE ? tau : -1;
}