import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count


def initialize(nbins, nrows, min_lat, min_lon, max_lat, max_lon):
    _cayula = ctypes.CDLL('./sied.so')
    _cayula.aoi_bins_length.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double)
    _cayula.aoi_rows_length.argtypes = (ctypes.c_int, ctypes.c_double, ctypes.c_double)
    _cayula.get_latlon.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double,
                                ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                                ctypes.POINTER(ctypes.c_double),
                                ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_int))
    num_aoi_bins = _cayula.aoi_bins_length(nbins, nrows, min_lat, min_lon, max_lat, max_lon)
    num_aoi_rows = _cayula.aoi_rows_length(nrows, min_lat, max_lat)

    lats = (ctypes.c_double * num_aoi_bins)()
    lons = (ctypes.c_double * num_aoi_bins)()
    basebins = (ctypes.c_int * num_aoi_rows)()
    nbins_in_row = (ctypes.c_int * num_aoi_rows)()
    aoi_bins = (ctypes.c_int * num_aoi_bins)()
    _cayula.get_latlon(nbins, nrows, min_lat, min_lon, max_lat, max_lon, basebins, nbins_in_row, lats, lons, aoi_bins)

    return basebins, nbins_in_row, lats, lons, num_aoi_rows, num_aoi_bins, aoi_bins


def get_params_modis(dataset, data_str):
    """
    Parses values from netCDF4 file for use in Belkin-O'Reilly algorithm
    :param dataset: netCDF4 object containing data
    :param data_str: string key for data in netCDF4 Dataset object
    :return: the total number of bins in binning scheme, the number of rows, list of all data containing bins,
    data value for each bin, weight value for each bin
    """
    total_bins = np.array(dataset.groups["level-3_binned_data"]["BinIndex"][:].tolist())[:, 3].sum()
    nrows = len(dataset.groups["level-3_binned_data"]["BinIndex"])
    binlist = np.array(dataset["level-3_binned_data"]["BinList"][:].tolist())
    bins = binlist[:, 0].astype("int") - 1
    weights = binlist[:, 3]
    sums = np.array(dataset.groups["level-3_binned_data"][data_str][:].tolist())[:, 0]
    data = np.log10(sums / weights)
    date = dataset.time_coverage_start

    return total_bins, nrows, bins, data, date


def sied(data, nbins, nrows, ndata_bins, data_bins, aoi_bins, basebins, nbins_in_row):
    _cayula = ctypes.CDLL('./sied.so')
    _cayula.initialize.argtypes = (ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_int),
                                ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
    in_data = (ctypes.c_double * ndata_bins)(*data)
    out_data = (ctypes.c_int * nbins)()
    data_bins = (ctypes.c_int * ndata_bins)(*data_bins)
    _cayula.initialize(in_data, out_data, nbins, ndata_bins, data_bins, aoi_bins)
    _cayula.cayula.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                            ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))

    in_data = out_data
    out_data = (ctypes.c_int * nbins)()
    _cayula.cayula(in_data, out_data, nbins, nrows, nbins_in_row, basebins)
    return list(out_data)


def map_files(directory, latmin, latmax, lonmin, lonmax):
    """
    Takes a directory of netCDF4 files of binned satellite data and creates shapefiles containing the values from
    the edge detection algorithm for each bin
    :param directory: directory path containing all netCDF4 files
    :param latmin: minimum latitude to include in output
    :param latmax: maximum latitude to include in output
    :param lonmin: minimum longitude to include in output
    :param lonmax: maximum longitude to include in output
    """
    cwd = os.getcwd()
    files = []
    outfiles = []
    if not os.path.exists(cwd + "/out"):
        os.makedirs(cwd + "/out")
    for root, dirs, file_names in os.walk(cwd + "/out"):
        for file in file_names:
            if file.endswith(".csv"):
                outfiles.append(file)
    outfiles.sort()
    for file in os.listdir(directory):
        if file.endswith(".nc"):
            files.append(directory + "/" + file)

    dataset = Dataset(files[0])
    ntotal_bins, nrows, data_bins, data, date = get_params_modis(dataset, "chlor_a")
    basebins, nbins_in_row, lats, lons, num_aoi_rows, num_aoi_bins, aoi_bins = initialize(ntotal_bins, nrows, 20., -180.,
                                                                                          80., -110.)
    dataset.close()
    for file in files:
        dataset = Dataset(file)
        ntotal_bins, nrows, data_bins, data, date = get_params_modis(dataset, "chlor_a")

        out_data = sied(data, num_aoi_bins, num_aoi_rows, len(data_bins), data_bins, aoi_bins, basebins, nbins_in_row)
        df = pd.DataFrame({"Latitude": list(lats), "Longitude": list(lons), "Data": out_data})
        df = df[df["Data"] > -999]
        year_month = dataset.time_coverage_start[:7]
        date = dataset.time_coverage_start[:10]
        if file.endswith("SNPP_CHL.nc"):
            outfile = date + "viirs_chlor.csv"
        else:
            outfile = date + '_chlor.csv'
        dataset.close()
        if not os.path.exists(cwd + "/out/" + year_month):
            os.makedirs(cwd + "/out/" + year_month)
        df.to_csv(cwd + "/out/" + year_month + "/" + outfile, index=False)


def main():
    cwd = os.getcwd()
    map_files(cwd + "/input", -60, 60, -180, 0)


if __name__ == "__main__":
    main()
