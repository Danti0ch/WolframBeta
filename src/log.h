#ifndef LOG_H
#define LOG_H

#define LOCATION __FILE__, __FUNCTION__, __LINE__
#define LOC_PARAMS const char* file_name, const char* func_name, const int line

const int  MAX_LOG_NAME_LEN = 256;
const char LOG_LOCATION[20] = "../logs/";

enum class LOG_TYPE{
	INVALID,
	ERROR,
	WARNING,
	INFO
};

/*
#define TO_LOG(type, str, ...)		\
ToLog(LOCATION, (type), (str), ...)
*/

#ifndef DEBUG_MODE
#define DEBUG_MODE 0/**
					 * DEBUG_MODE = 1,  дамп будет показывать все данные о ноде,
					 *		           логи будут выводиться при каждом вызове функций, обозначая состояние структуры
					 *
					 * DEBUG_MODE = 0,    дамп будет показывать только информацию о значении нода,
					 *                    логи будут выводиться при ошибках, предупреждениях и ситуациях,
					 */
#endif

#ifndef ENABLE_CMD_LOGS
#define ENABLE_CMD_LOGS 0/**
						  * Если равен 1, то логи выводятся в консоль
						  *
						  */
#endif

#define TERM_RESET   	  "\033[0m"
#define ERROR_FONT        "\033[1;31m"
#define WARNING_FONT  	  "\033[1;33m"
#define INFO_FONT	      "\033[1;37m"
#define TERM_GREEN_FONT   "\033[1;32m"
#define INVALID_LOG_FONT  "\033[1;34m"

void LogInit();

void LogClose();

void ToLog(LOG_TYPE type, const char* str, ...);

#endif
