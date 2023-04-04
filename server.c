/*! Функция сервера */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <time.h>
#include <stdarg.h>

#include "server.h"
#include "logic.h"

#define PORT 5555
#define MAXBUF 1024


// Глобальные переменные для хранения аргументов командной строки
char *log_file = NULL; // имя файла журнала
int timeout = 0; // время ожидания сообщений от клиента в секундах
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

// Функция для обработки таймера неактивности клиентской стороны
void timeout_handler(int signum) {
    // Закрываем сокет и файл журнала
    if (sockfd != -1) {
        close(sockfd);
    }
    if (logfd != NULL) {
        fclose(logfd);
    }
    // Выводим сообщение об ошибке
    fprintf(stderr, "Превышено время ожидания сообщений от клиента.\n");

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
            case 't': // время ожидания сообщений от клиента
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
        log_file = "server.log";
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

// Функция для установки таймера неактивности клиентской стороны с помощью alarm и signal
void set_timer() {
    // Проверяем, что время ожидания задано
    if (timeout > 0) {
        // Устанавливаем таймер с помощью alarm
        alarm(timeout);
        // Устанавливаем обработчик сигнала SIGALRM
        signal(SIGALRM, timeout_handler);
    }
}



// Главная функция сервера
int main(int argc, char *argv[]) {
    int numbytes;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXBUF];
    socklen_t len;

    // Разбираем аргументы командной строки
    parse_args(argc, argv);

    // Открываем файл журнала
    open_log();

    // Создаем сокет
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Проверяем на ошибки
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    // Устанавливаем параметры сокета
    servaddr.sin_family = AF_INET; // семейство адресов IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // автоматический выбор IP-адреса сервера
    servaddr.sin_port = htons(PORT); // порт сервера
    memset(servaddr.sin_zero, '\0', sizeof servaddr.sin_zero);

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr) == -1) {
        perror("bind");
        exit(1);
    }

    // Выводим информацию о сервере на экран и в файл журнала
    printf("Сервер слушает на %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    write_log("Сервер слушает на %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

    // Устанавливаем обработчики сигналов SIGINT, SIGTERM и SIGSEGV
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);
    // Входим в бесконечный цикл обработки запросов от клиентов
    while (1) {
        // Устанавливаем таймер неактивности клиентской стороны
        set_timer();
        // Принимаем данные от клиента и запоминаем его адрес в cliaddr
        len = sizeof(cliaddr); // длина адреса клиента
        numbytes = recvfrom(sockfd, buffer, MAXBUF, 0, (struct sockaddr *) &cliaddr, &len);
        // Проверяем на ошибки
        if (numbytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        buffer[numbytes] = '\0'; // добавляем нулевой символ в конец сообщения
        // Выводим информацию о клиенте и его запросе на экран и в файл журнала
        printf("Получен запрос от %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        write_log("Получен запрос от %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        printf("Пакет длиной %d байтов\n", numbytes);
        write_log("Пакет длиной %d байтов\n", numbytes);
        printf("Пакет содержит \"%s\"\n", buffer);
        write_log("Пакет содержит \"%s\"\n", buffer);
        // Если включен флаг verbose, то выводим дополнительную информацию о запросе
        if (verbose) {
            printf("Дополнительная информация:\n");
            write_log("Дополнительная информация:\n");
            // TODO: добавить функцию для вывода дополнительной информации о запросе
        }
        double a, b, c, d;
        int n;
        n = sscanf(buffer, "%lf %lf %lf %lf", &a, &b, &c, &d);
        if (n == 3) { // квадратное уравнение
            SolveQuadratic(a, b, c); // решаем квадратное уравнение и выводим результаты
        } else if (n == 4) { // кубическое уравнение
            SolveCubic(a, b, c, d); // решаем кубическое уравнение и выводим разложение на множители
        } else { // неверный формат запроса
            printf("Неверный формат запроса.\n");
            write_log("Неверный формат запроса.\n");
        }
    }
    return 0;
}