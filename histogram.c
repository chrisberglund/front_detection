/**
 * Histogram analysis
 */

#include "histogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
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
    int *occurrence = (int *) malloc(sizeof(int) * nbins);
    double *histogram = (double *) malloc(sizeof(double) * nbins);
    for (int i = 0; i < nbins; i++) occurrence[i] = 0;
    for (int i = 0; i < nbins; i++) histogram[i] = 0;
    int nvalid = 0;
    for (int i = 0; i < npixels; i++) {
        if (data[i] != -999) {
            int test = data[i];
            occurrence[data[i]]++;
            nvalid++;
        }
    }
    for (int i = 0; i < nbins; i++) {
        if (nvalid > 0)
            histogram[i] = (double) occurrence[i] / nvalid;
        else
            histogram[i] = 0;
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
    sort(copy, 0, (width * width) - 1);
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
int histogramAnalysis(int *window, int width, int nvalues) {
    double *histogram = getHistogram(window, width * width, nvalues);
    double between, mulow, muhigh, mulowMax, muhighMax, num, dem;
    double nlow = 0, nhigh = 0, nlowMax = 0, nhighMax = 0;
    double maxBetween = 0;
    int threshold = -1;
    for (int i = 1; i < nvalues - 1; i++) {  //Assuming 0 or nvalues can't be the best threshold
        nlow = 0;
        nhigh = 0;
        num = 0;
        dem = 0;
        for (int j = 0; j < i; j++) {
            nlow += histogram[j];
            num += (j) * histogram[j];
            dem += histogram[j];
        }
        if (dem == 0)
            continue;
        mulow = num / dem;
        num = 0;
        dem = 0;
        for (int j = i; j < nvalues; j++) {
            nhigh += histogram[j];
            num += (j) * histogram[j];
            dem += histogram[j];
        }
        if (dem == 0)
            continue;
        muhigh = num / dem;
        between = betweenGroupVariance(mulow, muhigh, nlow, nhigh);
        if (i == 1) {
            threshold = i;
            maxBetween = between;
            nlowMax = nlow;
            nhighMax = nhigh;
            mulowMax = mulow;
            muhighMax = muhigh;
        }
        if (between > maxBetween) {
            threshold = i;
            maxBetween = between;
            nlowMax = nlow;
            nhighMax = nhigh;
            mulowMax = mulow;
            muhighMax = muhigh;
        }
    }

    if (isTooLarge(window, width, threshold))
        return -1;

    double within = withinGroupVariance(histogram, mulowMax, muhighMax, nlowMax, nhighMax, threshold, nvalues);
    double theta = maxBetween / (maxBetween + within);
    free(histogram);
    if (theta >= CRIT_VALUE && !isTooLarge(window, width, threshold)) {
        return threshold;
    } else {
        return -1;
    }
}