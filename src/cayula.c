#include <stdlib.h>
#include "histogram.h"
#include "helpers.h"
#include "cohesion.h"
#include "contour.h"
#include "filter.h"
#include "cayula.h"

void cayula(int *data, int *out_data, int n_bins, int nrows, int *n_bins_in_row, int *basebins) {
    int *filtered_data = malloc(n_bins * sizeof(int));

    median_filter(data, filtered_data, n_bins, nrows, n_bins_in_row, basebins);
    int *edge_pixels = malloc(n_bins * sizeof(int));

    for (int i = 0; i < n_bins; i++) {
        if (data[i] == FILL_VALUE) {
            out_data[i] = -1;
        } else {
            out_data[i] = 0;
        }
        edge_pixels[i] = 0;
    }

    int half_step = WINDOW_WIDTH / 2;
    int *edge_window = malloc(WINDOW_AREA * sizeof(int));
    int *window = malloc(WINDOW_AREA * sizeof(int));
    int *bin_window = malloc(WINDOW_AREA * sizeof(int));
    for (int i = half_step - 1; i < nrows - half_step; i += WINDOW_WIDTH) {
        for (int j = half_step - 1; j < n_bins_in_row[i] - half_step; j += WINDOW_WIDTH) {
            if (n_bins_in_row[i - WINDOW_WIDTH + 1] < WINDOW_WIDTH || n_bins_in_row[i + WINDOW_WIDTH] < WINDOW_WIDTH) {
                continue;
            }
            get_window(basebins[i] + j, i, WINDOW_WIDTH, filtered_data, n_bins_in_row, basebins, window);
            int threshold = histogram_analysis(window);
            if (threshold > 0) {
                get_bin_window(basebins[i] + j, i, WINDOW_WIDTH, n_bins_in_row, basebins, bin_window);
                if (cohesive(window, threshold)) {
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
    free(edge_window);
    free(window);
    free(bin_window);
    contour(edge_pixels, filtered_data, out_data, n_bins, nrows, n_bins_in_row, basebins);
    free(filtered_data);
    free(edge_pixels);
}

