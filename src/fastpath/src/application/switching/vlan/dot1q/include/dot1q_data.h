/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename dot1q_data.c
*
* @purpose  data prototypes for dot1q
*
* @component dot1q
*
* @comments none
*
* @create 11/1/2005
*
* @author akulkarni
*
* @end
*             
**********************************************************************/
#ifndef DOT1Q_DATA_H__
#define DOT1Q_DATA_H__

#define vlanTree_t          avlTree_t
#define vlanTreeTables_t    avlTreeTables_t

#define vlanTreeCreate      avlCreateAvlTree

/* Data Management */
void *    vlanDataSearch(void *pTree, L7_uint32 vlanID);
L7_BOOL	  vlanDataGet(void *pTree, L7_uint32 vlanID, dot1q_vlan_dataEntry_t *pData);
L7_RC_t   vlanEntryAdd(vlanTree_t *pTree, void * pData);
L7_RC_t   vlanEntryDelete(vlanTree_t *pTree, L7_uint32 vlanID);
L7_uint32 vlanDataCount(vlanTree_t *pTree);
void*     vlanFirstDataEntry(vlanTree_t *pTree);
void*     vlanNextDataEntry(vlanTree_t *pTree, L7_uint32 vlanID);
L7_RC_t vlanFirstDataEntryGet(vlanTree_t *pTree, dot1q_vlan_dataEntry_t *pData);
L7_RC_t vlanNextDataEntryGet(vlanTree_t *pTree, L7_uint32 vlanID, dot1q_vlan_dataEntry_t *pData);

#endif
