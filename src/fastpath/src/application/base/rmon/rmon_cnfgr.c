/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snmp_rmon.c
*
* @purpose  includes the Initial EntryCreate calls for the RMON MIB groups
*           it also implements the callback method from NIM
*
* @component SNMP-RMON
*
*
* @create  08/06/2001
*
* @author  Kumar Manish
* @end
*
**********************************************************************/

#include "rmon_include.h"

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

/* RMON group header files */

#include "rmon_common.h"
#include "rmon_stat.h"
#include "rmon_history.h"
#include "rmon_alarm.h"
#include "rmon_event.h"
#include "rmon_index.h"

#include "nimapi.h"
#include "usmdb_nim_api.h"
#include "sysapi.h"
#include "rmon_exports.h"

/* Allocate memory for lag Table */
rmonLagEntry_t *rmonLagEntryArray;

/* Allocate memory for Ether Stat table */
rmonEtherStatsEntry_t *rmonEtherStatsEntryArray;

/* Allocate memory for HistoryControl table */
rmonHistoryControlEntry_t *rmonHistoryControlEntryArray;

/*Allocate Memory for Alarm Table */
rmonAlarmEntry_t *rmonAlarmEntryArray;

/* Allocate Memory for Event Table */
rmonEventEntry_t *rmonEventEntryArray;

L7_int32 L7_rmon_task_id;

/* Bit mask of NIM events that RMON is registered to receive */
PORTEVENT_MASK_t rmonNimEventMask;

/* Semaphore to protect the rmon tables.
*/
static void * rmon_sem = 0;

/* forward declaration */
static L7_RC_t rmonCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  to re-create entries for RMON groups
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rmonReInitializeTables()
{
  L7_uint32 intIfNum = 0;

  /* Recreate all RMON interfaces */
  while (usmDbGetNextVisibleIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
  {
      if (rmonIsValidIntf(intIfNum))
      {
          rmonIntfCreate(intIfNum);
      }
  }
}


/*********************************************************************
* @purpose  to create entries for RMON groups
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void rmonUnconfigureTables(){

    if (rmonLagArrayInitialize() != L7_SUCCESS)
        printf("Ether Lag Entry Unconfiguration Failed\n");
    if(rmonEtherStatsTableUnconfigure() != L7_SUCCESS)
        printf("Ether Stat Entry Unconfiguration Failed\n");
    if(rmonHistoryControlTableUnconfigure() != L7_SUCCESS)
        printf("History Control Entry Unconfiguration Failed\n");
    if(rmonAlarmTableUnconfigure() != L7_SUCCESS)
        printf("Polling For Alarm Cannot be Started\n");
    if(rmonEventTableUnconfigure() != L7_SUCCESS)
        printf("Event Entry Default Creation Failed\n");
}

/*********************************************************************
* @purpose  Determine whether RMON can be enabled on an interface of
*           a given type.
*
* @param    intfType  {(input}) interface type
*
* @returns  L7_TRUE if RMON can run on this interface
*
* @notes    RMON can be enabled on physical ports and LAGs
*
* @end
*********************************************************************/
L7_BOOL rmonIsValidIntfType(L7_INTF_TYPES_t intfType)
{
  return ((intfType == L7_PHYSICAL_INTF) ||
          (intfType == L7_LAG_INTF));
}

/*********************************************************************
* @purpose  Determine whether RMON can be enabled on a specific interface.
*
* @param    intIfNum  {(input}) Internal interface number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL rmonIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t intfType;

  return ((nimGetIntfType(intIfNum, &intfType) == L7_SUCCESS) &&
          rmonIsValidIntfType(intfType));
}

/*********************************************************************
* @purpose  Handle creation of an interface.
*
* @param    intIfNum  {(input}) Internal interface number
*
* @returns  L7_SUCCESS if creation handled successfully
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rmonIntfCreate(L7_uint32 intIfNum)
{
    L7_uint32 ifIndex = 1;
    L7_uint32 snmpIndex = 0;
    L7_uint32 i;
    L7_RC_t rc;
    L7_INTF_TYPES_t intfType;

    if (usmDbExtIfNumFromIntIfNum(intIfNum, &ifIndex) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    rc = osapiSemaTake(rmon_sem, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
        L7_LOG_ERROR (rc);
    }

    if (intfType == L7_LAG_INTF)
    {
        for (i = 1; i <= L7_MAX_NUM_LAG_INTF; i++)
        {
            if (rmonLagEntryArray[i].lagInterface == 0)
            {
                rmonLagEntryArray[i].lagInterface = intIfNum;
                rmonLagEntryArray[i].snmpIndexStat = 0;
                rmonLagEntryArray[i].snmpIndexHist1 = 0;
                rmonLagEntryArray[i].snmpIndexHist2 = 0;
                break;
            }
        }
    }
    else if (intfType == L7_PHYSICAL_INTF)
    {
        /* Set default values for the entries in EtherStat table */
        /* corresponding to the ifIndex and  make the status as valid */
        rmonEtherStatsEntryCreate(intIfNum, ifIndex, RMON_ETHER_STAT_STATUS_VALID,
                                  &snmpIndex);

        if (DEFAULT_HISTORY_TABLE_INITIALIZE() == L7_TRUE)
        {
            /* Set default values for the entries in HistoryControl table */
            /* corresponding to the ifIndex and  the control interval & make the status as valid */
            if (rmonHistoryControlEntryCreate(snmpIndex * 2 - 1, intIfNum, ifIndex,
                                              RMON_HISTORY_CONTROL_INTERVAL_ONE,
                                              RMON_HISTORY_CONTROL_STATUS_VALID)!= L7_SUCCESS)
            {
                osapiSemaGive (rmon_sem);
                return L7_FAILURE;
            }

            if (rmonHistoryControlEntryCreate(snmpIndex * 2, intIfNum, ifIndex,
                                              RMON_HISTORY_CONTROL_INTERVAL_TWO,
                                              RMON_HISTORY_CONTROL_STATUS_VALID)!= L7_SUCCESS)
            {
                osapiSemaGive (rmon_sem);
                return L7_FAILURE;
            }
        }
    }

    osapiSemaGive (rmon_sem);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle delete of an interface.
