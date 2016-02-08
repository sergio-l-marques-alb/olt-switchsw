/*
 * ptinHapi_control.h
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 */

#ifndef PTINHAPI_CONTROL_H_
#define PTINHAPI_CONTROL_H_

#define PTINHAPI_MGMT_EVENTS_BEGIN 0

typedef enum ptinHapiMgmtEvents_s
{
  /***************************************************************/
  /* Events shared with all                                      */
  /***************************************************************/
  ptinHapiMgmtBegin = PTINHAPI_MGMT_EVENTS_BEGIN,

  /***************************************************************/
  /* Events from configurator                                    */
  /***************************************************************/
  ptinHapiCnfgr,

  ptinHapiCnfgrEvents

}ptinHapiMgmtEvents_t;


typedef struct
{
  L7_uint32 event;
  L7_uint32 intf;
  L7_uint32 instance;
  L7_uint32 timeStamp;
  union
  {
   L7_CNFGR_CMD_DATA_t CmdData;
   NIM_EVENT_COMPLETE_INFO_t status;
  }data;
}PTINHAPI_MSG_t;

#endif /* PTINHAPI_CONTROL_H_ */
