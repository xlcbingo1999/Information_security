#include "test.h"

int main() {
    int result_test1 = TEST_random();
    if(result_test1 != 0) {
        printf("---------- TEST_random failed! ----------\n\n");
    } else {
        printf("---------- TEST_random succeeded! ----------\n\n");
    }
    int result_test2 = TEST_given_p_and_q();
    if(result_test2 != 0) {
        printf("---------- TEST_random failed! ----------\n\n");
    } else {
        printf("---------- TEST_random succeeded! ----------\n\n");
    }
    int result_test3 = TEST_random_str();
    if(result_test3 != 0) {
        printf("---------- TEST_random_str failed! ----------\n\n");
    } else {
        printf("---------- TEST_random_str succeeded! ----------\n\n");
    }
    return 0;
}