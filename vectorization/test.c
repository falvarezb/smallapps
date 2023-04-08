
#include <inttypes.h>
#include <assert.h>
#include <stdio.h>

extern int32_t* run(char*, char*, char*);

void assert_array(int32_t* actual, int32_t* expected, size_t size) {
    for (int i = 0; i < size; i++){
        assert(actual[i] ==  expected[i]);
    }    
}

void testfile32() {
    char* file_name1 = "testfiles/filetest32";
    char* file_name2 = "testfiles/filetest32copy";
    char* run_mode = "";
    int32_t* result = run(file_name1, file_name2, run_mode);
    int32_t expected_result[] = {1685613382, -445000628, -617589916, -1744019892, -1346433568, -826163600, 473181882, -518510528};
    size_t size = 8;
    assert_array(result, expected_result, size);
}

int main(int n, char **args) {
    testfile32();
}