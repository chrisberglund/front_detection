#include "unity.h"

#include "contour.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_contour_gradient_ratio(void) {
    int arr[25] = { 50,  83, 100, 248, 118,
                    110,  67,  95, 168, 149,
                    195,  58,  21, 204, 215,
                    118,  84, 154,  22, 211,
                    113,  64, 126,  97, 235};
    TEST_ASSERT_EQUAL_DOUBLE(0.3690171758724715, gradient_ratio(arr, -999));
}

/*
void test_contour_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement contour");
}*/
