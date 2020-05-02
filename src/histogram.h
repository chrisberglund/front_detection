//
// Created by Christopher Berglund on 11/1/19.
//
#include <stdbool.h>
#ifndef SIED_HISTOGRAM_H
#define SIED_HISTOGRAM_H
double mean(const double *histogram, int threshold, bool high, int nvalues);
int histogram_analysis(const int *window);
#endif //SIED_HISTOGRAM_H
