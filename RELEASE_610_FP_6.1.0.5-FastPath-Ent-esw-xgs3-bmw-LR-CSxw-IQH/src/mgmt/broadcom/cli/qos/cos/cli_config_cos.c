/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/cli_config_cos.c
 *
 * @purpose acl config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/07/2003
 *
 * @author  Jagdish
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "dot1q_api.h"
#include "l7_cos_api.h"
#include "ews.h"
#include "clicommands_cos.h"
#include "usmdb_qos_cos_api.h"
#include "cos_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_util_diffserv_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  To configure the Global COS Queue Min Bandwidth parameters
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
* @notes none
*
* @cmdsyntax cos-queue {min-bandwidth <bw-0>...<bw-n>}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandGlobalCosQueueMinBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 numPara;
  L7_uint32 sumPara = 0;
  L7_uint32 bandWidth[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_qosCosQueueBwList_t minBw;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != L7_MAX_CFG_QUEUES_PER_PORT)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_CosQueueMinBandwidth, L7_MAX_CFG_QUEUES_PER_PORT-1);
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &bandWidth[numPara - 1])!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if ((bandWidth[numPara - 1] < L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) || (bandWidth[numPara - 1] > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if ((bandWidth[numPara-1] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
      sumPara = sumPara + bandWidth[numPara - 1];
    }

    if (sumPara > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_BandwidthTotal, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX);
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      minBw.bandwidth[numPara - 1] = bandWidth[numPara - 1];
    }

    /* validation here for min bw greater than max bw. If greater then set the max. bw to the minimum value else proceed*/

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMinBandwidthListSet(unit,L7_ALL_INTERFACES,&minBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MinBwSet);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoCosQueueMinBandwidth);
    }

    /* set the minBw data structure to 0 and call the usmdb for set the min. bandwidth */
    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      minBw.bandwidth[numPara - 1] = FD_QOS_COS_QCFG_MIN_BANDWIDTH;
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMinBandwidthListSet(unit,L7_ALL_INTERFACES, &minBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MinBwSet);
      }
    }

  }   /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}  /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Global COS Queue strict parameters
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
* @notes none
*
* @cmdsyntax cos-queue {strict <queue-id1> [<queue-id2>..<queue-idn>]}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandGlobalCosQueueStrict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, i;
  L7_uint32 unit;
  L7_uint32 numPara;
  L7_uint32 queueId;
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 begin[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 end[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL schedTypeNotSet = L7_FALSE;
  L7_BOOL first = L7_TRUE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  /* Error Checking for command Type : Exit if the command is of type no command*/

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg < 1 || numArg > L7_MAX_CFG_QUEUES_PER_PORT)
    {
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_Strict_1, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, L7_ALL_INTERFACES, &schedTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueSchedulerList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }

      schedTypeList.schedType[queueId] = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;

      memset (stat, 0, sizeof(stat));
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbQosCosQueueSchedulerTypeSet(unit, L7_ALL_INTERFACES, queueId,
                                             schedTypeList.schedType[queueId]) != L7_SUCCESS)
        {
          schedTypeNotSet = L7_TRUE;

          /* save all queue IDs for which scheduler type could not be set */
          memset (buf, 0, sizeof(buf));
          if (first == L7_TRUE)
          {
            sprintf(buf, "%d", queueId);
          }
          else
          {
            sprintf(buf, ", %d", queueId);
          }
          strcat(stat, buf);
          first = L7_FALSE;
        }
      }

      /* display all queue IDs for which scheduler type could not be set */
      if (schedTypeNotSet == L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_qos_UnableToSetScheduleTypeForQueue, stat);
      }
    }   /* END OF FOR LOOP */

  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg < 1 || numArg > L7_MAX_CFG_QUEUES_PER_PORT)
    {
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_NoStrict, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, L7_ALL_INTERFACES, &schedTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueSchedulerList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }

      schedTypeList.schedType[queueId] = L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED;

      memset (stat, 0, sizeof(stat));
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbQosCosQueueSchedulerTypeSet(unit, L7_ALL_INTERFACES, queueId,
                                             schedTypeList.schedType[queueId]) != L7_SUCCESS)
        {
          schedTypeNotSet = L7_TRUE;

          /* save all queue IDs for which scheduler type could not be set */
          memset (buf, 0, sizeof(buf));
          if (first == L7_TRUE)
          {
            sprintf(buf, "%d", queueId);
          }
          else
          {
            sprintf(buf, ", %d", queueId);
          }
          strcat(stat, buf);
          first = L7_FALSE;
        }
      }

      /* display all queue IDs for which scheduler type could not be set */
      if (schedTypeNotSet == L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_qos_UnableToSetScheduleTypeForQueue, stat);
      }
    }   /* END OF FOR LOOP */

  }   /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}  /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue Minimum Bandwidth parameters
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
* @notes none
*
* @cmdsyntax cos-queue {min-bandwidth <bw-0>...<bw-n>}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandInterfaceCosQueueMinBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit, s, p, interface;
  L7_uint32 numPara;
  L7_uint32 sumPara = 0;
  L7_uint32 bandWidth[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_qosCosQueueBwList_t minBw;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg != L7_MAX_CFG_QUEUES_PER_PORT)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_CosQueueMinBandwidth, L7_MAX_CFG_QUEUES_PER_PORT-1);
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &bandWidth[numPara - 1])!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if ((bandWidth[numPara - 1] < L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN) || (bandWidth[numPara - 1] > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if ((bandWidth[numPara-1] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
      sumPara = sumPara + bandWidth[numPara - 1];
    }

    if (sumPara > L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_BandwidthTotal, L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX);
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      minBw.bandwidth[numPara - 1] = bandWidth[numPara - 1];
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMinBandwidthListSet(unit,interface,&minBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MinBwSet);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoCosQueueMinBandwidth);
    }

    /* set the minBw data structure to 0 and call the usmdb for set the min. bandwidth */
    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      minBw.bandwidth[numPara - 1] = FD_QOS_COS_QCFG_MIN_BANDWIDTH;
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMinBandwidthListSet(unit,interface, &minBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MinBwSet);
      }
    }

  }   /* END OF NO COMMAND */
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUCNTION */

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue parameters
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
* @notes none
*
* @cmdsyntax cos-queue {strict <queue-id1> [<queue-id2>..<queue-idn>]}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandInterfaceCosQueueStrict(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, i;
  L7_uint32 unit, s, p, interface;
  L7_uint32 numPara;
  L7_uint32 queueId;
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 begin[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 end[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg < 1 || numArg > L7_MAX_CFG_QUEUES_PER_PORT)
    {
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_Strict_1, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, interface, &schedTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueSchedulerList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }

      schedTypeList.schedType[queueId] = L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
    }   /* END OF FOR LOOP */

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueSchedulerTypeListSet(unit,interface,&schedTypeList) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_ScheduleTypeSet );
      }
    }

  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg < 1 || numArg > L7_MAX_CFG_QUEUES_PER_PORT)
    {
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_NoStrict, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueSchedulerTypeListGet(unit, interface, &schedTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueSchedulerList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidInteger);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }

      schedTypeList.schedType[queueId] = L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED;
    }   /* END OF FOR LOOP */

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueSchedulerTypeListSet(unit,interface,&schedTypeList) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_ScheduleTypeSet );
      }
    }
  }   /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUCNTION */

