/* 
 * $Id: tlv_msg.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        tlv_msg.c
 * Purpose:     TLV Message Utility
 *
 */

#include <sal/core/libc.h>
#include <bcm/error.h>
#include <appl/cputrans/tlv_msg.h>

#define UINT8_SIZE     (sizeof(uint8))
#define UINT16_SIZE    (sizeof(uint16))
#define UINT32_SIZE    (sizeof(uint32))

#define TLV_TYPE_SIZE           UINT8_SIZE
#define TLV_LENGTH_SIZE         UINT16_SIZE
#define TLV_HEADER_SIZE        (TLV_TYPE_SIZE + TLV_LENGTH_SIZE)


#define TLV_END_SIZE            TLV_TYPE_SIZE

#define PACK_UINT8(buffer, value)  (*(buffer) = (value))

#define PACK_UINT16(buffer, value)                  \
    do {                                            \
        uint16 _raw;                                \
        _raw = bcm_htons(value);                    \
        sal_memcpy((buffer), &_raw, UINT16_SIZE);   \
    } while (0)

#define PACK_UINT32(buffer, value)                  \
    do {                                            \
        uint32 _raw;                                \
        _raw = bcm_htonl(value);                    \
        sal_memcpy((buffer), &_raw, UINT32_SIZE);   \
    } while (0)

#define UNPACK_UINT8(buffer, value)  ((value) = *(buffer))

#define UNPACK_UINT16(buffer, value)                \
    do {                                            \
        uint16 _raw;                                \
        sal_memcpy(&_raw, (buffer), UINT16_SIZE);   \
        (value) = bcm_ntohs(_raw);                  \
    } while (0)

#define UNPACK_UINT32(buffer, value)                \
    do {                                            \
        uint32 _raw;                                \
        sal_memcpy(&_raw, (buffer), UINT32_SIZE);   \
        (value) = bcm_ntohl(_raw);                  \
    } while (0)


#define TLV_MSG_LENGTH_CHECK(msg, size)          \
    if ((msg->buffer_end - msg->end) < (size)) { return BCM_E_MEMORY; }

#define TLV_MSG_END_CHECK(msg)                   \
    if (msg->cur_ptr >= msg->buffer_end) { return BCM_E_NOT_FOUND; }

#define PARAM_NULL_CHECK(arg)  \
    if ((arg) == NULL) { return BCM_E_PARAM; }

/*
 * Function:
 *     _tlv_msg_pack_uint8
 *     _tlv_msg_pack_uint16
 *     _tlv_msg_pack_uint32
 * Purpose:
 *     Packs given value into message object in network byte order,
 *     and updates 'cur_ptr' current pointer for writing.
 * Parameters:
 *     msg   - (IN/OUT) Message object to write to
 *     value - Value to pack
 * Returns:
 *     None
 */
STATIC void
_tlv_msg_pack_uint8(tlv_msg_t *msg, uint8 value)
{
    PACK_UINT8(msg->cur_ptr, value);
    msg->cur_ptr += UINT8_SIZE;
}

STATIC void
_tlv_msg_pack_uint16(tlv_msg_t *msg, uint16 value)
{
    PACK_UINT16(msg->cur_ptr, value);
    msg->cur_ptr += UINT16_SIZE;
}

STATIC void
_tlv_msg_pack_uint32(tlv_msg_t *msg, uint32 value)
{
    PACK_UINT32(msg->cur_ptr, value);
    msg->cur_ptr += UINT32_SIZE;
}

/*
 * Function:
 *     _tlv_msg_unpack_uint8
 *     _tlv_msg_unpack_uint16
 *     _tlv_msg_unpack_uint32
 * Purpose:
 *     Unpacks value from given message object and
 *     updates 'cur_ptr' current pointer for reading.
 * Parameters:
 *     msg   - (IN/OUT) Message object pointer
 *     value - (OUT) Returns unpacked value from message
 * Returns:
 *     None
 */
STATIC void
_tlv_msg_unpack_uint8(tlv_msg_t *msg, uint8 *value)
{
    UNPACK_UINT8(msg->cur_ptr, *value);
    msg->cur_ptr += UINT8_SIZE;
}

STATIC void
_tlv_msg_unpack_uint16(tlv_msg_t *msg, uint16 *value)
{
    UNPACK_UINT16(msg->cur_ptr, *value);
    msg->cur_ptr += UINT16_SIZE;
}

STATIC void
_tlv_msg_unpack_uint32(tlv_msg_t *msg, uint32 *value)
{
    UNPACK_UINT32(msg->cur_ptr, *value);
    msg->cur_ptr += UINT32_SIZE;
}

