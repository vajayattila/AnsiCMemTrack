#ifndef ANSI_C_MACRO_UTILS_H
#define ANSI_C_MACRO_UTILS_H

#ifdef _MSC_VER
    #define STRCPY(dest, destsz, src) strcpy_s(dest, destsz, src)
    #define STRDUP(dest, destsz, src, objid) { \
        size_t _sz = destsz + 1; \
        dest = ansi_c_mem_track_malloc(_sz, __FILE__, __FUNCTION__, "char*", objid); \
        if(dest){ \
            strncpy_s((char*)dest, _sz, (const char*)src, destsz); \
        }\
    }
    #define C_STRDUP(dest, destsz, src) { \
        size_t _sz = destsz + 1; \
        dest = malloc(_sz); \
        if(dest){ \
            strncpy_s((char*)dest, _sz, (const char*)src, destsz); \
        }\
    }
    #define STRNCPY(dest, destsz, src, count) strncpy_s(dest, destsz, src, count)
    #define SPRINTF sprintf_s
    #define FOPEN fopen_s
    #define localtime_func(time, timeinfo) localtime_s(timeinfo, time)
#else
    #define STRCPY(dest, destsz, src){ \
        STRNCPY(dest, destsz, src, strlen(src) + 1); \
    }
    #define STRDUP(dest, destsz, src, objid){ \
        size_t _sz = strlen(src) + 1; \
        dest = ansi_c_mem_track_malloc(_sz, __FILE__, __FUNCTION__, "char*", objid); \
        if (dest) { \
            STRNCPY(dest, destsz, src, _sz); \
        } \
    } 
    #define C_STRDUP(dest, destsz, src){ \
        size_t _sz = strlen(src) + 1; \
        dest = malloc(_sz); \
        if (dest) { \
            STRNCPY(dest, destsz, src, _sz); \
        } \
    } 
    #define STRNCPY(dest, destsz, src, count) { \
        size_t _sz = (destsz); \
        strncpy(dest, src, _sz - 1); \
        dest[_sz - 1] = '\0'; \
    } 
    #define SPRINTF sprintf
    #define FOPEN fopen
    #ifdef _POSIX_C_SOURCE
        #define localtime_func(time, timeinfo) localtime_r(time, timeinfo)
    #else
        #define localtime_func(time, timeinfo) localtime(time)
    #endif
#endif

#endif // ANSI_C_MACRO_UTILS_H