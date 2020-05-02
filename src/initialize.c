/*
 * Functions for calculating the integer data values to be used in the single image edge detection algorithm
 */

#include "initialize.h"
#include <stdlib.h>
#include <math.h>
#include "cayula.h"

typedef struct coordinates {
    double latitude;
    double longitude;
} Coordinates;

void bin2latlon(int bin, int row, const int *nBinsInRow, const double *latrows, int *basebins, int nrows,
                coordinates *coords) {
    if (bin < 1) {
        bin = 1;
    }
    double clat = latrows[row];
    double clon;
    clon = 360.0 * (bin - basebins[row] + 0.5) / nBinsInRow[row] - 180.0;
    coords->latitude = clat;
    coords->longitude = clon;
}

/*
 * Function:  getLatLon
 * --------------------
 * Calculates the latitude and longitude values for each bin
 *
 * args:
 *      double *lats: pointer to an output array for latitude values
 *      double *lons: pointer to an output array for longitude values
 *      int *outRows: pointer to an output array for the row number for each bin
 *      int *outBins: pointer to an output array for the bin number for each bin. These bin numbers begin with 0
 *      int nrows: the number of rows in the binning scheme
 *      int *dataBins: pointer to an array containing the bin number for each data containing bin
 *      int nbins: number of bins in the binning scheme
 */
void getLatLon(double *lats, double *lons, int *outRows, int *outBins, int nrows, int nbins) {
    double *latrows = (double *) malloc(sizeof(double) * nrows);
    int *nBinsInRow = (int *) malloc(nrows * sizeof(int));
    int *basebins = (int *) malloc(nrows * sizeof(int));
    /*
     * Get the latitude value for each row and the bin number of the first bin in each row
     */
    for (int i = 0; i < nrows; ++i) {
        latrows[i] = ((i + 0.5) * 180.0 / nrows) - 90;
        nBinsInRow[i] = (int) (2 * nrows * cos(latrows[i] * M_PI / 180.0) + 0.5);
        if (i == 0) {
            basebins[i] = 1;
        } else {
            basebins[i] = basebins[i - 1] + nBinsInRow[i - 1];
        }
    }
    coordinates *coords;
    coords = (coordinates *) malloc(sizeof(coordinates));
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        if (row + 1 < nrows && i >= basebins[row + 1]) {
            row++;
        }
        outBins[i] = i;
        outRows[i] = row;
        bin2latlon(outBins[i], row, nBinsInRow, latrows, basebins, nrows, coords);
        lats[i] = coords->latitude;
        lons[i] = coords->longitude;
    }

    free(coords);
    free(latrows);
}


int aoi_bins_length(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon) {
    int counter = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 100. / nrows - 90;
        if (row_lat >= min_lat && row_lat <= max_lat) {
            int nbins_in_row = (int) (2 * nrows * cos(row_lat * M_PI / 180.) + 0.5);
            for (int j = 0; j < nbins_in_row; j++) {
                double lon = 360.0 * (j + 0.5) / n_bins_in_row - 180.0;
                if (lon >= min_lon && lon <= max_lon) counter++;
            }
        }
    }
    return counter;
}

int aoi_rows_length(int nrows, double min_lat, double max_lat) {
    int counter = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 100. / nrows - 90;
        if (row_lat >= min_lat && row_lat <= max_lat) counter++;
    }
    return counter;
}

void get_latlon(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon, int *basebins,
        int *nbins_in_row double *lats, double *lons, int *bins) {
    int aoi_bin = 0;
    int bin = 1;
    int row = 0;
    for (int i = 0; i < nrows; i++) {
        double row_lat = (i + 0.5) * 100. / nrows - 90;
        if (row_lat >= min_lat && row_lat <= max_lat) row++;
        nbins_in_row[row] = (int) (2 * nrows * cos(row_lat * M_PI / 180.) + 0.5);
        basebins[row] = aoi_bin;
        row++;
        for (int j = 0; j < nbins_in_row[row]; j++) {
            double lon = 360.0 * (j + 0.5) / n_bins_in_row[row] - 180.0;
            if (lon >= min_lon && lon <= max_lon && row_lat >= min_lat && row_lat <= max_lat) {
                lats[aoi_bin] = row_lat;
                lons[aoi_bin] = lon;
                bins[aoi_bin] = bin;
                aoi_bin++;
            }
            bin++;
        }
    }
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

void initialize(int *in_data, int *out_data, int nbins, int ndata_bins, int *data_bins, const int *bins) {

    int i = 0, j = 0;
    int min_value = 999;
    int max_value = -999;
    for (int k = 0; k < ndata_bins; k++) {
        if (in_data[k] < min_value) {
            min_value = in_data[i];
        } else if (in_data[k] > max_value) {
            max_value = in_data[k]
        }
    }

    while (i < ndata_bins || j < nbins) {
        if (data_bins[i] == bins[j]) {
            double ratio = (double)(in_data[i] + fabs(min_value)) / fabs(max_value - min_value);
            out_data[bins[j] - 1] = (int) (ratio * 255);
            if (i < ndata_bins) ++i;
            if (j < nbins) ++j;
        } else if (a[i] > b[j] && j < 12) {
            out_data[bins[j] - 1] = FILL_VALUE;
            ++j;
        } else {
            ++i;
        }
    }
}
