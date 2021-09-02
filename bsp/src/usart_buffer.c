/*
 * usart_buffer.c
 *
 * TxBuffer and RxBuffer implementation
 *
 * Created: 04.07.2021
 *
 * Modified: 04.07.2021 
 * Author: sv99@inbox.ru
 */

#include "qpc.h" /* QPC framework API */
#include "usart_buffer.h"
#include "logger.h"

// Q_DEFINE_THIS_FILE

/****************************************************************************/
/* TxBuffer implementation                                                  */
/****************************************************************************/
/**
 * @description
 * Initialize the ring buffer by giving it the storage for the data.
 *
 * @param[in,out] me    pointer (see @ref oop)
 * @param[in]     qRing an array to serve as the ring buffer for the data
 * @param[in]     qLen  the length of the @p qRing buffer
 *
 */
void TxBuffer_init(TxBuffer* const me, uint8_t* qRing, UsartBufferCtr const qLen)
{
    me->ring = qRing; /* the beginning of the ring buffer */
    me->len = qLen;
    me->head = 0U;
    me->tail = 0U;
    me->count = 0U;
}

/****************************************************************************/
/**
 * @description
 * Push a data to the buffer.
 * Don't check space in the buffer for speed. Need check empty space before!
 *
 * @param[in,out] me     pointer (see @ref oop)
 * @param[in]     data   symbol data
 */
void TxBuffer_push(TxBuffer* const me, uint8_t data)
{
    /* insert data */
    me->ring[me->tail] = data;
    ++me->tail;
    /* need to wrap the head? */
    if (me->tail == me->len) {
        me->tail = 0U; /* wrap around */
    }
    ++me->count;
}

/****************************************************************************/
/**
 * @description
 * Retrieves a data from the ring buffer.
 *
 * @param[in,out] me     pointer (see @ref oop)
 *
 */
uint8_t TxBuffer_pull(TxBuffer* const me)
{
    uint8_t data;

    if (me->count == 0U) {
        /* empty */
        data = 0U;
    } else {
        /* get from head */
        data = me->ring[me->head];
        ++me->head;
        if (me->head == me->len) { /* need to wrap the head? */
            me->head = 0U;         /* wrap around */
        }
        --me->count;
    }
    return data;
}

/****************************************************************************/
/**
 * @description
 * True if buffer is full.
 *
 * @param[in,out] me     pointer (see @ref oop)
 *
 */
bool TxBuffer_is_full(TxBuffer* const me)
{
    return me->count == me->len;
}

/****************************************************************************/
/**
 * @description
 * True if buffer is empty.
 *
 * @param[in,out] me     pointer (see @ref oop)
 *
 */
bool TxBuffer_is_empty(TxBuffer* const me)
{
    return me->count == 0U;
}

/****************************************************************************/
/* RxBuffer implementation                                                  */
/****************************************************************************/
/**
 * @description
 * Initialize the ring buffer by giving it the storage for the data.
 *
 * @param[in,out] me   pointer (see @ref oop)
 * @param[in]     qIn  an array to serve as the input buffer for the data
 * @param[in]     qOut an array to serve as the output buffer
 * @param[in]     qLen the length of the @p qOut buffer
 *
 */
void RxBuffer_init(RxBuffer* const me, uint8_t* qIn, uint8_t* qOut, UsartBufferCtr const qLen)
{
    memset(me->in, 0U, me->len);
    memset(me->out, 0U, me->len);
    me->in = qIn;   /* the beginning of the ring buffer */
    me->out = qOut; /* the beginning of the ring buffer */
    me->len = qLen;
    me->in_tail = 0U;
    me->out_tail = 0U;
}

/****************************************************************************/
/**
 * @description
 * Push a data to the ring buffer.
 *
 * @param[in,out] me     pointer (see @ref oop)
 * @param[in]     data   symbol data
 */
void RxBuffer_write(RxBuffer* const me, uint8_t data)
{
    /* store data to the input buffer            */
    /* if input buffer is full then data is lost */

    if (me->in_tail == me->len) {
        /* save 0U to the end output buffer */
        me->in[me->len - 1] = 0U;
    } else {
        me->in[me->in_tail] = data;
        ++me->in_tail;
    }
}

/****************************************************************************/
/*! Flush data from the ring buffer to the output buffer and set valid flag */
void RxBuffer_flush(RxBuffer* const me)
{
    /* copy data from the input to the output buffer */
    memmove(me->out, me->in, me->len);
    memset(me->in, 0U, me->len);
    me->out_tail = me->in_tail;
    me->in_tail = 0;
}
