/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDebug.h
*
* @purpose  debug and statistics related structs and APIs.
*
* @component pimsm
*
* @comments
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#ifndef _PIMSM_DEBUG_H_
#define  _PIMSM_DEBUG_H_

#define PIMSM_STR_LEN 80 /* used in state name size for FSMs */

extern L7_uchar8 pimsmUpStrmSGStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmUpStrmSGEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmUpStrmSGRptStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmUpStrmSGRptEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmUpStrmStarGStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmUpStrmStarGEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmUpStrmStarStarRPStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmUpStrmStarStarRPEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmPerScopeZoneCandBSRStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmPerScopeZoneCandBSREventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmPerScopeZoneNonCandBSRStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmPerScopeZoneNonCandBSREventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmDnStrmPerIntfSGStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmDnStrmPerIntfSGEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmDnStrmPerIntfSGRptStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmDnStrmPerIntfSGRptEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmDnStrmPerIntfStarGStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmDnStrmPerIntfStarGEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmDnStrmPerIntfStarStarRPStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmDnStrmPerIntfStarStarRPEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmPerIntfSGAssertStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmPerIntfSGAssertEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmPerIntfStarGAssertStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmPerIntfStarGAssertEventName[][PIMSM_STR_LEN];

extern L7_uchar8 pimsmRegPerSGStateName[][PIMSM_STR_LEN];
extern L7_uchar8 pimsmRegPerSGEventName[][PIMSM_STR_LEN];

extern void pimsmDebugDummy();

typedef enum
{
  PIMSM_TRACE_DISABLE = 1,
  PIMSM_TRACE_FATAL = 2,
  PIMSM_TRACE_ERROR,
  PIMSM_TRACE_WARNING,
  PIMSM_TRACE_NORMAL,
  PIMSM_TRACE_NOTICE,
  PIMSM_TRACE_INFO,
  PIMSM_TRACE_DEBUG,
  PIMSM_TRACE_MAX
} pimsmTraceFlags_t;

/*NOTE:
 pimsmDebugFlags_t, pimsmDebugPathFlags_t and 
 pimsmDebugFlagName in pimsmdebug.c
 all the above should be updated in consistent fashion.
 */
typedef enum
{
    PIMSM_DEBUG_NONE =0,
    PIMSM_DEBUG_MACROS,            /* al macros definitions*/
    PIMSM_DEBUG_REG_RX_TX_FSM,       /*register/register-stop & fsm */
    PIMSM_DEBUG_HELLO_RX_TX,         /* Hello Rx/Tx */
    PIMSM_DEBUG_JOIN_RX_TX,          /* Join/Prune Rx/Tx */
    PIMSM_DEBUG_S_G_RPT,             /* (S,G,rpt) UP, DOWN FSMs & TREE */
    PIMSM_DEBUG_S_G,                 /*  (S,G) UP, DOWN FSMs & TREE  */
    PIMSM_DEBUG_STAR_G,              /* (*,G) UP, DOWN FSMs & TREE  */
    PIMSM_DEBUG_STAR_STAR_RP,        /* (*,*,RP) UP, DOWN FSMs & TREE */
    PIMSM_DEBUG_ASSERT_FSM,          /* ( (*,G) and (S,G) FSMs and Assert Rx/Tx) */
    PIMSM_DEBUG_RP,                  /* RP file */
    PIMSM_DEBUG_BSR,                 /* BSR file */
    PIMSM_DEBUG_BSR_FSM,             /* (cand and non-cand) */
    PIMSM_DEBUG_MFC_INTERACTION,     /* all MFC interactions */
    PIMSM_DEBUG_MGMD_INTERACTION,    /* all MGMD interactions */
    PIMSM_DEBUG_RTO_INTERACTION,     /* all RTO interactions */
    PIMSM_DEBUG_UI_INTERACTION,      /* all CLI/SNMP interactions */
    PIMSM_DEBUG_INTF_NEIGHBOR,       /*  intf and neighbor related */
    PIMSM_DEBUG_MISC,                /* (for v6 & anything else ) */
    PIMSM_DEBUG_ALL,
    PIMSM_DEBUG_MAX,
}pimsmDebugFlags_t;


typedef enum
{
    PIMSM_DEBUG_BIT_NONE = 0x00000,
    PIMSM_DEBUG_BIT_MACROS  = 0x00001,          /* al macros definitions*/
    PIMSM_DEBUG_BIT_REG_RX_TX_FSM = 0x00002,   /*register/register-stop & fsm */
    PIMSM_DEBUG_BIT_HELLO_RX_TX = 0x00004,  /* Hello Rx/Tx */
    PIMSM_DEBUG_BIT_JOIN_RX_TX = 0x00008,  /* Join/Prune Rx/Tx */
    PIMSM_DEBUG_BIT_S_G_RPT = 0x00010,  /* (S,G,rpt) UP, DOWN FSMs & TREE */
    PIMSM_DEBUG_BIT_S_G  = 0x00020,/*  (S,G) UP, DOWN FSMs & TREE  */
    PIMSM_DEBUG_BIT_STAR_G = 0x00040,/* (*,G) UP, DOWN FSMs & TREE  */
    PIMSM_DEBUG_BIT_STAR_STAR_RP = 0x00080, /* (*,*,RP) UP, DOWN FSMs & TREE */
    PIMSM_DEBUG_BIT_ASSERT_FSM = 0x00100,  /* ( (*,G) and (S,G) FSMs and Assert Rx/Tx) */
    PIMSM_DEBUG_BIT_RP = 0x00200,  /* RP file */
    PIMSM_DEBUG_BIT_BSR = 0x00400,  /* BSR file */
    PIMSM_DEBUG_BIT_BSR_FSM  = 0x00800,/* (cand and non-cand) */
    PIMSM_DEBUG_BIT_MFC_INTERACTION = 0x01000, /* all MFC interactions */
    PIMSM_DEBUG_BIT_MGMD_INTERACTION = 0x02000, /* all MGMD interactions */
    PIMSM_DEBUG_BIT_RTO_INTERACTION = 0x04000,  /* all RTO interactions */
    PIMSM_DEBUG_BIT_UI_INTERACTION = 0x08000,   /* all CLI/SNMP interactions */
    PIMSM_DEBUG_BIT_INTF_NEIGHBOR = 0x10000,   /*  intf and neighbor related */
    PIMSM_DEBUG_BIT_MISC  = 0x20000, /* (for v6 & anything else ) */
    PIMSM_DEBUG_BIT_ALL = 0xfffff
}pimsmDebugPathFlags_t;

