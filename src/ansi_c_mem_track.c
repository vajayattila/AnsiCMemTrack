#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../include/ansi_c_mem_track.h"
#include "../include/ansi_c_macro_utils.h"

MemoryInfo g_mem_info;
static size_t free_calls = 0;
static size_t next_object_id = 1;

MemoryBlock* ansi_c_mem_track_copy_memory_block(const MemoryBlock* mb) {
    if (!mb) {
        return NULL;
    }

    // Allocate memory for the new MemoryBlock
    MemoryBlock* new_mb = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!new_mb) {
        return NULL;
    }

    // Copy the data to the new MemoryBlock
    new_mb->address = mb->address;
    C_STRDUP(new_mb->comment, strlen(mb->comment), mb->comment);
    C_STRDUP(new_mb->file_name, strlen(mb->file_name), mb->file_name);
    new_mb->is_allocated = mb->is_allocated;
    new_mb->optional_object_id = mb->optional_object_id;
    new_mb->size = mb->size;
    C_STRDUP(new_mb->type, strlen(mb->type), mb->type);

    return new_mb;
}

void ansi_c_mem_track_init_memory_block(MemoryBlock* mb, void* address, size_t size, bool is_allocated, const char* file_name, const char* comment, const char* type, size_t optional_object_id) {
    mb->address = address;
    mb->size = size;
    mb->is_allocated = is_allocated;
    char *file_name_poi=NULL, *comment_poi=NULL, *type_poi=NULL;
    if (file_name) {
        C_STRDUP(file_name_poi, strlen(file_name), file_name);
    } 
    mb->file_name = file_name_poi;
    if (comment) {
        C_STRDUP(comment_poi, strlen(comment), comment);
    }
    mb->comment = comment_poi;
    if (type) {
        C_STRDUP(type_poi, strlen(type), type);
    }
    mb->type = type_poi;
    mb->optional_object_id = optional_object_id;
}

void ansi_c_mem_track_free_memory_block(MemoryBlock* mb) {
    if (!mb) {
        return;
    }

    // Free the dynamically allocated memory in the MemoryBlock
    if (mb->comment) {
        free((void*)mb->comment);
    };
    if (mb->file_name) {
        free((void*)mb->file_name);
    }
    if (mb->type) {
        free((void*)mb->type);
    }

    // Free the MemoryBlock itself
    free(mb);
}

