#include "helpers.h"
#include "cayula.h"

/*
 * Function:  get_window
 * --------------------
 * Selects a window of data values centered on a given bin with a given width. Window must not cross the edge of the
 * boundaries of the original data map. This function selects the north and south adjacent bins using the ratio
 * between the position of the bin in its row and the total number of bins in the row.
 *
 * args:
 *      int bin: bin number of the center bin in the window. If the width of the window is even, then this is the upper
 *      left bin in the center.
 *      int row: the row number of the center bin. Row numbers begin with 0.
 *      int width: the width of the desired window. Width must be a positive number greater than 2.
 *      int *n_bins_in_row: pointer to an array containing the number of bins in each row
 *      int *basebins: pointer to an array containing the bin number for the first bin in each row
 *      int *window: pointer to output array for the window. The array should be of width * width length
 * returns:
 *      int the number of fill values contained in the window
 */
int get_window(int bin, int row, int width, const int *data, const int *n_bins_in_row,
                const int *basebins, int window[]) {
    int nfill_values = 0;
    int column_neighbor;
    double ratio = (bin - basebins[row]) / (double) n_bins_in_row[row];
    if (width % 2 == 0) {
        int max_distance = width >> 1;
        int current_row = row - max_distance;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = (int) (ratio * n_bins_in_row[current_row] + 0.5) + basebins[current_row];
            for (int j = 0; j < width; j++) {
                window[i + j] = data[column_neighbor + j - max_distance];
                if (window[i+j] == FILL_VALUE)  nfill_values++;
            }
            current_row++;
        }
    } else {
        int max_distance = (width - 1) >> 1;
        int current_row = row - max_distance;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = (int) (ratio * n_bins_in_row[current_row] + 0.5) + basebins[current_row];
            for (int j = 0; j < width; j++) {
                window[i + j] = data[column_neighbor + j - max_distance];
                if (window[i+j] == FILL_VALUE)  nfill_values++;
            }
            current_row++;
        }
    }
    return nfill_values;
}

void get_bin_window(int bin, int row, int width, const int *n_bins_in_row, const int *basebins, int window[]) {
    int nfill_values = 0;
    int column_neighbor;
    double ratio = (bin - basebins[row]) / (double) n_bins_in_row[row];
    if (width % 2 == 0) {
        int max_distance = width >> 1;
        int current_row = row - max_distance;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = (int) (ratio * n_bins_in_row[current_row] + 0.5) + basebins[current_row];
            for (int j = 0; j < width; j++) {
                window[i + j] = column_neighbor + j - max_distance;
            }
            current_row++;
        }
    } else {
        int max_distance = (width - 1) >> 1;
        int current_row = row - max_distance;
        int area = width * width;
        for (int i = 0; i < area; i += width) {   //Iterate through the first bins of each row in window
            column_neighbor = (int) (ratio * n_bins_in_row[current_row] + 0.5) + basebins[current_row];
            for (int j = 0; j < width; j++) {
                window[i + j] = column_neighbor + j - max_distance;
            }
            current_row++;
        }
    }
}