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


#define CRIT_VALUE 0.7

/*
 * 
 */
void get_histogram(const int *data, double *histogram, int n_pixels) {
    int occurrence[256] = {0};
    memset(histogram, 0, sizeof(int) * 256);
    int nvalid = 0;
    for (int i = 0; i < n_pixels; i++) {
        if (data[i] != -999) {
            occurrence[data[i]]++;
            nvalid++;
        }
    }
    for (int i = 0; i < 256; i++) {
        if (nvalid > 0)
            histogram[i] = (double) occurrence[i] / nvalid;
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
        for (int i = nvalues - 1; i >= threshold - 1; i--) {
            num += (i + 1) * histogram[i];
            dem += histogram[i];
        }
    } else {
        for (int i = 0; i < threshold - 1; i++) {
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

/**
 * Checks if segmentation results in too large of a segment
 */
bool isTooLarge(const int *window, int width, int threshold) {
    int *copy = malloc((width * width) * sizeof(int));
    for (int i = 0; i < (width * width); i++) {
        copy[i] = window[i];
    }
    //sort(copy, 0, (width * width) - 1);
    int i = 0;
    while (copy[i] < threshold) {
        i++;
    }
    free(copy);
    return (((double) i / (width * width) < 0.25) || ((double) i / (width * width)) > 0.75);
}

/**
 *
 * @param window
 * @param width
 * @param nvalues
 * @return threshold value, -1 if unimodal
 */
int histogram_analysis(int *window, int width, int nvalues) {
    double histogram[256];
    get_histogram(window, histogram, nvalues);
    double between, mu_low, mu_high, mu_low_max, mu_high_max, num, dem;
    double nlow = 0, nhigh = 0, nlowMax = 0, nhighMax = 0;
    double maxBetween = 0;
    int threshold = -1;
    for (int i = 1; i < nvalues - 1; i++) {  //Assuming 0 or nvalues can't be the best threshold
        nlow = 0;
        nhigh = 0;
        /*
         * Calculate statistics for body below current threshold
         */
        num = 0;
        dem = 0;
        for (int j = 0; j < i; j++) {
            nlow += histogram[j];
            num += (j) * histogram[j];
            dem += histogram[j];
        }
        if (dem == 0) continue;
        mu_low = num / dem;
        /*
         * Calculate statistics for body below current threshold
         */
        num = 0;
        dem = 0;
        for (int j = i; j < nvalues; j++) {
            nhigh += histogram[j];
            num += (j) * histogram[j];
            dem += histogram[j];
        }
        if (dem == 0) continue;
        mu_high = num / dem;
        between = betweenGroupVariance(mu_low, mu_high, nlow, nhigh);
        if (i == 1) {
            threshold = i;
            maxBetween = between;
            nlowMax = nlow;
            nhighMax = nhigh;
            mu_low_max = mu_low;
            mu_high_max = mu_high;
        }
        if (between > maxBetween) {
            threshold = i;
            maxBetween = between;
            nlowMax = nlow;
            nhighMax = nhigh;
            mu_low_max = mu_low;
            mu_high_max = mu_high;
        }
    }

    if (isTooLarge(window, width, threshold))
        return -1;

    double within = withinGroupVariance(histogram, mu_low_max, mu_high_max, nlowMax, nhighMax, threshold, nvalues);
    double theta = maxBetween / (maxBetween + within);
    free(histogram);
    if (theta >= CRIT_VALUE && !isTooLarge(window, width, threshold)) {
        return threshold;
    } else {
        return -1;
    }
}