#define PIMSM_DEBUG_PRINTF sysapiPrintf
/* remove below line to suppress all pimsm trace msgs */
#define PIMSM_TRACE_ENABLE

#define PIMSM_MAX_MSG_SIZE                   256 
#define PIMSM_MAX_DBG_ADDR_SIZE              IPV6_DISP_ADDR_LEN



#ifdef PIMSM_TRACE_ENABLE
extern pimsmTraceFlags_t pimsmCurrTraceLevel;
extern pimsmDebugPathFlags_t pimsmCurrDebugPathBits;
extern L7_uint32 pimsmDebugMsgCnt;
/*
 * Note: Ensure that string passed to PIMSM_TRACE is not exceeding 200 characters
 */
#define PIMSM_TRACE(pathFlags, traceLevel, __fmt__, __args__...)         \
 {                                                    \
  if (((pathFlags & pimsmCurrDebugPathBits) == pathFlags) &&  \
      (traceLevel) <= pimsmCurrTraceLevel )  {              \
     L7_char8  __buf1__[PIMSM_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[PIMSM_MAX_MSG_SIZE];          \
      pimsmDebugMsgCnt++;                             \
     (void)osapiSnprintf (__buf1__, PIMSM_MAX_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, PIMSM_MAX_MSG_SIZE,   \
               "\nPIMSM: LINE[%d] %s %d %%%% %s",         \
               __LINE__, __FUNCTION__, pimsmDebugMsgCnt, __buf1__);   \
        PIMSM_DEBUG_PRINTF(__buf2__);                                     \
  }                                                   \
 }             
#define PIMSM_TRACE_ADDR(pathFlags, traceLevel, __str__, __xaddr__)  \
 {                                                    \
  if (((pathFlags & pimsmCurrDebugPathBits) == pathFlags) &&  \
      (traceLevel) <= pimsmCurrTraceLevel )  {              \
     L7_char8  __buf1__[PIMSM_MAX_MSG_SIZE];    \
     L7_char8  __buf2__[PIMSM_MAX_MSG_SIZE];          \
      pimsmDebugMsgCnt++;                             \
     (void)inetAddrHtop((__xaddr__),__buf1__);               \
      (void)osapiSnprintf (__buf2__, PIMSM_MAX_MSG_SIZE,   \
        "\nPIMSM: LINE[%d] %s %d %%%% %s: %s",        \
        __LINE__,  __FUNCTION__,pimsmDebugMsgCnt, __str__,__buf1__);   \
       PIMSM_DEBUG_PRINTF(__buf2__);                                      \
  }                                                   \
 }             
#define PIMSM_TRACE_BITX(pathFlags, traceLevel, __xaddr__)  \
 {                                                    \
   if(((pathFlags & pimsmCurrDebugPathBits) == pathFlags) &&      \
    (traceLevel) <= pimsmCurrTraceLevel )  {             \
         BITX_DEBUG_PRINT(__xaddr__);                 \
   }                                                  \
 }             

#define PIMSM_TRACE_PKT(pathFlags, traceLevel, __msg__, __msgLen__)            \
{                                                                              \
 L7_uint32 __index__ = 0;                                                      \
 L7_uchar8 *__buffer__ = __msg__;                                              \
  if(((pathFlags & pimsmCurrDebugPathBits) == pathFlags) &&                      \
    (traceLevel) <= pimsmCurrTraceLevel )  {                                 \
    PIMSM_DEBUG_PRINTF ("\n***********************************************\n");\
    while (__index__ < __msgLen__)                                             \
    {                                                                          \
      PIMSM_DEBUG_PRINTF ("%2.2x ", __buffer__[__index__]);                  \
      __index__++;                                                             \
      if ((__index__ % 16) == 0)                                                \
        PIMSM_DEBUG_PRINTF ("\n");                                             \
    }                                                                          \
    PIMSM_DEBUG_PRINTF ("\n***********************************************\n");\
  }                                                                            \
}             

/*
#define PIMSM_DEBUG_ASSERT(p) ((p) ? (void)0 : printf(\
   "Assertion failed: %s,file %s, line %d",#p, __FILE__, __LINE__))
*/
#else
#define PIMSM_TRACE (traceLevel, __fmt__, __args__...)   
/*#define PIMSM_DEBUG_ASSERT(p)*/
#define PIMSM_TRACE_PKT(traceLevel, __msg__, __msgLen__)
#endif

extern void pimsmDebugUTDummy ();                         
#endif /* _PIMSM_DEBUG_H_ */

