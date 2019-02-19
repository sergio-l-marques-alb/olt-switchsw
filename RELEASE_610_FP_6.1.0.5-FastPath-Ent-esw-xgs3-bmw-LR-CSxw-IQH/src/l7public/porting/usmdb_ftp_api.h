/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename  usmdb_ftp_api.h
*
* @purpose   
*
* @component base
*
* @comments
*
* @create    26/12/2006
*
* @author    SivaRamaKrishna.Mannem
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef _USMDB_FTP_API_H_
#define _USMDB_FTP_API_H_
/*********************************************************************
* @purpose  Does an ftp download from a ftp server
*
* @param    hostName   @b{(input)}ipaddress of the ftpserver
* @param    fileName   @b{(input)}name of the file on the server.
* @param    dirName    @b{(input)}directory name on which the file
*                                 is to be put on server
* @param    usr        @b{(input)}user name to be used for ftp server
* @param    passwd     @b{(input)}password to be used for ftp server
* @param    dest_file  @b{(input)} name of destination file.
* @returns  L7_TRANSFER_SUCCESS
* @returns  L7_TFTP_FAILED
* @returns  L7_FLASH_FAILED
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmdbFtpDownload(L7_char8  *hostName, L7_char8  *file, L7_char8 *dir_name, L7_char8  *usr, L7_char8  *passwd, L7_char8  *dest_file);
#endif

