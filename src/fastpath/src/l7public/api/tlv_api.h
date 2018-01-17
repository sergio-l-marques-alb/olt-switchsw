/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   tlv_api.h
*
* @purpose    API function prototypes and data structures for the TLV utility
*
* @component  tlv
*
* @comments   none
*
* @create     07/05/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/

#ifndef _TLV_API_H_
#define _TLV_API_H_

#include "l7_common.h"


/*-----------------------------------------------------------------------------
   TLV Utility Usage Notes:
  
   The API functions described in this file are intended for use in building
   a TLV in a standard format that can have any type of content.  This utility
   is not intended for parsing (reading) an existing TLV, since that requires
   global awareness of each TLV definition and content.

   In the following function descriptions, the term 'TLV block' refers to the
   overall TLV entry, designated by a specific type value.  This TLV entry can
   itself be comprised of zero or more subordinate TLV entries, each of which
   can contain zero or more subordinate TLV entries, and so forth.  The TLV
   utility implementation imposes limits on the maximum number of TLV blocks 
   that can exist at one time, the maximum allowed TLV block size, the maximum
   TLV nesting depth, etc.

   Prior to working with a TLV block, the application must register with the 
   TLV utility for each independent TLV block that the application wishes to
   use concurrently.  The application must retain the TLV handle value that is
   output for a successful registration, and use this handle in all other TLV
   utility API function calls.  The application unregisters its TLV block usage
   when it shuts down.  While this registration allows for multiple TLV blocks
   to be separately managed, most applications need only one.  A sample
   registration sequence is shown here:

        tlvRegister
        .
        . (TLV block available for use during life of application)
        .
        tlvUnregister

   When working with a TLV block, an application typically invokes the utility
   APIs in the following relative order:

        tlvCreate
        tlvWrite X
        tlvComplete
        tlvQuery
        .
        . (use TLV contents)
        .
        tlvDelete

   A more complex TLV structure uses tlvOpen/tlvClose function pairs, perhaps
   including tlvWrite functions within an open TLV.  To illustrate, using 
   indentation for readability, where A, B, C, etc. represent different TLV
   'type' values:

        tlvCreate
        tlvOpen A
          tlvWrite B
          tlvWrite C
          tlvOpen D
          tlvClose D
          tlvOpen E
            tlvOpen D
            tlvWrite G
            tlvClose D
          tlvClose E
        tlvClose A
        tlvComplete
        tlvQuery
        .
        . (use TLV contents)
        .
        tlvDelete

 *-----------------------------------------------------------------------------
 */
 
/* TLV handle type */
typedef L7_uint32   L7_tlvHandle_t;

/* generic TLV structure definition
 * NOTE: all contents must be specified in network byte order (big endian) */
typedef struct
{
  L7_uint32     type;                           /* TLV type identifier        */
  L7_uint32     length;                         /* TLV length of value field  */
  L7_uchar8     valueStart[1];                  /* start of TLV value field   */
} L7_tlv_t;

/* size of header portion (type, length fields) of TLV structure definition */
#define L7_TLV_HEADER_SIZE    (sizeof(L7_uint32) * 2)

/* max name string length for application registration */
#define L7_TLV_APP_NAME_MAX   15

/* user parser function typedef */
/*********************************************************************
* @purpose  User function to interpret the type-specific contents of a TLV
*
* @param    pTlv        @{(input)}  Pointer to a single TLV entry
* @param    nestLvl     @{(input)}  Nesting level of this TLV (0=top level)
* @param    pEntrySize  @{(output)} Pointer to TLV entry size output location
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The nestLvl parameter indicates the nesting depth of this TLV
*           relative to the top-level (i.e., outermost) TLV.  A top-level
*           TLV is designated by a nestLvl of 0.
*
* @notes    The user function must provide the TOTAL size of the TLV, which
*           includes the size of the type and length fields, based on its
*           internal knowledge of the TLV type-based definition.
*       
* @end
*********************************************************************/
typedef L7_RC_t (*L7_tlvUserParseFunc_t)(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                                         L7_uint32 *pEntrySize);



