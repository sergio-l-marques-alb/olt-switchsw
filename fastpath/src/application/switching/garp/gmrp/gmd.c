/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    gmd.c
* @purpose     GMRP data base
* @component   GARP
* @comments    none
* @create      03/06/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/
/*************************************************************

*************************************************************/


#include "string.h"
#include "l7_common.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "avl_api.h"
#include "garp.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gmrapi.h"
#include "gmd.h"
#include "dll_api.h"
#include "mfdb_api.h"
#include "gmrp_api.h"
#include "garp_debug.h"

extern GarpInfo_t garpInfo;

/*********************************************************************
* @purpose  destroy DB
*
* @param    void *            pointer to DB
*
* @returns  None
*
* @notes   Destroys the instance of gvd, releasing previously allocated database and
*          control space.
*
* @end
*********************************************************************/
void gmd_destroy_gmd(void *gmd)
{ /*delete all members of this dll then proceed to destroy the dll itself*/
  L7_dll_member_t *local_member;
  L7_dll_t *local_gmd;
  gmrp_index_t *local_index;
  L7_ushort16 vlanId;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_RC_t rc;

  local_gmd = (L7_dll_t *)gmd;


  while(DLLFirstGet(local_gmd,&local_member)==L7_SUCCESS)
  {
    local_index = (gmrp_index_t *)local_member->data;
    /*need to delete these entries from the the mfdb too!!!*/
    memcpy((void *)&vlanId,(void *)local_index->vlanIdmacAddress,2);
    memcpy((void *)macAddr,(void *)&local_index->vlanIdmacAddress[2],L7_MAC_ADDR_LEN);
    /* we are going to delete the GMRP entry in gmr_destroy_gmr. This is
     * going to result in fewer calls to HAPI as we are not going to send
     * group registration modify events for each gid
     */
    #if 0
      rc = gmrpEntryDelete((L7_uint32)vlanId,macAddr);
    #endif
    /*delete dll member*/
    rc = DLLMemberDelete(local_gmd,local_member);

    /*free our space*/
    osapiFree(L7_GMRP_COMPONENT_ID, local_index);

        /* Reset the data to NULL as this data pointer is already freed.*/
        local_member->data = L7_NULLPTR;

    /*free dll member */
    rc = DLLMemberDestroy(local_member);
  }
  /*now destroy the dll itself*/
  rc = DLLDestroy(local_gmd);

  return;

}

/*********************************************************************
* @purpose  find DB entry
*
* @param    void *            pointer to DB
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmd_find_entry(  void *my_gmd, L7_uchar8 *key,
                         L7_uint32 *found_at_index)
{
  L7_uint32 size=0;
  L7_dll_t *local_gmd;
  L7_dll_member_t *local_member;
  L7_RC_t rc=L7_FAILURE;
  gmrp_index_t *local_index;
  local_gmd = (L7_dll_t *)my_gmd;

  *found_at_index = 0;
  rc = DLLSizeGet(local_gmd,&size);
  if(size > 0 )
  {
    rc = DLLFirstGet(local_gmd,&local_member);
    while(rc==L7_SUCCESS)
    {
      local_index = (gmrp_index_t *)local_member->data;
      if (memcmp((void*)key,(void*)local_index->vlanIdmacAddress,2+L7_MAC_ADDR_LEN)==0)
      {
        /*found my key return with the index it was found at*/
        return L7_TRUE;
      }
      else
      {
        (*found_at_index)++;
        rc = DLLNextGet(&local_member);
      }
    }
  }
  else
    return L7_FALSE;

  /*if we reach here then we did not find the entry so return failure*/
  return L7_FALSE;

 }

