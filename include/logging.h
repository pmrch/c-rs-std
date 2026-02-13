#ifndef LOGGING_H
#define LOGGING_H

// Default log level
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

// Log levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

// DEBUG
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) \
    printf("\x1b[36m[DEBUG]\x1b[0m %s:%d:%s: " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

// INFO
#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) \
    printf("\x1b[32m[INFO]\x1b[0m %s:%d:%s: " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

// WARN
#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) \
    printf("\x1b[33m[WARN]\x1b[0m %s:%d:%s: " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

// ERROR
#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...) \
    printf("\x1b[31m[ERROR]\x1b[0m %s:%d:%s: " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

#endif