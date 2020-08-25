/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi_debug.h
*
* @purpose   This file contains prototypes and definitions particular to 
*            DAPI debug facilities.
*
* @component dapi
*
* @comments
*
* @create    4/10/2003
*
* @author    gator
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_DAPI_DEBUG_H
#define INCLUDE_DAPI_DEBUG_H

#include "dapi.h"
#include "dapi_struct.h"
#include "log.h"
#include "sysapi.h"

#define DAPI_DEBUG_MSG_SIZE           254
#define DAPI_DEBUG_WARN_MSG_SIZE      (DAPI_DEBUG_MSG_SIZE)
#define DAPI_DEBUG_INFO_MSG_SIZE      (DAPI_DEBUG_MSG_SIZE)

#define DAPI_DEBUG_DECODE             5
#define DAPI_DEBUG_INDICATIONS        4
#define DAPI_DEBUG_INFORMATIONAL      3
#define DAPI_DEBUG_WARNINGS           2
#define DAPI_DEBUG_ERRORS             1
#define DAPI_DEBUG_NONE               0

/* 
 * This sequence of defines sets the DAPI debug options based on passed in compile time
 * options. Ideally these options are articulated and manipulted in dapi_debug.cfg. If
 * not, default values equating to DAPI debug disabled are assigned.
 */
#ifndef DAPI_DEBUG_MSGLVL_INIT
#ifdef  DAPI_DEBUG_MSGLVL
#ifdef  DAPI_DEBUG_MSG_ON
#define DAPI_DEBUG_MSGLVL_INIT DAPI_DEBUG_MSGLVL
#endif
#endif /* DAPI_DEBUG_MSGLVL */
#endif /* DAPI_DEBUG_MSGLVL_INIT */

#ifndef DAPI_DEBUG_MSGLVL_INIT
#define DAPI_DEBUG_MSGLVL_INIT DAPI_DEBUG_NONE
#endif

/* 
 * The same holds true for the individual enables. Unless explicitely defined, the default
 * is DAPI debug disabled.
 */
#ifdef DAPI_DEBUG_MSGLVL
#ifndef DAPI_DEBUG_MSGLVL_UNSPECIFIED 
#define DAPI_DEBUG_MSGLVL_UNSPECIFIED DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_CONTROL 
#define DAPI_DEBUG_MSGLVL_CONTROL DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_SYSTEM 
#define DAPI_DEBUG_MSGLVL_SYSTEM DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_INTF 
#define DAPI_DEBUG_MSGLVL_INTF DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_LOGICAL_INTF 
#define DAPI_DEBUG_MSGLVL_LOGICAL_INTF DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_LAG
#define DAPI_DEBUG_MSGLVL_LAG DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_ADDR 
#define DAPI_DEBUG_MSGLVL_ADDR DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_QVLAN 
#define DAPI_DEBUG_MSGLVL_QVLAN DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_GARP 
#define DAPI_DEBUG_MSGLVL_GARP DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_ROUTING_ROUTE 
#define DAPI_DEBUG_MSGLVL_ROUTING_ROUTE DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_ROUTING_ARP
#define DAPI_DEBUG_MSGLVL_ROUTING_ARP DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_ROUTING_INTF
#define DAPI_DEBUG_MSGLVL_ROUTING_INTF DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_ROUTING_INTF 
#define DAPI_DEBUG_MSGLVL_ROUTING_INTF DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_FRAME 
#define DAPI_DEBUG_MSGLVL_FRAME DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_QOS_DIFFSERV 
#define DAPI_DEBUG_MSGLVL_QOS_DIFFSERV DAPI_DEBUG_NONE
#endif
#ifndef DAPI_DEBUG_MSGLVL_QOS_ACL 
#define DAPI_DEBUG_MSGLVL_QOS_ACL DAPI_DEBUG_NONE
#endif
#endif /* DAPI_DEBUG_MSGLVL */

/* 
 * Well known DAPI error codes. Space is left to OR in additional data
 */
#define DAPI_ERROR_GENERAL      0xF0000000UL
#define DAPI_ERROR_NO_ROUTER    0x60000000UL
#define DAPI_ERROR_NO_SUPPORT   0x50000000UL
#define DAPI_ERROR_NO_MEM       0x40000000UL
#define DAPI_ERROR_NO_USP       0x30000000UL
#define DAPI_ERROR_NO_CARD      0x20000000UL
#define DAPI_ERROR_NO_GET       0x10000000UL

