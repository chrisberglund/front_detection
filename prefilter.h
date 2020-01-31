#include "cayula.h"
#ifndef SIED_PREFILTER_H
#define SIED_PREFILTER_H
struct rawData {
    const double *values;
    const double *weights;
};
void createFullBinArray(int totalBins, int nrows, int *outBins, double *lats, double *lons, struct binRows rows,
                        int *outData);

void calculateDataValues(struct rawData inData, const int *dataBins, int nbins, int *outData, bool chlora);
#endif //SIED_PREFILTER_H