/*********************************************************************
*
* @purpose  To configure the Interface Traffic-Shape
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
* @notes none
*
* @cmdsyntax traffic-shape <bw>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandInterfaceTrafficShape(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit,s,p,interface;
  L7_uint32 bandWidth;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  s = EWSSLOT(ewsContext);
  p = EWSPORT(ewsContext);

  if (usmDbIntIfNumFromUSPGet(unit, s, p, &interface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_TrafficShape_1);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bandWidth) != L7_SUCCESS)
    {
      if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParamKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
      }
    }

    if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      if ((bandWidth < L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN) || (bandWidth > L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParamKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
      }
    }
    else
    {
      if ((bandWidth < L7_QOS_COS_INTF_SHAPING_RATE_MIN) || (bandWidth > L7_QOS_COS_INTF_SHAPING_RATE_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
      }
      if ((bandWidth % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
   }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueIntfShapingRateSet(unit,interface,bandWidth) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_qos_TrafficShapeSet);
      }
    }
  }  /* END OF NORMAL COMMAND */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoTrafficShape);
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueIntfShapingRateSet(unit,interface,FD_QOS_COS_QCFG_INTF_SHAPING_RATE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_qos_TrafficShapeSet);
      }
    }
  }  /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To configure the Global Traffic-Shape
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
* @notes none
*
* @cmdsyntax traffic-shape <bw>
*  no traffic-shape
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandGlobalTrafficShape(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit;
  L7_uint32 bandWidth;
  
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_TrafficShape_1);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bandWidth)!=L7_SUCCESS)
    {
      if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParamKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
      }
    }

    if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      if ((bandWidth < L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN) || (bandWidth > L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParamKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
      }
    }
    else
    {
      if ((bandWidth < L7_QOS_COS_INTF_SHAPING_RATE_MIN) || (bandWidth > L7_QOS_COS_INTF_SHAPING_RATE_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
      }
      if ((bandWidth % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_TrafficShapeBwParam, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
      }
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueIntfShapingRateSet(unit,L7_ALL_INTERFACES,bandWidth) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_qos_TrafficShapeSet);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoTrafficShape);
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueIntfShapingRateSet(unit, L7_ALL_INTERFACES, FD_QOS_COS_QCFG_INTF_SHAPING_RATE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_qos_TrafficShapeSet);
      }
    }
  }  /* END OF NO COMMAND */
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue Maximum parameters
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
* @notes none
*
* @cmdsyntax cos-queue {min-bandwidth <bw-0>...<bw-n>}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandCosQueueMaxBandwidth(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 numArg;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 numPara;
  L7_uint32 bandWidth[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_qosCosQueueBwList_t maxBw, minBw;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {

      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all COS USMDB calls */
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != L7_MAX_CFG_QUEUES_PER_PORT)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_CosQueueMaxBandwidth, L7_MAX_CFG_QUEUES_PER_PORT-1);
    }

    /* get the current min bw parms for comparison against proposed max bw */
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_FALSE)
    {
      memset(&minBw, 0, sizeof(minBw));
    }
    else if (usmDbQosCosQueueMinBandwidthListGet(unit,interface,&minBw) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MaxBwMinGet);
    }

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &bandWidth[numPara - 1])!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
      if ((bandWidth[numPara-1] < L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN) ||
          (bandWidth[numPara-1] > L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
      if ((bandWidth[numPara-1] % L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE) != 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidBandwidth, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN, L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX, L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE);
      }
      /* do not allow non-zero max bw to be set lower than min bw for a queue */
      if ((bandWidth[numPara-1] != 0) && (bandWidth[numPara-1] < minBw.bandwidth[numPara-1]))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_CosQueueMaxBandwidthTooLow, numPara-1, minBw.bandwidth[numPara-1]);
      }
    }   /* END OF FOR LOOP */

    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      maxBw.bandwidth[numPara - 1] = bandWidth[numPara - 1];
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMaxBandwidthListSet(unit,interface,&maxBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MaxBwSet);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoCosQueueMaxBandwidth);
    }

    /* set the maxBw data structure to 0 and call the usmdb for set the max. bandwidth */
    for (numPara = 1 ; numPara <= L7_MAX_CFG_QUEUES_PER_PORT ; numPara++)
    {
      maxBw.bandwidth[numPara - 1] = FD_QOS_COS_QCFG_MAX_BANDWIDTH;
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosQueueMaxBandwidthListSet(unit,interface,&maxBw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_MaxBwSet);
      }
    }
  }   /* END OF NO COMMAND */

  cliSyntaxBottom(ewsContext);
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}  /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue parameters
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
* @notes none
*
* @cmdsyntax cos-queue {random-detect <queue-id1> [<queue-id2>..<queue-idn>]}
*  no cos-queue {random-detect <queue-id1> [<queue-id2>..<queue-idn>]}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandCosQueueRandomDetect(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 numPara;
  L7_uint32 queueId, i;
  L7_qosCosQueueMgmtTypeList_t mgmtTypeList;

  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 begin[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 end[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {

      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg < 1 || numArg > L7_MAX_CFG_QUEUES_PER_PORT)
    {
      memset (buf, 0, sizeof(buf));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      memset (stat, 0, sizeof(stat));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_CosQueueRandomDetect, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueMgmtTypeListGet(unit, interface, &mgmtTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueMgmtTypeList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ConVer);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }
      mgmtTypeList.mgmtType[queueId] = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;
    }   /* END OF FOR LOOP */

    if (usmDbQosCosQueueMgmtTypeListSet(unit,interface,&mgmtTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_CosQueueRandomDetectSet );
    }
  }  /* END OF NORMAL COMMAND */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ((numArg < 1) || (numArg > L7_MAX_CFG_QUEUES_PER_PORT))
    {
      memset (buf, 0, sizeof(buf));
      memset (begin, 0, sizeof(begin));
      memset (end, 0, sizeof(end));
      memset (stat, 0, sizeof(stat));
      for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
      {
        strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
        strcat(end, "]");
      }
      sprintf(buf, begin);
      strcat(buf, end);
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, pStrErr_qos_NoCosQueueRandomDetect, buf);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }

    /* retrieve current settings for all queues before applying user changes */
    if (usmDbQosCosQueueMgmtTypeListGet(unit, interface, &mgmtTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_qos_GetQueueMgmtTypeList);
    }

    /* Till number of arguments specified */
    for (numPara = 1; numPara <= numArg; numPara++)
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+numPara], &queueId)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ConVer);
      }
      if ((queueId < L7_QOS_COS_QUEUE_ID_MIN) || (queueId > L7_QOS_COS_QUEUE_ID_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }
      mgmtTypeList.mgmtType[queueId] = L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP;
    }   /* END OF FOR LOOP */

    if (usmDbQosCosQueueMgmtTypeListSet(unit,interface,&mgmtTypeList) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_CosQueueRandomDetectSet );
    }
  }   /* END OF NO COMMAND */
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUCNTION */

