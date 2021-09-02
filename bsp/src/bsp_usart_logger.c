/*
 * logger.c
 *
 *  Created: 29.03.2021
 * Modified: 11.05.2021
 *   Author: sv99@inbox.ru
 */

#include "qpc.h"
#include "bsp.h"
#include "bsp_pins.h"
#include "usart_buffer.h"

const Q_ROM char CRLF[] = "\r\n";
const Q_ROM char LF[] = "\n";

#define LOGGER_TX_BUFFER_SIZE 128

/* the Logger TxBuffer instance */
static TxBuffer LoggerTxBuffer;
static uint8_t USART_LOGGER_txbuf[LOGGER_TX_BUFFER_SIZE];

void BSP_LOGGER_init(void)
{
    LOGGER.BAUD = (uint16_t)USART_BAUD_RATE(19200); /* set baud rate register */

    // LOGGER.CTRLA = 0 << USART_ABEIE_bp /* Auto-baud Error Interrupt Enable: disabled */
    //		 | 0 << USART_DREIE_bp /* Data Register Empty Interrupt Enable: disabled */
    //		 | 0 << USART_LBME_bp /* Loop-back Mode Enable: disabled */
    //		 | USART_RS485_DISABLE_gc /* RS485 Mode disabled */
    //		 | 0 << USART_RXCIE_bp /* Receive Complete Interrupt Enable: disabled */
    //		 | 0 << USART_RXSIE_bp /* Receiver Start Frame Interrupt Enable: disabled */
    //		 | 0 << USART_TXCIE_bp; /* Transmit Complete Interrupt Enable: disabled */

    LOGGER.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
                   | 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
                   | 0 << USART_RXEN_bp     /* Receiver Enable: disabled */
                   | USART_RXMODE_NORMAL_gc /* Normal mode */
                   | 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
                   | USART_TXEN_bm;         /* Transmitter Enable: enabled */

    // LOGGER.CTRLC = USART_CMODE_ASYNCHRONOUS_gc /* Asynchronous Mode */
    //		 | USART_CHSIZE_8BIT_gc /* Character size: 8 bit */
    //		 | USART_PMODE_DISABLED_gc /* No Parity */
    //		 | USART_SBMODE_1BIT_gc; /* 1 stop bit */

    // LOGGER.DBGCTRL = 0 << USART_DBGRUN_bp; /* Debug Run: disabled */

    // LOGGER.EVCTRL = 0 << USART_IREI_bp; /* IrDA Event Input Enable: disabled */

    // LOGGER.RXPLCTRL = 0x0 << USART_RXPL_gp; /* Receiver Pulse Length: 0x0 */

    // LOGGER.TXPLCTRL = 0x0 << USART_TXPL_gp; /* Transmit pulse length: 0x0 */

    TxBuffer_init(&LoggerTxBuffer, USART_LOGGER_txbuf, LOGGER_TX_BUFFER_SIZE);
}

void BSP_LOGGER_dre_isr(void)
{
    /* Check if all data is transmitted */
    if (!TxBuffer_is_empty(&LoggerTxBuffer)) {
        /* Start transmission */
        LOGGER.TXDATAL = TxBuffer_pull(&LoggerTxBuffer);
    }

    if (TxBuffer_is_empty(&LoggerTxBuffer)) {
        /* Disable UDRE interrupt */
        LOGGER.CTRLA &= ~USART_DREIE_bm;
    }
}

void logger_byte(uint8_t data)
{
    /* Wait for free space in buffer */
    while (TxBuffer_is_full(&LoggerTxBuffer))
        ;

    QF_INT_DISABLE();
    /* Store data in the buffer */
    TxBuffer_push(&LoggerTxBuffer, data);
    QF_INT_ENABLE();

    /* Enable UDRE interrupt */
    LOGGER.CTRLA |= USART_DREIE_bm;
}

void logger_n(const char* str)
{
#ifndef NLOGGER
    // for (size_t i = 0; i < strlen(str); i++) {
    // logger_byte(str[i]);
    //}
    while (*str != '\0') {
        logger_byte(*str);
        str++;
    }
#endif /* NLOGGER */
}

void logger_n_P(const Q_ROM char* str)
{
#ifndef NLOGGER
#if defined(__GNUC__)
    char sym;

    sym = pgm_read_byte(str);
    while (sym != '\0') {
        logger_byte(sym);
        str++;
        sym = pgm_read_byte(str);
    }
#else
    while (*str != '\0') {
        logger_byte(*str++);
    }
#endif
#endif /* NLOGGER */
}

void logger(const char* str)
{
#ifndef NLOGGER
    logger_n(str);
    logger_n_P(CRLF);
#endif /* NLOGGER */
}

void logger_P(const Q_ROM char* str)
{
#ifndef NLOGGER
    logger_n_P(str);
    logger_n_P(CRLF);
#endif /* NLOGGER */
}

void logger_ln(void)
{
#ifndef NLOGGER
    logger_n_P(CRLF);
#endif /* NLOGGER */
}
