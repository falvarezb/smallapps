/**
 * Module with examples on how to use vector operations
 */

#include <immintrin.h> // Include header for AVX instructions
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define BASE_SIZE 8 // 8 integers
#define BUF_SIZE (BASE_SIZE*1024*1024) // multiple of BASE_SIZE


void print_array(int32_t* arr, size_t size) {
    for (int i = 0; i < size; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

long get_file_size(FILE* file, char* file_name) {
    if(fseek(file, 0, SEEK_END) == 0) {
        long file_size;
        if((file_size = ftell(file)) > -1) {
            rewind(file);
            return file_size;
        }
        printf("error %d while sizing file %s", errno, file_name);        
        exit(EXIT_FAILURE);
    }
    printf("error while sizing file %s", file_name);        
    exit(EXIT_FAILURE);

}

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
 * @brief Add elements of the arrays 'a' and 'b' in a scalar fashion and stores the result in array 'c' 
 */
void add_arrays_scalar(int32_t *a, int32_t *b, int32_t *c, size_t size){   
    // print_array(a, size); 
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

int32_t* run(char* file_name1, char* file_name2, char* run_mode) {            
    FILE* file1 = open_file(file_name1);
    FILE* file2 = open_file(file_name2);
    long file_size1 = get_file_size(file1, file_name1);
    long file_size2 = get_file_size(file2, file_name2);
    assert(file_size1 == file_size2);

    //file size must be a multiple of 4 so that its content can be interpreted as 4-byte integers
    assert(file_size1 % sizeof(int32_t) == 0);

    size_t num_integers = file_size1/sizeof(int32_t);
    int32_t* buf1 = allocate_memory(BUF_SIZE);
    int32_t* buf2 = allocate_memory(BUF_SIZE);
    int32_t* result = allocate_memory(num_integers);    
    size_t result_position = 0;

    clock_t start_time, end_time;
    double elapsed_time;

    //TIMED CODE
    start_time = clock();
    size_t num_read1, num_read2;
    while ((num_read1 = read_next(file1, file_name1, buf1, BUF_SIZE)) > 0 && 
        (num_read2 = read_next(file2, file_name2, buf2, BUF_SIZE)) > 0) {
        size_t min_num_read = num_read1 < num_read2 ? num_read1 : num_read2;

        if (run_mode != NULL && strcmp(run_mode, "AVX2") == 0){            
            add_arrays_avx2(buf1, buf2, result + result_position, min_num_read);
        }
        else{            
            add_arrays_scalar(buf1, buf2, result + result_position, min_num_read);
        }
        
        result_position += min_num_read;
    }
    //END TIMED CODE
    end_time = clock();
    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", elapsed_time);

    // print_array(result, num_integers);
    return result;
}

#ifndef TEST

int main(int n, char **args) {
    if (n < 3) {
        printf("USAGE: main file1 file2 [AVX2]; if vectorization argument is not provided, the scalar version is run");
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

#endif
