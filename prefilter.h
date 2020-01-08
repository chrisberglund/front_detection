#ifndef SIED_PREFILTER_H
#define SIED_PREFILTER_H
int createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, int fillValue,
                       int **outBins, const double *inData, const double *weights,
                       double **outLats, double **outLons, int **outBinsInRow, int **outBasebins,
                       int **outData, int *outRows, bool chlora, double minLat, double maxLat, double minLon, double maxLon);
#endif //SIED_PREFILTER_H
