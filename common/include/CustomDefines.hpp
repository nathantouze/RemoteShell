#ifndef _CUSTOM_DEFINES_
#define _CUSTOM_DEFINES_

#define MAX_CMD_LENGTH 512
#define MAX_OUTPUT_LENGTH 16384
#define MAX_OS_LENGTH 32
#define MAX_PWD_LENGTH 128
#define MAX_USERNAME_LENGTH 64

#ifdef _WIN32
    #define POPEN _popen
    #define STRDUP _strdup
    #define PCLOSE _pclose
    #define SETENV _putenv
    #define SHELL_PATH "ComSpec=C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe"
    #define SEPARATOR '\\'
#else
    #define POPEN popen
    #define PCLOSE pclose
    #define STRDUP strdup
    #define SETENV putenv
    #define SHELL_PATH "SHELL=/bin/bash"
    #define SEPARATOR '/'
#endif

#endif