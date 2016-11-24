/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        ospfmib.c
 *
 * @purpose         OSPF MIB tables support(according to RFC 1850)
 *
 * @component       Routing OSPF Component
 *
 * @comments
 *
 * @create          03/01/2000
 *
 * @author          Elena Korchemny
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/application/routing/protocol/ospf/ospfmib.c,v 1.1 2011/04/18 17:10:29 mruas Exp $";
#endif


#include "std.h"
#include "oll.ext"
#include "xx.ext"

/* --- specific include files --- */
#include "rfc1443.h"
#include "spapi.ext"
#include "spcfg.h"
#include "spobj.h"

#include "ospfmib.ext"
#include "sysmib.h"


/* ---- internal definitions --- */

/* IP address length */
#define IP_ADDR_SIZE 4


/******************************************************/
/* Range Test tables definitions for OSPF MIB Tables  */
/******************************************************/

/* ---- ospfAreaTable ---- */

static t_rangeTestTable ospfAreaTestTable[] =
{{ospfAreaTableAuthTypeSubId,    0, 2},
 {ospfAreaTableImpAsExtSubId,    1, 2 },
 {ospfAreaTableAreaSummarySubId, 1, 2}, 
 {ospfAreaTableRowStatusSubId, active, destroy}};


/* ---- ospfStubAreaTable ---- */

static t_rangeTestTable ospfStubAreaTestTable[] =  
{{ospfStubAreaTableRowStatusSubId, active, destroy},
{ospfStubAreaTableMetricTypeSubId, 1, 3}};


/* ---- ospfAreaRangeTable - obsolete table ---- */

/*static t_rangeTestTable ospfAreaRangeTestTable[] =  
{{ospfAreaRangeTableRowStatusSubId, active, destroy}
{ospfAreaRangeTableEffectSubId, 1, 2}}; */


/* ---- ospfHostTable ---- */

static t_rangeTestTable ospfHostTestTable[] =  
{{ospfHostTableRowStatusSubId, active, destroy}};


/* ---- ospfIfTable ---- */

static t_rangeTestTable ospfIfTestTable[] =  
{ {ospfIfTableAdminStatusSubId, enabled, disabled},
  {ospfIfTableRowStatusSubId, active, destroy},
  {ospfIfTableMltCastFrwrdSubId, 1, 3},
  {ospfIfTableIfDemandSubId, _true, _false},
  {ospfIfTableAuthTypeSubId, 0, 255} };


/* ---- ospfIfMetricTable ---- */

static t_rangeTestTable ospfIfMetricTestTable[] =  
{{ospfIfMetricTableRowStatusSubId, active, destroy}};

                                    
/* ---- ospfVirtIfTable ---- */

static t_rangeTestTable ospfVirtIfTestTable[] =  
{{ospfVirtIfTableRowStatusSubId, active, destroy},
  {ospfVirtIfTableAuthTypeSubId, 0, 255} };


/* ---- ospfNbrTable ---- */

static t_rangeTestTable ospfNbrTestTable[] =  
{  {ospfNbrTableNbrPriorSubId, 0, 255},
   {ospfNbrTableRowStatusSubId, active, destroy},
   {ospfNbrTableHelloSuprSubId, _true, _false}};

/* Convert physical interface type to default ospf link type*/
static e_IFO_Types CONVERT_PHYTYPE_TO_IFTYPE[] =
   /* OSPF_ETHERNET,   OSPF_FRAME_RELAY,   OSPF_ATM */
{  IFO_BRDC ,       IFO_PTP,            IFO_PTP};


/* ---- ospfAreaAggregateTable ---- */

static t_rangeTestTable ospfAreaAggrTestTable[] =  
{  {ospfAreaAggrTableRowStatusSubId, active, destroy},
   {ospfAreaAggrTableEffectSubId, 1, 2}};


/*------------------------
 * Static procedures
 *------------------------*/
/* static functions definitions */

static e_Err ospfStubAreaTable_Active(t_Handle ospfMibObj, ospfStubAreaEntry *p);
static e_Err ospfStubAreaTable_Deactive(t_Handle ospfMibObj, ospfStubAreaEntry *p);
static e_Err ospfStubAreaTable_InternalNewAndActive(t_Handle ospfMibObj,
                                                    ulng stubAreaId,
                                                    ulng stubAreaTOS,
                                                    ulng metric,
                                                    e_StubMetricType metricType);

static e_Err ospfHostTable_Active(t_Handle ospfMibObj, ospfHostEntry *p);
static e_Err ospfHostTable_Deactive(t_Handle ospfMibObj, ospfHostEntry *p);
static e_Err ospfHostTable_InternalNewAndActive(t_Handle ospfMibObj,
                                                t_S_HostRouteCfg *p_HostCfg);

static ospfIfEntry *ospfFindIfEntryFromIfIndex(t_Handle ospfMibObj, ulng ifIndex );
static e_Err ospfIfTable_Active(t_Handle ospfMibObj, ospfIfEntry  *p);
static e_Err ospfIfTable_Deactive(t_Handle ospfMibObj, ospfIfEntry  *p);

static e_Err ospfIfMetricTable_InternalNewAndActive(t_Handle ospfMibObj,
                                                    ulng ifIpAddr,
                                                    ulng ifAddrLessIf,
                                                    ulng ifTOS,
                                                    word mValue);

static e_Err ospfVirtIfTable_Active(t_Handle ospfMibObj, ospfVirtIfEntry *p);
static e_Err ospfVirtIfTable_Deactive(ospfVirtIfEntry  *p);

static e_Err ospfNbrTable_Active(t_Handle ospfMibObj, ospfNbrEntry *p);
static e_Err ospfNbrTable_Deactive(t_Handle ospfMibObj, ospfNbrEntry *p);

static e_Err ospfAreaAggrTable_Deactive(t_Handle ospfMibObj, ospfAreaAggrEntry *p);
static e_Err ospfAreaAggrTable_Active(t_Handle ospfMibObj, ospfAreaAggrEntry *p);
static e_Err ospfAreaAggrTable_InternalNewAndActive(t_Handle ospfMibObj,
                                                    ulng aggrAreaId,
                                                    ulng aggrLsdbType,
                                                    ulng aggrNet,
                                                    ulng aggrMask,
                                                    e_ospfAreaAggrEffect effect);

/********************************************************************/
/* OSPF MIB object type definitions                                 */
/* This object must be created before OSPF MIB code is called.      */
/* t_Handle of this object is used in each procedure call.          */
/********************************************************************/

typedef struct t_OspfMibObj
{
   struct t_OspfMibObj *next;
   struct t_OspfMibObj *prev;

   /* OSPF Router object and router manager object */
   t_Handle ospfRouterObj; /* Have to be set during start up of the system */
   t_Handle ospfRouterMng;

   /* MIB tables */
                                 /* 1. general variables: in router
                                       configuration structure */
   t_Handle  ospfAreaTable;      /* 2. area data structure handle */
   t_Handle  ospfStubAreaTable;  /* 3. area stub metric table handle */
   t_Handle  ospfLsdbTable;      /* 4. link state database handle */
                                 /* 5. address range table - obsolete */
   t_Handle  ospfHostTable;      /* 6. host table handle */
   t_Handle  ospfIfTable;        /* 7. i/f table handle */
   t_Handle  ospfIfMetricTable;  /* 8. i/f metric table handle */
   t_Handle  ospfVirtIfTable;    /* 9. virtual i/f table handle */
   t_Handle  ospfNbrTable;       /* 10. neighbor table handle */
   t_Handle  ospfVirtNbrTable;   /* 11. virtual neighbor table handle */
   t_Handle  ospfExtLsdbTable;   /* 12. exernal LS database handle */
   t_Handle  ospfAreaAggrTable;  /* 13. area aggregate table handle */

   /* Callback routines */
   t_ospfMibCallbacks ospfMibCallbacks;

} t_OspfMibObj;

static t_OspfMibObj *ospfMibObjList;


/* ------- References to OSPF MIB tables ------- */
/* Most service routines have a "t_Handle ospfMibObj" parameter, */
/* which is a pointer to the MIB object. All the tables will be  */
/* accessed through this object. */

#define _ospfAreaTable_      ((t_OspfMibObj *)ospfMibObj)->ospfAreaTable
#define _ospfStubAreaTable_  ((t_OspfMibObj *)ospfMibObj)->ospfStubAreaTable
#define _ospfLsdbTable_      ((t_OspfMibObj *)ospfMibObj)->ospfLsdbTable
#define _ospfHostTable_      ((t_OspfMibObj *)ospfMibObj)->ospfHostTable
#define _ospfIfTable_        ((t_OspfMibObj *)ospfMibObj)->ospfIfTable
#define _ospfIfMetricTable_  ((t_OspfMibObj *)ospfMibObj)->ospfIfMetricTable
#define _ospfVirtIfTable_    ((t_OspfMibObj *)ospfMibObj)->ospfVirtIfTable
#define _ospfNbrTable_       ((t_OspfMibObj *)ospfMibObj)->ospfNbrTable
#define _ospfVirtNbrTable_   ((t_OspfMibObj *)ospfMibObj)->ospfVirtNbrTable
#define _ospfExtLsdbTable_   ((t_OspfMibObj *)ospfMibObj)->ospfExtLsdbTable
#define _ospfAreaAggrTable_  ((t_OspfMibObj *)ospfMibObj)->ospfAreaAggrTable


/*********************************************************************
 * @purpose              Create new OSPF MIB object
 *
 *
 * @param  RTO_Obj       @b{(input)}    parent RTO object handler
 * @param  rtrMng        @b{(input)}    router manager handler
 * @param  p_ospfMibObj  @b{(input)}   t_Handle of the created OSPF MIB
 *                                     object
 *
 * @returns              E_OK       - success
 * @returns              E_BADPARM  - invalid parameter
 * @returns              E_NOMEMORY - can't allocate memory
 *
 * @notes         Create new OSPF MIB object.
 *                This object stores handles of all MIB tables defined in
 *                OSPF MIB. It has to be created before any ospfMib procedure
 *                is called to create/delete/read OSPF MIB information.
 *
 *                Store t_Handle of the the router object to ospfRouterObj
 *                and t_Handle of the router manager to ospfRouterMng .
 *                This procedure has to be called by stack manager during
 *                start up, when the RTO object is created.
 *
 * @end
 * *******************************************************************/
e_Err ospfCreateMibObj (t_Handle RTO_Obj, t_Handle rtrMng,
                               t_Handle  *p_ospfMibObj)
{
   t_OspfMibObj *p_ospfMib;

   if(!RTO_Obj)
      return E_BADPARM;

   /* Allocate memory for the object */
   p_ospfMib = (t_OspfMibObj *)XX_Malloc(sizeof(t_OspfMibObj));

   if (p_ospfMib)
      memset (p_ospfMib, 0, sizeof(t_OspfMibObj));
   else
      return E_NOMEMORY;

   /* set the "owner" router object field and router manager field */
   p_ospfMib->ospfRouterObj = RTO_Obj;
   p_ospfMib->ospfRouterMng = rtrMng;

   XX_AddToDLList(p_ospfMib, ospfMibObjList);
   *p_ospfMibObj = p_ospfMib;

   return E_OK;

} /* End of ospfCreateMibObj procedure */



/*********************************************************************
 * @purpose                 Deletes OSPF MIB object.
 *
 *
 * @param   ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 *
 * @returns                 E_OK - success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfDestroyMibObj (t_Handle ospfMibObj)
{
   t_OspfMibObj *p = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_OK;

   if (p->ospfAreaTable)
      L7Mib_DeleteTable(p->ospfAreaTable);

   if (p->ospfStubAreaTable)
      L7Mib_DeleteTable(p->ospfStubAreaTable);

   if (p->ospfLsdbTable)
      L7Mib_DeleteTable(p->ospfLsdbTable);

   if (p->ospfHostTable)
      L7Mib_DeleteTable(p->ospfHostTable);

   if (p->ospfIfTable)
      L7Mib_DeleteTable(p->ospfIfTable);

   if (p->ospfIfMetricTable)
      L7Mib_DeleteTable(p->ospfIfMetricTable);

   if (p->ospfVirtIfTable)
      L7Mib_DeleteTable(p->ospfVirtIfTable);

   if (p->ospfNbrTable)
      L7Mib_DeleteTable(p->ospfNbrTable);

   if (p->ospfVirtNbrTable)
      L7Mib_DeleteTable(p->ospfVirtNbrTable);

   if (p->ospfExtLsdbTable)
      L7Mib_DeleteTable(p->ospfExtLsdbTable);

   if (p->ospfAreaAggrTable)
      L7Mib_DeleteTable(p->ospfAreaAggrTable);

   XX_DelFromDLList(p, ospfMibObjList);

   XX_Free(ospfMibObj);
   return E_OK;

}  /* End of ospfDestroyMibObj procedure */



/*********************************************************************
 * @purpose      Get the handle of the router object associated with the
 *               current MIB table, and a handle of its router manager
 *
 *
 * @param   ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param   rtrObj       @b{(input)}  the returned parent RTO object handler
 * @param   rtrMng       @b{(input)}  the returned router manager handler
 *
 * @returns              E_OK      - success
 * @returns              E_BADPARM - invalid parameter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetRouterObj(t_Handle ospfMibObj, t_Handle *rtrObj, t_Handle *rtrMng)
{
   t_OspfMibObj *p = (t_OspfMibObj *)ospfMibObj;

   if((!ospfMibObj) || (!rtrObj) || (!rtrMng))
      return E_BADPARM;

   *rtrObj = p->ospfRouterObj;
   *rtrMng = p->ospfRouterMng;

   return E_OK;
}


/*********************************************************************
 * @purpose       Binds user supplied procedures to ospfMibCallbacks
 *                structure. All these procedures are used in OSPF MIB
 *                support code
 *
 * @param ospfMibObj    @b{(input)}   t_Handle of the OSPF MIB object
 * @param mibCallbacks  @b{(input)}   structure that contains user-supplied
 *                                    callback routines
 *
 * @returns       E_OK      - success
 * @returns       E_BADPARM - invalid parameter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfSetMibCallbacks (t_Handle ospfMibObj,
                           t_ospfMibCallbacks *mibCallbacks)
{
   t_OspfMibObj *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if((!ospfMibObj) || (!mibCallbacks))
      return E_BADPARM;

   p_ospfMib->ospfMibCallbacks.f_Get_IfLowLayerId =
                mibCallbacks->f_Get_IfLowLayerId;
   
   p_ospfMib->ospfMibCallbacks.f_Get_IfParams =
                mibCallbacks->f_Get_IfParams;

   p_ospfMib->ospfMibCallbacks.f_Get_IfMask =
               mibCallbacks->f_Get_IfMask;

   p_ospfMib->ospfMibCallbacks.f_Get_HostRoute =
               mibCallbacks->f_Get_HostRoute;

   p_ospfMib->ospfMibCallbacks.f_ifoCreateDelInd = 
               mibCallbacks->f_ifoCreateDelInd;
   return E_OK;
}


/*********************************************************************
 * @purpose      Sets the OSPF Router ID.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  rtrId        @b{(input)}  The OSPF router id (IP address)
 *
 * @returns             E_OK     - success
 * @returns             not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfSetRouterId(t_Handle ospfMibObj, ulng rtrId)
{
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;
   t_S_RouterCfg  rtrCfg;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.RouterId     = rtrId;
   rtrCfg.RouterStatus = ROW_CHANGE;
   return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
}



/*********************************************************************
 * @purpose      Returns the OSPF Router IP address.
 *
 *
 * @param ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param rtrId        @b{(input)}  a handle to the returned OSPF router IP
 *                                  Address
 *
 * @returns            E_OK      - success
 * @returns            E_BADPARM - invalid parameter(s)
 * @returns            E_FAILED  - acion failed
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetRouterId(t_Handle ospfMibObj, ulng *rtrId)
{
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;
   t_S_RouterCfg  rtrCfg;

   if ((!ospfMibObj) || (!rtrId))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *rtrId = rtrCfg.RouterId;
   return E_OK;
}



/*********************************************************************
 * @purpose      Sets the administrative status of OSPF process in the
 *               router.
 *
 *
 * @param ospfMibObj     @b{(input)}   t_Handle of the OSPF MIB object
 * @param admStatus      @b{(input)}   the new value of admin. status
 *
 * @returns       E_OK     - success
 * @returns       not E_OK - failure
 *
 * @notes         The value "enabled" denotes that the OSPF
 *               Process is active on at least one interface; "disabled"
 *               disables it on all interfaces.
 *
 * @end
 * ********************************************************************/
e_Err ospfSetAdminStatus(t_Handle ospfMibObj, e_ospfAdminStatus admStatus)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.AdminStat    = ((admStatus == enabled) ? TRUE : FALSE);
   rtrCfg.RouterStatus = ROW_CHANGE;

   return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
}


/*********************************************************************
 * @purpose              Returns the administrative status of the OSPF
 *                       router.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  admStatus     @b{(input)}  the returned admin. status
 *
 * @returns              E_OK     - success
 * @returns              not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetAdminStatus(t_Handle ospfMibObj, e_ospfAdminStatus *admStatus)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!admStatus))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;
    
   *admStatus = ((rtrCfg.AdminStat == TRUE) ? enabled : disabled);
   return E_OK;
}




/*********************************************************************
 * @purpose       Returns the current version number of the OSPF protocol.
 *
 *
 * @param         @b{(input)}  none
 *
 * @returns       The OSPF protocol version number
 * @returns
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int ospfGetVersionNumber(void)
{
   return OSPF_VERSION;
}


/*********************************************************************
 * @purpose      Returns value of the flag indicating whether this router
 *               is an area border router.
 *
 *
 * @param   ospfMibObj   @b{(input)}   t_Handle of the OSPF MIB object
 * @param   flag         @b{(input)}   the OSPF area border router flag
 *                                     value
 *
 * @returns              E_OK     - success
 * @returns              not E_OK - failure
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetAreaBrdRouterFlag(t_Handle ospfMibObj, e_TruthValue *flag)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!flag))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *flag = ((rtrCfg.AreaBdrRtrStatus == TRUE) ? _true : _false);
   return E_OK;
}


/*********************************************************************
 * @purpose      This function can no longer be used. The user is no
 *               longer ble to explicitly configure the ASBR status.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  flag         @b{(input)}  the new flag value to be set
 *                                   (true or false)
 *
 * @returns            E_OK     - success
 * @returns            not E_OK - failure
 *
 * @notes        Instead, the user configures route redistribution from
 *               specific sources.A router that redistributes from one
 *               or more sources is considered an ASBR.
 *
 * @end
 * ********************************************************************/
e_Err ospfSetASBrdRouterFlag(t_Handle ospfMibObj, e_TruthValue flag)
{
    return E_FAILED;
}



/*********************************************************************
 * @purpose      Returns value of the flag indicating whether this router
 *               is configured as an Autonomous System border router.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  flag         @b{(input)}  the OSPF AS border router flag value
 *
 * @returns             E_OK     - success
 * @returns             not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetASBrdRouterFlag(t_Handle ospfMibObj, e_TruthValue *flag)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!flag))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *flag = ((rtrCfg.ASBdrRtrStatus == TRUE) ? _true : _false);
   return E_OK;
}


/*********************************************************************
 * @purpose      Returns the number of external link-state advertisements
 *               in the link-state database.
 *
 *
 * @param  ospfMibObj  @b{(input)}   t_Handle of the OSPF MIB object
 * @param  count       @b{(input)}   the returned external LSA counter
 *                                   value
 *
 * @returns            E_OK     - success
 * @returns            not E_OK - failure
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ospfGetExternLsaCount(t_Handle ospfMibObj, ulng *count)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!count))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *count = rtrCfg.ExternLsaCount;
   return E_OK;
}


/*********************************************************************
 * @purpose      Returns the sum of the LS checksums of the external
 *               link-state advertisements contained in the link-state
 *               database.
 *
 *
 * @param  ospfMibObj     @b{(input)}   t_Handle of the OSPF MIB object
 * @param  cksumSum       @b{(input)}   returned value of external LSA
 *                                      checksums sum
 *
 * @returns               E_OK     - success
 * @returns               not E_OK - failure
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ospfGetExternLsaCksumSum(t_Handle ospfMibObj, ulng *cksumSum)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *cksumSum = rtrCfg.ExternLsaCksumSum;
   return E_OK;
}



/*********************************************************************
 * @purpose              Returns the router's support for type_of_
 *                       service routing.
 *
 *
 * @param  ospfMibObj    @b{(input)}   t_Handle of the OSPF MIB object
 * @param  tosSupport    @b{(input)}   TRUE when TOS routing is supported;
 *                                      FALSE when TOS routing is not supported.
 *
 * @returns              E_OK     - success
 * @returns              not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetTOSSupport(t_Handle ospfMibObj, e_TruthValue *tosSupport)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!tosSupport))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *tosSupport = (rtrCfg.TOSSupport ? _true : _false);
   return E_OK;
}
 


/*********************************************************************
 * @purpose      Sets the router's support for type_of_service routing.
 *
 *
 * @param  ospfMibObj    @b{(input)}   t_Handle of the OSPF MIB object
 * @param  tosSupport    @b{(input)}   the new value of TOS support to be
 *                                     set
 *
 * @returns              E_OK     - success
 * @returns              not E_OK - failure
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ospfSetTOSSupport(t_Handle ospfMibObj, e_TruthValue tosSupport)
{
   /* This version of OSPF has no TOS support. So, there's nothing to do. */
   return E_OK;

   /*t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.RouterStatus = ROW_CHANGE;  
   rtrCfg.TOSSupport   = ((tosSupport == _true) ? TRUE: FALSE);
   return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg); */
}



/*********************************************************************
 * @purpose      Returns the number of the new link-state advertisements
 *               that have been originated
 *
 *
 * @param   ospfMibObj  @b{(input)}   t_Handle of the OSPF MIB object
 * @param   count       @b{(input)}   the returned number of the new
 *                                    originated LSAs
 *
 * @returns             E_OK     - success
 * @returns             not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetOriginateNewLsas(t_Handle ospfMibObj, ulng *count)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!count))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *count = rtrCfg.OriginateNewLsas;
   return E_OK;
}


/*********************************************************************
 * @purpose      Returns the number of link-state advertisements received
 *               determined to be new instantiations
 *
 *
 * @param  ospfMibObj  @b{(input)}   t_Handle of the OSPF MIB object
 * @param  count       @b{(input)}   the returned number of received LSAs
 *
 * @returns            E_OK     - success
 * @returns            not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetRxNewLsas(t_Handle ospfMibObj, ulng *count)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!count))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *count = rtrCfg.RxNewLsas;
   return E_OK;
}




/*********************************************************************
 * @purpose      Returns the value of ospf overflow state (TRUE or FALSE)
 *
 *
 * @param  ospfMibObj      @b{(input)}   t_Handle of the OSPF MIB object
 * @param  oflowState      @b{(input)}   the returned overflow state
 *
 * @returns               E_OK     - success
 * @returns               not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetOverflowState (t_Handle ospfMibObj, e_TruthValue *oflowState)
{
   t_OspfMibObj *p_ospfMib = (t_OspfMibObj *)ospfMibObj;
   t_RTO        *p_rtr;

   if ((!ospfMibObj) || (!oflowState))
      return E_BADPARM;

   p_rtr = (t_RTO *)(p_ospfMib->ospfRouterObj);
   *oflowState = ((p_rtr->IsOverflowed == TRUE) ? _true: _false);
   return E_OK;
}

 /*********************************************************************
 * @purpose    Sets value of ospfExtLsdbLimit.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  limit        @b{(input)}  the new value of the ospfExtLsdbLimit
 *
 * @returns             E_OK     - success
 * @returns             not E_OK - failure
 *
 * @notes
 *            Sets value of ospfExtLsdbLimit.
 *            "ospfExtLsdbLimit" is the maximum number of non_default
 *            AS-external-LSAs entries that can be stored in the link_state
 *            database. If the value is -1, then there is no limit.
 *
 *            When the number of non_default AS-external-LSAs in the
 *            router's link_state database reaches ospfExtLsdbLimit, the
 *            router enters OverflowState.
 *
 *            The router never holds more than ospfExtLsdbLimit non-default
 *            AS-external-LSAs in its database.
 *
 *            OspfExtLsdbLimit must be set identically in all routers attached
 *            to the OSPF backbone and/or any regular OSPF area (i.e. OSPF stub
 *            area and NSSAs are excluded).
 *
 * @end
 * ********************************************************************/
e_Err ospfSetExtLsdbLimit(t_Handle ospfMibObj, ulng limit)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.ExtLsdbLimit = limit;
   rtrCfg.RouterStatus = ROW_CHANGE;
   return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
}




/*********************************************************************
 * @purpose       Returns the maximum number of non_default AS-external-
 *                LSAs entries that can be stored in the link_state
 *                database.
 *
 *
 * @param  ospfMibObj  @b{(input)}   t_Handle of the OSPF MIB object
 * @param  limit       @b{(input)}   the returned limit
 *
 * @returns            E_OK     - success
 * @returns            not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetExtLsdbLimit(t_Handle ospfMibObj, ulng *limit)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!limit))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *limit = rtrCfg.ExtLsdbLimit;
   return E_OK;
}




/*********************************************************************
 * @purpose                        Sets value of ospfExitOverflowInterval.
 *
 *
 * @param  ospfMibObj              @b{(input)}  t_Handle of the OSPF
 *                                              MIB object
 * @param  exitOverflowInterval    @b{(input)}  interval value to set
 *
 * @returns                        E_OK     - success
 * @returns                        not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfSetExitOverflowInterval(t_Handle ospfMibObj,
                                  ulng exitOverflowInterval)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.ExitOverflowInterval  = exitOverflowInterval;
   rtrCfg.RouterStatus = ROW_CHANGE;

   return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
}




/*********************************************************************
 * @purpose               Returns the ospfExitOverflowInterval value.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param oflowInterval   @b{(input)}  the returned value of the ospfExitOver
 *                                     flowInterval
 *
 * @returns               E_OK     - success
 * @returns               not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetExitOverflowInterval(t_Handle ospfMibObj, ulng *oflowInterval)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!oflowInterval))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *oflowInterval = rtrCfg.ExitOverflowInterval;
   return E_OK;
}



/*********************************************************************
 * @purpose                 Sets value of ospfMulticastExtensions
 *
 *
 * @param  ospfMibObj       @b{(input)}   t_Handle of the OSPF MIB object
 * @param  multicastExt     @b{(input)}   multicast flag bitmap
 *
 * @returns                 TRUE  - new multicast extension bits/flags
 *                                  have been set successfully
 * @returns                 FALSE - incorrect value were requested to
 *                                  set, value of multicast extension
 *                                  bits/flags have't been reset.
 * @returns
 *
 * @notes
 *            Sets value of ospfMulticastExtensions.
 *
 *      "ospfMulticastExtensions" is a bitmask indicating whether the
 *      router is forwarding IP multicast datagrams based on the
 *      algorithms defined in the Multicast Extensions to OSPF.
 *
 *      Bit 0, if set, indicates that the router can forward IP multicast
 *      datagrams in the router's directly attached areas (called
 *      intra-area multicast routing).
 *      Bit 1, if set, indicates that the router can forward IP multicast
 *      datagrams between OSPF areas (called inter-area multicast routing).
 *      Bit 2, if set, indicates that the router can forward IP multicast
 *      datagrams between Autonomous Systems (called inter-AS multicast
 *      routing).
 *      Only certain combinations of bit  settings  are allowed,  namely:
 *       0  - no multicast forwarding is enabled,
 *       1  - intra-area multicasting  only, 
 *       3  - intra-area  and  inter-area  multicasting, 
 *       5  - intra-area and inter-AS  multicasting
 *       7  - multicasting  everywhere.
 *
 *      By default, no multicast forwarding is enabled.
 *
 * @end
 * ********************************************************************/
