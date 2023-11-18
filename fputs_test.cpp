#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

#include <string>
#include <iostream>

#include "tools.h"
#include "tests.h"
#include "stats.h"

#define originalFileName "test.txt"
#define testFileName "test_copy.txt"

    // generate test FILE* test value
    // see tests.h
    // use the functions specified by tools.h to create appropriate test values
    // you can use a copy of test.txt as a file to test on
}

const char *generateCSTR(int test_id) {
    // generate a `const char*` test value
    // see tests.h
    // use the functions specified by tools.h to create appropriate test values
}

// waiting time before querying the child's exit status
// You might want to try using a smaller value in order to get the CI results faster,
// but there is a chance that your tests will start failing because of the timeout
const double wait_time = 1.0;

void test_fputs(const TestCase &str_testCase, const TestCase &file_testCase) {
    // execute a single test
    // use the functions in stats.h to record all tests
int main(int argc, const char** argv) {
    // execute all tests and catch exceptions
    for (int i = 0; i < testCases_CSTR_count; i++) {
        for (int j = 0; j < testCases_FILE_count; j++) {
            try {
                test_fputs(testCases_CSTR[i], testCases_FILE[j]);
            } catch (const char* msg) {
                std::cerr << "Exception: " << msg << std::endl;
            }
        }
    }

    // print summary
    print_summary();

    return 0;
}

