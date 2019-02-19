/*
 * $Id: nlmcmbasic.c,v 1.2.8.1 Broadcom SDK $
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


 
#include <nlmcmbasic.h>
#include <nlmcmstring.h>
#include <nlmcmportable.h>
#include <nlmcmdebug.h>
#include <nlmcmcast.h>

void NlmCmBasic__WriteStdErr(const char* msg)
{
    NlmCm__printf(msg) ;    
}


void NlmCmBasic__Panic(const char* msg)
{
    NlmCmBasic__WriteStdErr("Error: Panic: ") ;
    NlmCmBasic__WriteStdErr(msg) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmDebug__Break() ;
    NlmCm__pvt_abort() ;
}


extern int
NlmCmBasic__DemandFileLine(
    NlmBool cond,
    const char* assertion,
    const char* file,
    int line)
{
    /*
     * FYI: emit an error indication before the assert fires
     *      to force the error to show up in the VC++ transcript
     *      so that the VC++ generated error/warning summary is correct.
     */
    if (!cond) {
    NlmCmBasic__FailedFileLine(assertion, file, line) ;
    NlmCmBasic__Panic("NlmCmBasic__Demand") ;
    }
    return cond;
}


NlmBool NlmCmBasic__Identity(const NlmBool b)
{
    return b ;
}


char* NlmCmCharStar__Identity(char* c)
{
    return c ;
}


const char* NlmCmConstCharStar__Identity(const char* c)
{
    return c ;
}


int 
NlmCmBasic__Note(const char* msg)
{
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=\n");
    NlmCmBasic__WriteStdErr("=---= NOTE: ") ; NlmCmBasic__WriteStdErr(msg) ; NlmCmBasic__WriteStdErr(" =---=\n") ;
    NlmCmBasic__WriteStdErr("=---=---=---=---=---=---=---=---=---=---=---=---=---=---=---=\n");
    return 0 ;
}


int
NlmCmBasic__FailedFileLine(const char* msg, const char* file, int line)
{
    char    linenum[10] ;
    NlmBool banner = 1 ;

    if (*msg == '@') {
    msg++ ;
    banner = 0 ;
    }

    /* FYI: using Emacs compatible error message format. NB: quite delicate. */
    NlmCm__snprintf(linenum, sizeof linenum, "%04d", line) ;

    NlmCmBasic__WriteStdErr("\n") ;
    if (banner) {
    NlmCmBasic__WriteStdErr("===============================================\n");
    NlmCmBasic__WriteStdErr("===============================================\n");
    NlmCmBasic__WriteStdErr("==  ####### ######  ######  ####### ######   ==\n");
    NlmCmBasic__WriteStdErr("==  #       #     # #     # #     # #     #  ==\n");
    NlmCmBasic__WriteStdErr("==  #       #     # #     # #     # #     #  ==\n");
    NlmCmBasic__WriteStdErr("==  #####   ######  ######  #     # ######   ==\n");
    NlmCmBasic__WriteStdErr("==  #       #   #   #   #   #     # #   #    ==\n");
    NlmCmBasic__WriteStdErr("==  #       #    #  #    #  #     # #    #   ==\n");
    NlmCmBasic__WriteStdErr("==  ####### #     # #     # ####### #     #  ==\n");
    }
    NlmCmBasic__WriteStdErr("===============================================\n");
    NlmCmBasic__WriteStdErr("===============================================\n");
    NlmCmBasic__WriteStdErr("ERROR in line ") ;
    NlmCmBasic__WriteStdErr(linenum) ;
    NlmCmBasic__WriteStdErr(" of file ") ;
    NlmCmBasic__WriteStdErr(file ? file : "<unknown>") ;
    NlmCmBasic__WriteStdErr(" ...\nFAILED: ") ;
    NlmCmBasic__WriteStdErr(msg) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("===============================================\n");
    NlmCmBasic__WriteStdErr("===============================================\n");

    return 0;
}


