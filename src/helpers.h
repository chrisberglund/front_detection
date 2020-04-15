//
// Created by Christopher Berglund on 11/1/19.
//

#ifndef SIED_HELPERS_H
#define SIED_HELPERS_H
int get_window(int bin, int row, int width, const int *data, const int *n_bins_in_row,
                const int *basebins, int window[]);
int findClosestValue(int arr[], int l, int r, int x);
#endif //SIED_HELPERS_H
