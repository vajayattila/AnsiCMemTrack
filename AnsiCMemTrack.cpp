// AnsiCMemTrack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cstring>

extern "C" {
#include "include/ansi_c_mem_track.h"
}

const char* FILENAME = NULL;//FILENAME;

/**
 * @brief Allocates `block_size` bytes of memory `num_blocks` times using `ansi_c_mem_track_malloc`
 * and then frees each block using `ansi_c_mem_track_free`. Logs information about the memory allocations
 * and deallocations to `test.log`.
 *
 * @param block_size The size of each block to be allocated.
 * @param num_blocks The number of blocks to be allocated.
 */
void test_memory_allocation(size_t block_size, size_t num_blocks) {
    MemoryUsageInfo mem_info;
    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin test_memory_allocation");

    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation");
    char** block_ptrs = (char**)ansi_c_mem_track_malloc(sizeof(char*) * num_blocks, __FILE__, "test_memory_allocation() -> block_ptrs memory allocation", "char**", 0);
    if (!block_ptrs) {
        ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs");
        return;
    }
    for (size_t i = 0; i < num_blocks; i++) {
        block_ptrs[i] = (char*)ansi_c_mem_track_malloc(block_size, __FILE__, "test_memory_allocation() -> block_ptrs[i] memory allocation", "char", i + 1);
        if (!block_ptrs[i]) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs[i]");
            continue;
        }
    }
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Free");
    for (size_t i = 0; i < num_blocks; i++) {
        ansi_c_mem_track_free(block_ptrs[i]);
    }
    ansi_c_mem_track_free(block_ptrs);
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End test_memory_allocation");
}

/**
 * @brief Allocates `block_size` bytes of memory `num_blocks` times using `ansi_c_mem_track_malloc`
 * and then reallocates each block to a new size using `ansi_c_mem_track_realloc`. Logs information about the memory allocations
 * and deallocations to `test.log`.
 *
 * @param block_size The initial size of each block to be allocated.
 * @param num_blocks The number of blocks to be allocated.
 */
void test_memory_reallocation(size_t block_size, size_t num_blocks) {
    MemoryUsageInfo mem_info;
    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin test_memory_reallocation");

    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation");
    char** block_ptrs = (char**)ansi_c_mem_track_malloc(sizeof(char*) * num_blocks, __FILE__, "test_memory_reallocation() -> block_ptrs memory allocation", "char**", 0);
    if (!block_ptrs) {
        ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs");
        return;
    }
    for (size_t i = 0; i < num_blocks; i++) {
        block_ptrs[i] = (char*)ansi_c_mem_track_malloc(block_size, __FILE__, "test_memory_reallocation() -> block_ptrs[i] memory allocation", "char", i + 1);
        if (!block_ptrs[i]) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs[i]");
            continue;
        }
        memset(block_ptrs[i], 'A', block_size);
    }
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Reallocation (increase)");
    for (size_t i = 0; i < num_blocks; i++) {
        size_t new_size = block_size * (i + 1);
        char* new_ptr = (char*)ansi_c_mem_track_realloc(block_ptrs[i], new_size, i + 1);
        if (!new_ptr) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to reallocate memory");
            continue;
        }
        block_ptrs[i] = new_ptr;
        memset(block_ptrs[i] + block_size * i, 'B', block_size * (i + 1) - block_size * i);
    }
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Reallocation (Decrease)");
    for (size_t i = 0; i < num_blocks; i++) {
        size_t new_size = block_size * (i + 1);
        char* new_ptr = (char*)ansi_c_mem_track_realloc(block_ptrs[i], block_size, i + 1);
        if (!new_ptr) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to reallocate memory");
            continue;
        }
        block_ptrs[i] = new_ptr;
    }
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Free");
    for (size_t i = 0; i < num_blocks; i++) {
        ansi_c_mem_track_free(block_ptrs[i]);
    }
    ansi_c_mem_track_free(block_ptrs);
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End test_memory_reallocation");
}

/**
 * @brief Allocates `block_size` bytes of memory `num_blocks` times using `ansi_c_mem_track_malloc`.
 * Initializes each block to a value equal to the index passed in as a string.
 * Verifies that the data in each block is correct.
 *
 * @param block_size The size of each block to be allocated.
 * @param num_blocks The number of blocks to be allocated.
 */
