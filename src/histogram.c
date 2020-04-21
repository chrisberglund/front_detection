/**
 * Histogram analysis
 */

#include "histogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "helpers.h"
#include "cayula.h"


#define CRIT_VALUE 0.7

/*
 * 
 */
void get_histogram(const int *data, double *histogram, int *count, int n_pixels) {
    memset(count, 0, 256 * sizeof(int));
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    int nvalid = 0;
    for (int i = 0; i < n_pixels; i++) {
        if (data[i] != -999) {
            count[data[i]]++;
            nvalid++;
        }
    }
    for (int i = 0; i < 256; i++) {
        if (nvalid > 0)
            histogram[i] = (double) count[i] / nvalid;
        else
            histogram[i] = 0;
    }
}

/**
 * Calculates mean parameter
 * @param histogram pointer to an array of nvalues length
 * @param threshold threshold value
 * @param high whether to calculate mean for the higher value body
 * @param nvalues number of bins in histogram
 * @return mean
 */
double mean(const double *histogram, int threshold, bool high, int nvalues) {
    double num = 0, dem = 0;
    if (high) {
        for (int i = nvalues - 1; i > threshold; i--) {
            num += (i + 1) * histogram[i];
            dem += histogram[i];
        }
    } else {
        for (int i = 0; i < threshold; i++) {
            num += ((i + 1) * histogram[i]);
            dem += histogram[i];
        }
    }
    return num / dem;
}

/**
 * Calculates the variance of the group
 * @param histogram histogram of values of the window
 * @param mu mean of the group
 * @param threshold threshold value
 * @param high whether or not this group is the higher value body or not
 * @param nvalues number of bins in histogram
 * @return group variance
 */
double groupVariance(const double *histogram, double mu, int threshold, bool high, int nvalues) {
    double num = 0;
    double dem = 0;
    if (high) {
        for (int i = threshold; i < 256; i++) {
            num += pow(i - mu, 2) * histogram[i];
            dem += histogram[i];
        }
        return num / (256 - threshold);
    } else {
        for (int i = 0; i < threshold; i++) {
            num += pow(i - mu, 2) * histogram[i];
            dem += histogram[i];
        }
        return num / dem;
    }
}

/**
 * Calculates the within group variance of the two bodies
 * @param histogram histogram of the window
 * @param mulow mean of lower value body
 * @param muhigh mean of higher value body
 * @param nlow N1
 * @param nhigh N2
 * @param threshold threshold
 * @param nvalues number of bins in histogram
 * @return within group variance
 */
double
withinGroupVariance(const double *histogram, double mulow, double muhigh, double nlow, double nhigh, int threshold,
                    int nvalues) {
    double lowGroup = groupVariance(histogram, mulow, threshold, false, nvalues) * nlow / (nlow + nhigh);
    double highGroup = groupVariance(histogram, muhigh, threshold, true, nvalues) * nhigh / (nlow + nhigh);
    return lowGroup * nlow/(nlow + nhigh) + highGroup * nhigh/(nlow + nhigh);
}

/**
 * Calculates between group variance
 * @param mulow mean of lower value body
 * @param muhigh mean of higher value body
 * @param nlow N1
 * @param nhigh N2
 * @return between group variance
 */
double betweenGroupVariance(double mulow, double muhigh, double nlow, double nhigh) {
    return pow((mulow - muhigh), 2) * (nlow * nhigh) / pow((nlow + nhigh), 2);
}

/*
 * Function:  too_large
 * --------------------
 * Check to see if segmenting the window by the given threshold results in too small of a segment.
 *
 * args:
 *      int *count: pointer to an array containing a histogram containing the count of each value in the window
 *      int threshold: the threshold by which to segment the window
 * returns:
 *      int: 1 if one of the segments is too large and 0 if both segments are of adequate size
 */
int too_large(const int *count, int threshold) {
    int sum = 0;
    double area = (double) (WINDOW_WIDTH * WINDOW_WIDTH);
    for (int i = 0; i < threshold; i++) {
        sum += count[i];
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
    double histogram[256];
    int count[256];
    get_histogram(window, histogram, count, WINDOW_WIDTH * WINDOW_WIDTH);
    int threshold = -1;
    double n_low = 0;
    double num_low = 0;
    double n_high = 0;
    double nlowMax = 0;
    double nhighMax = 0;
    double mu_high = 0;
    double mu_low_max = 0;
    double mu_low = 0;
    double mu_high_max = 0;
    double num_high = 0;
    double max_between = 0;
    for (int i = 0; i < 256; i++) {
        n_high += histogram[i];
        num_high += i * histogram[i];
    }

    for (int i = 0; i  < 254; i++) {
        n_high -= histogram[i];
        num_high -= i * histogram[i];
        n_low += histogram[i];
        num_low += (i) * histogram[i];
        if (n_low == 0 || n_high == 0) continue;
        mu_low = num_low / n_low;
        mu_high = num_high / n_high;
        double between = betweenGroupVariance(mu_low, mu_high, n_low, n_high);

        if (between > max_between) {
            threshold = i + 1;
            max_between = between;
            nlowMax = n_low;
            nhighMax = n_high;
            mu_low_max = mu_low;
            mu_high_max = mu_high;
        }

    }

    if (too_large(count, threshold)) {
        return -1;
    }

    double within = withinGroupVariance(histogram, mu_low_max, mu_high_max, nlowMax, nhighMax, threshold, 256);
    double theta = max_between / (max_between + within);

    if (theta >= CRIT_VALUE) {
        return threshold;
    } else {
        return -1;
    }
}