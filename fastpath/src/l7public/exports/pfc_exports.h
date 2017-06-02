/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename pfc_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __PFC_EXPORTS_H_
#define __PFC_EXPORTS_H_

/* Policy Component Feature List */
typedef enum
{
  L7_PFC_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID,
  L7_PFC_FEATURE_ID_TOTAL,                 /* total number of enum values */
} L7_PFC_FEATURE_IDS_t;

/* PFC Action values */
typedef enum
{
  L7_PFC_ACTION_DROP = 0,  
  L7_PFC_ACTION_NO_DROP,    
} L7_PFC_ACTION_t;


#define PFC_MODE_DISABLE  ((L7_uchar8) 0)
#define PFC_MODE_ENABLE   ((L7_uchar8) 1)

#define PFC_PRI_DROP_MODE    ((L7_uint32) 0)
#define PFC_PRI_NODROP_MODE  ((L7_uint32) 1)

#define L7_QOS_PFC_PRIORITY_ALL         8
#define L7_QOS_PFC_PRIORITY_MIN         0
#define L7_QOS_PFC_PRIORITY_MAX         (L7_QOS_PFC_PRIORITY_ALL-1)

#endif /* __PFC_EXPORTS_H_*/
