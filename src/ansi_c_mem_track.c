#include "../include/ansi_c_mem_track.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define SPRINTF sprintf_s
#define FOPEN fopen_s
#else
#define SPRINTF sprintf
#define FOPEN fopen
#endif

#ifdef _WIN32
#define localtime_func(time, timeinfo) localtime_s(timeinfo, time)
#else
#ifdef _POSIX_C_SOURCE
#define localtime_func(time, timeinfo) localtime_r(time, timeinfo)
#else
#define localtime_func(time, timeinfo) localtime(time)
#endif
#endif

MemoryInfo g_mem_info;
static size_t free_calls = 0;
static size_t next_object_id = 1;

bool ansi_c_mem_track_init(void) {
    if (g_mem_info.is_initialized) {
        return true;
    }

    g_mem_info.blocks = (MemoryBlock*)malloc(DEFAULT_CAPACITY * sizeof(MemoryBlock));
    if (!g_mem_info.blocks) {
        return false;
    }
    g_mem_info.capacity = DEFAULT_CAPACITY;
    g_mem_info.total_size = 0;
    g_mem_info.size = 0;
    g_mem_info.total_memory_usage = 0;
    g_mem_info.memory_usage = 0;
    g_mem_info.total_freed_memory = 0;

    g_mem_info.is_initialized = true;
    return true;
}

/**
 * @brief Cleans up the memory allocated by the MemoryInfo struct.
 *
 * This function frees the dynamically allocated array of MemoryBlock structs and resets the MemoryInfo struct's
 * values to their default state.
 *
 * @param mem_info A pointer to the MemoryInfo struct.
 */
static void cleanup_memory(MemoryInfo* mem_info) {
    if (mem_info->blocks) {
        free(mem_info->blocks);
        mem_info->blocks = NULL;
    }
    mem_info->capacity = 0;
    mem_info->total_size = 0;
    mem_info->size = 0;
    mem_info->total_memory_usage = 0;
    mem_info->memory_usage = 0;
    mem_info->total_freed_memory = 0;
}

void ansi_c_mem_track_deinit(void) {
    if (!g_mem_info.is_initialized || !g_mem_info.blocks) {
        return;
    }

    for (size_t i = 0; i < g_mem_info.size; i++) {
        if (g_mem_info.blocks[i].comment) {
            free((void*)g_mem_info.blocks[i].comment);
        }
        if (g_mem_info.blocks[i].file_name) {
            free((void*)g_mem_info.blocks[i].file_name);
        }
        if (g_mem_info.blocks[i].type) {
            free((void*)g_mem_info.blocks[i].type);
        }
        g_mem_info.blocks[i].is_allocated = false;
    }
    cleanup_memory(&g_mem_info);
    g_mem_info.is_initialized = false;
}

void* ansi_c_mem_track_malloc(size_t size, const char* file_name, const char* comment, const char* type, size_t optional_object_id) {
    if (!g_mem_info.is_initialized) {
        ansi_c_mem_track_init();
    }

    if (size == 0) {
        return NULL;
    }

    if (g_mem_info.capacity == g_mem_info.size) {
        MemoryBlock* new_blocks = (MemoryBlock*)realloc(g_mem_info.blocks, sizeof(MemoryBlock) * (g_mem_info.capacity + DEFAULT_CAPACITY));
        if (!new_blocks) {
            return NULL;
        }
        g_mem_info.blocks = new_blocks;
        g_mem_info.capacity += DEFAULT_CAPACITY;
    }

    void* address = malloc(size);
    if (!address) {
        return NULL;
    }

    MemoryBlock block = { 
        address, size, (file_name?(const char*)_strdup(file_name): file_name), (comment ? (const char*)_strdup(comment) : comment),
        (type ? (const char*)_strdup(type): type), true, optional_object_id
    };
    g_mem_info.blocks[g_mem_info.size] = block;
    g_mem_info.total_size++;
    g_mem_info.size++;
    g_mem_info.memory_usage += size;
    g_mem_info.total_memory_usage += size;

    return address;
}

