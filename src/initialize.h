#ifndef INITIALIZE_H
#define INITIALIZE_H
#include <stdbool.h>

int aoi_bins_length(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon);
int aoi_rows_length(int nrows, double min_lat, double max_lat);
void get_latlon(int nbins, int nrows, double min_lat, double min_lon, double max_lat, double max_lon, int *basebins,
                int *nbins_in_row, double *lats, double *lons, int *bins);
void initialize(int *in_data, int *out_data, int nbins, int ndata_bins, int *data_bins, const int *bins);
#endif //INITIALIZE_H