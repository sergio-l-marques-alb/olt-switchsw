/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
*
* @filename   boxs.h
*
* @purpose    Box services support
*
* @component  
*
* @comments
*
* @create     1/15/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/
#ifndef BOXS_H
#define BOXS_H


/* Header files must be self resolving. */
#include "sysapi_hpc.h"
#include "log.h"
#include "bxs_exports.h"

typedef enum
{
  L7_BOXS_REQUEST = 1,
  L7_BOXS_RESPONSE,
} boxsMsgType_t;


typedef enum 
{
	BOXS_EVENT_NONE = 0,
	BOXS_EVENT_INSERTION,
	BOXS_EVENT_REMOVAL,
	BOXS_EVENT_BECOME_OPERATIONAL,
	BOXS_EVENT_FAILURE
} BOXS_ITEM_EVENT_t;


typedef enum
{
	BOXS_EVENT_TEMP_NONE = 0,
	BOXS_EVENT_TEMP_ABOVE_THRESHOLD,
	BOXS_EVENT_TEMP_BELOW_THRESHOLD,
	BOXS_EVENT_TEMP_WITHIN_NORMAL_RANGE
} BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t;


typedef struct
{
  boxsMsgType_t msgType;
  L7_uint32       unitNum;
  L7_RC_t         boxsStatus; 
} boxsMsgHeader_t;


typedef struct
{
  L7_uint32 timestamp;
  /* Fans */
  L7_uint32 NumofFans;
  HPC_FAN_DATA_t FanStatusData[L7_MAX_FANS_PER_UNIT];

  /* Power modules */
  L7_uint32 NumofPwMods;
  HPC_POWER_SUPPLY_DATA_t PwModStatusData[L7_MAX_PW_MODS_PER_UNIT];

  /* Temp. sensors */
  L7_uint32 NumofTempSensors;
  HPC_TEMP_SENSOR_DATA_t TempSensorStatusData[L7_MAX_TEMP_SENSORS_PER_UNIT];
  BOXS_TEMPERATURE_CROSS_RANGE_EVENT_t TempSensorLastReportedEvent[L7_MAX_TEMP_SENSORS_PER_UNIT];

  /* SFPs */
  L7_uint32 NumofSFPs;
  HPC_SFP_DATA_t sfpStatusData[L7_MAX_SFPS_PER_UNIT];

  /* XFPs */
  L7_uint32 NumofXFPs;
  HPC_XFP_DATA_t xfpStatusData[L7_MAX_XFPS_PER_UNIT];
} boxsData_t;


typedef struct
{
  boxsMsgHeader_t hdr;
  boxsData_t      boxsData;  
} boxsMsg_t;


#define BOXS_CLIENT_TIMEOUT 5
#define BOXS_MSG_SIZE sizeof(boxsMsg_t)

/* Begin Function Prototypes */
L7_RC_t boxsInitialize(void);
void boxsUninitialize(void);
L7_RC_t boxsHPCReceiveCallback(L7_enetMacAddr_t src_key,
                               boxsMsg_t *msg, L7_uint32 length);
void boxsReqTask(void);
void boxsRspTask(void);
void boxsGlobalStatusGet(boxsData_t *pBoxsData, L7_uint32 *global_status);
#endif

