import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count

class EdgeDetector:

    def __find_num_aoi_bins(self, nbins, nrows, min_lat, min_lon, max_lat, max_lon):
        _cayula = ctypes.CDLL('./sied.so')
        _cayula.aoi_bins_length.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double)
        _cayula.aoi_rows_length.argtypes = (ctypes.c_int, ctypes.c_double, ctypes.c_double)
        num_aoi_bins = _cayula.aoi_bins_length(nbins, nrows, min_lat, min_lon, max_lat, max_lon)
        num_aoi_rows = _cayula.aoi_rows_length(nrows, min_lat, max_lat)

        return num_aoi_bins, num_aoi_rows

    def __find_aoi_bins(self):
        _cayula = ctypes.CDLL('./sied.so')
        _cayula.get_latlon.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                       ctypes.c_double,
                                       ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                                       ctypes.POINTER(ctypes.c_double),
                                       ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_int))

        lats = (ctypes.c_double * self.num_aoi_bins)()
        lons = (ctypes.c_double * self.num_aoi_bins)()
        basebins = (ctypes.c_int * self.num_aoi_rows)()
        nbins_in_row = (ctypes.c_int * self.num_aoi_rows)()
        aoi_bins = (ctypes.c_int * self.num_aoi_bins)()
        _cayula.get_latlon(self.nbins, self.nrows, self.min_lat, self.min_lon, self.max_lat, self.max_lon, basebins, nbins_in_row, lats, lons, aoi_bins)
        return lats, lons, basebins, nbins_in_row, aoi_bins

    def __init__(self, nbins, nrows, min_lat, min_lon, max_lat, max_lon):
        self.nbins = nbins
        self.nrows = nrows
        self.min_lat = min_lat
        self.min_lon = min_lon
        self.max_lat = max_lat
        self.max_lon = max_lon
        self.num_aoi_bins, self.num_aoi_rows = self.__find_num_aoi_bins(nbins, nrows, min_lat, min_lon, max_lat, max_lon)
        self.lats, self.lons, self.basebins, self.nbins_in_row, self.aoi_bins = self.__find_aoi_bins()

    def sied(self, data, data_bins):
        _cayula = ctypes.CDLL('./sied.so')
        data_arr = (ctypes.c_double * len(data_bins))(*data)
        data_bins_arr = (ctypes.c_int * len(data_bins))(*data_bins)
        _cayula.initialize.argtypes = (ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_int),
                                       ctypes.c_int, ctypes.c_int,ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
        aoi_data = (ctypes.c_int * self.num_aoi_bins)()
        _cayula.initialize(data_arr, aoi_data, self.nbins, len(data_bins), data_bins_arr, self.aoi_bins)
        _cayula.cayula.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                                   ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
        out_data = (ctypes.c_int * self.num_aoi_bins)()
        _cayula.cayula(aoi_data, out_data, self.nbins, self.nrows, self.nbins_in_row, self.basebins)

        df = pd.DataFrame(data={"Data": out_data[:self.num_aoi_bins]})
        df["latitude"] =  self.lats[:self.num_aoi_bins]
        df["Longitude"] = self.lons[:self.num_aoi_bins]
        print(df.groupby("Data").count())
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
    bins = binlist[:, 0].astype("int") - 1
    weights = binlist[:, 3]
    sums = np.array(dataset.groups["level-3_binned_data"][data_str][:].tolist())[:, 0]
    data = sums / weights
    date = dataset.time_coverage_start

    return total_bins, nrows, bins, data, date

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
        if 'ENVISAT' in file:
            year = file[17:21] + '-' + file[21:23] + '-' + file[23:25] + 'meris_chlor.csv'
        elif 'V20' in file:
            dataset = Dataset(directory + '/' + file)
            date = dataset.time_coverage_start[:10]
            year = date + 'viirs_chlor.csv'
            dataset.close()
        elif file.endswith(".nc"):
            dataset = Dataset(directory + '/' + file)
            date = dataset.time_coverage_start[:10]
            year = date + '_sst.csv'
            dataset.close()
        if file.endswith(".nc"):
            if year not in outfiles:
                files.append(directory + "/" + file)

    dataset = Dataset(files[0])
    ntotal_bins, nrows, data_bins, data, date = get_params_modis(dataset, "sst")
    detector = EdgeDetector(ntotal_bins, nrows, -90, -180, 90, 180)
    dataset.close()
    for file in files:
        dataset = Dataset(file)
        ntotal_bins, nrows, data_bins, data, date = get_params_modis(dataset, "sst")
        out_data = detector.sied(data, data_bins)
        #print(df.groupby("Data").count())
        """
        year_month = dataset.time_coverage_start[:4]
        date = dataset.time_coverage_start[:10]
        if "SNPP" in file:
            outfile = date + "viirs_chlor.csv"
        elif "SEASTAR" in file:
            outfile = date + "seawifs_chlor.csv"
        elif "ENVISAT_MERIS" in file:
            outfile = date + "meris_chlor.csv"
        else:
            outfile = date + '_sst.csv'
        dataset.close()
        if not os.path.exists(cwd + "/out/" + year_month):
            os.makedirs(cwd + "/out/" + year_month)
        df.to_csv(cwd + "/out/" + year_month + "/" + outfile, index=False)
        print("Saving " + outfile)
        """

def main():
    cwd = os.getcwd()
    map_files(cwd + "/input", -60, 60, -180, 0)


if __name__ == "__main__":
    main()
