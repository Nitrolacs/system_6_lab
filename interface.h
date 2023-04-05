/*!
 * \file interface.h
 * \brief Заголовочный файл с описанием функций
 *
 * Данный файл содержит в себе определение основных
 * функций, используемых для обработки параметров командной строки (CLI).
*/

#ifndef INC_5_LAB_INTERFACE_H
#define INC_5_LAB_INTERFACE_H

/*!
 * \brief Разбирает аргументы командной строки
 * \param[in] argc Количество аргументов командной строки
 * \param[in] argv Массив указателей на строки, содержащие аргументы
 * \param[in] logFile Указатель на название log файла
 * \param[in] logFile Указатель на время ожидания
 * \param[in] a Указатель на первый коэффициент
 * \param[in] b Указатель на второй коэффициент
 * \param[in] c Указатель на третий коэффициент
 * \param[in] d Указатель на четвёртый коэффициент
 * \return Код ошибки
 */
int
ParseArgsClient(int argc, char* argv[], char** logFile, int* timeout, double* a,
          double* b, double* c,
          double* d);

/*!
 * \brief Разбирает аргументы командной строки
 * \param[in] argc Количество аргументов командной строки
 * \param[in] argv Массив указателей на строки, содержащие аргументы
 * \param[in] logFile Указатель на название log файла
 * \param[in] logFile Указатель на время ожидания
 */
void parseArgsServer(int argc, char* argv[], char** logFile, int* timeout);

#endif //INC_5_LAB_INTERFACE_H
