/*
 * $Id: nlmcmctype.c,v 1.2.8.1 Broadcom SDK $
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
 */


 
#ifdef NLMCTYPE_PRINT_TABLES
#include <ctype.h>
#include <nlmcmctype_def.h>
#else
#include <nlmcmctype.h>
#endif

unsigned short __NlmCmCtype_TestTbl[256] = {
    0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804,
    0x0804, 0x1904, 0x0904, 0x0904, 0x0904, 0x0904, 0x0804, 0x0804,
    0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804,
    0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804, 0x0804,
    0x1940, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0,
    0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0,
    0x0c69, 0x0c69, 0x0c69, 0x0c69, 0x0c69, 0x0c69, 0x0c69, 0x0c69,
    0x0c69, 0x0c69, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0,
    0x08e0, 0x0e63, 0x0e63, 0x0e63, 0x0e63, 0x0e63, 0x0e63, 0x0a63,
    0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63,
    0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63, 0x0a63,
    0x0a63, 0x0a63, 0x0a63, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x08e0,
    0x08e0, 0x0c73, 0x0c73, 0x0c73, 0x0c73, 0x0c73, 0x0c73, 0x0873,
    0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873,
    0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873, 0x0873,
    0x0873, 0x0873, 0x0873, 0x08e0, 0x08e0, 0x08e0, 0x08e0, 0x0804,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
} ;

int NlmCm__tolower(int x)
{
    return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
}

int NlmCm__toupper(int x)
{
    return (x >= 'a' && x <= 'z') ? x - 'a' + 'A' : x;
}



#ifdef NLMCTYPE_PRINT_TABLES

/* Use the following to regenerate the table above */
int main(
    int argc, 
    char* argv[]) 
{
    int i ;
    unsigned short *testTbl = __NlmCmCtype_TestTbl ;

    for (i = 0; i < 256; i++ ) {
    testTbl[i] = 0 ;
    if (isalnum(i))  testTbl[i] |= (1 << _NlmCm__isalnum_pos) ;
    if (isalpha(i))  testTbl[i] |= (1 << _NlmCm__isalpha_pos) ; 
    if (iscntrl(i))  testTbl[i] |= (1 << _NlmCm__iscntrl_pos) ;
    if (isdigit(i))  testTbl[i] |= (1 << _NlmCm__isdigit_pos) ; 
    if (islower(i))  testTbl[i] |= (1 << _NlmCm__islower_pos) ;
    if (isgraph(i))  testTbl[i] |= (1 << _NlmCm__isgraph_pos) ;
    if (isprint(i))  testTbl[i] |= (1 << _NlmCm__isprint_pos) ;
    if (ispunct(i))  testTbl[i] |= (1 << _NlmCm__ispunct_pos) ;
    if (isspace(i))  testTbl[i] |= (1 << _NlmCm__isspace_pos) ;
    if (isupper(i))  testTbl[i] |= (1 << _NlmCm__isupper_pos) ;
    if (isxdigit(i)) testTbl[i] |= (1 << _NlmCm__isxdigit_pos) ;
    if (isblank (i)) testTbl[i] |= (1 << _NlmCm__isblank_pos) ;
    if (isascii(i))  testTbl[i] |= (1 << _NlmCm__isascii_pos) ;
    }   

    NlmCm__printf("unsigned short __NlmCmCtype_TestTbl[256] = {\n") ;
    NlmCm__printf("    ") ;
    for (i=0 ; i<256 ;  i++) {
    if (i) {
        if ((i % 8) == 0)
        NlmCm__printf(",\n    ") ;
        else
        NlmCm__printf(", ") ;
    }
    NlmCm__printf("0x%04x", testTbl[i]) ;
    }
    NlmCm__printf("\n} ;\n\n") ;

    return 0 ;  
}
#endif
