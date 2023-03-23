
# AnsiCMemTrack
AnsiCMemTrack is a memory management tool for ANSI C environments. It keeps track of the allocated and freed memory, and is able to perform memory allocation, deallocation, and resizing. This library provides an easy-to-use interface for memory management, which is especially useful for large-scale projects with complex memory management requirements.

## Usage Guide
To use AnsiCMemTrack in your project, follow these steps:

* Include the header file ansi_c_mem_track.h in your source code.
* Call the ansi_c_mem_track_init() function before any memory allocation takes place.
* Allocate memory using the ansi_c_mem_track_malloc() function instead of malloc().
* Reallocate memory using the ansi_c_mem_track_realloc() function instead of realloc().
* Free memory using the ansi_c_mem_track_free() function instead of free().
* When you are finished using AnsiCMemTrack, call the ansi_c_mem_track_cleanup_allocations() function to free any remaining memory and clean up the allocation tracking data.
* Finally, call the ansi_c_mem_track_deinit() function to release any resources used by AnsiCMemTrack.

## Functions: 

### `void ansi_c_mem_track_init()`
Initializes the AnsiCMemTrack library by setting up the data structures necessary for tracking memory allocations and deallocations. This function should be called before any memory allocation takes place in your program.

#### Parameters
None.

#### Return Value
None.

#### Example Usage
```c
#include "ansi_c_mem_track.h"

int main() {
    ansi_c_mem_track_init();
    /* Allocate and free memory using the AnsiCMemTrack library */
    ansi_c_mem_track_deinit();
    return 0;
}
```

### `void ansi_c_mem_track_deinit(void)`
This function deinitializes the AnsiCMemTrack library, releasing any resources used by the library.

#### Example Usage
```c
#include "ansi_c_mem_track.h"

int main() {
    // Initialize library
    ansi_c_mem_track_init();

    // Perform memory allocation and deallocation

    // Deinitialize library
    ansi_c_mem_track_deinit();
    
    return 0;
}
```

### `ansi_c_mem_track_malloc`
Allocates a block of memory of the given size, and tracks the allocation with AnsiCMemTrack. The file, function, and type parameters should be used to identify the source of the allocation for tracking purposes.

#### Parameters
* `size`: The size of the memory block to allocate, in bytes.
* `file`: The name of the source file where the allocation takes place.
* `func`: The name of the function where the allocation takes place.
* `type`: A string that identifies the type of memory being allocated.
* `count`: The number of items being allocated.

#### Return Value
Returns a pointer to the newly allocated memory block, or NULL if the allocation fails.

#### Example
```c
int* int_ptr = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, __func__, "int", 1);
```

#### Notes
This function should be used instead of malloc() to ensure proper tracking of memory allocations.

### `ansi_c_mem_track_realloc`
Reallocates a block of memory of the given size, and tracks the allocation with AnsiCMemTrack. The file, function, and type parameters should be used to identify the source of the allocation for tracking purposes.
Parameters

#### Parameters
* ptr: A pointer to the previously allocated memory block to be reallocated.
* size: The size of the memory block to allocate, in bytes.
* file: The name of the source file where the allocation takes place.
* func: The name of the function where the allocation takes place.
* type: A string that identifies the type of memory being allocated.
* count: The number of items being allocated.

#### Return Value
Returns a pointer to the newly allocated memory block, or NULL if the allocation fails.

#### Example
```c
int* int_ptr = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, __func__, "int", 1);
// ...
int_ptr = (int*)ansi_c_mem_track_realloc(int_ptr, sizeof(int) * 10, __FILE__, __func__, "int", 1);
```

#### Notes
This function should be used instead of realloc() to ensure proper tracking of memory allocations.

### ansi_c_mem_track_free
Frees the memory block pointed to by the given pointer and tracks the deallocation with AnsiCMemTrack.

#### Parameters
* *ptr: A pointer to the memory block to free.

#### Return Value
None

#### Example
```c
int* int_ptr = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, __func__, "int", 1);
ansi_c_mem_track_free(int_ptr);
```
#### Notes
This function should be used instead of free() to ensure proper tracking of memory deallocations.

### `ansi_c_mem_track_free_by_object_id`
Frees the memory block allocated with the given object ID, and removes the allocation tracking data associated with it.

#### Parameters
* *object_id: The ID of the object whose memory block should be freed.

#### Example
```c
//...
// allocate memory blocks
int* int_ptr1 = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, "test_function() -> int_ptr1 memory allocation", "int", 1);
int* int_ptr2 = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, "test_function() -> int_ptr2 memory allocation", "int", 2);
int* int_ptr3 = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, "test_function() -> int_ptr3 memory allocation", "int", 3);

// free memory blocks by object ID
ansi_c_mem_track_free_by_object_id(2);

// allocate memory block with object ID = 4
int* int_ptr4 = (int*)ansi_c_mem_track_malloc_with_id(sizeof(int), __FILE__, "test_function() -> int_ptr4 memory allocation with ID = 4", "int", 4);

// free memory block with object ID = 1
ansi_c_mem_track_free_by_object_id(1);
//...
```

#### Notes
This function should only be used to free memory allocated with ansi_c_mem_track_malloc() or ansi_c_mem_track_realloc(). Using an invalid object ID may result in undefined behavior.