*
* @param    intIfNum  {(input}) Internal interface number
*
* @returns  L7_SUCCESS if delete handled successfully
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rmonIntfDelete(L7_uint32 intIfNum)
{
    L7_uint32 ifIndex = 1;
    L7_uint32 i;
    L7_RC_t rc;
    L7_INTF_TYPES_t intfType;

    /* convert internal interface number to external interface number(ifIndex) using USMDB call */
    if (usmDbExtIfNumFromIntIfNum(intIfNum, &ifIndex) != L7_SUCCESS)
        return L7_FAILURE;

    if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    rc = osapiSemaTake(rmon_sem, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
        L7_LOG_ERROR (rc);
    }

    if (intfType == L7_LAG_INTF)
    {
        /* zero out the lag array entry */
        for (i = 1; i <= L7_MAX_NUM_LAG_INTF; i++)
        {
            if (rmonLagEntryArray[i].lagInterface == intIfNum)
            {
                memset(&rmonLagEntryArray[i], 0, sizeof(rmonLagEntry_t));
                break;
            }
        }
    }

    /* Make the status of the entry in the Ether Stat table INVALID */
    rmonEtherStatsEntryDelete(ifIndex);

    /* Make the status of the entry in the History Control table INVALID */
    if (DEFAULT_HISTORY_TABLE_INITIALIZE() == L7_TRUE)
    {
        if (rmonHistoryControlEntryDelete(ifIndex,
                                          RMON_HISTORY_CONTROL_INTERVAL_ONE) != L7_SUCCESS)
        {
            osapiSemaGive(rmon_sem);
            return L7_FAILURE;
        }

        if (rmonHistoryControlEntryDelete(ifIndex,
                                          RMON_HISTORY_CONTROL_INTERVAL_TWO) != L7_SUCCESS)
        {
            osapiSemaGive(rmon_sem);
            return L7_FAILURE;
        }
    }

    osapiSemaGive(rmon_sem);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback registered with NIM for startup.
*
* @param    startupPhase - CREATE or ACTIVATE startup
*
* @returns void
*
* @comments  Takes no action on ACTIVATE startup.
*
* @end
*
*********************************************************************/
void rmonIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
    L7_uint32 intIfNum;
    L7_RC_t rc;

    if (startupPhase == NIM_INTERFACE_CREATE_STARTUP)
    {
        rc = nimFirstValidIntfNumber(&intIfNum);
        while (rc == L7_SUCCESS)
        {
          if (rmonIsValidIntf(intIfNum))
          {
              rmonIntfCreate(intIfNum);
          }
          rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
        }

        /* Now ask NIM to send any future changes for these event types */
        PORTEVENT_SETMASKBIT(rmonNimEventMask, L7_CREATE);
        PORTEVENT_SETMASKBIT(rmonNimEventMask, L7_DELETE);
        nimRegisterIntfEvents(L7_RMON_COMPONENT_ID, rmonNimEventMask);

        nimStartupEventDone(L7_RMON_COMPONENT_ID);
        return;
    }
    else if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
    {
        nimStartupEventDone(L7_RMON_COMPONENT_ID);
        return;
    }

    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RMON_COMPONENT_ID,
            "RMON NIM startup callback invoked with invalid phase %d",
            startupPhase);
}

