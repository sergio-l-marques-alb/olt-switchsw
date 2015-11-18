/*
 * $Id: simintf.c,v 1.12 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * SBX SIM SDK interface
 */

#include <shared/bsl.h>

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/debug.h>
#include <assert.h>

#include "simsock.h"

#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h> 
#include <sal/appl/sal.h>
#include <shared/util.h>

#include <soc/sbx/caladan3/cop.h>

#define DELIMIT ' '
#define ENDOFSTRING '\n'
#define MULTIBYTE_DELIMIT ':'

#define MAX_BUFFER_SIZE 8*1024

static char send_buffer[MAX_BUFFER_SIZE];

/*
 * Function:
 *    soc_sbx_caladan3_sim_block_encode_simple
 * Purpose
 *    Routine to encode a block, table info into text for messaging
 *    returns error status
 */
int
soc_sbx_caladan3_sim_block_encode_simple(char *buffer, char *block, char *table, char *op )
{
    int size;

    if (!buffer || !block || !table || !op) {
        return SOC_E_PARAM;
    }
    size = sal_snprintf(buffer, 64, "%s%c%s%c%s", block, DELIMIT, 
                        op, DELIMIT ,table);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_sbx_caladan3_sim_block_encode_simple got: "
                          "Block %s Table: %s Op: %s \n"),
                 block, table, op));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("generated: buffer: %s \n"),
                 buffer));
    return size;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_block_encode
 * Purpose
 *    Routine to encode a block, table info into text for messaging
 *    returns error status
 */
int
soc_sbx_caladan3_sim_block_encode(char *buffer, char *block,
                                    char *table, char *op, uint32 addr)
{
    int size;

    if (!buffer || !block || !table || !op) {
        return SOC_E_PARAM;
    }
    size = sal_snprintf(buffer, 64, "%s%c%s%c%s%c%s%c0x%x ", block, DELIMIT,
                        op, DELIMIT, table, DELIMIT, "addr", DELIMIT, addr);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_sbx_caladan3_sim_block_encode got: "
                          "Block %s Table: %s Op: %s Addr: %x\n"),
                 block, table, op, addr));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("generated: buffer: %s \n"),
                 buffer));
    return size;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_keyword_encode
 * Purpose
 *    Routine to encode keyword value into message 
 */
