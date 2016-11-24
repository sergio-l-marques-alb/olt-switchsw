/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    rip_vend_ctrl.h
* @purpose     RIP vendor-specific internal definitions
* @component   RIP Mapping Layer
* @comments    none
* @create      05/21/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef _RIP_VEND_CTRL_H_
#define _RIP_VEND_CTRL_H_

/* general IP Mapping headers */
#include "l7_ripinclude.h"
#include "rip_vend_exten.h"
#include "defs.h"                       /* RIP vendor definitions */

/* undo the printf remapping used for the vendor code */
#ifdef printf
#undef printf
#endif

/* Common global externs */
extern struct ripMapCtrl_s   ripMapCtrl_g;

/* Debug message levels */
#define RIP_MAP_MSGLVL_ON     0         /* use where printf is desired */
#define RIP_MAP_MSGLVL_HI     1
#define RIP_MAP_MSGLVL_MED    50
#define RIP_MAP_MSGLVL_LO     100
#define RIP_MAP_MSGLVL_OFF    10000     /* some arbitrarily large value */

/* DEBUG build only: general printf-style macro for debug messages */
#define RIP_MAP_PRT(mlvl, fmt, args...) \
          { \
          if (ripMapCtrl_g.msgLvl >= (mlvl)) \
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, fmt, ##args); \
          }

/* literal definitions */
#define RIP_MAP_RTR_INTF_MAX    (L7_RTR_MAX_RTR_INTERFACES +1)


/* RIP task control structure */
typedef struct ripMapTaskCtrl_s
{
  void *        semId;                  /* semaphore object handle */
  L7_BOOL       semAvail;               /* flag indicating state of sema */
  L7_BOOL       taskInit;               /* signifies rip_Task completed init */
} ripMapTaskCtrl_t;


/* RIP control context */
typedef struct ripMapCtrl_s
{
  ripMapTaskCtrl_t  taskCtrl;           /* controls for vendor RIP task */
  void *            rnTreeSemId;        /* RIP radix tree semaphore */
  L7_uint32         msgLvl;             /* debug message control */
  L7_BOOL           traceEnabled;       /* controls RIP trace messages */
} ripMapCtrl_t;


/* RIP pass through structure */
typedef struct ripPassThru_s
{
  rip_redist_proto rip_proto;
  unsigned int metric;
} ripPassThru_t;

/*-------------------------------------*/
/* Fixed RIP configuration definitions */
/*-------------------------------------*/

/* NOTE:  These RIP configuration values are only changeable at compile
 *        time.  The user interface does not currently support setting
 *        these values.
 */

#define RIP_MAP_CFG_RDISC_PREF    DEF_PreferenceLevel         /* from defs.h */
#define RIP_MAP_CFG_RDISC_INT     DefMaxAdvertiseInterval     /* from defs.h */

/* int_state bit preferences (must do any validity checking in the code) */

/* NOTE:  Must only use 0 or 1 value in the following macro */
#define RIP_MAP_CFG_PREFER(_val, _is) ((_val) * (_is))

/* set your int_state bit preference here */
#define RIP_MAP_CFG_NO_AG         RIP_MAP_CFG_PREFER( 0, IS_NO_AG       )
#define RIP_MAP_CFG_NO_SUPER_AG   RIP_MAP_CFG_PREFER( 0, IS_NO_SUPER_AG )
#define RIP_MAP_CFG_NO_RDISC      RIP_MAP_CFG_PREFER( 0, IS_NO_RDISC    )
#define RIP_MAP_CFG_NO_SOL_OUT    RIP_MAP_CFG_PREFER( 0, IS_NO_SOL_OUT  )
#define RIP_MAP_CFG_SOL_OUT       RIP_MAP_CFG_PREFER( 1, IS_SOL_OUT     )
#define RIP_MAP_CFG_NO_ADV_OUT    RIP_MAP_CFG_PREFER( 0, IS_NO_ADV_OUT  )
#define RIP_MAP_CFG_ADV_OUT       RIP_MAP_CFG_PREFER( 1, IS_ADV_OUT     )
#define RIP_MAP_CFG_BCAST_RDISC   RIP_MAP_CFG_PREFER( 0, IS_BCAST_RDISC )
#define RIP_MAP_CFG_NO_RIP        RIP_MAP_CFG_PREFER( 0, IS_NO_RIP      )
#define RIP_MAP_CFG_PM_RDISC      RIP_MAP_CFG_PREFER( 0, IS_PM_RDISC    )
#define RIP_MAP_CFG_DISTRUST      RIP_MAP_CFG_PREFER( 0, IS_DISTRUST    )
#define RIP_MAP_CFG_REDIRECT_OK   RIP_MAP_CFG_PREFER( 1, IS_REDIRECT_OK )