Bool ospfSetMulticastExtensions(t_Handle ospfMibObj, ulng multicastExt)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return FALSE;

   /* Check if the required multicast extension bitmask is correct */
   switch (multicastExt)
   {
      case 0:
      case 1:
      case 3:
      case 5:
      case 7:
         rtrCfg.RouterStatus = ROW_READ;
         if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
            return E_FAILED;

         rtrCfg.MulticastExtensions = multicastExt;
         rtrCfg.RouterStatus        = ROW_CHANGE;
         return RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
   }

   /* If we get here, the required mulicast extension bitmask  */
   /* is incorrect. Do not update ospfMulticastExt variable.   */
   return FALSE;
}


/*********************************************************************
 * @purpose                Returns the ospfMulticastExtensions value
 *
 *
 * @param  ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  multicastExt    @b{(input)}   multicast flag bitmap
 *
 * @returns                 E_OK     - success
 * @returns                 not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
 e_Err ospfGetMulticastExtensions (t_Handle ospfMibObj, ulng *multicastExt)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!multicastExt))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *multicastExt = rtrCfg.MulticastExtensions;
   return E_OK;
}


/*********************************************************************
 * @purpose              Sets value of ospfDemandExtensions.
 *
 *
 * @param  ospfMibObj    @b{(input)}   t_Handle of the OSPF MIB object
 * @param  demandExt     @b{(input)}   value of tag_TruthValue type
 *                                     (_true(1), _false(2))
 *
 * @returns              E_OK     - success
 * @returns              not E_OK - failure
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfSetDemandExtensions(t_Handle ospfMibObj, e_TruthValue demandExt)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if(!ospfMibObj)
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   rtrCfg.DemandExtensions    = ((demandExt == enabled) ? TRUE : FALSE);
   rtrCfg.RouterStatus = ROW_CHANGE;

   RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg);
   return E_OK;
}



/*********************************************************************
 * @purpose             Returns value of ospfDemandExtensions.
 *
 *
 * @param  ospfMibObj  @b{(input)}   t_Handle of the OSPF MIB object
 *                                   demandExt
 * @param  _true       @b{(input)}   if the router supports demand routing
 * @param  _false      @b{(input)}   if the router does not support demand
 *                                   routing
 *
 * @returns            E_OK     - success
 * @returns            not E_OK - failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfGetDemandExtensions(t_Handle ospfMibObj, e_TruthValue *demandExt)
{
   t_S_RouterCfg  rtrCfg;
   t_OspfMibObj  *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!ospfMibObj) || (!demandExt))
      return E_BADPARM;

   rtrCfg.RouterStatus = ROW_READ;
   if (RTO_Config(p_ospfMib->ospfRouterObj, &rtrCfg) != E_OK)
      return E_FAILED;

   *demandExt = ((rtrCfg.DemandExtensions == TRUE) ? enabled : disabled);
   return E_OK;
}
/* ------- End of SET/GET procedures for the OSPF global variables -----*/



/*********************************************************************
 * @purpose             Create a new row in the ospf Area Table
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  The area identifier - IP address
 *
 * @returns             E_OK       - success
 * @returns             E_IN_MATCH - entry already exists
 * @returns             E_BADPARM  - invalid parameter
 * @returns             E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_New(t_Handle ospfMibObj, ulng areaId)
{
   ospfAreaEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   if(_ospfAreaTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfAreaTable_, &areaId))
         return E_IN_MATCH;
   }
   else
   {
      _ospfAreaTable_ = OLL_New(0,                             /* do not use DA */                        
                                1,                             /* number of keys */
                                (ulng)offsetof(ospfAreaEntry, ospfAreaId), /* key offset */
                                (ulng)sizeof(ulng));                 /* key length */
   }


   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfAreaEntry));
   if( !p )
      return E_NOMEMORY;

   memset( p, 0, sizeof(ospfAreaEntry));
   p->ospfAreaId = areaId;

   /* fill in "configuration fields" by default          */
   /* "Statistics" fields are not kept in MIB data base. */
   p->info.importASExt   = AREA_IMPORT_EXTERNAL;    /* 1 */
   p->info.areaSummary   = AREA_NO_SUMMARY;         /* 1 */
   p->info.rowStatus     = notReady;
   p->owner              = NULLP;
   p->info.internalCreated = FALSE;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfAreaTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfAreaTable_New procedure */


/*********************************************************************
 * @purpose      Create a new row in the ospf Area Table and activate it
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  The area identifier - IP address
 * @param  areaObj      @b{(input)}  Pointer to the area object
 *
 * @returns             E_OK       - success
 * @returns             E_IN_MATCH - entry already exists
 * @returns             E_BADPARM  - invalid parameter
 * @returns             E_NOMEMORY - can't allocate memory
 *
 * @notes
 *          Create a new row in the ospf Area Table and activate it.
 *          The procedure is called from OSPF protocol code after
 *          a new area object is created.
 *          The new row parameters are set according to the area object
 *          parameters.
 *          If the row already exists (i.e. it is SNMP created area),
 *          the area object is bound to "owner" field and reconfigured
 *          according to the row fields. RowStatus is not changed.
 *
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_InternalNewActive(t_Handle ospfMibObj, ulng areaId,
                                      t_Handle p_areaObj)
{
   ospfAreaEntry          *p = NULLP;
   t_S_AreaCfg             areaCfg;
   Bool                    rowExists = FALSE;
   ospfStubAreaEntry      *p_StubAreaEntry;
   t_S_StubAreaEntry       stubAreaCfg;

   ospfAreaAggrEntry      *p_AreaAggrEntry;
   t_S_AreaAggregateEntry  areaAggrCfg;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if (!p_areaObj)
      return E_FAILED;

   /* Get the area config parameters from the object */
   areaCfg.AreaStatus  = ROW_READ;
   ARO_Config(p_areaObj, &areaCfg);

   if (_ospfAreaTable_ == 0)
   {
      /* The table is empty, so the row doesn't exist. */
      /* Initialize the table. */
      _ospfAreaTable_ = OLL_New(0,                      /* do not use DA */                        
                         1,                             /* number of keys */
                         (ulng)offsetof(ospfAreaEntry, ospfAreaId), /* key offset */
                         (ulng)sizeof(ulng));                 /* key length */
   }
   else
   {
      /* check if the table entry already exists */
      if((p = OLL_Find(_ospfAreaTable_, &areaId)))
         rowExists = TRUE;
   }
 
   /* If the row doesn't exist, create it and insert to the */
   /* ospfAreaTable ordered linked list                   */
   if (!rowExists)
   {
      /* Allocate memory for the new entry of the table */
      p = XX_Malloc(sizeof(ospfAreaEntry));
      if( !p )
         return E_NOMEMORY;

      memset( p, 0, sizeof(ospfAreaEntry));
      p->ospfAreaId = areaId;

      /* fill the new row fields according to the object parameters */
      p->info.importASExt   = areaCfg.ExternalRoutingCapability;
      p->info.areaSummary   = areaCfg.AreaSummary;
      p->info.rowStatus     = active;
      p->owner              = NULLP;
      p->info.internalCreated = TRUE;

      /* Add a new entry to the ordered linked list */
      if( OLL_Insert(_ospfAreaTable_, (void *)p ) != E_OK )
      {
         XX_Free(p);
         return E_FAILED;
      }

   }  /* End of creating the new row  */

   /* Set the owner*/
   p->owner = p_areaObj;
   
   /* If the row was created before the object, we need to compare */
   /* the config parameters in the row and in the object.          */
   /* If differences are discovered, set the object parameters     */
   /* according the row.                                           */
   if (rowExists)
   {
      if ((p->info.importASExt != areaCfg.ExternalRoutingCapability) ||
           (p->info.areaSummary != areaCfg.AreaSummary))
      {
         areaCfg.ExternalRoutingCapability = p->info.importASExt;
         areaCfg.AreaSummary               = p->info.areaSummary;
         areaCfg.AreaStatus                = ROW_CHANGE;
         ARO_Config(p_areaObj, &areaCfg);
      }
   }

   /* Find the default metric for this area in ospfStubArea table.    */
   /* If the default metric exists and active, bind it to the area    */
   /* object. (Only TOS 0 is supported by our OSPF implementation, so */
   /* the appropriate entry in ospfStubAreaTable can be found by one  */
   /* lookup operation.                                               */
   if ((p_StubAreaEntry = ospfStubAreaTable_Lookup(ospfMibObj, areaId, (ulng)0)))
   {
         stubAreaCfg.StubTOS         = 0;
         stubAreaCfg.StubMetric      = p_StubAreaEntry->info.metric;
         stubAreaCfg.StubMetricType  = p_StubAreaEntry->info.metricType;
         stubAreaCfg.StubStatus      = ROW_CREATE_AND_GO;

         /* Call ARO_StubMetric procedure to bind the stub area parameters */
         ARO_StubMetric (p_areaObj, &stubAreaCfg);
   }

   /* Find the area aggregate parameters in ospfAreaAggr table. */
   /* If area aggr rows exist and active, bind the aggr         */
   /* parameters to the area object.                            */
   p_AreaAggrEntry = OLL_FindNext(_ospfAreaAggrTable_, &areaId, (ulng *)-1);
   while(p_AreaAggrEntry && 
         (p_AreaAggrEntry->ospfAreaAggrAreaId == areaId))
   {
      /* Fill area aggregate config. structure */
      areaAggrCfg.AreaId          = areaId;
      areaAggrCfg.LsdbType        = (word)(p_AreaAggrEntry->ospfAreaAggrLsdbType);
      areaAggrCfg.NetIpAdr        = p_AreaAggrEntry->ospfAreaAggrNet;
      areaAggrCfg.SubnetMask      = p_AreaAggrEntry->ospfAreaAggrMask;
      areaAggrCfg.AggregateStatus = ROW_CREATE_AND_GO;
      areaAggrCfg.AggregateEffect = p_AreaAggrEntry->info.effect;

      /* Set area aggr parameter into the area object */
      ARO_AddressRange (p_areaObj, &areaAggrCfg);

      p_AreaAggrEntry = OLL_GetNext(_ospfAreaAggrTable_, p_AreaAggrEntry);
   }

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfAreaTable_InternalNewActive procedure */





/*********************************************************************
 * @purpose             Delete a row from the OSPF Area MIB Table
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  The area identifier - IP address
 *
 * @returns             E_OK     - Success
 * @returns             Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_Delete(t_Handle ospfMibObj, ulng areaId)
{
   ospfAreaEntry *p = NULLP;
   ospfIfEntry   *p_ifEntry = NULLP;
   Bool           endOfIfTable = FALSE;
   Bool           ifFound      = FALSE;

   if (!ospfMibObj)
      return E_BADPARM;

   if (!(p = OLL_Find(_ospfAreaTable_, &areaId)))
      return E_OK;

   /* Check if any active interface, connecting to the area, exists */
   while (!endOfIfTable && !ifFound )
   {
      p_ifEntry = OLL_GetNext (_ospfIfTable_, p_ifEntry);

      if (!p_ifEntry)
         endOfIfTable = TRUE;
      else
          if ((p_ifEntry->info.areaId == p->ospfAreaId) &&
              (p_ifEntry->info.rowStatus == active))
          ifFound = TRUE;
   }
   
   /* If an active interface of the area has been found, the area row */
   /* cannot be deleted */
   if (ifFound)
      return E_FAILED;

   /*If we get here, the area can be deleted without problem */
   L7MIB_EXTRACTDELETE(p, _ospfAreaTable_);
   return E_OK;

} /* End of ospfAreaTable_Delete procedure */



/*********************************************************************
 * @purpose      Delete a row from the OSPF Area MIB Table, and delete all
 *               hosts, area ranges and stub area metrics associalted with
 *               this area.
 *
 *
 * @param  ospfMibObj   @b{(input)}   t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}   The area identifier - IP address
 *
 * @returns             E_OK     - Success
 * @returns             Not E_OK - Failure
 *
 * @notes         This routine is to be called from OSPF protocol code only.
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_DeleteInternal(t_Handle ospfMibObj, ulng areaId)
{
   ospfAreaEntry     *p            = NULLP;
   ospfHostEntry     *p_Host       = NULLP;
   ospfStubAreaEntry *p_Stub       = NULLP;
   ospfAreaAggrEntry *p_AreaAggr   = NULLP;
   ospfAreaAggrEntry *p_AggrNext   = NULLP;
   ulng               stubTOS      = 0;

   if (!ospfMibObj)
      return E_BADPARM;

   if (!(p = OLL_Find(_ospfAreaTable_, &areaId)))
      return E_OK;

   /* If the area object was created by the SNMP manager, */
   /* indicate that ARO object has been deleted.          */
   if (p->info.internalCreated == FALSE)
   {
      p->owner = NULLP;
      return E_OK;
   }
   
   /* Find all hosts associated with this area, and delete them. */
   p_Host = (ospfHostEntry *)OLL_GetNext(_ospfHostTable_, p_Host);
   while (p_Host)
   {
      if (p_Host->info.areaId == areaId)
      {
         /* Remove the entry from the ospfHostTable */
         L7MIB_EXTRACTDELETE(p_Host, _ospfHostTable_);
         p_Host = NULL;
      }
      p_Host = (ospfHostEntry *)OLL_GetNext(_ospfHostTable_, p_Host);
   }

   /* Find the stub area metric associated with this area, and delete it. */
   p_Stub = (ospfStubAreaEntry *)OLL_Find(_ospfStubAreaTable_, &areaId, &stubTOS);
   if (p_Stub)
   {
      /* Remove the entry from the ospfStubAreaTable */
      L7MIB_EXTRACTDELETE(p_Stub, _ospfStubAreaTable_);
   }

   /* Find all area aggregates associated with this area, and delete them. */
   /* Find match with area ID */
   p_AreaAggr = OLL_FindNext(_ospfAreaAggrTable_, &areaId, (void *)-1);
   while (p_AreaAggr && (p_AreaAggr->ospfAreaAggrAreaId == areaId))
   {
      p_AggrNext = OLL_GetNext(_ospfAreaAggrTable_, p_AreaAggr);

      L7MIB_EXTRACTDELETE(p_AreaAggr, _ospfAreaAggrTable_);
      p_AreaAggr = p_AggrNext;
   }

   /* If we get here, the area can be deleted without problem */
   L7MIB_EXTRACTDELETE(p, _ospfAreaTable_);
   return E_OK;

} /* End of ospfAreaTable_DeleteInternal procedure */




/*********************************************************************
 * @purpose       Set OSPF area object handle into the entry of the
 *                ospfAreaTable.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  The area identifier
 *                                   (index in ospfAreaTable)
 * @param  areObjId     @b{(input)}  OSPF area handle
 *
 * @returns             E_OK     - Success
 * @returns             Not E_OK - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_SetOwner (t_Handle ospfMibObj, ulng areaId,
                              t_Handle areaObj)
{
   ospfAreaEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &areaId);
   if(!p)
      return E_NOT_FOUND;
   p->owner = areaObj;
   return E_OK;
}




/*********************************************************************
 * @purpose      Get OSPF area object handle from the entry in the ospfAreaTable
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  The OSPF area identifier (IP address)
 *
 * @returns            OSPF area handle - Success
 * @returns            NULLP            - Otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaTable_GetOwner(t_Handle ospfMibObj, ulng areaId)
{
   ospfAreaEntry *p;

   if (!ospfMibObj)
      return NULLP;

   p = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &areaId);
   return p ? p->owner : NULLP;
}




/*********************************************************************
 * @purpose             Get an entry in the ospfAreaTable.
 *
 *
 * @param  ospfMibObj   @b{(input)}   t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}   the OSPF area identifier (IP address)
 *
 * @returns             pointer to the appropriate entry of ospfAreaTable
 *                      on success
 * @returns             NULLP - otherwise
 * 
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaTable_Lookup(t_Handle ospfMibObj, ulng areaId)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfAreaTable_, &areaId);

   return p;
}




/*********************************************************************
 * @purpose      Test the parameters in a row of the ospfAreaTable before
 *               setting them by SNMP agent.
 *
 *
 * @param  lastOid    @b{(input)}   parameter offset in the row
 * @param  newValue   @b{(input)}   value to set
 * @param  p          @b{(input)}   pointer to ospfAreaEntry
 *
 * @returns           E_OK     - success, value can be set
 * @returns           E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfAreaEntry *p_areaEntry = (ospfAreaEntry *)p;  

   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfAreaTestTable, 
                            sizeof(ospfAreaTestTable)/sizeof(ospfAreaTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   /* Since OSPF Area configuration parameters are "CREATE_READ" objects, */
   /* they can be changed (set) only during conceptual row creation.      */
   /* Rowstatus are "READ_WRITE" objects, so they can be set at any time. */

   if ( lastOid == ospfAreaTableRowStatusSubId)  
      return E_OK;
   else
      if((p_areaEntry->info.rowStatus != notReady) &&
         (p_areaEntry->info.rowStatus != notInService))
          return E_FAILED;
      return E_OK;   
}




/*********************************************************************
 * @purpose               Get the next entry in the ospfAreaTable.
 *
 *
 * @param   ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param   prevAreaId    @b{(input)}  the OSPF area identifier
 *                                       (IP address)
 *
 * @returns              pointer to the next (after prevAreaId) entry in the
 *                       ospfAreaTable and the next area id on success
 *                       NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/


/*----------------------------------------------------------------
 * ROUTINE:    ospfAreaTable_GetNext
 * DESCRIPTION:
 *    Get the next entry in the ospfAreaTable.
 * ARGUMENTS:
 *    ospfMibObj - t_Handle of the OSPF MIB object
 *    prevAreaId - the OSPF area identifier (IP address)
 * RESULTS:
 *    pointer to the next (after prevAreaId) entry in the 
 *       ospfAreaTable and the next area id on success
 *    NULL otherwise
 *----------------------------------------------------------------*/
t_Handle ospfAreaTable_GetNext(t_Handle ospfMibObj, ulng prevAreaId,
                               ulng *p_AreaId)
{
   ospfAreaEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfAreaTable_ == 0)
      return 0;

   next = (ospfAreaEntry *)OLL_FindNext(_ospfAreaTable_, &prevAreaId);
   if(next)
   {
      *p_AreaId =  next->ospfAreaId;
      return (t_Handle)next;
   }

   return NULLP;
}




/*********************************************************************
 * @purpose                 Get the first entry in the ospfAreaTable.
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 *
 * @returns  pointer to the first entry in the ospfAreaTable and the first
 *           areaId on success NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaTable_GetFirst(t_Handle ospfMibObj, ulng *p_AreaId)
{
   ospfAreaEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfAreaTable_ == 0)
      return NULLP;

   first = OLL_GetNext(_ospfAreaTable_, NULLP );

   if(first)
   {
      *p_AreaId = first->ospfAreaId;
      return (t_Handle)first;
   }

   return NULLP;
}




/*********************************************************************
 * @purpose           Set new value for row status of the ospfAreaTable
 *                    and check consistency of this value.
 *
 *
 * @param ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param areaId        @b{(input)}  The area identifier (index of the
 *                                                              table)
 * @param value         @b{(input)}  Value to be set into row status
 *
 * @returns             E_OK      - success, value can be set
 * @returns             E_BADPARM - bad parameter(s)
 * @returns             E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_SetRowStatus(t_Handle ospfMibObj, ulng areaId, ulng value)
{
   ospfAreaEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &areaId);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row.                                        */

   if(!p)
   {
      switch(value)
      {
         case createAndWait:
            /* Create new row */
            return ospfAreaTable_New(ospfMibObj, areaId);
            break;
         case createAndGo:
            /* Create new row. If creation is succsessfull activate the row */
            if(ospfAreaTable_New(ospfMibObj, areaId) == E_OK)
            {
               p = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &areaId);
               if (p)
               {
                  p->info.rowStatus = active;
                  return E_OK;
               }
               else
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the  OLL     */
                  return E_FAILED;
               }
            }  /* End of "if ospfAreaTable_New" */
            else
               return E_FAILED;

            break;

         default:       /* Any other value of rowStatus is impossible */
                        /* for inexisting row. */
            return E_FAILED;
            
      }     /*End of "switch (value)" command */
   }     /* End of "if (!p)" */
      
   /* An entry has been found. Check the new row status against the existing */
   /* one and perform appropriate actions. */
   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
            p->info.rowStatus = active;

         return E_OK;

         break;

      case notInService:
      case notReady:
         p->info.rowStatus = value;

         return E_OK;

         break;

      case destroy:
         return ospfAreaTable_Delete(ospfMibObj, areaId);

   } /* End of switch command in case a status of an existing row is changed */

   return E_OK;

}  /* End of ospfAreaTable_SetRowStatus procedure */




/*********************************************************************
 * @purpose       Update the Area Table entry according to the are
 *                configuration.
 *
 *
 * @param ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param p_Cfg        @b{(input)}  a handle of the area parameters structure,
 *                                  including the 'command' parameter
 *
 * @returns            E_OK       - success
 * @returns            E_BADPARM  - invalid parameter
 *
 * @notes             This procedure is to be called only from
 *                    the OSPF code(not from the SNMP code).
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaTable_Update(t_Handle ospfMibObj, t_S_AreaCfg *p_Cfg)
{
   ospfAreaEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   switch(p_Cfg->AreaStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;                           
      case  ROW_CHANGE:
         /* find the area entry and update it */
         p = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &p_Cfg->AreaId);

         if (!p)
            return E_FAILED;

         p->info.importASExt = p_Cfg->ExternalRoutingCapability;
         p->info.areaSummary = p_Cfg->AreaSummary;
      break;                           
      case  ROW_READ:
         /* do nothing */
      break;                           
      case  ROW_DESTROY:
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
      default:
         return E_BADPARM;
   }

   return E_OK;
}



/*********************************************************************
 * @purpose       Create a new row in the ospf Area Default Metric Table
 *                (ospfStubAreaTable).
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param stubAreaId        @b{(input)}  The area identifier - IP address
 * @param stubTOS           @b{(input)}  Stub area type of service
 *
 * @returns                 E_OK       - success
 * @returns                 E_IN_MATCH - entry already exists
 * @returns                 E_BADPARM  - invalid parameter
 * @returns                 E_NOMEMORY - can't allocate memory
 *
 * @notes                   This procedure is to be used only
 *                          from the SNMP code.
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_New(t_Handle ospfMibObj, ulng stubAreaId,
                            ulng stubAreaTOS)
{
   ospfStubAreaEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   /* Only TOS 0 is supported in OSPF implementation */
   if (stubAreaTOS)
      return E_FAILED;

   /* Check if the area is defined in ospfAreaTable.             */
   /* This test is not needed: the default metric could be       */
   /* defined before the area for which it is intended is added. */
   /* if(!(p_areaEntry = OLL_Find(_ospfAreaTable_, &stubAreaId))) 
      return E_FAILED */

   if(_ospfStubAreaTable_ != 0)
   {
      /* Check if the table entry already exists */
      if(OLL_Find(_ospfStubAreaTable_, &stubAreaId, &stubAreaTOS ))
         return E_IN_MATCH;
   }
   else
   {
      _ospfStubAreaTable_ = OLL_New(0,                 /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfStubAreaEntry, ospfStubAreaId), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfStubAreaEntry, ospfStubAreaTOS), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfStubAreaEntry));
   if( !p )
      return E_NOMEMORY;

   memset( p, 0, sizeof(ospfStubAreaEntry));
   p->ospfStubAreaId  = stubAreaId;
   p->ospfStubAreaTOS = stubAreaTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfStubAreaTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* fill in default values where possible */
   p->info.metricType = STUB_OSPF_METRIC;
   p->info.metric     = 0;
   p->info.rowStatus  = notReady;

   p->info.internalCreated = FALSE;
   p->info.SNMPdisable     = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfStubAreaTable_New procedure */



/*********************************************************************
 * @purpose       Create a new row in the ospf Area Default Metric Table
 *                (ospfStubAreaTable).
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param stubAreaId     @b{(input)}  The area identifier - IP address
 * @param stubTOS        @b{(input)}  Stub area type of service
 *
 * @returns              E_OK       - success
 * @returns              E_IN_MATCH - entry already exists
 * @returns              E_BADPARM  - invalid parameter
 * @returns              E_NOMEMORY - can't allocate memory
 *
 * @notes                This procedure is to be used only
 *                       from the OSPF code (not from the SNMP code).
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_InternalNewAndActive(t_Handle ospfMibObj,
                                             ulng stubAreaId, ulng stubAreaTOS,
                                             ulng metric,
                                             e_StubMetricType metricType)
{
   ospfStubAreaEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency: only TOS 0 is */
   /* supported in OSPF implementation.   */
   if (stubAreaTOS)
      return E_FAILED;

   /* Check if the area is defined in ospfAreaTable. */
   if(OLL_Find(_ospfAreaTable_, &stubAreaId) == NULL)
      return E_FAILED;

   if(_ospfStubAreaTable_ != 0)
   {
      /* Check if the table entry already exists */
      if(OLL_Find(_ospfStubAreaTable_, &stubAreaId, &stubAreaTOS ))
         return E_OK;
   }
   else
   {
      _ospfStubAreaTable_ = OLL_New(0,                 /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfStubAreaEntry, ospfStubAreaId), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfStubAreaEntry, ospfStubAreaTOS), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfStubAreaEntry));
   if( !p )
      return E_NOMEMORY;

   memset( p, 0, sizeof(ospfStubAreaEntry));
   p->ospfStubAreaId  = stubAreaId;
   p->ospfStubAreaTOS = stubAreaTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfStubAreaTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* fill in values according to the ARO configuration */
   p->info.metricType = metricType;
   p->info.metric     = metric;
   p->info.rowStatus  = active;

   p->info.internalCreated = TRUE;
   p->info.SNMPdisable     = FALSE;
 
   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfStubAreaTable_InternalNewAndActive procedure */




