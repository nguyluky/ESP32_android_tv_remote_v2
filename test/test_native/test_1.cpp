#include <unity.h>
#include <stdio.h>

void test_1(void) {
    printf("Test 1\n");
    TEST_ASSERT_EQUAL(1, 1);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_1);
    UNITY_END();
}