/* the code uses this macro to set or clear the bit accordingly */
#define RIP_MAP_CFG_INT_STATE_SET(_is, _x) \
          { if ((_x)) ((_is) |= (_x)); else ((_is) &= ~(_x)); }


/*-------------------------------------------------------*/
/* Internal RIP MAP vendor component function prototypes */
/*-------------------------------------------------------*/

/*-----------------*/
/* rip_vend_ctrl.c */
/*-----------------*/


/*********************************************************************
* @purpose  Set up relevant config parms in the specified RIP
*           interface structure
*
* @param    ifp         pointer to vendor RIP interface structure
*
* @returns  void
*
* @notes    This function is called directly by the RIP vendor code.
*
* @end
*********************************************************************/
void ripMapVendUserParmsGet(struct interface *ifp);

/*********************************************************************
* @purpose  Provides the maximum number of RIP routes allowed
*
* @param    void
*
* @returns  Maximum number of RIP routes to support
*
* @notes    none
*
* @end
*********************************************************************/
int ripMapVendRipRoutesMaxGet(void);

/*********************************************************************
* @purpose  Checks a semaphore to determine if the RIP task should be
*           suspended
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapVendTaskControl(void);

/*********************************************************************
* @purpose  Obtains a semaphore for mutually-exclusive access to the
*           RIP radix node tree processing critical section
*
* @param    void
*
* @returns  0           successful
* @returns  -1          error occurred
*
* @notes    none
*
* @end
*********************************************************************/
int ripMapVendRnTreeCritEnter(void);

/*********************************************************************
* @purpose  Releases RIP radix node tree mutual exclusion semaphore
*
* @param    void
*
* @returns  0           successful
* @returns  -1          error occurred
*
* @notes    none
*
* @end
*********************************************************************/
int ripMapVendRnTreeCritExit(void);

/*********************************************************************
* @purpose  Sets RIP tracing mode on or off
*
* @param    mode        RIP trace mode (0=off, any other value=on)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapVendTraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Handles trace logging from the RIP application
*
* @param    id          trace point identifier
* @param    fmt         format string
* @param    args...     variable length argument list
*
* @returns  void
*
* @notes    Displaying RIP trace entries requires both the
*           ripMapCtrl_g.traceEnabled mode flag to be set to L7_TRUE
*           **AND** the RIP mapping layer message level to be set
*           to medium or better.
*
* @end
*********************************************************************/
void ripMapVendTrace(int id, const char *fmt, ...);

/*********************************************************************
* @purpose  Invokes the message logging facility
*
* @param    message     Message to be logged
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapVendLogMsg(char *message);

/*********************************************************************
* @purpose  Wrapper for SYSAPI_PRINTF to display longer output information
*
* @param    msgLvl      message level specifier
* @param    fmt         format string
* @param    args...     variable length argument list
*
* @returns  void
*
* @notes    Function currently limited to printing up to 254 characters.
*
* @end
*********************************************************************/
void ripMapVendSysPrint(L7_uint32 msgLvl, const char *fmt, ...);

/*********************************************************************
* @purpose  This routine is called whenever a route table flush command
*           is executed by the user-interface
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteTableFlushCallBack(void);

/**************************************************************************
* @purpose     Convert an IP address from a hex value to an ASCII string
*
* @param       ipAddr     @b{(input)}  IP address to be converted (host byte
*                                        order)
* @param       buf        @b{(output)} location to store IP address string
* @param       buflen     @b{(input)}  length of the buffer provided by the
*                                      calling function.
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments    Caller must provide an output buffer of at least
* @comments    OSAPI_INET_NTOA_BUF_SIZE bytes (as defined in
* @comments    osapi.h).
*
* @end
*************************************************************************/
uint ripMapVendInetNtoa (naddr ipAddr, u_char *buf, uint buflen);