/*********************************************************************
 * @purpose       Delete a row from the OSPF Stub Area MIB Table.
 *                This routine is to be called from SNMP only.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param stubAreaId     @b{(input)}  The area identifier - IP address
 * @param stubTOS        @b{(input)}  The stub area type of service
 *
 * @returns              E_OK     - Success
 * @returns              Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_Delete(t_Handle ospfMibObj, ulng stubAreaId,
                               ulng stubTOS)
{
   ospfStubAreaEntry *p;
   t_S_StubAreaEntry  p_StubCfg;
   ospfAreaEntry     *p_areaEntry;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfStubAreaEntry *)OLL_Find(_ospfStubAreaTable_, &stubAreaId, &stubTOS);
   if (!p)
      /* we cannot do anything, the entry does not exist */
      return E_OK;

   if (p->info.internalCreated == TRUE)
   {
      /* Mustn't remove internally created objects via SNMP. */
      return E_FAILED;
   }

   /* Verify that the area is defined in ospfAreaTable and */
   /* its owner is set. If yes, we need to remove the stub */
   /* area entry from the HL of the area object.           */
   if((p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &stubAreaId)))
      if (p_areaEntry->owner)
      {
         p_StubCfg.StubTOS    = stubTOS;
         p_StubCfg.StubStatus = ROW_DESTROY;
         p->info.SNMPdisable  = TRUE; /* prevent deletion by DeleteInternal */
         ARO_StubMetric(p_areaEntry->owner, &p_StubCfg);
      }

   /* The last thing is to remove the entry from the ospfStubAreaTable */
   L7MIB_EXTRACTDELETE(p, _ospfStubAreaTable_);
   return E_OK;

} /* End of ospfStubAreaTable_Delete procedure */




/*********************************************************************
 * @purpose                 Get an entry in the ospfStubAreaTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param stubAreaId        @b{(input)}  The OSPF Stub area identifier
 *                                       (IP address)
 * @param stubTOS           @b{(input)}  The stub area type of service
 *
 * @returns                 pointer to the appropriate entry of ospfStub
 *                          AreaTable on success
 * @returns                 NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfStubAreaTable_Lookup(t_Handle ospfMibObj, ulng stubAreaId,
                                  ulng stubTOS)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfStubAreaTable_, &stubAreaId, &stubTOS);

   return p;
}


/*********************************************************************
 * @purpose                 Get the next entry in the ospfStubAreaTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param prevStubAreaId    @b{(input)}  the stub OSPF area identifier
 *                                       (IP address)
 * @param prevStubAreaTOS   @b{(input)}  the stub area type of service
 * @param p_StubAreaId      @b{(input)}  the next stub area identifier
 * @param p_StubAreaTOS     @b{(input)}  the next stub areatype of service
 *
 * @returns       pointer to the next (after the previous) entry in the
 *                ospfStubAreaTable and the next area id on success
 * @returns       NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfStubAreaTable_GetNext(t_Handle ospfMibObj, ulng prevStubAreaId,
                                   ulng prevStubAreaTOS, ulng  *p_StubAreaId,
                                   ulng *p_StubAreaTOS)
{
   ospfStubAreaEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfStubAreaTable_ == 0)
      return NULLP;

   next = (ospfStubAreaEntry *)OLL_FindNext(_ospfStubAreaTable_, 
                                       &prevStubAreaId, &prevStubAreaTOS);
   if(next)
   {
      *p_StubAreaId =  next-> ospfStubAreaId;
      *p_StubAreaTOS = next-> ospfStubAreaTOS;
      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}


/*********************************************************************
 * @purpose                 Get the first entry in the ospfStubAreaTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param p_StubAreaId      @b{(input)}  the next stub area identifier
 * @param p_StubAreaTOS     @b{(input)}  the next stub areatype of
 *                                       service
 *
 * @returns        pointer to the first entry in the
 *                 ospfStubAreaTable and the indexes of the first entry
 *                 on success
 * @returns        NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfStubAreaTable_GetFirst(t_Handle ospfMibObj, ulng  *p_StubAreaId,
                                    ulng *p_StubAreaTOS)
{
   ospfStubAreaEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfStubAreaTable_ == 0)
      return NULLP;

   first = (ospfStubAreaEntry *)OLL_GetNext(_ospfStubAreaTable_, NULLP);
   if(first)
   {
      *p_StubAreaId =  first-> ospfStubAreaId;
      *p_StubAreaTOS = first-> ospfStubAreaTOS;
      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}



/*********************************************************************
 * @purpose       Test the parameters in a row of the ospfStubAreaTable
 *                before setting them by SNMP agent.
 *
 *
 * @param lastOid    @b{(input)}  parameter offset in the row
 * @param newValue   @b{(input)}  value to set
 * @param p          @b{(input)}  pointer to ospfAreaEntry
 *
 * @returns          E_OK      - success, value can be set
 * @returns          E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfStubAreaEntry *p_StubAreaEntry = (ospfStubAreaEntry *)p;

   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfStubAreaTestTable, 
                            sizeof(ospfStubAreaTestTable)/
                            sizeof(ospfStubAreaTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   if ( lastOid == ospfStubAreaTableRowStatusSubId)
   {
      if (p && (p_StubAreaEntry->info.internalCreated == TRUE))
         return E_FAILED;
   }

   /* All other parameters can be changed "on the fly". */
   return E_OK;
}



/*********************************************************************
 * @purpose       Set new value for row status of the ospfStubAreaTable
 *                and check consistency of this value.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  stubAreaId    @b{(input)}  The area identifier (index of
 *                                                             the table)
 * @param  stubTOS       @b{(input)}  The stub area type of service
 * @param  value         @b{(input)}  Value to be set into row status
 *
 * @returns              E_OK      - success, value can be set
 * @returns              E_BADPARM - bad parameter(s)
 * @returns              E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_SetRowStatus(t_Handle ospfMibObj, ulng stubAreaId,
                                     ulng stubTOS, ulng value)
{
   ospfStubAreaEntry *p;
   e_Err              rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfStubAreaEntry *)OLL_Find(_ospfStubAreaTable_, &stubAreaId, &stubTOS);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfStubAreaTable_New(ospfMibObj, stubAreaId, stubTOS);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfStubAreaTable_New(ospfMibObj, stubAreaId,  stubTOS)) != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfStubAreaEntry *)OLL_Find(_ospfStubAreaTable_, &stubAreaId, 
                                                                    &stubTOS);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfStubAreaTable OLL. Delete the created row.    */
                  ospfStubAreaTable_Delete(ospfMibObj, stubAreaId, stubAreaId );
                  return E_FAILED;
               }
               else 
               {
                  /* Everything is OK. Try to activate the row */
                  if (ospfStubAreaTable_Active(ospfMibObj, p)!= E_OK)
                  {
                     ospfStubAreaTable_Delete(ospfMibObj, stubAreaId, stubAreaId);
                     return E_FAILED;
                  }
                  else
                     return E_OK;   
               }

            } /* End of if rc = ospfStubAreaTable_New != E_OK */  
            
            break;

         default:              /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */ 


   /* An existing row  status changing. Check the requested row status */
   /* against the existing one and perform appropriate actions.        */
   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
         {
            return (ospfStubAreaTable_Active(ospfMibObj, p));
         }
         break;

      case notInService:
      case notReady:
         if(p->info.rowStatus == active)
         {
            /* Verify if the row can be deactivated in order to */
            /* change any configuration parameter. ???          */
             ospfStubAreaTable_Deactive(ospfMibObj, p);
         }
         p->info.rowStatus = value;
         return E_OK;

         break;

      case destroy:
         return ospfStubAreaTable_Delete(ospfMibObj, stubAreaId, stubTOS);

         break;

      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfStubAreaTable_SetRowStatus procedure */




/*********************************************************************
 * @purpose       Set ospfStubAreaTable row to active status.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param p              @b{(input)}  pointer to an appropriate entry
 *                                   in the table
 *
 * @returns             E_OK      - success, value can be set
 * @returns             E_BADPARM - bad parameter(s)
 * @returns             E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_Active(t_Handle ospfMibObj, ospfStubAreaEntry *p)
{
   t_S_StubAreaEntry   stubAreaCfg;
   ospfAreaEntry      *p_areaEntry;

   if (!ospfMibObj)
      return E_BADPARM;

   if (!p)
      return E_FAILED;

   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Find the appropriate row in ospfAreaTable and the area owner.        */
   /* If area or its owner is not found, we cannot bind here the default   */ 
   /* metric (stub area parameters) to the area object. It will be done    */
   /* when the area object will be created (i.e. when the first interface  */
   /* will be defined on this area). Then ARO_StubMetric procedure will be */
   /* called from  ospfAreaTable_InternalNewActive. Here we do nothing.    */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &(p->ospfStubAreaId));
   if(!(p_areaEntry) || (p_areaEntry && !(p_areaEntry->owner)))
   {
      p->info.rowStatus = active;
      return E_OK;
   }

   /* Fill Stub area config parameters according to the row fields */
   stubAreaCfg.StubTOS         = p->ospfStubAreaTOS;
   stubAreaCfg.StubMetric      = p->info.metric;
   stubAreaCfg.StubMetricType  = p->info.metricType;
   stubAreaCfg.StubStatus      = ROW_CREATE_AND_GO;

   /* Call ARO_StubMetric procedure to bind the stub area   */
   /* parameters to the hash list of the appropriated area. */
   if (ARO_StubMetric (p_areaEntry->owner, &stubAreaCfg)!= E_OK)
      return E_FAILED;

   /* If we get here everything is OK */
   p->info.rowStatus = active;
   return E_OK;
   
}  /* End of ospfStubAreaTable_Active */


/*********************************************************************
 * @purpose       Set ospfStubAreaTable row to not active status, i.e.
 *                notReady or notInService.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param p               @b{(input)}  pointer to an appropriate entry
 *                                     in the table
 *
 * @returns               E_OK      - success, value can be set
 * @returns               E_BADPARM - bad parameter(s)
 * @returns               E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_Deactive(t_Handle ospfMibObj, ospfStubAreaEntry *p)
{
   t_S_StubAreaEntry  p_StubCfg;
   ospfAreaEntry     *p_areaEntry;

   if (!ospfMibObj)
      return E_BADPARM;

   if (!p)
      return E_FAILED;

   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Find the appropriate row in ospfAreaTable and get the owner.         */
   /* If the area or its owner is not found, we don't need to do anything. */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &(p->ospfStubAreaId));

   if(!(p_areaEntry) || (p_areaEntry && !(p_areaEntry->owner)))
      return E_OK;

   /* Call ARO_StubMetric procedure to remove the stub area parameters */
   /* from the hash list of the appropriated area.                     */
   p_StubCfg.StubTOS    = p->ospfStubAreaTOS;
   p_StubCfg.StubStatus = ROW_DESTROY;
   p->info.SNMPdisable  = TRUE; /* prevent deletion by DeleteInternal */
   ARO_StubMetric (p_areaEntry->owner, &p_StubCfg);
   p->info.SNMPdisable  = FALSE;

   return E_OK;
   
}  /* End of ospfStubAreaTable_Deactive procedure */




/*********************************************************************
 * @purpose       Update the Stub Area (Metric) Table according to the
 *               'command' parameter: either create new row, or update
 *               /destroy an existing one.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param stubAreaId        @b{(input)}  The area identifier - IP address
 * @param p_Metric          @b{(input)}  a handle of the metric parameters
 *                                      structure,including the 'command'
 *                                      parameter
 *
 * @returns                 E_OK       - success
 * @returns                 E_IN_MATCH - entry already exists
 * @returns                 E_BADPARM  - invalid parameter
 * @returns                 E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfStubAreaTable_Update(t_Handle ospfMibObj, ulng stubAreaId,
                               t_S_StubAreaEntry *p_Metric)
{
   ospfStubAreaEntry *p = NULL;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check if the table entry already exists */
   p = (ospfStubAreaEntry *)OLL_Find(_ospfStubAreaTable_, &stubAreaId,
                                     &p_Metric->StubTOS);

   switch (p_Metric->StubStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
         /* update the StubArea MIB table */
         if (!p)
            ospfStubAreaTable_InternalNewAndActive(ospfMibObj, stubAreaId,
                                                   p_Metric->StubTOS,
                                                   p_Metric->StubMetric,
                                                   p_Metric->StubMetricType);
      break;                           
      case  ROW_CHANGE:
         if (!p)
            return E_FAILED;

         /* p->ospfStubAreaTOS = p_Metric->StubTOS; */
         p->info.metric     = p_Metric->StubMetric;
         p->info.metricType = p_Metric->StubMetricType;
      break;                           
      case  ROW_READ:
         /* do nothing */
      break;                           
      case  ROW_DESTROY:
         if (p && (p->info.SNMPdisable == FALSE))
         {
            /* Remove the entry from the ospfStubAreaTable */
            L7MIB_EXTRACTDELETE(p, _ospfStubAreaTable_);
         }
      break;
      default:
         return E_BADPARM;
   }

   /* if we got here, everything is OK */
   return E_OK;
}

/*********************************************************************
 * @purpose       create a new row in the ospf Host Table.
 *                This routine is to be called from the SNMP code only.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  hostIpAddr    @b{(input)}  the host IP address
 * @param  hostTOS       @b{(input)}  the host type of service
 *
 * @returns              E_OK       - success
 * @returns              E_IN_MATCH - entry already exists
 * @returns              E_BADPARM  - invalid parameter
 * @returns              E_NOMEMORY - can't allocate memory
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_New(t_Handle ospfMibObj, ulng hostIpAddr, ulng hostTOS)
{
   ospfHostEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency (only TOS = 0 is supported*/
   if((!hostIpAddr ) || (hostTOS))
      return E_FAILED;

   if(_ospfHostTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfHostTable_, &hostIpAddr, &hostTOS ))
         return E_IN_MATCH;
   }
   else
   {
      _ospfHostTable_ = OLL_New(0,                     /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfHostEntry, ospfHostIpAddres), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfHostEntry, ospfHostTOS), /* key offset */
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfHostEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfHostEntry));

   /* Set the key values into the new entry */
   p->ospfHostIpAddres = hostIpAddr;
   p->ospfHostTOS      = hostTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfHostTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* fill in default values where possible */

   p->info.metric = (ulng)0;
   p->info.areaId = (ulng)0;

   p->info.rowStatus       = notReady;
   p->info.internalCreated = FALSE;
   p->info.SNMPdisable     = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfHostTable_New procedure */




/*********************************************************************
 * @purpose      Create a new row in the ospf Host Table. This routine is
 *               called only from the OSPF code (not from the SNMP).
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p_HostCfg      @b{(input)}  the host configuration structure
 *
 * @returns               E_OK       - success
 * @returns               E_BADPARM  - invalid parameter
 * @returns               E_BADPARM  - invalid parameter
 * @returns               E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_InternalNewAndActive(t_Handle ospfMibObj,
                                         t_S_HostRouteCfg *p_HostCfg)
{
   ospfHostEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency (only TOS = 0 is supported) */
   if((!p_HostCfg->HostIpAdr) || (p_HostCfg->HostTOS))
      return E_FAILED;

   if(_ospfHostTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfHostTable_, &(p_HostCfg->HostIpAdr),
                  &(p_HostCfg->HostTOS)))
         return E_OK;
   }
   else
   {
      _ospfHostTable_ = OLL_New(0,                     /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfHostEntry, ospfHostIpAddres), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfHostEntry, ospfHostTOS), /* key offset */
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfHostEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfHostEntry));

   /* Set the key values into the new entry */
   p->ospfHostIpAddres = p_HostCfg->HostIpAdr;
   p->ospfHostTOS      = p_HostCfg->HostTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfHostTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in values according to the ARO_HostConfig entry */
   p->info.metric    = p_HostCfg->HostMetric;
   p->info.areaId    = p_HostCfg->AreaId;

   p->info.rowStatus       = active;
   p->info.internalCreated = TRUE;
   p->info.SNMPdisable     = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfHostTable_InternalNewAndActive procedure */



/*********************************************************************
 * @purpose       Delete a row from the OSPF Host MIB Table. This routine
 *                is to be called from the SNMP code only.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  hostIpAddr    @b{(input)}  the host IP address
 * @param  hostTOS       @b{(input)}  the host type of service
 *
 * @returns              E_OK     - Success
 * @returns              Not E_OK - Failure
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_Delete(t_Handle ospfMibObj, ulng hostIpAddr, ulng hostTOS)
{
   ospfHostEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfHostEntry *)OLL_Find(_ospfHostTable_, &hostIpAddr, &hostTOS);
   if (p && (!p->info.internalCreated))
   {
      L7MIB_EXTRACTDELETE(p, _ospfHostTable_);
   }
   return E_OK;

} /* End of ospfHostTable_Delete procedure */



/*********************************************************************
 * @purpose              Get an entry in the ospfHostTable.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param hostIpAddr     @b{(input)}  the host IP address
 * @param hostTOS        @b{(input)}  the host type of service
 *
 * @returns       pointer to the appropriate entry of ospfHostTable on
 *                success
 * @returns       NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfHostTable_Lookup(t_Handle ospfMibObj, ulng hostIpAddr,
                              ulng hostTOS)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfHostTable_, &hostIpAddr, &hostTOS);

   return p;
}



/*********************************************************************
 * @purpose      Get the next entry in the ospfHostTable
 *
 *
 * @param  ospfMibObj       @b{(input)}   t_Handle of the OSPF MIB objec
 * @param  prevHostIpAddr   @b{(input)}   the IP address of the host
 * @param  prevHostTOS      @b{(input)}   the type of service of the route being
 *                                        configured
 *
 * @param  p_HostIpAddr     @b{(output)}  the IP address of the host
 * @param  p_HostTOS       @b{(output)}  the type of service of the route being configured
 *
 * @returns         pointer to the next (after the previous) entry in the
 *                  ospfHostTable and the next entry indexes on success
 *                  NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfHostTable_GetNext(t_Handle ospfMibObj, ulng prevHostIpAddr,
                               ulng prevHostTOS, ulng *p_HostIpAddr,
                               ulng *p_HostTOS)
{
   ospfHostEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfHostTable_ == 0)
      return NULLP;

   next = (ospfHostEntry *)OLL_FindNext(_ospfHostTable_, 
                                       &prevHostIpAddr, &prevHostTOS);
   if(next)
   {
      *p_HostIpAddr = next-> ospfHostIpAddres;
      *p_HostTOS    = next->ospfHostTOS;

      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}

/*********************************************************************
 * @purpose                 Get the first entry in the ospfHostTable.
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p_HostIpAddr     @b{(output)} the IP address of the host
 * @param  ospfMibObj       @b{(output)} the type of service of the route 
 *                                       being configured
 *
 * @returns                 pointer to the first entry in the
 *                          ospfHostTable and indexes of the first entry
 *                          NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfHostTable_GetFirst(t_Handle ospfMibObj, ulng *p_HostIpAddr,
                                ulng *p_HostTOS)
{
   ospfHostEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfHostTable_ == 0)
      return NULLP;

   first = (ospfHostEntry *)OLL_GetNext(_ospfHostTable_, NULLP);
   if(first)
   {
      *p_HostIpAddr = first->ospfHostIpAddres;
      *p_HostTOS    = first->ospfHostTOS;

      return (t_Handle)first;
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}



/*********************************************************************
 * @purpose      Test the parameters in a row of the ospfHostTable before
 *               setting them by SNMP agent.
 *
 *
 * @param  lastOid        @b{(input)}  parameter offset in the row
 * @param  newValue       @b{(input)}  value to set
 * @param  p              @b{(input)}  pointer to ospfAreaRangeEntry
 *
 * @returns               E_OK     - success, value can be set
 * @returns               E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfHostEntry *p_hostEntry = (ospfHostEntry *)p; 
   
   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfHostTestTable, 
                            sizeof(ospfHostTestTable)/
                            sizeof(ospfHostTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   /* OSPF Host table fields are "CREATE_READ" objects.             */
   /* Therefore they can be changed (set) only during conceptual row      */
   /* creation (i.e. before the row status becomes "active").             */
   /* Rowstatus are "READ_WRITE" objects, so they can be set at any time. */

   if (lastOid == ospfHostTableRowStatusSubId)
   {
      if (p)
         if (p_hostEntry->info.internalCreated == TRUE)
            return E_FAILED;
      return E_OK;
   }
   else
      if((p_hostEntry->info.rowStatus != notReady) &&
            (p_hostEntry->info.rowStatus != notInService))
         return E_FAILED;

   return E_OK;   
}



/*********************************************************************
 * @purpose      Set new value for row status of the ospfHostTable and
 *               check consistency of this value.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param hostIpAddr     @b{(input)}  the host IP address
 * @param hostTOS        @b{(input)}  the host type of service
 * @param value          @b{(input)}  Value to be set into row status
 *
 * @returns              E_OK      - success, value can be set
 * @returns              E_BADPARM - bad parameter(s)
 * @returns              E_FAILED  - inconsistent value
 *
 * @notes                This procedure is
 *                       designed to be called from SNMP agent code.
 *                       (row status can be changed only for SNMP created hosts.)
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_SetRowStatus(t_Handle ospfMibObj, ulng hostIpAddr,
                                 ulng hostTOS, ulng value)
{
   ospfHostEntry *p;
   e_Err          rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfHostEntry *)OLL_Find(_ospfHostTable_, &hostIpAddr, &hostTOS);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfHostTable_New(ospfMibObj, hostIpAddr, hostTOS);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfHostTable_New(ospfMibObj, hostIpAddr, hostTOS)) != E_OK)
               return rc;
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfHostEntry *)OLL_Find(_ospfHostTable_,
                                                   &hostIpAddr, &hostTOS);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfHostTable OLL. Delete the created row.    */
                  ospfHostTable_Delete(ospfMibObj, hostIpAddr, hostTOS);
                  return E_FAILED;
               }
               else
               {
                  /* New row has been successfully created. */
                  /* Activate the row */
                  if ((rc = ospfHostTable_Active(ospfMibObj, p)) != E_OK)
                  {
                     ospfHostTable_Delete(ospfMibObj, hostIpAddr, hostTOS);
                     return rc;
                  }
                  else
                  {
                     return E_OK;
                  }                     
               }

            } /* End of if rc = ospfHostTable_New != E_OK */  
            
            break;

         default:              /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */

     
   /* An existing row  status changing. Check the requested row status */
   /* against the existing one and perform appropriate actions.        */
   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
            return ospfHostTable_Active(ospfMibObj, p);

         break;

      case notReady:
      case notInService:
         if(p->info.rowStatus == active)
         {
            if ( ospfHostTable_Deactive(ospfMibObj, p)== E_OK)
            {
               p->info.rowStatus = value;
               return E_OK;
            }
            else 
               return E_FAILED;  /* SNMP agent has to return inconsistentValue */
         }
         else  /* The row is not active now */
            return E_FAILED;  /* SNMP agent has to return inconsistentValue */

         break;

      case destroy:
         if (p->info.rowStatus == active)
            ospfHostTable_Deactive(ospfMibObj, p);

         return ospfHostTable_Delete(ospfMibObj, hostIpAddr, hostTOS);

         break;

      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfHostTable_SetRowStatus procedure */



/*********************************************************************
 * @purpose      Activates a row of the ospfHostTable (i.e. creates a
 *               new HL entry in the appropriate ARO object).
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p            @b{(input)}  pointer to ospfHostEntry
 *
 * @returns             E_OK      - success, the row has been activate
 * @returns             E_BADPARM - bad parameter(s)
 * @returns             E_FAILED  - the row cannot be activate
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_Active(t_Handle ospfMibObj, ospfHostEntry *p)
{
   ospfAreaEntry        *p_areaEntry;
   t_S_HostRouteCfg      HostCfg;
   t_ospfMibIfLowParams  ifLowParams;
   t_OspfMibObj         *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Double-check */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Verify if the area is defined in ospfAreaTable and its owner is set.*/
   /* If no, the host row cannot be activated.                            */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &(p->info.areaId));

   if (!p_areaEntry || (p_areaEntry && !p_areaEntry->owner))
      /* host row can't be activated till the area is defined and activated */
      return E_FAILED;

   /* Fill host config structure */
   HostCfg.AreaId         = p->info.areaId;
   HostCfg.HostIpAdr      = p->ospfHostIpAddres;
   HostCfg.HostTOS        = p->ospfHostTOS;
   HostCfg.HostMetric     = (word)p->info.metric;
   HostCfg.IsVpnSupported = FALSE;
   HostCfg.HostStatus     = ROW_CREATE_AND_GO;
   HostCfg.PrevEntry      = NULL;
 
   /* Get the host route (the interface directly connected with  */
   /* the host), interface low layer handler and phys. type of   */ 
   /* the interface via user's callback procedures.             */
   memset(&ifLowParams, 0, sizeof(t_ospfMibIfLowParams));

   if (p_ospfMib->ospfMibCallbacks.f_Get_HostRoute &&
       p_ospfMib->ospfMibCallbacks.f_Get_IfParams)
   {
      if(p_ospfMib->ospfMibCallbacks.f_Get_HostRoute (p->ospfHostIpAddres, 
                                        &HostCfg.IfIndex) != E_OK)
          return E_FAILED;

       if(p_ospfMib->ospfMibCallbacks.f_Get_IfParams(HostCfg.IfIndex, 
                                        &ifLowParams) != E_OK)
          return E_FAILED;
         
       HostCfg.PhyType = ifLowParams.ifPhyType;
   }
   else
      return E_FAILED;

   /* Bind the new host to the area object */
   if (ARO_HostConfig(p_areaEntry->owner, &HostCfg) != E_OK)
      return E_FAILED;

   /* If we reach here, everything is OK */
   p->info.rowStatus = active;
   return E_OK;

}  /* End of ospfHostTable_Active procedure */



/*********************************************************************
 * @purpose      Deactivates a row of the ospfHostTable (i.e. deletes an
 *               appropriate HL entry from the ARO object).
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p                @b{(input)}  pointer to ospfHostEntry
 *
 * @returns                 E_OK      - success, the row has been activate
 * @returns                 E_BADPARM - bad parameter(s)
 * @returns                 E_FAILED  - the row cannot be activate
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_Deactive(t_Handle ospfMibObj, ospfHostEntry *p)
{
   ospfAreaEntry     *p_areaEntry;
   t_S_HostRouteCfg   p_HostCfg;

   if (!ospfMibObj)
      return E_BADPARM;

	/* Double-check */
	if (p->info.internalCreated == TRUE)
		return E_FAILED;

   /* Verify if the area is defined in ospfAreaTable and its owner is set.*/
   /* If no, no action is needed.                                         */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, &(p->info.areaId));

   if (!p_areaEntry || (p_areaEntry && !p_areaEntry->owner))
      return E_OK;

   /* If we get here, ARO object exists. Fill host config structure */
   p_HostCfg.HostIpAdr    = p->ospfHostIpAddres;
   p_HostCfg.HostStatus   = ROW_DESTROY;

   /* Config ARO object in order to delete the host (deleting is always OK). */
   p->info.SNMPdisable = TRUE; /* prevent deletion be DelteInternal */
   ARO_HostConfig(p_areaEntry->owner, &p_HostCfg); 
   p->info.SNMPdisable = FALSE;

   return E_OK;
}  /* End of ospfHostTable_Deactive procedure */