int
soc_sbx_caladan3_sim_keyword_encode(char *buffer, char *keyword)
{
    int len;

    if (!buffer) {
        return SOC_E_PARAM;
    }
    len = sal_snprintf(buffer, 64, "%s%c", keyword, DELIMIT);

    return len;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_keyword_decode
 * Purpose
 *    Routine to check keyword is present in message at this location
 */
int
soc_sbx_caladan3_sim_keyword_decode(char *buffer, char *keyword)
{
    if (!buffer) {
        return SOC_E_PARAM;
    }
    return (strncmp(buffer, keyword, strlen(keyword)));

}


/*
 * Function:
 *    soc_sbx_caladan3_sim_verb_encode
 * Purpose
 *    Routine to encode verb value into message if verb given 
 *    or just encode the value if no verb given
 *    returns value and length of message generated
 */
int
soc_sbx_caladan3_sim_verb_encode(char *buffer, char *verb, int value)
{
    int len;

    if (!buffer) {
        return SOC_E_PARAM;
    }
    len = 0;
    if (verb) {
        len = sal_snprintf(buffer, 64, "%s%c", verb, DELIMIT);
    }
    len += sal_snprintf(buffer+len, 64, "0x%x%c", value, DELIMIT);

    return len;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_verb_decode
 * Purpose
 *    Routine to decode verb value from message if verb given 
 *    or just return the value if no verb given
 *    returns value and length of message consumed
 */
int
soc_sbx_caladan3_sim_verb_decode(char *buffer, char *verb, int *parsedlen)
{
    char *p; int len;
    char b[64];
    int value;

    if (!buffer || !parsedlen) {
        return SOC_E_PARAM;
    }
    p = strchr(buffer, DELIMIT);
    if (!p) 
        p = strchr(buffer, ENDOFSTRING);
    if (p) {
        len = p-buffer;
        memcpy(b, buffer, len);
        b[len]=0;
        if ((verb) && (sal_strcmp(b, verb)==0)) {
            /* Extract status */
            buffer += len+1;
            p = strchr(buffer, DELIMIT);
            if (p) {
                p[0] = 0; p++;
                value = _shr_ctoi(buffer);
                *parsedlen  = (len + (p-buffer) + 1);
                return value;
            }
        } else {
            value = _shr_ctoi(b);
            *parsedlen  = len + 1;
            return value;
        }
    }
    *parsedlen = 0;
    return 0;
}


/*
 * Function:
 *    soc_sbx_caladan3_sim_status_decode
 * Purpose
 *    Routine to decode a response status, from message
 *    returns error status
 */
int
soc_sbx_caladan3_sim_status_decode(char *buffer, char *verb, int *parsedlen)
{
    int istatus;

    if (!buffer || !verb  || !parsedlen) {
        return SOC_E_PARAM;
    }

    *parsedlen = 0;
    istatus = soc_sbx_caladan3_sim_verb_decode(buffer, verb, parsedlen);

    if (istatus == 0) {
        return  SOC_E_NONE;
    } else if (istatus == 2) {
        return  SOC_E_PARAM;
    } else {
        return SOC_E_FAIL;
    }
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_block_decode
 * Purpose
 *    Routine to decode a block, table info into text for messaging
 *    returns error status
 */
int
soc_sbx_caladan3_sim_block_decode(char *buffer, char *block,
                                    char *table, char *op, uint32 *addr)
{
    char *p; int len;
    if (!buffer || !block || !table || !op) {
        return SOC_E_PARAM;
    }
    p = strchr(buffer, DELIMIT);
    if (p) {
        len = p-buffer;
        sal_snprintf(block,len, buffer);
        block[len]=0;
        buffer+=len;
    }

    p = strchr(buffer, DELIMIT);
    if (p) {
        len = p-buffer;
        sal_snprintf(op,len, buffer);
        op[len]=0;
        buffer+=len;
    }

    p = strchr(buffer, DELIMIT);
    if (p) {
        len = p-buffer;
        sal_snprintf(table,len, buffer);
        table[len]=0;
        buffer+=len;
    }

    /* Skipping 'addr' */
    p = strchr(buffer, DELIMIT);
    if (p) {
        len = p-buffer;
        buffer+=len;
    }
    *addr = _shr_ctoi(buffer);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_sbx_caladan3_sim_block_decode got: "
                          "Block %s Table: %s Op: %s Addr: %x\n"),
                 block, table, op, *addr));
    return 0;
}


/*
 * Function:
 *    soc_sbx_caladan3_sim_field_encode
 * Purpose
 *    Routine to encode a packed field data into
 *      text for messaging
 *    returns error status
 */
int
soc_sbx_caladan3_sim_field_encode(char *buffer, char *field,
                                    uint8 *value, int size)
{
    char *p;
    int l, j;
    int d = 1;
    int be = (*(char*)&d) == 0;

    if (!buffer || !value || !field ) {
        return 0;
    }
    p = buffer;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("field encode: %s size %d "),
                 field, size));
    l = sal_snprintf(p, 64, "%s%c0x%x", field, DELIMIT, size);
    p += l;
    if (be) {
        for (j=0; j < size; j++)
        {
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j]);
            p += l;
        }
    } else {
        j = 0;
        while (size >= 4)
        {
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j+3]);
            p += l;
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j+2]);
            p += l;
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j+1]);
            p += l;
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j]);
            p += l;

            size -= 4;
            j += 4;
        }
        if (size > 2) {
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j+2]);
            p += l;
        }
        if (size > 1) {
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j+1]);
            p += l;
        }
        if (size > 0) {
            l = sal_snprintf(p, 64, "%c0x%x", DELIMIT, value[j]);
            p += l;
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("generated: buffer: %s\n"),
                 buffer));
    return (p-buffer);
}

/*
 * Function:
 *   soc_sbx_caladan3_sim_split
 * Purpose
 *   Break a string of data into field,value pairs
 */
