/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename fdb.h
*
* @purpose Forwarding Data Base Manager header File
*
* @component fdb
*
* @comments none
*
* @create 09/14/2000
*
* @author bmutz
*
* @end
*             
**********************************************************************/


#ifndef FDB_H
#define FDB_H

#include "l7_common.h"
#include "defaultconfig.h"
#include "commdefs.h"

#define FDB_QUEUE                     "FDQ-Q "
#define FDB_CFG_FILENAME  "fdb.cfg"
#define FDB_CFG_VER_1     0x1
#define FDB_CFG_VER_CURRENT FDB_CFG_VER_1

/* Used to size array of interface statistics. */
#define L7_FDB_INTF_INDEX_MAX      (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1)

typedef struct
{
#ifdef L7_PRODUCT_SMARTPATH
  L7_ushort16   fid;
  L7_ushort16   vlanId;
#else
  L7_uint32   fid;
  L7_uint32   vlanId;
#endif
  L7_uint32   addressAgingTimeOut;

} fdbFIDData_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  fdbFIDData_t   fdbFIDData[L7_MAX_FILTERING_DATABASES];
  L7_uint32      checkSum;

} fdbCfgData_t;

typedef struct fdbIdTable_t
{
    L7_BOOL   insue;
    L7_uint32 fdbId;
} fdbIdTable_s;

typedef struct 
{
  L7_uint32 dynamic_entries;            /* rfc 2674: dot1qFdbDynamicCount, current   */
  L7_uint32 static_entries;             /* static entries of type delete-on-reset, 
                                           delete-on-timeout, permanent, or secure */
  L7_uint32 internal_entries;           /* keep count of other entries */
} fdbStatsEntry_t;

typedef struct fdb_stats_s
{
  L7_uint32 total_adds;                 /* total add attempts                        */
  L7_uint32 total_dels;                 /* total delete attempts                     */
  L7_uint32 dup_adds;                   /* additions of existent entries             */
  L7_uint32 bad_adds;                   /* failure to allocate space for add         */
  L7_uint32 bad_dels;                   /* attempts to delete non-existant entries   */
  L7_uint32 most_entries;               /* maximum entries ever in table             */
  L7_uint32 cur_entries;                /* current entries in table                  */
  L7_uint32 dynamic_entries;            /* rfc 2674: dot1qFdbDynamicCount, current   */
                                        /* number of dynamic entries in the table    */
  L7_uint32 static_entries;             /* static entries of type delete-on-reset, 
                                           delete-on-timeout, permanent, or secure */
  L7_uint32 internal_entries;           /* keep count of other entries */
  fdbStatsEntry_t vlanStats[L7_MAX_FILTERING_DATABASES];  /* statistics per vlan */

  /* index by internal interface number */
  fdbStatsEntry_t intfStats[L7_FDB_INTF_INDEX_MAX];       /* statistics per interface */

} fdb_stats_t;

typedef struct fdbLearnMsg
{
  L7_uchar8 msgsType;                    /* add or del              */
  L7_uchar8 entryType;                   /* fdb_entry_type_t        */
  L7_uchar8 mac_addr[L7_MAC_ADDR_LEN];   /* mac address             */
#ifndef L7_PRODUCT_SMARTPATH
  L7_uint32 intIfNum; /* port interface number   */
  L7_uint32 vlanId;
#else
  L7_ushort16 intIfNum; /* port interface number   */
  L7_ushort16 vlanId;
#endif
  L7_BOOL reflect_to_hw;  /* PTin added */
} fdbLearnMsg_t;


/*********************************************************************
* @purpose  Saves fdb user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbSave(void);

/*********************************************************************
* @purpose  Checks if fdb user config data is changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL fdbHasDataChanged(void);
void fdbResetDataChanged(void);

/*********************************************************************
* @purpose  Build default fdb config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void fdbBuildDefaultConfigData(L7_uint32 ver);
                                                                              
/*********************************************************************
* @purpose  FDB Task    
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void fdbTask( void );

/*********************************************************************
* @purpose  Phase 1 initialization
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseOneInit(void);

/*********************************************************************
* @purpose  Phase 2 initialization
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseTwoInit(void);

/*********************************************************************
* @purpose  Phase 3 initialization
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseThreeInit(void);


/*********************************************************************
* @purpose  Phase Exec cleanup
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
void fdbPhaseExecFini(void);

/*********************************************************************
* @purpose  Phase Execute initialization
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseExecInit();


/*********************************************************************
* @purpose  Phase 1 cleanup
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
void fdbPhaseOneFini(void);

/*********************************************************************
* @purpose  Phase 2 cleanup
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
void fdbPhaseTwoFini(void);

/*********************************************************************
* @purpose  Phase 3 cleanup
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseThreeFini();

/*********************************************************************
* @purpose  Reset all the Tables in the FDB
*
* @param    void 
*          
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbPhaseUnconfig(void);


/* fdb_migrate.c */

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void fdbMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* FDB_H */
