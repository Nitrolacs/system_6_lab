//
// Created by stud on 04.04.23.
//

#ifndef INC_6_LAB_SIGNALS_H
#define INC_6_LAB_SIGNALS_H

void signalHandler(int signum);

void timeoutHandler();

void openLog(char **log_file, char* logFileName);

void writeLog(const char *format, ...);

void setTimer(int timeout);



#endif //INC_6_LAB_SIGNALS_H