/*********************************************************************
* @purpose  Process Link state changes and create / delete entries in Statistics
*           and History tables as per those changes
*
* @param    intIfnum    port changing state
* @param    event       new state
* @param    correlator  identifies interface event
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rmonIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,
                               NIM_CORRELATOR_t correlator)
{
    L7_RC_t rc = L7_SUCCESS;
    NIM_EVENT_COMPLETE_INFO_t status;

    if (rmonIsValidIntf(intIfNum))
    {
        switch (event)
        {
            case (L7_CREATE):
                rc = rmonIntfCreate(intIfNum);
                break;

            case (L7_DELETE):
                rc = rmonIntfDelete(intIfNum);
                break;

            default:
                /* All other NIM events are don't cares */
                break;
        }
    }

    status.intIfNum = intIfNum;
    status.component = L7_RMON_COMPONENT_ID;
    status.response.rc = L7_SUCCESS;
    status.response.reason = NIM_ERR_RC_UNUSED;
    status.event = event;
    status.correlator = correlator;
    nimEventStatusCallback(status);

    return rc;
}



/*********************************************************************
*
* @purpose  timer function, which sleeps for one second and calls the
*           callback function in a forever loop
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
rmonTimerCall()
{
  L7_RC_t rc;

  for (;;)
  {

    osapiSleep(1);

    /* TODO: check to see if USMDB is ready for access */

    rc = osapiSemaTake (rmon_sem, L7_WAIT_FOREVER);
    if (rc != L7_SUCCESS)
    {
     L7_LOG_ERROR (rc);
    }


    rmonLagTimerCallback();

    rmonHistoryTimerCallback();

    rmonAlarmTimerCallBack();

    osapiSemaGive (rmon_sem);
  }/* forever loop ends */
}

/*****************************************************************************************
*
* @purpose  Callback function for timer to perform check whether a lag is configured
*           or not, if it is configured push the entry in the corresponding index mapping
*           table for Stat and History groups, if lag is unconfigured(after being configured),
*           remove the entry from corresponding index mapping tables
*
* @param    void
*
* @returns  void

* @notes    none
*
* @end
*******************************************************************************************/
void
rmonLagTimerCallback()
{
    L7_uint32 countLag =1;
    L7_uint32 ifIndex;
    L7_uint32 lagSnmpIndexStat;
    L7_uint32 lagSnmpIndexHist1;
    L7_uint32 lagSnmpIndexHist2;
    rmonLagEntry_t* lge;


    for(countLag = 1; countLag <= L7_MAX_NUM_LAG_INTF; countLag++)
    {
        lge = &rmonLagEntryArray[countLag];

        /* convert internal interface number to external interface number(ifIndex) using USMDB call */
        if(lge->lagInterface > 0 && usmDbExtIfNumFromIntIfNum(lge->lagInterface,&ifIndex)==L7_SUCCESS)
        {
            /* Determine if this **external** interface number, or ifIndex to be displayed exists,
            i.e., if the lag is configured , insert an entry for this interface in the corresponding
            RMON table */
            if((lge->snmpIndexStat == 0) &&
                         (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) == L7_SUCCESS))
            {
                if(rmonEtherStatsTableInsert(lge->lagInterface,ifIndex, &lagSnmpIndexStat) == L7_SUCCESS)
                    lge->snmpIndexStat = lagSnmpIndexStat;
            }
        if (DEFAULT_HISTORY_TABLE_INITIALIZE() == L7_TRUE)
        {
              if((lge->snmpIndexHist1 == 0) &&
                           (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) == L7_SUCCESS))
              {

                  if(rmonHistoryControlTableInsert(lge->lagInterface,ifIndex,
                                      RMON_HISTORY_CONTROL_INTERVAL_ONE, &lagSnmpIndexHist1) == L7_SUCCESS)
                      lge->snmpIndexHist1 =  lagSnmpIndexHist1;
              }

              if((lge->snmpIndexHist2 == 0) &&
                           (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) == L7_SUCCESS))
              {

                  if(rmonHistoryControlTableInsert(lge->lagInterface,ifIndex,
                                      RMON_HISTORY_CONTROL_INTERVAL_TWO, &lagSnmpIndexHist2) == L7_SUCCESS)
                      lge->snmpIndexHist2 =  lagSnmpIndexHist2;
              }
        }

            /* Determine if this **external** interface number, or ifIndex to be displayed doesn't exist,
            i.e., if the lag is unconfigured(after being configured , remove the entry for this
            interface from the corresponding RMON table */

            if((lge->snmpIndexStat != 0) &&
                         (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) != L7_SUCCESS))
            {
                if(rmonEtherStatsTableRemove(lge->snmpIndexStat) == L7_SUCCESS)
                    lge->snmpIndexStat = 0;

            }

        if (DEFAULT_HISTORY_TABLE_INITIALIZE() == L7_TRUE)
        {
              if((lge->snmpIndexHist1 != 0) &&
                          (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) != L7_SUCCESS))
              {
                  if(rmonHistoryControlTableRemove(lge->snmpIndexHist1) == L7_SUCCESS)
                      lge->snmpIndexHist1 = 0;
              }

              if((lge->snmpIndexHist2 != 0) &&
                          (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,ifIndex) != L7_SUCCESS))
              {
                  if(rmonHistoryControlTableRemove(lge->snmpIndexHist2) == L7_SUCCESS)
                      lge->snmpIndexHist2 = 0;

              }
        }

        }

    }/* end of for loop */
    return;
}


