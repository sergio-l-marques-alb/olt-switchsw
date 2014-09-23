/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/qos/cos/cli_show_cos.c
*
* @purpose Class of Service show commands for the cli
*
* @component user interface
*
* @comments
*
* @create  04/13/2004
*
* @author  kmans
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "cos_exports.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_cos_api.h"
#endif

#include <datatypes.h>
#include "default_cnfgr.h"
#include "clicommands_card.h"
#include "clicommands_cos.h"
#include "cliutil.h"
#include "ews.h"

#include "l7_cos_api.h"
/* GLOBALS */

/*********************************************************************
*
* @purpose  display Traffic Class summary information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show interfaces cos-queue [<slot/port>]
*
* @cmdhelp Display Traffic Class Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/
const L7_char8 *commandShowInterfacesCosQueue(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val;
   L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  title[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  line[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 interface = 0;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_uint32 argSlotPort=1;
   L7_uint32 unit=0;
   L7_uint32 itype, i, numArgs;
   L7_QOS_COS_QUEUE_MGMT_TYPE_t mgmtType;
   L7_qosCosQueueBwList_t queueBwMinList, queueBwMaxList;
   L7_qosCosQueueSchedTypeList_t queueSchedTypeList;
   L7_qosCosQueueMgmtTypeList_t queueMgmtTypeList;
   L7_BOOL minSuccess = L7_FALSE;
   L7_BOOL maxSuccess = L7_FALSE;
   L7_BOOL schedulerTypeSuccess = L7_FALSE;
   L7_BOOL mgmtTypeSuccess = L7_FALSE;

   memset(&queueBwMinList, 0, sizeof(queueBwMinList));
   memset(&queueBwMaxList, 0, sizeof(queueBwMaxList));
   memset(&queueMgmtTypeList, 0, sizeof(queueMgmtTypeList));
   memset(&queueSchedTypeList, 0, sizeof(queueSchedTypeList));
   cliSyntaxTop(ewsContext);

   numArgs = cliNumFunctionArgsGet();

   if (numArgs == 0)
   {
      interface = L7_ALL_INTERFACES;
   }
   else
   {

      if (cliIsStackingSupported() == L7_TRUE)
      {
        rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

        if (rc == L7_FAILURE)
        {
        sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    rc = usmDbIntfTypeGet(interface, &itype);

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetIntf);
    }

    /* check if interface is supported for COS queueing */
    if (usmDbQosCosQueueIntfIndexGet(unit, interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
    }

    /* First pass through, interface has already been verified */

  }
  if (interface == L7_ALL_INTERFACES)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_qos_GlobalCfg);
  }
  else
  {
    if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_common_Intf);
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      osapiStrncpySafe(buf, cliDisplayInterfaceHelp(val, intSlot, intPort), sizeof(buf));
      ewsTelnetPrintf (ewsContext, "%-13.11s",buf);
      }
   }

   /* Shaping Rate */
   if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE)
   {
      if (usmDbQosCosQueueIntfShapingRateGet(unit, interface, &val) == L7_SUCCESS)
      {
      cliFormat(ewsContext,pStrInfo_qos_IntfShapingRate);
      ewsTelnetPrintf (ewsContext, "%-d", val);
      }
   }

   /* Management Type */
   if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
   {
      if (usmDbQosCosQueueMgmtTypePerIntfGet(unit, interface, &mgmtType) == L7_SUCCESS)
      {
      cliFormat(ewsContext,pStrInfo_qos_QueueMgmtType);
      memset (stat, 0,sizeof(stat));
         if (mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP)
         {
        sprintf(stat, "%-s", pStrInfo_qos_TailDrop);
         }
         else if (mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_WRED)
         {
        sprintf(stat, "%-s", pStrInfo_qos_WeightedRandomEarlyDiscardWred);
         }

         ewsTelnetWrite(ewsContext,stat);
      }
   }

   /* WRED Decay */
   if (usmDbQosCosQueueWredDecayExponentGet(unit, interface, &val) == L7_SUCCESS)
   {
    cliFormat(ewsContext,pStrInfo_qos_WredDecayExponent);
    ewsTelnetPrintf (ewsContext, "%-d", val);
  }

  memset (title, 0,sizeof(title));
  memset (line, 0,sizeof(line));
   if ((interface == L7_ALL_INTERFACES) ||
       (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE))
   {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE)
      {
         if (usmDbQosCosQueueMinBandwidthListGet(unit, interface, &queueBwMinList) == L7_SUCCESS)
         {
        strcatAddBlanks (0, 0, 0, 3, L7_NULLPTR, title, pStrInfo_qos_MinBandwidth);
            strcat(line,  "--------------   ");
            minSuccess = L7_TRUE;
         }
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE)
      {
         if (usmDbQosCosQueueMaxBandwidthListGet(unit, interface, &queueBwMaxList) == L7_SUCCESS)
         {
        strcatAddBlanks (0, 0, 0, 3, L7_NULLPTR, title, pStrInfo_qos_MaxBandwidth);
            strcat(line,  "--------------   ");
            maxSuccess = L7_TRUE;
         }
      }

      if (usmDbQosCosQueueSchedulerTypeListGet(unit, interface, &queueSchedTypeList) == L7_SUCCESS)
      {
      strcatAddBlanks (0, 0, 0, 3, L7_NULLPTR, title, pStrInfo_qos_SchedulerType);
         strcat(line,  "--------------   ");
         schedulerTypeSuccess = L7_TRUE;
      }

      /* NOTE:  Display the following UNLESS mgmt type configurable only at the interface level */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE)
      {
         if (usmDbQosCosQueueMgmtTypeListGet(unit, interface, &queueMgmtTypeList) == L7_SUCCESS)
         {
        strcatAddBlanks (0, 0, 0, 3, L7_NULLPTR, title, pStrInfo_qos_QueueMgmtType);
            strcat(line,  "---------------------   ");
            mgmtTypeSuccess = L7_TRUE;
         }
      }

    ewsTelnetWriteAddBlanks (2, 0, 0, 3, L7_NULLPTR, ewsContext,pStrInfo_qos_QueueId_1);
      ewsTelnetWrite(ewsContext, title);

      ewsTelnetWrite(ewsContext,"\r\n--------   ");
      ewsTelnetWrite(ewsContext, line);
      for (i = 0 ; i < L7_MAX_CFG_QUEUES_PER_PORT ; i++)
      {
         if (usmDbQosCosQueueIdIndexGet(unit, i) == L7_SUCCESS)
         {
        ewsTelnetPrintf (ewsContext, "\r\n%-11u", i);

            /* MIN Bandwidth */
            if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE) && (minSuccess == L7_TRUE))
            {
          ewsTelnetPrintf (ewsContext, "%-17d", queueBwMinList.bandwidth[i]);
            }

            /* MAX Bandwidth */
            if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE) && (maxSuccess == L7_TRUE))
            {
          ewsTelnetPrintf (ewsContext, "%-17d", queueBwMaxList.bandwidth[i]);
            }

            /* Scheduler Type */
            if (schedulerTypeSuccess == L7_TRUE)
            {
               if (queueSchedTypeList.schedType[i] == L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT)
          {
            sprintf(stat, "%-17s", pStrInfo_qos_Strict);
          }
               else if (queueSchedTypeList.schedType[i] == L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED)
          {
            sprintf(stat, "%-17s", pStrInfo_qos_Weighted);
          }
          else
          {
            sprintf(stat, "%-17.7s", pStrInfo_common_NotApplicable);
          }
               ewsTelnetWrite(ewsContext,stat);
            }

            /* Queue Mgmt Type */
            if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE) && (mgmtTypeSuccess == L7_TRUE))
            {
               if (queueMgmtTypeList.mgmtType[i] == L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP)
          {
            sprintf(stat, "%-24s", pStrInfo_qos_TailDrop);
          }
               else if (queueMgmtTypeList.mgmtType[i] == L7_QOS_COS_QUEUE_MGMT_TYPE_WRED)
          {
            sprintf(stat, "%-24s", pStrInfo_qos_Wred);
          }
          else
          {
                  sprintf(stat, "%-24.10s", "---");
          }
          ewsTelnetWrite(ewsContext,stat);
        }

      }
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display Traffic Class summary information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show interfaces random-detect [<slot/port>]
*
* @cmdhelp Display Traffic Class Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/
const L7_char8 *commandShowInterfacesRandomDetect(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val, queueId, j;
   L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8             tmpVal[L7_MAX_CFG_DROP_PREC_LEVELS+1];
   L7_uchar8             tmpStr[((L7_MAX_CFG_DROP_PREC_LEVELS+1)*4)+1];
   L7_char8  title1[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  title2[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  line[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 interface = 0;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_uint32 argSlotPort=1;
   L7_uint32 unit=0;
   L7_uint32 itype, numArgs;
   L7_qosCosDropParmsList_t dropParmsList;
   L7_BOOL maxSuccess = L7_FALSE;

   cliSyntaxTop(ewsContext);

   numArgs = cliNumFunctionArgsGet();

   if (numArgs == 0)
   {
      interface = L7_ALL_INTERFACES;
   }
   else
   {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);
        if (rc == L7_FAILURE)
        {
          sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
          return cliSyntaxReturnPrompt (ewsContext, buf);
        }  
        else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
        }
      }
      else
      {
        if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      } 

      if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      rc = usmDbIntfTypeGet(interface, &itype);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetIntf);
      }

      /* check if interface is supported for COS queueing */
      if (usmDbQosCosQueueIntfIndexGet(unit, interface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
      }
   }
   if (interface == L7_ALL_INTERFACES)
   {
     ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_qos_GlobalCfg);
   }
   else
   {
     if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
     {
       cliFormat(ewsContext,pStrInfo_common_Intf);
       memset (buf, 0,sizeof(buf));
       memset (stat, 0,sizeof(stat));
       osapiStrncpySafe(buf, cliDisplayInterfaceHelp(val, intSlot, intPort), sizeof(buf));
       ewsTelnetPrintf (ewsContext, "%-13.11s",buf);
     }
   } 

   if (usmDbQosCosQueueDropParmsListGet(unit, interface, &dropParmsList) != L7_SUCCESS)
   {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetWredParams);
   }

   memset (title1, 0,sizeof(title1));
   memset (title2, 0,sizeof(title2));
   memset (line, 0,sizeof(line));
   /* if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
   {
       if (usmDbQosCosQueueDropPrecIndexGet(unit, val) == L7_SUCCESS)
      {
         strcat(title1, "     Drop        ");
         strcat(title2, "Precedence Level ");
         strcat(line,   "---------------- ");
         dropPrecSuccess = L7_TRUE;
      }
   }  */

   strcatAddBlanks (0, 0, 6, 8, L7_NULLPTR, title1, pStrInfo_qos_Wred);
   strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, title2, pStrInfo_qos_MinThresh_2);
   strcat(line,   "----------------- ");

   if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) == L7_TRUE)
   {
     strcatAddBlanks (0, 0, 5, 9, L7_NULLPTR, title1, pStrInfo_qos_Wred);
     strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, title2, pStrInfo_qos_MaxThresh_1);
     strcat(line,   "----------------- ");
     maxSuccess = L7_TRUE;
   }

   strcatAddBlanks (0, 0, 4, 4, L7_NULLPTR, title1, pStrInfo_qos_Wred);
   osapiStrncat(title2, pStrInfo_qos_DropProbability, sizeof(title2));
   strcat(line,   "-----------------");

   ewsTelnetWrite(ewsContext,"\r\n\r\n         ");
   ewsTelnetWrite(ewsContext, title1);
   ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_qos_QueueId_1);
   ewsTelnetWrite(ewsContext, title2);

   ewsTelnetWrite(ewsContext,"\r\n-------- ");
   ewsTelnetWrite(ewsContext, line);

   for (queueId = 0 ; queueId < L7_MAX_CFG_QUEUES_PER_PORT ; queueId++)
   {
      if (usmDbQosCosQueueIdIndexGet(unit, queueId) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-9u", queueId);
        /* WRED Minimum Threshold */
        memset (stat, 0,sizeof(stat));
        memset(tmpVal, 0, sizeof(tmpVal));
        memset(tmpStr, 0, sizeof(tmpStr));
        for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
        {
          tmpVal[j] = dropParmsList.queue[queueId].minThreshold[j];
        }
        cliCosQueueThreshStringFormat(tmpVal, tmpStr);
        ewsTelnetPrintf (ewsContext, "%-18s", tmpStr);

        if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) == L7_TRUE) &&
            (maxSuccess == L7_TRUE))
        {
          /* WRED Maximum Threshold */
          memset (stat, 0,sizeof(stat));
          memset(tmpVal, 0, sizeof(tmpVal));
          memset(tmpStr, 0, sizeof(tmpStr));
          for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
          {
            tmpVal[j] = dropParmsList.queue[queueId].wredMaxThreshold[j];
          }
          cliCosQueueThreshStringFormat(tmpVal, tmpStr);
          ewsTelnetPrintf (ewsContext, "%-18s", tmpStr);
        }

        /* WRED Drop Probability */
        memset (stat, 0,sizeof(stat));
        memset(tmpVal, 0, sizeof(tmpVal));
        memset(tmpStr, 0, sizeof(tmpStr));
        for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
        {
          tmpVal[j] = dropParmsList.queue[queueId].dropProb[j];
        }
        cliCosQueueThreshStringFormat(tmpVal, tmpStr);
        ewsTelnetPrintf (ewsContext, "%-18s", tmpStr);
      }
   }

   return cliSyntaxReturnPrompt (ewsContext, "");
}
/*********************************************************************
*
* @purpose  display Traffic Class summary information
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show interfaces tail-drop-threshold [<slot/port>]
*
* @cmdhelp Display Traffic Class Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/

const L7_char8 *commandShowInterfacesTailDropThreshold(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 val, queueId, j;
   L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  title[L7_CLI_MAX_STRING_LENGTH];
   L7_char8  line[L7_CLI_MAX_STRING_LENGTH];
   L7_uchar8             tmpVal[L7_MAX_CFG_DROP_PREC_LEVELS + 1];
   L7_uchar8             tmpStr[(L7_MAX_CFG_DROP_PREC_LEVELS*3)+1];
   L7_uint32 interface = 0;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_uint32 argSlotPort=1;
   L7_uint32 unit=0;
   L7_uint32 itype, numArgs;
   L7_qosCosDropParmsList_t dropParmsList;

   cliSyntaxTop(ewsContext);

   numArgs = cliNumFunctionArgsGet();

   if (numArgs == 0)
   {
      interface = L7_ALL_INTERFACES;
      unit = cliGetUnitId();
   }
   else
   {
      if (cliIsStackingSupported() == L7_TRUE)
{
  rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

  if (rc == L7_FAILURE)
  {
        sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    rc = usmDbIntfTypeGet(interface, &itype);

    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetIntf);
    }

    /* check if interface is supported for COS queueing */
    if (usmDbQosCosQueueIntfIndexGet(unit, interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
    }

    /* First pass through, interface has already been verified */
  }
  if (interface == L7_ALL_INTERFACES)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_qos_GlobalCfg);
  }
  else
  {
    if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_common_Intf);
      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      osapiStrncpySafe(buf, cliDisplayInterfaceHelp(val, intSlot, intPort), sizeof(buf));
      ewsTelnetPrintf (ewsContext, "%-13.11s",buf);
    }
  }

  if (usmDbQosCosQueueDropParmsListGet(unit, interface, &dropParmsList) != L7_SUCCESS)
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetTdropParams);
  }
  memset (title, 0,sizeof(title));
  memset (line, 0,sizeof(line));
  strcatAddBlanks (0, 0, 0, 2, L7_NULLPTR, title, pStrInfo_qos_TailDropThresh_1);
   strcat(line,  "-------------------  ");

  ewsTelnetWriteAddBlanks (2, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_qos_QueueId_1);
   ewsTelnetWrite(ewsContext, title);

   ewsTelnetWrite(ewsContext,"\r\n--------  ");
   ewsTelnetWrite(ewsContext, line);

   for (queueId = 0 ; queueId < L7_MAX_CFG_QUEUES_PER_PORT ; queueId++)
   {
      if (usmDbQosCosQueueIdIndexGet(unit, queueId) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "\r\n%-10u", queueId);

        /* Tail Drop Threshold */
            
        memset(tmpVal, 0, sizeof(tmpVal));
        memset(tmpStr, 0, sizeof(tmpStr));
        for (j = 0; j < (L7_MAX_CFG_DROP_PREC_LEVELS+1); j++)
        {
          tmpVal[j] = dropParmsList.queue[queueId].tailDropMaxThreshold[j];
        }
        cliCosQueueThreshStringFormat(tmpVal, tmpStr);
        ewsTelnetPrintf (ewsContext, "%-21s", tmpStr);
      }
   } /* for */

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display Class of Service IP DSCP Mapping
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show interface cos-queue [<slot/port>]
*
* @cmdhelp Display Traffic Class Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClassofServiceIpDscpMapping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 dscp, nextDscp, trafficClass;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_char8 dscpString[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit, interface, rc;
   L7_uint32 argSlotPort=1;
   L7_uint32 intSlot;
   L7_uint32 intPort;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   /* determine if displaying interface config or global config parms */
   if (cliNumFunctionArgsGet() == 0)
   {
      /* displaying global config values */
      interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
   }
   else
   {
      /* displaying interface config values (but only if intf feature is supported) */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowClassOfServiceIpDscpMapPing);
    }

    if (cliNumFunctionArgsGet() != 1)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_ShowClassOfServiceIpDscpMapPingPort, cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    if (cliIsStackingSupported() == L7_TRUE)
    {
      rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

      if (rc == L7_FAILURE)
      {
        sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      }

      /* check if interface is supported for COS mapping */
      if (usmDbQosCosMapIpDscpIntfIndexGet(unit, interface) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 4, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_IpDscpTrafficClass);
   ewsTelnetWrite(ewsContext, "\r\n -------------    -------------");

   dscp = 0;
   nextDscp = 0;
   if (usmDbQosCosMapIpDscpIndexGet(unit, dscp) == L7_SUCCESS)
   {
      do
      {
         dscp = nextDscp;
         if (usmDbQosCosMapIpDscpTrafficClassGet(unit, interface, dscp, &trafficClass) != L7_SUCCESS)
      {
        break;
      }
      memset (buf, 0, sizeof(buf));
      memset (dscpString, 0, sizeof(dscpString));
         cliDiffservConvertDSCPValToString(dscp, dscpString);
      ewsTelnetPrintf (ewsContext, "\r\n   %-13.12s %7d", dscpString, trafficClass);
      } while (usmDbQosCosMapIpDscpIndexGetNext(unit, dscp, &nextDscp) == L7_SUCCESS);
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display Class of Service IP Precedence Mapping
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show interface cos-queue [<slot/port>]
*
* @cmdhelp Display Traffic Class Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClassofServiceIpPrecedenceMapping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 precedence, trafficClass;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit, interface, rc;
   L7_uint32 argSlotPort=1;
   L7_uint32 intSlot;
   L7_uint32 intPort;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   /* determine if displaying interface config or global config parms */
   if (cliNumFunctionArgsGet() == 0)
   {
      /* displaying global config values */
      interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
   }
   else
   {
      /* displaying interface config values (but only if intf feature is supported) */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_ShowClassOfServiceIpPrecedenceMapPing);
    }

    if (cliNumFunctionArgsGet() != 1)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_ShowClassOfServiceIpPrecedenceMapPingPort, cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    if (cliIsStackingSupported() == L7_TRUE)
    {
      rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

      if (rc == L7_FAILURE)
      {
        sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
     }

     /* check if interface is supported for COS mapping */
     if (usmDbQosCosMapIpPrecIntfIndexGet(unit, interface) != L7_SUCCESS)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_qos_IpPrecedenceTrafficClass);
   ewsTelnetWrite(ewsContext, "\r\n -------------    -------------");

   if (usmDbQosCosMapIpPrecIndexGet(unit, 0) == L7_SUCCESS)
   {

      for (precedence = 0; precedence < L7_QOS_COS_MAP_NUM_IPPREC; precedence++)
      {
         if (usmDbQosCosMapIpPrecTrafficClassGet(unit, interface, precedence, &trafficClass) != L7_SUCCESS)
      {
        break;
      }

      ewsTelnetPrintf (ewsContext, "\r\n%8d %16d", precedence, trafficClass);
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}
/*********************************************************************
*
* @purpose  display Class of Service Trust
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  exec> show classofservice trust [<slot/port>]
*
* @cmdhelp Display COS Summary information.
*
* @cmddescript shows the traffic class information for all traffic classes
*              in the system.

*
* @end
*
*********************************************************************/
const L7_char8 *commandShowClassofServiceTrust(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
   L7_uint32 unit, interface, rc;
   L7_uint32 argSlotPort=1;
   L7_uint32 intSlot;
   L7_uint32 intPort;
   L7_QOS_COS_MAP_INTF_MODE_t trust;
   L7_uint32 defaultTrafficClass;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   /* determine if displaying interface config or global config parms */
   if (cliNumFunctionArgsGet() == 0)
   {
      /* displaying global config values */
      interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
   }
   else
   {
      if (cliNumFunctionArgsGet() != 1)
      {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_ShowClassOfServiceTrustPort, cliSyntaxInterfaceHelp());
      return cliSyntaxReturnPrompt (ewsContext, buf);
      }

      if (cliIsStackingSupported() == L7_TRUE)
      {
         rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

         if (rc == L7_FAILURE)
         {
        sprintfAddBlanks (1, 0, 0, 1, pStrErr_common_Error, buf, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext, "%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      }

      /* check if interface is supported for COS mapping */
      if (usmDbQosCosMapTrustModeIntfIndexGet(unit, interface) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_qos_IntfNotSupported);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_qos_ClassOfServiceTrustMode);

   if (usmDbQosCosMapTrustModeGet (unit, interface, &trust) == L7_SUCCESS)
   {
      switch (trust)
      {
      case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
      osapiStrncpySafe(buf, pStrInfo_qos_Untrusted, sizeof(buf));
         break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
      osapiStrncpySafe(buf, pStrInfo_qos_Dot1p, sizeof(buf));
         break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
      osapiStrncpySafe(buf, pStrInfo_qos_IpPrecedence, sizeof(buf));
         break;
      case  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
      osapiStrncpySafe(buf, pStrInfo_qos_IpDscp, sizeof(buf));
         break;
      }
      ewsTelnetWrite(ewsContext, buf);
   }

   if (usmDbQosCosMapUntrustedPortDefaultTrafficClassGet(unit, interface, &defaultTrafficClass) == L7_SUCCESS)
   {
     if (trust == L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
     {
      osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_UntrustedTrafficClass_1, defaultTrafficClass);
       ewsTelnetWrite(ewsContext, buf);
     }
     else if ((trust == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC) ||
              (trust == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP))
     {
      osapiSnprintfAddBlanks (2, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_NonIpTrafficClass_1, defaultTrafficClass);
       ewsTelnetWrite(ewsContext, buf);
     }
   }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
* @purpose  Format a threshold display string based on number of
*           drop precedence levels
*
* @param    val[]       @b{(input)}  Array of values to format
* @param    *pStr       @b{(output)} Ptr to string output location
*
* @returns  void
*
* @comments Private helper function, only to be used by cosQueueConfigShow().
*
* @comments val[] is assumed to be an array[L7_MAX_CFG_DROP_PREC_LEVELS+1]
*           while pStr is assumed to be a buffer of at least
*           (((L7_MAX_CFG_DROP_PREC_LEVELS+1)*4)+1) bytes to hold the formatted
*           output threshold information in the form "xxx/xxx/xxx/xxx".  Each
*           value to be displayed is assumed to require three digits.
*
* @end
*********************************************************************/
void cliCosQueueThreshStringFormat(const L7_uchar8 val[], L7_uchar8 * pStr)
{
  char * p = (char *)pStr;
  L7_uint32     j;

  *p = (char)L7_EOS;                    /* just a precaution */

  for (j = 0; j < (L7_MAX_CFG_DROP_PREC_LEVELS + 1); j++)
  {
    if (j > 0)
    {
      sprintf(p++, "/");
    }                                  /* value separator */

    sprintf(p, "%3u", val[j]);
    p += 3;                             /* incr must match preceding fmt width*/

  } /* endfor j */
}
