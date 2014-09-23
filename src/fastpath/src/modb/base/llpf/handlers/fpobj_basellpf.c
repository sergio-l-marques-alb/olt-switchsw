
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
********************************************************************************
*
* @filename fpobj_basellpf.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to llpf-object.xml
*
* @create  25 September 2009, Friday
*
* @notes   Management Object Handler file for Link Local Protocol Filtering 
*
* @author  Vijayanand, K
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "ctype.h"
#include "usmdb_util_api.h"
#include "_xe_basellpf_obj.h"
#include "usmdb_llpf_api.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

/*********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to capital case
*
* @param L7_char8 *buf
*
* @returns  L7_SUCCESS  It was able to convert all chars to capital case
* @returns  L7_FAILURE  should never return failure
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          capital case, checking the buffer for empty string
* @end
*
*********************************************************************/
static L7_RC_t fpObjUtilConvertToUpperCase(L7_char8 * buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  for ( i = 0; i < strlen(buf); i++ )
  {
    if ( buf[i] != '\n' || buf[i] != '\0' )
    {
      c_tmp = (L7_char8)  toupper(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return L7_SUCCESS;
}
/******************************************************************************
*
* @purpose  Converts LLPF Protocol enum values to string.
* @param protocol       @b{(input)} The current protocol value.
* @param strprotocol    @b{(input)} Pointer to strProtocol
* @param strProtoSize   @b{(input)} string length of protocol size buffer
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  show interface llpf all|([unit/]slot/port <cr>)
*
* @cmdhelp Displays LLPF mode per interface.
*
* @end
*
******************************************************************************/
static L7_RC_t fpObjUtilLlpfUtilGetProtoType(L7_uint32 protocol,L7_char8 *strProtocol ,L7_uint32 strProtoSize)
{
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 lseek;
  
  lseek = strlen(pStrInfo_base_LlpfBlock);

  if(strProtocol != L7_NULL)
  {
   rc = L7_SUCCESS;
   switch(protocol)
   {
     case L7_LLPF_BLOCK_TYPE_ISDP:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfIsdp + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
       break;

     case L7_LLPF_BLOCK_TYPE_VTP:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfVtp + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
       break;

     case L7_LLPF_BLOCK_TYPE_DTP:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfDtp + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
       break;

     case L7_LLPF_BLOCK_TYPE_UDLD:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfUdld + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
       break;

     case L7_LLPF_BLOCK_TYPE_PAGP:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfPagp + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
       break;

     case L7_LLPF_BLOCK_TYPE_SSTP:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfSstp + lseek);
       fpObjUtilConvertToUpperCase(strProtocol);
      break;

     case L7_LLPF_BLOCK_TYPE_ALL:
       osapiSnprintf(strProtocol,strProtoSize,pStrInfo_base_LlpfAll);
       /* Just only the letter 'b' need to be turned up as upper case here */
       strProtocol[0] = toupper(pStrInfo_base_LlpfAll[0]);
       break;

     default:
       rc = L7_FAILURE;
       break;
   }
  }
  else
  {
    rc = L7_FAILURE;
  }
  return rc;
}
/*******************************************************************************
* @function fpObjGet_basellpf_ifIndex
*
* @purpose Get 'ifIndex'
* @description  [ifIndex] valid LLPF Interface Number   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basellpf_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objifIndexValue;

  xLibU32_t nextObjifIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof(objifIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_basellpf_ifIndex,
                (xLibU8_t *) &objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
     owa.l7rc = usmDbIntIfNumTypeFirstGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,
                                       &nextObjifIndexValue);
  }
  else
  {
      FPOBJ_TRACE_CURRENT_KEY(bufp, &objifIndexValue, owa.len);
      owa.l7rc = usmDbIntIfNumTypeNextGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF | USM_LAG_INTF,0,objifIndexValue,
                                       &nextObjifIndexValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjifIndexValue,
                      sizeof(nextObjifIndexValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basellpf_protocolType
*
* @purpose Get 'protocolType'
* @description  [protocolType]  The type of LLPF Protocol that needs to be
* configured for blocking or un blocking.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basellpf_protocolType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objprotocolTypeValue;

  xLibU32_t nextObjProtocolTypeValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = L7_FAILURE;

  /* retrieve key: ProtocolIndex*/
  owa.len = sizeof(objprotocolTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_basellpf_protocolType,
                (xLibU8_t *) &objprotocolTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
     nextObjProtocolTypeValue = L7_LLPF_BLOCK_TYPE_ISDP;
     owa.l7rc = L7_SUCCESS;
  }
  else
  {
     if(objprotocolTypeValue < L7_LLPF_BLOCK_TYPE_ALL)
     {
        nextObjProtocolTypeValue = (objprotocolTypeValue +1);
        owa.l7rc = L7_SUCCESS;
     }

  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjProtocolTypeValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjProtocolTypeValue,
                      sizeof(nextObjProtocolTypeValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basellpf_BlockMode
*
* @purpose Get 'BlockMode'
* @description  [BlockMode] LLPF Protocol Filter Blocking Mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basellpf_BlockMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objBlockModeValue;

  xLibU32_t keyifIndexValue;
  xLibU32_t keyprotocolTypeValue;


  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ifIndex */
  owa.len = sizeof(keyifIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_basellpf_ifIndex,
                              (xLibU8_t *) &keyifIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyifIndexValue, 
            owa.len);

  /* retrieve key: protocolType */
  owa.len = sizeof(keyprotocolTypeValue);
  owa.rc = xLibFilterGet(wap, XOBJ_basellpf_protocolType,
                              (xLibU8_t *) &keyprotocolTypeValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyprotocolTypeValue, 
          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbLlpfIntfBlockModeGet(keyifIndexValue,
                keyprotocolTypeValue,&objBlockModeValue);
  if(owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
     FPOBJ_TRACE_EXIT(bufp, owa);
     return owa.rc;  
  }

   FPOBJ_TRACE_VALUE (bufp, &objBlockModeValue, sizeof(objBlockModeValue));

  /* return the object value: BlockMode */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objBlockModeValue,
                      sizeof(objBlockModeValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_basellpf_BlockMode
*
* @purpose Set 'BlockMode'
* @description  [BlockMode] LLPF Protocol Filter Blocking Mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basellpf_BlockMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objBlockModeValue;

  xLibU32_t keyifIndexValue;
  xLibU32_t keyprotocolTypeValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: BlockMode */
  owa.len = sizeof(objBlockModeValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objBlockModeValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBlockModeValue, owa.len);


    /* retrieve key: ifIndex */
    owa.len = sizeof(keyifIndexValue);
    owa.rc = xLibFilterGet(wap, XOBJ_basellpf_ifIndex,
                                (xLibU8_t *) &keyifIndexValue, 
                                &owa.len);
    if(owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING; 
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY(bufp, &keyifIndexValue, 
            owa.len);

    /* retrieve key: protocolType */
    owa.len = sizeof(keyprotocolTypeValue);
    owa.rc = xLibFilterGet(wap, XOBJ_basellpf_protocolType,
                                (xLibU8_t *) &keyprotocolTypeValue, 
                                &owa.len);
    if(owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING; 
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY(bufp, &keyprotocolTypeValue, 
            owa.len);

    /* set the value in application */
    owa.l7rc = usmDbLlpfIntfBlockModeSet(keyifIndexValue,
                 keyprotocolTypeValue, objBlockModeValue);

    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
    }
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_basellpf_BlockModeSummary
*
* @purpose Get 'BlockMode'
* @description  [BlockMode] LLPF Protocol Filter Blocking Mode   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basellpf_BlockModeSummary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t concatinateString,objBlockModeValueSummary;

  xLibU32_t keyifIndexValue,objBlockModeValue,keyprotocolTypeValue = 0;
  xLibU32_t blkModeStrLen;

  FPOBJ_TRACE_ENTER(bufp);

#define REM_STR_LEN(a) (sizeof(a)-strlen(a))
#define PROTOCOL_DELIMITER ", "

  /* retrieve key: ifIndex */
  owa.len = sizeof(keyifIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_basellpf_ifIndex,
                              (xLibU8_t *) &keyifIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyifIndexValue, 
            owa.len);
  memset(objBlockModeValueSummary,0x00,sizeof(objBlockModeValueSummary));

  if (owa.rc == XLIBRC_SUCCESS)
  {
    while((owa.l7rc == L7_SUCCESS) && 
          (keyprotocolTypeValue < L7_LLPF_BLOCK_TYPE_LAST))
    {
      /* get the value from application */
      owa.l7rc = usmDbLlpfIntfBlockModeGet(keyifIndexValue,
                  keyprotocolTypeValue,&objBlockModeValue);
   
      if((owa.l7rc == L7_SUCCESS) && (objBlockModeValue == L7_ENABLE))
      {
        memset(concatinateString,0x00,sizeof(concatinateString));
        fpObjUtilLlpfUtilGetProtoType(keyprotocolTypeValue,concatinateString,sizeof(concatinateString));
        osapiStrncat(objBlockModeValueSummary,concatinateString,REM_STR_LEN(objBlockModeValueSummary));
        osapiStrncat(objBlockModeValueSummary,PROTOCOL_DELIMITER, REM_STR_LEN(objBlockModeValueSummary));
      }
      keyprotocolTypeValue++;
    }
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* This is to remove the delimiter for last protocol field */
  blkModeStrLen = strlen(objBlockModeValueSummary);
  objBlockModeValueSummary[blkModeStrLen -strlen(PROTOCOL_DELIMITER)] = '\0';
 
  FPOBJ_TRACE_VALUE (bufp, &objBlockModeValueSummary, sizeof(objBlockModeValueSummary));

  /* return the object value: BlockMode */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objBlockModeValueSummary,
                      sizeof(objBlockModeValueSummary));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

