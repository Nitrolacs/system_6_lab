/*! Функция клиента */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <time.h>
#include <stdarg.h>

#include "client.h"
#include "interface.h"

#define PORT 5555
#define MAXDATASIZE 1024


// Глобальные переменные для хранения аргументов командной строки
char *log_file = NULL; // имя файла журнала
int timeout = 0; // время ожидания ввода пользователя в секундах
int verbose = 0; // флаг для вывода дополнительной информации

// Глобальные переменные для хранения дескрипторов сокета и файла журнала
int sockfd = -1; // дескриптор сокета
FILE *logfd = NULL; // дескриптор файла журнала

// Функция для обработки сигналов, приводящих к аварийному завершению процесса
void signal_handler(int signum) {
    // Закрываем сокет и файл журнала
    if (sockfd != -1) {
        close(sockfd);
    }
    if (logfd != NULL) {
        fclose(logfd);
    }
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
void timeout_handler(int signum) {
    // Закрываем сокет и файл журнала
    if (sockfd != -1) {
        close(sockfd);
    }
    if (logfd != NULL) {
        fclose(logfd);
    }
    // Выводим сообщение об ошибке
    fprintf(stderr, "Превышено время ожидания ввода пользователя.\n");
    // Выходим из программы с кодом ошибки
    exit(1);
}

// Функция для разбора аргументов командной строки
void parse_args(int argc, char *argv[]) {
    int opt;
    // Опции для getopt
    const char *optstring = "l:t:v";
    // Парсим аргументы с помощью getopt
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'l': // имя файла журнала
                log_file = optarg;
                break;
            case 't': // время ожидания ввода пользователя
                timeout = atoi(optarg);
                break;
            case 'v': // флаг для вывода дополнительной информации
                verbose = 1;
                break;
            default: // неверный аргумент
                fprintf(stderr, "Использование: %s [-l log_file] [-t timeout] [-v]\n", argv[0]);
                exit(1);
        }
    }
}

// Функция для открытия файла журнала для записи или создания его, если он не существует
void open_log() {
    // Если имя файла журнала не задано, то используем стандартное имя
    if (log_file == NULL) {
        log_file = "client.log";
    }
    // Открываем файл журнала для записи или создаем его, если он не существует
    logfd = fopen(log_file, "a");
    // Проверяем на ошибки
    if (logfd == NULL) {
        perror("fopen");
        exit(1);
    }
}

// Функция для записи сообщений в файл журнала с помощью fprintf
void write_log(const char *format, ...) {
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
    // Выводим сообщение в файл журнала с помощью переменного числа аргументов
    va_list args;
    va_start(args, format);
    vfprintf(logfd, format, args);
    va_end(args);
}

// Функция для установки таймера неактивности пользователя с помощью alarm и signal
void set_timer() {
    // Проверяем, что время ожидания задано
    if (timeout > 0) {
        // Устанавливаем таймер с помощью alarm
        alarm(timeout);
        // Устанавливаем обработчик сигнала SIGALRM
        signal(SIGALRM, timeout_handler);
    }
}

int main(int argc, char* argv[])
{
    int sockfd; // дескриптор сокета
    char buffer[MAXDATASIZE]; // буфер для приема и отправки данных
    struct sockaddr_in servAddr; // структура адреса сервера

    // Объявляем переменные для коэффициентов уравнения
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0; // Добавляем переменную для четвертого коэффициента

    // Вызываем функцию для обработки коэффициентов уравнения из командной строки
    int result = ProcessCoefficients(argc, argv, &a, &b, &c, &d);

    // Проверяем результат функции
    if (result != 0)
    {
        fprintf(stderr, "Ошибка при обработке коэффициентов.\n");
        exit(1);
    }

    // Разбираем аргументы командной строки
    parse_args(argc, argv);

    // Открываем файл журнала
    open_log();
    // Создаем сокет с протоколом UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Проверяем на ошибки
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    servAddr.sin_family = AF_INET; // семейство адресов IPv4
    servAddr.sin_addr.s_addr = inet_addr(
            "127.0.0.1"); // адрес сервера (локальный)
    servAddr.sin_port = htons(PORT); // порт

    // Формируем строку с коэффициентами уравнения в зависимости от количества аргументов
    if (argc == 7) {
        sprintf(buffer, "%lf %lf %lf", a, b, c);
    } else {
        sprintf(buffer, "%lf %lf %lf %lf", a, b, c, d);
    }

    // Отправляем данные серверу с помощью функции sendto
    if (sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        perror("sendto");
        exit(1);
    }

    // Выводим информацию об отправленном запросе на экран и в файл журнала
    printf("Отправлен запрос: %s\n", buffer);
    write_log("Отправлен запрос: %s\n", buffer);

    // Устанавливаем таймер неактивности пользователя
    set_timer();

    // Принимаем данные от сервера с помощью функции recvfrom
    int numbytes = recvfrom(sockfd, buffer, MAXDATASIZE - 1, 0,
                            NULL, NULL);
    // Проверяем на ошибки
    if (numbytes == -1) {
        perror("recvfrom");
        exit(1);
    }
    buffer[numbytes] = '\0'; // добавляем нулевой символ в конец сообщения

    // Выводим информацию о полученном ответе на экран и в файл журнала
    printf("Получен ответ: %s\n", buffer);
    write_log("Получен ответ: %s\n", buffer);

    // Закрываем сокет и файл журнала
    close(sockfd);
    fclose(logfd);

    return 0;
}
