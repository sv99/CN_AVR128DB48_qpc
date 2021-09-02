/*
 * logger.h
 *
 *  Created: 29.03.2021
 * Modified: 25.04.2021
 *   Author: sv99@inbox.ru
 */

#ifndef LOGGER_H_
#define LOGGER_H_

extern const Q_ROM char CRLF[];
extern const Q_ROM char LF[];

void logger_byte(const uint8_t data);
void logger_ln(void);
void logger(const char* str);
void logger_P(const Q_ROM char* str);
void logger_n(const char* str);
void logger_n_P(const Q_ROM char* str);

#endif /* LOGGER_H_ */