/*********************************************************************
*
* @purpose  Callback function for timer to perform polling activities
*           for Alarm  and raising alarm accordingly
*
* @param    void
*
* @returntype void
*
* @notes      none
*
* @end
*********************************************************************/
void
rmonAlarmTimerCallBack()
{
    L7_uint32 countAlarm = 0;
    L7_uint32 delta;
    L7_uint32 value;
    rmonAlarmEntry_t* alm;

    for(countAlarm = 0; countAlarm < RMON_ALARM_ENTRY_MAX_NUM; countAlarm++)
    {
        alm = &rmonAlarmEntryArray[countAlarm];

        if(alm->status == RMON_ALARM_ENTRY_STATUS_VALID)
        {
            alm->alarmTimerCount++;
            if(alm->sampleType == RMON_ALARM_ENTRY_SAMPLETYPE_ABSOLUTE)
            {
                if(alm->alarmTimerCount == alm->interval)
                {
                    alm->alarmTimerCount = 0;
                    if(rmonAlarmSampledValueGet(alm->variable, &value) != L7_SUCCESS)
                    {
                        rmonEventEntryDelete(alm->risingEventIndex);
                        rmonEventEntryDelete(alm->fallingEventIndex);
                        rmonAlarmEntryDelete(alm->index);
                        continue;
                    }
                    alm->value = value;
                    if(rmonAlarmStart(alm) != L7_SUCCESS)
                        continue;
                }
            }
            if(alm->sampleType == RMON_ALARM_ENTRY_SAMPLETYPE_DELTA)
            {
                /* check every half-interval.  If interval == 1, it will not check every half-interval*/
                if(alm->alarmTimerCount == (alm->interval/2) ||
                   alm->alarmTimerCount >= alm->interval)
                {
                    if (alm->alarmTimerCount >= alm->interval)
                    {
                      /* reset timer after full interval */
                      alm->alarmTimerCount = 0;
                    }

                    if(rmonAlarmSampledValueGet(alm->variable, &value) != L7_SUCCESS)
                    {
                        rmonEventEntryDelete(alm->risingEventIndex);
                        rmonEventEntryDelete(alm->fallingEventIndex);
                        rmonAlarmEntryDelete(alm->index);
                        continue;
                    }

                    /* find new interval delta */
                    if (alm->interval == 1)
                    {
                        /* store last full interval

                        alm->prevValue gets the polled value of variable
                        alm->prevDeltaValue is not used
                        alm->value gets the delta over the last interval
                        */

                        alm->value = value - alm->prevValue;
                        alm->prevValue = value;
                    }
                    else
                    {
                        /* store last two half intervals

                        alm->prevValue gets the value of variable at the last half-interval
                        alm->prevDeltaValue gets the polled value of variable
                        alm->value gets the delta over the last full interval
                        */

                        delta = value - alm->prevValue;
                        alm->prevValue = alm->prevDeltaValue;
                        alm->prevDeltaValue = value;
                        alm->value = delta;
                    }

                    rmonAlarmStart(alm);
                }
            }
        }
    }
    return;
}



