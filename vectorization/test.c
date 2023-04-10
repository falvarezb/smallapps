
#include <stdint.h>
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
    size_t size = 8;    
    int32_t expected_result[] = {1685613382, -445000628, -617589916, -1744019892, -1346433568, -826163600, 473181882, -518510528};

    char* run_mode = "";
    int32_t* result = run(file_name1, file_name2, run_mode);
    assert_array(result, expected_result, size);

    run_mode = "AVX2";
    result = run(file_name1, file_name2, run_mode);    
    assert_array(result, expected_result, size);
}

void testfile64() {
    char* file_name1 = "testfiles/filetest64";
    char* file_name2 = "testfiles/filetest64copy";
    int32_t expected_result[] = {1653239700, -1319713446, 2091309360, -1080189766, 603232712, 455946188, 1763699284, -1046572764,
    -1428600120, -1601384636, 1618116198, 773187136, 176541914, -311566690, 793470738, 1180972056};
    size_t size = 16;

    char* run_mode = "";
    int32_t* result = run(file_name1, file_name2, run_mode);
    assert_array(result, expected_result, size);

    run_mode = "AVX2";
    result = run(file_name1, file_name2, run_mode);
    assert_array(result, expected_result, size);
}

void testfile36() {
    char* file_name1 = "testfiles/filetest36";
    char* file_name2 = "testfiles/filetest36copy";
    int32_t expected_result[] = {-1410064974, -621034864, 1987078652, -1131987198, 454457864, -1796557598, 1054020734, 544343742, 1470187204};
    size_t size = 9;

    char* run_mode = "";
    int32_t* result = run(file_name1, file_name2, run_mode);
    assert_array(result, expected_result, size);

    run_mode = "AVX2";
    result = run(file_name1, file_name2, run_mode);
    assert_array(result, expected_result, size);
}

int main(int n, char **args) {
    testfile32();
    testfile64();
    testfile36();
}