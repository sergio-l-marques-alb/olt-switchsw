/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/qos/acl/vlan_acl_cfg.c
 *
 * @purpose     Code in support of the vlan_acl_cfg.html page
 *
 * @component   web
 *
 * @comments
 *
 * @create      21 Dec 2006
 *
 * @author      dbhargava
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_qos_common.h"
#include "strlib_qos_web.h"
#include <stdio.h>
#include <string.h>
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "usmdb_qos_acl_api.h"
#include "acl_exports.h"
#include "dot1q_exports.h"
#include "usmdb_sim_api.h"
#include "web_oem.h"
#include "cli_web_util.h"
#include "util_pstring.h"

#define ACL_TYPE_CHANGED      1
#define ACL_VLAN_CHANGED      2
#define ACL_DIRECTION_CHANGED 3
#define ACL_ID_CHANGED        4


/*********************************************************************
*
* @purpose Get all the MAC ACLs in an options list drop down box
*
* @param context EmWeb/Server request context handle
*
* @param optionp
*
* @param iterator
*
* @returns none
*
* @notes
*
* @end
*
*********************************************************************/
void *ewaFormSelect_mac_acl_list(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  static L7_uint32 aclID, sel_acl;
  L7_uint32 unit;
  L7_RC_t rc;
  L7_char8 buf[APP_BUFFER_SIZE], aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 * bufChoice;
  L7_uint32 itrValid;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  itrValid = L7_FALSE;

  unit = usmDbThisUnitGet();
  if (iterator == NULL)
  {
    rc = usmDbQosAclMacIndexGetFirst(unit, &aclID);
    sel_acl = aclID;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      if (appInfo.data[20] != 0)
      {
        sel_acl = appInfo.data[20];
      }
    }
  }
  else
  {
    rc = usmDbQosAclMacIndexGetNext(unit, aclID, &aclID);
  }

  if (rc==L7_SUCCESS)
  {
    memset(buf, 0, sizeof(buf));
    rc = usmDbQosAclMacNameGet(unit, aclID, aclName);
    if (rc==L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(buf, aclName);
      bufChoice = ewsContextNetHandle(context)->buffer;
      osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

      optionp->choice = bufChoice;
      optionp->valuep = (void *) &aclID;

      if (sel_acl == aclID)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }

      itrValid = L7_TRUE;
    }
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return L7_NULL;
  }

}

/*********************************************************************
*
* @purpose Get all the acl id's in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @returns option
*
* @end
*
*********************************************************************/
void *ewaFormSelect_ip_acl_list(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  static L7_uint32 aclNum, sel_acl;
  L7_uint32 unit;
  static L7_RC_t rc;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_char8 * bufChoice;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  unit = usmDbThisUnitGet();

  memset(buf, 0, sizeof(buf));
  if (iterator == NULL)
  {
    if ((rc = usmDbQosAclNumGetFirst(unit, &aclNum)) != L7_SUCCESS)
    {
      rc = usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IP, &aclNum);
    }
    sel_acl = aclNum;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      if (appInfo.data[20] != 0)
      {
        sel_acl = appInfo.data[20];
      }
    }
  }
  else
  {
    if ((rc = usmDbQosAclNumGetNext(unit, aclNum, &aclNum)) != L7_SUCCESS)
    {
      rc = usmDbQosAclNamedIndexGetNext(unit, L7_ACL_TYPE_IP, aclNum, &aclNum);
    }
  }

  if (rc == L7_SUCCESS)
  {
    if (usmDbQosAclNamedIndexCheckValid(unit, L7_ACL_TYPE_IP, aclNum) == L7_SUCCESS)
    {
      if (usmDbQosAclNameGet(unit, aclNum, aclName) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf),  "%s", aclName);
      }
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%d", aclNum);
    }

    bufChoice = ewsContextNetHandle(context)->buffer;
    osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

    optionp->choice = bufChoice;
    optionp->valuep = (void *) &aclNum;

    if (sel_acl == aclNum)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    return (void *) optionp;
  }
  return L7_NULL;
}

