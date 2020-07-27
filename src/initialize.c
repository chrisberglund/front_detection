/*
 * Functions for calculating the integer data values to be used in the single image edge detection algorithm
 */

#include "initialize.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cayula.h"


int aoi_bins_length(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon) {
    int counter = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 180. / nrows - 90;
        if (row_lat >= min_lat && row_lat <= max_lat) {
            int nbins_in_row = (int) (2 * nrows * cos(row_lat * M_PI / 180.) + 0.5);
            for (int j = 0; j < nbins_in_row; j++) {
                double lon = 360.0 * (j + 0.5) / nbins_in_row - 180.0;
                if (lon >= min_lon && lon <= max_lon) counter++;
            }
        }
    }
    return counter;
}

int aoi_rows_length(int nrows, double min_lat, double max_lat) {
    int counter = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 180. / nrows - 90;
        if (row_lat >= min_lat && row_lat <= max_lat)
        {
            counter++;
        }
    }
    return counter;
}

void get_latlon(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon, int *basebins,
        int *nbins_in_row, double *lats, double *lons, int *bins) {
    int aoi_bin = 0;
    int bin = 0;
    int row = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 180. / nrows - 90;
        int nrow_bins = (int) (2 * nrows * cos(row_lat * M_PI / 180.) + 0.5);
        if (row_lat >= min_lat && row_lat <= max_lat) {
            nbins_in_row[row] = 0;
            basebins[row] = aoi_bin;
        }
        for (int j = 0; j < nrow_bins; j++) {
            double lon = 360.0 * (j + 0.5) / nrow_bins - 180.0;
            if (lon >= min_lon && lon <= max_lon && row_lat >= min_lat && row_lat <= max_lat) {
                lats[aoi_bin] = row_lat;
                lons[aoi_bin] = lon;
                bins[aoi_bin] = bin;
                nbins_in_row[row]++;
                aoi_bin++;
            }
            bin++;
        }
        if (row_lat >= min_lat && row_lat <= max_lat) {
            row++;
        }
    }
}

int binary_search(int *arr, int l, int r, int x)
{
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return binary_search(arr, l, mid - 1, x);

        return binary_search(arr, mid + 1, r, x);
    }
    return -1;
}

/*
 * Function:  initialize
 * --------------------
 * Fills an array with integer values of the input data ranging from 0 to 255 with missing data filled with the fill value.
 *
 * args:
 *      double *in_data: pointer to an array containing the input data
 *      int *out_data: pointer to the output array
 *      int nbins: number of bins in the area of interest
 *      int ndata_bins: number of data containing bins
 *      int *data_bins: pointer to an array containing the bin number for each data containing bin
 *      int *bins: pointer to an array containing the bin number for all bins in the area of interest
 */

void initialize(double *in_data, int *out_data, int nbins, int ndata_bins, int *data_bins, const int *bins) {
    double min_value = 999.;
    double max_value = -999.;
    for (int k = 0; k < ndata_bins; k++) {
        if (in_data[k] < min_value) {
            min_value = in_data[k];
        } else if (in_data[k] > max_value) {
            max_value = in_data[k];
        }
    }

    for (int i = 0; i < nbins; i++) {
        out_data[i] = FILL_VALUE
        int idx = binary_search(data_bins, 0, ndata_bins, bins[i]);
        if (idx == -1) {
            out_data[i] = FILL_VALUE;
        } else {
            double ratio = (double)(in_data[idx] + fabs(min_value)) / fabs(max_value - min_value);
            out_data[i] = (int) (ratio * 255);
        }
    }

    for (int i = 0; i < ndata_bins; i++) {
        double ratio = (double)(in_data[idx] + fabs(min_value)) / fabs(max_value - min_value);
        out_data[] = (int) (ratio * 255);
    }
}
