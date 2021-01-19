#ifndef _CUSTOM_DEFINES_
#define _CUSTOM_DEFINES_

#define MAX_CMD_LENGTH 512
#define MAX_OUTPUT_LENGTH 8192

#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

#endif