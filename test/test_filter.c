#include "unity.h"
#include <stdlib.h>
#include "helpers.h"
#include "filter.h"

const int FILL_VALUE = -999;

void setUp(void) {
}

void tearDown(void) {
}
/*
void test_filter_median9(void) {
    int arr[9] = {144,233,178,102, 72, 1, 52, 246, 254};
    int mdn = median9(arr);
    TEST_ASSERT_EQUAL_INT(144, mdn);
    int arr2[9] = {145,145,145,145,145,145,145,145,145};
    mdn = median9(arr2);
    TEST_ASSERT_EQUAL_INT(145, mdn);
}

void test_filter_medianN(void) {
    int arr[9] = {FILL_VALUE, 222, 147, 3, 254, 29, FILL_VALUE, 3, 99};
    int mdn = medianN(arr, 2);
    TEST_ASSERT_EQUAL_INT(99, mdn);
    int arr2[9] = {FILL_VALUE, 108, FILL_VALUE, 42, FILL_VALUE, 232, 19, 188, 69};
    mdn = medianN(arr2, 3);
    TEST_ASSERT_EQUAL_INT(89, mdn);
    int arr3[9] = {FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE,FILL_VALUE};
    mdn = medianN(arr3, 9);
    TEST_ASSERT_EQUAL_INT(-999, mdn);
}*/

void test_filter_median_filter_no_fill(void) {

    int arr[144] = {148, 66, 169, 185, 255, 241, 84, 41, 80, 100, 136, 74,
                    245, 216, 38, 110, 127, 2, 203, 152, 248, 44, 237, 23,
                    134, 99, 227, 186, 19, 173, 179, 51, 139, 89, 4, 132,
                    141, 48, 221, 232, 72, 50, 166, 187, 11, 76, 189, 194,
                    181, 45, 17, 195, 53, 121, 252, 164, 39, 57, 242, 118,
                    153, 6, 150, 226, 113, 202, 233, 133, 230, 160, 149, 222,
                    155, 211, 171, 31, 97, 8, 49, 123, 78, 95, 157, 63,
                    128, 183, 234, 62, 138, 143, 71, 126, 147, 239, 101, 199,
                    7, 26, 3, 58, 207, 35, 122, 40, 129, 34, 5, 33,
                    115, 1, 42, 83, 75, 244, 188, 214, 146, 212, 93, 156,
                    112, 55, 246, 47, 105, 98, 92, 228, 162, 158, 59, 27,
                    114, 88, 29, 193, 180, 24, 204, 32, 151, 191, 54, 13};


    int filtered_data[144];
    int nbins = 144;
    int nrows = 12;
    int nbins_in_row[12] = {12,12,12,12,12,12,12,12,12,12,12,12};
    int basebins[12];
    for (int i = 0; i < 12; i++) {
        basebins[i] = (i * 12);
    }
    int test[1] = {FILL_VALUE};
    int arr_expected[144] = {FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE,148, 169, 169, 173, 173, 152, 139,  89, 100,  89, FILL_VALUE ,
                             FILL_VALUE, 141, 186, 127, 110, 127, 166, 166,  89,  89,  89, FILL_VALUE,
                             FILL_VALUE, 134, 186, 186, 121, 121, 166, 164,  76,  76, 118, FILL_VALUE,
                             FILL_VALUE, 141, 150, 150, 121, 121, 166, 166, 133, 149, 160, FILL_VALUE,
                             FILL_VALUE, 153, 150, 113, 113, 113, 133, 133, 123, 149, 149, FILL_VALUE,
                             FILL_VALUE, 155, 171, 138, 113, 113, 126, 126, 133, 149, 157, FILL_VALUE,
                             FILL_VALUE, 155,  62,  97,  62,  97,  71, 122, 123, 101,  95, FILL_VALUE,
                             FILL_VALUE, 42,  58,  75,  83, 138, 126, 129, 146, 129, 101, FILL_VALUE,
                             FILL_VALUE, 42,  47,  75,  83, 105, 122, 146, 158, 129,  59, FILL_VALUE,
                             FILL_VALUE, 88,  55,  83,  98, 105, 188, 162, 162, 151,  93, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE};
    median_filter(arr, filtered_data, nbins, nrows, nbins_in_row, basebins);
    TEST_ASSERT_EQUAL_INT_ARRAY(arr_expected, filtered_data, 144);
}

void test_filter_median_filter_yes_fill(void) {

    int arr[144] = {148, 66, FILL_VALUE, 185, 255, 241, 84, 41, 80, 100, 136, 74,
                    245, 216, 38, 110, 127, 2, 203, 152, 248, 44, 237, 23,
                    134, 99, 227, 186, 19, 173, 179, 51, 139, 89, 4, 132,
                    141, 48, 221, 232, 72, 50, 166, 187, 11, 76, 189, 194,
                    181, 45, 17, 195, 53, 121, 252, 164, 39, 57, 242, 118,
                    153, 6, 150, 226, 113, 202, 233, 133, 230, 160, 149, 222,
                    155, 211, 171, 31, 97, 8, 49, 123, 78, 95, 157, 63,
                    128, 183, 234, 62, 138, 143, 71, 126, 147, 239, 101, 199,
                    7, 26, 3, 58, 207, 35, 122, 40, 129, 34, 5, 33,
                    115, 1, 42, 83, 75, 244, 188, 214, 146, 212, 93, 156,
                    112, 55, 246, 47, 105, 98, 92, 228, 162, 158, 59, 27,
                    114, 88, 29, 193, 180, 24, 204, 32, 151, 191, 54, 13};


    int filtered_data[144];
    int nbins = 144;
    int nrows = 12;
    int nbins_in_row[12] = {12,12,12,12,12,12,12,12,12,12,12,12};
    int basebins[12];
    for (int i = 0; i < 12; i++) {
        basebins[i] = (i * 12);
    }
    int test[1] = {FILL_VALUE};
    int arr_expected[144] = {FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE,141, 148, 156, 173, 173, 152, 139,  89, 100,  89, FILL_VALUE ,
                             FILL_VALUE, 141, 186, 127, 110, 127, 166, 166,  89,  89,  89, FILL_VALUE,
                             FILL_VALUE, 134, 186, 186, 121, 121, 166, 164,  76,  76, 118, FILL_VALUE,
                             FILL_VALUE, 141, 150, 150, 121, 121, 166, 166, 133, 149, 160, FILL_VALUE,
                             FILL_VALUE, 153, 150, 113, 113, 113, 133, 133, 123, 149, 149, FILL_VALUE,
                             FILL_VALUE, 155, 171, 138, 113, 113, 126, 126, 133, 149, 157, FILL_VALUE,
                             FILL_VALUE, 155,  62,  97,  62,  97,  71, 122, 123, 101,  95, FILL_VALUE,
                             FILL_VALUE, 42,  58,  75,  83, 138, 126, 129, 146, 129, 101, FILL_VALUE,
                             FILL_VALUE, 42,  47,  75,  83, 105, 122, 146, 158, 129,  59, FILL_VALUE,
                             FILL_VALUE, 88,  55,  83,  98, 105, 188, 162, 162, 151,  93, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE,
                             FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE, FILL_VALUE};
    median_filter(arr, filtered_data, nbins, nrows, nbins_in_row, basebins);
    TEST_ASSERT_EQUAL_INT_ARRAY(arr_expected, filtered_data, 144);
}

