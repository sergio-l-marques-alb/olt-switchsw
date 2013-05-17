/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  mfc_debug.h
*
* @purpose   The purpose of this file is to export the functionalities
*            implemented by the corresponding C file (mfc_debug.c).
*
* @component Multicast Forwarding Cache (MFC)
*
* @comments  This file should not be included by any header or C files
*            other than those that belong to the MFC component itself.
*            All other components MUST use the functionalities exported
*            by the mfc_api.h file only.
*
* @create    March 24, 2006
*
* @author    ddevi.
* @end
*
**********************************************************************/
#ifndef _MFC_DEBUG_H
#define _MFC_DEBUG_H

/**********************************************************************
       Include Files (only those required to compile this file)
**********************************************************************/
#include "l7_common.h"

/**********************************************************************
                 Typedefs and Defines
**********************************************************************/

#define MFC_MAX_MSG_SIZE 256
#define MFC_DEBUG_PRINTF sysapiPrintf

typedef enum
{
  MFC_DEBUG_RX = 0,     /* Trace the complete Receive Data path */
  MFC_DEBUG_TX,         /* Trace the complete Transmission Data path */
  MFC_DEBUG_EVENTS,     /* Trace all the EVENT generations and receptions */
  MFC_DEBUG_TIMERS,     /* Trace all Timer activities */
  MFC_DEBUG_FAILURES,   /* Trace all failures */
  MFC_DEBUG_APIS,       /* Trace all the APIs invoked */
  MFC_DEBUG_RATE_LIMIT, /* Trace the WrongIf Rate Limit Path */
  MFC_DEBUG_MRP_UPDATES, /* Trace the MRP->MFC Updates and Deletes */
  MFC_DEBUG_HW_UPDATES, /* Trace the MFC->HW Updates and Deletes */
  MFC_DEBUG_FLAG_LAST   /* This should not be used by the code. It MUST be the
                           last element of this list and is meant to indicate
                           the number of enumerations defined */
} MFC_DEBUG_FLAGS_t;

#define MFC_DEBUG(dbg_type, __fmt__, __args__...) \
{                                                    \
  if (mfcDebugFlagCheck(dbg_type) == L7_TRUE)  {              \
     L7_char8  __buf1__[MFC_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[MFC_MAX_MSG_SIZE];          \
     L7_timespec time;                              \
     osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, MFC_MAX_MSG_SIZE, __fmt__, ## __args__);  \
     (void)osapiSnprintf (__buf2__, MFC_MAX_MSG_SIZE,   \
           "\n%02d:%02d:%02d MFC: In %s() - %s",         \
           time.hours, time.minutes, time.seconds, __FUNCTION__, __buf1__);   \
     MFC_DEBUG_PRINTF(__buf2__);                                     \
  }                                                   \
 }


#define MFC_DEBUG_ADDR(dbg_type, __str__, __xaddr__)  \
 {                                                    \
  if (mfcDebugFlagCheck(dbg_type) == L7_TRUE)  {              \
     L7_char8  __buf1__[MFC_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[MFC_MAX_MSG_SIZE];          \
     L7_timespec time;                              \
     osapiUpTime(&time);                             \
     (void)inetAddrHtop((__xaddr__),__buf1__);               \
     (void)osapiSnprintf (__buf2__, MFC_MAX_MSG_SIZE,   \
           "\n%02d:%02d:%02d MFC: In %s() -%s %s",        \
           time.hours, time.minutes, time.seconds, __FUNCTION__, __str__, __buf1__);   \
     MFC_DEBUG_PRINTF(__buf2__);                                      \
  }                                                   \
 }

#define MFC_LOG_MSG(familyType, __fmt__, __args__...) \
 do \
 {\
   L7_char8  __buf1__[MFC_MAX_MSG_SIZE];          \
   L7_char8  __buf2__[MFC_MAX_MSG_SIZE];          \
   L7_char8  __buf3__[MFC_MAX_MSG_SIZE] ="MFC";   \
   (void)osapiSnprintf(__buf1__,MFC_MAX_MSG_SIZE, __fmt__, ## __args__);   \
   if(familyType == L7_AF_INET)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__,MFC_MAX_MSG_SIZE, "\n%s - %s",         \
                 " MFC-V4 ", __buf1__);                \
   } \
   else if(familyType == L7_AF_INET6)        \
   {                                                       \
     (void)osapiSnprintf(__buf2__, MFC_MAX_MSG_SIZE, "\n %s - %s",         \
                 " MFC-V6  ", __buf1__);                \
   }\
   else \
   { \
     (void) osapiSnprintf(__buf2__,MFC_MAX_MSG_SIZE, " \n %s : %s", \
                    __buf3__, __buf1__);    \
   }\
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_MCAST_MAP_COMPONENT_ID, __buf2__);  \
 }while(0)


/**********************************************************************
                  Function Prototypes
**********************************************************************/
extern L7_RC_t mfcDebugEnable(void);
extern L7_RC_t mfcDebugDisable(void);
extern L7_RC_t mfcDebugAllSet(void);
extern L7_RC_t mfcDebugAllReset(void);
extern L7_RC_t mfcDebugFlagSet(MFC_DEBUG_FLAGS_t flag);
extern L7_RC_t mfcDebugFlagReset(MFC_DEBUG_FLAGS_t flag);
extern L7_BOOL mfcDebugFlagCheck(MFC_DEBUG_FLAGS_t dbg_type);
#endif /* _MFC_DEBUG_H */
