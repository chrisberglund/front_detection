/**
 * Histogram analysis
 */

#include "histogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "helpers.h"

#define CRIT_VALUE 0.7

/**
 * Constructs a histogram
 * @param data data to construct histogram with
 * @param npixels number of pixels in data set
 * @param nbins number of bins for the histogram to contain
 * @return pointer to an array containing all the bins of the histogram. Caller is responsible for freeing
 */
double *getHistogram(const int *data, int npixels, int nbins) {
    int *occurrence = malloc(sizeof(int) * nbins);
    double *histogram = malloc(sizeof(double) * nbins);
    for (int i = 0; i < nbins; i++) occurrence[i] = 0;
    for (int i = 0; i < nbins; i++) histogram[i] = 0;
    for (int i = 0; i < npixels; i++) {
        occurrence[data[i] - 1]++;
    }
    for (int i = 0; i < nbins; i++) {
        histogram[i] = (double) occurrence[i] / npixels;
    }
    free(occurrence);
    return histogram;
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
    if (high) {
        for (int i = nvalues - 1; i >= threshold - 1; i--) {
            num += (i + 1) - mu * histogram[i];
        }
        return num / (nvalues - threshold + 1);
    } else {
        for (int i = 0; i < threshold - 1; i++) {
            num += (i + 1) - mu * histogram[i];
        }
        return num / (threshold - 1);
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
    return lowGroup + highGroup;
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
    sort(copy, 0, (width * width) - 1);
    int i = 0;
    while (window[i] < threshold) {
        i++;
    }
    return (((double) i / (width * width) < 0.25) || ((double) i / (width * width)) > 0.75);
}

bool areTwoBodiesPresent(int *window, int width, int nvalues) {
    double *histogram = getHistogram(window, width * width, nvalues);
    double between, mulow, muhigh, mulowMax = 0, muhighMax = 0, num, dem;
    double nlow = 0, nhigh = 0, nlowMax = 0, nhighMax = 0;
    double maxBetween = -1;
    int threshold = -1;
    for (int i = 1; i < nvalues - 1; i++) {  //Assuming 0 or nvalues can't be the best threshold
        num = 0;
        dem = 0;
        for (int j = 0; j < i; i++) {
            nlow += histogram[i];
            num += (i + 1) * histogram[i];
            dem += histogram[i];
        }
        mulow = num / dem;
        num = 0;
        dem = 0;
        for (int j = nvalues - 1; j >= i; i--) {
            nhigh += histogram[i];
            num += (i + 1) * histogram[i];
            dem += histogram[i];
        }
        muhigh = num / dem;
        between = betweenGroupVariance(mulow, muhigh, nlow, nhigh);
        if (between > maxBetween) {
            threshold = i + 1;
            maxBetween = between;
            nlowMax = nlow;
            nhighMax = nhigh;
            mulowMax = mulow;
            muhighMax = muhigh;
        }
    }
    double within = withinGroupVariance(histogram, mulowMax, muhighMax, nlowMax, nhighMax, threshold, nvalues);
    double theta = maxBetween / (maxBetween + within);
    free(histogram);
    return (theta >= CRIT_VALUE && !isTooLarge(window, width, threshold));
}