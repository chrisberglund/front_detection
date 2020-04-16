#include "unity.h"

#include <stdlib.h>
#include "helpers.h"
#include "filter.h"

const int FILL_VALUE = -999;
void setUp(void) {
}

void tearDown(void) {
}

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
    TEST_ASSERT_EQUAL_INT(FILL_VALUE, mdn);
}
/*
void test_filter_median_filter(void) {
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
    int *data = malloc(144 * sizeof(int));
    for (int i = 0; i < 144; i++) {
        data[i] = arr[i];
    }
    int *filtered_data = malloc(144 * sizeof(int));
    int nbins = 144;
    int nrows = 12;
    int nbins_in_row[12] = {12};
    int basebins[12];
    for (int i = 0; i < 12; i++) {
        basebins[i] = (i * 12) + 1;
    }
    median_filter(arr, filtered_data, nbins, nrows, nbins_in_row, basebins);
    TEST_ASSERT_EQUAL_INT(134, arr[37]);
    free(data);
    free(filtered_data);
}
*/