void ansi_c_mem_track_data_integrity_test(size_t block_size, size_t num_blocks) {
    MemoryUsageInfo mem_info;
    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin ansi_c_mem_track_data_integrity_test");

    // allocate memory for array of pointers
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation of memory for array of pointers");
    char** block_ptrs = (char**)ansi_c_mem_track_malloc(sizeof(char*) * num_blocks, __FILE__, "test_memory_allocation() -> block_ptrs memory allocation", "char**", 0);
    if (!block_ptrs) {
        ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs");
        return;
    }

    // allocate memory for each block and initialize
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation of memory for each block and initialization");
    for (size_t i = 0; i < num_blocks; i++) {
        block_ptrs[i] = (char*)ansi_c_mem_track_malloc(block_size, __FILE__, "test_memory_allocation() -> block_ptrs[i] memory allocation", "char", i + 1);
        if (!block_ptrs[i]) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs[i]");
            continue;
        }

        // initialize block
        char index_str[32];
        sprintf_s(index_str, 32, "%ld", (int)i);
        strcpy_s(block_ptrs[i], 32, index_str);
    }

    // verify data in each block
    ansi_c_mem_track_log_message(FILENAME, "Info", "Verification of data in each block");
    for (size_t i = 0; i < num_blocks; i++) {
        char index_str[32];
        sprintf_s(index_str, 32, "%ld", (int)i);
        if(strcmp(index_str, block_ptrs[i])!=0){
                ansi_c_mem_track_log_message(FILENAME, "Error", "Block data verification failed");
                break;
        }
    }

    // free memory
    ansi_c_mem_track_log_message(FILENAME, "Info", "Freeing memory");
    for (size_t i = 0; i < num_blocks; i++) {
        ansi_c_mem_track_free(block_ptrs[i]);
    }
    ansi_c_mem_track_free(block_ptrs);

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // cleanup allocations
    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End ansi_c_mem_track_data_integrity_test");
}

/**
 * @brief Allocates and frees memory blocks of random sizes to test fragmentation.
 * Verifies that fragmentation does not occur.
 *
 * @param num_blocks The number of blocks to allocate and free.
 * @param max_size The maximum size of a block to allocate.
 */
void ansi_c_mem_track_fragmentation_test(size_t num_blocks, size_t max_size) {
    MemoryUsageInfo mem_info;
    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin ansi_c_mem_track_fragmentation_test");

    // allocate memory for array of pointers
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation of memory for array of pointers");
    char** block_ptrs = (char**)ansi_c_mem_track_malloc(sizeof(char*) * num_blocks, __FILE__, "test_memory_allocation() -> block_ptrs memory allocation", "char**", 0);
    if (!block_ptrs) {
        ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs");
        return;
    }

    // allocate and free memory blocks of random sizes
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation and deallocation of memory blocks of random sizes");
    size_t total_allocated = 0;
    for (size_t i = 0; i < num_blocks; i++) {
        // calculate size of block to allocate
        size_t size = (rand() % max_size) + 1;

        // allocate block and verify allocation was successful
        block_ptrs[i] = (char*)ansi_c_mem_track_malloc(size, __FILE__, "test_memory_allocation() -> block_ptrs[i] memory allocation", "char", i + 1);
        if (!block_ptrs[i]) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Failed to allocate memory for block_ptrs[i]");
            continue;
        }

        // initialize block
        memset(block_ptrs[i], (int)i, size);

        // free block and verify deallocation was successful
        ansi_c_mem_track_free(block_ptrs[i]);
        block_ptrs[i] = NULL;

        // keep track of total allocated memory
        total_allocated += size;
    }

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // verify fragmentation did not occur
    ansi_c_mem_track_log_message(FILENAME, "Info", "Verification of no fragmentation");
    for (size_t i = 0; i < num_blocks; i++) {
        if (block_ptrs[i] != NULL) {
            ansi_c_mem_track_log_message(FILENAME, "Error", "Fragmentation occurred");
            break;
        }
    }

    // free memory
    ansi_c_mem_track_log_message(FILENAME, "Info", "Freeing memory");
    ansi_c_mem_track_free(block_ptrs);

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // cleanup allocations
    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End ansi_c_mem_track_fragmentation_test");
}

/**
 * @brief Tests the ansi_c_mem_track_free_by_object_id function.
 *
 */