/*********************************************************************
* @purpose  Allocates resources for construction of a new TLV block
*
* @param    tlvSizeMax  @{(input)}  Maximum size of TLV block needed
* @param    appId       @{(input)}  Application identifier
* @param    appName     @{(input)}  Application name string
* @param    pTlvHandle  @{(output)} Pointer to TLV handle output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not enough resources for a new TLV
* @returns  L7_FAILURE  
*
* @notes    The tlvSizeMax parm is used as an upper limit of the TLV
*           block size required by the application.  It must be specified
*           as a value from 4 to 65536.
*
* @notes    The appId parm is primarily used for debugging.  It is 
*           recommended that a component ID (or something similar)
*           be used as a means of identifying which application owns
*           a particular TLV block resource.  A value of zero is permitted.
*       
* @end
*********************************************************************/
L7_RC_t tlvRegister(L7_uint32 tlvSizeMax, L7_uint32 appId, L7_uchar8 *pAppName,
                    L7_tlvHandle_t *pTlvHandle);

/*********************************************************************
* @purpose  Destroys the specified TLV block and deallocates any dynamic
*           memory used during ts construction
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Upon successful completion, the tlvHandle value no longer
*           represents this TLV block.
*
* @notes    This function can be issued at any time regardless of the 
*           state of the TLV block.
*       
* @end
*********************************************************************/
L7_RC_t tlvUnregister(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Activates specified TLV block for construction
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block currently in use
* @returns  L7_FAILURE  
*
* @notes    Once this command completes successfully, a tlvDelete() is needed
*           before this TLV block can be reused for a subsequent TLV creation.
*           The L7_ERROR return indicates the TLV block is currently in use.
*
* @end
*********************************************************************/
L7_RC_t tlvCreate(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Indicates construction of entire specified TLV block is complete
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block still open
* @returns  L7_FAILURE  
*
* @notes    Once this command completes successfully, the specified TLV block
*           contents can no longer be updated.  Use the tlvQuery() function to
*           obtain starting location and length of this completed TLV block.
*
* @notes    The L7_ERROR return is typically due to one or more tlvOpen() 
*           functions not having an associated tlvClose() issued prior to
*           the tlvComplete().
*
* @notes    Regardless of the return value from this function, tlvDelete()
*           must still be called to dispose of any TLV that has been created. 
*       
* @end
*********************************************************************/
L7_RC_t tlvComplete(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Deactivates specified TLV block, preventing further use
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    TLV block was not created
* @returns  L7_FAILURE  
*
* @notes    Upon successful completion, the TLV block is no longer 
*           considered to exist and should not be accessed with the
*           TLV pointer previously provided by tlvQuery().
*
* @notes    This function can be called any time following a tlvCreate()
*           and does not require a tlvComplete() to be issued first.  All
*           outstanding tlvOpen() operations are considered null-and-void.
*
* @notes    The actual contents of the TLV block control and data areas
*           are left intact until a subsequent tlvCreate() is issued to
*           help facilitate debugging.  This content is still considered
*           invalid from an operational perspective, however.
*
* @end
*********************************************************************/
L7_RC_t tlvDelete(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Destroys the specified TLV block and deallocates any dynamic
*           memory used during ts construction
*
* @param    tlvHandle   @{(input)}  TLV block handle
* @param    ppTlv       @{(output)} Pointer to TLV buffer pointer output location
* @param    pTlvLen     @{(output)} Pointer to TLV length output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Specified TLV block has not been completed
* @returns  L7_FAILURE  
*
* @notes    This function is only valid following a successful tlvComplete().
*
* @notes    The output TLV length value represents the total number of bytes
*           occupied by the TLV block in the output buffer, including the
*           type and length fields of the first TLV within it.
*       
* @end
*********************************************************************/
L7_RC_t tlvQuery(L7_tlvHandle_t tlvHandle, L7_tlv_t **ppTlv, 
                 L7_uint32 *pTlvLen);

/*********************************************************************
* @purpose  Appends a fixed-sized TLV entry to the specified TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
* @param    length      @{(input)} Number of bytes in pValue buffer
* @param    pValue      @{(input)} Pointer to TLV entry data character buffer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    All data passed through pValue is treated as a character array
*           and must be in big-endian format (per the TLV definition).
*
* @notes    If no additional data is defined for the TLV entry, the length
*           value must be specified as 0 (pValue is ignored in this case).
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there
*           is no more room in the TLV block for this TLV entry.
*
* @end
*********************************************************************/
L7_RC_t tlvWrite(L7_tlvHandle_t tlvHandle, L7_uint32 type, L7_uint32 length,
                 L7_uchar8 *pValue);

/*********************************************************************
* @purpose  Starts appending a variable-sized TLV entry to the specified 
*           TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
* @param    length      @{(input)} Number of bytes in pValue buffer
* @param    pValue      @{(input)} Pointer to TLV entry data character buffer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    All data passed through pValue is treated as a character array
*           and must be in big-endian format (per the TLV definition).
*
* @notes    A variable-sized TLV entry may contain some natively-defined
*           data (per length and pValue parms) and often contains additional
*           fixed- and/or variable-sized TLVs within its value field.
*
* @notes    If no native data is defined for the TLV entry, the length
*           value must be specified as 0 (pValue is ignored in this case);
*           this has no bearing on whether any nested TLVs are defined 
*           within this TLV entry.
*
* @notes    Each tlvOpen() must be paired with a corresponding tlvClose().
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there are
*           too many TLVs open within the TLV block simultaneously.
*       
* @end
*********************************************************************/
L7_RC_t tlvOpen(L7_tlvHandle_t tlvHandle, L7_uint32 type, L7_uint32 length,
                L7_uchar8 *pValue);

/*********************************************************************
* @purpose  Finishes the definition of a currently-open TLV entry within
*           the specified TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    type        @{(input)} TLV type identifier code
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Operation not permitted
* @returns  L7_FAILURE  
*
* @notes    Each tlvClose() must be paired with its corresponding tlvOpen().
*           Once a TLV entry is successfully closed, its contents can no 
*           longer be modified (does not terminate modification to the overall
*           TLV block, however -- see tlvComplete() function)
*
* @notes    The type parameter must be the same as was used for the
*           corresponding tlvOpen(), but is used more for error checking since
*           it is not always guaranteed to uniquely identify which of the 
*           currently open TLV entries is being closed.
*
* @notes    The L7_ERROR return is typically due to the tlvComplete() having
*           already been issued for the specified TLV block, or if there are
*           no TLVs open within the TLV block.
*       
* @end
*********************************************************************/
L7_RC_t tlvClose(L7_tlvHandle_t tlvHandle, L7_uint32 type);

/*********************************************************************
* @purpose  Checks if the specified TLV block handle is valid
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_TRUE     TLV handle is registered
* @returns  L7_FALSE    TLV handle not registered
*
* @notes    This function indicates that a TLV block corresponding
*           to the specified handle is registered.  It does not imply
*           anything about the current state or contents of the TLV block.
*
* @end
*********************************************************************/
L7_BOOL tlvHandleIsValid(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Checks if the specified TLV handle represents an active
*           TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_TRUE     TLV handle is valid
* @returns  L7_FALSE    TLV handle not valid
*
* @notes    This function indicates that a TLV block corresponding
*           to the specified handle is both registered and created.  It 
*           does not imply anything about the completion state or contents
*           of the TLV block.
*
* @end
*********************************************************************/
L7_BOOL tlvBlockIsActive(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Parse the TLV contents
*
* @param    pTlv          @{(input)} Pointer to TLV start location
* @param    userParseFunc @{(input)} User parser function
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Error reported by user parser function
* @returns  L7_FAILURE
*
* @notes    Does not interpret the TLV contents, but rather navigates the
*           TLV, including any nested TLVs.  A user function is called
*           to do any TLV type-specific interpretation and processing (see
*           typedef for function prototype).
*
* @end
*********************************************************************/
L7_RC_t tlvParse(L7_tlv_t *pTlv, L7_tlvUserParseFunc_t userParseFunc);

/*********************************************************************
* @purpose  Start tracing for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    Trace is automatically started when each TLV block is registered.
*
* @end
*********************************************************************/
L7_RC_t tlvTraceStart(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Stop tracing for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t tlvTraceStop(L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Display the trace data for this TLV block
*
* @param    tlvHandle   @{(input)} TLV block handle
* @param    format      @{(input)} Format trace output (L7_TRUE, L7_FALSE)
* @param    count       @{(input)} Number of entries to display
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A count value of 0 is used to display all entries.
*
* @end
*********************************************************************/
L7_RC_t tlvTraceShow(L7_tlvHandle_t tlvHandle, L7_BOOL format, L7_uint32 count);

#endif /* _TLV_API_H_ */
