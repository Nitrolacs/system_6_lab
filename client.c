/*! Функция клиента */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client.h"
#include "interface.h"

#define PORT 5555
#define MAXDATASIZE 1024

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

    // Создаем сокет с протоколом UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

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
    sendto(sockfd, buffer, strlen(buffer), 0,
           (struct sockaddr *) &servAddr, sizeof(servAddr));

    printf("Отправлен запрос: %s\n",
           buffer); // выводим отправленные данные

    // Закрываем сокет
    close(sockfd);

    return 0;
}