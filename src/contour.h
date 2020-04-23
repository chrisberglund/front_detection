#ifndef SIED_CONTOUR_H
#define SIED_CONTOUR_H
double gradient_ratio(const int *window, int fillValue);
void contour(int *bins, int *inData, int *filteredData, int *outData, int ndata, int nrows, const int *nBinsInRow,
             const int *basebins, int fillValue);
#endif //SIED_CONTOUR_H
