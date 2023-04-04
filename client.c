/*! Функция клиента */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client.h"
#include "interface.h"
#include "signals.h"

#define PORT 5555
#define MAXDATASIZE 1024

int main(int argc, char *argv[]) {
    int sockfd; // дескриптор сокета
    char buffer[MAXDATASIZE]; // буфер для приема и отправки данных
    struct sockaddr_in servAddr; // структура адреса сервера

    // Объявляем переменные для коэффициентов уравнения
    double a = 0;
    double b = 0;
    double c = 0;
    double d = 0; // Добавляем переменную для четвертого коэффициента

    // Устанавливаем обработчики сигналов SIGINT, SIGTERM и SIGSEGV
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);

    // Объявляем и инициализируем переменные для хранения аргументов командной строки
    char *log_file = NULL; // имя файла журнала
    int timeout = 0; // время ожидания ввода пользователя в секундах

    // Вызываем функцию для обработки коэффициентов уравнения из командной строки
    int result = ParseArgsClient(argc, argv, &log_file, &timeout, &a, &b, &c,
                                 &d);

    // Проверяем результат функции
    if (result != 0) {
        fprintf(stderr, "Ошибка при обработке коэффициентов.\n");
        exit(1);
    }

    // Объявляем и инициализируем переменную для хранения дескриптора файла журнала
    FILE *logfd = NULL;

    char* logFileName = "client.log";

    // Открываем файл журнала
    openLog(&log_file, &logfd, logFileName);

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
    if (d == 0) {
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
    writeLog(logfd, "Отправлен запрос: %s\n", buffer);

    // Устанавливаем таймер неактивности пользователя
    setTimer(timeout);

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
    writeLog(logfd, "Получен ответ: %s\n", buffer);

    // Закрываем сокет и файл журнала
    close(sockfd);
    fclose(logfd);

    return 0;
}
