#include "ptin_dhcp_database.h"
#include "ptin_dhcp_database_api.h"
#include "ptin_snoop_stats.h"
#include "ptin_flow.h"
#include "traces.h"

#define DHCP_RELAY_DATABASE_MAX_ENTRIES 1024
typedef struct _st_dhcp_relay_database {
  L7_uint8  active;
  struct {
    L7_uint16 intIfNum;
    L7_uint16 vlanId;
    L7_uint16 innerVlanId;
  } params;
  struct {
    L7_char8 circuitId[FD_DS_MAX_REMOTE_ID_STRING];
    L7_char8 remoteId[FD_DS_MAX_REMOTE_ID_STRING];
  } strings;
} st_dhcp_relay_database;

st_dhcp_relay_database dhcp_relay_database[DHCP_RELAY_DATABASE_MAX_ENTRIES];


static L7_RC_t ptin_dhcp_circuitId_validate(L7_uchar8 *circuitId);


// Initialize the DHCP database
// Returns:
//  L7_SUCCESS or L7_FAILURE
//
void ptin_dhcp_database_init(void)
{
  memset(dhcp_relay_database,0x00,sizeof(st_dhcp_relay_database)*DHCP_RELAY_DATABASE_MAX_ENTRIES);
}


// Configure an entry of the DHCP database
// Input:
//  intIfNum:      interface Id
//  vlanId:        Outer vlanId
//  innerVloanId:  Inner vlanId
//  circuitId:     String with circuitId reference
//  remoteId:      String with remoteId reference
// Returns:
//  L7_SUCCESS or L7_FAILURE
//
L7_RC_t ptin_dhcp_database_config(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_int i, free_pos=-1;

  /* Validate arguments */
  if (circuitId==L7_NULLPTR || *circuitId=='\0' ||
      remoteId==L7_NULLPTR || *remoteId=='\0')
  {
    printf("%s: Invalid arguments\r\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Validate circuit id string */
  if (ptin_dhcp_circuitId_validate(circuitId)!=L7_SUCCESS)
  {
    printf("%s: Circuit id is not valid!\r\n",__FUNCTION__);
    return L7_FAILURE;
  }

  // Find a free position
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    // Find the first free position
    if (free_pos<0 && !dhcp_relay_database[i].active)
      free_pos = i;

    // if this entry already exists, break loop
    if (dhcp_relay_database[i].active &&
        dhcp_relay_database[i].params.intIfNum==intIfNum &&
        dhcp_relay_database[i].params.vlanId==vlanId &&
        dhcp_relay_database[i].params.innerVlanId==innerVlanId )
      break;
  }

  // Entry not found, and no free entries... leave function
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES && free_pos<0)
    return L7_FAILURE;

  // Add this client for DHCP statistics
  if (ptin_dhcp_evcClient_add_fromIntfVlan(intIfNum,vlanId,innerVlanId)!=L7_SUCCESS)  {
    printf("%s: Error adding Client (%d) for DHCP stats\r\n",__FUNCTION__,innerVlanId);
    return L7_FAILURE;
  }
  printf("%s: Client (%d) added for DHCP stats\r\n",__FUNCTION__,innerVlanId);

  // If not found a configured entry with these parameters, use an empty position
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
    i = free_pos;

  // Fill pos with data
  dhcp_relay_database[i].params.intIfNum    = intIfNum;
  dhcp_relay_database[i].params.vlanId      = vlanId;
  dhcp_relay_database[i].params.innerVlanId = innerVlanId;
  if (circuitId!=L7_NULLPTR)
    memcpy(dhcp_relay_database[i].strings.circuitId,circuitId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);
  if (remoteId!=L7_NULLPTR)
    memcpy(dhcp_relay_database[i].strings.remoteId,remoteId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);

  dhcp_relay_database[i].active = L7_TRUE;

  return L7_SUCCESS;
}

