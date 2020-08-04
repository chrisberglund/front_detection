import unittest
import numpy as np
target = __import__("main")

class TestSum(unittest.TestCase):

    def test_init(self):
        detector = target.EdgeDetector(23761676, 4320, -90, -180, 90, 180)
        self.assertEqual(detector.num_aoi_bins, 23761676, "Wrong number of bins for whole planet")
        self.assertEqual(len(detector.lats), 23761676, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 23761676, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 23761676, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 4320, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), -90, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), -180, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 90, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 180, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 0, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[23761675], 23761675, "Last bin number is invalid")

        self.assertEqual(detector.nbins, detector.num_aoi_bins, "These should be equal")

    def test_init_min_lat(self):
        detector = target.EdgeDetector(23761676, 4320, 0, -180, 90, 180)
        self.assertEqual(detector.num_aoi_bins, 11880838, "Wrong number of bins for minimum lat")
        self.assertEqual(len(detector.lats), 11880838, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 11880838, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 11880838, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 2160, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), 0, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), -180, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 90, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 180, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 11880838, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[11880837], 23761675, "Last bin number is invalid")

    def test_init_min_lon(self):
        detector = target.EdgeDetector(23761676, 4320, -90, 0, 90, 180)
        self.assertEqual(detector.num_aoi_bins, 11881908, "Wrong number of bins for minimum lon")
        self.assertEqual(len(detector.lats), 11881908, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 11881908, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 11881908, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 4320, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), -90, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), 0, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 90, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 180, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 1, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[11881907], 23761675, "Last bin number is invalid")

    def test_init_max_lat(self):
        detector = target.EdgeDetector(23761676, 4320, -90, -180, 0, 180)
        self.assertEqual(detector.num_aoi_bins, 11880838, "Wrong number of bins for maximum lat")
        self.assertEqual(len(detector.lats), 11880838, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 11880838, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 11880838, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 2160, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), -90, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), -180, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 0, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 180, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 0, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[11880837], 11880837, "Last bin number is invalid")

    def test_init_max_lon(self):
        detector = target.EdgeDetector(23761676, 4320, -90, -180, 90, 0)
        self.assertEqual(detector.num_aoi_bins, 11881908, "Wrong number of bins for minimum lon")
        self.assertEqual(len(detector.lats), 11881908, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 11881908, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 11881908, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 4320, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), -90, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), -180, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 90, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 0, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 0, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[11881907], 23761674, "Last bin number is invalid")
    """
    def test_create_from_netCDF(self):
        detector = target.EdgeDetector.create_from_netCDF("./input/AQUA_MODIS.20020704.L3b.DAY.SST.nc", -90, -180, 90, 0)
        self.assertEqual(detector.num_aoi_bins, 11881908, "Wrong number of bins for minimum lon")
        self.assertEqual(len(detector.lats), 11881908, "Wrong number of latitudes")
        self.assertEqual(len(detector.lons), 11881908, "Wrong number of longitudes")
        self.assertEqual(len(detector.aoi_bins), 11881908, "Wrong number of aoi_bins")
        self.assertEqual(len(detector.nbins_in_row), 4320, "Wrong number of nbins_in_row")

        self.assertGreaterEqual(np.min(detector.lats), -90, "Smallest lat outside range")
        self.assertGreaterEqual(np.min(detector.lons), -180, "Smallest lon outside range")
        self.assertLessEqual(np.max(detector.lats), 90, "Largest lat outside range")
        self.assertLessEqual(np.max(detector.lons), 0, "Largest lon outside range")

        self.assertEqual(detector.aoi_bins[0], 0, "First bin number is invalid")
        self.assertEqual(detector.aoi_bins[11881907], 23761674, "Last bin number is invalid")

    def test_get_data_from_netCDF(self):
        detector = target.EdgeDetector.create_from_netCDF("./input/AQUA_MODIS.20020704.L3b.DAY.SST.nc", -90, -180, 90, 0)
        data, data_bins, date = detector.get_data_from_netCDF("./input/AQUA_MODIS.20020704.L3b.DAY.SST.nc", "sst")
        self.assertGreaterEqual(data_bins[0], 0, "Smallest bin number is too small")
        self.assertLessEqual(data_bins[-1], 23761674, "Largest bin number is too big")
    """

if __name__ == '__main__':
    unittest.main()
