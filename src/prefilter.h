#include <stdbool.h>

#ifndef SIED_PREFILTER_H
#define SIED_PREFILTER_H
void getLatLon(double *lats, double *lons, int *outRows, int *outBins, int nrows, int nbins);
void createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, int fillValue,
                        int *outBins, const double *inData, const double *weights,
                        double *lats, double *lons, int *nBinsInRow, int *basebins,
                        int *outData, bool chlora);
#endif //SIED_PREFILTER_H