// Remove an entry of the DHCP database
// Input:
//  intIfNum:      interface Id
//  vlanId:        Outer vlanId
//  innerVlanId:   Inner vlanId
// Returns:
//  L7_SUCCESS or L7_FAILURE
//
L7_RC_t ptin_dhcp_database_remove(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId)
{
  L7_uint16 i, entry;
  L7_uint8  client_still_exists;

  // Find table position
  entry = DHCP_RELAY_DATABASE_MAX_ENTRIES;
  client_still_exists = L7_FALSE;
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    if (dhcp_relay_database[i].active &&
        dhcp_relay_database[i].params.intIfNum==intIfNum &&
        dhcp_relay_database[i].params.vlanId==vlanId)  {

      if (dhcp_relay_database[i].params.innerVlanId==innerVlanId)
        entry = i;
    }
    else {
      if (dhcp_relay_database[i].params.innerVlanId==innerVlanId)
        client_still_exists = L7_TRUE;
    }
  }

  // No entry found
  if (entry>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
    return L7_SUCCESS;

  // Remove client from DHCP stats
  if (!client_still_exists)  {
    if (ptin_dhcp_evcClient_remove_fromIntfVlan(intIfNum,vlanId,innerVlanId)!=L7_SUCCESS)  {
      printf("%s: Error removing Client (%d) for DHCP stats",__FUNCTION__,innerVlanId);
      return L7_FAILURE;
    }
    printf("%s: Client (%d) removed for DHCP stats",__FUNCTION__,innerVlanId);
  }

  // Reset data
  memset(&dhcp_relay_database[entry],0x00,sizeof(st_dhcp_relay_database));
  dhcp_relay_database[entry].active = L7_FALSE;

  return L7_SUCCESS;
}

// Get the circuitId and remoteId of an entry in the DHCP database
// Input:
//  intIfNum:      interface Id
//  vlanId:        Outer vlanId
//  innerVlanId:   Inner vlanId
// Output:
//  circuitId:     String with circuitId reference
//  remoteId:      String with remoteId reference
// Returns:
//  L7_SUCCESS or L7_FAILURE
//
L7_RC_t ptin_dhcp_database_get(L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 innerVlanId, L7_char8 *circuitId, L7_char8 *remoteId)
{
  L7_int i;

  // Find a free position
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    // if this entry already exists, break loop
    if (dhcp_relay_database[i].active &&
        dhcp_relay_database[i].params.intIfNum==intIfNum &&
        dhcp_relay_database[i].params.vlanId==vlanId &&
        dhcp_relay_database[i].params.innerVlanId==innerVlanId )
      break;
  }
  // Entry not found... leave function
  if (i>=DHCP_RELAY_DATABASE_MAX_ENTRIES)
  {
    if (circuitId!=L7_NULLPTR)  *circuitId='\0';
    if (remoteId!=L7_NULLPTR)   *remoteId ='\0';
    return L7_FAILURE;
  }

  // Fill strings with data
  if (circuitId!=L7_NULLPTR)
    memcpy(circuitId,dhcp_relay_database[i].strings.circuitId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);
  if (remoteId!=L7_NULLPTR)
    memcpy(remoteId,dhcp_relay_database[i].strings.remoteId,sizeof(L7_char8)*FD_DS_MAX_REMOTE_ID_STRING);

  return L7_SUCCESS;
}


