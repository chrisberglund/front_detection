import unittest
target = __import__("main")

class TestSum(unittest.TestCase):

    def test_init(self):
        detector = target.edgeDetector(23761676, 4320, -90, -180, 90, 180)
        self.assertEqual(detector.num_aoi_bins, 23761676, "Wrong number of bins for whole planet")

    def test_sum_tuple(self):
        self.assertEqual(target.foo(2,1), 5, "Should be 6")

if __name__ == '__main__':
    unittest.main()