/*********************************************************************
 * @purpose      Update the Host Table according to the 'command' parameter.
 *               either create new row, or update/destroy an existing one.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param p_Hosts        @b{(input)}  a handle of the host parameters structure,
 *                                    including the 'command' parameter
 *
 * @returns       E_OK       - success
 * @returns       E_IN_MATCH - entry already exists
 * @returns       E_BADPARM  - invalid parameter
 * @returns       E_NOMEMORY - can't allocate memory
 *
 * @notes         This procedure is to be called only from the OSPF code
 *               (not from the SNMP code)
 *
 * @end
 * ********************************************************************/
e_Err ospfHostTable_Update(t_Handle ospfMibObj, t_S_HostRouteCfg *p_Hosts)
{
   ospfHostEntry *p = NULL;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfHostEntry *)OLL_Find(_ospfHostTable_, &p_Hosts->HostIpAdr,
                                 &p_Hosts->HostTOS);

   switch(p_Hosts->HostStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
         if (!p)
            ospfHostTable_InternalNewAndActive(ospfMibObj, p_Hosts);
      break;
      case  ROW_CHANGE:
         if (!p)
            return E_FAILED;

         p->info.metric = p_Hosts->HostMetric;
         p->info.areaId = p_Hosts->AreaId;
      break;
      case  ROW_READ:
      case  ROW_READ_NEXT:
         /* do nothing */
      break;
      case  ROW_DESTROY:
         if (p && (!p->info.SNMPdisable))
         {
            L7MIB_EXTRACTDELETE(p, _ospfHostTable_);
         }
      break;
      default:
         return E_BADPARM;
      break;
   }

   return E_OK;
}


/*********************************************************************
 * @purpose                Create a new row in the OSPF Interface MIB Table.
 *
 *
 * @param   ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr         @b{(input)}  the interface IP address
 * @param  ifAddrLessIf     @b{(input)}  ifIndex
 *
 * @returns                 E_OK       - success
 * @returns                 E_IN_MATCH - entry already exists
 * @returns                 E_BADPARM  - invalid parameter
 * @returns                 E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_New(t_Handle ospfMibObj, ulng ifIpAddr, ulng ifAddrLessIf)
{
   ospfIfEntry          *p            = NULLP;
   Bool                  stopSearch   = FALSE;
   t_ospfMibIfLowParams  ifLowParams;
   t_OspfMibObj         *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency: IP address 0 is possible in point- */
   /* to-point i/f, but our implementation doesn't allow ifIndex 0. */
   if (!ifAddrLessIf)
      return E_BADPARM;

   if(_ospfIfTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf))
         return E_IN_MATCH;

      /* Check if one of the interface indexes was already used */
      while ((p = OLL_GetNext(_ospfIfTable_, p)) && !stopSearch)
      {
         if((p->ospfIfIpAddres == ifIpAddr) &&
            (p->ospfAddrLessIf == ifAddrLessIf))
            return E_FAILED; 
         else
            if((p->ospfIfIpAddres > ifIpAddr) &&
               (p->ospfAddrLessIf > ifAddrLessIf))
               stopSearch = TRUE;
      }
   }
   else
   {
      if(_ospfIfTable_ == 0)
         _ospfIfTable_ = OLL_New(0,                    /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfIfEntry, ospfIfIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfIfEntry, ospfAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfIfEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfIfEntry));

   /* Set the key values into the new entry */
   p->ospfIfIpAddres = ifIpAddr;
   p->ospfAddrLessIf = ifAddrLessIf;

   /* Add a new entry to the ordered linked list */
   if(OLL_Insert(_ospfIfTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in configuration fields by the default values,
      as specified in RFC 1850. */
   p->info.areaId           = 0;
   p->info.adminStatus      = enabled;
   p->info.rtrPriority      = 1;
   p->info.transitDelay     = INF_TRANS_DELAY_DFLT;
   p->info.retransInterval  = RXMT_INTERVAL_DFLT;
   p->info.helloInterval    = HELLO_INTERVAL_DFLT;
   p->info.deadInterval     = ROUTER_DEAD_INTERVAL_DFLT;
   p->info.pollInterval     = POLL_INTERVAL_DFLT;
   memset(p->info.authKey, 0, 8);     /* nil auth key */
   p->info.rowStatus        = notReady;
   p->info.mtCastForwarding = IF_MLTCST_BLOCKED; /* 1 */
   p->info.demand           = _false; /* Demand OSPF procedure is not used */
   p->info.authType         = IFO_NULL_AUTH;

   /* Set ifType default value */
   p->info.ifType           =  IFO_BRDC;  /* only for init purpose */

   memset(&ifLowParams, 0, sizeof(t_ospfMibIfLowParams));
   if (p_ospfMib->ospfMibCallbacks.f_Get_IfParams)
   {
      p_ospfMib->ospfMibCallbacks.f_Get_IfParams (ifAddrLessIf, &ifLowParams);
      p->info.ifType = CONVERT_PHYTYPE_TO_IFTYPE[ifLowParams.ifPhyType];
   }

   p->info.internalCreated = FALSE;
   p->info.SNMPdisable     = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfIfTable_New procedure */



/*********************************************************************
 * @purpose                 Create a new row in the OSPF Interface MIB
 *                          Table and activate it.
 *
 *
 * @param  ospfMibObj       @b{(input)}    t_Handle of the OSPF MIB object
 * @param  ifIpAddr         @b{(input)}    the interface IP address
 * @param  ifAddrLessIf     @b{(input)}    ifIndex
 * @param  ifObj            @b{(input)}    t_Handle of the corresponding
 *                                         IFO object (the owner)
 *
 * @returns                 E_OK       - success
 * @returns                 E_IN_MATCH - entry already exists
 * @returns                 E_BADPARM  - invalid parameter
 * @returns                 E_NOMEMORY - can't allocate memory
 *
 * @notes
 *                 This procedure can be called from OSPF protocol code only.
     It is called from OSPF protocol code after a new interface object
     is created.
     The new row parameters are set according to the interface object
     parameters.

 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_InternalNewAndActive(t_Handle ospfMibObj, ulng ifIpAddr,
                                       ulng ifAddrLessIf, t_Handle ifObj)
{
   ospfIfEntry    *p            = NULLP;
   Bool            stopSearch   = FALSE;
   t_IFO_Cfg       ifCfg;

   /* Check parameters */
   if ((!ospfMibObj) || (!ifObj))
      return E_BADPARM;

   /* Check indexes consistency: IP address 0 is possible in point- */
   /* to-point i/f, but our implementation doesn't allow ifIndex 0. */
   if (!ifAddrLessIf)
      return E_BADPARM;

   if(_ospfIfTable_ != 0)
   {
      /* check if the table entry already exists */
      if ((p = OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf)))
      {
         /* do nothing */
         return E_IN_MATCH;
      }

      /* Check if one of the interface indexes was already used */
      while ((p = OLL_GetNext(_ospfIfTable_, p)) && !stopSearch)
      {
         if((p->ospfIfIpAddres == ifIpAddr) &&
            (p->ospfAddrLessIf == ifAddrLessIf))
            return E_FAILED; 
         else
            if((p->ospfIfIpAddres > ifIpAddr) &&
               (p->ospfAddrLessIf > ifAddrLessIf))
               stopSearch = TRUE;
      }
   }
   else
   {
      if(_ospfIfTable_ == 0)
         _ospfIfTable_ = OLL_New(0,                    /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfIfEntry, ospfIfIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfIfEntry, ospfAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfIfEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfIfEntry));

   /* Set the key values into the new entry */
   p->ospfIfIpAddres = ifIpAddr;
   p->ospfAddrLessIf = ifAddrLessIf;

   /* Add a new entry to the ordered linked list */
   if(OLL_Insert(_ospfIfTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in configuration fields according to the object        */
   /* parameters. Get the area config parameters from the object. */
   ifCfg.IfStatus = ROW_READ;
   if (IFO_Config(ifObj, &ifCfg) != E_OK)
      return E_FAILED;

   p->info.areaId           = ifCfg.AreaId;
   p->info.ifType           = ifCfg.Type;
   p->info.adminStatus      = ifCfg.AdminStat;
   p->info.rtrPriority      = ifCfg.RouterPriority;
   p->info.transitDelay     = ifCfg.InfTransDelay;
   p->info.retransInterval  = ifCfg.RxmtInterval;
   p->info.helloInterval    = ifCfg.HelloInterval;
   p->info.deadInterval     = ifCfg.RouterDeadInterval;
   p->info.pollInterval     = ifCfg.NbmaPollInterval;
   p->info.mtCastForwarding = ifCfg.MulticastForwarding;
   p->info.demand           = ifCfg.IfDemand;
   p->info.authType         = ifCfg.AuType;
   memset(p->info.authKey, 0, 8);

   p->info.rowStatus        = active;
   p->info.internalCreated  = TRUE;
   p->info.SNMPdisable      = FALSE;
   p->owner                 = ifObj;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfIfTable_InternalNewAndActive procedure */



/*********************************************************************
 * @purpose      Delete a row from the OSPF Interface MIB Table. Called
 *               when interface is deleted by SNMP request
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr       @b{(input)}  the interface IP address
 * @param  ifAddrLessIf   @b{(input)}  ifIndex
 *
 * @returns               E_OK     - Success
 * @returns               Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_Delete(t_Handle ospfMibObj, ulng ifIpAddr, ulng ifAddrLessIf)
{
   ospfIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   if (!p)
      return E_OK;

   /* Check whether the row was internally created (i.e. */
   /* by stack manager), or not (i.e. created via SNMP). */
   /* We can only delete what was created by SNMP.       */
   if (p->info.internalCreated == TRUE)
   {
      /* If the user of this library acted correctly and called SetTest, */
      /* we will never get here as it's illegal. But, just in case...    */
      return E_FAILED;
   }
   else
   {
      /* Check if this row is active. If yes, delete the interface object */
      if ((p->info.rowStatus == active) && p->owner)
      {
         p->info.SNMPdisable = TRUE; /* to prevent deletion by DeleteInternal */
         IFO_Delete (p->owner, (ulng)1);
      }

      L7MIB_EXTRACTDELETE(p, _ospfIfTable_);
   }

   return E_OK;

} /* End of ospfIfTable_Delete procedure */



/*********************************************************************
 * @purpose      Delete a row from the OSPF Interface MIB Table.
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr         @b{(input)}  the interface IP address
 * @param  ifAddrLessIf     @b{(input)}  ifIndex
 *
 * @returns                 E_BADPARM
 * @returns                 E_OK
 * @returns                 E_FAILED
 *
 * @notes       This routine is called only from the protocol code,
 *               not from the SNMP.
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_DeleteInternal(t_Handle ospfMibObj, ulng ifIpAddr,
                                 ulng ifAddrLessIf)
{
   ospfIfEntry       *p;
   ospfIfMetricEntry *p_Metric;
   ulng               metricTOS = 0;  /* we only support TOS 0 */

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   if (!p)
      return E_OK;

   if (p->info.SNMPdisable == TRUE)
   {
      /* IFO_Delete was called from ospfIfTable_Deactive. Don't do anything. */
      return E_OK;
   }

   if (p->info.internalCreated == FALSE)
   {
      /* this row was created via SNMP. Can't delete it internally. */
      return E_FAILED;
   }

   /* check if there's a metric for this i/f */
   p_Metric = (ospfIfMetricEntry *)OLL_Find(_ospfIfMetricTable_, &ifIpAddr,
                                               &ifAddrLessIf, &metricTOS);

   /* if metric exists and was created internally, delete it */
   if (p_Metric && (p_Metric->info.internalCreated == TRUE))
   {
      L7MIB_EXTRACTDELETE(p_Metric, _ospfIfMetricTable_);
   }

   L7MIB_EXTRACTDELETE(p, _ospfIfTable_);

   return E_OK;

} /* End of ospfIfTable_DeleteInternal procedure */




/*********************************************************************
 * @purpose              Get an entry in the ospfIfTable.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param ifIpAddr       @b{(input)}  the interface IP address
 * @param ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having no IP addr)
 *
 * @returns       pointer to the appropriate entry of ospfIfTable on success
 * @returns       NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfTable_Lookup(t_Handle ospfMibObj, ulng ifIpAddr,
                            ulng ifAddrLessIf)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   return p;
}



/*********************************************************************
 * @purpose                  Get the next entry in the ospfIfTable.
 *
 *
 * @param  ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param  prevIfIpAddr      @b{(input)}  the interface IP address
 * @param  prevIfAddrLessIf  @b{(input)}  ifIndex
 *
 * @returns           pointer to the next (after the previous) entry in the
 *                    ospfIfTable and indexes of the next element
 *                    NULL otherwise
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfTable_GetNext(t_Handle ospfMibObj, ulng prevIfIpAddr,
                             ulng prevIfAddrLessIf, ulng *p_IfIpAddr,
                             ulng *p_IfAddrLessIf)
{
   ospfIfEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfIfTable_ == 0)
      return NULLP;

   next = (ospfIfEntry *)OLL_FindNext(_ospfIfTable_, 
                                       &prevIfIpAddr, &prevIfAddrLessIf);
   if(next)
   {
      *p_IfIpAddr = next->ospfIfIpAddres;
      *p_IfAddrLessIf = next->ospfAddrLessIf;

      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}


/*********************************************************************
 * @purpose                 Get the first entry in the ospfIfTable
 *
 *
 * @param  ospfMibObj       @b{(input)}   t_Handle of the OSPF MIB object
 *
 * @param  p_IfIpAddr       @b{(output)}  the interface IP address
 * @param  p_IfAddrLessIf   @b{(output)}  ifIndex
 *
 * @returns        pointer to the first entry in the
 *                 ospfIfTable and indexes of the first element
 *                 NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfTable_GetFirst(t_Handle ospfMibObj, ulng *p_IfIpAddr,
                              ulng *p_IfAddrLessIf)
{
   ospfIfEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfIfTable_ == 0)
      return NULLP;

   first = (ospfIfEntry *)OLL_GetNext(_ospfIfTable_, NULLP);
   if(first)
   {
      *p_IfIpAddr     = first->ospfIfIpAddres;
      *p_IfAddrLessIf = first->ospfAddrLessIf;

      return (t_Handle)first;      
   }

   /* If we've reached here, first entry of the table was not found */
   return NULLP;
}



/*********************************************************************
 * @purpose       Test the parameters in a row of the ospfIfTable before
 *                setting them by SNMP agent.
 *
 *
 * @param  lastOid   @b{(input)}  parameter offset in the row
 * @param  newValue  @b{(input)}  value to set
 * @param  p         @b{(input)}  pointer to ospfIfEntry
 *
 * @returns          E_OK     - success, value can be set
 * @returns          E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfIfEntry  *p_ifEntry = (ospfIfEntry *)p;

   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfIfTestTable, 
                            sizeof(ospfIfTestTable)/
                            sizeof(ospfIfTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   /* OSPF If table fields are "READ CREATE" objects.                     */
   /* Therefore they can be changed (set) only during conceptual row      */
   /* creation (i.e. before the row status becomes "active").             */

   switch (lastOid)
   {
      case ospfIfTableRowStatusSubId:
         /* If the interface was internally created (by stack manager), */
         /* its row status mustn't be changed from outside (via SNMP).  */
         if (p && (p_ifEntry->info.internalCreated == TRUE))
            return E_FAILED;

      case ospfIfTableAdminStatusSubId:
         return E_OK;

      case ospfIfTableAreaId:
         /* If the interface was internally created (by stack manager), */
         /* its Area ID mustn't be changed from outside (via SNMP).     */
         /* For SNMP-created interfaces, AreaId field can be changed    */
         /* only when rowStatus is not active.                          */
         if (!p)
            return E_FAILED;
         if (p_ifEntry->info.internalCreated == TRUE)
            return E_FAILED;
         if ((p_ifEntry->info.rowStatus != notReady) &&
               (p_ifEntry->info.rowStatus != notInService))
            return E_FAILED;

      default:
         /* all other not read-only objects can be set when the admin      */
         /* status of the interface is disabled or rowStatus is not active */
         if (!p)
            return E_FAILED;
         if ((p_ifEntry->info.rowStatus == active) &&
               (p_ifEntry->info.adminStatus == enabled))
            return E_FAILED;
   }
   return E_OK;   
}




/*********************************************************************
 * @purpose      Set new value for row status of the ospfIfTable
 *               and check consistency of this value.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr       @b{(input)}  the interface IP address
 * @param  ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having no
 *                                                              IP addr)
 * @param  value          @b{(input)}  Value to be set into row status
 *
 * @returns               E_OK      - success, value can be set
 * @returns               E_BADPARM - bad parameter(s)
 * @returns               E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_SetRowStatus(t_Handle ospfMibObj, ulng ifIpAddr,
                               ulng ifAddrLessIf, ulng value)
{
   ospfIfEntry *p;
   e_Err         rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfIfTable_New(ospfMibObj, ifIpAddr, ifAddrLessIf);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfIfTable_New(ospfMibObj, ifIpAddr, ifAddrLessIf)) != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, 
                                                   &ifIpAddr, &ifAddrLessIf);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfIfTable OLL. Delete the created row.          */
                  ospfIfTable_Delete(ospfMibObj, ifIpAddr, ifAddrLessIf);
                  return E_FAILED;
               }
               else 
               {
                  /* New row has been successfully created. */ 
                  /* Activate the row */
                  if ((rc = ospfIfTable_Active(ospfMibObj, p)) != E_OK)
                  {
                     ospfIfTable_Delete(ospfMibObj, ifIpAddr, ifAddrLessIf);
                     return rc;
                  }
                  else
                  {
                     return E_OK;
                  }                     
               }

            } /* End of "if rc = ospfIfTable_New != E_OK" */  

            break;

         default:               /* Any other value of rowStatus is impossible */
                                /* for inexisting row.                        */
            return E_FAILED;    /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */


   /* An existing row  status changing. Check the requested row status */
   /* against the existing one and perform appropriate actions.        */
   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
            return ospfIfTable_Active(ospfMibObj, p);

         break;

      case notReady:
      case notInService:
         if(p->info.rowStatus == active)
            ospfIfTable_Deactive(ospfMibObj, p); 

         p->info.rowStatus = value;

         return E_OK;

         break;

      case destroy:
         if(p->info.rowStatus == active)
            ospfIfTable_Deactive(ospfMibObj, p);

         return ospfIfTable_Delete(ospfMibObj, ifIpAddr, ifAddrLessIf);

         break;

      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfIfTable_SetRowStatus procedure */



/*********************************************************************
 * @purpose      Set new value for admin status of the ospfIfTable
 *               and check consistency of this value
 *
 *
 * @param  ospfMibObj    @b{(input)}   t_Handle of the OSPF MIB object
 * @param  ifIpAddr      @b{(input)}   the interface IP address
 * @param  ifAddrLessIf  @b{(input)}   ifIndex (for interfaces having no IP addr)
 * @param  value         @b{(input)}   Value to be set into row status
 *
 * @returns           E_OK      - success, value can be set
 * @returns           E_BADPARM - bad parameter(s)
 * @returns           E_FAILED  - inconsistent value
 *
 * @notes    
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_SetAdminStatus(t_Handle ospfMibObj, ulng ifIpAddr,
                                 ulng ifAddrLessIf, e_ospfAdminStatus value)
{
   ospfIfEntry *p;
   t_IFO_Cfg    ifCfg;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   if (!p)
      return E_FAILED; 

   /* Check if the row is already in the requested admin status. */
   /* If yes, do nothing. */
   if (p->info.adminStatus == value)
      return E_OK;

   /* Check if rowStatus of the table entry is active.                  */
   /* If not, just change adminStatus to the requested one.             */
   /* If yes, it means that OSPF interface object has already been      */
   /* created, so it has to be disabled (stopped) or enabled (started). */
   if ((p->info.rowStatus == active) && p->owner)
   {  
      /* Read configuration paqrameters of the IFO object */
      ifCfg.IfStatus  = ROW_READ;
      if(IFO_Config(p->owner, &ifCfg) != E_OK)
         return E_FAILED;

      /* Set Admin status into the interface object.  */
      /* While the i/f was disabled, other parameters */
      /* could have changed. Set them into the IFO.   */
      ifCfg.AdminStat           = (value == enabled) ? TRUE : FALSE;
      ifCfg.AuType              = p->info.authType;
      ifCfg.RouterDeadInterval  = p->info.deadInterval;
      ifCfg.IfDemand            = p->info.demand;
      ifCfg.HelloInterval       = p->info.helloInterval;
      ifCfg.Type                = p->info.ifType;
      ifCfg.MulticastForwarding = p->info.mtCastForwarding;
      ifCfg.NbmaPollInterval    = p->info.pollInterval;
      ifCfg.RxmtInterval        = p->info.retransInterval;
      ifCfg.RouterPriority      = p->info.rtrPriority;
      ifCfg.InfTransDelay       = p->info.transitDelay;
      ifCfg.IfStatus            = ROW_CHANGE;
      memcpy(ifCfg.AuKey, p->info.authKey, 8);
      if (IFO_Config(p->owner, &ifCfg) != E_OK)
         return E_FAILED;

      if (value == enabled)
      {
         if (IFO_Up(p->owner) != E_OK)
            return E_FAILED;
      }
      else
         if (IFO_Down(p->owner) != E_OK)
            return E_FAILED;
   }

   p->info.adminStatus = value;

   return E_OK;

} /* End of ospfIfTable_SetAdminStatus procedure */



/*********************************************************************
 * @purpose      Set OSPF interface object handle into the entry of the
 *               ospfIfTable.
 *
 *
 * @param   ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param   ifIpAddr      @b{(input)}  the interface IP address
 * @param   ifAddrLessIf  @b{(input)}  ifIndex
 * @param   ifObj         @b{(input)}  t_Handle of the corresponding 
 *                                     IFO object(the owner)
 *
 * @returns        E_OK	    - Success
 * @returns        Not E_OK - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_SetOwner (t_Handle ospfMibObj, ulng ifIpAddr,
                            ulng ifAddrLessIf, t_Handle ifObj)
{
   ospfIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);
   if(!p)
      return E_NOT_FOUND;
   p->owner = ifObj;
   return E_OK;
}



/*********************************************************************
 * @purpose       Get OSPF interface object handle from the entry in
 *                the ospfIfTable.
 *
 *
 * @param  ospfMibObj     @b{(input)}   t_Handle of the OSPF MIB object
 * @param  ifIpAddr       @b{(input)}   the interface IP address
 * @param  ifAddrLessIf   @b{(input)}   ifIndex
 * 
 * 
 *
 * 
 *
 * @returns        OSPF area handle - Success
 * @returns        NULLP            - Otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfTable_GetOwner(t_Handle ospfMibObj, ulng ifIpAddr,
                              ulng ifAddrLessIf)
{
   ospfIfEntry *p;

   if (!ospfMibObj)
      return NULLP;

   p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);
   return p ? p->owner : NULLP;
}


/*********************************************************************
 * @purpose      Activate the ospf Interface table row by creating new
 *               interface object(IFO object) and activating it.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param p              @b{(input)}  a pointer to the appropriate entry in
 *                                    the ospfIpTable
 *
 * @returns         E_OK	    - Success
 * @returns         Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_Active(t_Handle ospfMibObj, ospfIfEntry  *p)
{
   t_Handle              LowLayerId = NULLP;
   t_Handle              p_IFOId;
   ospfIfMetricEntry    *p_MetricEntry;
   t_IFO_Cfg             IfCfg;
   t_S_IfMetric          IfMetrConfig; 
   ulng                  nilTOS = 0;
   t_ospfMibIfLowParams  ifLowParams;
   t_OspfMibObj         *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!ospfMibObj)
      return E_BADPARM;

   if (!p)
      return E_FAILED;

   /* Before interface object creating we need to check  if metric   */
   /* definitions (metric value and TOS) exist for this interface.   */
   /* Look up the appropriate row  in the ospfIfMetricTable and then */
   /* check the rowStatus, it has to be active. The third index of   */
   /* the ospfIfMetricTable is TOS, only TOS 0 is supported.         */
   p_MetricEntry = OLL_Find(_ospfIfMetricTable_, &(p->ospfIfIpAddres), 
                             &(p->ospfAddrLessIf), &nilTOS);

   if (!p_MetricEntry)
      return E_FAILED;

   if (p_MetricEntry->info.rowStatus != active)
      return E_FAILED;

   /* Get the low layer handler by calling user's procedure */
   if (p_ospfMib->ospfMibCallbacks.f_Get_IfLowLayerId)
      LowLayerId = 
        p_ospfMib->ospfMibCallbacks.f_Get_IfLowLayerId(p->ospfAddrLessIf);

   if(!LowLayerId)
      return E_FAILED;
   
   /* Create IFO object for the interface */
   if (IFO_Init(p_ospfMib->ospfRouterObj, LowLayerId, &p_IFOId) != E_OK)
      return E_FAILED;

   /* Initialize configuration parameters of the IFO object */
   memset(&IfCfg, 0, sizeof(t_IFO_Cfg));
   IfCfg.IpAdr    = p->ospfIfIpAddres;
   IfCfg.IfIndex  = (t_LIH)(p->ospfAddrLessIf);
   IfCfg.AreaId   = p->info.areaId;

   IfCfg.IsVpnSupported = FALSE; /* Proprietary VPN support is to be */ 
   IfCfg.VpnCos         = 0;     /* set to FALSE and VpnCos to 0    */

   IfCfg.VirtTransitAreaId = 0;
   IfCfg.VirtIfNeighbor    = 0;

   IfCfg.Type           = p->info.ifType;
   IfCfg.AdminStat      = TRUE; /* we need set this field to TRUE, otherwise */
                                /* IFO_Config does not set ifStatus to active */ 
   IfCfg.RouterPriority = p->info.rtrPriority;
   IfCfg.InfTransDelay  = p->info.transitDelay;
   IfCfg.RxmtInterval   = p->info.retransInterval;
   IfCfg.HelloInterval  = p->info.helloInterval;
   IfCfg.RouterDeadInterval = p->info.deadInterval;
   IfCfg.NbmaPollInterval   = p->info.pollInterval;
   IfCfg.AuType             = p->info.authType;
   memcpy (&IfCfg.AuKey, &(p->info.authKey), 8);
   IfCfg.IfDemand           = (p->info.demand == _true ? TRUE : FALSE);
   IfCfg.MulticastForwarding = p->info.mtCastForwarding;

   IfCfg.AckInterval = 1; /* second; This field is not defined in OSPF-MIB standard */ 
   
   /* Set MTU size */
   memset(&ifLowParams, 0, sizeof(t_ospfMibIfLowParams));
   if (p_ospfMib->ospfMibCallbacks.f_Get_IfParams)
   {
      if(p_ospfMib->ospfMibCallbacks.f_Get_IfParams (IfCfg.IfIndex, &ifLowParams) != E_OK)
         goto ifActive_fail_exit;

      /* IfCfg.MaxMTUsize = ifLowParams.ifMTUsize;*/
   }
   else
      goto ifActive_fail_exit;

   /* Set IpMask */
   if (p_ospfMib->ospfMibCallbacks.f_Get_IfMask)
      IfCfg.IpMask = 
            p_ospfMib->ospfMibCallbacks.f_Get_IfMask(p->ospfAddrLessIf, p->ospfIfIpAddres);
   else
      goto ifActive_fail_exit;

   IfCfg.IfStatus  = ROW_CREATE_AND_GO;  
   /* Config the new interface object according to the row fields */
   if(IFO_Config(p_IFOId, &IfCfg) != E_OK)
      goto ifActive_fail_exit;

   /* Now config the metric of the interface */
   memset(&IfMetrConfig, 0, sizeof(IfMetrConfig));
   IfMetrConfig.IfIndex   = (t_LIH)p->ospfAddrLessIf;
   IfMetrConfig.Metric    = p_MetricEntry->info.metricValue;
   IfMetrConfig.MetricTOS = p_MetricEntry->ospfIfMetricTOS;
   IfMetrConfig.MetricStatus = ROW_CREATE_AND_GO;

   if (IFO_MetricConfig( p_IFOId,  &IfMetrConfig) != E_OK)
      goto ifActive_fail_exit;

   /* Send indication to the user that IFO object was created */
   /* successfully */
   if (p_ospfMib->ospfMibCallbacks.f_ifoCreateDelInd)
   {
      if (p_ospfMib->ospfMibCallbacks.f_ifoCreateDelInd(p_ospfMib->ospfRouterMng,
                                        p_ospfMib->ospfRouterObj,
                                        p->ospfIfIpAddres, p->ospfAddrLessIf, 
                                        TRUE, p_IFOId)!= E_OK)
         goto ifActive_fail_exit;
   }
   
   /* If we get here everything is OK. Set the owner. */
   /* Set the row status to active and start */
   /* OSPF HELLO protocol on the interface.  */
   p->owner = p_IFOId;

   if (p->info.adminStatus == enabled)
      if (IFO_Up(p_IFOId) != E_OK)
         goto ifActive_fail_exit;

   p->info.rowStatus = active;
   
   return E_OK; 
   