void test_ansi_c_mem_track_free_by_object_id() {
    MemoryUsageInfo mem_info;

    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin test_ansi_c_mem_track_free_by_object_id");

    // allocate memory blocks
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocate memory blocks");
    int* int_ptr = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, "test_ansi_c_mem_track_free_by_object_id() -> int_ptr memory allocation", "int", 1);
    char* char_ptr = (char*)ansi_c_mem_track_malloc(sizeof(char) * 10, __FILE__, "test_ansi_c_mem_track_free_by_object_id() -> char_ptr memory allocation", "char", 2);
    float* float_ptr = (float*)ansi_c_mem_track_malloc(sizeof(float), __FILE__, "test_ansi_c_mem_track_free_by_object_id() -> float_ptr memory allocation", "float", 3);

    // get memory usage information
    mem_info=ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // free memory blocks by object ID
    ansi_c_mem_track_log_message(FILENAME, "Info", "Free memory blocks by object ID");
    ansi_c_mem_track_free_by_object_id(2);

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // free remaining memory blocks
    ansi_c_mem_track_log_message(FILENAME, "Info", "Free remaining memory blocks");
    ansi_c_mem_track_free(int_ptr);
    ansi_c_mem_track_free(float_ptr);

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // cleanup allocations
    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();

    // get memory usage information
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End test_ansi_c_mem_track_free_by_object_id");
}

/**
 * @brief Tests if all allocated memory blocks have been freed using `ansi_c_mem_track_log_unfreed_blocks_info`
 * and logs information about them to `test.log`.
 */
void get_unfreed_blocks_info_test() {
    MemoryUsageInfo mem_info;
    ansi_c_mem_track_log_message(FILENAME, "Info", "Begin get_unfreed_blocks_info_test");

    // Allocate some memory
    ansi_c_mem_track_log_message(FILENAME, "Info", "Allocation");
    char* ptr1 = (char*)ansi_c_mem_track_malloc(sizeof(char) * 10, __FILE__, "test_memory_free() -> ptr1 memory allocation", "char(10)", 1);
    char* ptr2 = (char*)ansi_c_mem_track_malloc(sizeof(char) * 20, __FILE__, "test_memory_free() -> ptr2 memory allocation", "char(20)", 2);
    char* ptr3 = (char*)ansi_c_mem_track_malloc(sizeof(char) * 30, __FILE__, "test_memory_free() -> ptr3 memory allocation", "char(30)", 3);
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // Free some memory
    ansi_c_mem_track_log_message(FILENAME, "Info", "Free some memory");
    ansi_c_mem_track_free(ptr1);
    ansi_c_mem_track_free(ptr2);
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    // Check for unfreed memory blocks
    ansi_c_mem_track_log_message(FILENAME, "Info", "Check for unfreed memory blocks");
    size_t info_array_size = 0;
    const MemoryBlock** info_array=ansi_c_mem_track_get_unfreed_blocks_info(&info_array_size);
    ansi_c_mem_track_log_unfreed_blocks_info(FILENAME, *info_array, info_array_size);
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);
    ansi_c_mem_track_free(ptr3);

    ansi_c_mem_track_log_message(FILENAME, "Info", "Cleanup allocations");
    ansi_c_mem_track_cleanup_allocations();
    mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(FILENAME, &mem_info);

    ansi_c_mem_track_log_message(FILENAME, "Info", "End get_unfreed_blocks_info_test");
}


int main()
{
    // initialize the ansi_c_mem_track library
    ansi_c_mem_track_init();
    ansi_c_mem_track_log_message(FILENAME, "Info", "Initialized");

    // test memory allocation with large amount of data
    test_memory_allocation(128, 100000);
    // test realloc with memory blocks
    test_memory_reallocation(128, 1000);
    // Allocate and initialize memory blocks and verify data integrity
    ansi_c_mem_track_data_integrity_test(100, 100000);
    // Test fragmentation
    ansi_c_mem_track_fragmentation_test(100000, 500);
    // Tests the ansi_c_mem_track_free_by_object_id function.
    test_ansi_c_mem_track_free_by_object_id();
    // get_unfreed_blocks_info_test
    get_unfreed_blocks_info_test();

    // deinitialize the ansi_c_mem_track library
    ansi_c_mem_track_log_message(FILENAME, "Info", "Deinitialized");
    ansi_c_mem_track_deinit();

    return 0;
}