/*
 * Function:
 *     _tlv_msg_end
 * Purpose:
 *     Marks the end of message by setting the End-TLV.
 * Parameters:
 *     msg - (IN/OUT) TLV message object to set End-TLV
 * Returns:
 *     BCM_E_NONE
 * Notes:
 *     Assumes caller has checked for available space in message buffer.
 */
STATIC int
_tlv_msg_end(tlv_msg_t *msg)
{
    PACK_UINT8(msg->cur_ptr, TLV_TYPE_END);
    msg->end = msg->cur_ptr + TLV_END_SIZE;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tlv_msg_value_add
 * Purpose:
 *     Adds a value for given message object.
 *     Only one of the value types will be added, indicated by
 *     first non-null pointer type.
 * Parameters:
 *     msg        - (IN/OUT) Message object where to add value
 *     value8     - Value of type uint8 to add
 *     value16    - Value of type uint16 to add
 *     value32    - Value of type uint32 to add
 *     value_size - Size of value
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure
 */
STATIC int
_tlv_msg_value_add(tlv_msg_t *msg, uint8 *value8, uint16 *value16,
                   uint32 *value32, int value_size)
{
    int  req_size;

    PARAM_NULL_CHECK(msg);

    /* Check available space */
    req_size = value_size;
    /* Add End-TLV size (only once) */
    if (msg->start == msg->end) {
        req_size += TLV_END_SIZE;
    }
    TLV_MSG_LENGTH_CHECK(msg, req_size);

    /* Write value */
    if (value8 != NULL) {
        _tlv_msg_pack_uint8(msg, *value8);
    } else if (value16 != NULL) {
        _tlv_msg_pack_uint16(msg, *value16);
    } else if (value32 != NULL) {
        _tlv_msg_pack_uint32(msg, *value32);
    } else {
        return BCM_E_NONE;
    }

    /* Update TLV length */
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length += value_size;
        PACK_UINT16(msg->tlv_length_ptr, msg->tlv_length);
    }

    /* End-TLV */
    _tlv_msg_end(msg);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tlv_msg_value_get
 * Purpose:
 *     Gets value from given message object.
 *     Only one of the value types will be read, indicated by
 *     first non-null pointer type.
 * Parameters:
 *     msg        - (IN/OUT) Message object where to read value from
 *     value8     - (OUT) Value of type uint8
 *     value16    - (OUT) Value of type uint16
 *     value32    - (OUT) Value of type uint32 
 *     value_size - Size of value to be read
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - No more data to read
 *     BCM_E_FAIL      - Size of data left to read is smaller than expected
 *     BCM_E_XXX       - Failure
 */
STATIC int
_tlv_msg_value_get(tlv_msg_t *msg, uint8 *value8, uint16 *value16,
                   uint32 *value32, int value_size)
{
    int  length_left;

    PARAM_NULL_CHECK(msg);
    TLV_MSG_END_CHECK(msg);

    /* If currently reading TLV, get TLV length; else, get message length */
    if (msg->tlv_length_ptr != NULL) {
        length_left = msg->tlv_left;
    } else {
        length_left = msg->buffer_end - msg->cur_ptr;
    }

    /* Check for remaining data length */
    if (length_left <= 0) {
        return BCM_E_NOT_FOUND;
    }

    if (length_left < value_size) {
        return BCM_E_FAIL;
    }

    /* TLV value */
    if (value8 != NULL) {
        _tlv_msg_unpack_uint8(msg, value8);
    } else if (value16 != NULL) {
        _tlv_msg_unpack_uint16(msg, value16);
    } else if (value32 != NULL) {
        _tlv_msg_unpack_uint32(msg, value32);
    } else {
        return BCM_E_NONE;
    }

    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_left -= value_size;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_t_init
 * Purpose:
 *     Initializes a TLV message structure.
 * Parameters:
 *     msg    - (IN/OUT) TLV message structure to initialize
 * Returns:
 *     None
 */
void
tlv_msg_t_init(tlv_msg_t *msg)
{
    if (msg != NULL) {
        sal_memset(msg, 0, sizeof(*msg));
    }

    return;
}

/*
 * Function:
 *     tlv_msg_buffer_set
 * Purpose:
 *     Sets the buffer for given TLV message object with given buffer.
 *     This routine must be called prior to setting or getting
 *     a TLV message.
 * Parameters:
 *     msg    - (IN/OUT) TLV message object
 *     buffer - (IN/OUT) Buffer where to write or read message
 *     length - Buffer size
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null TLV message pointer
 */
int
tlv_msg_buffer_set(tlv_msg_t *msg, uint8 *buffer, int length)
{
    PARAM_NULL_CHECK(msg);

    if (buffer == NULL) {
        length = 0;
    }

    /* Set pointers to buffer */
    msg->start          = msg->end = msg->cur_ptr = buffer;
    msg->buffer_end     = buffer + length;
    msg->tlv_length_ptr = NULL;
    msg->tlv_length     = 0;
    msg->tlv_left       = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_add
 * Purpose:
 *     Adds a new TLV to current message object.
 * Parameters:
 *     msg  - (IN/OUT) Message object where to add TLV to
 *     type - TLV type
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure, other
 */
int
tlv_msg_add(tlv_msg_t *msg, tlv_msg_type_t type)
{
    int  req_size;

    PARAM_NULL_CHECK(msg);

    /* Check available space */
    req_size = TLV_HEADER_SIZE;
    /* Add End-TLV size if needed */
    if (msg->start == msg->end) {
        req_size += TLV_END_SIZE;
    }
    TLV_MSG_LENGTH_CHECK(msg, req_size);

    /* Set TLV type and length */
    _tlv_msg_pack_uint8(msg, type);
    msg->tlv_length_ptr = msg->cur_ptr;
    msg->tlv_length = 0;
    _tlv_msg_pack_uint16(msg, msg->tlv_length);

    /* End-TLV */
    _tlv_msg_end(msg);

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_get
 * Purpose:
 *     Gets next TLV type and length from given message object.
 * Parameters:
 *     msg    - (IN/OUT) Message object where to get TLV
 *     type   - (OUT) Returns TLV field type
 *     length - (OUT) If non-null, returns TLV length
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - End of message, no more TLV elements
 *     BCM_E_XXX       - Failure
 */
int
tlv_msg_get(tlv_msg_t *msg, tlv_msg_type_t *type, tlv_msg_length_t *length)
{
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(type);

    TLV_MSG_END_CHECK(msg);

    /* This skips to next TLV if in the middle of reading a TLV */
    msg->cur_ptr += msg->tlv_left;

    msg->tlv_length_ptr = NULL;
    msg->tlv_length = 0;
    msg->tlv_left = 0;

    /* TLV type */
    _tlv_msg_unpack_uint8(msg, type);
    if (*type == TLV_TYPE_END) {
        if (length != NULL) {
            *length = 0;
        }
        return BCM_E_NOT_FOUND;
    }

    /* TLV length */
    msg->tlv_length_ptr = msg->cur_ptr;
    _tlv_msg_unpack_uint16(msg, &msg->tlv_length);
    msg->tlv_left = msg->tlv_length;
    if (length != NULL) {
        *length = msg->tlv_length;
    }

    return BCM_E_NONE;
}

/*
 * Value adders
 *
 * Adds values into a TLV message.  If there are multiple values associated
 * with a TLV message header or a TLV type, the routine of the correct
 * type must be called for each one.
 */
/*
 * Function:
 *     tlv_msg_uint8_add
 *     tlv_msg_uint16_add
 *     tlv_msg_uint32_add
 * Purpose:
 *     Adds a value for given message object.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to add value
 *     value - Value to add
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure other
 */
int
tlv_msg_uint8_add(tlv_msg_t *msg, uint8 value)
{
    return(_tlv_msg_value_add(msg, &value, NULL, NULL, UINT8_SIZE));
}

int
tlv_msg_uint16_add(tlv_msg_t *msg, uint16 value)
{
    return(_tlv_msg_value_add(msg, NULL, &value, NULL, UINT16_SIZE));
}

int
tlv_msg_uint32_add(tlv_msg_t *msg, uint32 value)
{
    return(_tlv_msg_value_add(msg, NULL, NULL, &value, UINT32_SIZE));
}

/*
 * Function:
 *     tlv_msg_string_add
 * Purpose:
 *     Adds a string value for given message object,
 *     including the null-terminator.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to add string
 *     value - String value to add
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - Failure
 */
int
tlv_msg_string_add(tlv_msg_t *msg, const char *value)
{
    int  value_size;   /* Size of value */
    int  req_size;     /* Total size required */
    
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(value);

    /* Check available space */
    value_size = sal_strlen(value) + 1;
    req_size = value_size;
    /* Add End-TLV size if needed */
    if (msg->start == msg->end) {
        req_size += TLV_END_SIZE;
    }
    TLV_MSG_LENGTH_CHECK(msg, req_size);

    /* Write value */
    while((*msg->cur_ptr++ = *value++) != '\0');

    /* Update TLV length */
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length += value_size;
        PACK_UINT16(msg->tlv_length_ptr, msg->tlv_length);
    }

    /* End-TLV */
    _tlv_msg_end(msg);

    return BCM_E_NONE;
}

/*
 * Value getters
 *
 * Gets values from a TLV message.  If there are multiple values
 * associated with a TLV message header or a TLV type, the
 * getter must be called in the same order and use the same type
 * as the corresponding adder.
 */
/*
 * Function:
 *     tlv_msg_uint8_get
 *     tlv_msg_uint16_get
 *     tlv_msg_uint32_get
 * Purpose:
 *     Gets a value from given message object.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to get value
 *     value - (OUT) Value
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - No more data to read
 *     BCM_E_FAIL      - Size of data left to read is smaller than expected
 *     BCM_E_XXX       - Failure
 */
int
tlv_msg_uint8_get(tlv_msg_t *msg, uint8 *value)
{
    PARAM_NULL_CHECK(value);

    return (_tlv_msg_value_get(msg, value, NULL, NULL, UINT8_SIZE));
}

int
tlv_msg_uint16_get(tlv_msg_t *msg, uint16 *value)
{
    PARAM_NULL_CHECK(value);

    return (_tlv_msg_value_get(msg, NULL, value, NULL, UINT16_SIZE));
}

int
tlv_msg_uint32_get(tlv_msg_t *msg, uint32 *value)
{
    PARAM_NULL_CHECK(value);

    return (_tlv_msg_value_get(msg, NULL, NULL, value, UINT32_SIZE));
}

/*
 * Function:
 *     tlv_msg_string_get
 * Purpose:
 *     Gets a string value from given message object.
 * Parameters:
 *     msg       - (IN/OUT) Message object where to get string from
 *     value_max - Size of array
 *     value     - (OUT) String value
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - Failure
 */
int
tlv_msg_string_get(tlv_msg_t *msg, int value_max, char *value)
{
    int  length_left;
    int  value_size;

    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(value);

    TLV_MSG_END_CHECK(msg);

    /* If currently reading TLV get TLV length; else, get message length */
    if (msg->tlv_length_ptr != NULL) {
        length_left = msg->tlv_left;
    } else {
        length_left = msg->buffer_end - msg->cur_ptr;
    }

    if (length_left <= 0) {
        return BCM_E_NOT_FOUND;
    }

    value_size = sal_strlen((const char*) msg->cur_ptr) + 1;
    if (value_size > length_left) {
        return BCM_E_FAIL;  /* Bad string value */
    }

    if (value_size > value_max) {
        return BCM_E_MEMORY;
    }

    /* Get string value */
    while((*value++ = *msg->cur_ptr++) != '\0');

    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_left -= value_size;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_length
 * Purpose:
 *     Gets the TLV message length, including the End-TLV marker.
 *     Only valid when setting TLV message.
 * Parameters:
 *     msg    - Message object
 *     length - (OUT) Message length
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - Null pointer
 *     BCM_E_XXX   - Failure
 */
int
tlv_msg_length(tlv_msg_t *msg, int *length)
{
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(length);

    *length = msg->end - msg->start;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_resize
 * Purpose:
 *     Copies contents of the old message buffer into the
 *     new buffer and updates message object to use new buffer.
 *     It is an error if the new buffer is smaller than what the message
 *     currently requires.
 *
 *     The caller is responsible for releasing the previous buffer
 *     back to whatever allocator was used.
 * Parameters:
 *     msg    - (IN/OUT) Message object to resize
 *     buffer - (OUT) New buffer to be used by message
 *     length - Buffer size
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - New buffer size is too small
 *     BCM_E_XXX    - Failure
 */
int
tlv_msg_resize(tlv_msg_t *msg, uint8 *buffer, int length)
{
    int    msg_size;
    uint8  *buffer_old;

    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(buffer);

    /* Check that new buffer is not smaller than message current length */
    BCM_IF_ERROR_RETURN(tlv_msg_length(msg, &msg_size));
    if (length < msg_size) {
        return BCM_E_MEMORY;
    }

    /* Copy buffer data */
    buffer_old = msg->start;
    sal_memcpy(buffer, buffer_old, msg_size);

    /* Update buffer information */
    msg->start       = buffer;
    msg->buffer_end  = buffer + length;
    msg->end         = buffer + (msg->end - buffer_old);
    msg->cur_ptr     = buffer + (msg->cur_ptr - buffer_old);
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length_ptr = buffer + (msg->tlv_length_ptr - buffer_old);
    }

    return BCM_E_NONE;
}