int
soc_sbx_caladan3_sim_split(char *buffer, int size, char**fields, char**values)
{
    int c;
    char *p, *p1, *p2;
    char *f, *v;
    int len = 0;
    c = 0;
    p = buffer;
    do {
        f = v = NULL;
        p1 = strchr(p, DELIMIT);
        if (p1) {
            len = p1-p;
            f = sal_alloc(len+1, "C3 SIM");
            if (!f) {
                while (--c > 0) { 
                    sal_free(fields[c]); 
                    sal_free(values[c]);
                }
                return 0;
            }
            memcpy(f, p, len);
            f[len]=0;
            p1++;
            p2 = strchr(p1, DELIMIT);
            if (p2) {
                len = p2-p1;
                p2++;
            } else {
                len = (buffer+size)-p1;
            }
            v = sal_alloc(len+1, "C3 SIM");
            if (!v) {
                while (--c > 0) { 
                    sal_free(fields[c]); 
                    sal_free(values[c]);
                }
                sal_free(f);
                return 0;
            }
            memcpy(v, p1, len);
            v[len]=0;
            fields[c] = f;
            values[c] = v;
            p = p2;
            c++;
        } else {
            break;
        }
    } while (p);
    return c; 
}

/*
 * Function:
 *   soc_sbx_caladan3_sim_get_value
 * Purpose
 *   decode a value from a string given the width
 */
int
soc_sbx_caladan3_sim_get_value(char *b, uint8* val, int width)
{
    char *p, *p1;
    int j;
    p = b;
    p1 = strchr(b, MULTIBYTE_DELIMIT);
    if (p1) {
        for (j=0, p1[0]=0; p1; j++, p=p1+1, p1=strchr(b, MULTIBYTE_DELIMIT)) {
            val[j] = _shr_ctoi(p);
        }
        if ((width) && (width != j*4)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("soc_sbx_caladan3_sim_field_decode length mismatch")));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("  expected %d got %d"), width, j*4));
            return -1;
        }
    } else {
        *(uint32*)val = _shr_ctoi(b);
    }
    return 0;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_field_decode
 * Purpose
 *    Routine to decode num entries of {field value} pair from a text 
 * message 
 *    Returns error status
 */
int
soc_sbx_caladan3_sim_field_decode(char *buffer, int size, char *field,
                                     uint8 *value, int width)
{
    int i = 0;
    int parsedlen = 0;
    int sz = 0;
    int d = 1;
    int be = (*(char*)&d) == 0;

    if (!buffer || !value || !field) {
        return 0;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_sbx_caladan3_sim_field_decode got: "
                          "buffer:%s size %d\n"),
                 buffer, size));
    if (sal_strlen(buffer) < sal_strlen(field) + 4 * width) {
        /* buffer should contain atleast all zeros (\b0x0) encoded */
        return SOC_E_PARAM;
    }
    parsedlen = 0;
    sz = soc_sbx_caladan3_sim_verb_decode(buffer, field, &parsedlen);
    if (parsedlen > 0) {
        if (sz != width) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("soc_sbx_caladan3_sim_field_decode "
                                "expected %d bytes got %d bytes"),
                       width, sz));
            return SOC_E_PARAM;
        }
        if (be) {
            for (i=0; i<sz; i++) {
                buffer +=  parsedlen;
                value[i] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
            }
        } else {
            i = 0;
            while(sz >= 4) {
                buffer +=  parsedlen;
                value[i+3] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
                buffer +=  parsedlen;
                value[i+2] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
                buffer +=  parsedlen;
                value[i+1] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
                buffer +=  parsedlen;
                value[i] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
                sz-=4;
                i+=4;
            }
            if (sz > 2 ) {
                buffer +=  parsedlen;
                value[i+2] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
            }
            if (sz > 1 ) {
                buffer +=  parsedlen;
                value[i+1] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
            }
            if (sz > 0 ) {
                buffer +=  parsedlen;
                value[i] = soc_sbx_caladan3_sim_verb_decode(buffer, NULL, &parsedlen);
            }
        }
        return SOC_E_NONE;
    }
            
    return SOC_E_PARAM;
}

/*
 * Function:
 *    soc_sbx_caladan3_sim_sendrcv
 * Purpose
 *    Send a message to the sim server and get the response back
 *    No processing of the message happens here, its up to the client
 */
