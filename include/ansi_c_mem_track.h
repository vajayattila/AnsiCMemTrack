#ifndef ANSI_C_MEM_TRACK_H
#define ANSI_C_MEM_TRACK_H

/**
  *
  * @file ansi_c_commandlineparser.h
  * @brief AnsiCMemTrack
  * 
  * This library provides a memory management tool in an ANSI C environment, which is capable of keeping track of
  * the allocated and freed memory, and able to perform memory allocation, deallocation, and resizing.
  *	@author Attila Vajay
  *	@email vajay.attila@gmail.com
  * @git https://github.com/vajayattila/AnsiCMemTrack.git
  * @date 2023.03.23.-2023.03.24.
  * @version 1.0
  * @license MIT License
  * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
  * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
  * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  * subject to the following conditions:
  * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
  * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  * For more information, see the file LICENSE.
  */


/**
 * @file ansi_c_mem_track.h
 * @brief ANSI C Memory Tracking Tool
 *
 * This library provides a memory management tool in an ANSI C environment, which is capable of keeping track of
 * the allocated and freed memory, and able to perform memory allocation, deallocation, and resizing.
 */

#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_CAPACITY 100

/**
 * @brief Memory block structure to store information about allocated memory blocks.
 */
typedef struct {
    void* address;        /**< The address of the memory block. */
    size_t size;          /**< The size of the memory block. */
    const char* file_name;/**< The name of the source file where the memory block was allocated. */
    const char* comment;  /**< A comment to identify the memory allocation. */
    const char* type;     /**< The type of data stored in the memory block. */
    bool is_allocated;    /**< The status of the memory block: allocated, freed. */
    size_t optional_object_id;  /**< The optional object ID to identify memory allocations. */
} MemoryBlock;

/**
 * @brief Data structure for tracking memory usage.
 */
typedef struct {
    MemoryBlock* blocks; /**< Dynamically allocated array of memory blocks. */
    size_t capacity; /**< Capacity of the `blocks` array. */
    size_t total_size; /**< Total size of all memory blocks allocated. */
    size_t size; /**< Number of memory blocks currently allocated. */
    size_t total_memory_usage; /**< Total memory usage (excluding overhead). */
    size_t memory_usage; /**< Memory usage (excluding overhead). */
    size_t total_freed_memory; /**< Total amount of memory freed so far. */
    bool is_initialized;        /**< The initialization status of the memory tracker. */
    MemoryBlock** get_unfreed_blocks_info_ptr; /**< A pointer to the array of MemoryBlock pointers representing the unfreed memory blocks */
    size_t get_unfreed_blocks_info_size; /**< Number items in unfreed blocks info array */
} MemoryInfo;

/**
 * @brief Struct for storing memory usage information.
 *
 * This struct is used to store information about the memory usage by the user's memory allocation functions.
 * It includes the total memory usage, the current number of memory blocks allocated, and the total amount of memory
 * freed so far.
 */
typedef struct {
    size_t size; /**< Number of memory blocks currently allocated. */
    size_t total_size; /**< Total size of all memory blocks allocated. */
    size_t total_user_memory_usage; /**< Total memory usage by the user's memory allocation functions. */
    size_t memory_usage; /**< Memory usage (excluding overhead). */
    size_t total_freed_memory; /**< Total amount of memory freed so far. */
} MemoryUsageInfo;

 /**
  * @brief Initializes the AnsiCMemTrack library
  *
  * This function initializes the AnsiCMemTrack library. This function should be called before any other function
  * in the library is used.
  *
  * @return true if initialization was successful, false otherwise.
  */
bool ansi_c_mem_track_init(void);

/**
 * @brief Allocates memory and tracks it.
 *
 * @param size The size of the memory to be allocated.
 * @param optional_object_id Optional object ID to identify memory allocations.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void* ansi_c_mem_track_malloc(size_t size, const char* file_name, const char* comment, const char* type, size_t optional_object_id);

/**
 * @brief Reallocates memory and tracks it.
 *
 * @param ptr A pointer to the memory block to be reallocated.
 * @param size The size of the new memory block.
 * @param optional_object_id Optional object ID to identify memory allocations.
 * @return A pointer to the reallocated memory, or NULL if the reallocation fails.
 */
void* ansi_c_mem_track_realloc(void* ptr, size_t size, size_t optional_object_id);

/**
 * @brief Frees the memory pointed to by `ptr` and stores information about the deallocation in the memory tracker.
 *
 * @param ptr Pointer to the memory to be freed.
 */
void ansi_c_mem_track_free(void* ptr);

/**
 * @brief Frees memory and updates the memory tracker based on the optional_object_id parameter.
 *
 * @param optional_object_id Optional object ID to identify memory allocations.
 * @return None.
 */