MemoryUsageInfo ansi_c_mem_track_get_info(void) {
    MemoryUsageInfo info = {
        .size = g_mem_info.size,
        .total_size = g_mem_info.total_size,
        .total_user_memory_usage = g_mem_info.total_memory_usage,
        .memory_usage = g_mem_info.memory_usage,
        .total_freed_memory = g_mem_info.total_freed_memory
    };
    return info;
}

bool ansi_c_mem_track_log_message(const char* file_name, const char* message_type, const char* message_text) {
    FILE* output_file;
    time_t raw_time;
    struct tm time_info;
    char timestamp[20];
    char* message;

    // Get the current time
    time(&raw_time);
    localtime_func(&raw_time, &time_info);

    // Format the timestamp
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &time_info);

    // Format the message
    size_t message_size = snprintf(NULL, 0, "[%s] %s: %s\n", timestamp, message_type, message_text);
    message = (char*)malloc(message_size + 1);
    if (!message) {
        return false;
    }
    snprintf(message, message_size + 1, "[%s] %s: %s\n", timestamp, message_type, message_text);

    // Write the message to the file or to the standard output
    if (file_name) {
        if (FOPEN(&output_file, file_name, "a") != 0) {
            free(message);
            return false;
        }
        fprintf(output_file, "%s", message);
        fclose(output_file);
    }
    else {
        printf("%s", message);
    }

    free(message);
    return true;
}

/**
 * @brief Prints the memory usage information to the specified file or to the standard output if the file name is NULL.
 *
 * This function prints the memory usage information to the specified file or to the standard output if the file name is NULL.
 * The memory usage information includes the current and peak memory usage, the total number of allocations, and the total number
 * of deallocations.
 *
 * @param file_name The name of the file to print the memory usage information to. If NULL, the information will be printed to the standard output.
 * @param mem_info The memory usage information to be printed.
 *
 * @return true if the information was successfully printed, false otherwise.
 */
bool ansi_c_mem_track_print_info(const char* file_name, const MemoryUsageInfo* mem_info) {
    FILE* output_file;

    // Get the current time
    time_t raw_time;
    struct tm time_info;
    time(&raw_time);
    localtime_func(&raw_time, &time_info);

    // Format the timestamp
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &time_info);

    // Get the required size of the message buffer
    const char* formatstr= "[%s] [MEMORY] Memory usage information:\n"
        "                               Current memory usage: %lu bytes\n"
        "                               Current allocations: %lu bytes\n"
        "                               Total memory usage: %lu bytes\n"
        "                               Total allocations: %lu\n"
        "                               Total freed memory: %lu\n";
    size_t message_size = snprintf(NULL, 0, formatstr,
        timestamp, (unsigned long)mem_info->memory_usage, (unsigned long)mem_info->size,(unsigned long)mem_info->total_user_memory_usage, 
        (unsigned long)mem_info->total_size, (unsigned long)mem_info->total_freed_memory);

    // Allocate the message buffer
    char* message = (char*)malloc(message_size + 1);
    if (!message) {
        return false;
    }

    // Format the message
    snprintf(message, message_size + 1, formatstr,
        timestamp, (unsigned long)mem_info->memory_usage, (unsigned long)mem_info->size, (unsigned long)mem_info->total_user_memory_usage,
        (unsigned long)mem_info->total_size, (unsigned long)mem_info->total_freed_memory);

    // Write the message to the file or to the standard output
    if (file_name) {
        if (FOPEN(&output_file, file_name, "a") != 0) {
            return false;
        }
        fprintf(output_file, "%s", message);
        fclose(output_file);
    }
    else {
        printf("%s", message);
    }

    if (message) {
        free(message);
    }
    return true;
}

