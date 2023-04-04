/*! Функция сервера */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "logic.h"
#include "interface.h"
#include "signals.h"

#define PORT 5555
#define MAXBUF 1024

// Главная функция сервера
int main(int argc, char *argv[]) {
    int numbytes;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXBUF];
    socklen_t len;

    char* log_file = NULL; // имя файла журнала
    int timeout = 0; // время ожидания сообщений от клиента в секундах

    // Разбираем аргументы командной строки
    parseArgsServer(argc, argv, &log_file, &timeout);

    int sockfd = -1; // дескриптор сокета
    FILE *logfd = NULL; // дескриптор файла журнала

    char* logFileName = "server.log";

    // Открываем файл журнала
    openLog(&log_file, &logfd, logFileName);

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
    writeLog(logfd, "Сервер слушает на %s:%d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

    // Устанавливаем обработчики сигналов SIGINT, SIGTERM и SIGSEGV
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);
    // Входим в бесконечный цикл обработки запросов от клиентов
    while (1) {
        // Устанавливаем таймер неактивности клиентской стороны
        setTimer(timeout);
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
        writeLog(logfd,"Получен запрос от %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        printf("Пакет длиной %d байтов\n", numbytes);
        writeLog(logfd,"Пакет длиной %d байтов\n", numbytes);
        printf("Пакет содержит \"%s\"\n", buffer);
        writeLog(logfd,"Пакет содержит \"%s\"\n", buffer);
        double a, b, c, d;
        sscanf(buffer, "%lf %lf %lf %lf", &a, &b, &c, &d);
        if (d == 0) { // квадратное уравнение
            SolveQuadratic(a, b, c); // решаем квадратное уравнение и выводим результаты
        } else if (d != 0) { // кубическое уравнение
            SolveCubic(a, b, c, d); // решаем кубическое уравнение и выводим разложение на множители
        } else { // неверный формат запроса
            printf("Неверный формат запроса.\n");
            writeLog(logfd, "Неверный формат запроса.\n");
        }
    }
}