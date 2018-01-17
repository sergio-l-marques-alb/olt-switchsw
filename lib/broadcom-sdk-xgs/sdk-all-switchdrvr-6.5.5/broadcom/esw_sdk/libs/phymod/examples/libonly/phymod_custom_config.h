/*
 * $Id: phymod_custom_config.h,v 1.1.2.6 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * System interface definitions for Switch SDK
 */

#ifndef __PHYMOD_CUSTOM_CONFIG_H__
#define __PHYMOD_CUSTOM_CONFIG_H__

#define PHYMOD_EAGLE_SUPPORT
#define PHYMOD_FALCON_SUPPORT

#define PHYMOD_DEBUG_ERROR(stuff_)
#define PHYMOD_DEBUG_VERBOSE(stuff_)
#define PHYMOD_DIAG_OUT(stuff_)

#define PHYMOD_USLEEP   my_usleep
#define PHYMOD_SLEEP    my_sleep
#define PHYMOD_MALLOC   my_malloc
#define PHYMOD_FREE     my_free

#ifndef TOOLS_NO_SIGNED_STDINT
#define int8_t int
#define int16_t int
#define int32_t int
#endif

#define PHYMOD_CONFIG_DEFINE_INT8_T     0
#define PHYMOD_CONFIG_DEFINE_INT16_T    0
#define PHYMOD_CONFIG_DEFINE_INT32_T    0

typedef unsigned long size_t;

extern void *my_memset(void *dest,int c,size_t cnt);
extern void *my_memcpy(void *dest,const void *src,size_t cnt);

extern int my_strcmp(const char *dest,const char *src);
extern int my_strncmp(const char *dest,const char *src,size_t cnt);
extern char *my_strncpy(char *dest,const char *src,size_t cnt);
extern char *my_strchr(const char *dest,int c);
extern char *my_strstr(const char *dest,const char *src);
extern size_t my_strlen(const char *str);
extern char *my_strcpy(char *dest,const char *src);
extern char *my_strcat(char *dest,const char *src);
extern char *my_strncat(char *dest,const char *src,size_t cnt);

extern unsigned long my_strtoul(const char *s, char **end, int base);

extern int my_snprintf(char *buf, size_t bufsize, const char *fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int my_sprintf(char *buf, const char *fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

#define PHYMOD_MEMSET   my_memset
#define PHYMOD_MEMCPY   my_memcpy

#define PHYMOD_STRCMP   my_strcmp
#define PHYMOD_STRNCMP  my_strncmp
#define PHYMOD_STRNCPY  my_strncpy
#define PHYMOD_STRCHR   my_strchr
#define PHYMOD_STRSTR   my_strstr
#define PHYMOD_STRLEN   my_strlen
#define PHYMOD_STRCPY   my_strcpy
#define PHYMOD_STRCAT   my_strcat
#define PHYMOD_STRNCAT  my_strncat

#define PHYMOD_STRTOUL  my_strtoul

#define PHYMOD_SPRINTF  my_sprintf
#define PHYMOD_SNPRINTF my_snprintf

/* No need to define size_t */
#define PHYMOD_CONFIG_DEFINE_SIZE_T     0

#ifdef TOOLS_UINT32_IS_LONG
#define PHYMOD_CONFIG_TYPE_UINT32_T     unsigned long
#define PHYMOD_CONFIG_MACRO_PRIu32      "ul"
#endif

#define LOG_WARN(ls_, stuff_)
#define BSL_META_U(u_, str_)
#define BSL_LS_SOC_PHY
#endif /* __PHYMOD_CUSTOM_CONFIG_H__ */
