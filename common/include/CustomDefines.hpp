#ifndef _CUSTOM_DEFINES_
#define _CUSTOM_DEFINES_

#define MAX_CMD_LENGTH 512
#define MAX_OUTPUT_LENGTH 8192
#define MAX_OS_LENGTH 32
#define MAX_PWD_LENGTH 128

#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
    #define SETENV _putenv
    #define GETOS get_windows_os
    #define SHELL_PATH "ComSpec=C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe"
#else
    #define POPEN popen
    #define PCLOSE pclose
    #define SETENV putenv
    #define GETOS get_other_os
    #define SHELL_PATH "SHELL=/bin/bash"
#endif

#endif