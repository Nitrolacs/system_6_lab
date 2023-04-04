/*!
 * \file interface.h
 * \brief Заголовочный файл с описанием функций
 *
 * Данный файл содержит в себе определение основной
 * функции, используемой для обработки параметров командной строки (CLI).
*/

#ifndef INC_5_LAB_INTERFACE_H
#define INC_5_LAB_INTERFACE_H

/*!
 * \brief Выводит меню программы
 * \param[in] argc Количество аргументов командной строки
 * \param[in] argv Массив указателей на строки, содержащие аргументы
 * \param[in] a Указатель на первый коэффициент
 * \param[in] b Указатель на второй коэффициент
 * \param[in] c Указатель на третий коэффициент
 * \param[in] d Указатель на четвёртый коэффициент
 * \return Код ошибки
 */
int
ProcessCoefficients(int argc, char* argv[], double* a, double* b, double* c,
                    double* d);

#endif //INC_5_LAB_INTERFACE_H
