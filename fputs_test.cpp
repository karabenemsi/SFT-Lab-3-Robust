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
const char* generateCSTR(int test_id) {
    // generate a `const char*` test value
    // see tests.h
    // use the functions specified by tools.h to create appropriate test values
    const char* valid_str = "Hello World!";
    const char invalid_str[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};

    switch (test_id) {
    case 0:
        return NULL;
        break;
    case 1:
        return (const char*)malloc_prot(sizeof(valid_str), valid_str, PROT_READ);
        break;
    case 2:
        return (const char*)malloc_prot(sizeof(valid_str), valid_str, PROT_WRITE);
        break;
    case 3:
        return (const char*)malloc_prot(sizeof(valid_str), valid_str, PROT_READ | PROT_WRITE);
        break;
    case 4:
        return (const char*)malloc_prot(sizeof(invalid_str), invalid_str, PROT_READ);
        break;
    case 5:
        return (const char*)malloc_prot(sizeof(invalid_str), invalid_str, PROT_READ);
        break;
    case 6:
        return (const char*)malloc_prot(sizeof(invalid_str), invalid_str, PROT_WRITE);
        break;
    case 7:
        return (const char*)malloc_prot(sizeof(valid_str), valid_str, PROT_NONE);
        break;

    default:
        break;
    }
    return NULL;
}

// waiting time before querying the child's exit status
// You might want to try using a smaller value in order to get the CI results faster,
// but there is a chance that your tests will start failing because of the timeout
const double wait_time = 0.2;

void test_fputs(const TestCase& str_testCase, const TestCase& file_testCase) {
    // execute a single test
    // use the functions in stats.h to record all tests
    record_start_test_fputs(str_testCase, file_testCase);
    // generate test values
    const char* str = generateCSTR(str_testCase.id);
    FILE* file = generateFILE(file_testCase.id);
    // create child process
    pid_t pid = fork();
    // execute fputs in child process
    if (pid == 0) {
        const int result = fputs(str, file);
        exit(result);
    } else {
        sleep(wait_time);
        int status;
        pid_t w = waitpid(pid, &status, WNOHANG);
        if (w == 0) {
            kill(pid, SIGKILL);
            record_timedout_test_fputs();
        } else {
            if (WIFEXITED(status)) {
                const int returnval = WEXITSTATUS(status);
                record_ok_test_fputs(returnval);
                // if (returnval == str_testCase.expected_returnvalue) {
                //      record_ok_test_fputs(returnval);
                // } else {
                //     record_error_test_fputs(returnval);
                // }
            } else if (WIFSIGNALED(status)) {
                const int signal = WTERMSIG(status);
                record_crashed_test_fputs(signal);
            } else {
                const int signal = WSTOPSIG(status);
                record_stopped_test_fputs(signal);
            }
        }
    }
}

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

