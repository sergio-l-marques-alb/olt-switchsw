/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename ews_common.h
*
* @purpose header for different mode structures
*
* @component user interface
*
* @comments none
*
* @create  20/06/2003
*
* @author  Jagdish
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef EWS_COMMON_H
#define EWS_COMMON_H


#define L7_TYPE_NAME_SIZE 32
#define L7_CLASS_NAME_SIZE 32
#define L7_SERVICE_NAME_SIZE 32
#define L7_MAINT_DOMAIN_NAME_SIZE 43 /* L7_DOT1AG_MD_NAME_LEN */
#define L7_POLICY_NAME_SIZE 32
#define L7_PCLASS_NAME_SIZE 32

#define L7_POOL_NAME_SIZE 32
#define L7_CLI_MAX_STRING_LENGTH  256

#define L7_MAC_ADDR_SIZE 6
#define L7_HOST_NAME_LEN_MAX      255

/* Structure to save the acl mode info*/
typedef struct
  {
    unsigned int aclid;
    unsigned int rulenum;
}ews_aclContext_t;


/* Structure to save the bwProvisioning mode info*/
typedef struct
{
char strType[L7_TYPE_NAME_SIZE];
char strClassName[L7_CLASS_NAME_SIZE];
char strBWAllocationName[L7_CLASS_NAME_SIZE];
}ews_bwpContext_t;


/* Structure to save the interface config mode info*/
typedef struct
{
  unsigned int unit;
  unsigned int slot;
  unsigned int port;
  unsigned int ifId;
}ews_ifContext_t;

typedef struct
{
  char strHost[L7_HOST_NAME_LEN_MAX];
} ews_tacacsContext_t;


typedef struct
{
  int start;
  int end;
}cliRange_t;

typedef struct
{
  cliRange_t range[L7_CLI_MAX_STRING_LENGTH];
} ews_rangeContext_t;

/* Following structure variables controls the running configuration
   by avoiding non configured modes in the output */

 /* we have treated that at most five inner modes will be there */

struct  ews_runningConfigContext_t
{
  boolean   showExit[5];
  boolean   showMode[5];
  char      modeStr[5][256];
  boolean   hideMode;
  boolean  init;
  unsigned int  count;
  unsigned int  current;
  unsigned int  showRunCounter;  /*for controlling show running config and  all output */
};

/* Structure to save the Line Config Name info */
typedef struct
{
  unsigned int lineConfigType;
} ews_lineConfigContext_t;

/* Structure to save the mac Access List mode info*/
typedef struct
{
  char strAccessListName[L7_CLASS_NAME_SIZE];
}ews_accessListContext_t;

/* Structure to save the dot1ad service mode info*/
typedef struct
{
  char strDot1adServiceName[L7_SERVICE_NAME_SIZE];
}ews_dot1adServiceContext_t;

/* Structure to save the dot1ag mode info*/
typedef struct
{
  char         domainName[L7_MAINT_DOMAIN_NAME_SIZE+1];
  int          mdLevel;
}ews_dot1agDomainContext_t;

/* Structure to save the ARP Access List mode info */
typedef struct
{
  char strArpAclName[L7_CLASS_NAME_SIZE];
}ews_arpAclContext_t;

/* Structure to save the AS mode info*/
typedef struct
{
unsigned int asnumber;
}ews_routerContext_t;


/* Structure to save the policyMap and PolicyClassMap mode info*/
typedef struct
{
char strPolicyName[L7_POLICY_NAME_SIZE];
char strClassName[L7_PCLASS_NAME_SIZE];
}ews_policymapContext_t;

/* Structure to save the class Map info*/
typedef struct
{
char strClassName[L7_CLASS_NAME_SIZE];
unsigned int aclType;
unsigned int aclId;
}ews_classmapContext_t;

/* Structure to save the Pool name info*/
typedef struct
{
char strPoolName[L7_POOL_NAME_SIZE];
}ews_dhcpsPoolConfigContext_t;

/* Structure to save the Pool name info*/
typedef struct
{
char strPoolName[L7_POOL_NAME_SIZE];
}ews_dhcp6sPoolConfigContext_t;

/* Structure to save the Wireless AP MAC Address info */
typedef struct
{
char macAddr[L7_MAC_ADDR_SIZE];
}ews_wirelessAPConfigContext_t;

/* Structure to save the Wireless Network ID info */
typedef struct
{
unsigned long networkId;
}ews_wirelessNetworkConfigContext_t;

/* Structure to save the Wireless AP Profile ID info */
typedef struct
{
unsigned long profileId;
}ews_wirelessAPProfileConfigContext_t;

/* Structure to save the Wireless AP Profile ID info */
typedef struct
{
unsigned long profileId;
unsigned long radioIndex;
}ews_wirelessAPProfileRadioConfigContext_t;

/* Structure to save the Wireless AP Profile ID VAP info */
typedef struct
{
unsigned long profileId;
unsigned long radioIndex;
unsigned long vapId;
}ews_wirelessAPProfileVAPConfigContext_t;

/* Structure to save the Captive Portal Id */
typedef struct
{
  unsigned short cpId;
  unsigned short webId;
}ews_captivePortalIdConfigContext_t;

#endif

