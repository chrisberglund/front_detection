//
// Created by Christopher Berglund on 11/2/19.
//

#ifndef SIED_FILTER_H
#define SIED_FILTER_H
void medianFilter(int *bins, int *data, int *filteredData, int nbins, int nrows,
                  int *nBinsInRow, int *basebins, int fillValue);
#endif //SIED_FILTER_H