/********************************************************
** Test Code for hot-pluggability.
*********************************************************/
void rmonDeleteTest (void)
{
    L7_uint32 intIfNum1 = 1;
    L7_uint32 intIfNum2 = 25;
    L7_uint32 intIfNum3 = 52;
    L7_uint32 intIfNum4 = 60;
    rmonIntfDelete(intIfNum1);
    rmonIntfDelete(intIfNum1);
    rmonIntfDelete(intIfNum2);
    rmonIntfDelete(intIfNum3);
    rmonIntfDelete(intIfNum4);
}

/*********************************************************************
*
* @purpose    Initialize the RMON Lag Array table.
*
* @returntype Always L7_SUCCESS
*
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t rmonLagArrayInitialize(void)
{
    L7_uint32 i;
    rmonLagEntry_t* lge;

    for(i = 1; i <= L7_MAX_NUM_LAG_INTF; i++)
    {
        lge = &rmonLagEntryArray[i];
        memset (lge, 0, sizeof (rmonLagEntry_t));
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.
*
* @returns  None
*
* @notes    This function completes synchronously. The return value
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
void rmonApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = 0;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             rmonRC    = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((rmonRC = rmonPhaseOneInit( &response, &reason )) != L7_SUCCESS)
                  rmonPhaseOneFini();
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rmonRC = rmonPhaseTwoInit( &response, &reason )) != L7_SUCCESS)
                  rmonPhaseTwoFini();
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                rmonRC = rmonPhaseThreeInit( &response, &reason );
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rmonRC = rmonCnfgrNoopProccess( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                rmonRC = rmonPhaseOneUnConfig( &response, &reason );
                break;
              case L7_CNFGR_RQST_U_PHASE2_START:
                rmonRC = rmonPhaseTwoUnConfig( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rmonRC = rmonCnfgrNoopProccess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rmonRC;

  if (rmonRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}


/*********************************************************************
* @purpose Initialize RMON for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseOneInit()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32  data_size;

/* Allocate memory for lag Table */
  data_size = sizeof (rmonLagEntry_t) * (L7_MAX_NUM_LAG_INTF + 1);
  rmonLagEntryArray = osapiMalloc (L7_RMON_COMPONENT_ID, data_size);
  if (rmonLagEntryArray == 0)
  {
    L7_LOG_ERROR (0);
  }
  memset (rmonLagEntryArray, 0, data_size);


/* Allocate memory for Ether Stat table */
  data_size = sizeof (rmonEtherStatsEntry_t) * (RMON_ETHER_STAT_ENTRY_MAX_NUM + 1);
  rmonEtherStatsEntryArray = osapiMalloc (L7_RMON_COMPONENT_ID, data_size);
  if (rmonEtherStatsEntryArray == 0)
  {
    L7_LOG_ERROR (0);
  }
  memset (rmonEtherStatsEntryArray, 0, data_size);


/* Allocate memory for HistoryControl table */
  data_size = sizeof (rmonHistoryControlEntry_t) * (RMON_HISTORY_CONTROL_ENTRY_MAX_NUM + 1);
  rmonHistoryControlEntryArray = osapiMalloc (L7_RMON_COMPONENT_ID, data_size);
  if (rmonHistoryControlEntryArray == 0)
  {
    L7_LOG_ERROR (0);
  }
  memset (rmonHistoryControlEntryArray, 0, data_size);


/*Allocate Memory for Alarm Table */
  data_size = sizeof (rmonAlarmEntry_t) * (RMON_ALARM_ENTRY_MAX_NUM + 1);
  rmonAlarmEntryArray = osapiMalloc (L7_RMON_COMPONENT_ID, data_size);
  if (rmonAlarmEntryArray == 0)
  {
    L7_LOG_ERROR (0);
  }
  memset (rmonAlarmEntryArray, 0, data_size);


