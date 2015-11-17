/*
 * $Id: nlmcmlogger.c,v 1.2.8.1 Broadcom SDK $
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


 
#include <soc/kbp/common/nlmcmbasic.h>
#include "nlmcmlogger.h"

#ifdef NLMPLATFORM_BCM
#include <sal/appl/io.h>
#endif


typedef struct NlmCmLogger
{
    nlm_u32 logLevel;   /* Level of Logging for all modules */
    nlm_u32 logModules;     /* Modules for which logging is enabled */
    FILE    *logFileP;
} NlmCmLogger;

/* static object of NlmCmLogger */

static NlmCmLogger gNlmCmLogger;


/*  This function initializes the gNlmCmLogger with the given value.
       If logFile is not given then it will log messages to console */
void NlmCmLogger__Create(NlmCmLogLevel logLevel, char *logFile)
{
    
    gNlmCmLogger.logLevel = logLevel;

    /* Set the module level */
    gNlmCmLogger.logModules = (1 << NLMNS_MODULE_ALL) - 1; /* Set all modules bit to 1*/
    
    /* Set the log file pointer */
    
#ifndef NLMPLATFORM_BCM

    if(logFile)
    {
        gNlmCmLogger.logFileP = NlmCmFile__fopen(logFile,"w"); 
    }
    else
    {
        gNlmCmLogger.logFileP = NULL;
    }
#else
    gNlmCmLogger.logFileP = NULL;
#endif
    
    return;
}

void NlmCmLogger__Destroy()
{
#ifndef NLMPLATFORM_BCM
    if(gNlmCmLogger.logFileP)
    {
        NlmCmFile__fclose(gNlmCmLogger.logFileP);
        gNlmCmLogger.logFileP = NULL;
    }
#endif
    gNlmCmLogger.logLevel = NLMCM_LOG_NONE;
    gNlmCmLogger.logModules = NLMNS_MODULE_NONE;

    return;
}

/* Get the logging level */
int NlmCmLogger__GetLevel()
{
      return gNlmCmLogger.logLevel;
}

/* Set the logging level. It returns the older version, 
so you can store and reset it at the end of the block */
nlm_u32 NlmCmLogger__SetLevel(NlmCmLogLevel logLevel)
{
    nlm_u32 oldLevel  = 0;

    oldLevel = gNlmCmLogger.logLevel;
   gNlmCmLogger.logLevel = logLevel;
   
    return (oldLevel);
}

/* Set logging for given module */
void NlmCmLogger__SetModule(NlmNsModules logModule)
{
    if(logModule == NLMNS_MODULE_NONE)
    {
        gNlmCmLogger.logModules = 0;  
    }
    else if(logModule == NLMNS_MODULE_ALL)
    {
        gNlmCmLogger.logModules = (1 << logModule) - 1; /* Set all modules bit to 1*/
    }
    else
    {
        gNlmCmLogger.logModules |= (1 << logModule); /* Set that module bit to 1*/
    }
    
    return;
    
}


void NlmCmLogger__DisableModule(NlmNsModules module)
{
    if(module != NLMNS_MODULE_NONE)
    {
        gNlmCmLogger.logModules &= (~(1 << module));
    }

    return;
}

/* Check if logging is enabled for the module at given level */
static NlmBool NlmCmLogger__IsEnable(NlmCmLogLevel logLevel, NlmNsModules module)
{
    if((gNlmCmLogger.logLevel < (nlm_u32)logLevel) ||(gNlmCmLogger.logModules == NLMNS_MODULE_NONE))
    {
        return NlmFalse;
    }

    if(gNlmCmLogger.logModules & (1 << module ))
    {
        return NlmTrue;
    }
    
    return NlmFalse;
 
}


/* Log the data if logging is enabled for that module */
void NlmCmLogger__Log (NlmNsModules module,
                       NlmCmLogLevel logLevel, const char *format, ...)
{       

    if (NlmCmLogger__IsEnable(logLevel, module))
    {
        NlmCm_va_list arg_pointer;  
        
        NmlCm_va_start(arg_pointer,format); 

#ifndef NLMPLATFORM_BCM
        
        if(gNlmCmLogger.logFileP)
        {
            NmlCm_vfprintf(gNlmCmLogger.logFileP, format, arg_pointer);
        }
        else
        {
            /* Write to console */
            NmlCm_vprintf(format, arg_pointer);
        }
#else
        NmlCm_vprintf(format, arg_pointer);     
#endif
        

    }
    return;
}

void NlmCmLogger__LogPrefix(
    NlmNsModules module,
    NlmCmLogLevel logLevel,
    nlm_u16 inuse,
    nlm_u8* prefix_p)
{
    nlm_u32 bit = 0 ;
    nlm_u32 thisbyte ;
    nlm_u32 i ;
    nlm_u32 first = 1 ;
    nlm_u32 bitByte = 0;
    nlm_u8 bitMask = 0;
    nlm_u8 resultBit = 0;
    FILE* fp = gNlmCmLogger.logFileP;

    if(!NlmCmLogger__IsEnable(logLevel, module))
        return;

#ifndef NLMPLATFORM_BCM
    if(!fp)
        fp = stdout;   
#endif

    while (bit < inuse)
    {
        i = 0 ;
        thisbyte = 0 ;
        while (i < 8 && bit < inuse)
        {
            bitByte = bit >> 3;
            bitMask = 0x80 >> (nlm_u8)(bit & 7);
            resultBit =  (prefix_p[bitByte] & bitMask) != 0;
            thisbyte |= resultBit << (7 - i) ;
            i++ ;
            bit++ ;
        }

        if (!first)
            NlmCmFile__fprintf(fp, ".");
        
        NlmCmFile__fprintf(fp, "%d", thisbyte) ;

        first = 0 ;
    }

    NlmCmFile__fprintf(fp, "/%d\n", inuse) ;

}



#ifndef NLMCM_LOGGING_ENABLE
#if defined NLMPLATFORM_UNIX || defined NLMPLATFORM_BCM 
void NlmCmLogger__Dummy( NlmNsModules module, ...)
{
    (void)module;
    return;
}
#endif  /* NLMPLATFORM_UNIX : to support Linux/Solaris/FreeBSD/BCM */

#ifdef NLMPLATFORM_VXWORKS
void NlmCmLogger__Dummy( NlmNsModules module, ...)
{
    (void)module;
    return;
}
#endif  /* NLMPLATFORM_VXWORKS*/

#endif /* NLMCM_LOGGING_ENABLE */



