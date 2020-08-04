import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count

class EdgeDetector:

    def __find_aoi_bins(self):
        row_lats= (np.arange(0,  self.nrows, dtype=np.double) + 0.5) * 180. / self.nrows - 90
        nbins_in_row = np.floor(2 * self.nrows * np.cos(row_lats * np.pi / 180.) + 0.5).astype(np.int)
        lons = []
        lats = np.repeat(row_lats, nbins_in_row)

        for i in range(0, self.nrows):
            lons.extend((360. * (np.arange(0, nbins_in_row[i], dtype=np.double) + 0.5) / nbins_in_row[i] - 180.).tolist())

        lats_idx = ((lats >= self.min_lat) & (lats <= self.max_lat)).nonzero()
        lons = np.array(lons)
        lons_idx = ((lats >= self.min_lon) & (lons <= self.max_lon)).nonzero()
        aoi_bins = np.intersect1d(lats_idx, lons_idx)
        _, nbins_in_row = np.unique(lats[aoi_bins], return_counts=True)
        basebins = np.cumsum(nbins_in_row) - nbins_in_row
        basebins = (ctypes.c_int * self.num_aoi_rows)(*basebins)
        nbins_in_row = (ctypes.c_int * self.num_aoi_rows)(*nbins_in_row)
        aoi_bins = (ctypes.c_int * self.num_aoi_bins)(aoi_bins)
        return lats[aoi_bins], lons[aoi_bins], basebins, nbins_in_row, aoi_bins

    def __init__(self, nbins, nrows, min_lat, min_lon, max_lat, max_lon):
        self.nbins = nbins
        self.nrows = nrows
        self.min_lat = min_lat
        self.min_lon = min_lon
        self.max_lat = max_lat
        self.max_lon = max_lon
        self.num_aoi_bins = nbins
        self.num_aoi_rows =  nrows
        self.lats, self.lons, self.basebins, self.nbins_in_row, self.aoi_bins = self.__find_aoi_bins()

    def initialize(self, data, data_bins):
        min_val = np.min(data)
        max_val = np.max(data)
        int_data = np.floor(255 * (data + abs(min_val)) / abs(max_val - min_val)).astype(np.int)
        index = range(0, len(self.aoi_bins))
        aoi_bins = np.array(self.aoi_bins[:])
        sorted_index = np.searchsorted(aoi_bins, data_bins)

        yindex = np.take(index, sorted_index, mode="clip")
        mask = aoi_bins[yindex] == data_bins

        aoi_idx = yindex[mask]

        index = range(0, len(data_bins))
        sorted_index = np.searchsorted(data_bins, aoi_bins)

        yindex = np.take(index, sorted_index, mode="clip")
        mask = data_bins[yindex] == aoi_bins

        data_idx = yindex[mask]

        aoi_data = np.full(self.num_aoi_bins, -999)
        aoi_data[aoi_idx] = int_data[data_idx]
        return aoi_data


    def sied(self, data, data_bins):
        _cayula = ctypes.CDLL('./sied.so')
        aoi_data = self.initialize(data, data_bins)
        aoi_data_arr = (ctypes.c_int * self.num_aoi_bins)(*aoi_data)
        _cayula.cayula.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int),
                                   ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int))
        out_data = (ctypes.c_int * self.num_aoi_bins)()
        _cayula.cayula(aoi_data_arr, out_data, self.num_aoi_bins, self.num_aoi_rows, self.nbins_in_row, self.basebins)
        df = pd.DataFrame(data={"Data": out_data[:self.num_aoi_bins]})
        df["Latitude"] =  self.lats
        df["Longitude"] = self.lons
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
    detector = EdgeDetector(ntotal_bins, nrows, 20, -180, 80, -120)
    dataset.close()
    for file in files:
        dataset = Dataset(file)
        ntotal_bins, nrows, data_bins, data, date = get_params_modis(dataset, "sst")
        df = detector.sied(data, data_bins)
        df = df[df["Data"] > -1]

        print(df.groupby("Data").count())

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


def main():
    cwd = os.getcwd()
    map_files(cwd + "/input", -60, 60, -180, 0)


if __name__ == "__main__":
    main()