ifActive_fail_exit:
   IFO_Delete(p_IFOId, (ulng)1);
   return E_FAILED;

}   /* end of ospfIfTable_Active procedure */



/*********************************************************************
 * @purpose      Deactivate the ospf Interface table row: delete the
 *               interface object(IFO object).
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p            @b{(input)}  a pointer to the appropriate entry in
 *                                   teh ospfIpTable
 *
 * @returns             E_OK	    - Success
 * @returns             Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_Deactive(t_Handle ospfMibObj, ospfIfEntry *p)
{
   t_OspfMibObj *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!p)
     return E_FAILED;

   /* Check whether the row is internally created. If yes, then the   */
   /* IFO object was created by the stack manager and we can't delete */
   /* it. This check is performed in ospfIfTable_SetTest, which is    */
   /* supposed to be called before ospfIfTable_SetRowStatus, so we    */
   /* would never get here, but we still check it just in case...     */
   if (p->info.internalCreated == FALSE)
   {
      p->info.SNMPdisable = TRUE;
      if(p->owner)
         IFO_Delete (p->owner, (ulng)1);

      p->info.SNMPdisable = FALSE;

      /* Send indication to the user that IFO object was deleted */
      if(p_ospfMib->ospfMibCallbacks.f_ifoCreateDelInd)
         p_ospfMib->ospfMibCallbacks.f_ifoCreateDelInd(p_ospfMib->ospfRouterMng,
                                           p_ospfMib->ospfRouterObj,
                                           p->ospfIfIpAddres, 
                                           p->ospfAddrLessIf, 
                                           FALSE, NULLP);

      p->owner = NULLP;
   }
   return E_OK;
}




/*********************************************************************
 * @purpose      Update the If Table entry according to the interface
 *               configuration.
 *
 *
 * @param  ospfMibObj  @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p_Cfg       @b{(input)}  a handle of the interface parameters
 *                                  structure,including the 'command'
 *                                  parameter
 *
 * @returns            E_OK       - success
 * @returns            E_BADPARM  - invalid parameter
 *
 * @notes        This procedure is to be called only
 *               from the OSPF code(not from the SNMP code).
 *
 * @end
 * ********************************************************************/
e_Err ospfIfTable_Update(t_Handle ospfMibObj, t_IFO_Cfg *p_Cfg)
{
   ospfIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   switch(p_Cfg->IfStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CHANGE:
         /* find the area entry and update it */
         p = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &p_Cfg->IpAdr, &p_Cfg->IfIndex);

         if (!p)
            return E_FAILED;

         p->info.adminStatus = p_Cfg->AdminStat;
         p->info.areaId = p_Cfg->AreaId;
         memcpy(p->info.authKey, p_Cfg->AuKey, 8);
         p->info.authType = p_Cfg->AuType;
         p->info.deadInterval = p_Cfg->RouterDeadInterval;
         p->info.demand = p_Cfg->IfDemand;
         p->info.helloInterval = p_Cfg->HelloInterval;
         p->info.ifType = p_Cfg->Type;
         p->info.mtCastForwarding = p_Cfg->MulticastForwarding;
         p->info.pollInterval = p_Cfg->NbmaPollInterval;
         p->info.retransInterval = p_Cfg->RxmtInterval;
         p->info.rtrPriority = p_Cfg->RouterPriority;
         p->info.transitDelay = p_Cfg->InfTransDelay;

      break;                           
      case  ROW_READ:
         /* do nothing */
      break;
      case  ROW_DESTROY:
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
      default:
         return E_BADPARM;
   }

   return E_OK;
}




/*********************************************************************
 * @purpose      Create a new row in the OSPF Interface Metric Table.
 *               Called from the SNMP code.
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr         @b{(input)}  the interface IP address
 * @param  ifAddrLessIf     @b{(input)}  ifIndex (for interfaces having no IP addr)
 * @param  ifTOS            @b{(input)}  the interface type of service
 *                                        (OSPF version 2 supports only TOS 0)
 *
 * @returns       E_OK       - success
 * @returns       E_IN_MATCH - entry already exists
 * @returns       E_BADPARM  - invalid parameter
 * @returns       E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_New(t_Handle ospfMibObj, ulng ifIpAddr,
                            ulng ifAddrLessIf, ulng ifTOS)
{
   ospfIfMetricEntry    *p;
   ulng                  baseMetric = 100000000;
   t_ospfMibIfLowParams  ifLowParams;
   t_OspfMibObj         *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if((!ifIpAddr ) || (!ifAddrLessIf))
      return E_FAILED;

   /* According to OSPF version 2 only tos_normal_service (i.e. TOS=0) */
   /* is supported */
   if (ifTOS)
      return E_FAILED;

   if(_ospfIfMetricTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfIfMetricTable_, &ifIpAddr, &ifAddrLessIf, &ifTOS))
         return E_IN_MATCH;
   }
   else
   {
      _ospfIfMetricTable_ = OLL_New(0,                 /* do not use DA */
                       3,                              /* number of keys */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetrIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetrAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetricTOS), /*key offset*/
                       (ulng)sizeof(ulng));
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfIfMetricEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfIfMetricEntry));

   /* Set the key values into the new entry */
   p->ospfIfMetrIpAddres = ifIpAddr;
   p->ospfIfMetrAddrLessIf = ifAddrLessIf;
   p->ospfIfMetricTOS      = ifTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfIfMetricTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in default values where possible. */
   memset(&ifLowParams, 0, sizeof(t_ospfMibIfLowParams));
   if(p_ospfMib->ospfMibCallbacks.f_Get_IfParams)
      if((p_ospfMib->ospfMibCallbacks.f_Get_IfParams(ifAddrLessIf,
                                           &ifLowParams) != E_OK) ||
          (ifLowParams.ifSpeed == 0))
      {
         /* A metric could be created before the interface is. In that case  */
         /* it's possible that at this point the interface doesn't exist yet */
         /* and so there's no low layer parameters for it. If that's so,     */
         /* create a metric with a default value 1000000000.                 */
         if (!OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf))
            ifLowParams.ifSpeed = 1;
         else
         {
            /* Failure: the matching interface exists, but its parameters */
            /* are invalid. Remove the unsuccessful metric entry.         */
            L7MIB_EXTRACTDELETE(p, _ospfIfMetricTable_);
            return E_FAILED;
         }
      }

   p->info.metricValue = (word)(baseMetric/ifLowParams.ifSpeed);
   if (p->info.metricValue == 0)
      p->info.metricValue = 1;

   p->info.rowStatus       = notReady;
   p->info.internalCreated = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfIfMetricTable_New procedure */



/*********************************************************************
 * @purpose      Create a new row in the OSPF Interface Metric Table.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having no IP addr)
 * @param  ifTOS          @b{(input)}  the interface type of service
 *                                     (OSPF version 2 supports only TOS 0)
 * @param  mValue         @b{(input)}  the metric value
 *
 * @param  ifIpAddr       @b{(input)}   the interface IP address
 *
 * @returns    E_OK       - success
 * @returns    E_IN_MATCH - entry already exists
 * @returns    E_BADPARM  - invalid parameter
 * @returns    E_NOMEMORY - can't allocate memory
 *
 * @notes      Called from the OSPF protocol code, when a new i/f
 *               metric is created.
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_InternalNewAndActive(t_Handle ospfMibObj,
                                             ulng ifIpAddr,
                                             ulng ifAddrLessIf,
                                             ulng ifTOS,
                                             word mValue)
{
   ospfIfMetricEntry    *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if((!ifIpAddr ) || (!ifAddrLessIf))
      return E_FAILED;

   /* According to OSPF version 2, only tos_normal_service */
   /* (i.e. TOS=0) is supported. */
   if (ifTOS)
      return E_FAILED;

   if(_ospfIfMetricTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfIfMetricTable_, &ifIpAddr, &ifAddrLessIf, &ifTOS))
         return E_IN_MATCH;
   }
   else
   {
      _ospfIfMetricTable_ = OLL_New(0,                 /* do not use DA */
                       3,                              /* number of keys */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetrIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetrAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfIfMetricEntry, ospfIfMetricTOS), /*key offset*/
                       (ulng)sizeof(ulng));
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfIfMetricEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfIfMetricEntry));

   /* Set the key values into the new entry */
   p->ospfIfMetrIpAddres   = ifIpAddr;
   p->ospfIfMetrAddrLessIf = ifAddrLessIf;
   p->ospfIfMetricTOS      = ifTOS;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfIfMetricTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   p->info.metricValue     = mValue;
   p->info.rowStatus       = active;
   p->info.internalCreated = TRUE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfIfMetricTable_InternalNewAndActive procedure */




/*********************************************************************
 * @purpose      Delete a row from the OSPF Interface Metric MIB Table.
 *               This routine is to be called from the SNMP code only.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIpAddr       @b{(input)}  the interface IP address
 * @param  ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having no IP addr)
 * @param  ifTOS          @b{(input)}  type of service of the i/f
 *
 * @returns         E_OK     - Success
 * @returns         Not E_OK - The row cannot be deleted
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_Delete(t_Handle ospfMibObj, ulng ifIpAddr,
                               ulng ifAddrLessIf, ulng ifTOS)
{
   ospfIfMetricEntry *p;
   ospfIfEntry       *p_IfEntry;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfMetricEntry *)OLL_Find(_ospfIfMetricTable_,
                                     &ifIpAddr, &ifAddrLessIf, &ifTOS);
   if (!p) 
   {
      /* The row does not exist already */
      return E_OK;
   }

   if (p && (p->info.internalCreated == TRUE))
   {
      /* this row was created internally, so can't be deleted via SNMP */
      return E_FAILED;
   }

   /* If there's an i/f using this metric, we can't  */
   /* delete this row from the OSPF If Metric Table. */
   p_IfEntry = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);

   if (p_IfEntry && (p_IfEntry->info.rowStatus == active))
      /* This metric row is in use by an active interface. Can't be deleted. */
      return E_FAILED;

   L7MIB_EXTRACTDELETE(p, _ospfIfMetricTable_);

   return E_OK;

} /* End of ospfIfMetricTable_Delete procedure */


/*********************************************************************
 * @purpose              Get an entry in the ospfIfMetricTable.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param ifIpAddr       @b{(input)}  the interface IP address
 * @param ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having no
 *                                                             IP addr)
 * @param ifTOS          @b{(input)}  the interface type of service
 *
 * @returns        pointer to the appropriate entry of ospfIfMetricTable
 *                 on success
 * @returns       NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfMetricTable_Lookup(t_Handle ospfMibObj, ulng ifIpAddr,
                                  ulng ifAddrLessIf, ulng ifTOS)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfIfMetricTable_, &ifIpAddr, &ifAddrLessIf, &ifTOS);

   return p;
}


/*********************************************************************
 * @purpose                 Get the next entry in the ospfIfMetricTable
 *
 *
 * @param  ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param  prevIfIpAddr      @b{(input)}  the interface IP address
 * @param  prevIfAddrLessIf  @b{(input)}  ifIndex (for interfaces having
 *                                                            no IP addr)
 * @param  prevTOS           @b{(input)}  type of service of the i/f
 * @param  p_IfIpAddr        @b{(output)} the interface IP address
 * @param  p_IfAddrLessIf    @b{(output)} ifIndex (for interfaces having no IP addr)
 * @param  p_IfTOS           @b{(output)} type of service of the i/f
 *
 * @returns    pointer to the next (after the previous) entry in the
 *             ospfIfMetricTable and the next entry indexes on success
 *             NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfMetricTable_GetNext(t_Handle ospfMibObj, ulng prevIfIpAddr,
                                   ulng prevIfAddrLessIf, ulng prevTOS,
                                   ulng *p_IfIpAddr, ulng *p_IfAddrLessIf,
                                   ulng *p_IfTOS)
{
   ospfIfMetricEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfIfMetricTable_ == 0)
      return NULLP;

   next = (ospfIfMetricEntry *)OLL_FindNext(_ospfIfMetricTable_, 
                                       &prevIfIpAddr, &prevIfAddrLessIf,
                                       &prevTOS);
   if(next)
   {
      *p_IfIpAddr     = next-> ospfIfMetrIpAddres;
      *p_IfAddrLessIf = next->ospfIfMetrAddrLessIf;
      *p_IfTOS        = next->ospfIfMetricTOS;

      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}




/*********************************************************************
 * @purpose                 Get the first entry in the ospfIfMetricTable
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 *
 * @param p_IfIpAddr        @b{(output)} the interface IP address
 * @param p_IfAddrLessIf    @b{(output)} ifIndex (for interfaces having no
 *                                                                IP addr)
 * @param p_IfTOS           @b{(output)} type of service of the i/f
 *
 * @returns              pointer to the first  entry in the ospfIfMetricTable
 *                       and the indexes of the first entry on success
 *                       NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfIfMetricTable_GetFirst(t_Handle ospfMibObj, ulng *p_IfIpAddr,
                                    ulng *p_IfAddrLessIf, ulng *p_IfTOS)
{
   ospfIfMetricEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfIfMetricTable_ == 0)
      return NULLP;

   first = (ospfIfMetricEntry *)OLL_GetNext(_ospfIfMetricTable_, NULLP);

   if(first)
   {
      *p_IfIpAddr     = first-> ospfIfMetrIpAddres;
      *p_IfAddrLessIf = first->ospfIfMetrAddrLessIf;
      *p_IfTOS        = first->ospfIfMetricTOS;

      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}




/*********************************************************************
 * @purpose      Test the parameters in a row of the ospfIfMetricTable
 *               before setting them by SNMP agent
 *
 *
 * @param  lastOid    @b{(input)}  parameter offset in the row
 * @param  newValue   @b{(input)}  value to set
 * @param  p          @b{(input)}  pointer to ospfAreaRangeEntry
 *
 * @returns           E_OK     - success, value can be set
 * @returns           E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfIfMetricEntry  *p_ifMetricEntry = (ospfIfMetricEntry *)p;

   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfIfMetricTestTable, 
                            sizeof(ospfIfMetricTestTable)/
                            sizeof(ospfIfMetricTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;

   if (lastOid == ospfIfMetricTableRowStatusSubId)
   {
      /* row status of internally created objects can't be changed via SNMP */
      if (p && (p_ifMetricEntry->info.internalCreated == TRUE))
         return E_FAILED;
   }

   /* All other parameters can be changed "on the fly". */
   return E_OK;
}


/*********************************************************************
 * @purpose      Set new value for row status of the ospfIfMetricTable
 *               and check consistency of this value
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param ifAddrLessIf   @b{(input)}  the interface IP address
 * @param ifAddrLessIf   @b{(input)}  ifIndex (for interfaces having
 *                                                        no IP addr)
 * @param ifTOS          @b{(input)}  the TOS metric being referenced
 * @param value          @b{(input)}  Value to be set into row status
 *
 * @returns              E_OK      - success, value can be set
 * @returns              E _BADPARM - bad parameter(s)
 * @returns              E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_SetRowStatus(t_Handle ospfMibObj, ulng ifIpAddr,
                                     ulng ifAddrLessIf, ulng ifTOS, ulng value)
{
   ospfIfMetricEntry *p;
   ospfIfEntry       *p_IfEntry;
   e_Err              rc = E_FAILED;
   Bool               ifNotInService = FALSE;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfMetricEntry *)OLL_Find(_ospfIfMetricTable_, 
                                &ifIpAddr, &ifAddrLessIf, &ifTOS);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfIfMetricTable_New(ospfMibObj, ifIpAddr, ifAddrLessIf, ifTOS);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfIfMetricTable_New(ospfMibObj, ifIpAddr, ifAddrLessIf, ifTOS))
                                                                     != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfIfMetricEntry *)OLL_Find(_ospfIfMetricTable_, 
                                          &ifIpAddr, &ifAddrLessIf, &ifTOS);
               if (!p)
               {
                  /*Something wrong occured: the row was successfully created*/
                  /*but hasn't been found in ospfIfTable OLL. Delete the row.*/
                  ospfIfMetricTable_Delete(ospfMibObj, ifIpAddr, ifAddrLessIf, ifTOS);
                  return E_FAILED;
               }
               else 
               {
                  /* The new row has been successfully created. */ 
                  /* Activate the row */
                  p->info.rowStatus = active;
                  return E_OK;                                     
               }
            } /* End of "if rc = ospfIfMetricTable_New != E_OK" */  
            
            break;

         default:              /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command  */
   }  /* End of if(!p) command; i.e. end of a new row processing */


   /* An existing row status changing. Check the requested row status  */
   /* against the existing one and perform appropriate actions.        */

   if ((e_RowStatus)value == p->info.rowStatus) 
      return E_OK;

   /* Row status of internally created objects can't be changed via SNMP.    */
   /* This is checked in the _SetTest routine, but just in case, check again.*/
   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Find the appropriated ospf interface and determine if it is */
   /* in service. The ospfIfMetricTable row can be deactivated or */
   /* deleted when the interface is out of service only.          */
   p_IfEntry = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &ifIpAddr, &ifAddrLessIf);
   ifNotInService = (!p_IfEntry || 
                     (p_IfEntry && 
                     ((p_IfEntry->info.rowStatus != active) || 
                     (p_IfEntry->info.adminStatus != enabled))));
   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
         {
            p->info.rowStatus = active;
            return E_OK;
         }         
         break;

      case notInService:
      case notReady:
         if(p->info.rowStatus == active)
         {
            /* Verify if the row can be deactivated. */ 
            if (ifNotInService)
            {
               p->info.rowStatus = value;
               return E_OK;
            }
            else 
               /* The interface using this metric is active now. So, we cannot*/
               /* set metric row to inactive state and change metric params.  */
               return E_FAILED;  
         }
         else  /* The row is not active state now */
         {
            p->info.rowStatus = value;
            return E_OK; 
         }

         break;

      case destroy:
         /* Verify if the row can be deleted. */
         if (ifNotInService)
            return ospfIfMetricTable_Delete(ospfMibObj, ifIpAddr, ifAddrLessIf, ifTOS);
         else 
            /* The interface using this metric is in service */
            /* now. The metric row cannot be deleted.        */
            return E_FAILED;
         break;

      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses (because the row already exists */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfIfMetricTable_SetRowStatus procedure */




/*********************************************************************
 * @purpose     update the Interface Metric Table according to the 'command'
 *              parameter.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param p_Metric        @b{(input)}  i/f metric parameters structure,
 *                                     including the 'command' parameter
 *
 * @returns              E_OK       - success
 * @returns              E _BADPARM - invalid parameter
 * @returns              E_FAILED   - inconsistent value
 *
 * @notes         either create a new row, or update/destroy
 *                an existing one This routine is to be called from OSPF
 *                protocol code only.
 *
 * @end
 * ********************************************************************/
e_Err ospfIfMetricTable_Update(t_Handle ospfMibObj, t_S_IfMetric *p_Metric)
{
   ospfIfMetricEntry *p;
   ospfIfEntry       *p_IfEntry;
   ulng               metricTOS = (ulng)p_Metric->MetricTOS;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfIfMetricEntry *)OLL_Find(_ospfIfMetricTable_, &p_Metric->IpAdr,
                                     &p_Metric->IfIndex, &metricTOS);

   switch(p_Metric->MetricStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
         if (!p)
            ospfIfMetricTable_InternalNewAndActive(ospfMibObj, p_Metric->IpAdr,
                                                   p_Metric->IfIndex,
                                                   metricTOS, p_Metric->Metric);
      break;
      case  ROW_CHANGE:
         if (!p)
            return E_FAILED;

         p->info.metricValue = p_Metric->Metric;

      break;
      case  ROW_READ:
      case  ROW_READ_NEXT:
         /* do nothing */
      break;
      case  ROW_DESTROY:
         if (p)
         {
            /* If there's an i/f using this metric , we can't */
            /* delete this row from the OSPF If Metric Table. */
            p_IfEntry = (ospfIfEntry *)OLL_Find(_ospfIfTable_,
                                                &p_Metric->IpAdr,
                                                &p_Metric->IfIndex);
            if (p_IfEntry && (p_IfEntry->info.rowStatus == active))
               return E_FAILED;

            L7MIB_EXTRACTDELETE(p, _ospfIfMetricTable_);
         }
      break;
      default:
         return E_BADPARM;
      break;
   }

   return E_OK;
}



/*********************************************************************
 * @purpose             Create a new row in the ospf Virtual Interface
 *                      Table
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param areaId        @b{(input)}  IP Address of the Transit Area that
 *                                   the virtual interface traverses
 * @param ifNbr         @b{(input)}  IP Address of the virtual neighbor
 *                                   router
 *
 * @returns             E_OK       - success
 * @returns             E_IN_MATCH - entry already exists
 * @returns             E_BADPARM  - invalid parameter
 * @returns             E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_New(t_Handle ospfMibObj, ulng areaId, ulng ifNbr)
{
   ospfVirtIfEntry  *p = NULLP;
   Bool              stopSearch = FALSE;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if(!ifNbr)
      return E_FAILED;

   if(_ospfVirtIfTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr))
         return E_IN_MATCH;

      /* No more than one virtual interface can  */
      /* be defined to the same virtual neighbor */
      while ((p = OLL_GetNext(_ospfVirtIfTable_, p)) && !stopSearch)
      {
         if(p->ospfVirtIfNbr == ifNbr) 
            return E_FAILED; 
         else
            if(p->ospfVirtIfNbr > ifNbr) 
               stopSearch = TRUE;
      }
   }
   else
   {
      _ospfVirtIfTable_ = OLL_New(0,                   /* do not use DA */                        
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfVirtIfEntry, ospfVirtIfAreaId), /* key offset */
                       (ulng)IP_ADDR_SIZE,                   /* key length */
                       (ulng)offsetof(ospfVirtIfEntry, ospfVirtIfNbr), /*key offset*/
                       (ulng)IP_ADDR_SIZE);                  /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfVirtIfEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfVirtIfEntry));

   /* Set the key values into the new entry */
   p->ospfVirtIfAreaId = areaId;
   p->ospfVirtIfNbr    = ifNbr;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfVirtIfTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in configuration fields by default values.   */
   p->info.transitDelay    = INF_TRANS_DELAY_DFLT;
   p->info.retransInterval = RXMT_INTERVAL_DFLT;
   p->info.helloInterval   = HELLO_INTERVAL_DFLT;
   p->info.deadInterval    = ROUTER_DEAD_INTERVAL_DFLT;
   p->info.authType        = IFO_NULL_AUTH;
   memset(p->info.authKey, 0, 8);
   p->info.rowStatus       = notReady;

   p->info.internalCreated = FALSE;
   p->info.SNMPdisable     = FALSE;
   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfVirtIfTable_New procedure */


/*********************************************************************
 * @purpose      Create a new row in the OSPF Virtual Interface MIB Table
 *               and configure it according to its "owner" IFO.
 *
 *
 * @param ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param areaId        @b{(input)}  IP Address of the Transit Area that
 *                                   the virtual
 * @param ifNbr         @b{(input)}  IP Address of the virtual neighbor
 *                                   router
 * @param ifObj         @b{(input)}  t_Handle of the corresponding IFO
 *                                   object(the owner)
 *
 * @returns             E_OK       - success
 * @returns             E_IN_MATCH - entry already exists
 * @returns             E_BADPARM  - invalid parameter
 * @returns             E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_InternalNewAndActive(t_Handle ospfMibObj, ulng areaId,
                                  ulng ifNbr, t_Handle ifObj)
{
   ospfVirtIfEntry  *p = NULLP;
   Bool              stopSearch = FALSE;
   t_IFO_Cfg         ifCfg;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if(!ifNbr)
      return E_FAILED;

   if(_ospfVirtIfTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr))
         return E_IN_MATCH;

      /* No more than one virtual interface can */
      /* be defined on the same transit area.   */
      while ((p = OLL_GetNext(_ospfVirtIfTable_, p)) && !stopSearch)
      {
            if(p->ospfVirtIfAreaId > areaId)
               stopSearch = TRUE;
      }   
   }
   else
   {
      _ospfVirtIfTable_ = OLL_New(0,                   /* do not use DA */                        
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfVirtIfEntry, ospfVirtIfAreaId), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfVirtIfEntry, ospfVirtIfNbr), /*key offset*/
                       (ulng)IP_ADDR_SIZE);            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfVirtIfEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfVirtIfEntry));

   /* Set the key values into the new entry */
   p->ospfVirtIfAreaId = areaId;
   p->ospfVirtIfNbr    = ifNbr;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfVirtIfTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in configuration fields according to the object para- */
   /* meters. Get the area config parameters from the object.    */
   ifCfg.IfStatus = ROW_READ;
   if (IFO_Config(ifObj, &ifCfg) != E_OK)
   {
     L7MIB_EXTRACTDELETE(p, _ospfVirtIfTable_);
     return E_FAILED;
   }

   p->info.transitDelay     = ifCfg.InfTransDelay;
   p->info.retransInterval  = ifCfg.RxmtInterval;
   p->info.helloInterval    = ifCfg.HelloInterval;
   p->info.deadInterval     = ifCfg.RouterDeadInterval;
   p->info.authType         = ifCfg.AuType;
   memcpy(p->info.authKey, ifCfg.AuKey, 8);
   p->info.keyLen           = 8;  /* default in our configuration */

   p->info.rowStatus       = active;
   p->info.internalCreated = TRUE;
   p->info.SNMPdisable     = FALSE;
   p->owner                = ifObj;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfVirtIfTable_InternalNewAndActive procedure */