void ansi_c_mem_track_free_block(void* ptr, size_t index) {
    MemoryBlock block = g_mem_info.blocks[index];
    free(ptr);
    g_mem_info.memory_usage -= block.size;
    g_mem_info.total_freed_memory += block.size;
    g_mem_info.blocks[index].is_allocated = false;
    g_mem_info.blocks[index].address = NULL;
    g_mem_info.blocks[index].size = 0;
    if (g_mem_info.blocks[index].file_name) {
        free((void*)g_mem_info.blocks[index].file_name);
        g_mem_info.blocks[index].file_name = NULL;
    }
    if (g_mem_info.blocks[index].comment) {
        free((void*)g_mem_info.blocks[index].comment);
        g_mem_info.blocks[index].comment = NULL;
    }
    if (g_mem_info.blocks[index].type) {
        free((void*)g_mem_info.blocks[index].type);
        g_mem_info.blocks[index].type = NULL;
    }
}

void cleanup_allocations_if_needed() {
    free_calls++;
    if (free_calls >= DEFAULT_CAPACITY) {
        ansi_c_mem_track_cleanup_allocations();
        free_calls = 0;
    }
}

void ansi_c_mem_track_free(void* ptr) {
    if (!ptr) {
        return;
    }

    size_t index = -1;
    for (size_t i = 0; i < g_mem_info.size; i++) {
        if (g_mem_info.blocks[i].address == ptr) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        free(ptr);
        return;
    }
    
    ansi_c_mem_track_free_block(ptr, index);
    cleanup_allocations_if_needed();
}

void ansi_c_mem_track_cleanup_allocations(void) {
    int index = 0;
    for (size_t i = 0; i < g_mem_info.size; i++) {
        if (g_mem_info.blocks[i].is_allocated) {
            g_mem_info.blocks[index] = g_mem_info.blocks[i];
            index++;
        }
    }
    if (index < DEFAULT_CAPACITY) {
        index = DEFAULT_CAPACITY;
    } 
    
    g_mem_info.size = index;

    if (g_mem_info.capacity > g_mem_info.size) {
        MemoryBlock* new_blocks = (MemoryBlock*)realloc(g_mem_info.blocks, sizeof(MemoryBlock) * g_mem_info.size);
        if (!new_blocks) {
            return;
        }
        g_mem_info.blocks = new_blocks;
        g_mem_info.capacity = g_mem_info.size;
    }
}

void* ansi_c_mem_track_realloc(void* ptr, size_t size, size_t optional_object_id) {
    if (!ptr) {
        return ansi_c_mem_track_malloc(size, __FILE__, "ansi_c_mem_track_realloc()", "Allocation with malloc because prt=NULL", optional_object_id);
    }

    size_t index = -1;
    for (size_t i = 0; i < g_mem_info.size; i++) {
        if (g_mem_info.blocks[i].address == ptr) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return NULL;
    }

    void* new_ptr = realloc(ptr, size);
    size_t sizechange=llabs(size- g_mem_info.blocks[index].size);
    bool increase = true; // increase or decrease
    if (size < g_mem_info.blocks[index].size) {
        increase = false;
    }

    if (new_ptr) {
        g_mem_info.blocks[index].address = new_ptr;
        g_mem_info.blocks[index].size = size;        
        if (increase) {
            g_mem_info.memory_usage += sizechange;
            g_mem_info.total_memory_usage += sizechange;
        }
        else {
            g_mem_info.memory_usage -= sizechange;
            g_mem_info.total_freed_memory += sizechange;
        }
    }

    return new_ptr;
}

/**
 * @brief Frees memory and updates the memory tracker based on the optional_object_id parameter.
 *
 * @param optional_object_id Optional object ID to identify memory allocations.
 * @return None.
 */
void ansi_c_mem_track_free_by_object_id(size_t optional_object_id) {

    for (size_t i = 0; i < g_mem_info.size;  i++) {
        if (g_mem_info.blocks[i].optional_object_id == optional_object_id) {
            ansi_c_mem_track_free_block(g_mem_info.blocks[i].address, i);
        }
    }
    cleanup_allocations_if_needed();
}

size_t ansi_c_mem_track_get_next_object_id(void)
{
    return next_object_id++;
}