bool ansi_c_mem_track_init(void) {
    if (g_mem_info.is_initialized) {
        return true;
    }

    g_mem_info.blocks = (MemoryBlock*)malloc(DEFAULT_CAPACITY * sizeof(MemoryBlock));
    if (!g_mem_info.blocks) {
        return false;
    }
    for (size_t i = 0; i < DEFAULT_CAPACITY; ++i) {
        ansi_c_mem_track_init_memory_block(&g_mem_info.blocks[i], NULL, 0, false, NULL, NULL, NULL, 0);
    }
    g_mem_info.capacity = DEFAULT_CAPACITY;
    g_mem_info.total_size = 0;
    g_mem_info.size = 0;
    g_mem_info.total_memory_usage = 0;
    g_mem_info.memory_usage = 0;
    g_mem_info.total_freed_memory = 0;
    g_mem_info.is_initialized = true;
    g_mem_info.get_unfreed_blocks_info_ptr = NULL;
    g_mem_info.get_unfreed_blocks_info_size = 0;
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
        for (size_t i=0; i < mem_info->size; ++i) {
            if (mem_info->blocks[i].is_allocated) {
                ansi_c_mem_track_free_memory_block(&mem_info->blocks[i]);
            }
        }
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

void ansi_c_mem_track_free_unfreed_blocks_info() {
    if (!g_mem_info.get_unfreed_blocks_info_ptr) {
        return;
    }

    for (size_t i = 0; i < g_mem_info.get_unfreed_blocks_info_size; ++i) {
        if (g_mem_info.get_unfreed_blocks_info_ptr[i]) {
            ansi_c_mem_track_free_memory_block(g_mem_info.get_unfreed_blocks_info_ptr[i]);
        }
    }

    free(g_mem_info.get_unfreed_blocks_info_ptr);
    g_mem_info.get_unfreed_blocks_info_ptr = NULL;
    g_mem_info.get_unfreed_blocks_info_size = 0;
}


void ansi_c_mem_track_deinit(void) {
    if (!g_mem_info.is_initialized || !g_mem_info.blocks) {
        return;
    }
    ansi_c_mem_track_free_unfreed_blocks_info();
    for (size_t i = 0; i < g_mem_info.size; i++) {
        if (g_mem_info.blocks[i].is_allocated) {
            ansi_c_mem_track_free_memory_block(&g_mem_info.blocks[i]);
        }
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

    char *file_name_poi = NULL, *comment_poi=NULL, *type_poi=NULL;
    if (file_name) {
        C_STRDUP(file_name_poi, strlen(file_name), file_name);
    }
    if (comment) {
        C_STRDUP(comment_poi, strlen(comment), comment);
    }
    if (type) {
        C_STRDUP(type_poi, strlen(type), type);
    }
    MemoryBlock block = { 
        address, size, file_name_poi, comment_poi, type_poi, true, optional_object_id
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
    size_t message_size = snprintf(NULL, 0, "%s [%s] %s\n", timestamp, message_type, message_text);
    message = (char*)malloc(message_size + 1);
    if (!message) {
        return false;
    }
    snprintf(message, message_size + 1, "%s [%s] %s\n", timestamp, message_type, message_text);

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
    const char* formatstr= "%s [MEMORY] Memory usage information:\n"
        "                             Current memory usage: %lu bytes\n"
        "                             Current allocations: %lu\n"
        "                             Total memory usage: %lu bytes\n"
        "                             Total allocations: %lu\n"
        "                             Total freed memory: %lu bytes\n";
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

void ansi_c_mem_track_free_block(size_t index) {
    if (g_mem_info.blocks[index].is_allocated) {
        g_mem_info.memory_usage -= g_mem_info.blocks[index].size;
        g_mem_info.total_freed_memory += g_mem_info.blocks[index].size;
        g_mem_info.blocks[index].is_allocated = false;
        free(g_mem_info.blocks[index].address);
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
    
    ansi_c_mem_track_free_block(index);
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
    
    g_mem_info.size = index;
    
    if (index < DEFAULT_CAPACITY) {
        index = DEFAULT_CAPACITY;
    } 
    
    if (g_mem_info.capacity > index) {
        MemoryBlock* new_blocks = (MemoryBlock*)realloc(g_mem_info.blocks, sizeof(MemoryBlock) * index);
        if (!new_blocks) {
            return;
        }
        g_mem_info.blocks = new_blocks;
        g_mem_info.capacity = index;
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
            ansi_c_mem_track_free_block(i);
        }
    }
    cleanup_allocations_if_needed();
}

size_t ansi_c_mem_track_get_next_object_id(void)
{
    return next_object_id++;
}

/**
 * @brief Searches for a memory block with the given pointer in the block_list.
 *
 * @param ptr A pointer to the memory block.
 *
 * @return A pointer to the found memory block, or NULL if not found.
 */
const MemoryBlock* find_block_by_ptr(void* ptr)
{
    for (size_t i = 0; i < g_mem_info.size; ++i) {
        if (g_mem_info.blocks[i].address == ptr) {
            return &g_mem_info.blocks[i];
        }
    }

    return NULL;
}

const MemoryBlock* ansi_c_mem_track_get_block_info(const void* ptr) {
    for (size_t i = 0; i < g_mem_info.size; ++i) {
        if (g_mem_info.blocks[i].address == ptr) {
            return &g_mem_info.blocks[i];
        }
    }
    return NULL;
}

bool ansi_c_mem_track_log_block_info(const char* file_name, const MemoryBlock* block) {
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
    const char* formatstr = "%s [MEMORY] Memory Block Information:\n"
        "                             Address: 0x%lx\n"
        "                             Size: %lu bytes\n"
        "                             File: %s\n"
        "                             Comment: %s\n"
        "                             Type: %s\n"
        "                             Is allocated: %s\n"
        "                             Object ID: %lu\n";
    size_t message_size = snprintf(NULL, 0, formatstr,
        timestamp, (uintptr_t)block->address, (unsigned long)block->size, block->file_name, block->comment, block->type, 
        (block->is_allocated?"True":"False"), block->optional_object_id);

    // Allocate the message buffer
    char* message = (char*)malloc(message_size + 1);
    if (!message) {
        return false;
    }

    // Format the message
    snprintf(message, message_size + 1, formatstr,
        timestamp, (uintptr_t)block->address, (unsigned long)block->size, block->file_name, block->comment, block->type,
        (block->is_allocated ? "True" : "False"), block->optional_object_id);

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

const MemoryBlock** ansi_c_mem_track_get_unfreed_blocks_info(size_t* count)
{
    // Count the number of unfreed blocks
    size_t unfreed_count = 0;
    for (size_t i = 0; i < g_mem_info.size; ++i) {
        if (g_mem_info.blocks[i].is_allocated) {
            ++unfreed_count;
        }
    }

    *count = 0;

    // free if necessary
    ansi_c_mem_track_free_unfreed_blocks_info();

    // Allocate memory for the unfreed blocks array
    g_mem_info.get_unfreed_blocks_info_ptr = (MemoryBlock**)malloc(sizeof(MemoryBlock) * unfreed_count);
    if (!g_mem_info.get_unfreed_blocks_info_ptr) {
        return NULL;
    }

    // Copy the unfreed blocks into the array
    for (size_t i = 0; i < g_mem_info.size && g_mem_info.get_unfreed_blocks_info_size < unfreed_count; ++i) {
        if (g_mem_info.blocks[i].is_allocated) {
            MemoryBlock* mb_ptr=ansi_c_mem_track_copy_memory_block(&g_mem_info.blocks[i]);
            if (mb_ptr) {
                g_mem_info.get_unfreed_blocks_info_ptr[g_mem_info.get_unfreed_blocks_info_size++] = mb_ptr;
                
            } else {
                return NULL;
            }
        }
    }
    *count = g_mem_info.get_unfreed_blocks_info_size;
    return g_mem_info.get_unfreed_blocks_info_ptr;
}

bool ansi_c_mem_track_log_unfreed_blocks_info(const char* file_name, const MemoryBlock** blocks, size_t count)
{
    bool retval = true;
    for (size_t i = 0; i < count && retval; ++i) {
        retval=ansi_c_mem_track_log_block_info(file_name, blocks[i]);
    }
    return retval;
}