/**************************************************************************
* @purpose     To get the rip intf communication pipe and len of msg
*
* @param       intfPipe         Pointer to the pipe descriptor
* @param       intfPipeMsgLen   Pointer to the pipe msg len
*
* @returns     none
*
* @comments    Used by vendor code.
*
* @end
*************************************************************************/
void ripMapVendRipIntfPipeGet(int *intfPipe, uint *intfPipeMsgLen);

/**************************************************************************
* @purpose     To get the rip global settings
*
* @param       *ripSplitHorizon      @b{(output)} Split Horizon Mode
* @param       *ripAutoSummary       @b{(output)} Auto Summary Mode
* @param       *ripHostRoutesAccept  @b{(output)} Host Routes Accept Mode
*
* @returns     none
*
* @comments    Used by vendor code.
*
* @end
*************************************************************************/
void ripMapVendGlobalsGet(int *ripSplitHorizon, int *ripAutoSummary,
                          int *ripHostRoutesAccept);

/*********************************************************************
* @purpose  This routine is called by the protocol whenever it receives
*           default metric change message.
*
* @param    metric           @b{(input)} Changed default metric.
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void ripMapVendRipDefaultMetricSet(uint metric);

/*********************************************************************
* @purpose  This routine is called whenever the default metric has changed
*
* @param    *rn          @b{(input)} Next route in the route table.
* @param    *arp         @b{(input)} Default metric value
*
* @returns  0            Give back the next route in the route table.
*
* @comments This function is called by rn_walktree to change the metric of
*           redistributed routes if default metric has changed.
*
* @end
*********************************************************************/
int ripMapVendRipDefaultMetricRouteTableCallback(struct radix_node *rn, struct walkarg *arp);

/*********************************************************************
* @purpose  This routine is called by the protocol whenever it receives
*           metric change message.
*
* @param    rip_proto        @b{(input)} Protocol for which metric has changed.
* @param    metric           @b{(input)} Changed default metric.
*
* @returns
*
* @comments
*
* @end
*********************************************************************/
void ripMapVendRipRouteRedistMetricSet(rip_redist_proto rip_proto, uint metric);

/*********************************************************************
* @purpose  This routine is called whenever the metric for a protocol has changed
*
* @param    *rn          @b{(input)} Next route in the route table.
* @param    *arp         @b{(input)} Protocol and metric value
*
* @returns  0            Give back the next route in the route table.
*
* @comments This function is called by rn_walktree to change the metric of
*           redistributed routes for a particular protocol.
*
* @end
*********************************************************************/
int ripMapVendRipRouteRedistMetricRouteTableCallback(struct radix_node *rn, struct walkarg *arp);

/*********************************************************************
* @purpose  Convert the redist protocol type vendor rip redist protocol type.
*
* @param    protocol         @b{(input)} Redistributed protocol type
* @param    *rip_proto       @b{(output)} Vendor rip protocol type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteRedistProtocolConvert(L7_REDIST_RT_INDICES_t protocol, rip_redist_proto *rip_proto);

/*********************************************************************
* @purpose  Convert the vendor rip redist protocol type to redist protocol type.
*
* @param    rip_proto        @b{(input)} Vendor rip protocol type
* @param    *protocol        @b{(output)} Redistributed protocol type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapVendRouteRedistRipProtoProtocolConvert(rip_redist_proto rip_proto, L7_REDIST_RT_INDICES_t *protocol);

/*********************************************************************
* @purpose  This routine converts the internal interface number to the
*           router interface number to be used by the vendor code
*
* @param    intIfNum   	@b{(input)} internal interface number
* @param    *rtrIfNum   @b{(output)} router interface number
*
* @returns  0   if the matching router interface number exists
* @returns  -1  if no matching router interface number exists
*
* @comments 
*           
* @end
*********************************************************************/
int ripMapVendIfNumToRtrIfNum(L7_uint32 intIfNum, L7_uint32 *rtrIfNum);

#endif /* _RIP_VEND_CTRL_H_ */
