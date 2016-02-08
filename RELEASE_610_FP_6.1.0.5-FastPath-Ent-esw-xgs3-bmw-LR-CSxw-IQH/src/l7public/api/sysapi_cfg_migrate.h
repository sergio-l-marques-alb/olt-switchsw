
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename sysapi_cfg_migrate.h
*
* @purpose sysapi defines and structure definitions
*
* @component sysapi
*
* @create	8/23/2004
*
* @author	Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef SYSAPI_CFG_MIGRATE_H
#define SYSAPI_CFG_MIGRATE_H

#include "nimapi.h"
#include "nvstoreapi.h"
#include "l7utils_api.h"

typedef struct
{
  L7_uchar8 filename[L7_MAX_FILENAME];  /* file name                       */
  L7_uint32 version;            /* software version of file        */
  L7_uint32 componentID;        /* L7 software component ID        */
  L7_uint32 type;               /* type of data-component specific */
  L7_uint32 length;             /* length of data, including hdr   */
  L7_uint32 dataChanged;        /* file changed if not NULL        */
} L7_fileHdrV1_t;

typedef struct
{
  L7_uchar8 filename[L7_MAX_FILENAME];  /* file name                       */
  L7_uint32 version;            /* software version of file        */
  L7_uint32 componentID;        /* L7 software component ID        */
  L7_uint32 type;               /* type of data-component specific */
  L7_uint32 length;             /* length of data, including hdr   */
  L7_uint32 dataChanged;        /* file changed if not NULL        */
  L7_uint32 savePointId;        /* random number generator */
  L7_uint32 targetDevice;       /* Identifies the hardware compatible with this file */
  L7_uint32 pad[8];             /* Space for future enhancements. */
} L7_fileHdrV2_t;

typedef L7_fileHdr_t L7_fileHdrV3_t;



#define DUMMY_MIGRATE_FUNCTION(oldVer,ver,newSize)\
do{\
  LOG_MSG("DUMMY Version %d %d Size %d\n", (L7_int32)oldVer, (L7_int32)ver, (L7_int32)newSize);\
}while(0);

#define MIGRATE_FUNCTION(oldVer,ver,newSize)\
do{\
  /* LOG_MSG("IMPLEMENTED Version %d %d Size %d\n",oldVer, ver, newSize); */\
}while(0);

#if 1
#define MIGRATE_INVALID_SLOT_PORT(slot,port)\
do{\
  LOG_MSG("Invalid slot %d or port %d or both\n",(L7_int32)slot, (L7_int32)port);\
}while(0);

#define MIGRATE_INVALID_INDEX(index)\
do{\
  LOG_MSG("Failed to get interface info for index %d\n", (L7_int32)index);\
}while(0);
#else

#define MIGRATE_INVALID_SLOT_PORT(...)
#define MIGRATE_INVALID_INDEX(...)
#endif

#define MIGRATE_INDEX_OUTOF_BOUND(index)\
do{\
  LOG_MSG("intIfNum %d is out of bound\n", (L7_int32)index);\
}while(0);

#define MIGRATE_INCOMPLETE()\
do{\
  LOG_MSG("This is incomplete: Check Back\n");\
}while(0);

#define MIGRATE_ONLY_VERSION(oldVer,ver,newSize)\
do{\
  LOG_MSG("IGNORE Version only changed: Version %d %d Size %d\n", (L7_int32)oldVer, (L7_int32)ver, (L7_int32)newSize);\
}while(0);

#define MIGRATE_LOG_INTERFACE_TRUNCATE(file, intfCfgIndex)\
do{\
  LOG_MSG ("File %s: Forced truncation of interface configuration migration at %d entries\n", file, (L7_int32)intfCfgIndex);\
}while(0);

#define MIGRATE_DEBUG LOG_MSG


#define MIGRATE_LOG_UNEXPECTED_VERSION(cur,exp)\
do{\
    LOG_MSG ("Unexpected Header Version= %d expecting %d\n", (L7_int32)cur, (L7_int32)exp);\
}while(0);

