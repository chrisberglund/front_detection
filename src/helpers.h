//
// Created by Christopher Berglund on 11/1/19.
//

#ifndef SIED_HELPERS_H
#define SIED_HELPERS_H
int get_window(int bin, int row, int width, const int *data, const int *n_bins_in_row,
                const int *basebins, int window[]);
void get_bin_window(int bin, int row, int width, const int *n_bins_in_row, const int *basebins, int window[]);
#endif //SIED_HELPERS_H
