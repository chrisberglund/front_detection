import unittest
target = __import__("main.py")

class TestSum(unittest.TestCase):

    def test_sum(self):
        self.assertEqual(target.foo(2,1), 3, "Should be 6")

    def test_sum_tuple(self):
        self.assertEqualtarget.foo(2,1), 5, "Should be 6")

if __name__ == '__main__':
    unittest.main()