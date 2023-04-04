/*!
 * \file logic.h
 * \brief Заголовочный файл с описанием функций
 *
 * Данный файл содержит в себе определение основных
 * функций, используемых для работы с квадратными и кубическими уравнениями.
*/

#ifndef INC_5_LAB_LOGIC_H
#define INC_5_LAB_LOGIC_H

/*!
 * \brief Решает квадратное уравнение и раскладывает на множители
 * \param[in] argc Количество аргументов командной строки
 * \param[in] argv Массив указателей на строки, содержащие аргументы
 * \param[in] a Первый коэффициент
 * \param[in] b Второй коэффициент
 * \param[in] c Третий коэффициент
 */
void SolveQuadratic(double a, double b, double c);

/*!
 * \brief Решает кубическое уравнение и раскладывает на множители
 * \param[in] a Первый коэффициент
 * \param[in] b Второй коэффициент
 * \param[in] c Третий коэффициент
 * \param[in] d Четвёртый коэффициент
 */
void SolveCubic(double a, double b, double c, double d);

#endif //INC_5_LAB_LOGIC_H
