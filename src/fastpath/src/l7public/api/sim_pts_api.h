#ifndef __SIM_PTS_API_H__
#define __SIM_PTS_API_H__

#include "sysapi.h"

typedef void (*SIM_PTS_CB_FUNC_t)(L7_uint32 portIndex);

typedef struct
{
  L7_BOOL valid;
  L7_COMPONENT_IDS_t compId;
  L7_uint32 priority;
  SIM_PTS_CB_FUNC_t ptsTimerCallback;

} REG_USERS_t;

typedef struct
{
  L7_BOOL valid;
  L7_uint32 origTimeout;
  L7_uint32 currTime;
} timer_block_t;

extern L7_uint32 debugTimerHelp;
#define SIM_PTS_PRINT(args...) if(debugTimerHelp == L7_TRUE) SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args)

#define PTS_MAX_USERS 2

#define PTS_MAX_PORTS (L7_MAX_PHYSICAL_PORTS_PER_UNIT + L7_MAX_NUM_LAG_INTF + 1)

/* value in ms*/
#define PTS_TIMER_TICK 250 

typedef timer_block_t regComps_t[PTS_MAX_USERS];

L7_RC_t simPtsInit();
L7_RC_t simPtsTimerStop(L7_uint32 userHandle, L7_uint32 portIndex);
L7_RC_t simPtsTimerStart(L7_uint32 userHandle, L7_uint32 portIndex, L7_uint32 interval);
L7_RC_t simPtsTimerModify(L7_uint32 userHandle, L7_uint32 portIndex, L7_uint32 interval);

L7_RC_t simPtsRegister(L7_COMPONENT_IDS_t comp, L7_uint32 prio,
                             SIM_PTS_CB_FUNC_t callbackFunc,
                             L7_uint32 *userHandle);

#endif /* __SIM_PTS_API_H__*/
