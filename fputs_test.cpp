#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
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



FILE* generateFILE(int test_id) {
    // generate test FILE* test value
    // see tests.h
    // use the functions specified by tools.h to create appropriate test values
    // you can use a copy of test.txt as a file to test on
    FILE* file;
    char* file_content;
    struct stat st;
    stat(originalFileName, &st);
    int fileSize = st.st_size;
    FILE* returnFile;
    filecopy(originalFileName, testFileName);


    switch (test_id) {
    case 0:
        return NULL;
        break;
    case 1:
        return fopen(testFileName, "r");
        break;
    case 2:
        return fopen(testFileName, "w");
        break;
    case 3:
        return fopen(testFileName, "r+");
        break;
    case 4:
        file = fopen(testFileName, "w");
        fclose(file);
        return file;
        break;
    case 5:
        file = fopen(testFileName, "r");
        file_content = new char[fileSize];
        fread(file_content, fileSize, 1, file);
        fclose(file);
        returnFile = (FILE*)malloc_prot(fileSize, file_content, PROT_READ);
        delete[] file_content;
        return returnFile;
        break;
    case 6:
        file = fopen(testFileName, "r");
        file_content = new char[fileSize];
        fread(file_content, fileSize, 1, file);
        fclose(file);
        returnFile = (FILE*)malloc_prot(fileSize, file_content, PROT_WRITE);
        delete[] file_content;
        return returnFile;
        break;
    case 7:
        file = fopen(testFileName, "r");
        file_content = new char[fileSize];
        fread(file_content, fileSize, 1, file);
        fclose(file);
        returnFile = (FILE*)malloc_prot(fileSize, file_content, PROT_READ | PROT_WRITE);
        delete[] file_content;
        return returnFile;
        break;
    case 8:
        return (FILE*)malloc_prot(fileSize, NULLpage(), PROT_READ);
        break;
    case 9:
        return (FILE*)malloc_prot(fileSize, NULLpage(), PROT_WRITE);
        break;
    case 10:
        return (FILE*)malloc_prot(fileSize, NULLpage(), PROT_READ | PROT_WRITE);
        break;
    case 11:
        return (FILE*)malloc_prot(fileSize, NULLpage(), PROT_NONE);
        break;
    default:
        break;
    }
    return NULL;
}

const char* generateCSTR(int test_id) {
    // generate a `const char*` test value
    // see tests.h
    // use the functions specified by tools.h to create appropriate test values
    const int str_len = 5;
    const char valid_str[5] = { 't','e','s','t','\0' };
    const char invalid_str[5] = { 't','e','s','t','2' };
    // const char* heap_str = (char*)malloc(str_len);

    switch (test_id) {
    case 0:
        return NULL;
        break;
    case 1:
        // strcpy((char*)valid_str, heap_str);
        return (const char*)malloc_prot(str_len, valid_str, PROT_READ);
        break;
    case 2:
        // strcpy((char*)valid_str, heap_str);
        return (const char*)malloc_prot(str_len, valid_str, PROT_WRITE);
        break;
    case 3:
        // strcpy((char*)valid_str, heap_str);
        return (const char*)malloc_prot(str_len, valid_str, PROT_READ | PROT_WRITE);
        break;
    case 4:
        // strcpy((char*)invalid_str, heap_str);
        return (const char*)malloc_prot(str_len, invalid_str, PROT_READ);
        break;
    case 5:
        // strcpy((char*)invalid_str, heap_str);
        return (const char*)malloc_prot(str_len, invalid_str, PROT_WRITE);
        break;
    case 6:
        // strcpy((char*)invalid_str, heap_str);
        return (const char*)malloc_prot(str_len, invalid_str, PROT_READ | PROT_WRITE);
        break;
    case 7:
        return (const char*)malloc_prot(str_len, valid_str, PROT_NONE);
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
    // int fully_successfull_str[5] = { 0,1,3,4,7 };

    // int successfull_cases[24][2] = {
    //     {2,0},
    //     {2,1},
    //     {2,2},
    //     {2,3},
    //     {2,4},
    //     {2,5},
    //     {2,6},
    //     {2,7},
    //     {2,8},
    //     {2,11},
    //     {5,0},
    //     {5,5},
    //     {5,6},
    //     {5,7},
    //     {5,8},
    //     {5,9},
    //     {5,10},
    //     {5,11},
    //     {6,0},
    //     {6,5},
    //     {6,6},
    //     {6,7},
    //     {6,8},
    //     {6,11}
    // };

    // for (int i = 0; i < 5; i++) {
    //     if (str_testCase.id == fully_successfull_str[i]) {
    //         return;
    //     }
    // }

    // for (int i = 0; i < 24; i++) {
    //     if (str_testCase.id == successfull_cases[i][0] && file_testCase.id == successfull_cases[i][1]) {
    //         return;
    //     }
    // }



    record_start_test_fputs(str_testCase, file_testCase);
    pid_t pid = fork();
    // execute fputs in child process
    if (pid == 0) {
        // child process
        const char* str = generateCSTR(str_testCase.id);
        FILE* file = generateFILE(file_testCase.id);
        const int result = fputs(str, file);
        exit(result);
    } else {
        // parent process
        sleep(wait_time);
        int status;
        pid_t w = waitpid(pid, &status, WNOHANG);
        if (w == 0) {
            kill(pid, SIGKILL);
            record_timedout_test_fputs();
        } else {
            if (WIFEXITED(status)) {
                int returnval = WEXITSTATUS(status);
                if (returnval == 255) returnval = -1; // EOF is -1 but gets converted to 255
                record_ok_test_fputs(returnval);
                // if (returnval != str_testCase.expected_returnvalue && returnval != file_testCase.expected_returnvalue) {
                //     std::cout << str_testCase.desc << "," << file_testCase.desc << " returnval: " << returnval << ", expected " << str_testCase.expected_returnvalue << " or " << file_testCase.expected_returnvalue << std::endl;
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


    // clean up
    remove(testFileName);

    // print summary
    print_summary();

    return 0;
}