int
NlmCmBasic__WarningFileLine(const char* msg, const char* file, int line)
{
    char    linenum[10] ;
    NlmBool banner = 1 ;
    const char* warning_in_line = "WARNING in line ";

    if (*msg == '@') {
    msg++ ;
    banner = 0 ;
    }

    if (*msg == '-') {
    msg++ ;
    warning_in_line = "ignored_WARNING in line ";
    }

    /*
      handle both @- and -@
    */
    if (*msg == '@') {
    msg++ ;
    banner = 0 ;
    }

    if (banner) {
    /* FYI: using Emacs compatible error message format. NB: quite delicate. */
    NlmCm__snprintf(linenum, sizeof linenum, "%04d", line) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("==============================================================\n");
    NlmCmBasic__WriteStdErr("==============================================================\n");
    NlmCmBasic__WriteStdErr("==  #    #    ##    #####   #    #     #    #    #   ####   ==\n");
    NlmCmBasic__WriteStdErr("==  #    #   #  #   #    #  ##   #     #    ##   #  #    #  ==\n");
    NlmCmBasic__WriteStdErr("==  #    #  #    #  #    #  # #  #     #    # #  #  #       ==\n");
    NlmCmBasic__WriteStdErr("==  # ## #  ######  #####   #  # #     #    #  # #  #  ###  ==\n");
    NlmCmBasic__WriteStdErr("==  ##  ##  #    #  #   #   #   ##     #    #   ##  #    #  ==\n");
    NlmCmBasic__WriteStdErr("==  #    #  #    #  #    #  #    #     #    #    #   ####   ==\n");
    NlmCmBasic__WriteStdErr("==============================================================\n");
    NlmCmBasic__WriteStdErr("==============================================================\n");
    NlmCmBasic__WriteStdErr(warning_in_line) ;
    NlmCmBasic__WriteStdErr(linenum) ;
    NlmCmBasic__WriteStdErr(" of file ") ;
    NlmCmBasic__WriteStdErr(file ? file : "<unknown>") ;
    NlmCmBasic__WriteStdErr(" ...\n       ... ") ;
    NlmCmBasic__WriteStdErr(msg) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("==============================================================\n");
    NlmCmBasic__WriteStdErr("==============================================================\n");
    }
    else {
    /* FYI: using Emacs compatible error message format. NB: quite delicate. */
    NlmCm__snprintf(linenum, sizeof linenum, "%04d", line) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    NlmCmBasic__WriteStdErr(warning_in_line) ;
    NlmCmBasic__WriteStdErr(linenum) ;
    NlmCmBasic__WriteStdErr(" of file ") ;
    NlmCmBasic__WriteStdErr(file ? file : "<unknown>") ;
    NlmCmBasic__WriteStdErr(" :: ") ;
    NlmCmBasic__WriteStdErr(msg) ;
    NlmCmBasic__WriteStdErr("\n") ;
    NlmCmBasic__WriteStdErr("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    }

    return 0;
}


int
NlmCmBasic__DieFileLine(const char* msg, const char* file, int line)
{
    NlmCmBasic__FailedFileLine(msg, file, line) ;
    NlmCmBasic__Panic("NlmCmBasic__DieFileLine") ;
    return 0 ;
}


void
NlmCmBasic__CopyZeroFill(
    nlm_u8*         dst,
    int             dstSize,
    const nlm_u8*   src,
    int             srcSize)
{
    assert(0 <= srcSize && srcSize <= dstSize) ;
    assert(dst) ;
    assert(src || srcSize == 0) ;
    NlmCm__memcpy(dst, src, srcSize) ;
    NlmCm__memset(dst + srcSize, 0, dstSize-srcSize) ;
}

#include <nlmcmexterncstart.h>
void
NlmCmBasic__Assert(NlmBool b)
{
    NlmCmBasic__Require(b) ;
}
#include <nlmcmexterncend.h>

extern uintvs_t
NlmCmUintVS__Identity(uintvs_t value)
{
    return value ;
}


extern nlm_8
NlmCmInt8__Identity(nlm_8 value)
{
    return value ;
}


extern nlm_16
NlmCmInt16__Identity(nlm_16 value)
{
    return value ;
}


extern nlm_32
NlmCmInt32__Identity(nlm_32 value)
{
    return value ;
}


extern int
NlmCmInt__Identity(int value) 
{
    return value ;
}


extern nlm_u8
NlmCmUint8__Identity(nlm_u8 value)
{
    return value ;
}


extern nlm_u16
NlmCmUint16__Identity(nlm_u16 value) 
{
    return value ;
}


extern nlm_u32
NlmCmUint32__Identity(nlm_u32 value)
{
    return value ;
}


extern unsigned int
NlmCmUint__Identity(unsigned int value)
{
    return value ;
}


extern void*
NlmCmVoidStar__Identity(void *value)
{
    return value ;
}


extern const nlm_u8*
NlmCmUint8Star__Identity(const nlm_u8 *value)
{
    return value ;
}


extern nlm_u8*
NlmCmBasic__Zero(
    void *data_area, 
    size_t data_len)
{
    NlmCm__memset(data_area, 0, data_len) ;
    return (nlm_u8*) data_area ;
}


extern NlmBool
NlmCmBasic__IsZero(
    const void *data_area, 
    size_t data_len)
{
    const nlm_u8* cp = (const nlm_u8*)(data_area);

    while( data_len--) {
    if (*cp++) return 0;
    }
    return 1;
}


NlmBool
NlmCmBasic__IsStrEq(const char* s, const char* t)
{
    return 0 == NlmCm__strcmp(s, t) ;
}






/*[]*/