/*********************************************************************
*
* @purpose  To enable WRED for interface
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
* @notes none
*
* @cmdsyntax random-detect
*  no random-detect
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandRandomDetect(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 numArg, rc = L7_SUCCESS;
  L7_uint32 unit, slot, port, interface;
  L7_QOS_COS_QUEUE_MGMT_TYPE_t val;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {

      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE)
  {
    /* command not supported in this mode -- give syntax help instead */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_common_CfgTrapFlagsOspf);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_RandomDetect_1);
    }

    val = L7_QOS_COS_QUEUE_MGMT_TYPE_WRED;

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbQosCosQueueMgmtTypePerIntfSet(unit, interface, val);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_RandomDetectSet);
      }
    }

  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoRandomDetect);
    }

    val = L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP;

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbQosCosQueueMgmtTypePerIntfSet(unit, interface, val);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoRandomDetectSet);
      }
    }
  }  /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Decay Exponent used by WRED
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
* @notes none
*
* @cmdsyntax random-detect exponential-weighting-constant <1-15>
*  no random-detect exponential-weighting-constant
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandRandomDetectExponentialWeightingConstant(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, rc = L7_SUCCESS;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 val;
  L7_uint32 argExponent = 1;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_RandomDetectExponentialWeightingConSta_3, L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN, L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX);
    }

    memset (buf, 0, sizeof(buf));
    rc = cliCheckIfInteger((L7_char8 *)argv[index + argExponent]);
    if (L7_FAILURE == rc)
    {
      sprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, pStrErr_qos_IntegerRandomDetectExponentialWeightingConSta);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    val = atoi(argv[index + argExponent]);

    if (val < L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN || val > L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (0,1, 0, 1, L7_NULLPTR,ewsContext, pStrInfo_qos_RandomDetectExponentialWeightingConSta_2, L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN, L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbQosCosQueueWredDecayExponentSet(unit, interface, val);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_ExponentSet);
      }
    }

  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_NoRandomDetectExponentialWeightingConSta);
    }

    val = FD_QOS_COS_QCFG_WRED_DECAY_EXP;

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc =  usmDbQosCosQueueWredDecayExponentSet(unit, interface, val);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_qos_NoExponentSet);
      }
    }

  }  /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the WRED parameters
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
* @notes none
*
* @cmdsyntax random-detect queue-parms <queue-id1> [<queue-id2>..<queue-idn>]}
*    min-thresh <min-thresh> <min-thresh> <min-thresh>
*    max-thresh <max-thresh> <max-thresh> <max-thresh>
*    drop-prob-scale <drop-prob-scale> <drop-prob-scale> <drop-prob-scale>
*  no random-detect queue-parms <queue-id1> [<queue-id2>..<queue-idn>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandRandomDetectQueueParms(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, minNormalParmCount, maxNormalParmCount;
  L7_uint32 minNoParmCount, maxNoParmCount;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 pos, i, j, rc = L7_SUCCESS, num = 0;
  L7_qosCosDropParmsList_t dList;
  L7_uint32 totalQueueNum = 0;

  L7_uint32 queue[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_uint32 minthresh[L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uint32 maxthresh[L7_MAX_CFG_DROP_PREC_LEVELS+1];
  L7_uint32 dropprob[L7_MAX_CFG_DROP_PREC_LEVELS+1];

  L7_uchar8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uchar8 begin[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 end[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 normalSyntaxStr[L7_CLI_MAX_LARGE_STRING_LENGTH + 128];
  L7_uchar8 noSyntaxStr[L7_CLI_MAX_LARGE_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* Construct syntax error string */
  memset (begin, 0, sizeof(begin));
  memset (end, 0, sizeof(end));
  for(i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++) 
  {
      dList.queue[i].mgmtType = L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED;
  }
  for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
    strcat(end, "]");
  }
  osapiSnprintf(buf, sizeof(buf), begin);
  strcat(buf, end);

  /* No form of syntax error is constructed with only queue ids */
  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, noSyntaxStr, sizeof(buf), pStrErr_qos_NoRandomDetectQueueParms, buf);

  strcatAddBlanks (0, 0, 1, 1, L7_NULLPTR, buf, pStrInfo_qos_MinThresh_3);
  memset (tmp, 0, sizeof(tmp));
  for (i = 1; i <= L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, tmp, pStrInfo_qos_MinThresh_1);
  }
  strcat(buf, tmp);

  strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_MaxThresh_2);
  memset (tmp, 0, sizeof(tmp));
  for (i = 1; i <= L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, tmp, pStrInfo_qos_MaxThresh);
  }
  strcat(buf, tmp);

  strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_DropProb_1);
  memset (tmp, 0, sizeof(tmp));
  for (i = 1; i <= L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, tmp, pStrInfo_qos_DropProb);
  }
  strcat(buf, tmp);

  /* Normal form of syntax error is needs queue ids, min-thresh, max-thresh, and dropprob */
  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, normalSyntaxStr, sizeof(buf), pStrErr_qos_RandomDetectQueueParms_1, buf);

  numArg = cliNumFunctionArgsGet();

  /* minNumOfQueues + numOfCommandTokens (3) + */
  /* numOfParmsForEachToken * numOfCommandTokens */
  minNormalParmCount = 1 + 3 + ((L7_MAX_CFG_DROP_PREC_LEVELS+1) * 3);

  /* maxNumOfQueues + numOfCommandTokens (3) + */
  /* numOfParmsForEachToken * numOfCommandTokens */
  maxNormalParmCount = L7_MAX_CFG_QUEUES_PER_PORT + 3 + ((L7_MAX_CFG_DROP_PREC_LEVELS+1) * 3);

  /* minNumOfQueues + numOfNoCommandTokens (0) */
  minNoParmCount = 1 + 0;

  /* maxNumOfQueues + numOfNoCommandTokens (0) */
  maxNoParmCount = L7_MAX_CFG_QUEUES_PER_PORT + 0;

  if (ewsContext->commType == CLI_NORMAL_CMD  && (numArg < minNormalParmCount || numArg > maxNormalParmCount))
  {
    return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && (numArg < minNoParmCount || numArg > maxNoParmCount))
  {
    return cliSyntaxReturnPrompt (ewsContext, noSyntaxStr);
  }

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  pos = 1;    /* initialize parameter position */

  while ((pos <= L7_MAX_CFG_QUEUES_PER_PORT) && (pos <= numArg))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+pos]) != L7_SUCCESS)
    {
      if ( (ewsContext->commType == CLI_NORMAL_CMD) )
      {
        break;
      }
      if ( (ewsContext->commType == CLI_NO_CMD) )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }
    }
    else
    {
      num = atoi(argv[index+ pos]);
    }
    if (num < L7_QOS_COS_QUEUE_ID_MIN || num > L7_QOS_COS_QUEUE_ID_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    }
    queue[totalQueueNum++] = num;
    pos++;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (pos == 1 || strcmp(argv[index+pos], pStrInfo_qos_MinThresh_3) != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
    }
    pos++;             /* skip over this command token */

    for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
    {
      if (cliCheckIfInteger((L7_char8 *)argv[index+ pos]) != L7_SUCCESS)    /* check for min-thresh range too */
      {
        return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
      }
      else
      {
        num = atoi(argv[index+ pos]);
      }

      if (num < L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN || num > L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_RandomDetectMinThreshPara, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX);
        ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidRandomDetectQueueParms);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else
      {
        minthresh[i] = num;
      }

      pos++;
    }

    if (strcmp(argv[index+pos], pStrInfo_qos_MaxThresh_2) != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
    }
    pos++;             /* skip over this command token */

    for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
    {
      if (cliCheckIfInteger((L7_char8 *)argv[index+ pos]) != L7_SUCCESS)    /* check for max-thresh range too */
      {
        return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
      }
      else
      {
        num = atoi(argv[index+ pos]);
      }

      if (num < L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN || num > L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_RandomDetectMaxThreshPara, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX);
        ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidRandomDetectQueueParms);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else if (num < minthresh[i])
      {
        /* max thresh cannot be less than min thresh for a given drop precedence level */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_qos_InvalidRandomDetectMaxThreshTooLow, (L7_QOS_COS_DROP_PREC_LEVEL_MIN+i), minthresh[i]);
      }
      else
      {
        maxthresh[i] = num;
      }
      pos++;
    }

    if (strcmp(argv[index+pos], pStrInfo_qos_DropProb_1) != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
    }
    pos++;             /* skip over this command token */

    for (i = 0; i < L7_MAX_CFG_DROP_PREC_LEVELS+1; i++)
    {
      if (cliCheckIfInteger((L7_char8 *)argv[index+ pos]) != L7_SUCCESS)    /* check for min-thresh range too */
      {
        return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
      }
      else
      {
        num = atoi(argv[index+ pos]);
      }

      if (num < L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN || num > L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_RandomDetectDropProbs_2, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX);
        ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidRandomDetectQueueParms);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else
      {
        dropprob[i] = num;
      }
      pos++;
    }

    rc = usmDbQosCosQueueDropParmsListGet(unit, interface, &dList);
    if (rc  != L7_SUCCESS)    /* check for min-thresh range too */
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetWredParams);
    }
    for (i = 0; i < totalQueueNum; i++)
    {
      for (j = 0; j < L7_MAX_CFG_DROP_PREC_LEVELS+1; j++)
      {
        dList.queue[queue[i]].minThreshold[j] = minthresh[j];
        dList.queue[queue[i]].wredMaxThreshold[j] = maxthresh[j];
        dList.queue[queue[i]].dropProb[j] = dropprob[j];
      }
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQosCosQueueDropParmsListSet(unit, interface, &dList);

      if (rc  != L7_SUCCESS)    /* check for min-thresh range too */
      {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetWredParams);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (pos == 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, noSyntaxStr);
    }

    for (i = 0; i < totalQueueNum; i++)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQosCosQueueDropDefaultsRestore(unit, interface, queue[i]);
        if (rc  != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetWredParamsBackToDefl);
        }
      }
    }

  }  /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Tail Drop parameters
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
* @notes none
*
* @cmdsyntax tail-drop queue-parms <queue-id1> [<queue-id2>..<queue-idn>]} threshold <0-16> <0-16> <0-16>
*  no tail-drop queue-parms <queue-id1> [<queue-id2>..<queue-idn>]}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTailDropQueueParms(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg, minNormalParmCount, maxNormalParmCount;
  L7_uint32 minNoParmCount, maxNoParmCount;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 pos, i, j, rc = L7_SUCCESS, num = 0;
  L7_qosCosDropParmsList_t dropList;
  L7_uint32 totalQueueNum = 0;

  L7_uint32 queue[L7_MAX_CFG_QUEUES_PER_PORT];
  L7_uint32 threshold[L7_MAX_CFG_DROP_PREC_LEVELS+1];

  L7_uchar8 tmp[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uchar8 begin[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 end[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 normalSyntaxStr[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uchar8 noSyntaxStr[L7_CLI_MAX_LARGE_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* Construct syntax error string */
  memset (begin, 0, sizeof(begin));
  memset (end, 0, sizeof(end));
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++) 
  {
      dropList.queue[i].mgmtType = L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED;
  }
  for (i = 2; i <= L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    strcatAddBlanks (0, 0, 1, 0, L7_NULLPTR, begin, pStrInfo_qos_QueueId_2);
    strcat(end, "]");
  }
  osapiSnprintf(buf, sizeof(buf), begin);
  strcat(buf, end);

  /* No form of syntax error is constructed with only queue ids */
  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, noSyntaxStr, sizeof(buf), pStrErr_qos_Not_1, buf);

  strcatAddBlanks (0, 0, 1, 1, L7_NULLPTR, buf, pStrInfo_qos_Thresh_2);
  memset (tmp, 0, sizeof(tmp));
  for (i = 0; i < (L7_MAX_CFG_DROP_PREC_LEVELS+1); i++)
  {
    strcatAddBlanks (0, 0, 0, 1, L7_NULLPTR, tmp, pStrInfo_common_Thresh_1);
  }
  strcat(buf, tmp);

  /* Normal form of syntax error needs queue ids, min-thresh, max-thresh, and dropprob */
  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, normalSyntaxStr, sizeof(buf), pStrErr_qos_TailDropQueueParms_1, buf);

  numArg = cliNumFunctionArgsGet();

  /* minNumOfQueues + numOfCommandTokens (1) + numOfParmsForToken */
  minNormalParmCount = 1 + 1 + (L7_MAX_CFG_DROP_PREC_LEVELS+1);

  /* maxNumOfQueues + numOfCommandTokens (1) + numOfParmsForToken */
  maxNormalParmCount = L7_MAX_CFG_QUEUES_PER_PORT + 1 + (L7_MAX_CFG_DROP_PREC_LEVELS+1);

  /* minNumOfQueues + numOfNoCommandTokens (0) */
  minNoParmCount = 1 + 0;

  /* maxNumOfQueues + numOfNoCommandTokens (0) */
  maxNoParmCount = L7_MAX_CFG_QUEUES_PER_PORT + 0;

  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg < minNormalParmCount || numArg > maxNormalParmCount))
  {
    return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
  }
  else if ((ewsContext->commType == CLI_NO_CMD) && ((numArg < minNoParmCount) || (numArg > maxNoParmCount)))
  {
    return cliSyntaxReturnPrompt (ewsContext, noSyntaxStr);
  }

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  pos = 1;    /* initialize parameter position */

  while ((pos <= L7_MAX_CFG_QUEUES_PER_PORT) && (pos <= numArg))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[index+pos]) != L7_SUCCESS)
    {
      if ( (ewsContext->commType == CLI_NORMAL_CMD) )
      {
        break;
      }
      if ( (ewsContext->commType == CLI_NO_CMD) )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
      }
    }
    else
    {
      num = atoi(argv[index+ pos]);
    }
    if (num < L7_QOS_COS_QUEUE_ID_MIN || num > L7_QOS_COS_QUEUE_ID_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_qos_InvalidQueueId, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    }
    queue[totalQueueNum++] = num;
    pos++;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*Check for four threshold values*/
    if ( numArg < (totalQueueNum + 1 + (L7_MAX_CFG_DROP_PREC_LEVELS+1)) )
    {
      return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);

    }

    if (pos == 1 || strcmp(argv[index+pos], pStrInfo_qos_Thresh_2) != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
    }
    pos++;             /* skip over this command token */

    for (i = 0; i < (L7_MAX_CFG_DROP_PREC_LEVELS+1); i++)
    {
      if (cliCheckIfInteger((L7_char8 *)argv[index+ pos]) != L7_SUCCESS)    /* check for threshold range too */
      {
        return cliSyntaxReturnPrompt (ewsContext, normalSyntaxStr);
      }
      else
      {
        num = atoi(argv[index+ pos]);
      }

      if (num < L7_QOS_COS_QUEUE_TDROP_THRESH_MIN || num > L7_QOS_COS_QUEUE_TDROP_THRESH_MAX)
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_TailDropThreshPara, L7_QOS_COS_QUEUE_TDROP_THRESH_MIN, L7_QOS_COS_QUEUE_TDROP_THRESH_MAX);
        ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidTailDropQueueParms);
        return cliSyntaxReturnPrompt (ewsContext, buf);
      }
      else
      {
        threshold[i] = num;
      }

      pos++;
    }

    rc = usmDbQosCosQueueDropParmsListGet(unit, interface, &dropList);
    if (rc  != L7_SUCCESS)    /* check for threshold range too */
    {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetTailDropParams);
    }
    for (i = 0; i < totalQueueNum; i++)
    {
      for (j = 0; j < (L7_MAX_CFG_DROP_PREC_LEVELS+1); j++)
      {
        dropList.queue[queue[i]].tailDropMaxThreshold[j] = threshold[j];
      }
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbQosCosQueueDropParmsListSet(unit, interface, &dropList);

      if (rc  != L7_SUCCESS)    /* check for threshold range too */
      {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetTailDropParams);
      }
    }
  }  /* END OF NORMAL COMMAND */

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (pos == 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, noSyntaxStr);
    }

    for (i = 0; i < totalQueueNum; i++)
    {
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbQosCosQueueDropDefaultsRestore(unit, interface, queue[i]);
        if (rc  != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_qos_SetTailDropParamsBackToDefl);
        }
      }
    }
  }  /* END OF NO COMMAND */

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUNCTION */

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue parameters
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
* @notes none
*
* @cmdsyntax classofservice ip-dscp-mapping
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClassofserviceIpDscpMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 numArg;
  L7_uint32 unit, slot, port, interface, def_tc;
  L7_uint32 dscpVal, trafficClassVal, maxNumTrafficClasses;
  L7_uint32 argDscpVal = 1;
  L7_uint32 argTrafficClass = 2;
  L7_char8 strDscpVal[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    unit = EWSUNIT(ewsContext);
  }
  else
  {
    unit = cliGetUnitId();
  }

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if(mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    interface = L7_ALL_INTERFACES;    /* use this for all dot1p USMDB calls */
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CosIpDscpMapPing);
    }

    if (strlen(argv[index+argDscpVal]) >= sizeof(strDscpVal))
    {
      osapiSnprintf(buf, sizeof(buf), dscpHelp, L7_QOS_COS_MAP_IPDSCP_MIN, L7_QOS_COS_MAP_IPDSCP_MAX);
      ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosIpDscp);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    osapiStrncpySafe(strDscpVal,argv[index+ argDscpVal], sizeof(strDscpVal));

    /* convert DSCP string to integer - check for L7_ERROR */
    if (cliDiffservConvertDSCPStringToValNoVerify(strDscpVal, &dscpVal) == L7_ERROR)
    {
      osapiSnprintf(buf, sizeof(buf), dscpHelp, L7_QOS_COS_MAP_IPDSCP_MIN, L7_QOS_COS_MAP_IPDSCP_MAX);
      ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosIpDscp);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    /* check the range */
    if (dscpVal < L7_QOS_COS_MAP_IPDSCP_MIN || dscpVal > L7_QOS_COS_MAP_IPDSCP_MAX)
    {
      osapiSnprintf(buf, sizeof(buf), dscpHelp, L7_QOS_COS_MAP_IPDSCP_MIN, L7_QOS_COS_MAP_IPDSCP_MAX);
      ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosIpDscp);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    maxNumTrafficClasses = dot1dPortNumTrafficClassesGet(interface);

    trafficClassVal = atoi(argv[index + argTrafficClass]);

    /* check the range */
    if (trafficClassVal < 0 || trafficClassVal >= maxNumTrafficClasses)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosTrafficClass);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosMapIpDscpTrafficClassSet(unit, interface, dscpVal, trafficClassVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_IpDscpMapPingSet );
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CosIpDscpMapPingNo);
    }

    for (dscpVal = L7_QOS_COS_MAP_IPDSCP_MIN; dscpVal <= L7_QOS_COS_MAP_IPDSCP_MAX; dscpVal++)
    {
      rc = usmDbQosCosMapIpDscpDefaultTrafficClassGet(unit, interface, dscpVal, &def_tc);
      if (rc == L7_SUCCESS)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          /* NOTE:  Called function will check existing value against new value
           *        to cover case where global config change affects some
           *        interfaces but not all.
           */
          rc = usmDbQosCosMapIpDscpTrafficClassSet(unit, interface, dscpVal, def_tc);
        }
      }

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_IpDscpSet);
      }
    } /* endfor */
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

} /* END OF FUCNTION */

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue parameters
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
* @notes none
*
* @cmdsyntax classofservice ip-precedence
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClassofserviceIpPrecedenceMapping(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 numArg;
  L7_uint32 unit, slot, port, interface, def_tc;
  L7_uint32 precVal, trafficClassVal, maxNumTrafficClasses;
  L7_uint32 argPrecVal = 1;
  L7_uint32 argTrafficClass = 2;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CosIpPrecedenceMapPing);
    }

    precVal = atoi(argv[index+ argPrecVal]);

    /* check the range */
    if (precVal < L7_QOS_COS_MAP_IPPREC_MIN || precVal > L7_QOS_COS_MAP_IPPREC_MAX)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_IpPrecedenceMapPingParaVal, L7_QOS_COS_MAP_IPPREC_MIN, L7_QOS_COS_MAP_IPPREC_MAX);
      ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosIpPrecedence);
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }

    maxNumTrafficClasses = dot1dPortNumTrafficClassesGet(interface);

    trafficClassVal = atoi(argv[index + argTrafficClass]);

    /* check the range */
    if (trafficClassVal < 0 || trafficClassVal >= maxNumTrafficClasses)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, L7_NULLPTR, ewsContext, pStrErr_qos_InvalidCosTrafficClass);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosMapIpPrecTrafficClassSet(unit, interface, precVal, trafficClassVal) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_IpPrecedenceMapPingSet );
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CosIpPrecedenceMapPingNo);
    }

    for (precVal = L7_QOS_COS_MAP_IPPREC_MIN; precVal <= L7_QOS_COS_MAP_IPPREC_MAX; precVal++)
    {
      rc = usmDbQosCosMapIpPrecDefaultTrafficClassGet(unit, interface, precVal, &def_tc);
      if (rc == L7_SUCCESS)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          /* NOTE:  Called function will check existing value against new value
           *        to cover case where global config change affects some
           *        interfaces but not all.
           */
          rc = usmDbQosCosMapIpPrecTrafficClassSet(unit, interface, precVal, def_tc);
        }
      }

      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_CouldNot,  ewsContext, pStrErr_qos_IpPrecedenceSet);
      }
    } /* endfor */
  }
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To print the acl command syntax
*
* @param EwsContext ewsContext
*
* @returntype void
*
* @notes
* @end
*
*********************************************************************/
void  cosSyntaxWrite(EwsContext ewsContext)
{
  L7_uchar8 cliSyntax[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * cliCurr;
  L7_char8 * delimiter = "{";
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_qos_CfgCosNot);
  }
  else
  {
    memset (cliSyntax, 0, sizeof(cliSyntax));
    cliCurr = cliSyntax;
    strcpyAddBlanks (1, 0, 0, 1, pStrErr_common_IncorrectInput, cliCurr, pStrErr_qos_CfgCosTrust);
    cliCurr += strlen(cliCurr);

    if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
    {
      strcpy(cliCurr, delimiter);
      cliCurr += strlen(delimiter);
      delimiter = " | ";

      strcpy(cliCurr, pStrErr_qos_CfgCosTrustDot1p);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      strcpy(cliCurr, delimiter);
      cliCurr += strlen(delimiter);
      delimiter = " | ";

      strcpy(cliCurr, pStrErr_qos_CfgCosTrustIpDscp);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
    {
      strcpy(cliCurr, delimiter);
      cliCurr += strlen(delimiter);
      delimiter = " | ";

      strcpy(cliCurr, pStrErr_qos_CfgCosTrustIpPrecedence);
      cliCurr += strlen(cliCurr);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_FEATURE_SUPPORTED) == L7_TRUE)
    {
      strcpy(cliCurr, delimiter);
      cliCurr += strlen(delimiter);

      strcpy(cliCurr, pStrErr_qos_CfgCosTrustUntrusted);
      cliCurr += strlen(cliCurr);
    }

    delimiter = "}'\r\n";
    strcpy(cliCurr, delimiter);
    cliCurr += strlen(delimiter);
    ewsTelnetWrite( ewsContext, cliSyntax);
  }
}

