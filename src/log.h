#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

const int MAX_LOG_NAME_LEN = 256;

enum class LOG_TYPE{
	INVALID,
	ERROR,
	INFO
};

#define LOCATION __FILE__, __FUNCTION__, __LINE__
#define LOC_PARAMS const char* file_name, const char* func_name, const int line

#define TO_LOG(type, str, ...)		\
ToLog(LOCATION, (type), (str), ...);

void LogInit();

void LogClose();

void ToLog(LOC_PARAMS, LOG_TYPE type, const char* str, ...);

#endif
