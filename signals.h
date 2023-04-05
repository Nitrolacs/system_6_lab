/*!
 * \file signals.h
 * \brief Заголовочный файл с описанием функций
 *
 * Данный файл содержит в себе определение основных
 * функций, используемых для обработки сигналов и настройки таймера.
*/

#ifndef INC_6_LAB_SIGNALS_H
#define INC_6_LAB_SIGNALS_H

/*!
 * \brief Функция для обработки сигналов, приводящих к завершению процесса
 * \param[in] signum Номер (тип) сигнала
 */
void signalHandler(int signum);

/*!
 * \brief Функция для обработки таймера неактивности пользователя
 */
void timeoutHandler();

/*!
 * \brief Функция для открытия файла журнала для записи или создания его
 * \param[in] logFile Указатель на файл
 * \param[in] logFile Название файла
 */
void openLog(char** logFile, char* logFileName);

/*!
 * \brief  Функция для записи сообщений в файл журнала с помощью fprintf
 * \param[in] format Формат сообщения
 * \param[in] ... Остальные параметры
 */
void writeLog(const char* format, ...);

/*!
 * \brief  Функция для установки таймера неактивности пользователя
 * \param[in] timeout Время ожидания
 */
void setTimer(int timeout);

#endif //INC_6_LAB_SIGNALS_H