/*********************************************************************
* @purpose Generate the list of configured vlan ids
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_vlan(EwsContext context,
                         EwsFormSelectOptionP optionp,
                         void * iterator )
{
  static L7_uint32 itrValid,
                   selected_vlanId=0,
                   vlanId = L7_DOT1Q_DEFAULT_VLAN;
  L7_char8 * bufChoice = ewsContextNetHandle(context)->buffer;
  L7_uchar8 buf[APP_BUFFER_SIZE];
  L7_RC_t rc;
  usmWeb_AppInfo_t appInfo;
  EwaNetHandle net = ewsContextNetHandle(context);
  
  itrValid = L7_FALSE;
  if (iterator == L7_NULL)
  {
    vlanId = L7_DOT1Q_DEFAULT_VLAN;
    selected_vlanId = 0;
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      selected_vlanId = appInfo.data[0];
    }
    rc = L7_SUCCESS;
    
  }
  else
  {
    if(vlanId < L7_ACL_MAX_VLAN_ID)
    {
       vlanId++;
       rc = L7_SUCCESS;
    }
    else
    {
       rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    if (selected_vlanId == 0)    /* Select the first existing if no selection */
    {
      selected_vlanId = 1;
    }
    optionp->valuep = (void *) &vlanId;
    osapiSnprintf(buf, sizeof(buf)-1, "%d", vlanId);
    osapiStrncpy(bufChoice, buf, sizeof(buf)-1);
    optionp->choice = bufChoice;
    if (vlanId == selected_vlanId)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    itrValid = L7_TRUE;
  }
  else
  {
    itrValid = L7_FALSE;
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }

  return L7_NULL;
}

/* *********************************************************************************
 * @purpose to get the Number of VLAN based ACLs whose info is to be displayed
 *
 * @param  unit unitIndex
 *
 * @returns Number of ACLs
 *
 * @end
 *
 *********************************************************************/
L7_uint32 usmWebNumVlanAclGet(L7_uint32 vlanId, L7_ACL_DIRECTION_t direction)
{
  L7_ACL_VLAN_DIR_LIST_t listInfo;

  if (vlanId == 0)
  {
    return L7_NULL;
  }

  if (usmDbQosAclVlanDirAclListGet(1, vlanId, direction, &listInfo) == L7_SUCCESS)
  {
    return listInfo.count;
  }
  else
  {
    return L7_NULL;
  }
}

/* *********************************************************************************
 * @purpose to get the first ACL num assigned to an vlan in a particular direction
 *
 * @param   L7_ACL_DIRECTION_t direction .Direction of the acl
 *                      L7_INBOUND_ACL  - inbound
 *                      L7_OUTBOUND_ACL - outbound
 * @param   L7_ACL_TYPE_t  type. Type of acl
 *                      L7_ACL_TYPE_IP     - IP ACL
 *                      L7_ACL_TYPE_IPV6   - IPv6 ACL
 *                      L7_ACL_TYPE_MAC    - MAC ACL
 *
 * @param   L7_uint32 VLan .The vlan configured
 *
 * @param   L7_uint32 *aclNum {{output}} Will hold the first acl id value.
 *
 * @returns L7_RC_t L7_FAILURE when entry not found.
 *                  L7_SUCCESS aclID found
 * @end
 *
 *********************************************************************/
