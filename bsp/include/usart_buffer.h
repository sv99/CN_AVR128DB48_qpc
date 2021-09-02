/*
 * usart_buffer.h
 *
 * TxBuffer and RxBuffer
 *
 *  Created: 04.07.2021
 * Modified: 04.07.2021
 *   Author: sv99@inbox.ru
 */

#ifndef USART_BUFFER_H
#define USART_BUFFER_H

/**
 * @description
 * Special string buffers for usart driver.
 */

/*! The data type to store the ring-buffer counters based on
 * the macro #QF_EQUEUE_CTR_SIZE. */
/**
 * @description
 * The dynamic range of this data type determines the maximum length
 * of the ring buffer.
 */
typedef uint8_t UsartBufferCtr;

/****************************************************************************/
/*! TxBuffer for Usart driver */
/**
 * @description
 * Simple stack with push and pull operations.
 */
typedef struct TxBuffer {

    /*! pointer to the start of the ring buffer. */
    uint8_t* ring;

    /*! the ring buffer length */
    UsartBufferCtr len;

    /*! offset to where next event will be inserted into the buffer. */
    UsartBufferCtr volatile tail;

    /*! offset of where next event will be extracted from the buffer. */
    UsartBufferCtr volatile head;

    /*! count symbols in the buffer. */
    UsartBufferCtr volatile count;

} TxBuffer;

/* public TxBuffer class operations */

void TxBuffer_init(TxBuffer* const me, uint8_t* qRing, UsartBufferCtr const qLen);
void TxBuffer_push(TxBuffer* const me, uint8_t data);
uint8_t TxBuffer_pull(TxBuffer* const me);
bool TxBuffer_is_full(TxBuffer* const me);
bool TxBuffer_is_empty(TxBuffer* const me);

/****************************************************************************/
/*! RxBuffer for Usart driver */
/**
 * @description
 * Double buffer for receiving data from the usart.
 * Short ring buffer used for delay overwrite output buffer.
 */
typedef struct RxBuffer {

    /*! pointer to the start of the input buffer. */
    uint8_t* in;

    /*! pointer to the start of the output buffer. */
    uint8_t* out;

    /*! the buffer length */
    UsartBufferCtr len;

    /*! offset to where next char will be inserted into the buffer. */
    UsartBufferCtr volatile in_tail;

    /*! offset to where next char will be inserted into the buffer. */
    UsartBufferCtr volatile out_tail;

} RxBuffer;

/* public RxBuffer class operations */

void RxBuffer_init(RxBuffer* const me, uint8_t* qIn, uint8_t* qOut, UsartBufferCtr const qLen);
void RxBuffer_write(RxBuffer* const me, uint8_t data);

/*! Flush ring buffer for next string, if need write from ring to the output. */
void RxBuffer_flush(RxBuffer* const me);

#endif /* USART_BUFFER_H */