/*********************************************************************
 * @purpose            Delete a row from the OSPF Virtual Interface MIB
 *                     Table.
 *
 *
 * @param ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param ifNbr        @b{(input)}  IP Address of the Transit Area that the
 *                                  virtual interface traverses
 * @param              @b{(input)}  IP Address of the virtual neighbor router
 *
 * @returns            E_OK     - Success
 * @returns            Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_Delete(t_Handle ospfMibObj, ulng areaId, ulng ifNbr)
{
   ospfVirtIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfVirtIfEntry *)OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr);

   if (!p)
      return E_OK;

   /* Check whether the row was internally created (i.e. */
   /* by stack manager), or not (i.e. created via SNMP). */
   /* We can only delete what was created by SNMP.       */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;
   else
   {
      if (p->owner)
      {
         p->info.SNMPdisable = TRUE; /* to prevent deletion by DeleteInternal */
         IFO_Delete (p->owner, (ulng)1);
      }

      L7MIB_EXTRACTDELETE(p, _ospfVirtIfTable_);
   }

   return E_OK;

} /* End of ospfVirtIfTable_Delete procedure */



/*********************************************************************
 * @purpose      Delete a row from the OSPF Virtual Interface MIB Table.
 *
 *
 * @param ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param areaId        @b{(input)}  IP Address of the Transit Area that the
 *                                   virtual interface traverses
 * @param ifNbr         @b{(input)} IP Address of the virtual neighbor router
 *
 * @returns       E_OK     - Success
 * @returns       Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_DeleteInternal(t_Handle ospfMibObj, ulng areaId,
                                     ulng ifNbr)
{
   ospfVirtIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfVirtIfEntry *)OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr);

   if (!p)
      return E_OK;

   if (p->info.SNMPdisable == TRUE)
      /* IFO_Delete was called from ospfVirtIfTable_Deactive. Return. */
      return E_OK;

   L7MIB_EXTRACTDELETE(p, _ospfVirtIfTable_);

   return E_OK;

} /* End of ospfVirtIfTable_DeleteInternal procedure */


/*********************************************************************
 * @purpose             Get an entry in the ospfVirtIfTable.
 *
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  IP Address of the Transit Area that the
 *                                   virtual interface traverse
 * @param  ifNbr        @b{(input)}  IP Address of the virtual neighbor router
 *
 * @returns             pointer to the appropriate entry of ospfVirtIfTable
 *                      on success
 * @returns             NULLP - otherwis
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtIfTable_Lookup(t_Handle ospfMibObj, ulng areaId, ulng ifNbr)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr);

   return p;
}



/*********************************************************************
 * @purpose                 Get the next entry in the ospfVirtIfTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param prevAreaId        @b{(input)}  the interface IP address
 * @param prevIfNbr         @b{(input)}  ifIndex (for interfaces having no IP
 *                                                                    addr)
 *
 * @param  p_AreaId         @b{(output)} the interface IP address
 * @param  p_IfNbr          @b{(output)} ifIndex (for interfaces having
 *                                                          no IP addr)
 *
 * @returns             pointer to the next (after the previous) entry in the
 *                      ospfIfTable and the next entry indexes on success
 * @returns             NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtIfTable_GetNext(t_Handle ospfMibObj, ulng prevAreaId,
                                 ulng prevIfNbr, ulng *p_AreaId,
                                 ulng *p_IfNbr)
{
   ospfVirtIfEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfVirtIfTable_ == 0)
      return NULLP;

   next = (ospfVirtIfEntry *)OLL_FindNext(_ospfVirtIfTable_, 
                                       &prevAreaId, &prevIfNbr);
   if(next)
   {
      *p_AreaId = next->ospfVirtIfAreaId;
      *p_IfNbr = next->ospfVirtIfNbr;
      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}



/*********************************************************************
 * @purpose               Get the first entry in the ospfVirtIfTable.
 *
 *
 * @param ospfMibObj      @b{(output)}  t_Handle of the OSPF MIB object
 * @param p_AreaId        @b{(output)}  the interface IP address
 * @param p_IfNbr         @b{(output)}  ifIndex (for interfaces having no
 *                                                               IP addr)
 *
 * @returns          pointer to the first entry in the
 *                   ospfVirtIfTable and the first entry indexes on success
 * @returns          NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtIfTable_GetFirst(t_Handle ospfMibObj, ulng *p_AreaId,
                                  ulng *p_IfNbr)
{
   ospfVirtIfEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfVirtIfTable_ == 0)
      return NULLP;

   first = (ospfVirtIfEntry *)OLL_GetNext(_ospfVirtIfTable_, NULLP);
   if(first)
   {
      *p_AreaId = first->ospfVirtIfAreaId;
      *p_IfNbr  = first->ospfVirtIfNbr;
      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}




/*********************************************************************
 * @purpose       Test the parameters in a row of the ospfVirtIfTable
 *                before setting them by SNMP agent
 *
 * @param lastOid        @b{(input)}  parameter offset in the row
 * @param newValue       @b{(input)}  value to set
 * @param p              @b{(input)}  pointer to ospfVirtIfEntry
 *
 * @returns              E_OK     - success, value can be set
 * @returns              E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfVirtIfEntry *p_virtIfEntry = (ospfVirtIfEntry *)p;
   
   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfVirtIfTestTable,
                            sizeof(ospfVirtIfTestTable)/
                            sizeof(ospfVirtIfTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;

   /* OSPF Virt Interface table fields are "CREATE_READ" objects. */
   /* Therefore they can be changed (set) only during conceptual  */
   /* row creation (i.e. before the row status becomes "active"). */

   if (lastOid == ospfVirtIfTableRowStatusSubId)
   {
      if (p_virtIfEntry)
         if (p_virtIfEntry->info.internalCreated == TRUE)
            return E_FAILED;
      return E_OK;
   }
   else
      if((p_virtIfEntry->info.rowStatus != notReady) &&
         (p_virtIfEntry->info.rowStatus != notInService))
          return E_FAILED;

      return E_OK;   
}



/*********************************************************************
 * @purpose      Set new value for row status of the ospfVirtIfTable
 *               and check consistency of this value.
 *
 * @param  ospfMibObj   @b{(input)}  t_Handle of the OSPF MIB object
 * @param  areaId       @b{(input)}  IP Address of the Transit Area that
 *                                   the virtual interface traverses
 * @param  ifNbr        @b{(input)}  IP Address of the virtual neighbor router
 *
 * @returns             E_OK     - success, value can be set
 * @returns             E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_SetRowStatus(t_Handle ospfMibObj, ulng areaId,
                                   ulng ifNbr, ulng value)
{
   ospfVirtIfEntry *p;
   e_Err            rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfVirtIfEntry *)OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfVirtIfTable_New(ospfMibObj, areaId, ifNbr);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfVirtIfTable_New(ospfMibObj, areaId, ifNbr)) != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfVirtIfEntry *)OLL_Find(_ospfVirtIfTable_, &areaId, &ifNbr);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfIfTable OLL. Delete the created row.          */
                  ospfVirtIfTable_Delete(ospfMibObj, areaId, ifNbr);
                  return E_FAILED;
               }
               else 
               {
                  /* New row has been successfully created. */ 
                  /* Activate the row */
                  if ((rc = ospfVirtIfTable_Active(ospfMibObj, p)) != E_OK)
                  {
                     ospfVirtIfTable_Delete(ospfMibObj, areaId, ifNbr);
                     return rc;
                  }
                  else
                  {
                     return E_OK;
                  }                     
               }

            } /* End of "if rc = ospfVirtIfTable_New != E_OK" */  
            
            break;

         default:               /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */


   /* An existing row  status changing. Check the requested row status */
   /* against the existing one and perform appropriate actions.        */
   if ((e_RowStatus)value == p->info.rowStatus) 
      return E_OK;

   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
         {
            return ospfVirtIfTable_Active(ospfMibObj, p);
         }         
         break;

      case notReady:
      case notInService:
         if(p->info.rowStatus == active)
            if (ospfVirtIfTable_Deactive(p) != E_OK)
               return E_FAILED;

         p->info.rowStatus = value;

         return E_OK;

         break;

      case destroy:
         return ospfVirtIfTable_Delete(ospfMibObj, areaId, ifNbr);

         break;
   
      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfVirtIfTable_SetRowStatus procedure */




/*********************************************************************
 * @purpose       Update the Virtual If Table entry according to the
 *                interface configuration.
 *
 *
 * @param ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param *p_Cfg        @b{(input)}  a handle of the virtual interface
 *                                   parameters structure,including the
 *                                  'command' parameter
 *
 * @returns             E_OK       - success
 * @returns             E_BADPARM  - invalid parameter
 *
 * @notes
 *               This procedure is to be called only from the OSPF code
 *                (not from the SNMP code).
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_Update(t_Handle ospfMibObj, t_IFO_Cfg *p_Cfg)
{
   ospfVirtIfEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   switch(p_Cfg->IfStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CHANGE:
         /* find the area entry and update it */
         p = (ospfVirtIfEntry *)OLL_Find(_ospfVirtIfTable_, &p_Cfg->VirtTransitAreaId,
                                         &p_Cfg->VirtIfNeighbor);

         if (!p)
            return E_FAILED;

         p->info.retransInterval = p_Cfg->RxmtInterval;
         p->info.transitDelay = p_Cfg->InfTransDelay;
         p->info.helloInterval = p_Cfg->HelloInterval;
         p->info.deadInterval = p_Cfg->RouterDeadInterval;
         p->info.authType = p_Cfg->AuType;
         memcpy(p->info.authKey, p_Cfg->AuKey, 8);

      break;                           
      case  ROW_READ:
         /* do nothing */
      break;                           
      case  ROW_DESTROY:
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
      default:
         return E_BADPARM;
   }

   return E_OK;
}



/*********************************************************************
 * @purpose                 Activate a row of the ospfVirtIfTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param p                 @b{(input)}  pointer to ospfVirtIfEntry
 *
 * @returns                 E_OK     - success, the row has been activate
 * @returns                 E_FAILED - the row cannot be activate
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_Active(t_Handle ospfMibObj, ospfVirtIfEntry *p)
{
   t_Handle      p_IFOId;
   t_IFO_Cfg     IfCfg;
   t_OspfMibObj *p_ospfMib = (t_OspfMibObj *)ospfMibObj;

   if ((!p) || (!ospfMibObj))
      return E_FAILED;

   /* If this is an internally created interface, this    */
   /* routine mustn't be called, as we don't allow to     */
   /* change the rowStatus of internally created objects. */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Create IFO object for the virtual interface. The third parameter */
   /* of IFO_Init - LowLayerId is 0, because it is virt. interface.    */
   if (IFO_Init(p_ospfMib->ospfRouterObj, NULLP, &p_IFOId)!= E_OK)
      return E_FAILED;

   /* Set the interface configuration parameters */
   memset(&IfCfg, 0, sizeof(t_IFO_Cfg));
   IfCfg.IfStatus  = ROW_CREATE_AND_GO;

   IfCfg.VirtTransitAreaId = p->ospfVirtIfAreaId;
   IfCfg.VirtIfNeighbor    = p->ospfVirtIfNbr;

   /* Set parameters that could have been changed. */
   IfCfg.InfTransDelay  = p->info.transitDelay;
   IfCfg.RxmtInterval   = p->info.retransInterval;
   IfCfg.HelloInterval  = p->info.helloInterval;
   IfCfg.RouterDeadInterval = p->info.deadInterval;
   IfCfg.AuType             = p->info.authType;
   memcpy (&IfCfg.AuKey, &(p->info.authKey), 8);

   IfCfg.IpAdr   = p->ospfVirtIfNbr;  /* Neighbor ID is used as IP addr  */
                                      /* of the virtual interface */
   IfCfg.IfIndex = 0;
   IfCfg.AreaId  = 0;
   IfCfg.IpMask  = 0;
   IfCfg.Type           = IFO_VRTL;  /* point-to-point interface */
   IfCfg.AdminStat      = TRUE;
   IfCfg.RouterPriority = 0;
   IfCfg.DrId           = 0;
   IfCfg.BackupId       = 0;

   /* Proprietary VPN support is to be set to FALSE and VpnCos to 0 */
   IfCfg.IsVpnSupported = FALSE;
   IfCfg.VpnCos         = 0;

   IfCfg.AckInterval = 1; /* second; This field is not defined in OSPF-MIB standard */

   /* Config the new interface object according to the row fields */
   if (IFO_Config(p_IFOId, &IfCfg) != E_OK)
   {
      /* Delete the created object and exit */
      IFO_Delete(p_IFOId, (ulng)1);
      return E_FAILED;
   }

   /* Set the owner and the row status. */
   p->owner          = p_IFOId;
   p->info.rowStatus = active;

   /* If we get here everything is OK. */
   return E_OK;

}  /* End of ospfVirtIfTable_Active */



/*********************************************************************
 * @purpose       Deactivate the ospf virtual i/f table row: delete
 *                the interface object (IFO object).
 *
 *
 * @param *p        @b{(input)}  a pointer of the Virtual Interface Table
 *                               row to be deactivated.
 *
 * @returns       E_OK            - Success
 * @returns       Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtIfTable_Deactive(ospfVirtIfEntry  *p)
{
  if (!p)
    return E_FAILED;

   /* Check whether this i/f was created internally. */
   /* If so, it must not be deactivated via SNMP.    */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;
   
   if (p->owner)
   {
      p->info.SNMPdisable = TRUE;
      IFO_Delete (p->owner, (ulng)1);
      p->info.SNMPdisable = FALSE;
      p->owner = NULLP;
   }

   return E_OK;
}




/*********************************************************************
 * @purpose                Create a new row in the OSPF Neighbor Table
 *
 *
 * @param  ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  nbrIpAddr       @b{(input)}  the neighbor IP address
 * @param  nbrAddrLessId   @b{(input)}  ifIndex (for interfaces having
 *                                                         no IP addr)
 *
 * @returns                E_OK       - success
 * @returns                E_IN_MATCH - entry already exists
 * @returns                E_BADPARM  - invalid parameter
 * @returns                E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_New(t_Handle ospfMibObj, ulng nbrIpAddr, ulng nbrAddrLessId)
{
   ospfNbrEntry  *p = NULLP;
   ospfIfEntry   *p_ifEntry = NULLP;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if((!nbrIpAddr ) || (!nbrAddrLessId))
      return E_FAILED;

   /* Check that the interface connecting our router with the neighbor exists */
   /* and it is an NBMA one. Only such neighbor may be configured via SNMP. */
   if (! (p_ifEntry = ospfFindIfEntryFromIfIndex(ospfMibObj, nbrAddrLessId)))  
      return E_FAILED;

   if ( p_ifEntry->info.ifType != IFO_NBMA)
      return E_FAILED;

   if(_ospfNbrTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId))
         return E_IN_MATCH;
   }
   else
   {
      _ospfNbrTable_ = OLL_New(0,                      /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfNbrEntry, ospfNbrIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,             /* key length */
                       (ulng)offsetof(ospfNbrEntry, ospfNbrAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfNbrEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfNbrEntry));

   /* Set the key values into the new entry */
   p->ospfNbrIpAddres   = nbrIpAddr;
   p->ospfNbrAddrLessIf =  nbrAddrLessId;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfNbrTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in the configuration fields by the default values. */
   p->info.rtrId            = nbrIpAddr; /* here we copy NBR IP address,  */
                                         /* the real routerID will be set */
                                         /* by the OSPF protocol code.    */
   p->info.priority         = 0;
   p->info.permanence       = permanent; /* 2  - i.e. configured and not */
                                         /* learned from protocol        */
   p->info.rowStatus        = notReady;
   p->info.internalCreated  = FALSE;
 
   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfNbrTable_New procedure */


/*********************************************************************
 * @purpose        Create and activate a new row in the OSPF Neighbor Table.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrIpAddr      @b{(input)}  the neighbor IP address
 * @param nbrAddrLessId  @b{(input)}  ifIndex
 * @param owner          @b{(input)}  t_Handle of the neighbor object
 *
 * @returns              E_OK       - success
 * @returns              E_IN_MATCH - entry already exists
 * @returns              E_BADPARM  - invalid parameter
 * @returns              E_NOMEMORY - can't allocate memory
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_InternalNewAndActive(t_Handle ospfMibObj, ulng nbrIpAddr,
                                        ulng nbrAddrLessId, t_Handle owner)
{
   ospfNbrEntry  *p;
   t_NBO         *nbrObj = (t_NBO *)owner;

   if ((!ospfMibObj) || (!owner))
      return E_BADPARM;

   /* Check indexes consistency: ifIndex is not allowed */
   if (!nbrAddrLessId)
      return E_FAILED;

   if(_ospfNbrTable_ != 0)
   {
      /* Check if the table entry already exists. */
      /* If this neighbor row was created via SNMP, set the owner and exit. */
      if((p = OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId)))
      {
         if (p->owner == NULL)
            p->owner = owner;
         return E_OK;
      }
   }
   else
   {
      _ospfNbrTable_ = OLL_New(0,                       /* do not use DA */                        
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfNbrEntry, ospfNbrIpAddres), /* key offset */
                       (ulng)IP_ADDR_SIZE,                   /* key length */
                       (ulng)offsetof(ospfNbrEntry, ospfNbrAddrLessIf), /*key offset*/
                       (ulng)sizeof(ulng));                  /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfNbrEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfNbrEntry));

   /* Set the key values into the new entry */
   p->ospfNbrIpAddres   = nbrIpAddr;
   p->ospfNbrAddrLessIf = nbrAddrLessId; 

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfNbrTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Set NBR owner and other fields according to the neighbor object */
   p->owner = owner;
   p->info.rtrId           = nbrObj->RouterId;
   p->info.priority        = nbrObj->RouterPriority;
   p->info.permanence      = dynamic;
   p->info.internalCreated = TRUE;

   /* Set the row status to active  */
   p->info.rowStatus = active;

   return E_OK;

} /* End of ospfNbrTable_InternalNewAndActive  */



/*********************************************************************
 * @purpose       Deletes a row from the OSPF Neighbor Table. This routine
 *                is to be called from the SNMP code only.
 *
 *
 * @param  ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  nbrIpAddr       @b{(input)}  the neighbor IP address
 * @param  nbrAddrLessId   @b{(input)}  ifIndex for i/f w/o and IP address
 *
 * @returns                E_OK      - deleted
 * @returns                E_BADPARM - bad parameter(s)
 * @returns
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_Delete(t_Handle ospfMibObj, ulng nbrIpAddr,
                          ulng nbrAddrLessId)
{
   ospfNbrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_,
                                &nbrIpAddr, &nbrAddrLessId);

   if (p)
      L7MIB_EXTRACTDELETE(p, _ospfNbrTable_);

   return E_OK;

} /* End of ospfNbrTable_Delete procedure */


/*********************************************************************
 * @purpose      Deletes a row from the OSPF Neighbor Table. This routine
 *               is to be called from the OSPF protocol code only.
 *
 *
 * @param ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrIpAddr        @b{(input)}  the neighbor IP address
 * @param nbrAddrLessId    @b{(input)}  ifIndex for i/f w/o and IP address
 *
 * @returns                E_OK      - deleted
 * @returns                E_BADPARM - bad parameter(s)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_DeleteInternal(t_Handle ospfMibObj, ulng nbrIpAddr,
                                  ulng nbrAddrLessId)
{
   ospfNbrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_,
                                &nbrIpAddr, &nbrAddrLessId);

   if (p)
   {
      if (p->info.internalCreated == TRUE)
      {
         /* This row was internally created. Delete it. */
         L7MIB_EXTRACTDELETE(p, _ospfNbrTable_);
      }
      else
      {
         /* This is an NBMA neighbor, created via SNMP. */
         /* Indicate that its owner object is deleted.  */
         p->owner = NULL;
      }
   }

   return E_OK;

} /* End of ospfNbrTable_DeleteInternal procedure */


/*********************************************************************
 * @purpose                Get an entry in the ospfNbrTable.
 *
 *
 * @param  ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  nbrIpAddr       @b{(input)}  the neighbor IP address
 * @param  nbrAddrLessId   @b{(input)}  ifIndex (for interfaces having no
 *                                                                IP addr)
 *
 * @returns                pointer to the appropriate entry of ospfNbrTable
 *                         on success
 * @returns                NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfNbrTable_Lookup(t_Handle ospfMibObj, ulng nbrIpAddr,
                             ulng nbrAddrLessId)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId);

   return p;
}



/*********************************************************************
 * @purpose                   Get the next entry in the ospfNbrTable.
 *
 *
 * @param  ospfMibObj         @b{(input)}  t_Handle of the OSPF MIB object
 * @param  prevNbrIpAddr      @b{(input)}  the interface IP address
 * @param  prevNbrAddrLessId  @b{(input)}  ifIndex (for interfaces having
 *                                                            no IP addr)
 *
 * @param  p_NbrIpAddr        @b{(output)}  the interface IP address
 * @param  p_NbrAddrLessId    @b{(output)}  ifIndex
 *
 * @returns      pointer to the next (after the previous) entry in the
 *               ospfNbrTable and the next entry indexes on succes
 * @returns      NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfNbrTable_GetNext(t_Handle ospfMibObj, ulng prevNbrIpAddr,
                              ulng prevNbrAddrLessId, ulng *p_NbrIpAddr,
                              ulng *p_NbrAddrLessId)
{
   ospfNbrEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfNbrTable_ == 0)
      return NULLP;

   next = (ospfNbrEntry *)OLL_FindNext(_ospfNbrTable_, 
                                       &prevNbrIpAddr, &prevNbrAddrLessId);
   if(next)
   {
      *p_NbrIpAddr = next->ospfNbrIpAddres;
      *p_NbrAddrLessId = next->ospfNbrAddrLessIf;
      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}


/*********************************************************************
 * @purpose                  Get the first entry in the ospfNbrTable.
 *
 *
 * @param  ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 *
 * @param  p_NbrIpAddr       @b{(output)}  the interface IP address
 * @param  p_NbrAddrLessId   @b{(output)}  ifIndex
 *
 * @returns       pointer to the first (after the previous) entry in the
 *                ospfNbrTable and the first entry indexes on success
 * @returns       NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfNbrTable_GetFirst(t_Handle ospfMibObj, ulng *p_NbrIpAddr,
                               ulng *p_NbrAddrLessId)
{
   ospfNbrEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfNbrTable_ == 0)
      return NULLP;

   first = (ospfNbrEntry *)OLL_GetNext(_ospfNbrTable_, NULLP);
   if(first)
   {
      *p_NbrIpAddr     = first->ospfNbrIpAddres;
      *p_NbrAddrLessId = first->ospfNbrAddrLessIf;
      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}


/*********************************************************************
 * @purpose       Test the parameters in a row of the ospfNbrTable before
 *                setting them by SNMP agent.
 *
 *
 * @param lastOid     @b{(input)}  parameter offset in the row
 * @param newValue    @b{(input)}  value to set
 * @param p           @b{(input)}  pointer to ospfAreaRangeEntry
 *
 * @returns           E_OK     - success, value can be set
 * @returns           E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfNbrEntry  *p_nbrEntry = (ospfNbrEntry *)p;
   
   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfNbrTestTable, 
                            sizeof(ospfNbrTestTable)/
                            sizeof(ospfNbrTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   /* OSPF Neighbor table fields are "CREATE_READ" objects.         */
   /* Therefore they can be changed (set) only during conceptual row      */
   /* creation (i.e. before the row status becomes "active").             */
   /* Rowstatus are "READ_WRITE" objects, so they can be set at any time. */

   if (lastOid == ospfNbrTableRowStatusSubId)  
      return E_OK;
   else
      if((p_nbrEntry->info.rowStatus != notReady) &&
         (p_nbrEntry->info.rowStatus != notInService))
          return E_FAILED;

      return E_OK;   
}



/*********************************************************************
 * @purpose      Set new value for row status of the ospfNbrTable
 *               and check consistency of this value.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrIpAddr       @b{(input)}  the neighbor IP address
 * @param nbrAddrLessId   @b{(input)}  ifIndex (for interfaces having
 *                                                        no IP addr)
 * @param value           @b{(input)}  new value of the rowStatus
 *
 * @returns               E_OK      - success, value can be set
 * @returns               E_BADPARM - bad parameter(s)
 * @returns               E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_SetRowStatus(t_Handle ospfMibObj, ulng nbrIpAddr,
                                ulng nbrAddrLessId, ulng value)
{
   ospfNbrEntry *p;
   e_Err         rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfNbrTable_New(ospfMibObj, nbrIpAddr, nbrAddrLessId);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfNbrTable_New(ospfMibObj, nbrIpAddr, nbrAddrLessId)) != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfIfTable OLL. Delete the created row.          */
                  ospfNbrTable_Delete(ospfMibObj, nbrIpAddr, nbrAddrLessId);
                  return E_FAILED;
               }
               else 
               {
                  /* New row has been successfully created. Activate the row*/
                  if((rc = ospfNbrTable_Active(ospfMibObj, p)) != E_OK)
                  {
                     ospfNbrTable_Delete(ospfMibObj, nbrIpAddr, nbrAddrLessId);
                     return rc;
                  }
                  else
                  {
                     return E_OK;
                  }                     
               }

            } /* End of "if rc = ospfNbrTable_New != E_OK" */  

            break;

         default:              /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 

      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */ 

   /* An existing row  status changing. */
   /* --------------------------------- */
   /* Check whether this row was "SNMP created" or "protocol created". */
   /* Protocol created neighbor row cannot be changed by SNMP.         */
   if (p->info.permanence != permanent)
      return E_FAILED;

   if ((e_RowStatus)value == p->info.rowStatus) 
      return E_OK;

   /*Check the requested row status against the existing */
   /* one and perform appropriate actions.               */

   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
         {
            return ospfNbrTable_Active(ospfMibObj, p);
         }         
         break;

      case notInService:
      case notReady:
         if (p->info.rowStatus == active)
            ospfNbrTable_Deactive(ospfMibObj, p);
      
         p->info.rowStatus = value;
         return E_OK;

         break;

      case destroy:
         if (p->info.rowStatus == active)
            ospfNbrTable_Deactive(ospfMibObj, p);

         ospfNbrTable_Delete(ospfMibObj, nbrIpAddr, nbrAddrLessId);
         return E_OK;

         break;

      default:  
         /* createAndGo, createAndWait are incorrect, because the row */
         /* already exist */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfNbrTable_SetRowStatus procedure */


