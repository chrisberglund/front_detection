import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count


def boa(total_bins, nrows, fill_value, rows, bins, data, weights, date, chlor_a=False, glob=False):
    """
    Performs the Belkin-O'Reilly front detection algorithm on the provided bins
    :param total_bins: total number of bins in the binning scheme
    :param nrows: number of rows in the binning scheme
    :param fill_value: value to fill empty bins with
    :param bins: bin numbers for all data containing bins
    :param data: weighted sum of the data for each bin
    :param weights: weights used to calculate weighted sum for each bin
    :param date: date of the temporal bin
    :param chlor_a: if the data is chlorophyll a concentration, the natural lograithm of the data will be used
    in edge detection default is false
    :return: pandas dataframe containing bin values of each bin resulting from edge detection algorithm
    """
    _cayula = ctypes.CDLL('./sied.so')
    _cayula.cayula.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int),
                               ctypes.POINTER(ctypes.c_int),
                               ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                               ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                               ctypes.POINTER(ctypes.c_int), ctypes.c_bool)
    bins_array_type = ctypes.c_int * len(bins)
    lats = (ctypes.c_double * total_bins)()
    lons = (ctypes.c_double * total_bins)()
    rows = (ctypes.c_int * len(rows))(*rows)
    data_array = (ctypes.c_double * len(data))(*data)
    data_out = (ctypes.c_int * total_bins)()
    weights_array = (ctypes.c_double * len(bins))(*weights)
    _cayula.cayula(total_bins, len(bins), nrows, fill_value, bins_array_type(*bins), rows, data_array, weights_array, lats,
                   lons,
                   data_out, chlor_a)
    lats = list(lats)
    lons = list(lons)
    final_data = list(data_out)
    df = pd.DataFrame(
        data={"Latitude": lats, "Longitude": lons, "Data": final_data, "Date": np.repeat(date, len(lats))})
    return df


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
    bins = binlist[:, 0].astype("int")
    weights = binlist[:, 3]
    data = np.array(dataset.groups["level-3_binned_data"][data_str][:].tolist())[:, 0]
    date = dataset.time_coverage_start

    return total_bins, nrows, bins, data, weights, date


def get_params_glob(dataset, data_str):
    total_bins = dataset.nb_grid_bins
    nrows = 4320
    bins = dataset.variables["col"][:]
    rows = dataset.variables["row"][:]
    weights = []
    data = dataset.variables["CHL1_mean"][:]
    date = dataset.period_start_day
    return total_bins, nrows, rows, bins, data, weights, date


def map_bins(dataset, latmin, latmax, lonmin, lonmax, glob):
    """
    Takes a netCDF4 dataset of binned satellite data and creates a geodataframe with coordinates and bin data values
    :param dataset: netCDF4 dataset containing bins and data values
    :param latmin: minimum latitude to include in output
    :param latmax: maximum latitude to include in output
    :param lonmin: minimum longitude to include in output
    :param lonmax: maximum longitude to include in output
    :return: geodataframe containing latitudes, longitudes, and data values of all bins within given extent
    """
    if glob:
        total_bins, nrows, rows, bins, data, weights, date = get_params_glob(dataset, "chlor_a")
    else:
        total_bins, nrows, bins, data, weights, date = get_params_modis(dataset, "chlor_a")
        rows = []
    df = boa(total_bins, nrows, -999, rows, bins, data, weights, date, True, glob)
    print("Cropping")
    df = df[(df.Latitude >= latmin) & (df.Latitude <= latmax) &
            (df.Longitude >= lonmin) & (df.Longitude <= lonmax)]
    df = df[df['Data'] > -999]
    return df


def map_file(args):
    cwd = os.getcwd()
    dataset = Dataset(args["file"])
    if args["glob"]:
        year_month = dataset.period_start_day[-2:]
        date = dataset.period_start_day
    else:
        year_month = dataset.time_coverage_start[:7]
        date = dataset.time_coverage_start[:10]
    if args["file"].endswith("SNPP_CHL.nc"):
        outfile = date + "viirs_chlor.csv"
    else:
        outfile = date + '_chlor.csv'

    if outfile not in args["outfiles"]:
        df = map_bins(dataset, args["latmin"], args["latmax"], args["lonmin"], args["lonmax"], args["glob"])
        dataset.close()
        if not os.path.exists(cwd + "/out/" + year_month):
            os.makedirs(cwd + "/out/" + year_month)
        try:
            df.to_csv(cwd + "/out/" + year_month + "/" + outfile, index=False)
        except IOError as err:
            print("Error while attempting to save shapefile:", err)

        print("Finished writing file %s", outfile)


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
    glob = False
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
            files.append({"file": directory + "/" + file, "latmin": latmin,
                          "latmax": latmax, "lonmin": lonmin, "lonmax": lonmax, "glob": glob, "outfiles": outfiles})

    pool = Pool(1)
    pool.map(map_file, files)


def main():
    cwd = os.getcwd()
    map_files(cwd + "/input", -80, 80, -180, 0)


if __name__ == "__main__":
    main()