L7_RC_t usmWebVlanAssignedAclFirstGet(L7_uint32 intIfNum,
                                      L7_ACL_DIRECTION_t direction,
                                      L7_ACL_TYPE_t type,
                                      L7_uint32 * aclNum, L7_uint32 * seqNum)
{
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_uint32 i;

  /* Get all the assigned acls for a given vlan and direction combination */
  if (usmDbQosAclVlanDirAclListGet(0, intIfNum, direction, &listInfo) == L7_SUCCESS)
  {
    for (i = 0; i < listInfo.count; i++)
    {
      /* Check for the correct type */
      if (listInfo.listEntry[i].aclType == type)
      {
        *aclNum = listInfo.listEntry[i].aclId;
        *seqNum = listInfo.listEntry[i].seqNum;
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;

}
/*********************************************************************
* @purpose Initialize the form data
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the vlan_acl_cfg form
*
* @returns none
*
* @notes appInfo.data[0] represents the acl id of the acl
*
* @end
*
*********************************************************************/
void ewaFormServe_vlan_acl_cfg(EwsContext context, EwaForm_vlan_acl_cfgP form )
{
  EwaNetHandle net = ewsContextNetHandle(context);
  usmWeb_AppInfo_t appInfo;
  L7_uint32 unit = usmDbThisUnitGet(),
            vlanId, list_counter, i,
            user_access = usmWebUserAccessGet(context);
  L7_ACL_DIRECTION_t direction = L7_INBOUND_ACL;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_char8        aclName[L7_ACL_NAME_LEN_MAX+1], 
                  buf[USMWEB_BUFFER_SIZE_256];

  form->value.acl_typ_change = L7_FALSE;
  form->status.acl_typ_change |= (EW_FORM_INITIALIZED);
  form->status.direction |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);
  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
    vlanId = appInfo.data[0];
    form->value.vlan = appInfo.data[0];
    form->status.vlan |= (EW_FORM_INITIALIZED);
    switch(appInfo.data[1])
    {
    case L7_INBOUND_ACL:
      form->value.direction = inbound;
      direction = L7_INBOUND_ACL;
      break;
    case L7_OUTBOUND_ACL:
      form->value.direction = outbound;
      direction = L7_OUTBOUND_ACL;
      break;
    default:
      form->value.direction = inbound;
      direction = L7_INBOUND_ACL;
      break;
    }

    if(user_access == USMWEB_READWRITE)
    {
      form->value.acl_type = none;
      if(appInfo.data[2] != 0)
      {
        if(appInfo.data[2] == L7_ACL_TYPE_IP)
        {
          form->value.acl_type = ip_acl;
          form->status.ip_acl_list |= (EW_FORM_INITIALIZED);
        }
        else if(appInfo.data[2] == L7_ACL_TYPE_MAC)
        {
          form->value.acl_type = mac_acl;
          form->status.mac_acl_list |= (EW_FORM_INITIALIZED);
        }
        else if(appInfo.data[2] == L7_ACL_TYPE_IPV6)
        {
          form->value.acl_type = ipv6_acl;
          form->status.ipv6_acl_list |= (EW_FORM_INITIALIZED);
        }
      }
      form->status.acl_type |= (EW_FORM_INITIALIZED);
      form->status.sequence_nbr |= (EW_FORM_INITIALIZED);
    }
  }
  else
  {
    vlanId = L7_DOT1Q_DEFAULT_VLAN;
    form->value.vlan = vlanId;
    form->value.direction = inbound;
    direction = L7_INBOUND_ACL;
    if (user_access == USMWEB_READWRITE)
    {
      form->status.acl_type |= (EW_FORM_INITIALIZED);
      form->value.acl_type = none;
      form->status.sequence_nbr |= (EW_FORM_INITIALIZED);
    }
  }

  memset(&listInfo, 0, sizeof(L7_ACL_INTF_DIR_LIST_t));
  if (usmDbQosAclVlanDirAclListGet(unit, vlanId, direction, &listInfo) == L7_SUCCESS)
  {
    list_counter = 0;
    for (i=0; i<listInfo.count; i++)
    {
      if (i==0)
      {
        osapiSnprintf(buf, sizeof(buf)-1, "%d", vlanId);
        form->value.vlanAclConfig[list_counter].vlan_id_ro = osapiStrDup(buf);
        switch(direction)
        {
        case L7_INBOUND_ACL:
          form->value.vlanAclConfig[list_counter].direction_ro = osapiStrDup(pStrInfo_qos_Inbound);
          break;
        case L7_OUTBOUND_ACL:
          form->value.vlanAclConfig[list_counter].direction_ro = osapiStrDup(pStrInfo_qos_Outbound);
          break;
        default:
          form->value.vlanAclConfig[list_counter].direction_ro = osapiStrDup(pStrInfo_qos_Inbound);
          break;
        }
      }
      else
      {
        form->value.vlanAclConfig[list_counter].vlan_id_ro = osapiStrDup("");
        form->value.vlanAclConfig[list_counter].direction_ro = osapiStrDup("");
      }
      form->status.vlanAclConfig[list_counter].vlan_id_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      form->status.vlanAclConfig[list_counter].direction_ro   |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

      if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_IP)
      {
        form->value.vlanAclConfig[list_counter].acl_typ_ro        = osapiStrDup(pStrInfo_qos_IpAcl);
        if (usmDbQosAclNamedIndexCheckValid(unit, L7_ACL_TYPE_IP, listInfo.listEntry[i].aclId) == L7_SUCCESS)
        {
          if (usmDbQosAclNameGet(unit, listInfo.listEntry[i].aclId, aclName) == L7_SUCCESS)
          {
            form->value.vlanAclConfig[list_counter].acl_id_ro     = osapiStrDup(aclName);
          }
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf)-1, "%d",listInfo.listEntry[i].aclId);
          form->value.vlanAclConfig[list_counter].acl_id_ro         = osapiStrDup(buf);
        }
      }      /* IPv6 ACL Name related changes */
      else if(listInfo.listEntry[i].aclType == L7_ACL_TYPE_IPV6)
      {
        form->value.vlanAclConfig[list_counter].acl_typ_ro   = osapiStrDup(pStrInfo_qos_Ipv6Acl);
        if(usmDbQosAclNameGet(unit, listInfo.listEntry[i].aclId, aclName) == L7_SUCCESS)
        {
          form->value.vlanAclConfig[list_counter].acl_id_ro = osapiStrDup(aclName);
        }
      }
      else if (listInfo.listEntry[i].aclType == L7_ACL_TYPE_MAC)
      {
        form->value.vlanAclConfig[list_counter].acl_typ_ro = osapiStrDup(pStrInfo_qos_MacAcl_1);
        if (usmDbQosAclMacNameGet(unit, listInfo.listEntry[i].aclId, aclName) == L7_SUCCESS)
        {
          form->value.vlanAclConfig[list_counter].acl_id_ro = osapiStrDup(aclName);
        }
      }
      form->status.vlanAclConfig[list_counter].acl_typ_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      form->status.vlanAclConfig[list_counter].acl_id_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

      osapiSnprintf(buf, sizeof(buf)-1, "%u", listInfo.listEntry[i].seqNum);
      form->value.vlanAclConfig[list_counter].sequence_nbr_ro = osapiStrDup(buf);
      form->status.vlanAclConfig[list_counter].sequence_nbr_ro |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      list_counter++;
    }
  }

  return;
}
/*********************************************************************
*
* @purpose Handle the form submission
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the vlan_acl_cfg form
*
* @returns none
*
* @note  appInfo.data[0] - stores VLAN Id.
*        appInfo.data[1] - stores Direction
*        appInfo.data[2] - stores ACL Type
*        appInfo.data[20] - stores ACL ID;
*        appInfo.data[4] - stores Sequence Num;
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_vlan_acl_cfg(EwsContext context,
                                     EwaForm_vlan_acl_cfgP form)
{

  EwaNetHandle net = ewsContextNetHandle(context);
  usmWeb_AppInfo_t appInfo;
  L7_ACL_DIRECTION_t direction = L7_INBOUND_ACL;
  L7_uint32 user_access = usmWebUserAccessGet(context),
            seqNum=0, aclNum, vlanId, i, tempSeqNum,
            unit = usmDbThisUnitGet();
  L7_BOOL rmvFlag, errFlag = L7_FALSE;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_char8  buf[128];
  
  rmvFlag = L7_FALSE;  /* remove was successful or not */

  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  vlanId = form->value.vlan;
  appInfo.data[0] = vlanId;

  switch(form->value.direction)
  {
  case inbound:
    direction = L7_INBOUND_ACL;
    break;
  case outbound:
    direction = L7_OUTBOUND_ACL;
    break;
  default:
    direction = L7_INBOUND_ACL;
    break;
  }
  appInfo.data[1] = direction;
  appInfo.data[2] = 0;    /*acl type change submit*/

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    seqNum = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
    if(strcmp(form->value.sequence_nbr, "") != 0)
    {    
      osapiStrncpySafe(buf, form->value.sequence_nbr,sizeof(buf));
      if ((cliWebConvertTo32BitUnsignedInteger(buf, &seqNum) != L7_SUCCESS) ||
          (seqNum == 0) ||
          (seqNum < L7_ACL_MIN_INTF_SEQ_NUM) || 
          (seqNum > L7_ACL_MAX_INTF_SEQ_NUM))

      {
           errFlag = L7_TRUE;
        usmWebAppInfoCatErrMsg (&appInfo, pStrErr_common_Error, pStrInfo_qos_ValIsOutOfRangeForSeqNumValidRangeIsTo, L7_ACL_MIN_INTF_SEQ_NUM, L7_ACL_MAX_INTF_SEQ_NUM);
           usmWebEwsContextSendReply(context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileVlanAclCfg);
         }
      }
    
    if(errFlag == L7_FALSE)
    {
       if(form->value.acl_type == ip_acl)
       {
         if(usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, form->value.ip_acl_list, seqNum)!=L7_SUCCESS)
         {
           appInfo.err.err_flag = L7_TRUE;
           osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignIpAcl, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
         }
       }
       else if(form->value.acl_type == ipv6_acl)
       {
         if(usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, form->value.ipv6_acl_list, seqNum) !=L7_SUCCESS)
         {
           appInfo.err.err_flag = L7_TRUE;
           osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignIpv6Acl, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
         }
       }
       else if(form->value.acl_type == mac_acl)
       {
         if(usmDbQosAclMacVlanDirectionAdd(unit, vlanId, direction, form->value.mac_acl_list, seqNum)!=L7_SUCCESS)
         {
           appInfo.err.err_flag = L7_TRUE;
           osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignMacAcl, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
         }
       }
    }
  }
  else if (form->status.Remove & EW_FORM_RETURNED)
  {
    rmvFlag = L7_TRUE;
    if (form->value.acl_type == ip_acl)
    {
      if(usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, form->value.ip_acl_list) !=L7_SUCCESS)
      {
        rmvFlag = L7_FALSE;
        appInfo.err.err_flag = L7_TRUE;
        osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignIpAcl,(sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
      }
    }
    else if (form->value.acl_type == ipv6_acl)
    {
      if(usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, form->value.ipv6_acl_list) != L7_SUCCESS)
      {
        rmvFlag = L7_FALSE;
        appInfo.err.err_flag = L7_TRUE;
        osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignIpv6Acl, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
      }
    }
    else if (form->value.acl_type == mac_acl)
    {
      if (usmDbQosAclMacVlanDirectionRemove(unit, vlanId, direction, form->value.mac_acl_list) != L7_SUCCESS)
      {
        rmvFlag = L7_FALSE;
        appInfo.err.err_flag = L7_TRUE;
        osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrErr_qos_FailedToAssignMacAcl, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
      }
    }
  }

  if((form->value.acl_typ_change == ACL_TYPE_CHANGED) || rmvFlag == L7_TRUE)
  {
    if (user_access == USMWEB_READWRITE)
    {
      if (form->value.acl_type == ip_acl)
      {
        /* First check if any acls are assigned. If no acl is assigned, then the selected
         * one is the first created acl. */
        if (usmWebVlanAssignedAclFirstGet(vlanId, direction, L7_ACL_TYPE_IP, &aclNum, &tempSeqNum) == L7_SUCCESS )
        {
          appInfo.data[20] = aclNum;
          appInfo.data[4] = tempSeqNum;
        }
        else if ((usmDbQosAclNumGetFirst(unit, &aclNum) != L7_SUCCESS) && (usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IP, &aclNum) != L7_SUCCESS))
        {
          appInfo.err.err_flag = L7_TRUE;
          osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrInfo_qos_NoIpAclsAreCurrentlyCfgured, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
        }
        else
        {
          appInfo.data[20] = aclNum;
        }
        appInfo.data[2] = L7_ACL_TYPE_IP;
      }
      else if (form->value.acl_type == mac_acl)
      {
        if(usmWebVlanAssignedAclFirstGet(vlanId, direction, L7_ACL_TYPE_MAC, &aclNum, &tempSeqNum) == L7_SUCCESS )
        {
          appInfo.data[20] = aclNum;
          appInfo.data[4] = tempSeqNum;
        }
        else if(usmDbQosAclMacIndexGetFirst(unit, &aclNum) != L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrInfo_qos_NoMacAclsAreCurrentlyCfgured, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
        }
        else
        {
          appInfo.data[20] = aclNum;
        }
        appInfo.data[2] = L7_ACL_TYPE_MAC;
      }
      else if (form->value.acl_type == ipv6_acl)
      {
        /* First check if any ipv6 acls are assigned. If no acl is assigned, then the selected
         * one is the first created acl. */
        if (usmWebVlanAssignedAclFirstGet(vlanId, direction, L7_ACL_TYPE_IPV6, &aclNum, &tempSeqNum) == L7_SUCCESS )
        {
          appInfo.data[20] = aclNum;
          appInfo.data[4] = tempSeqNum;
        }
        else if(usmDbQosAclNamedIndexGetFirst(unit, L7_ACL_TYPE_IPV6, &aclNum) != L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          osapiStrncatAddBlanks (1, 0, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrInfo_qos_NoIpv6AclsAreCurrentlyCfgured, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg))-1);
        }
        else
        {
          appInfo.data[20] = aclNum;
        }
        appInfo.data[2] = L7_ACL_TYPE_IPV6;
      }
    }  /* user access check */
  }
  /* If user has changed direction */
  else if(form->value.acl_typ_change == ACL_DIRECTION_CHANGED)
  {
    appInfo.data[2] = 0;    /* default (no acl type selected) */
    if(usmDbQosAclVlanDirAclListGet(unit, vlanId, direction, &listInfo) == L7_SUCCESS)
    {
      appInfo.data[1] = direction;
      appInfo.data[2] = listInfo.listEntry[0].aclType;
      appInfo.data[20] = listInfo.listEntry[0].aclId;
      appInfo.data[4] = listInfo.listEntry[0].seqNum;
    }
  }
  /* If interface is changed */
  else if (form->value.acl_typ_change == ACL_VLAN_CHANGED)
  {
    appInfo.data[1] = L7_INBOUND_ACL;    /* default direction*/
    appInfo.data[2] = 0;
    if(usmDbQosAclVlanDirAclListGet(unit, vlanId, L7_INBOUND_ACL, &listInfo) == L7_SUCCESS)
    {
      appInfo.data[2] = listInfo.listEntry[0].aclType;
      appInfo.data[20] = listInfo.listEntry[0].aclId;
      appInfo.data[4] = listInfo.listEntry[0].seqNum;
    }
  }
  /* If ACL ID is selected */
  else if (form->value.acl_typ_change == ACL_ID_CHANGED ||
           form->status.Submit & EW_FORM_RETURNED)
  {
    /* normal acl id change or submit*/
    if (form->value.acl_type == ip_acl)
    {
      appInfo.data[2] = L7_ACL_TYPE_IP;
      appInfo.data[20] = form->value.ip_acl_list;
    }
    else if (form->value.acl_type == mac_acl)
    {
      appInfo.data[2] = L7_ACL_TYPE_MAC;
      appInfo.data[20] = form->value.mac_acl_list;
    }
    else if(form->value.acl_type == ipv6_acl)
    {
      appInfo.data[2] = L7_ACL_TYPE_IPV6;
      appInfo.data[20] = form->value.ipv6_acl_list;
    }

    if(usmDbQosAclVlanDirAclListGet(unit, vlanId, L7_INBOUND_ACL, &listInfo) == L7_SUCCESS)
    {
      for(i=0; i<= listInfo.count; i++)
      {
        if(listInfo.listEntry[i].aclType == appInfo.data[2]
           && appInfo.data[20] == listInfo.listEntry[i].aclId)
        {
          appInfo.data[4] = listInfo.listEntry[i].seqNum;
          break;
        }
      }
    }
  }

  return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileVlanAclCfg);
}
