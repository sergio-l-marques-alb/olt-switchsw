/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sshc_api.c
*
* @purpose SSHC API functions
*
* @component sshc
*
* @comments none
*
* @create 03/28/2006
*
* @author ikiran
*
* @end
*
**********************************************************************/
#include "sshc_include.h"
#include "sshd_exports.h"

extern int sftpc_main(char *host, int address_family, char *username, char *password,
                      unsigned int clisocket, char *remote_file,
                      char *local_file, unsigned int cmd);
extern int scp_main(char *hostname, int address_family, char *username, char *password,
                    unsigned int clisocket, char *remote_file,
                    char *local_file, unsigned int updownflag);

L7_char8 sshc_username[L7_SSHC_USERNAME_SIZE_MAX];
L7_char8 sshc_password[L7_SSHC_PASSWORD_SIZE_MAX];

/*********************************************************************
*
* @purpose  Get the remote user name
*
* @param    username  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshcTransferRemoteUsernameGet(L7_char8 *username)
{
  osapiStrncpySafe(username, sshc_username, sizeof(sshc_username));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the remote password
*
* @param    password  @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshcTransferRemotePasswordGet(L7_char8 *password)
{
  osapiStrncpySafe(password, sshc_password, sizeof(sshc_username));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the remote user name
*
* @param    username  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshcTransferRemoteUsernameSet(L7_char8 *username)
{
  osapiStrncpySafe(sshc_username, username, sizeof(sshc_username));
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Set the remote password
*
* @param    password  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshcTransferRemotePasswordSet(L7_char8 *password)
{
  if (password == L7_NULLPTR)
  {
    memset(sshc_password, '\0', sizeof(sshc_password));
    return L7_SUCCESS;
  }
  osapiStrncpySafe(sshc_password, password, sizeof(sshc_password));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Begin SFTP file transfer
*
* @param    host            @b{(input)}
* @param    address_family  @b{(input)}
* @param    clisocket       @b{(input)}
* @param    remote_file     @b{(input)}
* @param    local_file      @b{(input)}
* @param    cmd             @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_int32 sshcTransferSftp(L7_char8 *host, L7_int32 address_family,
                          L7_uint32 clisocket, L7_char8 *remote_file,
                          L7_char8 *local_file, L7_uint32 cmd)
{
  return sftpc_main(host, address_family, sshc_username, sshc_password,
                      clisocket, remote_file, local_file, cmd);
}

/*********************************************************************
*
* @purpose  Begin SCP file transfer
*
* @param    host            @b{(input)}
* @param    address_family  @b{(input)}
* @param    clisocket       @b{(input)}
* @param    remote_file     @b{(input)}
* @param    local_file      @b{(input)}
* @param    updownflag      @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_int32 sshcTransferScp(L7_char8 *hostname, L7_int32 address_family,
                         L7_uint32 clisocket, L7_char8 *remote_file,
                         L7_char8 *local_file, L7_uint32 updownflag)
{
  return scp_main(hostname, address_family, sshc_username, sshc_password,
                  clisocket, remote_file, local_file, updownflag);
}

