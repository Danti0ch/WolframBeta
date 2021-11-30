#include "log.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

static FILE* log_file = NULL;

//------------LOCAL-FUNCTIONS-DECLARATION----------------------------------

void get_log_name(char* buf);

//--------------PUBLIC-FUNCTIONS-DEFINITIONS-----------------------------------------

void LogInit(){

	#if ENABLE_CMD_LOGS
		char name[MAX_LOG_NAME_LEN] = "";
		get_log_name(name);
		printf("%s\n", name);
		log_file = fopen(name, "w");
		assert(log_file != NULL);

		ToLog(LOG_TYPE::INFO, "Logging initiated");
	#endif

	return;
}

//-----------------------------------------------------

void LogClose(){

	#if ENABLE_CMD_LOGS
		assert(log_file != NULL);
		ToLog(LOG_TYPE::INFO, "Logging closed");
		fclose(log_file);
	#endif

	return;
}

//-----------------------------------------------------

void ToLog(LOG_TYPE type, const char* str, ...){

	assert(str      != NULL);
	assert(log_file != NULL);

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

    char time_str[20] = "";
    strftime(time_str, 16, "%H:%M:%S", ptm);

	va_list args;
	va_start(args, str);

	if(type == LOG_TYPE::ERROR){
		fprintf(log_file, "[ERROR] %s: ", time_str);
	}
	else if(type == LOG_TYPE::INFO){
		fprintf(log_file, "[INFO] %s: ", time_str);
	}
	else if(type == LOG_TYPE::WARNING){
		fprintf(log_file, "[WARNING] %s: ", time_str);
	}
	vfprintf(log_file, str, args);
	fprintf(log_file, "\n");

	#if ENABLE_CMD_LOGS
		
		if(type == LOG_TYPE::ERROR){
			printf("%s[ERROR] %s: ", ERROR_FONT, time_str);
		}
		else if(type == LOG_TYPE::INFO){
			printf("%s[INFO] %s: ", WARNING_FONT, time_str);
		}
		else if(type == LOG_TYPE::WARNING){
			printf("%s[WARNING] %s: ", INFO_FONT, time_str);
		}
		
		vprintf(str, args);
		printf("%s\n", TERM_RESET);
	#endif

	va_end(args);

	return;
}

//--------------LOCAL-FUNCTIONS-DEFINITIONS-----------------------------------------

void get_log_name(char* buf){

	strcpy(buf, LOG_LOCATION);

    time_t rawtime = time(NULL);
    struct tm *ptm = localtime(&rawtime);

    strftime(buf + strlen(buf), MAX_LOG_NAME_LEN, "%H-%M-%S(%d.%m.%Y)", ptm);

    return;
}
