#include "unity.h"

#include "helpers.h"

const int FILL_VALUE = -999;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_get_window_no_fill(void)
{
    int arr[102] = {148, 66, 169, 185, 255, 241,
                    245, 216, 38, 110, 127, 2, 203,
                    134, 99, 227, 186, 19, 173, 179, 51,
                      141, 48, 221, 232, 72, 50, 166, 187, 11,
                    181, 45, 17,  195, 53, 121, 252, 164, 39, 57,
                    153, 6, 150, 226, 113, 202, 233, 133, 230, 160, 149,
                    155, 211, 171, 31, 97, 8, 49, 123, 78, 95, 157,
                    128, 183, 234, 62, 138, 143, 71, 126, 147, 239,
                    7, 26, 3, 58, 207, 35, 122, 40, 129,
                    115, 1, 42, 83, 75, 244, 188, 214,
                    112, 55, 246, 47, 105, 98, 92,
                    114, 88, 29, 193, 180, 24};
    int n_bins_in_row[12] = {6,7,8,9,10,11,11,10,9,8,7,6};
    int basebins[12] = {0, 6, 13, 21, 30, 40, 51, 62, 72, 81, 89, 96};
    int window[9];
    int expected_window[9] = {221,232, 72, 17, 195, 53, 150, 226, 113};
    int n_invalid = get_window(33, 4, 3, arr, n_bins_in_row, basebins, window);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected_window, window, 9);
    TEST_ASSERT_EQUAL_INT(0, n_invalid);
}

void test_get_window_fill_values(void) {
    int arr[102] = {148, 66, 169, 185, 255, 241,
                    245, 216, 38, 110, 127, 2, 203,
                    134, 99, 227, 186, 19, 173, 179, 51,
                    141, 48, 221, 232, 72, 50, 166, 187, 11,
                    181, 45, FILL_VALUE,  195, 53, 121, 252, 164, 39, 57,
                    153, 6, 150, 226, FILL_VALUE, 202, 233, 133, 230, 160, 149,
                    155, 211, 171, 31, 97, 8, 49, 123, 78, 95, 157,
                    128, 183, 234, 62, 138, 143, 71, 126, 147, 239,
                    7, 26, 3, 58, 207, 35, 122, 40, 129,
                    115, 1, 42, 83, 75, 244, 188, 214,
                    112, 55, 246, 47, 105, 98, 92,
                    114, 88, 29, 193, 180, 24};
    int n_bins_in_row[12] = {6,7,8,9,10,11,11,10,9,8,7,6};
    int basebins[12] = {0, 6, 13, 21, 30, 40, 51, 62, 72, 81, 89, 96};
    int window[9];
    int expected_window[9] = {221,232, 72, FILL_VALUE, 195, 53, 150, 226, FILL_VALUE};
    int n_invalid = get_window(33, 4, 3, arr, n_bins_in_row, basebins, window);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected_window, window, 9);
    TEST_ASSERT_EQUAL_INT(2, n_invalid);

    //TEST_ASSERT_EQUAL_DOUBLE(0.2,0.2);
}

void test_get_window_even(void) {
    int data[81] = {
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0
    };
    int window[16];
    int basebins[9];
    int nbins_in_row[9];
    for (int i = 0; i < 9; i++) {
        basebins[i] = i * 9;
        nbins_in_row[i] = 9;
    }
    int expected_window[16] = {1, 0, 1, 0,1, 0, 1, 0,1, 1, 0, 0,0, 0, 0, 0,};
    get_window(39, 4, 4, data, nbins_in_row, basebins, window);
    TEST_ASSERT_EQUAL_INT_ARRAY(expected_window, window, 16);
}