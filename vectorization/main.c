/**
 * Module with examples on how to use vector operations. Vector operations can be applied to all elements of a vector at once, in
 * a single clock cycle.
 * 
 * In 2008, Intel introduced a new set of high-performance instructions called Advanced Vector Extensions (AVX) 
 * to perform SIMD (single instruction, multiple data) processing.
 * 
 * Access to AVX instructions is done through special C functions called intrinsic functions. 
 * An intrinsic function doesn't necessarily map to a single instruction though
 * 
 * See https://www.codeproject.com/Articles/874396/Crunching-Numbers-with-AVX-and-AVX
 * 
 */


#include <immintrin.h> // Include header for AVX instructions
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#define BASE_SIZE 32 // size in bytes of 8 integers
#define BUF_SIZE (BASE_SIZE*1024L*1024L) // multiple of BASE_SIZE
#define FILE_SIZE (BUF_SIZE*512L) // multiple of BUF_SIZE

/**
 * @brief Add elements of the arrays 'a' and 'b' by using AVX2 instructions and stores the result in array 'c'
 * 
 * This function uses 256-bit AVX to process 8 integers at a time: 8 integers * 32 bits/integer (int32_t)
 * 
 * In order to compile AVX2 intrinsics, the compiler must be passed the flag '-mavx2'
 */
void add_arrays_avx2(int32_t *a, int32_t *b, int32_t *c, size_t size){    
    for (size_t i = 0; i < size; i += 8){
        __m256i a_vec = _mm256_loadu_si256((__m256i *)(a + i)); // Load 8 elements from a
        __m256i b_vec = _mm256_loadu_si256((__m256i *)(b + i)); // Load 8 elements from b
        __m256i c_vec = _mm256_add_epi32(a_vec, b_vec);         // Add the two vectors element-wise
        _mm256_storeu_si256((__m256i *)(c + i), c_vec);         // Store the result back to memory
    }
}

/**
 * @brief Similar to 'add_arrays_avx2' but using AVX512
 * 
 * Although this function can be compiled by using the compiler's flag '-mavx512', the corresponding processor instructions are
 * not supported by my Intel(R) Core(TM) i9-9880H CPU @ 2.30GHz and trying to run it results in the error
 * "illegal hardware instruction" 
 * 
 * Only AVX2 is supported:
 * >> sysctl -a | grep machdep.cpu.leaf7_features
 * machdep.cpu.leaf7_features: RDWRFSGS TSC_THREAD_OFFSET SGX BMI1 AVX2 SMEP BMI2 ERMS INVPCID FPU_CSDS MPX RDSEED ADX SMAP CLFSOPT IPT SGXLC MDCLEAR IBRS STIBP L1DF ACAPMSR SSBD
 * 
 */
void add_arrays_avx512(int32_t *a, int32_t *b, int32_t *c, size_t size){    
    for (size_t i = 0; i < size; i += 16){   // Process 16 integers at a time (assuming 512-bit AVX)
        __m512i a_vec = _mm512_loadu_si512((__m512i *)(a + i)); // Load 16 elements from a
        __m512i b_vec = _mm512_loadu_si512((__m512i *)(b + i)); // Load 16 elements from b
        __m512i c_vec = _mm512_add_epi32(a_vec, b_vec);         // Add the two vectors element-wise
        _mm512_storeu_si512((__m512i *)(c + i), c_vec);         // Store the result back to memory
    }
}

/**
 * @brief Add elements of the arrays 'a' and 'b' in a scalar fashion and stores the result in array 'c' 
 */
void add_arrays_scalar(int32_t *a, int32_t *b, int32_t *c, size_t size){    
    for (size_t i = 0; i < size; i++){ 
        c[i] = a[i] + b[i];
    }
}

/**
 * @brief Read next chunk of data from the file into the buffer
 * 
 * Read data is interpreted as 4-byte integers
 * Return number of read integers
 */
size_t read_next(FILE *file, char* file_name, int32_t* buf, size_t buf_size) {
    size_t num_read = fread(buf, sizeof(int32_t), buf_size, file);
    if (num_read < buf_size && ferror(file)) {            
        printf("error %d while reading file '%s'", errno, file_name);
        exit(EXIT_FAILURE);
    }        
    return num_read;
}

FILE* open_file(char* file_name) {
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        printf("error %d while opening file %s", errno, file_name);        
        exit(EXIT_FAILURE);
    }
    return file;
}

/**
 * @brief Allocate heap memory to store 4-byte integers
 */
int32_t* allocate_memory(size_t num_integers) {
    int32_t* buf = malloc(sizeof(int32_t)*num_integers);
    if(buf == NULL) {
        printf("error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    return buf;
}

void run(char* file_name1, char* file_name2, char* run_mode) {
    
    size_t num_integers = FILE_SIZE/sizeof(int32_t);
    int32_t* buf1 = allocate_memory(BUF_SIZE);
    int32_t* buf2 = allocate_memory(BUF_SIZE);
    int32_t* result = allocate_memory(num_integers);    
    size_t result_position = 0;

    FILE* file1 = open_file(file_name1);
    FILE* file2 = open_file(file_name2);

    size_t num_read1, num_read2;
    while ((num_read1 = read_next(file1, file_name1, buf1, BUF_SIZE)) > 0 && 
        (num_read2 = read_next(file2, file_name2, buf2, BUF_SIZE)) > 0) {
        size_t min_num_read = num_read1 < num_read2 ? num_read1 : num_read2;

        if (run_mode != NULL && strcmp(run_mode, "AVX2") == 0){            
            add_arrays_avx2(buf1, buf2, result + result_position, min_num_read);
        }
        else if (run_mode != NULL && strcmp(run_mode, "AVX512") == 0){            
            add_arrays_avx512(buf1, buf2, result + result_position, min_num_read);
        }
        else{            
            add_arrays_scalar(buf1, buf2, result + result_position, min_num_read);
        }
        
        result_position += min_num_read;
    }

    // for (int i = 0; i < num_integers; i++){
    //     printf("%d ", result[i]);
    // }
    // printf("\n");
}

/**
 * Before running, compile with the correct values of BUF_SIZE and FILE_SIZE
 */
int main(int n, char **args) {
    if (n < 3) {
        printf("USAGE: main file1 file2 [AVX2|AVX512]; if 3rd argument is not provided, the scalar version is run");
        exit(EXIT_FAILURE);
    }

    char* file_name1 = args[1];
    char* file_name2 = args[2];
    char* run_mode = NULL;

    if (n > 3){        
        run_mode = args[3];
    }    
    run(file_name1, file_name2, run_mode);   
}
