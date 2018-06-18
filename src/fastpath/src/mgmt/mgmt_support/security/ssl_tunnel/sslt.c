/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt.c
*
* @purpose SSL Tunnel base functionality
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#include "sslt_include.h"
#include "openssl_api.h"
#include "osapi.h"
#include "dtlapi.h"

extern ssltGlobal_t ssltGlobal;

/*********************************************************************
*
* @purpose Handle the exchange of data between the secure and unsecure
*          connections
*
* @param ssl @b{(input)} pointer to the SSL connection object
* @param sock_fd @b{(input)} file descriptor for the unsecure connection
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 ssltConnectionLoop(SSL *ssl,
                             L7_int32 sock_fd,
                             ssltSecureTypes_t secureType)
{
  L7_uchar8 *buf;
  L7_int32 ssl_rfd, max_fd, result;
  fd_set read_fds;
  L7_int32 nread = L7_NULL;
  L7_BOOL terminateTunnel = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 operMode;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltConnectionLoop, ssl = %x, sock_fd = %d\n", (L7_uint32)ssl, sock_fd);

  buf = (L7_uchar8 *)osapiMalloc(L7_FLEX_SSLT_COMPONENT_ID, SSLT_BUF_SIZE);

  operMode = (SSLT_SECURE_ADMIN==secureType)?ssltGlobal.ssltOperMode:ssltGlobal.ssltAuxOperMode;
  while (operMode == L7_ENABLE)
  {
    struct timeval timeout;
    timeout.tv_sec  = 30;
    timeout.tv_usec = L7_NULL;

    ssl_rfd = SSL_get_rfd(ssl);
    if (ssl_rfd < 0)
    {
      break;
    }

    FD_ZERO(&read_fds);
    FD_SET(ssl_rfd, &read_fds);
    FD_SET(sock_fd, &read_fds);

    max_fd = (((sock_fd) > (ssl_rfd)) ? (sock_fd) : (ssl_rfd));

    result = select(max_fd + 1, &read_fds, L7_NULL, L7_NULL, &timeout);

    if (result <= L7_NULL)
    {
      /* select has timed out or an error has occured */
      terminateTunnel = L7_TRUE;

      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
        fprintf(stdout,"Select timeout, terminate tunnel!\n");
    }

    if (ssl_rfd && FD_ISSET(ssl_rfd, &read_fds))
    {
      bzero(buf, SSLT_BUF_SIZE);

      /* Remove any previous errors */
      ERR_clear_error();

      nread = SSL_read(ssl, buf, SSLT_BUF_SIZE);

      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        fprintf(stdout,"SSL Secure read - nread = %d\n", nread);

      switch(SSL_get_error(ssl, nread))
        {

      case SSL_ERROR_NONE:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_NONE\n");

          /* No errors and there is data to send to on the unsecure connection */
          ssltServerUnSecureDataSend(sock_fd, buf, nread);

          break;

      case SSL_ERROR_ZERO_RETURN:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_ZERO_RETURN\n");

          /* end of data - SSL connection has closed */
          terminateTunnel = L7_TRUE;

          break;

      case SSL_ERROR_WANT_READ:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_WANT_READ\n");

          /* Waiting for the incomming SSL record to complete */
          break;

      case SSL_ERROR_WANT_WRITE:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_WANT_WRITE\n");

          /* Retry the read after a write */

          break;

      case SSL_ERROR_SYSCALL:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_SYSCALL\n");

          if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
          {
            /* Just continue */
          }
          else
          {
            /* an error occured, drop the connection */
            terminateTunnel = L7_TRUE;
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_SYSCALL - terminate\n");
          }

          break;
      case SSL_ERROR_SSL:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"SSL_ERROR_SSL\n");

      default:
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
            fprintf(stdout,"default\n");

          /* an error occured, drop the connection */
          terminateTunnel = L7_TRUE;

          break;
        }
    }

    if (sock_fd && FD_ISSET(sock_fd, &read_fds))
    {
      /* Data is ready for reading from the unsecure connection */
      bzero(buf, SSLT_BUF_SIZE);
      nread = read(sock_fd, buf, SSLT_BUF_SIZE);

      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        fprintf(stdout,"Unsecure read - nread = %d\n", nread);

      if (nread <= L7_NULL)
      {
        /* no more data or error on unsecure read */
        terminateTunnel = L7_TRUE;
      }
      else if (nread > L7_NULL)
      {
        /* Don't send secure data if the connection is terminating */
        if (terminateTunnel != L7_TRUE)
        {
          if ((rc = ssltServerSecureDataSend(ssl, buf, nread)) != L7_SUCCESS)
          {
            /* An error occured on the SSL write */
            terminateTunnel = L7_TRUE;
          }
        }
      }
    }

    if (terminateTunnel == L7_TRUE)
    {
      /* Revert to a blocking SSL connection for easier shutdown */
      opensslSecureBlockingSet(ssl);

      SSL_shutdown(ssl);

      break;
    }
    operMode = (SSLT_SECURE_ADMIN==secureType)?ssltGlobal.ssltOperMode:ssltGlobal.ssltAuxOperMode;
  } /* while */

  osapiFree(L7_FLEX_SSLT_COMPONENT_ID, buf);

  return (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) ? 1 : 0;
}