/* Allocate Memory for Event Table */
  data_size = sizeof (rmonEventEntry_t) * (RMON_EVENT_ENTRY_MAX_NUM + 1);
  rmonEventEntryArray = osapiMalloc (L7_RMON_COMPONENT_ID, data_size);
  if (rmonEventEntryArray == 0)
  {
    L7_LOG_ERROR (0);
  }
  memset (rmonEventEntryArray, 0, data_size);

  rmon_sem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if(rmonLagArrayInitialize() != L7_SUCCESS)
      sysapiPrintf("Ether Lag Entry Default Creation Failed\n");
  if(rmonEtherStatsTableInitialize() != L7_SUCCESS)
      sysapiPrintf("Ether Stat Entry Default Creation Failed\n");
  if(rmonHistoryControlTableInitialize() != L7_SUCCESS)
      sysapiPrintf("History Control Entry Default Creation Failed\n");
  if(rmonAlarmTableInitialize() != L7_SUCCESS)
      sysapiPrintf("Polling For Alarm Cannot be Started\n");
  if(rmonEventTableInitialize() != L7_SUCCESS)
      sysapiPrintf("Event Entry Default Creation Failed\n");

  L7_rmon_task_id = osapiTaskCreate("RMONTask", rmonTimerCall, 0, 0,
                                     L7_DEFAULT_STACK_SIZE,
                                     L7_DEFAULT_TASK_PRIORITY,
                                     L7_DEFAULT_TASK_SLICE);


  if (L7_rmon_task_id == L7_ERROR)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RMON_COMPONENT_ID,
            "Failed to Create RMON Task.\n");

  return(rc);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rmonPhaseOneFini(void)
{
  if (L7_rmon_task_id != L7_ERROR)
  {
     osapiTaskDelete(L7_rmon_task_id);
  }

  if (rmonLagEntryArray != L7_NULLPTR)
  {
    memset (rmonLagEntryArray, 0, (sizeof (rmonLagEntry_t) * (L7_MAX_NUM_LAG_INTF + 1) ));
    osapiFree(L7_RMON_COMPONENT_ID, rmonLagEntryArray);
  }

  if (rmonEtherStatsEntryArray != L7_NULLPTR)
  {
    memset (rmonEtherStatsEntryArray, 0, (sizeof (rmonEtherStatsEntry_t) * (RMON_ETHER_STAT_ENTRY_MAX_NUM + 1)));
    osapiFree(L7_RMON_COMPONENT_ID, rmonEtherStatsEntryArray);
  }

  if (rmonAlarmEntryArray != L7_NULLPTR)
  {
    memset (rmonAlarmEntryArray, 0, (sizeof (rmonAlarmEntry_t) * (RMON_ALARM_ENTRY_MAX_NUM + 1)));
    osapiFree(L7_RMON_COMPONENT_ID, rmonAlarmEntryArray);
  }

  if (rmonHistoryControlEntryArray != L7_NULLPTR)
  {
    memset (rmonHistoryControlEntryArray, 0, (sizeof (rmonHistoryControlEntry_t) * (RMON_HISTORY_CONTROL_ENTRY_MAX_NUM + 1)));
    osapiFree(L7_RMON_COMPONENT_ID, rmonHistoryControlEntryArray);
  }

  if (rmonEventEntryArray != L7_NULLPTR)
  {
    memset (rmonEventEntryArray, 0, (sizeof (rmonEventEntry_t) * (RMON_EVENT_ENTRY_MAX_NUM + 1) ));
    osapiFree(L7_RMON_COMPONENT_ID, rmonEventEntryArray);
  }

  if (rmon_sem != L7_NULLPTR)
  {
    osapiSemaDelete(rmon_sem);
  }
}

/*********************************************************************
* @purpose Initialize RMON for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseTwoInit()
{
  if (nimRegisterIntfChange(L7_RMON_COMPONENT_ID, rmonIntfChangeCallback,
                            rmonIntfStartupCallback,
                            NIM_STARTUP_PRIO_DEFAULT) != L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RMON_COMPONENT_ID,
             "RMON failed to register for interface events.");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void rmonPhaseTwoFini(void)
{
  nimDeRegisterIntfChange(L7_RMON_COMPONENT_ID);
}

/*********************************************************************
* @purpose Initialize RMON for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseThreeInit()
{
  L7_RC_t rc = L7_SUCCESS;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&rmonNimEventMask, 0, sizeof(rmonNimEventMask));
  nimRegisterIntfEvents(L7_RMON_COMPONENT_ID, rmonNimEventMask);

  return(rc);
}


/*********************************************************************
* @purpose Unconfigure RMON for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseOneUnConfig()
{
  /* nothing to do */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Unconfigure RMON for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired.
*
* @end
*********************************************************************/
L7_RC_t rmonPhaseTwoUnConfig()
{
   /* clear RMON configuration */
  rmonUnconfigureTables();
  /* reapply rmon configuration */
  rmonReInitializeTables();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
static L7_RC_t rmonCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}

/* End Function Definitions */