/**
 * Update circuit id for all database entries
 * 
 * @param access_node_id_arg : New access node identifier 
 * @param l2_type_arg : New L2 type 
 * @param slot_field_arg : New slot field 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_dhcp_database_accessNodeId_update(L7_char8 *access_node_id_arg, L7_uchar8 *l2_type_arg, L7_uchar8 *slot_field_arg)
{
  L7_int i, j;
  char *circuitId;

  char str_new[FD_DS_MAX_REMOTE_ID_STRING];
  char circuitId_suffix[FD_DS_MAX_REMOTE_ID_STRING];

  char accessNodeId[FD_DS_MAX_ACCESS_NODE_ID_STRING];
  char l2type[FD_DS_MAX_L2_TYPE_STRING];
  char slot[FD_DS_MAX_SLOT_STRING];

  /* Copy and truncate access node identifier */
  strncpy(accessNodeId, access_node_id_arg, FD_DS_MAX_ACCESS_NODE_ID_STRING);
  accessNodeId[FD_DS_MAX_ACCESS_NODE_ID_STRING-1] = '\0';
  /* Replace space characters from access_node_id to '_' */
  for (j=0; j<strlen(accessNodeId); j++)
  {
    if (accessNodeId[j]==' ')  accessNodeId[j]='_';
  }

  /* Copy and truncate L2 type */
  strncpy(l2type, l2_type_arg, FD_DS_MAX_L2_TYPE_STRING);
  l2type[FD_DS_MAX_L2_TYPE_STRING-1] = '\0';
  /* Replace space characters from l2_type to '_' */
  for (j=0; j<strlen(l2type); j++)
  {
    if (l2type[j]==' ')  l2type[j]='_';
  }

  /* Copy and truncate slot field */
  strncpy(slot, slot_field_arg, FD_DS_MAX_SLOT_STRING);
  slot[FD_DS_MAX_SLOT_STRING-1] = '\0';
  /* Replace space and ':' characters from slot to '_' */
  for (j=0; j<strlen(slot); j++)
  {
    if (slot[j]==' ' || slot[j]==':')  slot[j]='_';
  }

  /* Length validation (2 spaces around l2type added, and one '/' character added) */
  if ( (strlen(accessNodeId) + strlen(l2type) + strlen(slot) + 3) >= (FD_DS_MAX_REMOTE_ID_STRING-1) )
  {
    printf("%s(%d) Strings to be replaced are too big\r\n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /* Form new string */
  strcpy(str_new, accessNodeId);
  strcat(str_new," ");
  strcat(str_new, l2type);
  strcat(str_new," ");
  strcat(str_new, slot);
  strcat(str_new,"/");

  printf("%s: New string for the circuit id: \"%s\"\r\n", __FUNCTION__, str_new);

  /* Run all used entries in database */
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    /* Process only active entries */
    if (!dhcp_relay_database[i].active)  continue;

    /* Pointer to circuit id */
    circuitId = dhcp_relay_database[i].strings.circuitId;

    /* Find first space character */
    for (j=0; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]!='\0'; j++)
    {
      if (circuitId[j]==' ')  break;
    }
    if (j>=FD_DS_MAX_REMOTE_ID_STRING || circuitId[j]=='\0')
    {
      printf("%s(%d) Invalid entry at index %u (circuitId=\"%s\")\r\n",__FUNCTION__,__LINE__,i,circuitId);
      continue;
    }

    /* Next character (space) */
    for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]==' '; j++);

    /* Find second space character */
    for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]!='\0'; j++)
    {
      if (circuitId[j]==' ')  break;
    }
    if (j>=FD_DS_MAX_REMOTE_ID_STRING || circuitId[j]=='\0')
    {
      printf("%s(%d) Invalid entry at index %u (circuitId=\"%s\")\r\n",__FUNCTION__,__LINE__,i,circuitId);
      continue;
    }

    /* Next character (space) */
    for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]==' '; j++);

    /* Find the ':' character */
    for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]!='\0'; j++)
    {
      if (circuitId[j]==':')  break;
    }
    if (j>=FD_DS_MAX_REMOTE_ID_STRING || circuitId[j]=='\0')
    {
      printf("%s(%d) Invalid entry at index %u (circuitId=\"%s\")\r\n",__FUNCTION__,__LINE__,i,circuitId);
      continue;
    }

    /* Character before */
    j--;
    /* Find '/' or space character */
    for ( ; j>=0; j--)
    {
      if (circuitId[j]=='/' || circuitId[j]==' ')  break;
    }
    if (j<0)
    {
      printf("%s(%d) Invalid entry at index %u (circuitId=\"%s\")\r\n",__FUNCTION__,__LINE__,i,circuitId);
      continue;
    }
    /* Next character */
    j++;

    /* Save the string that will not be replaced */
    strncpy(circuitId_suffix, &circuitId[j], FD_DS_MAX_REMOTE_ID_STRING-j);
    circuitId_suffix[FD_DS_MAX_REMOTE_ID_STRING-j-1] = '\0';

    printf("%s: Index %u => String to be maintained: \"%s\"\r\n", __FUNCTION__, i, circuitId_suffix);

    /* Copy circuit id prefix to destination */
    strcpy(circuitId, str_new);
    strcat(circuitId, circuitId_suffix);

    printf("%s(%d) Index=%u => New circuit id string (intIfNum=%u, vlan=%u, innerVlan=%u) is \"%s\"\r\n",__FUNCTION__,__LINE__,
           i, dhcp_relay_database[i].params.intIfNum, dhcp_relay_database[i].params.vlanId, dhcp_relay_database[i].params.innerVlanId, circuitId);
  }

  return L7_SUCCESS;
}


