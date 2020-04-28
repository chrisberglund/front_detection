#include <stdlib.h>
#include "histogram.h"
#include "prefilter.h"
#include "helpers.h"
#include "cohesion.h"
#include "contour.h"
#include "filter.h"
#include "cayula.h"
#include "initialize.h"

void define(double *lats, double *lons, int *outRows, int *outBins, int nrows, int totalBins) {
    getLatLon(lats, lons, outRows, outBins, nrows, totalBins);
}

void cayula(int *data, int *out_data, int n_bins, int nrows, int *n_bins_in_row, int *basebins) {
    int *filtered_data = malloc(n_bins * sizeof(int));

    median_filter(data, filtered_data, n_bins, nrows, n_bins_in_row, basebins);
    int *edge_pixels = malloc(n_bins * sizeof(int));
    for (int i = 0; i < n_bins; i++) {
        if (data[i] == FILL_VALUE) {
            out_data[i] = FILL_VALUE;
        } else {
            out_data[i] = 0;
        }
    }
    int half_step = WINDOW_WIDTH / 2;
    int area = WINDOW_WIDTH * WINDOW_WIDTH;
    for (int i = half_step - 1; i < nrows - half_step; i += half_step) {
        for (int j = half_step - 1; j < n_bins_in_row[i] - half_step; j += half_step) {
            if (n_bins_in_row[i - half_step] < WINDOW_WIDTH || n_bins_in_row[i + half_step] < WINDOW_WIDTH) {
                continue;
            }
            int window[area];
            get_window(basebins[i] + j, i, WINDOW_WIDTH, filtered_data, n_bins_in_row, basebins, window);
            int threshold = histogram_analysis(window);

            if (threshold > 0) {
                int bin_window[area];
                get_bin_window(basebins[i] + j, i, WINDOW_WIDTH, n_bins_in_row, basebins, bin_window);
                if (cohesive(window, threshold)) {
                    int edge_window[area];
                    find_edge(window, edge_window, threshold);
                    for (int k = 0; k < WINDOW_WIDTH; k++) {
                        for (int m = 0; m < WINDOW_WIDTH; m++) {
                            if (edge_window[k * WINDOW_WIDTH + m]) {
                                edge_pixels[bin_window[k * WINDOW_WIDTH + m]] = edge_window[k * WINDOW_WIDTH + m];
                            }
                        }
                    }
                }
            }
        }
    }
    contour(edge_pixels, filtered_data, out_data, n_bins, nrows, n_bins_in_row, basebins);
    free(filtered_data);
    free(edge_pixels);
}

