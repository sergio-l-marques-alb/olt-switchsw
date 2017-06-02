#ifndef __DOT3AD_HELPER_H__
#define __DOT3AD_HELPER_H__

#include "dot3ad_lac.h"
#include "ms_api.h"

typedef enum
{
  DOT3AD_HELPER_PDU_START = 1,
  DOT3AD_HELPER_PDU_STOP,
  DOT3AD_HELPER_PDU_MODIFY,
  DOT3AD_HELPER_TIMER_EXPIRE,
  /* The control message beyond this msg type
     These messages are not based on port. The port based precede ctl
  */
  DOT3AD_HELPER_CONTROL_START, 
  DOT3AD_HELPER_SWITCHOVER_START,
  DOT3AD_HELPER_SWITCHOVER_DONE,
  DOT3AD_HELPER_PURGE,
  DOT3AD_HELPER_CONTROL_END, 
  DOT3AD_HELPER_MSG_TYPE_LAST
} dot3ad_helper_msgType_t;

typedef enum
{
  DOT3AD_HELPER_PORT_NOT_CONFIG,
  DOT3AD_HELPER_PORT_CURRENT,
  DOT3AD_HELPER_PORT_STALE
}dot3ad_helper_states_t;

typedef struct
{
  L7_uint32      ctrlMsgType;
} dot3ad_helper_ctrl_data_t;

typedef struct 
{
  msSyncMsgHdr_t msHeader;
  dot3ad_helper_ctrl_data_t ctrlData;
}dot3ad_helper_ctrl_msg_t;

typedef struct
{
  dot3ad_pdu_t pdu;
  dot3ad_helper_msgType_t msgType;
  L7_uint32 interval;
  L7_uint32 intIfNum;
  nimUSP_t usp;

} dot3adHelperMsg_t;

typedef struct 
{
  msSyncMsgHdr_t msHeader;
  dot3adHelperMsg_t helperData;
}dot3ad_helper_data_msg_t;


#define DOT3AD_HELPER_PRIO (L7_DOT1P_NUM_TRAFFIC_CLASSES - 1)
#define DOT3AD_HELPER_CTL_MSG_LEN (sizeof(dot3ad_helper_ctrl_msg_t))
#define DOT3AD_HELPER_IS_CTL_MSG(_msgType) (((_msgType) > DOT3AD_HELPER_CONTROL_START) && \
                                             ((_msgType) < DOT3AD_HELPER_CONTROL_END))   
extern L7_uint32 dot3adHelperUsrHandle;
void dot3adHelperTimerCallback(L7_uint32 portIndex);
L7_RC_t dot3adHelperIssueCmd(void *msg, L7_BOOL controlMsg);


/*********************************************************************
* @purpose  Initiate the Helper subsystem for dot3ad.
*
* @param    
*
* @returns  L7_SUCCESS if succesful
*           L7_NO_MEMORY if out of memory
*           L7_FAILURE otherwise
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t dot3adHelperInit();

/*********************************************************************
* @purpose  Returns the resources allocated by the helper subsystem
*
* @param    
*
* @returns  void
*
* @notes    
*       
* @end
*********************************************************************/
void dot3adHelperFini();

/*********************************************************************
* @purpose  dot3ad helper task which serves the local unit
*
* @param    
*
* @returns  void
*
* @notes    This task upon receiving notification from core tx task stores the PDU in the 
*           
*       
* @end
*********************************************************************/
void dot3ad_helper_task();

#endif /*__DOT3AD_HELPER_H__*/