### `ansi_c_mem_track_get_info`
Retrieves the current memory usage information as a MemoryUsageInfo struct.

#### Parameters
none

#### Return Value
Returns a MemoryUsageInfo struct containing the current memory usage information.

#### Example
```c
MemoryUsageInfo mem_info = ansi_c_mem_track_get_info();
```

#### Notes
This function can be used to retrieve information about the current state of memory usage, such as the total amount of memory allocated and the number of allocations currently in use. The information is returned as a `MemoryUsageInfo` struct, which contains the following fields:
* `total_bytes_allocated`: The total number of bytes allocated.
* `total_allocations`: The total number of allocations made.
* `total_frees`: The total number of frees made.
* `current_bytes_allocated`: The current number of bytes allocated.
* `current_allocations`: The current number of allocations.
* `current_frees`: The current number of frees.

The `MemoryUsageInfo` struct is defined in the header file `ansi_c_mem_track.h`.

### ansi_c_mem_track_print_info
Prints memory usage information to the specified output stream. The output can be customized by providing a MemoryUsageInfo structure that contains information about the memory usage.

#### Parameters
* `stream`: The output stream to print the information to.
* `mem_info`: A pointer to a MemoryUsageInfo structure that contains the memory usage information to be printed.

#### Return Value
None.

#### Example
```c
MemoryUsageInfo mem_info = ansi_c_mem_track_get_info();
ansi_c_mem_track_print_info(stdout, &mem_info);
```

#### Notes
This function is useful for debugging memory usage and checking for memory leaks.

It is important to note that this function only prints the memory usage information and does not free any memory. Use the ansi_c_mem_track_cleanup_allocations() function to free any remaining memory and clean up the allocation tracking data.

### `ansi_c_mem_track_log_message`
Logs a message to the output console, with the given message level, message text, source file, function, and line number.

#### Parameters
* `file`: The name of the source file where the log message is generated.
* `level`: The level of the log message. Possible values are: "Debug", "Info", "Warning", and "Error".
* `msg`: The text of the log message.
* `func`: The name of the function where the log message is generated. (Optional)
* `line`: The line number where the log message is generated. (Optional)

#### Example
```c
ansi_c_mem_track_log_message(__FILE__, "Info", "Memory allocation succeeded", __func__, __LINE__);
```

#### Notes
This function is used to log messages during memory allocation, deallocation, and tracking operations for debugging and diagnostic purposes.

### ansi_c_mem_track_get_next_object_id
Returns the next available object ID. This function is used to generate unique object IDs for separate memory management instances.

#### Return Value
Returns the next available object ID.

#### Example
```c
size_t object_id = ansi_c_mem_track_get_next_object_id();
```

#### Notes
Users can call this function to obtain a unique object ID for their separate memory management instances.


### Examples

#### Separate Memory Management in C++ using AnsiCMemTrack
This example demonstrates how to use AnsiCMemTrack in a C++ environment with two separate classes for memory management, which handle memory allocation and deallocation separately using the object_id parameter.
```c
#include "ansi_c_mem_track.h"
#include <iostream>

class MyClassA {
public:
    MyClassA() {
        // allocate memory using object_id 1
        object_id=ansi_c_mem_track_get_next_object_id();
        ptr1 = (int*)ansi_c_mem_track_malloc(sizeof(int), __FILE__, "MyClassA::MyClassA() -> ptr1 memory allocation", "int", object_id );
    }
    ~MyClassA() {
        // free memory using object_id 1
        ansi_c_mem_track_free_by_object_id(object_id);
    }
private:
    int* ptr1;
    size_t object_id;
};

class MyClassB {
public:
    MyClassB() {
        // allocate memory using object_id 2
        object_id=ansi_c_mem_track_get_next_object_id();
        ptr2 = (float*)ansi_c_mem_track_malloc(sizeof(float), __FILE__, "MyClassB::MyClassB() -> ptr2 memory allocation", "float", object_id );
    }
    ~MyClassB() {
        // free memory using object_id 2
        ansi_c_mem_track_free_by_object_id(object_id);
    }
private:
    float* ptr2;
    size_t object_id;
};

int main() {
    // initialize AnsiCMemTrack library
    ansi_c_mem_track_init();

    {
        // create instances of MyClassA and MyClassB
        MyClassA objA;
        MyClassB objB;

        // print memory usage information
        MemoryUsageInfo mem_info = ansi_c_mem_track_get_info();
        ansi_c_mem_track_print_info(__FILE__, &mem_info);
    }

    // print memory usage information after objects are destroyed
    MemoryUsageInfo mem_info = ansi_c_mem_track_get_info();
    ansi_c_mem_track_print_info(__FILE__, &mem_info);

    // cleanup allocations and deinitialize AnsiCMemTrack library
    ansi_c_mem_track_cleanup_allocations();
    ansi_c_mem_track_deinit();

    return 0;
}
```

### Requirements
* C compiler that supports C99 or later versions (e.g., GCC, Clang)
* Optional: C11 support for building the test application (e.g., GCC, Clang) 
* Standard C library (libc)
* The library has been tested on Windows (using Visual Studio 2019).

#### Note
The C11 support is only required for building the test application and not for using the AnsiCMemTrack library itself.

### License
The AnsiCMemTrack library is licensed under the MIT License. See the `LICENSE` file for more information.