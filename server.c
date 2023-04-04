/*! Функция сервера */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "server.h"
#include "logic.h"

#define PORT 5555
#define MAXBUF 1024

// Главная функция сервера
int main()
{
    int numbytes; // количество байтов, полученных или отправленных
    int sockfd; // дескриптор сокета
    struct sockaddr_in servaddr, cliaddr; // структуры адресов сервера и клиента
    char buffer[MAXBUF]; // буфер для приема и отправки данных
    socklen_t len; // длина адреса клиента

    // Создаем сокет с протоколом UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // Заполняем структуру адреса сервера
    servaddr.sin_family = AF_INET; // семейство адресов IPv4
    servaddr.sin_addr.s_addr = inet_addr(
            "127.0.0.1"); // автоматический выбор IP-адреса сервера
    servaddr.sin_port = htons(PORT); // порт сервера в сетевом порядке байтов
    memset(servaddr.sin_zero, '\0',
           sizeof servaddr.sin_zero); // обнуляем оставшуюся часть структуры

    // связываем сокет с адресом сервера
    if (bind(sockfd, (struct sockaddr *) &servaddr,
            sizeof servaddr) == -1)
    {
        perror("bind");
        exit(1);
    }

    printf("Сервер слушает на %s:%d\n", inet_ntoa(servaddr.sin_addr),
           ntohs(servaddr.sin_port));

    // бесконечный цикл обработки запросов клиентов
    while (1)
    {
        // Принимаем данные от клиента
        len = sizeof(cliaddr); // длина адреса клиента

        // получаем сообщение от клиента в буфер buffer и запоминаем его адрес в cliaddr
        if ((numbytes = recvfrom(sockfd, buffer, MAXBUF, 0,
                                 (struct sockaddr *) &cliaddr,
                                 &len)) == -1)
        {
            perror("recvfrom");
            exit(1);
        }

        buffer[numbytes] = '\0'; // добавляем нулевой символ в конец сообщения

        printf("Получен запрос от %s:%d\n",
               inet_ntoa(cliaddr.sin_addr),
               ntohs(cliaddr.sin_port)); // выводим адрес клиента
        printf("Пакет длиной %d байтов\n", numbytes);
        printf("Пакет содержит \"%s\"\n", buffer);


        double a, b, c, d; // коэффициенты уравнения
        int n; // количество коэффициентов уравнения

        n = sscanf(buffer, "%lf %lf %lf %lf", &a, &b, &c,
                   &d); // читаем коэффициенты из сообщения

        if (n == 3)
        {
            // квадратное уравнение
            SolveQuadratic(a, b,
                            c); // решаем квадратное уравнение и выводим разложение на множители
        }
        else if (n == 4)
        {
            // кубическое уравнение
            SolveCubic(a, b, c,
                        d); // решаем кубическое уравнение и выводим разложение на множители
        }
        else
        {
            printf("Неверный формат запроса.\n"); // некорректное количество коэффициентов
        }
    }

    close(sockfd); // закрываем сокет

    return 0;
}