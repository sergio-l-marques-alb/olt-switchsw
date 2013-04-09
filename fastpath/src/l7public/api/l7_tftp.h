/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  tftp.h
*
* @purpose   This is the header file for TFTP.
*
* @component software
*
* @comments
*
* @create
*
* @author    Kunal Kapila
*
* @end
*
*********************************************************************/

#ifndef L7_TFTP_H
#define L7_TFTP_H

#include <stdio.h>
#include "l7_common.h"
#include "l3_addrdefs.h"

/* TFTP constant values.  */

#define TFTP_SEGSIZE        512 /* data segment size    */
#define TFTP_TIMEOUT        10  /* secs between rexmt's */
#define TFTP_PORT           69  /* tftp default port    */
#define TFTP_TIMEOUT_RETRY  5
#define TFTP_DATA_PKT_HDR_SIZE  4       /* Size of tftp packet header
                                           in the tftp data packet */
#define TFTP_ACK_PKT_SIZE   4 /* Size of TFTP ACK, 2 bytes of opcode and
                                 2 bytes of block number */
#define TFTP_REQUEST_PKT_HDR_SIZE  4    /* 2 bytes - request type
                                           1 byte  - trailing zero for filename string
                                           1 byte  - trailing zero for transfer mode string */
#define TFTP_AUTO_INSTALL_TIMEOUT  4    /* initial timeout used by auto-install process*/
#define TFTP_AUTO_INSTALL_NUM_TRIES  6  /* number of tryes to download config file used by auto-install process*/

/* TFTP Op Codes/packet types */

#define TFTP_RRQ    01          /* read request     */
#define TFTP_WRQ    02          /* write request    */
#define TFTP_DATA   03          /* data packet      */
#define TFTP_ACK    04          /* acknowledgement  */
#define TFTP_ERROR  05          /* error packet     */


/*
  TFTP message formats are:


Type        Op #     Format without header

             2 bytes   string   1 byte  string  1 byte
            ------------------------------------------
TFTP_RRQ/  | [01|02]| filename |  0   | mode   |   0 |
TFTP_WRQ    ------------------------------------------

             2 bytes  2 bytes   n bytes
            ---------------------------
TFTP_DATA  |   03   | block # | data  |
            ---------------------------

             2 bytes   2 bytes
            ------------------
TFTP_ACK   |   04   | block #|
            ------------------

             2 bytes  2 bytes     string  1 byte
            -------------------------------------
TFTP_ERROR |  05    | ErrorCode | ErrMsg |   0  |
            -------------------------------------
*/


/* TFTP packet structure.  */

typedef struct
{
  L7_ushort16 blockOrError;
  L7_char8 data[TFTP_SEGSIZE];
} L7_TFTP_FORMAT_t;

typedef struct
{
  L7_ushort16 th_opcode;        /* packet op code   */
  union
  {
    L7_char8 request[TFTP_SEGSIZE + 2]; /* request string */
    L7_TFTP_FORMAT_t misc;
  } th;
} L7_TFTP_PACKET_t;

typedef L7_int32 (*L7_TFTP_CALLBACK_t)(void *arg, L7_uchar8 *buf, L7_ushort16 len);

#define TFTP_DEFAULT_IP_TTL_NUMBER   64    /* default IP TTL number for TFTP requests */

typedef struct
{
  FILE*               fp;
  L7_int32            fd; 
  L7_int32            clientSocket;
  L7_uint32           lastReceivedBlock;
  L7_uchar8           family;  /* L7_AF_INET, L7_AF_INET6, ... */
  L7_sockaddr_t*      saddr;
  L7_TFTP_CALLBACK_t  callback;
  void*               cbarg;

} tftpTransferParams_t;

/* useful defines to access the TFTP packet */

#define th_request  th.request  /* request string   */
#define th_block    th.misc.blockOrError        /* block number     */
#define th_error    th.misc.blockOrError        /* error code       */
#define th_errMsg   th.misc.data        /* error message    */
#define th_data     th.misc.data        /* data             */

/**************************************************************************
* @purpose  Create TFTP transfer access semaphore
*
* @comments The semaphore allows to run TFTP transfers synchronously
*
* @end
*************************************************************************/
void tftpTransferAccessSemCreate(void);

/**************************************************************************
* @purpose  Delete TFTP transfer access semaphore
*
* @comments The semaphore allows to run TFTP transfers synchronously
*
* @end
*************************************************************************/
void tftpTransferAccessSemDelete(void);

/*********************************************************************
* @purpose  To Get or put an ascii or binary file from/to the TFTP server
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    pFilename @b{(input)}  name of the file
* @param    pCommand  @b{(input)}  command which is to be executed
* @param    pMode     @b{(input)}  mode of file which is to be transferred
* @param    fd        @b{(input)}  file descriptor
* @returns  OK or ERROR
* @notes    none
*
* @end
*********************************************************************/
L7_TRANSFER_STATUS_t L7_TFTP_COPY(L7_inet_addr_t *inet_address, L7_char8 *pFilename,
                                  L7_char8 *pCommand, L7_char8 *pMode, L7_int32 fd,
                                  L7_uint8 requestDelay, L7_TFTP_CALLBACK_t callback, void *arg, L7_BOOL val);

/*********************************************************************
* @purpose  To get an ascii or binary file from TFTP server to FASTPATH switch
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    filename  @b{(input)}  name of the file
* @param    fileMode  @b{(input)}  mode of file which is to be transferred
* @param    fd        @b{(input)}  file descriptor
* @returns  OK or Error
* @notes    none
*
* @end
*********************************************************************/
L7_TRANSFER_STATUS_t L7_TFTP_GET (L7_inet_addr_t * inet_address, L7_char8 * fileName,
                                  L7_char8 * fileMode, L7_int32 fd, L7_uint8 requestDelay,
                                  L7_TFTP_CALLBACK_t callback, void *cbarg, L7_BOOL val);

/*********************************************************************
* @purpose  To put an ascii or binary file from FASTPATH switch to TFTP server
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    filename  @b{(input)}  name of the file
* @param    fileMode  @b{(input)}  mode of file which is to be transferred
* @param    fileDesc  @b{(input)}  file descriptor
* @returns  OK or Error
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t L7_TFTP_PUT (L7_inet_addr_t *inet_address, L7_char8 *filename,
                     L7_char8 *fileMode, L7_int32 fileDesc,
                     L7_TFTP_CALLBACK_t callback, void *arg);

#endif /* L7_TFTP_H */
