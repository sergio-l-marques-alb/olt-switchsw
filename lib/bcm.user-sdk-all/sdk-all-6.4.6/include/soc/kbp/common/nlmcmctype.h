/*
 * $Id: nlmcmctype.h,v 1.2.8.2 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


 
/*
 * Replacement macros for ctype functions/macros. 
 * Note that we only support the C-Locale.
 */
#ifndef INCLUDED_NLMCMCTYPE_H
#define INCLUDED_NLMCMCTYPE_H

#ifndef NLMPLATFORM_BCM
#include <nlmcmbasic.h>
#include <nlmcmctype_def.h>
#include <nlmcmexterncstart.h>
#else
#include <soc/kbp/common/nlmcmbasic.h>
#include <soc/kbp/common/nlmcmctype_def.h>
#include <soc/kbp/common/nlmcmexterncstart.h>
#endif

extern unsigned short __NlmCmCtype_TestTbl[] ;

#define __NlmCmCtypeLookup(x,t) (__NlmCmCtype_TestTbl[(x)&0xff] & (1 << t))

#define _NlmCmCtypeLookup(x,t)  __NlmCmCtypeLookup((x),t)


#define NlmCm__isalnum(x)   _NlmCmCtypeLookup((x),_NlmCm__isalnum_pos)
#define NlmCm__isalpha(x)   _NlmCmCtypeLookup((x),_NlmCm__isalpha_pos)
#define NlmCm__iscntrl(x)   _NlmCmCtypeLookup((x),_NlmCm__iscntrl_pos)
#define NlmCm__isdigit(x)   _NlmCmCtypeLookup((x),_NlmCm__isdigit_pos)
#define NlmCm__isxdigit(x)  _NlmCmCtypeLookup((x),_NlmCm__isxdigit_pos)
#define NlmCm__islower(x)   _NlmCmCtypeLookup((x),_NlmCm__islower_pos)
#define NlmCm__isgraph(x)   _NlmCmCtypeLookup((x),_NlmCm__isgraph_pos)
#define NlmCm__isprint(x)   _NlmCmCtypeLookup((x),_NlmCm__isprint_pos)
#define NlmCm__ispunct(x)   _NlmCmCtypeLookup((x),_NlmCm__ispunct_pos)
#define NlmCm__isspace(x)   _NlmCmCtypeLookup((x),_NlmCm__isspace_pos)
#define NlmCm__isupper(x)   _NlmCmCtypeLookup((x),_NlmCm__isupper_pos)
#define NlmCm__isblank(x)   _NlmCmCtypeLookup((x),_NlmCm__isblank_pos)

#define NlmCm__isascii(x)   (((x) & ~0x7f) == 0)
#define NlmCm__toascii(x)   ((x) & 0x7f)

/* Define these as functions to avoid side-effects
 */
extern int NlmCm__tolower(int x) ;
extern int NlmCm__toupper(int x) ;

#ifndef NLMPLATFORM_BCM
#include <nlmcmexterncend.h>
#else
#include <soc/kbp/common/nlmcmexterncend.h>
#endif

#endif


/*[]*/

