/*  -*-  Mode:C; c-basic-offset:4 -*- */
/*
 * $Id: sbZfKaEgPortRemapEntry.hx,v 1.2 Broadcom SDK $
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
 */


#ifndef SB_ZF_ZFKAEGPORTREMAPENTRY_H
#define SB_ZF_ZFKAEGPORTREMAPENTRY_H

#define SB_ZF_ZFKAEGPORTREMAPENTRY_SIZE_IN_BYTES 2
#define SB_ZF_ZFKAEGPORTREMAPENTRY_SIZE 2
#define SB_ZF_ZFKAEGPORTREMAPENTRY_M_NRESERVED_BITS "15:9"
#define SB_ZF_ZFKAEGPORTREMAPENTRY_M_NMCFIFO_BITS "8:8"
#define SB_ZF_ZFKAEGPORTREMAPENTRY_M_NFIFOENABLE_BITS "7:7"
#define SB_ZF_ZFKAEGPORTREMAPENTRY_M_NFIFONUM_BITS "6:0"


typedef struct _sbZfKaEgPortRemapEntry {
  uint32 m_nReserved;
  uint8 m_nMcFifo;
  uint8 m_nFifoEnable;
  uint32 m_nFifoNum;
} sbZfKaEgPortRemapEntry_t;

uint32
sbZfKaEgPortRemapEntry_Pack(sbZfKaEgPortRemapEntry_t *pFrom,
                            uint8 *pToData,
                            uint32 nMaxToDataIndex);
void
sbZfKaEgPortRemapEntry_Unpack(sbZfKaEgPortRemapEntry_t *pToStruct,
                              uint8 *pFromData,
                              uint32 nMaxToDataIndex);
void
sbZfKaEgPortRemapEntry_InitInstance(sbZfKaEgPortRemapEntry_t *pFrame);

#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_KAEGPORTREMAPENTRY_SET_RESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_MCFIFO(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFOENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFONUM(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~0x7f) | ((nFromData) & 0x7f); \
          } while(0)

#else
#define SB_ZF_KAEGPORTREMAPENTRY_SET_RESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_MCFIFO(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFOENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFONUM(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x7f) | ((nFromData) & 0x7f); \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_KAEGPORTREMAPENTRY_SET_RESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_MCFIFO(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[2] = ((pToData)[2] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFOENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFONUM(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[3] = ((pToData)[3] & ~0x7f) | ((nFromData) & 0x7f); \
          } while(0)

#else
#define SB_ZF_KAEGPORTREMAPENTRY_SET_RESERVED(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~(0x7f << 1)) | (((nFromData) & 0x7f) << 1); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_MCFIFO(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[1] = ((pToData)[1] & ~0x01) | ((nFromData) & 0x01); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFOENABLE(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~(0x01 << 7)) | (((nFromData) & 0x01) << 7); \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_SET_FIFONUM(nFromData,pToData,nMaxToDataIndex) \
          do { \
           (pToData)[0] = ((pToData)[0] & ~0x7f) | ((nFromData) & 0x7f); \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_KAEGPORTREMAPENTRY_GET_RESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 1) & 0x7f; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_MCFIFO(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[2]) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFOENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[3] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFONUM(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3]) & 0x7f; \
          } while(0)

#else
#define SB_ZF_KAEGPORTREMAPENTRY_GET_RESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 1) & 0x7f; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_MCFIFO(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[1]) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFOENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[0] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFONUM(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x7f; \
          } while(0)

#endif
#ifdef SAND_BIG_ENDIAN_HOST
#define SB_ZF_KAEGPORTREMAPENTRY_GET_RESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[2] >> 1) & 0x7f; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_MCFIFO(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[2]) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFOENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[3] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFONUM(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[3]) & 0x7f; \
          } while(0)

#else
#define SB_ZF_KAEGPORTREMAPENTRY_GET_RESERVED(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[1] >> 1) & 0x7f; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_MCFIFO(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[1]) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFOENABLE(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint8) ((pFromData)[0] >> 7) & 0x01; \
          } while(0)

#define SB_ZF_KAEGPORTREMAPENTRY_GET_FIFONUM(nToData,pFromData,nMaxFromDataIndex) \
          do { \
           (nToData) = (uint32) ((pFromData)[0]) & 0x7f; \
          } while(0)

#endif
#endif
