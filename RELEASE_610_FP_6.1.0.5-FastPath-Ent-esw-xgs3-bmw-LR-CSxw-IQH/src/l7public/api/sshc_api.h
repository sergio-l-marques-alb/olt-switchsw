/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sshc_api.h
*
* @purpose SSHC API header
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
#ifndef INCLUDE_SSHC_API_H
#define INCLUDE_SSHC_API_H

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
L7_RC_t sshcTransferRemoteUsernameGet(L7_char8 *username);
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
L7_RC_t sshcTransferRemotePasswordGet(L7_char8 *password);
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
L7_RC_t sshcTransferRemoteUsernameSet(L7_char8 *username);
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
L7_RC_t sshcTransferRemotePasswordSet(L7_char8 *password);

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
                          L7_char8 *local_file, L7_uint32 cmd);

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
                         L7_char8 *local_file, L7_uint32 updownflag);

#endif /* INCLUDE_SSHC_API_H */