/*********************************************************************
*
* @purpose  To configure the Interface COS Queue parameters
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
* @notes none
*
* @cmdsyntax classofservice trust {dot1p | ip-dscp | ip-precedence | untrusted}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClassofserviceTrust(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 unit, slot, port, interface;
  L7_uint32 val = 0;
  L7_uint32 argTrustVal = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* determine if function called from interface config or global config */
  if (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_INTERFACE_CONFIG_MODE))
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      unit = EWSUNIT(ewsContext);
    }
    else
    {
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    slot = EWSSLOT(ewsContext);
    port = EWSPORT(ewsContext);

    if (usmDbIntIfNumFromUSPGet(unit,  slot, port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
    interface = L7_ALL_INTERFACES;        /* use this for all dot1p USMDB calls */
  }

  if ((ewsContext->commType == CLI_NORMAL_CMD && numArg != 1) ||
      (ewsContext->commType == CLI_NO_CMD && numArg != 0))
  {
    cosSyntaxWrite(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strcmp(argv[index+argTrustVal], pStrErr_qos_CfgCosTrustDot1p) <= 0)
    {
      val =  L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P;
    }
    else if (strcmp(argv[index+argTrustVal], pStrErr_qos_CfgCosTrustIpDscp) <= 0)
    {
      val =  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP;
    }
    else if (strcmp(argv[index+argTrustVal], pStrErr_qos_CfgCosTrustIpPrecedence) <= 0)
    {
      val =  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC;
    }
    else
    {
      val =  L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED;
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
      val = FD_QOS_COS_MAP_INTF_TRUST_MODE;
  }

  if (usmDbQosCosMapIntfIsValid(unit, interface) == L7_TRUE)
  {

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbQosCosMapTrustModeSet(unit, interface, val) != L7_SUCCESS)
      {
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_TrustSet );
        }
        else if (ewsContext->commType == CLI_NO_CMD)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR,  ewsContext,pStrErr_qos_Not );
        }
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
#if defined(FEAT_METRO_CPE_V1_0)
      if (interface != L7_ALL_INTERFACES)
      {
        L7_uint32 globMode;

        if (usmDbQosCosMapTrustModeGet(L7_NULL, L7_ALL_INTERFACES, &globMode) == L7_SUCCESS &&
            globMode == L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED && val == L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_qos_NoteUnTrustSet);
        }
      }
#endif
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext,pStrErr_qos_IntfNotSupported);
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