/*********************************************************************
 * @purpose           Write OSPF neighbor object handle into the entry
 *                    of the ospfNbrTable.
 *
 *
 * @param ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrIpAddr        @b{(input)}  the neighbor IP address
 * @param nbrAddrLessId    @b{(input)}  ifIndex of the interface connecting
 *                                      the neighbor with the router
 * @param nbrObj           @b{(input)}  t_handle of the neighbor object
 *
 * @returns                E_OK            - Success
 * @returns                Not E_OK - Failure.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_SetOwner (t_Handle ospfMibObj, ulng nbrIpAddr,
                             ulng nbrAddrLessId, t_Handle nbrObj)
{
   ospfNbrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId);
   if(!p)
      return E_NOT_FOUND;
   p->owner = nbrObj;
   return E_OK;
}



/*********************************************************************
 * @purpose       Get OSPF neighbor object handle from the entry in the
 *                ospfNbrTable.
 *
 *
 * @param ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrIpAddr        @b{(input)}  the neighbor IP address
 * @param nbrAddrLessId    @b{(input)}  ifIndex of the interface connecting
 *                                      the neighbor with the router
 *
 * @returns                OSPF area handle - Success
 * @returns                NULLP            - Otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfNbrTable_GetOwner(t_Handle ospfMibObj, ulng nbrIpAddr,
                               ulng nbrAddrLessId)
{
   ospfNbrEntry *p;

   if (!ospfMibObj)
      return NULLP;

   p = (ospfNbrEntry *)OLL_Find(_ospfNbrTable_, &nbrIpAddr, &nbrAddrLessId);
   return p ? p->owner : NULLP;
}


/*********************************************************************
 * @purpose                 Activate an entry/row in the ospfNbrTabl.
 *
 *
 * @param  ospfMibObj       @b{(input)}    t_Handle of the OSPF MIB object
 *                                         (to be passed to ospfFindIf
 *                                                EntryFromIfIndex routine).
 * @param  p                @b{(input)}    pointer to an appropriate entry of the
 *                                         table
 *
 * @returns                E_OK     - the row has been activated successfully
 * @returns                E_FAILED - otherwise
 *
 * @notes         i.e. add
 *                a new entry to neighbor list of an appropriate
 *                interface.The procedure is designed for activating
 *                SNMP created neighbor row.
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_Active (t_Handle ospfMibObj, ospfNbrEntry *p)
{
   ospfIfEntry   *p_ifEntry;
   t_S_IfNbmaCfg  nbmaCfg;

   if (!p)
      return E_FAILED;

   /* Find ospfIfTable entry defined the interface connecting the neighbor*/
   /* to the router. */
   if (! (p_ifEntry = ospfFindIfEntryFromIfIndex(ospfMibObj, p->ospfNbrAddrLessIf)))
      return E_FAILED;

   /* Check the type of the interface. This neighbor is SNMP created, */
   /* so the only correct interface type for this neighbor is NBMA.   */
   if ( p_ifEntry->info.ifType != IFO_NBMA)
      return E_FAILED;

   /*Check if the interface row is active (i.e. interface object was created)*/
   if(p_ifEntry->info.rowStatus != active)
      return E_FAILED;

   /* Fill neighbor configuration fields and call IFO_NbmaConfig proc.*/
   /* to create a new entry to the interface neighbor hash list       */
   nbmaCfg.IpAdr     = p->ospfNbrIpAddres;
   nbmaCfg.RouterId  = p->info.rtrId;
   nbmaCfg.CanBeDr   = p->info.priority;      
   nbmaCfg.PrevEntry = 0;             /* used by ospf protocol code; */
                                      /* must be set to 0            */
   nbmaCfg.NbmaStatus = ROW_CREATE_AND_WAIT;

   if (p_ifEntry->owner)
   {
      /* "Hello" message won't be sent if the i/f is not restarted */
      /* (IFO_Down and IFO_Up) after the new neighbor is added.    */
      if (IFO_Down(p_ifEntry->owner) != E_OK)
         return E_FAILED;

      IFO_NbmaConfig (p_ifEntry->owner, &nbmaCfg);

      if (IFO_Up(p_ifEntry->owner) != E_OK)
         return E_FAILED;
   }

   p->info.rowStatus = active;
   return E_OK;
     
} /*  End of ospfNbrTable_Active procedure */




/*********************************************************************
 * @purpose               Deactivate an entry/row in the ospfNbrTabl.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param p               @b{(input)}  pointer to an appropriate entry of
 *                                     the table
 *
 * @returns               E_OK     - the row has been activated successfully
 * @returns               E_FAILED - otherwise
 *
 * @notes
 *              i.e. delete the neighbor from the neighbor list of an
 *                   appropriate interface.
 *
 * @end
 * ********************************************************************/
e_Err ospfNbrTable_Deactive (t_Handle ospfMibObj, ospfNbrEntry *p)
{
   ospfIfEntry   *p_ifEntry;
   t_S_IfNbmaCfg  nbmaCfg;
   Bool           ifSearchStop = FALSE;
   ulng           nilIfIpAddr  = 0;

   if (!ospfMibObj)
      return NULLP;

   if (!p)
      return E_OK;

   /* Find ospfIfTable entry defined the interface connecting the neighbor */
   /* to the router. The ifIndex is the second key in the ospfIfTable OLL. */
   if((p_ifEntry = OLL_Find (_ospfIfTable_, &nilIfIpAddr, &(p->ospfNbrAddrLessIf))))
      ifSearchStop = TRUE;
   else 
      p_ifEntry = OLL_FindNext(_ospfIfTable_, &nilIfIpAddr, &(p->ospfNbrAddrLessIf));
   
   while (!ifSearchStop)
   {
      if (p_ifEntry)
      {   
         if (p_ifEntry->ospfAddrLessIf >= p->ospfNbrAddrLessIf)
             ifSearchStop = TRUE;
         else
            p_ifEntry = OLL_GetNext(_ospfIfTable_, p_ifEntry);
      }
      else  /* There is no next element in the list. Stop searching.*/
         ifSearchStop = TRUE;
   } /* End of while stmt  */

   /* Check if the interface entry has been found and it is active. */
   /* If not - don't need do anything. */
   if(p_ifEntry &&
     (p_ifEntry->ospfAddrLessIf == p->ospfNbrAddrLessIf) &&
     (p_ifEntry->info.rowStatus == active))
   {
      /* Remove the neighbor form the interface neighbor hash list */
      nbmaCfg.RouterId   = p->info.rtrId;
      nbmaCfg.NbmaStatus = ROW_DESTROY;
      IFO_NbmaConfig (p_ifEntry->owner, &nbmaCfg);
   }

    /* We always return OK  */
    return E_OK;

} /*  End of ospfNbrTable_Deactive procedure */


/*********************************************************************
 * @purpose       This is internal utility that searches ospfIfTable
 *                and looks for an entry having the second key equal
 *                to the given ifIndex.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  ifIndex       @b{(input)}  interface index
 *
 * @returns              pointer to an appropriate entry of ospfIfTable
 * @returns              NULLP on failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ospfIfEntry *ospfFindIfEntryFromIfIndex (t_Handle ospfMibObj, ulng ifIndex)
{
   ospfIfEntry  *p_ifEntry;
   Bool          SearchStop = FALSE;

   if (!ospfMibObj)
      return NULLP;

   p_ifEntry = (ospfIfEntry *)OLL_GetNext (_ospfIfTable_, NULLP);

   /*   ulng          nilIfIpAddr;  */

/*   if (!(p_ifEntry = (ospfIfEntry *)OLL_GetNext (_ospfIfTable_, NULLP)))
      SearchStop = TRUE;
   else 
      p_ifEntry = OLL_FindNext(_ospfIfTable_, &nilIfIpAddr, &ifIndex); */
   
   while (!SearchStop)
   {
      if (p_ifEntry)
      {   
         if (p_ifEntry->ospfAddrLessIf >= ifIndex)
             SearchStop = TRUE;
         else
            p_ifEntry = OLL_GetNext(_ospfIfTable_, p_ifEntry);
      }
      else
         /* There is no next element in the list. Stop searching. */
         SearchStop = TRUE;
   }

   /* Check if the interface entry has been found */
   if(p_ifEntry && (p_ifEntry->ospfAddrLessIf == ifIndex))
      return p_ifEntry;

   /* if we get here the entry has not been found */
      return NULLP;

}  /* End of ospfFindIfEntryFromIfIndex procedure */




/*********************************************************************
 * @purpose       Create a new row in the OSPF Virtual Neighbor Table.
 *
 *
 * @param ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrAreaIpAddr    @b{(input)}  the transit area identifier
 *                                      (IP address)
 * @param nbrRtrIpAddr     @b{(input)}  neighboring router in the autonomous system
 *                                      (IP router)
 * @param owner            @b{(input)}
 *
 * @returns                E_OK       - success
 * @returns                E_IN_MATCH - entry already exists
 * @returns                E_BADPARM  - invalid parameter
 * @returns                E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtNbrTable_NewInternal(t_Handle ospfMibObj, ulng nbrAreaIpAddr,
                                   ulng nbrRtrIpAddr, t_Handle owner)
{
   ospfVirtNbrEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if((!nbrAreaIpAddr ) || (!nbrRtrIpAddr))
      return E_FAILED;

   if(_ospfVirtNbrTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfVirtNbrTable_, &nbrAreaIpAddr, &nbrRtrIpAddr))
         return E_IN_MATCH;
   }
   else
   {
      _ospfVirtNbrTable_ = OLL_New(0,                  /* do not use DA */
                       2,                              /* number of keys */
                       (ulng)offsetof(ospfVirtNbrEntry, ospfVirtNbrArea), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfVirtNbrEntry, ospfVirtNbrRtrId), /*key offset*/
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfVirtNbrEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfVirtNbrEntry));

   /* Set the key values into the new entry */
   p->ospfVirtNbrArea  = nbrAreaIpAddr;
   p->ospfVirtNbrRtrId = nbrRtrIpAddr;
   p->owner            = owner;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfVirtNbrTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }
   
   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfVirtNbrTable_NewInternal procedure */



/*********************************************************************
 * @purpose       Delete a row from the OSPF Virtual Neighbor MIB Table.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrAreaIpAddr     @b{(input)}  the transit area identifier
 *                                       (IP address)
 * @param nbrRtrIpAddr      @b{(input)}  neighboring router in the autonomous
 *                                       system (IP router)
 *
 * @returns                 E_OK     - Success
 * @returns                 Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtNbrTable_Delete(t_Handle ospfMibObj, ulng nbrAreaIpAddr,
                              ulng nbrRtrIpAddr)
{
   ospfVirtNbrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfVirtNbrEntry *)OLL_Find(_ospfVirtNbrTable_, 
                                              &nbrAreaIpAddr, &nbrRtrIpAddr);

   if (p)
   {   
      L7MIB_EXTRACTDELETE(p, _ospfVirtNbrTable_);
   }
   return E_OK;

} /* End of ospfVirtNbrTable_Delete procedure */


/*********************************************************************
 * @purpose               Get an entry in the ospfVirtNbrTable.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  nbrAreaIpAddr  @b{(input)}  the transit area identifier (IP address)
 * @param  nbrRtrIpAddr   @b{(input)}  neighboring router in the autonomous system
 *                                     (IP router)
 *
 * @returns               pointer to the appropriate entry of ospfVirtNbrTable
 *                        on success
 * @returns               NULLP - otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtNbrTable_Lookup(t_Handle ospfMibObj, ulng nbrAreaIpAddr,
                                 ulng nbrRtrIpAddr)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfVirtNbrTable_, &nbrAreaIpAddr, &nbrRtrIpAddr);

   return p;
}


/*********************************************************************
 * @purpose                  Get the next entry in the ospfVirtNbrTable.
 *
 *
 * @param  ospfMibObj           @b{(input)}  t_Handle of the OSPF MIB object
 * @param  prevVirtNbrAreaId    @b{(input)}  the interface IP address
 * @param  prevVirtNbrRtrId     @b{(input)}  ifIndex (for interfaces having
 *                                                              no IP addr)
 *
 * @param  p_VirtNbrAreaId      @b{(input)}  the i/f IP address
 * @param  p_VirtNbrRtrId       @b{(output)} ifIndex
 *
 * @returns    pointer to the next (after the previous) entry in the
 *             ospfVirtNbrTable on success
 * @returns    NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtNbrTable_GetNext(t_Handle ospfMibObj, ulng prevVirtNbrAreaId,
                                  ulng prevVirtNbrRtrId, ulng *p_VirtNbrAreaId,
                                  ulng *p_VirtNbrRtrId)
{
   ospfVirtNbrEntry *next;

   if ((!ospfMibObj) || (!p_VirtNbrAreaId) || (!p_VirtNbrRtrId))
      return NULLP;

   if(_ospfVirtNbrTable_ == 0)
      return NULLP;

   next = (ospfVirtNbrEntry *)OLL_FindNext(_ospfVirtNbrTable_, 
                                       &prevVirtNbrAreaId, &prevVirtNbrRtrId);
   if(next)
   {
      *p_VirtNbrAreaId = next->ospfVirtNbrArea;
      *p_VirtNbrRtrId  = next->ospfVirtNbrRtrId;
 
      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}


/*********************************************************************
 * @purpose                  Get the first entry in the ospfVirtNbrTable.
 *
 *
 * @param  ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 *
 * @param  p_VirtNbrAreaId   @b{(output)}  the i/f IP address
 * @param  p_VirtNbrRtrId    @b{(output)}  ifIndex
 *
 * @returns     pointer to the firs entry in the ospfVirtNbrTable on success
 * @returns     NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtNbrTable_GetFirst(t_Handle ospfMibObj, ulng *p_VirtNbrAreaId,
                                   ulng *p_VirtNbrRtrId)
{
   ospfVirtNbrEntry *first;

   if ((!ospfMibObj) || (!p_VirtNbrAreaId) || (!p_VirtNbrRtrId))
      return NULLP;

   if(_ospfVirtNbrTable_ == 0)
      return NULLP;

   first = (ospfVirtNbrEntry *)OLL_GetNext(_ospfVirtNbrTable_, NULLP);

   if(first)
   {
      *p_VirtNbrAreaId = first->ospfVirtNbrArea;
      *p_VirtNbrRtrId  = first->ospfVirtNbrRtrId;
 
      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}



 /*********************************************************************
 * @purpose      Write OSPF neighbor object handle into the entry of the
 *               ospfVirtNbrTable.
 *
 *
 * @param  ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param  nbrAreaIpAddr   @b{(input)}  the virt. neighbor area IP address
 * @param  nbrRtrIpAddr    @b{(input)}  the neighbor router IP address
 * @param  nbrObj          @b{(input)}  t_handle of the neighbor object
 *
 * @returns                E_OK            - Success
 * @returns                Not E_OK - Failure.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfVirtNbrTable_SetOwner (t_Handle ospfMibObj, ulng nbrAreaIpAddr,
                                 ulng nbrRtrIpAddr, t_Handle nbrObj)
{
   ospfVirtNbrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfVirtNbrEntry *)OLL_Find(_ospfVirtNbrTable_,
                                     &nbrAreaIpAddr, &nbrRtrIpAddr);
   if(!p)
      return E_NOT_FOUND;

   p->owner = nbrObj;
   return E_OK;
}



/*********************************************************************
 * @purpose       Get OSPF neighbor object handle from the entry in the
 *                ospfVirtNbrTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param nbrAreaIpAddr     @b{(input)}  the virt. neighbor area IP address
 * @param nbrRtrIpAddr      @b{(input)}  the neighbor router IP address
 *
 * @returns                 OSPF area handle - Success
 * @returns                 NULLP            - Otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfVirtNbrTable_GetOwner(t_Handle ospfMibObj, ulng nbrAreaIpAddr,
                                   ulng nbrRtrIpAddr)
{
   ospfVirtNbrEntry *p;

   if (!ospfMibObj)
      return NULLP;

   p = (ospfVirtNbrEntry *)OLL_Find(_ospfVirtNbrTable_, &nbrAreaIpAddr,
                                    &nbrRtrIpAddr);
   return p ? p->owner : NULLP;
}

/*********************************************************************
 * @purpose       Create a new row in the OSPF Area Aggregate Table.
 *
 *
 * @param  ospfMibObj       @b{(input)}  t_Handle of the OSPF MIB object
 * @param  aggrAreaId       @b{(input)}  the area the address aggregate is to be
 *                                       found within
 * @param  aggrLsdbType     @b{(input)}  the type of address aggregate; could be:
 *                                       summaryLink (3) or nssaExternalLink (7)
 * @param  aggrNet          @b{(input)}  IP address of the Net or Subnet indicated
 *                                       by the range
 * @param  aggrMask         @b{(input)}  Subnet mask
 *
 * @returns                 E_OK       - success
 * @returns                 E_IN_MATCH - entry already exists
 * @returns                 E_BADPARM  - invalid parameter
 * @returns                 E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_New(t_Handle ospfMibObj, ulng aggrAreaId,
                            ulng aggrLsdbType, ulng aggrNet, ulng aggrMask)
{
   ospfAreaAggrEntry  *p;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Check indexes consistency */
   if((!aggrLsdbType) || (!aggrNet))
      return E_FAILED;

   if(_ospfAreaAggrTable_ != 0)
   {
      /* check if the table entry already exists */
      if(OLL_Find(_ospfAreaAggrTable_, &aggrAreaId, &aggrLsdbType, &aggrNet, &aggrMask))
         return E_IN_MATCH;

      /* Check if exist entry with the same areaId and netId as the new ones */
      p = OLL_FindNext(_ospfAreaAggrTable_, &aggrAreaId, (ulng *)-1);
      while(p && (p->ospfAreaAggrAreaId == aggrAreaId))
      {
         if (p->ospfAreaAggrNet == aggrNet)
            /* Row with the index pair areaId/NetId id already exists, */
            /* so the new row cannot be added */
            return E_FAILED;

          p = OLL_GetNext(_ospfAreaAggrTable_, p);
      }
   }
   else
   {
      _ospfAreaAggrTable_ = OLL_New(0,                 /* do not use DA */
                       4,                              /* number of keys */
                       (ulng)offsetof(ospfAreaAggrEntry, ospfAreaAggrAreaId), /* key offset */
                       (ulng)sizeof(ulng),
                       (ulng)offsetof(ospfAreaAggrEntry, ospfAreaAggrLsdbType), /* key offset */
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfAreaAggrEntry, ospfAreaAggrNet),  /*key offset*/
                       (ulng)sizeof(ulng),             /* key length */
                       (ulng)offsetof(ospfAreaAggrEntry, ospfAreaAggrMask),
                       (ulng)sizeof(ulng));            /* key length */
   }

   /* Allocate memory for the new entry of the table */
   p = XX_Malloc(sizeof(ospfAreaAggrEntry));
   if( !p )
      return E_NOMEMORY;

   /* Initialize the new entry */
   memset( p, 0, sizeof(ospfAreaAggrEntry));

   /* Set the key values into the new entry */
   p->ospfAreaAggrAreaId   = aggrAreaId;
   p->ospfAreaAggrLsdbType = aggrLsdbType;
   p->ospfAreaAggrNet      = aggrNet;
   p->ospfAreaAggrMask     = aggrMask;

   /* Add a new entry to the ordered linked list */
   if( OLL_Insert(_ospfAreaAggrTable_, (void *)p ) != E_OK )
   {
      XX_Free(p);
      return E_FAILED;
   }

   /* Fill in the ebtry by init values. */
   p->info.rowStatus       = notReady;
   p->info.effect          = advertiseMatching;
   p->info.internalCreated = FALSE;
   p->info.SNMPdisable     = FALSE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfAreaAggrTable_New procedure */



/*********************************************************************
 * @purpose       Create a new row in the OSPF Area Aggregate Table.
 *                This routine is to be called from OSPF protocol code only.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  aggrAreaId     @b{(input)}  the area the address aggregate is to be
 *                                     found within
 * @param  aggrLsdbType   @b{(input)}  the type of address aggregate; could be:
 *                                     summaryLink (3) or nssaExternalLink (7)
 * @param  aggrNet        @b{(input)}  IP address of the Net or Subnet indicated
 *                                     by the range
 * @param  aggrMask       @b{(input)}  Subnet mask
 * @param  effect         @b{(input)}  advertise or don't advertize the matching
 *
 * @returns               E_OK       - success
 * @returns               E_IN_MATCH - entry already exists
 * @returns               E_BADPARM  - invalid parameter
 * @returns               E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_InternalNewAndActive(t_Handle ospfMibObj,
                                             ulng aggrAreaId,
                                             ulng aggrLsdbType, ulng aggrNet,
                                             ulng aggrMask,
                                             e_ospfAreaAggrEffect effect)
{
   e_Err              e;
   ospfAreaAggrEntry *p;

   e = ospfAreaAggrTable_New(ospfMibObj, aggrAreaId, aggrLsdbType, aggrNet,
                             aggrMask);

   if (e == E_IN_MATCH)
      return E_OK;

   if (e != E_OK)
      return e;

   p = (ospfAreaAggrEntry *)OLL_Find(_ospfAreaAggrTable_, &aggrAreaId, 
                                      &aggrLsdbType, &aggrNet, &aggrMask);

   if (!p)
      return E_FAILED;

   /* Fill in the correct values. */
   p->info.effect    = effect;
   p->info.rowStatus = active;

   p->info.internalCreated = TRUE;

   /* If we get here, everything is OK */
   return E_OK;

}  /* End of ospfAreaAggrTable_InternalNewAndActive procedure */




/*********************************************************************
 * @purpose       Delete a row from the OSPF Area Aggregate MIB Table.
 *                This routine is to be called from SNMP code only.
 *
 *
 * @param  ospfMibObj    @b{(input)}  t_Handle of the OSPF MIB object
 * @param  aggrAreaId    @b{(input)}  the area the address aggregate is
 *                                    to be found within
 * @param  aggrLsdbType  @b{(input)}  the type of address aggregate
 * @param  aggrNet       @b{(input)}  IP address of the Net or Subnet
 *                                    indicated by the range
 * @param  aggrMask      @b{(input)}  Subnet mask
 *
 * @returns              E_OK     - Success
 * @returns              Not E_OK - Failure
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_Delete(t_Handle ospfMibObj, ulng aggrAreaId,
                               ulng aggrLsdbType, ulng aggrNet, ulng aggrMask)
{
   ospfAreaAggrEntry *p;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfAreaAggrEntry *)OLL_Find(_ospfAreaAggrTable_, &aggrAreaId, 
                                      &aggrLsdbType, &aggrNet, &aggrMask);

   /* internally created objects can't be deleted via SNMP */
   if (p)
   {
      if (p->info.internalCreated == TRUE)
         return E_FAILED;
      L7MIB_EXTRACTDELETE(p, _ospfAreaAggrTable_)
   }

   return E_OK;

} /* End of ospfAreaAggrTable_Delete procedure */



/*********************************************************************
 * @purpose                 Get an entry in the ospfAreaAggrTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param aggrAreaId        @b{(input)}  the area the address aggregate
 *                                       is to be found within
 * @param aggrLsdbType      @b{(input)}  the type of address aggregate
 * @param aggrNet           @b{(input)}  IP address of the Net or Subnet
 *                                       indicated by the range
 * @param aggrMask          @b{(input)}  Subnet mask
 *
 * @returns                 pointer to the appropriate entry of
 *                          ospfAreaAggrTable on success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaAggrTable_Lookup(t_Handle ospfMibObj, ulng aggrAreaId,
                                  ulng aggrLsdbType, ulng aggrNet,
                                  ulng aggrMask)
{
   t_Handle p = NULLP;

   if (!ospfMibObj)
      return NULLP;

   p = OLL_Find(_ospfAreaAggrTable_, &aggrAreaId, &aggrLsdbType, &aggrNet,
                &aggrMask);

   return p;
}



/*********************************************************************
 * @purpose       Get the next entry in the ospfAreaAggrTable.
 *
 *
 * @param ospfMibObj        @b{(input)}  t_Handle of the OSPF MIB object
 * @param prevAggrAreaId    @b{(input)}  the area the address aggregate
 *                                       is to be found within
 * @param prevAggrLsdbType  @b{(input)}  the type of address aggregate
 * @param prevAggrNet       @b{(input)}  IP address of the Net or Subnet
 *                                       indicated by the range
 * @param prevAggrMask      @b{(input)}  Subnet mask
 *
 * @param p_AggrAreaId      @b{(output)}  the area the address aggregate
 *                                          is to be found within
 * @param p_AggrLsdbType    @b{(output)}  the type of address aggregate
 * @param p_AggrNet         @b{(output)}  IP address of the Net or Subnet
 *                                        indicated by the range
 * @param p_AggrMask        @b{(output)}  Subnet mask
 *
 * @returns      pointer to the next (after the previous) entry in the
 *               ospfAreaAggrTable on success
 * @returns      NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaAggrTable_GetNext(t_Handle ospfMibObj, ulng prevAggrAreaId,
                                   ulng prevAggrLsdbType, ulng prevAggrNet,
                                   ulng prevAggrMask, ulng *p_AggrAreaId,
                                   ulng *p_AggrLsdbType, ulng *p_AggrNet,
                                   ulng *p_AggrMask)
{
   ospfAreaAggrEntry *next;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfAreaAggrTable_ == 0)
      return NULLP;

   next = (ospfAreaAggrEntry *)OLL_FindNext(_ospfAreaAggrTable_,
                                            &prevAggrAreaId, &prevAggrLsdbType,
                                            &prevAggrNet, &prevAggrMask);
   if(next)
   {
      *p_AggrAreaId   = next->ospfAreaAggrAreaId;
      *p_AggrLsdbType = next->ospfAreaAggrLsdbType;
      *p_AggrNet      = next->ospfAreaAggrNet;
      *p_AggrMask     = next->ospfAreaAggrMask;
 
      return (t_Handle)next;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}




/*********************************************************************
 * @purpose                Get the first entry in the ospfAreaAggrTable.
 *
 *
 * @param ospfMibObj       @b{(input)}   t_Handle of the OSPF MIB object
 *
 * @param p_AggrAreaId     @b{(output)}  the area the address aggregate
 *                                       is to be found within
 * @param p_AggrLsdbType   @b{(output)}  the type of address aggregate
 * @param p_AggrNet        @b{(output)}  IP address of the Net or Subnet
 *                                       indicated by the range
 * @param p_AggrMask       @b{(output)}  Subnet mask
 *
 * @returns                pointer to the first entry in the
 *                         ospfAreaAggrTable on success
 * @returns                NULL otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle ospfAreaAggrTable_GetFirst(t_Handle ospfMibObj, ulng  *p_AggrAreaId,
                                    ulng *p_AggrLsdbType, ulng *p_AggrNet,
                                    ulng *p_AggrMask)
{
   ospfAreaAggrEntry *first;

   if (!ospfMibObj)
      return NULLP;

   if(_ospfAreaAggrTable_ == 0)
      return NULLP;

   first = (ospfAreaAggrEntry *)OLL_GetNext(_ospfAreaAggrTable_, NULLP);
   if(first)
   {
      *p_AggrAreaId   = first->ospfAreaAggrAreaId;
      *p_AggrLsdbType = first->ospfAreaAggrLsdbType;
      *p_AggrNet      = first->ospfAreaAggrNet;
      *p_AggrMask     = first->ospfAreaAggrMask;
 
      return (t_Handle)first;      
   }

   /* If we've reached here, next entry of the table was not found */
   return NULLP;
}



