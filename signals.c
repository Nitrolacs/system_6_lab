#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#include "signals.h"

// Функция для обработки сигналов, приводящих к аварийному завершению процесса
void signalHandler(int signum) {
    // Выводим сообщение об ошибке в зависимости от типа сигнала
    switch (signum) {
        case SIGINT:
            fprintf(stderr, "Программа прервана пользователем.\n");
            break;
        case SIGTERM:
            fprintf(stderr, "Программа завершена системой.\n");
            break;
        case SIGSEGV:
            fprintf(stderr, "Программа вызвала ошибку сегментации.\n");
            break;
        default:
            fprintf(stderr, "Программа завершена неизвестным сигналом.\n");
            break;
    }
    // Выходим из программы с кодом ошибки
    exit(1);
}

// Функция для обработки таймера неактивности пользователя
void timeoutHandler() {
    // Выводим сообщение об ошибке
    fprintf(stderr, "Превышено время ожидания.\n");
    // Выходим из программы с кодом ошибки
    exit(1);
}

// Функция для открытия файла журнала для записи или создания его, если он не существует
void openLog(char **log_file, FILE **logfd, char* logFileName) {
    // Если имя файла журнала не задано, то используем стандартное имя
    if (*log_file == NULL) {
        *log_file = logFileName;
    }
    // Открываем файл журнала для записи или создаем его, если он не существует
    *logfd = fopen(*log_file, "a");
    // Проверяем на ошибки
    if (*logfd == NULL) {
        perror("fopen");
        exit(1);
    }
}

// Функция для записи сообщений в файл журнала с помощью fprintf
void writeLog(FILE *logfd, const char *format, ...) {
    // Проверяем, что файл журнала открыт
    if (logfd == NULL) {
        return;
    }
    // Получаем текущее время и форматируем его в строку
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm);
    // Выводим время в файл журнала
    fprintf(logfd, "[%s] ", time_str);

    // Этот код позволяет записывать разные сообщения в файл журнала
    // с помощью одной функции write_log.

    // Выводим сообщение в файл журнала с помощью переменного числа аргументов
    va_list args;  // информация о списке аргументов.
    // Инициализировать переменную args с помощью макроса va_start, передав
    // ей последний известный параметр функции write_log. В данном случае,
    // это параметр format, который содержит формат сообщения.
    va_start(args, format);
    // Первым параметром vfprintf является дескриптор файла журнала logfd,
    // вторым - формат сообщения format, третьим - список аргументов args.
    vfprintf(logfd, format, args);
    // Освобождаем ресурсы, связанные со списком аргументов args, с
    // помощью макроса va_end.
    va_end(args);
}


// Функция для установки таймера неактивности пользователя с помощью alarm и signal
void setTimer(int timeout) {
    // Проверяем, что время ожидания задано
    if (timeout > 0) {
        // Устанавливаем таймер с помощью alarm
        alarm(timeout);
        // Устанавливаем обработчик сигнала SIGALRM
        signal(SIGALRM, timeoutHandler);
    }
}