#define MIGRATE_ASSERT(x)\
do{\
  if(!(x))\
  {\
    L7_char8 * _filename = __FILE__; \
    l7utilsFilenameStrip(&_filename); \
    LOG_MSG("[%s: %d]: Configuration Migration Assertion\n", _filename, __LINE__);\
  }\
}while(0);

#define MIGRATE_COPY_STRUCT(dst,src)\
do{\
  /* MIGRATE_ASSERT(sizeof(dst) == sizeof(src)); */\
  if(sizeof(dst) == sizeof(src))\
    memcpy(&dst, &src, sizeof(src));\
  else\
  {\
    L7_char8 * _filename = __FILE__; \
    l7utilsFilenameStrip(&_filename); \
    LOG_MSG("[%s: %d]: Data Sizes (%d-%d) are DIFFERENT\n", _filename, __LINE__, \
            (L7_int32)sizeof(dst), (L7_int32)sizeof(src));\
  }\
}while(0);

#define MIGRATE_COPY_NIM_INTF_MASK(dst,src)\
do{\
  if(sizeof(dst) < sizeof(src))\
    memcpy(&dst, &src, sizeof(dst));\
  else\
    memcpy(&dst, &src, sizeof(src));\
}while(0);


#define SYSAPI_MIGRATE_UNUSED(var) (void)(var)


/* Undefine to turn on tracing of sysapi messages */
/* #define MIGRATE_TRACE LOG_MSG */
#define MIGRATE_TRACE(__fmt__, __args__... ) {}
#include "transfer_exports.h"

/*********************************************************************
* @purpose  Create mapping for slot/port from release 4.0 to subsequent release
*
* @param    index           (@input)index of interface entry in release 4.0 config file
* @param    configId        (@output)pointer to current release nimConfigID_t structure
* @param    configIdOffset  (@output)pointer to current release configIdOffset for interface
* @param    intfType        (@output)pointer to interface type (one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileRel4_0SlotPortToIntfInfoGet (L7_uint32 slot,
                                                  L7_uint32 port,
                                                  nimConfigID_t * configId,
                                                  L7_uint32 * configIdOffset,
                                                  L7_INTF_TYPES_t * intfType);

/*********************************************************************
* @purpose  Create mapping for interfaces from release 4.0 to subsequent release
*
* @param    index           (@input)index of interface entry in release 4.0 config file
* @param    configId        (@output)pointer to current release nimConfigID_t structure
* @param    configIdOffset  (@output)pointer to current release configIdOffset for interface
* @param    intfType        (@output)pointer to interface type (one of L7_INTF_TYPES_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileRel4_0IndexToIntfInfoGet (L7_uint32 index,
                                               nimConfigID_t * configId,
                                               L7_uint32 * configIdOffset,
                                               L7_INTF_TYPES_t * intfType);

/*********************************************************************
*
* @purpose To fill VLAN interface mapping
*
* param   mappingIndex  Mapping Internal Interface Number
* param   vlanId
* param   slot
* param   port
*
* @returns L7_ERROR
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCfgFileMapVlanInterfaces (L7_uint32 mappingIndex, L7_uint32 vlanId,
                                        L7_uint32 slot, L7_uint32 port);
/*********************************************************************
*
* @purpose Determine if this is a valid unit for migration
*
* @param   void
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes    Between release 4.2 and 4.3, the number of valid ports
*           for non-stacking builds decreased due to a reduction
*           in the number of allowable unit IDs. Do not migrate
*           the ports from unsupported boxes.
*
* @end
*
*********************************************************************/
L7_BOOL sysapiCfgFileRel4_3ValidUnit(L7_uint32 unit);



/*********************************************************************
* @purpose  Converts the external interface number from release 4.0 
*           to release 4.1 versions
*
* @param    extIfNum        release 4.0 external interface number (ifIndex)
* @param    *unit           pointer to unit parameter
* @param    *slot           pointer to slot parameter
* @param    *port           pointer to port parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void sysapiCfgFileMigrateExtIfNumToUnitSlotPortRel4_0Get (L7_uint32 extIfNum, 
                                                            L7_uint32 *unit,
                                                            L7_uint32 *slot,
                                                            L7_uint32 *port);

#endif /* SYSAPI_CFG_MIGRATE_H */