/*********************************************************************
 * @purpose       Test the parameters in a row of the ospfAreaAggrTable
 *                before setting them by SNMP agent.
 *
 *
 * @param lastOid      @b{(input)}  parameter offset in the row
 * @param newValue     @b{(input)}  value to set
 * @param p            @b{(input)}  pointer to ospfAreaAggrEntry
 *
 * @returns            E_OK     - success, value can be set
 * @returns            E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_SetTest(word lastOid, ulng newValue, t_Handle p)
{
   ospfAreaAggrEntry *p_areaAggrEntry = (ospfAreaAggrEntry *)p;
   
   /* The first verification: the new value of the object is in range */
   if (!CheckValueInRange(ospfAreaAggrTestTable, 
                            sizeof(ospfAreaAggrTestTable)/
                            sizeof(ospfAreaAggrTestTable[0]),
                            lastOid, newValue))
      return E_FAILED;
   
   /* OSPF Neighbor table fields are "CREATE_READ" objects.               */
   /* Therefore they can be changed (set) only during conceptual row      */
   /* creation (i.e. before the row status becomes "active").             */
   /* Rowstatus are "READ_WRITE" objects, so they can be set at any time. */

   if (lastOid == ospfAreaAggrTableRowStatusSubId)
   {
      /* row status of internally created objects can't be changed via SNMP */
      if (p && (p_areaAggrEntry->info.internalCreated == TRUE))
         return E_FAILED;
      return E_OK;
   }

   /* All other parameters can be changed "on the fly". */
   return E_OK;
}



/*********************************************************************
 * @purpose       Set new value for row status of the ospfAreaAggrTable
 *                and check consistency of this value.
 *
 *
 * @param ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param aggrAreaId     @b{(input)}  the area the address aggregate is
 *                                    to be found within
 * @param aggrLsdbType   @b{(input)}  the type of address aggregate
 * @param aggrNet        @b{(input)}  IP address of the Net or Subnet
 *                                    indicated by the range
 * @param aggrMask       @b{(input)}  Subnet mask
 * @param value          @b{(input)}  new value of the rowStatus
 *
 * @returns              E_OK      - success, value can be set
 * @returns              E_BADPARM - bad parameter(s)
 * @returns              E_FAILED  - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_SetRowStatus(t_Handle ospfMibObj, ulng aggrAreaId,
                                     ulng aggrLsdbType, ulng aggrNet,
                                     ulng aggrMask, ulng value)
{
   ospfAreaAggrEntry *p;
   e_Err              rc = E_FAILED;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfAreaAggrEntry *)OLL_Find(_ospfAreaAggrTable_, &aggrAreaId, &aggrLsdbType, 
                                &aggrNet, &aggrMask);

   /* If entry has not been found, the required (new) row status can be   */
   /* only "createAndWait" or "createAndGo".  I.e. SNMP manager wants to  */
   /* create new conceptional row in the table.                           */
   if(!p)
   {
      switch(value)
      {
         case createAndWait:     /* Create a new row */
            return ospfAreaAggrTable_New(ospfMibObj, aggrAreaId, aggrLsdbType,
                                         aggrNet, aggrMask);

            break;
         case createAndGo:       /* Create a new row and activate it */
            if((rc = ospfAreaAggrTable_New(ospfMibObj, aggrAreaId, aggrLsdbType, 
                                          aggrNet, aggrMask)) != E_OK)
               return rc;       
            else
            {
               /* Find ptr to the new row created above */
               p = (ospfAreaAggrEntry *)OLL_Find(_ospfAreaAggrTable_, &aggrAreaId,
                                                 &aggrLsdbType, &aggrNet, &aggrMask);
               if (!p)
               {
                  /* Something wrong occured: the row was successfully */
                  /* created, but has not been found into the          */
                  /* ospfIfTable OLL. Delete the created row.          */
                  ospfAreaAggrTable_Delete(ospfMibObj, aggrAreaId, aggrLsdbType, 
                                           aggrNet, aggrMask);
                  return E_FAILED;
               }
               else 
               {
                  /* New row has been successfully created. */ 
                  /* Activate the row */
                  if((rc = ospfAreaAggrTable_Active(ospfMibObj, p)) != E_OK)
                  {
                     ospfAreaAggrTable_Delete(ospfMibObj, aggrAreaId, aggrLsdbType, 
                                             aggrNet, aggrMask);
                     return rc;
                  }
                  else
                  {
                     return E_OK;
                  }                     
               }

            } /* End of "if rc = ospfAreaAggrTable_New != E_OK" */  
            
            break;

         default:               /* Any other value of rowStatus is impossible */
                               /* for inexisting row.                        */
            return E_FAILED;   /* SNMP agent has to return inconsistentValue */ 
                               
      } /* End of switch command for the case when the row does not exist */
   }  /* End of if(!p) command; i.e. end of a new row processing */


   /* An existing row  status changing. Check the requested row status */
   /* against the existing one and perform appropriate actions.        */
   if ((e_RowStatus)value == p->info.rowStatus) 
      return E_OK;

   switch(value)
   {
      case active:
         if(p->info.rowStatus == notInService || p->info.rowStatus == notReady)
         {
            return ospfAreaAggrTable_Active(ospfMibObj, p);
         }         
         break;
  
      case notReady:
      case notInService:
         if (p->info.rowStatus == active)
            ospfAreaAggrTable_Deactive(ospfMibObj, p);

         p->info.rowStatus = value;
         return E_OK;

         break;

      case destroy:
         if (p->info.rowStatus == active)
            ospfAreaAggrTable_Deactive(ospfMibObj, p);

         ospfAreaAggrTable_Delete(ospfMibObj, aggrAreaId, aggrLsdbType, aggrNet, aggrMask);
         return E_OK;

         break;

      default:  
         /* notReady, createAndGo, createAndWait are incorrect */
         /* requested row statuses */
         return E_FAILED;  /* SNMP agent has to return inconsistentValue */

   } /* End of switch command in case a status of an existing row is changed */

   return E_FAILED;

} /* End of ospfAreaAggrTable_SetRowStatus procedure */




/*********************************************************************
 * @purpose            Activates ospfAreaAggrTable row.
 *
 *
 * @param ospfMibObj   @b{(input)} t_Handle of the OSPF MIB object
 * @param p            @b{(input)} pointer to an appropriate entry of
 *                                 the table
 *
 * @returns            E_OK     - success, value can be set
 * @returns            E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_Active (t_Handle ospfMibObj, ospfAreaAggrEntry *p)
{
   ospfAreaEntry            *p_areaEntry;
   t_S_AreaAggregateEntry    areaAggrCfg;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Double-checking */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Find the appropriate row in ospfAreaTable. If the area row exist */
   /* and active, we need to set address range into the area object.   */
   /* if not we cannot activate the area aggregate row */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_, 
                                          &(p->ospfAreaAggrAreaId));

   /* If the appropriated area is not active, we do nothing here.*/
   /* The aggr parameters will be bound to the area object later */ 
   /* during the area activation.*/
   if((!p_areaEntry) ||
      (p_areaEntry && !p_areaEntry->owner))
   {
       p->info.rowStatus = active;
       return E_OK;
   }
         
   /* Fill area aggregate config.structure */
   areaAggrCfg.AreaId          = p->ospfAreaAggrAreaId;
   areaAggrCfg.LsdbType        = (word)(p->ospfAreaAggrLsdbType);
   areaAggrCfg.NetIpAdr        = p->ospfAreaAggrNet;
   areaAggrCfg.SubnetMask      = p->ospfAreaAggrMask;
   areaAggrCfg.AggregateStatus = ROW_CREATE_AND_GO;
   areaAggrCfg.AggregateEffect = p->info.effect;

   /* Set area aggr parameter into the area object */
   if (ARO_AddressRange (p_areaEntry->owner, &areaAggrCfg) != E_OK)
      return E_FAILED;
   
   /* If we get here everithing is OK */
   p->info.rowStatus = active;
   return E_OK;

} /* End of ospfAreaAggrTable_Active */



/*********************************************************************
 * @purpose           Deactivates ospfAreaAggrTable row.
 *
 *
 * @param ospfMibObj  @b{(input)}  t_Handle of the OSPF MIB object
 * @param p           @b{(input)}  pointer to an appropriate entry of the
 *                                 table
 *
 * @returns           E_OK     - success, value can be set
 * @returns           E_FAILED - inconsistent value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err ospfAreaAggrTable_Deactive (t_Handle ospfMibObj, ospfAreaAggrEntry *p)
{
   ospfAreaEntry            *p_areaEntry;
   t_S_AreaAggregateEntry    areaAggrCfg;

   if (!ospfMibObj)
      return E_BADPARM;

   /* Double-checking */
   if (p->info.internalCreated == TRUE)
      return E_FAILED;

   /* Find the appropriate row in ospfAreaTable. If the area row exists    */
   /* and active, we "ask" the area object to remove area aggr information */
   /* from the area data structure, in not we don't need to do anything.   */
   p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_,
                                          &(p->ospfAreaAggrAreaId));
   if(p_areaEntry)
      if (p_areaEntry->owner)
      {
         /* Fill area aggregate config. structure and call ARO procedure. */
         /* The ARO routine performs the search by 8 bytes: net address   */
         /* and subnet mask.                                              */
         areaAggrCfg.NetIpAdr        = p->ospfAreaAggrNet;
         areaAggrCfg.SubnetMask      = p->ospfAreaAggrMask;
         areaAggrCfg.AggregateStatus = ROW_DESTROY;
         p->info.SNMPdisable         = TRUE;  /* don't delete the row */
         ARO_AddressRange (p_areaEntry->owner, &areaAggrCfg);
         p->info.SNMPdisable         = FALSE;
      }

   return E_OK;

}; /* End of ospfAreaAggrTable_Deactive */



 /*********************************************************************
 * @purpose      Update the Area Aggregate Table according to the 'command'
 *               parameter.
 *
 *
 * @param ospfMibObj      @b{(input)}  t_Handle of the OSPF MIB object
 * @param aggrAreaId      @b{(input)}  the area the address aggregate is to be found
 *                                     within
 * @param p_AdrRange      @b{(input)}  area range parameters structure, including the
 *                                     'command' parameter
 *
 * @returns        E_OK       - success
 * @returns        E_IN_MATCH - entry already exists
 * @returns        E_BADPARM  - invalid parameter
 * @returns        E_NOMEMORY - can't allocate memory
 *
 * @notes       either create new row, or update/destroy an
 *               existing one.This routine is to be called from OSPF protocol
 *               code only.
 *
 * @end
 *********************************************************************/
e_Err ospfAreaAggrTable_Update(t_Handle ospfMibObj, ulng aggrAreaId,
                               t_S_AreaAggregateEntry *p_AdrRange)
{
   ospfAreaAggrEntry *p            = NULL;
   ulng               aggrLsdbType = (ulng)p_AdrRange->LsdbType;

   if (!ospfMibObj)
      return E_BADPARM;

   p = (ospfAreaAggrEntry *)OLL_Find(_ospfAreaAggrTable_, &aggrAreaId,
                                     &aggrLsdbType,
                                     &p_AdrRange->NetIpAdr,
                                     &p_AdrRange->SubnetMask);

   switch (p_AdrRange->AggregateStatus)
   {
      case  ROW_ACTIVE:
      case  ROW_NOT_IN_SERVICE:
         /* do nothing */
      break;
      case  ROW_CREATE_AND_GO:
      case  ROW_CREATE_AND_WAIT:
         if (!p)
            ospfAreaAggrTable_InternalNewAndActive(ospfMibObj, aggrAreaId,
                                                   (ulng)p_AdrRange->LsdbType,
                                                   p_AdrRange->NetIpAdr,
                                                   p_AdrRange->SubnetMask,
                                                   p_AdrRange->AggregateEffect);
      break;
      case  ROW_CHANGE:
         if (!p)
            return E_FAILED;

         p->info.effect = p_AdrRange->AggregateEffect;

      break;                           
      case  ROW_READ:
      case  ROW_READ_NEXT:
         /* do nothing */
      break;
      case  ROW_DESTROY:
         if (p && (!p->info.SNMPdisable))
         {
            L7MIB_EXTRACTDELETE(p, _ospfAreaAggrTable_);
         }
      break;                           
      default:
         return E_BADPARM;
   }

   /* if we got here, everything is OK */
   return E_OK;
}

/*--------------------------------------------------------------------
 *
 *  Access procedures for read-only fields of all the OSPF MIB tables
 *
 *--------------------------------------------------------------------*/
ulng OSPFAREA_AUTHSPFRUNS(t_Handle p)
{
   ospfAreaEntry *p_Entry = (ospfAreaEntry *)p;
   t_S_AreaCfg    areaCfg;

   /* If area object exists retrive data from the area object.*/
   areaCfg.AreaStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (ARO_Config(p_Entry->owner, &areaCfg) == E_OK)
         return areaCfg.SpfRuns;

   return 0;
}

ulng OSPFAREA_BDRRTRCOUNT(t_Handle p)
{
   ospfAreaEntry *p_Entry = (ospfAreaEntry *)p;
   t_S_AreaCfg    areaCfg;

   /* If area object exists retrive data from the area object.*/
   areaCfg.AreaStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (ARO_Config(p_Entry ->owner, &areaCfg) == E_OK)
         return areaCfg.AreaBdrRtrCount; 

   return 0;
}

ulng OSPFAREA_LSACOUNT(t_Handle p)
{
   ospfAreaEntry *p_Entry = (ospfAreaEntry *)p;
   t_S_AreaCfg    areaCfg;

   /* If area object exists retrive data from the area object.*/
   areaCfg.AreaStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (ARO_Config(p_Entry->owner, &areaCfg) == E_OK)
         return areaCfg.AreaLsaCount; 

   return 0;
}

ulng OSPFAREA_LSACHKSUM(t_Handle p)
{
   ospfAreaEntry *p_Entry = (ospfAreaEntry *)p;
   t_S_AreaCfg    areaCfg;

   /* If area object exists retrive data from the area object.*/
   areaCfg.AreaStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (ARO_Config(p_Entry->owner, &areaCfg) == E_OK)
         return areaCfg.AreaLsaCksumSum; 

   return 0;
}

e_Err OSPFSTUBAREA_SETMETRIC(t_Handle ospfMibObj, t_Handle p, ulng metric)
{
   ospfStubAreaEntry *p_Entry = (ospfStubAreaEntry *)p;
   ospfAreaEntry     *p_areaEntry;
   t_S_StubAreaEntry  p_StubCfg;

   if (!p)
      return E_FAILED;

   if (p_Entry->info.metric == metric)
      return E_OK;

   /* find the "parental" area object */
   if ( (p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_,
                                                 &p_Entry->ospfStubAreaId)) )
      if (p_areaEntry->owner)
      {
         p_StubCfg.StubTOS    = p_Entry->ospfStubAreaTOS;  /* 0 */
         p_StubCfg.StubStatus = ROW_READ;
         if (ARO_StubMetric(p_areaEntry->owner, &p_StubCfg) != E_OK)
            return E_FAILED;

         p_StubCfg.StubMetric = metric;
         p_StubCfg.StubStatus = ROW_CHANGE;
         if (ARO_StubMetric(p_areaEntry->owner, &p_StubCfg) != E_OK)
            return E_FAILED;
      }

   p_Entry->info.metric = metric;
   return E_OK;
}

e_Err OSPFSTUBAREA_SETMETRICTYPE(t_Handle ospfMibObj, t_Handle p,
                                 e_StubMetricType  metricType)
{
   ospfStubAreaEntry *p_Entry = (ospfStubAreaEntry *)p;
   ospfAreaEntry     *p_areaEntry;
   t_S_StubAreaEntry  p_StubCfg;

   if (!p)
      return E_FAILED;

   if (p_Entry->info.metricType == metricType)
      return E_OK;

   /* find the "parental" area object */
   if ( (p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_,
                                                 &p_Entry->ospfStubAreaId)) )
      if (p_areaEntry->owner)
      {
         p_StubCfg.StubTOS    = p_Entry->ospfStubAreaTOS;  /* 0 */
         p_StubCfg.StubStatus = ROW_READ;
         if (ARO_StubMetric(p_areaEntry->owner, &p_StubCfg) != E_OK)
            return E_FAILED;

         p_StubCfg.StubMetricType = metricType;
         p_StubCfg.StubStatus     = ROW_CHANGE;
         if (ARO_StubMetric(p_areaEntry->owner, &p_StubCfg) != E_OK)
            return E_FAILED;
      }

   p_Entry->info.metricType = metricType;
   return E_OK;
}

word OSPFIF_STATE(t_Handle p)
{
   ospfIfEntry *p_Entry = (ospfIfEntry *)p;
   t_IFO_Cfg    ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return ifCfg.State; 

   return 1;  /*"down" state */

}

ulng OSPFIF_DESRTR(t_Handle p)
{
   ospfIfEntry *p_Entry = (ospfIfEntry *)p;
   t_IFO_Cfg    ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return ifCfg.DrId; 

   return 0;
}

ulng OSPFIF_BACKUPDESRTR(t_Handle p)
{
   ospfIfEntry *p_Entry = (ospfIfEntry *)p;
   t_IFO_Cfg    ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return (ulng)ifCfg.BackupId; 

   return 0;
}

ulng OSPFIF_EVENTS(t_Handle p)
{
   ospfIfEntry *p_Entry = (ospfIfEntry *)p;
   t_IFO_Cfg    ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return ifCfg.EventsCounter;
         
   return 0;
}

e_Err OSPFIF_SETAUTHKEY(t_Handle p, byte *authKey, int keyLen)
{
   ospfIfEntry *p_Entry = (ospfIfEntry *)p;
   t_IFO_Cfg    ifCfg;

   /* Store authorization key both in MIB OLL and then attempt */
   /* to store it into IFO object (if the interface is active) */
   
   memset(p_Entry->info.authKey, 0, 8);
   memcpy(p_Entry->info.authKey, authKey, keyLen);

   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
   {
      ifCfg.IfStatus  = ROW_READ;
      if (IFO_Config(p_Entry->owner, &ifCfg) != E_OK)
         return E_FAILED;

      ifCfg.IfStatus  = ROW_CHANGE;
      memcpy(ifCfg.AuKey, authKey, 8);
      if (IFO_Config(p_Entry->owner, &ifCfg) != E_OK)
         return E_FAILED;    
    }

   /* If we get here everything is OK */
    return E_OK;
}



/*********************************************************************
 * @purpose       Set the metric value for a certain interface. Called
 *                from the SNMP code.
 *
 *
 * @param  ospfMibObj     @b{(input)}  t_Handle of the OSPF MIB object
 * @param  p              @b{(input)}  t_Handle of the IfMetricEntry
 * @param  metric         @b{(input)}  the metric value to be set
 *
 * @returns               E_OK       - success
 * @returns               E_IN_MATCH - entry already exists
 * @returns               E_BADPARM  - invalid parameter
 * @returns               E_NOMEMORY - can't allocate memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err OSPFIFMETRIC_SETMETRIC(t_Handle ospfMibObj, t_Handle p_ifm, word metric)
{
   ospfIfMetricEntry *p;
   ospfIfEntry       *p_if;
   t_S_IfMetric       mcfg;

   if (!p_ifm)
      return E_FAILED;

   p = (ospfIfMetricEntry *)p_ifm;

   /* find the IFO object that owns the metric */
   p_if = (ospfIfEntry *)OLL_Find(_ospfIfTable_, &p->ospfIfMetrIpAddres,
                                  &p->ospfIfMetrAddrLessIf);

   /* If the i/f exists, then change its metric. If not, */
   /* this metric entry was probably created by SNMP in  */
   /* preparation to creating a new interface.           */
   if (p_if && p_if->owner)
   {
      mcfg.MetricTOS = p->ospfIfMetricTOS;
      mcfg.MetricStatus = ROW_READ;
      if (IFO_MetricConfig(p_if->owner, &mcfg) != E_OK)
         return E_FAILED;

      mcfg.Metric = metric;
      mcfg.MetricStatus = ROW_CHANGE;
      if (IFO_MetricConfig(p_if->owner, &mcfg) != E_OK)
         return E_FAILED;
   }

   p->info.metricValue = metric;
   return E_OK;
}

word OSPFVIRTIF_STATE(t_Handle p)
{
   ospfVirtIfEntry *p_Entry = (ospfVirtIfEntry *)p;
   t_IFO_Cfg        ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return ifCfg.State; 

   return 1;  /* down state */
}

ulng OSPFVIRTIF_EVENTS(t_Handle p)
{
   ospfVirtIfEntry *p_Entry = (ospfVirtIfEntry *)p;
   t_IFO_Cfg        ifCfg;

   /* If IFO object exists retrive data from the object.*/
   ifCfg.IfStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (IFO_Config(p_Entry->owner, &ifCfg) == E_OK)
         return ifCfg.EventsCounter; 

   return 0;
}

e_Err OSPFVIRTIF_SETAUTHKEY(t_Handle p, byte *authKey, int keyLen)
{
   ospfVirtIfInfo  p_ifInfo = ((ospfVirtIfEntry *)p)->info;
   t_IFO_Cfg       ifCfg;
   /* p_ifInfo.authKey is now an array that stores the authentication
      key */
   memcpy(p_ifInfo.authKey, authKey, keyLen);
   p_ifInfo.keyLen   = keyLen;

   if ((p_ifInfo.rowStatus == active) && ((ospfVirtIfEntry *)p)->owner)
   {
      ifCfg.IfStatus  = ROW_READ;
      if (IFO_Config(((ospfVirtIfEntry *)p)->owner, &ifCfg) != E_OK)
         return E_FAILED;

      ifCfg.IfStatus  = ROW_CHANGE;
      memcpy(ifCfg.AuKey, authKey, keyLen);
      if (IFO_Config(((ospfVirtIfEntry *)p)->owner, &ifCfg) != E_OK)
         return E_FAILED;    
    }

   /* If we get here everything is OK */
    return E_OK;
}

ulng OSPFNBR_GETDEADTIMERREMAINING (t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrDeadTimerRemaining; 

   return 0;
}

ulng OSPFNBR_GETUPTIME(t_Handle p)
{
  ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
        if (nbrCfg.NbrState == (NBO_FULL+1))
          return nbrCfg.NbrUptime; 

   return 0;
}

word OSPFNBR_GETPRIORITY (t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrPriority; 

   return 0;
}

word OSPFNBR_GETIFINDEX (t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.IfIndex; 

   return 0;
}

word OSPFNBR_GETAREA (t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrArea; 

   return 0;
}

ulng OSPFNBR_OPTIONS (t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrOptions; 

   return 0;
}

word OSPFNBR_STATE(t_Handle p)
{
   ospfNbrEntry      *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg    nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrState; 

   /* if row is not active or there's no owner, the state is "down" */
   return NBO_DOWN;
}

ulng OSPFNBR_EVENTS(t_Handle p)
{
   ospfNbrEntry     *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg   nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrEvents; 

   return 0;
}

ulng OSPFNBR_LSRETRANSQLEN(t_Handle p)
{
   ospfNbrEntry     *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg   nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry ->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrLsRetransQLen; 

   return 0;
}

e_TruthValue OSPFNBR_HELLOSUPPRESSED(t_Handle p)
{
   ospfNbrEntry     *p_Entry = (ospfNbrEntry *)p;
   t_S_NeighborCfg   nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if ((p_Entry->info.rowStatus == active) && p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return (nbrCfg.NbrHelloSuppressed ? _true : _false); 

   return _false;
}

ulng OSPFVIRTNBR_IPADDR(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.IpAdr; 

   return 0;
}

ulng OSPFVIRTNBR_OPTIONS(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrOptions; 

   return 0;
}

word OSPFVIRTNBR_STATE(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrState; 

   return 0;
}

ulng OSPFVIRTNBR_EVENTS(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrEvents; 

   return 0;
}

ulng OSPFVIRTNBR_LSRETRANSQLEN(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return nbrCfg.NbrLsRetransQLen; 

   return 0;
}

e_TruthValue OSPFVIRTNBR_HELLOSUPPRESSED(t_Handle p)
{
   ospfVirtNbrEntry   *p_Entry = (ospfVirtNbrEntry *)p;
   t_S_NeighborCfg     nbrCfg;

   /* If NBR object exists retrive data from the object.*/
   nbrCfg.NbmaNbrStatus  = ROW_READ;
   if (p_Entry->owner)
      if (NBO_Config(p_Entry->owner, &nbrCfg) == E_OK)
         return (nbrCfg.NbrHelloSuppressed ? _true : _false); 

   return _false;
}

e_Err OSPFAREAAGGR_SETEFFECT(t_Handle ospfMibObj, t_Handle p,
                             e_ospfAreaAggrEffect effect)
{
   ospfAreaAggrEntry      *p_Entry = (ospfAreaAggrEntry *)p;
   ospfAreaEntry          *p_areaEntry;
   t_S_AreaAggregateEntry  AreaRangeCfg;

   if (!p)
      return E_FAILED;

   if (p_Entry->info.effect == effect)
      return E_OK;

   /* find the "parental" area object */
   if ( (p_areaEntry = (ospfAreaEntry *)OLL_Find(_ospfAreaTable_,
                                               &p_Entry->ospfAreaAggrAreaId)) )
      if (p_areaEntry->owner)
      {
         /* The ARO routine performs the search by */
         /* 8 bytes: net address and subnet mask.  */
         AreaRangeCfg.NetIpAdr        = p_Entry->ospfAreaAggrNet;
         AreaRangeCfg.SubnetMask      = p_Entry->ospfAreaAggrMask;
         AreaRangeCfg.AggregateStatus = ROW_READ;
         if (ARO_AddressRange(p_areaEntry->owner, &AreaRangeCfg) != E_OK)
            return E_FAILED;

         AreaRangeCfg.AggregateEffect = effect;
         AreaRangeCfg.AggregateStatus = ROW_CHANGE;
         if (ARO_AddressRange(p_areaEntry->owner, &AreaRangeCfg) != E_OK)
            return E_FAILED;
      }

   p_Entry->info.effect = effect;
   return E_OK;
}

/* Internal utility searching ospfAreaTable and looks for */
/* the first area that the area object was created for. */
t_Handle getAnyAreaObjId (t_Handle ospfMibObj)
{
   ospfAreaEntry  *p_areaEntry = NULLP;
   t_Handle        areaObj      = NULLP;
   int             i            = 0;
   word            maxAreaNum;

   if (!ospfMibObj)
      return NULLP;

   maxAreaNum   = OLL_HowMany(_ospfAreaTable_);
   do  
   {
      i++;
      p_areaEntry = OLL_GetNext(_ospfAreaTable_, p_areaEntry );
      if(p_areaEntry)
         areaObj =  p_areaEntry->owner;
   }
   while (!areaObj && p_areaEntry && (i<= maxAreaNum));

   return areaObj;
}

/*  End of osfmib.c file */