/**
 * Extract the PON interface from the circuit Id. 
 * 
 * @author mruas (11/13/2012)
 * 
 * @param circuitId : circuit Id string
 * @param len : maximum length
 * @param dstPort : destination port to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_dhcp_database_dstPort_get(L7_uchar8 *circuitId, L7_int len, L7_int *dstPort)
{
  L7_int port;
  L7_int j, index0, index1;

  /* Validate arguments */
  if (circuitId==L7_NULLPTR || *circuitId=='\0' || len==0)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                "%s: Invalid parameters\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Find the first space (before l2_type) */
  for (j=0; j<len && circuitId[j]!='\0'; j++)
  {
    if (circuitId[j]==' ')  break;
  }
  if (j>=len || circuitId[j]=='\0')
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                "%s: Cannot find first space\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Next character (space) */
  for ( ; j<len && circuitId[j]==' '; j++);

  /* Find second space character */
  for ( ; j<len && circuitId[j]!='\0'; j++)
  {
    if (circuitId[j]==' ')  break;
  }
  if (j>=len || circuitId[j]=='\0')
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                "%s: Cannot find second space\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Next character (space) */
  for ( ; j<len && circuitId[j]==' '; j++);

  /* Find ':' character */
  for ( ; j<len && circuitId[j]!='\0'; j++)
  {
    if (circuitId[j]==':')  break;
  }
  if (j>=len || circuitId[j]=='\0')
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                "%s: Cannot find ':' character\n",__FUNCTION__);
    return L7_FAILURE;
  }
  /* One character back */
  j--;

  /* PON interface ends here */
  index1 = j;

  /* Go back and search for the '/' or space character */
  for ( ; j>=0; j--)
  {
    if (circuitId[j]=='/' || circuitId[j]==' ')
      break;
  }
  if (j<0)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                "%s: Cannot find '/' or space character\n",__FUNCTION__);
    return L7_FAILURE;
  }
  /* One character forth */
  j++;

  /* PON interface starts here */
  index0 = j;

  port = 0;
  for (j=index0; j<=index1; j++)
  {
    if (circuitId[j]<'0' || circuitId[j]>'9')
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,
                  "%s: Not numerical characters for the PON\n",__FUNCTION__);
      break;
    }

    /* Update port value */
    port *= 10;
    port += (circuitId[j]-'0');
  }

  /* Return PON interface */
  if (dstPort!=L7_NULLPTR)  *dstPort = port;

  return L7_SUCCESS;
}


static L7_RC_t ptin_dhcp_circuitId_validate(L7_uchar8 *circuitId)
{
  L7_int j, k, pos;

  /* Circuit id, must have two separated space characters, followed by a '/' character, and finally a ':' character */

  /* Find first space */
  for (j=0; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]!='\0'; j++)
  {
    if (circuitId[j]==' ')  break;
  }
  if (j>=FD_DS_MAX_REMOTE_ID_STRING || circuitId[j]=='\0')
    return L7_FAILURE;

  /* Next character */
  for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]==' '; j++);

  /* Find second space */
  for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]!='\0'; j++)
  {
    if (circuitId[j]==' ')  break;
  }
  if (j>=FD_DS_MAX_REMOTE_ID_STRING || circuitId[j]=='\0')
    return L7_FAILURE;

  /* Next character */
  for ( ; j<FD_DS_MAX_REMOTE_ID_STRING && circuitId[j]==' '; j++);

  /* Find the last ocurrence of the ':' character */
  pos = -1;
  for (k=j; k<FD_DS_MAX_REMOTE_ID_STRING && circuitId[k]!='\0'; k++)
  {
    if (circuitId[k]==':')  pos=k;
  }
  /* If there is no ':' character, this string is invalid */
  if (pos==-1)
    return L7_FAILURE;

  /* At this point 'pos' tells us where is the last ':' character */
  /* Run all positions from j to pos-1, and replace ':' and space characters to '_' */
  for (k=j; k<pos; k++)
  {
    if (circuitId[k]==':' || circuitId[k]==' ')  circuitId[k]='_';
  }

  /* Before the ':' we should have a '/' character */
  for ( ; j<pos; j++)
  {
    if (circuitId[j]==' ' || circuitId[j]=='/')  break;
  }
  if (j>=pos || circuitId[j]!='/')
    return L7_FAILURE;

  /* Success */
  return L7_SUCCESS;
}


L7_RC_t ptin_dhcp_database_dump(void)
{
  L7_int i, pon_port;

  /* Run all used entries in database */
  for (i=0; i<DHCP_RELAY_DATABASE_MAX_ENTRIES; i++)
  {
    /* Process only active entries */
    if (!dhcp_relay_database[i].active)  continue;

    pon_port=-1;
    ptin_dhcp_database_dstPort_get(dhcp_relay_database[i].strings.circuitId, strlen(dhcp_relay_database[i].strings.circuitId), &pon_port);

    printf("[Index=%03u] intIfNum=%2u, vlan=%4u, innerVlan=%4u: CircuitId=\"%s\" RemoteId=\"%s\" [pon=%d]\r\n",i,
           dhcp_relay_database[i].params.intIfNum, dhcp_relay_database[i].params.vlanId, dhcp_relay_database[i].params.innerVlanId,
           dhcp_relay_database[i].strings.circuitId,
           dhcp_relay_database[i].strings.remoteId,
           pon_port);
  }

  return L7_SUCCESS;
}