void ansi_c_mem_track_free_by_object_id(size_t optional_object_id);

/**
 * @brief Deinitializes the memory tracker and frees any remaining memory allocated by the library.
 *
 * @return None.
 */
void ansi_c_mem_track_deinit(void);

/**
 * @brief Returns information about the memory usage of the user's code.
 * This function returns information about the memory usage of the user's code, including the total number of
 * memory blocks allocated, the total size of all memory blocks allocated, the current memory usage, and the total
 * amount of memory freed so far.
 * @return A MemoryUsageInfo struct containing the memory usage information.
 */
MemoryUsageInfo ansi_c_mem_track_get_info(void);

/**
 * @brief Writes a log message to the specified file.
 *
 * This function writes a log message to the specified file or to the standard output if the file name is NULL.
 * The log message includes the current date and time, the message type, and the message text.
 *
 * @param file_name The name of the file to write the log message to. If NULL, the message will be written to the standard output.
 * @param message_type The type of the log message (e.g. INFO, WARNING, ERROR).
 * @param message_text The text of the log message.
 *
 * @return true if the message was successfully written, false otherwise.
 */
bool ansi_c_mem_track_log_message(const char* file_name, const char* message_type, const char* message_text);

/**
 * @brief Prints information about memory usage to the specified file or to the standard output.
 *
 * This function prints information about the memory usage of the program to the specified file or to the standard output
 * in a human-readable format. If file_name is NULL, the information is printed to the standard output.
 *
 * @param file_name The name of the file to write the information to, or NULL to write to the standard output.
 * @param info A pointer to a MemoryUsageInfo struct containing the memory usage information.
 *
 * @return true if the information was successfully printed, false otherwise.
 */
bool ansi_c_mem_track_print_info(const char* file_name, const MemoryUsageInfo* info);

/**
 * @brief Cleans up deallocated memory blocks from the memory usage tracking array.
 *
 * This function removes all memory blocks from the memory usage tracking array where the "is_allocated" flag is false.
 * It also shrinks the array if the number of unused elements exceeds a certain threshold.
 */
void ansi_c_mem_track_cleanup_allocations(void);

/**
 * @brief Returns a unique object_id to be used in memory allocation tracking.
 * This function generates a unique object_id by incrementing a variable every time it is called.
 * The generated object_id can be used to track memory allocations and deallocations.
 * @return The generated object_id.
 */
size_t ansi_c_mem_track_get_next_object_id(void);

/*@brief Get information about a memory block with the given pointer.
*
* @param ptr A pointer to the memory block.
*
* @return A pointer to a const MemoryBlock struct with information about the memory block, or NULL if not found.
*/
const MemoryBlock* ansi_c_mem_track_get_block_info(const void* ptr);

/**
 * @brief Logs information about the given memory block.
 *
 * @param file_name The name of the log file, or NULL to log to standard output.
 * @param block A pointer to the memory block to log.
 *
 * @return true if logging was successful, false otherwise.
 */
bool ansi_c_mem_track_log_block_info(const char* file_name, const MemoryBlock* block);

/**
 * @brief Gets an array of pointers to unfreed memory blocks.
 *
 * The returned array and the memory it points to are dynamically allocated
 * and should be freed by calling this function again or by calling
 * `ansi_c_mem_track_deinit`. The returned array must be treated as
 * read-only, and its contents should not be modified or freed.
 *
 * @param count A pointer to a size_t variable that will receive the number of
 * unfreed blocks. If there was an error, this will be set to 0.
 *
 * @return An array of pointers to unfreed memory blocks, or NULL if there was an
 * error. The returned array and the memory it points to are dynamically
 * allocated and should be freed by calling this function again or by calling
 * `ansi_c_mem_track_deinit` or recalling `ansi_c_mem_track_get_unfreed_blocks_info`. The returned array must be treated as read-only,
 * and its contents should not be modified or freed.
 *
 * @note The returned array and the memory it points to should be freed by
 * calling this function again or by calling `ansi_c_mem_track_deinit`.
 */
const MemoryBlock** ansi_c_mem_track_get_unfreed_blocks_info(size_t* count);

/**
 * @brief Logs information about unfreed memory blocks to a file or stdout.
 *
 * This function logs information about unfreed memory blocks to the specified file or to stdout if file_name is NULL.
 *
 * @param file_name The name of the file to write the log to, or NULL to write to stdout.
 * @param blocks An array of unfreed MemoryBlock structs.
 * @param count The number of unfreed MemoryBlock structs in the blocks array.
 * @return True if the log was successfully written, false otherwise.
 */
bool ansi_c_mem_track_log_unfreed_blocks_info(const char* file_name, const MemoryBlock* blocks, size_t count);

#endif /* ANSI_C_MEM_TRACK_H */