/*********************************************************************
* @purpose  create DB entry
*
* @param    void *            pointer to DB
* @param    Vlan_id           key
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmd_create_entry(void *my_gmd, L7_uchar8 *key,
                         L7_uint32 *created_at_index)
{
  L7_dll_member_t *local_member;
  L7_dll_t *local_gmd;
  L7_RC_t rc;
  gmrp_index_t *local_index;
  L7_uint32 created_at=0;
  mfdbMemberInfo_t memInfo;
  L7_ushort16 vlanId;
  L7_char8  buf[32];
  L7_uint32 i;

  /*bzero the info structure*/
  bzero((char*)&memInfo,(L7_int32)sizeof(mfdbMemberInfo_t));

  /*update the structure*/
  memcpy((void *)&vlanId,(void *)key, 2);
  memcpy((void *)memInfo.macAddr,(void *)&key[2],L7_MAC_ADDR_LEN);

  memInfo.vlanId = vlanId;
  memInfo.user.componentId = L7_MFDB_PROTOCOL_GMRP;
  memcpy((void *)memInfo.user.description,(void *)L7_MFDB_NETWORK_CONFIGURED,L7_MFDB_COMPONENT_DESCR_STRING_LEN);
  memInfo.user.type = L7_MFDB_TYPE_DYNAMIC;

  /* Set up debug buffer */
  memset(buf, 0,sizeof(buf));
  for (i = 0;i < L7_MAC_ADDR_LEN ;i++)
  {
      sprintf(buf,"%02x:",memInfo.macAddr[i]);
  }

  GARP_TRACE_INTERNAL_EVENTS("gmd_create_entry: vlan %d, address: %s\n",
                             vlanId, buf);

  /*need to check if we can accomodate this entry in the mfdb*/
  rc = mfdbEntryExist((char *)memInfo.macAddr,(L7_uint32)vlanId);
  if(rc==L7_NOT_EXIST)
  {
    rc = mfdbEntryAdd(&memInfo);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_FAILURE || rc == L7_TABLE_IS_FULL)
      {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GMRP_COMPONENT_ID,
              "GMRP failure adding MFDB entry: vlan %d and address %s."
              " MFDB table is full.", vlanId, buf);
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GMRP_COMPONENT_ID,
              "GMRP failure adding MFDB entry: vlan %d and address %s."
              " MAC Address is in restricted range.", vlanId, buf);
      }
          return L7_FALSE;
      }
      garpInfo.infoCounters.gmrpRegistationCount++;
  }

  local_gmd = (L7_dll_t *)my_gmd;

  local_index = (gmrp_index_t *)osapiMalloc(L7_GMRP_COMPONENT_ID, (L7_uint32)sizeof(gmrp_index_t));
  if (local_index == L7_NULL)
  {
    if(mfdbEntryDelete(&memInfo) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                "GMRP failure undoing MFDB entry: vlan %d and address %s\n", vlanId, buf);
    }

    garpInfo.infoCounters.gmrpRegistationCount--;
    return L7_FALSE;
  }
  /*copy the key into the local_index*/
  memcpy((void*)local_index->vlanIdmacAddress,key,2+L7_MAC_ADDR_LEN);
  /*create the dll member first and then insert it at the bottom of the gmd dll*/
  if (DLLMemberCreate(&local_member,(void*)local_index)!= L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GMRP failure adding DLL entry: vlan %d and address %s\n", vlanId, buf);
      if (mfdbEntryDelete(&memInfo) != L7_SUCCESS)
      {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
                  "GMRP failure undoing MFDB after DLL failure: vlan %d and address %s\n", vlanId, buf);
          osapiFree(L7_GMRP_COMPONENT_ID, local_index);
          return L7_FALSE;
       }
      garpInfo.infoCounters.gmrpRegistationCount--;
  }

  if ( DLLBottomInsert(local_gmd,local_member)!= L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "GMRP failure with DLL insertion: vlan %d and address %s\n", vlanId, buf);
      (void) mfdbEntryDelete(&memInfo);
      garpInfo.infoCounters.gmrpRegistationCount--;
      osapiFree(L7_GMRP_COMPONENT_ID, local_index);
      (void) DLLMemberDestroy(local_member);
      return L7_FALSE;
  }

  (void) DLLSizeGet(local_gmd,&created_at);
  *created_at_index = created_at - 1;

  return L7_TRUE;



}

/*********************************************************************
* @purpose  delete DB entry
*
* @param    void *            pointer to DB
* @param    L7_uint32         index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmd_delete_entry(void *my_gmd, L7_uint32  delete_at_index)
{
  L7_dll_member_t *local_member;
  L7_dll_t *local_gmd;
  L7_RC_t rc;
  gmrp_index_t *local_index;

  local_gmd = (L7_dll_t *)my_gmd;

  {
      /* Debug Block */
      L7_ushort16   vlanId;
      L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
      L7_uchar8     buf[8];
      L7_uint32     i;

      if (DLLFirstGet(local_gmd,&local_member)==L7_SUCCESS)
      {
        local_index = (gmrp_index_t *)local_member->data;
        memcpy((void *)&vlanId,(void *)local_index->vlanIdmacAddress,2);
        memcpy((void *)macAddr,(void *)&local_index->vlanIdmacAddress[2],L7_MAC_ADDR_LEN);

        memset(buf, 0,sizeof(buf));
        for (i = 0;i < L7_MAC_ADDR_LEN ;i++)
        {
            sprintf(buf,"%02x:",macAddr[i]);
        }
        GARP_TRACE_INTERNAL_EVENTS("gmd_delete_entry: vlan %d, address: %s\n",
                                 vlanId, buf);
      } else {
        GARP_TRACE_INTERNAL_EVENTS("gmd_delete_entry: failed to get first entry\n");
      }
  }  /* end debug block */


  rc = DLLIndexGet(local_gmd,&local_member,delete_at_index);
  if(rc==L7_FAILURE)
    return L7_FALSE;
  local_index = (gmrp_index_t *)local_member->data;

  /*free our space*/
  osapiFree(L7_GMRP_COMPONENT_ID, local_index);
  /*delete dll member*/
  rc = DLLMemberDelete(local_gmd,local_member);
  /* Reset the data to NULL as this data pointer is already freed.*/
  local_member->data = L7_NULLPTR;
  /*free dll member */
  rc = DLLMemberDestroy(local_member);

  return L7_TRUE;

}

/*********************************************************************
* @purpose  get the 6 byte mac address
*
* @param    L7_uint32         index
* @param    mac address*           key
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmd_get_key(void *my_gmd, L7_uint32 index, L7_uchar8 *key)
{
  L7_dll_member_t *local_member;
  L7_dll_t *local_gmd;
  L7_RC_t rc;
  gmrp_index_t *local_index;

  local_gmd = (L7_dll_t *)my_gmd;
  rc = DLLIndexGet(local_gmd,&local_member,index);
  if(rc==L7_FAILURE)
    return L7_FALSE;
  local_index = (gmrp_index_t*)local_member->data;
  memcpy(key,&local_index->vlanIdmacAddress[2],L7_MAC_ADDR_LEN);
  return L7_TRUE;
}

/*********************************************************************
* @purpose  creates DB
*
* @PARAM    L7_uint32         MAX VLANS
* @param    void *            pointer to DB
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  Creates a new instance of gmd, allocating space for up to max_vlans
*         VLAN IDs.
*
*         Returns True if the creation suceeded together with a pointer to the
*         gvd information.
*
* @end
*********************************************************************/
L7_BOOL gmd_create_gmd(void **gmd)
{
  L7_dll_t *local_gmd;

  if(DLLCreate(&local_gmd) == L7_FAILURE)
  {
    return L7_FALSE;
  }
  else
  {
    *gmd = local_gmd;
    return L7_TRUE;
  }
}