/*********************************************************************
*
* @purpose Send data over the unsecure TCP connection
*
* @param sock_fd @b{(input)} file descriptor of unsecure connection
* @param buf @b{(input)} pointer to data
* @param size @b{(input)} amount of data to send
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltServerUnSecureDataSend(L7_uint32 sock_fd,
                                L7_uchar8 *buf,
                                L7_uint32 size)
{
  L7_uint32 err, nwritten;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltServerUnSecureDataSend - size = %d\n", size);

  for (nwritten = L7_NULL;  nwritten < size;  nwritten += err)
  {
    err = write(sock_fd, buf + nwritten, size - nwritten);

    if (err <= L7_NULL)
    {
      if (err < L7_NULL)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: Error on unsecure send\n");
      }
      break;
    }
  }

  return;
}

/*********************************************************************
*
* @purpose Send data over the secure SSL connection.
*
* @param ssl @b{(input)} pointer to secure connection object
* @param buf @b{(input)} pointer to data
* @param size @b{(input)} amount of data to send
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerSecureDataSend(SSL *ssl,
                                 L7_uchar8 *buf,
                                 L7_uint32 size)
{
  L7_uint32 nwritten;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltServerSecureDataSend - size = %d\n", size);

  while(size > L7_NULL)
    {
      /* Remove any previous errors */
      ERR_clear_error();

      nwritten = SSL_write(ssl, buf, size);

      switch(SSL_get_error(ssl, nwritten))
        {
          case SSL_ERROR_NONE:
            /* Some or all data was sent */
            size -= nwritten;

            break;

          case SSL_ERROR_ZERO_RETURN:
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_ZERO_RETURN\n");

            break;

          case SSL_ERROR_WANT_READ:
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_WANT_READ\n");

            /* Allow a read to occur first */
            return L7_SUCCESS;

            break;

          case SSL_ERROR_WANT_WRITE:
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_WANT_WRITE, size=%x\n",size);

            /* Try the write again */
            break;

          case SSL_ERROR_SSL:
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_SSL\n");

            /* Error, the SSL connection should be terminated */
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                    "SSLT: SSL_ERROR_SSL on SSL_write\n");

            return L7_FAILURE;

            break;

          case SSL_ERROR_SYSCALL:
            if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
              fprintf(stdout,"SSL_ERROR_SYSCALL, size=%x\n", size);

            /* Error, the SSL connection should be terminated */
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                    "SSLT: SSL_ERROR_SYSCALL on SSL_write\n");

            return L7_FAILURE;

            break;

          default:
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                    "SSLT: Unknown error on SSL_write\n");

            /* Error, the SSL connection should be terminated */
            return L7_FAILURE;

            break;
          } /* case */
    } /* while */

  return L7_SUCCESS;
}

