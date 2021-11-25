#include "log.h"
#include <time.h>

static FILE* log_file = NULL;

// 1 - only file
// 2 - only term
// 3 - file and term
// else - nothing

#define LOG_SOURCE 0
#if LOG_SOURCE == 1 || LOG_SOURCE == 3
	static FILE* log_file = NULL;
#endif

#define ERROR_LVL 0
#define DEBUG_LVL 1
#define TRACE_LVL 2

#define TERM_RESET   	  "\033[0m"
#define ERROR_FONT        "\033[1;31m"
#define WARNING_FONT  	  "\033[1;33m"
#define TERM_WHITE_FONT   "\033[1;37m"
#define TERM_GREEN_FONT   "\033[1;32m"
#define INVALID_LOG_FONT  "\033[1;34m"

#define LOG_LVL

//------------LOCAL-FUNCTIONS-DECLARATION----------------------------------

void get_log_name(char* buf);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void LogInit(){

	#if LOG_SOURCE == 1 || LOG_SOURCE == 3

		char name[MAX_LOG_NAME_LEN] = "";
		get_log_name(name);
		log_file = fopen(name, "w");
		assert(log_file != NULL);
	#endif

	return;
}

//-----------------------------------------------------

void LogClose(){

	#if LOG_SOURCE == 1 || LOG_SOURCE == 3
		assert(log_file != NULL);

		fclose(log_file);
	#endif

	return;
}

//-----------------------------------------------------

void ToLog(LOG_TYPE type, const char* str, ...){

	assert(str != NULL);

	va_list args;
	va_start(args, str);

	#if LOG_SOURCE == 1 || LOG_SOURCE == 3
		assert(log_file != NULL);

		if(type == LOG_TYPE::ERROR){
			fprintf(log_file, "%s[++ERROR++]: %s", ERROR_FONT);
			vfprintf(log_file, str, args);
			fprintf(log_file, "%s", TERM_RESET);
		}
		else if(type == LOG_TYPE::INFO){
			vfprintf(log_file, str, args);
		}
		else{
			fprintf(log_file, "%s[++WARNING++] invalid log type%s\n", WARNING_FONT, TERM_RESET);
		}

	#elif LOG_SOURCE == 2 || LOG_SOURCE == 3
		if(type == LOG_TYPE::ERROR){
			printf("%s[++ERROR++]: %s", ERROR_FONT);
			vprintf(str, args);
			printf("%s", TERM_RESET);
		}
		else if(type == LOG_TYPE::INFO){
			vprintf(str, args);
		}
		else{
			printf("%s[++WARNING++] invalid log type%s\n", WARNING_FONT, TERM_RESET);
		}
	#endif

	va_end(args);

	return;
}

//--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

void get_log_name(char* buf){

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

    strftime(buf, MAX_LOG_NAME_LEN, "%H:%M:%S_%d.%m.%Y", ptm);

    return;
}
