import pandas as pd
import numpy as np
from pathlib import Path
from netCDF4 import Dataset
from calendar import monthrange

def find_aoi_bins(nrows, min_lat, max_lat, min_lon, max_lon):
    row_lats = (np.arange(0,  nrows, dtype=np.double) + 0.5) * 180. / nrows - 90
    nbins_in_row = np.floor(2 * nrows * np.cos(row_lats * np.pi / 180.) + 0.5).astype(np.int)
    lons = []
    lats = np.repeat(row_lats, nbins_in_row)

    for i in range(0, nrows):
        lons.extend((360. * (np.arange(0, nbins_in_row[i], dtype=np.double) + 0.5) / nbins_in_row[i] - 180.).tolist())

    lats_idx = ((lats >= min_lat) & (lats <= max_lat)).nonzero()
    lons = np.array(lons)
    lons_idx = ((lats >= min_lon) & (lons <= max_lon)).nonzero()
    aoi_bins = np.intersect1d(lats_idx, lons_idx)
    return lats[aoi_bins], lons[aoi_bins], aoi_bins

def read_modis(filename):
    dataset = Dataset(filename)
    binlist = np.array(dataset["level-3_binned_data"]["BinList"][:].tolist())
    bins = binlist[:, 0].astype("int") - 1
    weights = binlist[:, 3]
    sums = np.array(dataset.groups["level-3_binned_data"]["sst"][:].tolist())[:, 0]
    data = sums / weights
    date = dataset.time_coverage_start
    dataset.close()
    return pd.DataFrame({"Bin": bins, "Data": data, "Date":date})

def main():
    dataset = Dataset("input/AQUA_MODIS.20200724.L3b.DAY.SST.nc")
    nrows = len(dataset.groups["level-3_binned_data"]["BinIndex"])
    lats, lons, aoi_bins = find_aoi_bins(nrows, 20, 80, -180, -120)
    dataset.close()
    path = Path("input/")
    for month in range(6, 12):
        print(month)
        for day in range(1, monthrange(2003, month)[1] + 1):
            print(day)
            df = pd.concat([read_modis(p) for p in path.rglob("AQUA_MODIS.????0918*")])
            df["Date"] = pd.to_datetime(df["Date"])
            df = df[df["Bin"].isin(aoi_bins)]
            mean = df.groupby("Bin").mean()
            df = df.set_index(["Bin","Date"])
            anom = (df["Data"] - mean["Data"]).reset_index()
            latlons = pd.DataFrame({"Latitude": lats, "Longitude": lons, "Bin": aoi_bins})
            anom = pd.merge(left=anom, right=latlons, on="Bin").drop("Bin", axis=1)
            for year in range(2003, 2019):
                anom[anom["Date"].dt.year == year].to_csv(f"/anomaly/{year}_{month}_{day}_sstanom.csv",index=False)


if __name__ == "__main__":
    main()   