int
soc_sbx_caladan3_sim_sendrcv(int unit, char *buffer, int *size)
{
    int rv = SOC_E_NONE;
    char b[8192]; 
    int bsize = 0;
    if (!buffer || !size) {
        return SOC_E_PARAM;
    }
    /* forcing \n */
    buffer[*size]='\n';
    *size = *size+1;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "caladan3_sim_sendrcv Sending:\n %s"), buffer));
    rv = send_message(unit, buffer, *size);
    if (SOC_SUCCESS(rv)) {
        memset(buffer, 0, *size);
        rv = recv_message(unit, b, &bsize); 
        if (SOC_SUCCESS(rv)) {
            sal_memcpy(buffer, b, bsize);
            buffer[bsize]=0;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Received buffer:\n %s "),
                         buffer));
            *size = bsize;
        }
    }
    return rv;
}


/*
 *   Function
 *     sbx_caladan3_cop_policer_create_sim
 *   Purpose
 *      COP create a policer
 *   Parameters
 *      (IN) unit   : unit number of the device
 *      (IN) cop    : cop instance 0 or 1
 *      (IN) segment: cop segment 0-31
 *      (IN) policer: policer ID
 *      (IN) config : policer configuration parameter
 *      (OUT) handle: policer handle, should be used when delete
 *   Returns
 *       SOC_E_NONE - successfully initialized
 *       SOC_E_* as appropriate otherwise
 */
int soc_sbx_caladan3_cop_policer_create_sim(int unit,
					    uint32 cop,
					    uint32 segment,
					    uint32 policer,
					    soc_sbx_caladan3_cop_policer_config_t *config,
					    uint32 *handle)
{

    int nextPrintPos = 0; 
    int bufferSizeRemain = MAX_BUFFER_SIZE; 
    int numCharPrinted; 
    int rv, status; 
    char *token; 
    char *delimiters = " \t"; 
    int ucode_seg; 

    if(cop == 0) { 
      ucode_seg = segment + 32; /* COP0_SEG_START */ 
    } 
    else { 
      ucode_seg = segment + 64; /* COP1_SEG_START */ 
    } 

         
    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "cop set ");
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 
     
        
    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "instance 0x%x ", cop);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 
     
    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "segment 0x%x ", ucode_seg);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "id 0x%x ", policer);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "RfcMode 0x%x ", config->uRfcMode);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "cir 0x%x ", config->uCIR);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "cbs 0x%x ", config->uCBS);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "eir 0x%x ", config->uEIR);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "ebs 0x%x ", config->uEBS);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "blind 0x%x ", config->bBlindMode);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "dropOnRed 0x%x ", config->bDropOnRed);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "coupling 0x%x ", config->bCoupling);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "CBSNoDecrement 0x%x ", config->bCBSNoDecrement);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "EBSNoDecrement 0x%x ", config->bEBSNoDecrement);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "CIRStrict 0x%x ", config->bCIRStrict);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "EIRStrict 0x%x ", config->bEIRStrict);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "LenAdjust 0x%x ", config->nLenAdjust);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    numCharPrinted = sal_snprintf(&send_buffer[nextPrintPos], bufferSizeRemain, "PktMode 0x%x ", config->bPktMode);
     nextPrintPos += numCharPrinted; 
    bufferSizeRemain -= numCharPrinted; 
    if (bufferSizeRemain == 0) return SOC_E_PARAM; 

    rv = soc_sbx_caladan3_sim_sendrcv(unit, send_buffer, &nextPrintPos); 
    if (SOC_FAILURE(rv)) { 
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "sim sendrcv failed (%d)\n"), rv));
        return rv; 
    } 
     
    token = strtok(send_buffer, delimiters); 
    if ((token == NULL) || (sal_strcasecmp(token, "status"))){ 
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Recv format unrecognized: %s\n"), send_buffer));
         return SOC_E_PARAM; 
    } 
    token = strtok(NULL, delimiters); 
    if (token == NULL){ 
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Missing status value\n")));
        return SOC_E_PARAM; 
    } 
    status = _shr_ctoi(token); 
    if (status != 0) { 
        return SOC_E_NOT_FOUND; 
    } 
     
     
    return SOC_E_NONE; 


}