/****************************************************************************************
*
* @purpose  Initialize DAPI debug ability including semaphores, queues and tasks.
*
* @param   *dapi_g    @b{(input)}  The driver object
* @param    cmdDecode @b{(input)}  Flag indicating whether to initialize the command 
*                                  decode message queue
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If DAPI debug is not enabled (DAPI_DEBUG_MSGLVL == DAPI_DEBUG_NONE), this function is not 
*           called.
*
* @end
*
****************************************************************************************/
L7_RC_t dapiDebugInit(DAPI_t *dapi_g, L7_BOOL     cmdDecode);

/****************************************************************************************
*
* @purpose  Intialize DAPI debug structures for a particular port.
*
* @param   *usp       @b{(input)}  The USP of the port that is to be set
* @param   *dapi_g    @b{(input)}  The driver object
*
* @returns  nothing
*
* @end
*
****************************************************************************************/
void dapiDebugPortInit(DAPI_USP_t *usp, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is enabled to the appropriate extent, this function will 
*           be called by DAPI_INFO_MSG and will display the warning information to the 
*           console. Formatting is from a string formed from a printf-like format string 
*           and argument list.
*
* @param   *format    @b{(input)}  The printf-like format string of the text destined 
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the 
*                                  format string. For each argument implied by the format 
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_INFO_MSG and is typically not called
*           by as a standalone function call. 
*
* @end
*
****************************************************************************************/
void dapiInfoMsg(const char *format, ...);

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is enabled to the appropriate extent, this function will 
*           be called by DAPI_WARN_MSG and will display the warning information to the 
*           console as opposed to recoding in the event log. Formatting includes the file,
*           line number and a string formed from a printf-like format string and argument 
*           list.
*
* @param   *fname     @b{(input)}  The filename recorded by the DAPI_WARN_MSG macro.
* @param    line      @b{(input)}  The line number recorded by the DAPI_WARN_MSG macro.
* @param   *format    @b{(input)}  The printf-like format string of the text destined 
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the 
*                                  format string. For each argument implied by the format 
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_WARN_MSG and is typically not called
*           by as a standalone function call. 
*
* @end
*
****************************************************************************************/
void dapiWarnMsg(const char *fname, L7_long32 line, const char *format, ...);

/****************************************************************************************
*
* @purpose  If DAPI/HAPI debug is not enabled to the sufficient extent, this function 
*           will be called by DAPI_WARN_MSG and will route the formatted warning 
*           information to the message log. Formatting includes the file, line number and
*           a string formed from a printf-like format string and argument list.
*
* @param   *fname     @b{(input)}  The filename recorded by the DAPI_WARN_MSG macro.
* @param    line      @b{(input)}  The line number recorded by the DAPI_WARN_MSG macro.
* @param   *format    @b{(input)}  The printf-like format string of the text destined 
*                                  for the message log.
* @param    arg       @b{(input)}  The printf-like argument list corresponding to the 
*                                  format string. For each argument implied by the format 
*                                  string, and argument must exist in the arg list.
*
* @returns  void
*
* @comments This function is used by the macro DAPI_WARN_MSG and is typically not called
*           by as a standalone function call. 
*
* @end
*
****************************************************************************************/
void dapiLogMsg(const char *fname, L7_ulong32 line, const char *format, ...);

/****************************************************************************************
*
* @purpose  Process each DAPI command and display decoded command info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command. For
*                               interface directed commands, this should be the port to 
*                               act on. For non-port specific commands, this should be 
*                               0.0.0.
* @param    cmd     @b{(input)} command to execute
* @param   *data    @b{(input)} data for the control function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCmdDisp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Process each DAPI command and display decoded command info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command. For
*                               interface directed commands, this should be the port to 
*                               act on. For non-port specific commands, this should be 
*                               0.0.0.
* @param    cmd     @b{(input)} command to execute
* @param   *data    @b{(input)} data for the control function
* @param    result  @b{(input)} Command return value
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCmdRspDisp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, L7_RC_t result, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Display decoded callback information.
*
* @param    cmd     @b{(input)} The callback command to execute
* @param   *cmdInfo @b{(input)} data for the callback function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugCommandDecode(DAPI_CMD_t cmd, void *cmdInfo, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Process each DAPI callback and display decoded callback info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the callback.
* @param    family  @b{(input)} The feature group
* @param    cmd     @b{(input)} The callback command to execute
* @param    event   @b{(input)} 
* @param   *cmdInfo @b{(input)} data for the callback function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCallbackDisp(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, void *data, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Process each DAPI callback and display decoded callback info if appropriate.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the callback.
* @param    family  @b{(input)} The feature group
* @param    cmd     @b{(input)} The callback command to execute
* @param    event   @b{(input)} 
* @param   *cmdInfo @b{(input)} data for the callback function
* @param    result  @b{(input)} Command return value
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugMsgCallbackRspDisp(DAPI_USP_t *usp, DAPI_FAMILY_t family, DAPI_CMD_t cmd, DAPI_EVENT_t event, 
                                 void *cmdInfo, L7_RC_t result, DAPI_t *dapi_g);

/****************************************************************************************
*
* @purpose  Display decoded callback information.
*
* @param    cmd     @b{(input)} The callback command to execute
* @param   *cmdInfo @b{(input)} data for the callback function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void

* @comments
*
* @end
*
****************************************************************************************/
void dapiDebugCallbackDecode(DAPI_CMD_t cmd, DAPI_EVENT_t event, void *cbInfo, DAPI_t *dapi_g);

/* 
 * Function-like macros. Depending on compile-time options support for these "functions"
 * may equate to informational messages, entries in the error log or message log, or no
 * code generated at all. If support compiled in, the current DAPI message level is tested
 * and if set to sufficient level, the information specified is displayed.
 */

/****************************************************************************************
*
* @purpose  Display informational message.
*
* @param   *dapi_g  @b{(input)} The driver object
* @param   *format  @b{(input)} The printf-like format string of the text destined 
*                               for the message log.
* @param    arg     @b{(input)} The printf-like argument list corresponding to the 
*                               format string. For each argument implied by the format 
*                               string, and argument must exist in the arg list.
*
* @returns  void

* @comments If DAPI debug messages are not compiled to sufficient level, no code is
*           generated for this macro.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INFORMATIONAL)
#define DAPI_INFO_MSG(dapi_g, format, args...)                          \
      if (((DAPI_t *)(dapi_g))->message_level >= DAPI_DEBUG_INFORMATIONAL) {  \
         dapiInfoMsg(format, ##args);                                   \
      }
#else
#define DAPI_INFO_MSG(dapi_g, format, args...)
#endif  

/****************************************************************************************
*
* @purpose  Display warning message.
*
* @param   *dapi_g  @b{(input)} The driver object
* @param   *format  @b{(input)} The printf-like format string of the text destined 
*                               for the message log.
* @param    arg     @b{(input)} The printf-like argument list corresponding to the 
*                               format string. For each argument implied by the format 
*                               string, and argument must exist in the arg list.
*
* @returns  void

* @comments If DAPI debug messages are not compiled to sufficient level, a message
*           log entry is generated for this macro.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_WARNINGS)
#define DAPI_WARN_MSG(dapi_g, format, args...)                     \
      if (((DAPI_t *)(dapi_g))->message_level >= DAPI_DEBUG_WARNINGS) {  \
        dapiWarnMsg(__FILE__, __LINE__, format, ##args);           \
      }
#else
#define DAPI_WARN_MSG(dapi_g, format, args...) dapiLogMsg(__FILE__, __LINE__, format, ##args)
#endif

/****************************************************************************************
*
* @purpose  Display error message.
*
* @param   *dapi_g  @b{(input)} The driver object
* @param    code    @b{(input)} A 32-bit error code. Any useful data can be recorded here.
*                               Convention is to use a well-known error code along with 
*                               user data.
*
* @returns  void

* @comments If DAPI debug messages are not compiled to sufficient level, a message
*           log error is generated for this macro and the box is reset.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_ERRORS)
#define DAPI_ERROR_MSG(dapi_g, code)                               \
      if (((DAPI_t *)(dapi_g))->message_level >= DAPI_DEBUG_ERRORS) {    \
        sysapiPrintf("FAILURE (%s: %d) Code: 0x%08LX\n", __FILE__, __LINE__, (L7_ulong32)(code)); \
      }
#else
#define DAPI_ERROR_MSG(dapi_g, code) L7_LOG_ERROR((L7_ulong32)(code))
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console indicating the DAPI command issued and the 
*           port it is issued to.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command.
* @param    cmd     @b{(input)} Command to execute
* @param   *data    @b{(input)} Associated data for the control function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_INDICATIONS (currently 4). Otherwise it expands to a function 
*           call that formats and displays the command info if the runtime messaging 
*           level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_COMMAND_MSG(usp, cmd, data, dapi_g) dapiDebugMsgCmdDisp((usp), (cmd), (data), (dapi_g))
#else
#define DAPI_DEBUG_COMMAND_MSG(usp, cmd, data, dapi_g)
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console indicating the DAPI command response and return
*           value
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command.
* @param    cmd     @b{(input)} Command executed
* @param   *data    @b{(input)} Response data for the control function
* @param    result  @b{(input)} Command return value
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_INDICATIONS (currently 4). Otherwise it expands to a function 
*           call that formats and displays the response info if the runtime messaging 
*           level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_COMMAND_RESPONSE_MSG(usp,   cmd,   data,   result,   dapi_g) \
                dapiDebugMsgCmdRspDisp((usp), (cmd), (data), (result), (dapi_g))
#else
#define DAPI_DEBUG_COMMAND_RESPONSE_MSG(usp,   cmd,   data,   result,   dapi_g)
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console decoding the parameters of a DAPI command 
*
* @param    cmd     @b{(input)} Command to execute
* @param   *cmdInfo @b{(input)} Associated data for the control function
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_DECODE (currently 5). Otherwise it expands to a function 
*           call that decodes, formats and displays the command parameters if the runtime 
*           messaging level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_DECODE)
#define DAPI_DEBUG_COMMAND_DECODE(cmd, cmdInfo, dapi_g) dapiDebugCommandDecode((cmd), (cmdInfo), (dapi_g))
#else
#define DAPI_DEBUG_COMMAND_DECODE(cmd, cmdInfo, dapi_g)
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console indicating the DAPI callback issued and the 
*           port it is issued from.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command.
* @param    family  @b{(input)} The callback family. Only one callback function can be 
*                               active per command famnily.
* @param    cmd     @b{(input)} Callback to execute
* @param    event   @b{(input)} The particular event that has occured
* @param   *data    @b{(input)} Associated data for the callback
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_INDICATIONS (currently 4). Otherwise it expands to a function 
*           call that formats and displays the callback info if the runtime messaging 
*           level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_CALLBACK_MSG(usp,   family,   cmd,   event,   data,   dapi_g) \
      dapiDebugMsgCallbackDisp((usp), (family), (cmd), (event), (data), (dapi_g))
#else
#define DAPI_DEBUG_CALLBACK_MSG(usp,   family,   cmd,   event,   data,   dapi_g)
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console indicating the DAPI callback issued and the 
*           port it is issued from.
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command.
* @param    family  @b{(input)} The callback family. Only one callback function can be 
*                               active per command famnily.
* @param    cmd     @b{(input)} Callback to execute
* @param    event   @b{(input)} The particular event that has occured
* @param   *data    @b{(input)} Associated data for the callback
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_INDICATIONS (currently 4). Otherwise it expands to a function 
*           call that formats and displays the callback info if the runtime messaging 
*           level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_CALLBACK_RESPONSE_MSG(usp,   family,   cmd,   event,   data,   result,   dapi_g) \
            dapiDebugMsgCallbackRspDisp((usp), (family), (cmd), (event), (data), (result), (dapi_g))
#else
#define DAPI_DEBUG_CALLBACK_RESPONSE_MSG(usp,   family,   cmd,   event,   data,   result,   dapi_g) 
#endif

/****************************************************************************************
*
* @purpose  Display a message to the console decoding the parameters of a DAPI callback
*
* @param    cmd     @b{(input)} Callback to execute
* @param   *cbInfo  @b{(input)} Associated data for the callback
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_DECODE (currently 5). Otherwise it expands to a function 
*           call that decodes, formats and displays the callback parameters if the 
*           runtime messaging level is set sufficiently.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_DECODE)
#define DAPI_DEBUG_CALLBACK_DECODE(cmd, event, cbInfo, dapi_g) dapiDebugCallbackDecode((cmd), (event), (cbInfo), (dapi_g))
#else
#define DAPI_DEBUG_CALLBACK_DECODE(cmd, event, cbInfo, dapi_g)
#endif /* DAPI_DEBUG_DECODE */

/****************************************************************************************
*
* @purpose  Perform basic DAPI level initialization for debug
*
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to success if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           not enabled. Otherwise it expands to a function call that initializes DAPI 
*           wide debug parameters.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_INIT(dapi_g)              dapiDebugInit((dapi_g), L7_TRUE)
#else

#if (DAPI_DEBUG_MSGLVL != DAPI_DEBUG_NONE)
#define DAPI_DEBUG_INIT(dapi_g)              dapiDebugInit((dapi_g), L7_FALSE)
#else                                            
#define DAPI_DEBUG_INIT(dapi_g)              L7_SUCCESS
#endif

#endif /* (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS) */

/****************************************************************************************
*
* @purpose  Perform port-based DAPI level initialization for debug
*
* @param   *usp     @b{(input)} The unit, slot, port designation for the command.
* @param   *dapi_g  @b{(input)} The driver object
*
* @returns  void
*
* @comments Macro expands to nothing if the compile time message level (DAPI_DEBUG_MSGLVL) is
*           less than DAPI_DEBUG_INDICATIONS (currently 4). Otherwise it expands to a function 
*           call that initializes port-based DAPI debug parameters.
*
* @end
*
****************************************************************************************/
#if (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS)
#define DAPI_DEBUG_PORT_INIT(usp, dapi_g)         dapiDebugPortInit((usp), (dapi_g))
#else
#define DAPI_DEBUG_PORT_INIT(usp, dapi_g)
#endif /* (DAPI_DEBUG_MSGLVL >= DAPI_DEBUG_INDICATIONS) */

/****************************************************************************************
*
* @purpose  Provides easy expander to IP Addresses for use as arguments to printf-like
*           statements.
*
* @param    ipAddr  @b{(input)} The 32-bit big-endian IP Address
*
* @returns  void
*
* @comments The printf-like statement must have in the format string four (4) %d symbols
*           to correspond to each of the expanded arguments. The preferred use would be
*           %d.%d.%d.%d .
*
* @end
*
****************************************************************************************/
#define IP_EXPAND(ipAddr) (unsigned char)(((ipAddr)>>24) & 0xFF),(unsigned char)(((ipAddr)>>16) & 0xFF),\
                          (unsigned char)(((ipAddr)>>8) & 0xFF), (unsigned char)((ipAddr) & 0xFF)     

/****************************************************************************************
*
* @purpose  Provides easy expander to MAC Addresses for use as arguments to printf-like
*           statements.
*
* @param   *macAddr @b{(input)} Pointer to a 6-byte char string containing the MAC address
*
* @returns  void
*
* @comments The printf-like statement must have in the format string six (6) %x symbols
*           to correspond to each of the expanded arguments. The preferred use would be
*           %02X:%02X:%02X:%02X:%02X:%02X .
*
* @end
*
****************************************************************************************/
#define MAC_EXPAND(macAddr) (macAddr)[0], (macAddr)[1], (macAddr)[2], (macAddr)[3], (macAddr)[4], (macAddr)[5]

/****************************************************************************************
*
* @purpose  Provides easy expander to USPs for use as arguments to printf-like statements.
*
* @param   *pUsp    @b{(input)} Pointer to a USP structure.
*
* @returns  void
*
* @comments The printf-like statement must have in the format string three (3) %d symbols
*           to correspond to each of the expanded arguments. The preferred use would be
*           %d.%d.%d .
*
* @end
*
****************************************************************************************/
#define USP_EXPAND(pUsp) ((DAPI_USP_t *)(pUsp))->unit,((DAPI_USP_t *)(pUsp))->slot,((DAPI_USP_t *)(pUsp))->port

/*******************************************************************************
*
* @purpose  Compress a USP value into a format usable as a single 32-bit value. Typically 
*           used for LOG_ERROR to convey the port information. 
*
* @param   *pUsp    @b{(input)} Pointer to a USP structure.
*
* @returns  L7_uint32           A compressed USP useful for debug
*
* @comments Only 16 bits are defined but a 32-bit unsigned integer is returned for convenience
*
* @end
*
*******************************************************************************/
#define USP_COMPRESS(pUsp)                          \
        (L7_uint32)((((pUsp)->unit<<12) & 0xF000) | \
                    (((pUsp)->slot<<8) & 0x0F00)  | \
                    (((pUsp)->port) & 0x00FF))


/*******************************************************************************
*
* @purpose  DeCompress a 32bit usp into the DAPI_USP_t
*
* @param    _uspCompressed  @b{(input)}  32 bit usp  
* @param   *pUsp            @b{(output)} Pointer to a USP structure.
*
* @returns  L7_uint32           A compressed USP useful for debug
*
* @comments Only 16 bits are defined but a 32-bit unsigned integer is returned for convenience
*
* @end
*
*******************************************************************************/
#define USP_DECOMPRESS(_uspCompressed,pUsp){        \
        (pUsp)->unit = (_uspCompressed>>12) & 0xF;  \
        (pUsp)->slot = (_uspCompressed>>8)  & 0xF;  \
        (pUsp)->port = (_uspCompressed)  & 0xFF; \
        } 

                    
#endif  /* INCLUDE_DAPI_DEBUG_H */
