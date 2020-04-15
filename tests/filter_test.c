#include "../test/unity.h"
#include "../src/filter.h"

void setUp(void) {

}

void tearDown(void) {

}

void test_median(void) {
    TEST_ASSERT_EQUAL(0, 4);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_median);
    return UNITY_END();
}