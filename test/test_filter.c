#include "unity.h"

#include "helpers.h"
#include "filter.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_filter_NeedToImplement(void)
{
    int arr[9] = {144,233,178,102, 72, 1, 52, 246, 254};
    int mdn = median9(arr);
    TEST_ASSERT(mdn == 144);
}
