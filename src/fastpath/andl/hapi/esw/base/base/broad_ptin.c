#include "broad_ptin.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_l2.h"
#include "ptin_hapi_l3.h"
#include "ptin_hapi_xlate.h"
#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_fp_bwpolicer.h"
#include "ptin_hapi_fp_counters.h"
#include "ptin_hapi_fp_utils.h"
#include "ptin_hapi_qos.h"
#include "ptin_ptp_fpga.h"
#include "ptin_structs.h"
#include "ptin_globaldefs.h"
#include "broad_common.h"
#include "broad_l2_vlan.h"
#include "broad_l2_std.h"
#include <bcm/oam.h>
#include <ethsrv_oam.h>
#include <nimapi.h>

#include <bcm/port.h>

#include <dapi_db.h>
#include <hpc_db.h>
#include <soc/higig.h>

static L7_RC_t hapiBroadPTinPrbsPreemphasisGet(DAPI_USP_t *usp, L7_uint16 *preemphasys, L7_int number_of_lanes);
static L7_RC_t hapiBroadPTinPrbsPreemphasisSet(DAPI_USP_t *usp, L7_uint16 *preemphasys, L7_int number_of_lanes, L7_BOOL force);


/**
 * Callback for generic operations
 */
typedef L7_RC_t (*broad_ptin_generic_f)(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);

/* Add here your callback prototype */
L7_RC_t broad_ptin_example(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_l2_maclimit(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_l2_maclimit_status(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_l3_intf(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_l3_ipmc(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_qos_classify(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_qos_remark(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_oam_sendlmm(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_oam_check_lm_counters(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_time_interface(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_ptp_fpga_entry(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_oam_fpga_entry(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_temperature_monitor(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);
L7_RC_t broad_ptin_shaper_max_burst(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g);


L7_RC_t broadPtin_oam_tx( int unit, int flags, bcm_gport_t gport_dst, bcm_mac_t *mac_dst, bcm_mac_t *mac_src, bcm_oam_endpoint_info_t *endpoint_info);
/* List of callbacks */
broad_ptin_generic_f ptin_dtl_callbacks[PTIN_DTL_MSG_MAX] = {  
  broad_ptin_example,
  broad_ptin_l2_maclimit,
  broad_ptin_l2_maclimit_status,
  broad_ptin_l3_intf, 
  broad_ptin_l3_ipmc,
  broad_ptin_qos_classify,
  broad_ptin_qos_remark,
  broad_ptin_oam_sendlmm,
  broad_ptin_oam_check_lm_counters,
  broad_ptin_time_interface,
  broad_ptin_ptp_fpga_entry,
  broad_ptin_oam_fpga_entry,
  broad_ptin_temperature_monitor,
  broad_ptin_shaper_max_burst
};

/**
 * First callback example
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_example(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dtl_example_t *example = (ptin_dtl_example_t *) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Hello World: usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (example == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Structure data pointer is null");
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(ptin_dtl_example_t))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(ptin_dtl_example_t));
    return L7_FAILURE;
  }

  switch (operation)
  {
    case DAPI_CMD_GET:
      PT_LOG_TRACE(LOG_CTX_HAPI, "Operation is DAPI_CMD_GET");
      /* Put here function for GET procedure */
      break;

    case DAPI_CMD_SET:
      PT_LOG_TRACE(LOG_CTX_HAPI, "Operation is DAPI_CMD_SET");
      /* Put here function for SET procedure */
      break;

    case DAPI_CMD_CLEAR:
      PT_LOG_TRACE(LOG_CTX_HAPI, "Operation is DAPI_CMD_CLEAR");
      /* Put here function for CLEAR procedure */
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Calling ptin_hapi_example...");
  return ptin_hapi_example(usp, example, dapi_g);
}

/**
 * First callback example
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_l2_maclimit(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc = L7_SUCCESS;
  ptin_l2_maclimit_t *entry;

  entry = (ptin_l2_maclimit_t*) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "%s: usp={%d,%d,%d} operation=%u dataSize=%u", __FUNCTION__, usp->unit, usp->slot, usp->port, operation, dataSize);

  // Check action: None-0 or Limit-1. If none, the MAC learning is not limited 
  if (entry->limit == (L7_uint32) -1)
  {
    entry->action=0;
    entry->send_trap=0;
  }

  rc = ptin_hapi_maclimit_setmax(usp, entry->vlanId, entry->limit, entry->action, entry->send_trap, dapi_g);

  return rc;
}

L7_RC_t broad_ptin_l2_maclimit_status(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc = L7_SUCCESS;
  ptin_l2_maclimit_status_t *entry;

  entry = (ptin_l2_maclimit_status_t*) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "%s: usp={%d,%d,%d} operation=%u dataSize=%u", __FUNCTION__, usp->unit, usp->slot, usp->port, operation, dataSize);

  rc = ptin_hapi_maclimit_status(usp, &entry->number_mac_learned, &entry->status, dapi_g);

  return rc;
}

/**
 * Handle L3 Interfaces
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_l3_intf(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dtl_l3_intf_t *intf = (ptin_dtl_l3_intf_t *) data;
  L7_RC_t             rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid Arguments: intf=%p", intf);
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(ptin_dtl_l3_intf_t))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(ptin_dtl_l3_intf_t));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Input Parameters [flags:0x08%X vid:%d mac_address:%02x:%02x:%02x:%02x:%02x:%02x l3_intf_id:%d mtu:%d]", intf->flags, intf->vid, 
           intf->mac_addr[0], intf->mac_addr[1], intf->mac_addr[2], intf->mac_addr[3], intf->mac_addr[4], intf->mac_addr[5], intf->l3_intf_id, intf->mtu);
  
  switch (operation)
  {
    case DAPI_CMD_GET:      
      rc = ptin_hapi_l3_intf_get(intf);
      break;

    case DAPI_CMD_SET:      
      rc = ptin_hapi_l3_intf_create(intf);
      break;

    case DAPI_CMD_CLEAR:      
      rc = ptin_hapi_l3_intf_delete(intf);      
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }
  return rc;  
}



/**
 * Handle L3 IP Multicast Entries
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_l3_ipmc(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dtl_ipmc_addr_t *ipmc = (ptin_dtl_ipmc_addr_t *) data;
  L7_RC_t               rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (ipmc == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid Arguments: intf=%p", ipmc);
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(ptin_dtl_ipmc_addr_t))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(ptin_dtl_ipmc_addr_t));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Input Parameters [flags:0x08%X vid:%d group_index:0x%x sourceAddr:0x%08x groupAddr:0x%08x]", ipmc->flags, ipmc->vid, ipmc->group_index, 
           ipmc->s_ip_addr.addr.ipv4.s_addr, ipmc->mc_ip_addr.addr.ipv4.s_addr);
  
  switch (operation)
  {
    case DAPI_CMD_GET:      
      rc = ptin_hapi_l3_ipmc_get(ipmc);
      break;

    case DAPI_CMD_SET:      
      rc = ptin_hapi_l3_ipmc_add(ipmc);
      break;

    case DAPI_CMD_CLEAR:      
      rc = ptin_hapi_l3_ipmc_remove(ipmc);      
      break;
    case DAPI_CMD_CLEAR_ALL:      
      rc = ptin_hapi_l3_ipmc_reset();
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }
  return rc;  
}

L7_RC_t broad_ptin_qos_classify(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_dtl_qos_t  *qos_cfg = (ptin_dtl_qos_t *) data;
  L7_RC_t          rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (qos_cfg == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid Arguments: intf=0x%08x", (L7_uint32) qos_cfg);
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(ptin_dtl_qos_t))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(ptin_dtl_qos_t));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Input Parameters [port_bmp=0x%016llx intVlan=%u NNIVlan=%u trust_mode=%u priority=%u/0x%x -> int_prio=%u , pRemark_prio=%u ",
            qos_cfg->ptin_port_bmp, qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->trust_mode, qos_cfg->priority, qos_cfg->priority_mask, qos_cfg->int_priority, qos_cfg->pbits_remark);

  dapiPort.usp = usp;
  dapiPort.dapi_g = dapi_g;

  switch (operation)
  {
    case DAPI_CMD_SET:      
      rc = ptin_hapi_qos_entry_add(&dapiPort, qos_cfg);
      break;

    case DAPI_CMD_CLEAR:      
      rc = ptin_hapi_qos_entry_remove(qos_cfg);
      break;
    case DAPI_CMD_CLEAR_ALL:
      rc = ptin_hapi_qos_table_flush(&dapiPort, qos_cfg);
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }
  return rc;  
}

L7_RC_t broad_ptin_qos_remark(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  L7_uint32 *remark_enable = (L7_uint32 *) data;
  L7_RC_t    rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (remark_enable == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid Arguments: intf=0x%08x", (L7_uint32) remark_enable);
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(L7_uint32))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(L7_uint32));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Input Parameters [Remark enable: %u]", *remark_enable);

  dapiPort.usp = usp;
  dapiPort.dapi_g = dapi_g;

  switch (operation)
  {
    case DAPI_CMD_SET:      
      rc = ptin_hapi_qos_egress_pbits_remark(&dapiPort, *remark_enable);
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }
  return rc;  
}

/**
 * Generic processor of DTL operations
 * 
 * @param usp 
 * @param cmd 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinGeneric(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptinDtlGenericMsg_t *ptinDtlGeneric = (ptinDtlGenericMsg_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (ptinDtlGeneric == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_DTL, "Null pointer");
    return L7_FAILURE;
  }

  /* Validate message id */
  if (ptinDtlGeneric->msgId >= PTIN_DTL_MSG_MAX)
  {
    PT_LOG_ERR(LOG_CTX_DTL, "Invalid Message ID (%u)", ptinDtlGeneric->msgId);
    return L7_FAILURE;
  }

  /* Validate datasize */
  if (ptinDtlGeneric->dataSize > PTIN_DTL_GENERICMSG_MAX_DATASIZE)
  {
    PT_LOG_ERR(LOG_CTX_DTL, "Invalid datasize (%u)", ptinDtlGeneric->dataSize);
    return L7_FAILURE;
  }

  /* Is a valid function pointer? */
  if (ptin_dtl_callbacks[ptinDtlGeneric->msgId] == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_DTL, "Null callback for message ID %u", ptinDtlGeneric->msgId);
    return L7_FAILURE;
  }

  /* Execute callback */
  rc = ptin_dtl_callbacks[ptinDtlGeneric->msgId](usp,
                                                 ptinDtlGeneric->getOrSet,
                                                 ptinDtlGeneric->dataSize,
                                                 ptinDtlGeneric->data,
                                                 dapi_g);
  /* Return result */
  return rc;
}

/**
 * Shaper and burst configuration
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_shaper_max_burst(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc = L7_SUCCESS;
  ptin_intf_shaper_t *entry;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  L7_uint32             bcmSpeed = 0;

  entry = (ptin_intf_shaper_t*) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "%s: usp={%d,%d,%d} operation=%u dataSize=%u", __FUNCTION__, usp->unit, usp->slot, usp->port, operation, dataSize);

  dapiPortPtr   = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr   = HAPI_PORT_GET( usp, dapi_g );

  PT_LOG_TRACE(LOG_CTX_HAPI, "speed %d", hapiPortPtr->speed );

  switch ( hapiPortPtr->speed )
  {
      case DAPI_PORT_SPEED_GE_1GBPS:
        bcmSpeed = 1000000;
        break;

      /* PTin added: Speed 2.5G */
      case DAPI_PORT_SPEED_GE_2G5BPS:
        bcmSpeed = 2500000;
        break;
      /* PTin end */

      case DAPI_PORT_SPEED_GE_10GBPS:
        bcmSpeed = 10000000;
        break;

      /* PTin added: Speed 40G */
      case DAPI_PORT_SPEED_GE_40GBPS:
        bcmSpeed = 40000000;
        break;

      /* PTin added: Speed 100G */
      case DAPI_PORT_SPEED_GE_100GBPS:
        bcmSpeed = 100000000;
        break;

      default:
        bcmSpeed = 2500000;
        break;
  }

  rc = ptin_hapi_qos_shaper_max_burst_config(usp->unit, hapiPortPtr->bcm_port, ((entry->max_rate*bcmSpeed)/100), entry->burst_size);

  return rc;
#if 0
  L7_RC_t rc = L7_SUCCESS;
  ptin_l2_maclimit_t *entry;

  entry = (ptin_l2_maclimit_t*) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "%s: usp={%d,%d,%d} operation=%u dataSize=%u", __FUNCTION__, usp->unit, usp->slot, usp->port, operation, dataSize);

  // Check action: None-0 or Limit-1. If none, the MAC learning is not limited 
  if (entry->limit == (L7_uint32) -1)
  {
    entry->action=0;
    entry->send_trap=0;
  }

  rc = ptin_hapi_maclimit_setmax(usp, entry->vlanId, entry->limit, entry->action, entry->send_trap, dapi_g);

  return rc;
#endif

#if 0
  ptin_dtl_l3_intf_t *intf = (ptin_dtl_l3_intf_t *) data;
  L7_RC_t             rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d} operation=%u dataSize=%u", usp->unit, usp->slot, usp->port, operation, dataSize);

  /* Validate data pointer */
  if (intf == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid Arguments: intf=%p", intf);
    return L7_FAILURE;
  }

  /* Validate data size */
  if (dataSize != sizeof(ptin_dtl_l3_intf_t))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid data size (%u VS %u)", dataSize, sizeof(ptin_dtl_l3_intf_t));
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Input Parameters [flags:0x08%X vid:%d mac_address:%02x:%02x:%02x:%02x:%02x:%02x l3_intf_id:%d mtu:%d]", intf->flags, intf->vid, 
           intf->mac_addr[0], intf->mac_addr[1], intf->mac_addr[2], intf->mac_addr[3], intf->mac_addr[4], intf->mac_addr[5], intf->l3_intf_id, intf->mtu);
  
  switch (operation)
  {
    case DAPI_CMD_GET:      
      rc = ptin_hapi_l3_intf_get(intf);
      break;

    case DAPI_CMD_SET:      
      rc = ptin_hapi_l3_intf_create(intf);
      break;

    case DAPI_CMD_CLEAR:      
      rc = ptin_hapi_l3_intf_delete(intf);      
      break;

    default:
      PT_LOG_ERR(LOG_CTX_HAPI, "Not recognized operation (%u)!", operation);
      return L7_FAILURE;
  }
  return rc;  
#endif
}



/**
 * Initialize HAPI PTin data structures
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinDataInit(void)
{
  L7_RC_t rc;

  PT_LOG_INFO(LOG_CTX_HAPI, "PTin HAPI Applying configs...");

  /* Initialize PTIN HAPI files */
  rc = hapi_ptin_data_init();

  PT_LOG_INFO(LOG_CTX_HAPI, "PTin HAPI data structs config: %d",rc);

  return rc;
}

/**
 * Initialize HAPI PTin module
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinInit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  PT_LOG_TRACE(LOG_CTX_HAPI, "PTin HAPI Applying configs...");

  /* Initialize PTIN HAPI files */
  rc = hapi_ptin_config_init();

  //hapi_ptin_phy_config_init();

  PT_LOG_INFO(LOG_CTX_HAPI, "PTin HAPI Configuration: %d",rc);

  return rc;
}

/**
 * Apply hardware procedure
 * 
 * @param usp 
 * @param cmd 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadHwApply(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_hwproc_t *hwproc = (ptin_hwproc_t *) data;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint8 enable, link;

  PT_LOG_TRACE(LOG_CTX_HAPI, "PTin HAPI Configuration: procedure=%u, param1=%d, param2=%d", hwproc->procedure, hwproc->param1, hwproc->param2);
  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  if (hwproc->procedure == PTIN_HWPROC_NONE)
  {
    PT_LOG_INFO(LOG_CTX_HAPI, "Nothing to do");
    return L7_SUCCESS;
  }

  switch (hwproc->procedure)
  {
  case PTIN_HWPROC_LINKSCAN:
    if (hwproc->operation == DAPI_CMD_GET)
    {
      rc = ptin_hapi_linkscan_get(usp, dapi_g, &enable); 
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_linkscan_get: rc=%d", rc);
      hwproc->param1 = (L7_int32) enable;
    }
    else if (hwproc->operation == DAPI_CMD_SET)
    {
      enable = (L7_uint8) hwproc->param1;
      rc = ptin_hapi_linkscan_set(usp, dapi_g, enable);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_linkscan_set (%u): rc=%d", hwproc->param1, rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break;

  case PTIN_HWPROC_FORCE_LINK:
    link = (L7_uint8) hwproc->param1;
    if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_link_force(usp, dapi_g, link, L7_ENABLE);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_force_link (link=%u, enable=1): rc=%d", hwproc->param1, rc);
    }
    else if (hwproc->operation == DAPI_CMD_CLEAR)
    {
      rc = ptin_hapi_link_force(usp, dapi_g, link, L7_DISABLE);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_force_link (link=%u, enable=0): rc=%d", hwproc->param1, rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break; 

  case PTIN_HWPROC_CLK_RECVR:
    if (hwproc->operation == DAPI_CMD_SET)
    {
      L7_int main_port, bckp_port;

      main_port = hwproc->param1;
      bckp_port = hwproc->param2;

      rc = ptin_hapi_clock_recovery_set(main_port, bckp_port, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_clock_recovery_set (main_port=%d, backup_port=%d): rc=%d", main_port, bckp_port, rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break;

  case PTIN_HWPROC_VCAP_DEFVID:
    if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_vcap_defvid(usp, hwproc->param1, hwproc->param2, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_vcap_defvid (outerVlan=%u, innerVlan=%u): rc=%d", hwproc->param1, hwproc->param2, rc);
    }
    else if (hwproc->operation == DAPI_CMD_CLEAR)
    {
      rc = ptin_hapi_vcap_defvid(usp, 0, 0, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_vcap_defvid (outerVlan=0, innerVlan=0): rc=%d", rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break; 

  case PTIN_HWPROC_UC_TTL1_CPU:
    if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_L3UcastTtl1ToCpu_set(usp, 1, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_L3UcastTtl1ToCpu_set");
    }
    else if (hwproc->operation == DAPI_CMD_CLEAR)
    {
      rc = ptin_hapi_L3UcastTtl1ToCpu_set(usp, 0, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_L3UcastTtl1ToCpu_set");
    }
    break; 

  case PTIN_HWPROC_FRAME_OVERSIZE:
    if (hwproc->operation == DAPI_CMD_GET)
    {
      rc = ptin_hapi_frame_oversize_get(usp, &hwproc->param1, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_frame_oversize_get: rc=%d", rc);
      else
        PT_LOG_TRACE(LOG_CTX_HAPI, "Oversize frame limite for port {%d,%d,%d} is %u bytes",
                  usp->unit, usp->slot, usp->port, hwproc->param1);
    }
    else if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_frame_oversize_set(usp, hwproc->param1, dapi_g);
      if (rc != L7_SUCCESS)
        PT_LOG_ERR(LOG_CTX_HAPI, "Error with ptin_hapi_frame_oversize_set (frame_size=%u): rc=%d", hwproc->param1, rc);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break;

  case PTIN_HWPROC_WARPCORE_RESET:
    if (hwproc->operation == DAPI_CMD_SET)
    {
      rc = ptin_hapi_warpcore_reset(hwproc->param1, L7_TRUE);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Operation not recognized: %u", hwproc->operation);
      rc = L7_FAILURE;
    }
    break;

  default:
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid procedure: %u", hwproc->procedure);
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Get slot mode list
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinSlotMode(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc = L7_SUCCESS;

  #if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
  ptin_slotmode_t *slotmode;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  L7_uint32                    *wcSlotMode;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;
  wcSlotMode           = dapiCardPtr->wcSlotMode;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Slot mode operation");

  slotmode = (ptin_slotmode_t *) data;

  /* Get operation */
  if (slotmode->operation == DAPI_CMD_GET)
  {
    memcpy(slotmode->slotMode, wcSlotMode, sizeof(L7_uint32)*PTIN_SYS_SLOTS_MAX);
  }
  /* Make a validation */
  else if (slotmode->operation == DAPI_CMD_SET)
  {
    /* Return failure if map is not valid */
    rc = hpcConfigWCmap_build(slotmode->slotMode, L7_NULLPTR);
  }

  //PT_LOG_INFO(LOG_CTX_HAPI, "PTin HAPI Configuration: %d",rc);
  #endif

  return rc;
}

/**
 * Set MEF Extension parameters
 * 
 * @param usp : interface
 * @param cmd : no meaning
 * @param data : priority (int)
 * @param dapi_g : port definitions
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinPortExt(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_HWPortExt_t  *portExt;
  L7_RC_t rc = L7_SUCCESS;
  L7_int  macLearn_enable, stationMove_enable, stationMove_prio, stationMove_samePrio, port_type;

  /* Extract priority */
  portExt = (ptin_HWPortExt_t *) data;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort,usp,dapi_g);

  /* Set operation */
  if (portExt->operation==DAPI_CMD_GET)
  {
    portExt->Mask = 0;

    /* MAC learning attributes */
    rc = hapi_ptin_l2learn_port_get( &dapiPort, &macLearn_enable, &stationMove_enable, &stationMove_prio, &stationMove_samePrio );

    if (rc==L7_SUCCESS)
    {
      portExt->macLearn_enable               = macLearn_enable;
      portExt->macLearn_stationMove_enable   = stationMove_enable;
      portExt->macLearn_stationMove_prio     = stationMove_prio;
      portExt->macLearn_stationMove_samePrio = stationMove_samePrio;
      portExt->Mask |=  PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO |
                        PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO;

      /* Egress port type */
      rc = hapi_ptin_egress_port_type_get(&dapiPort, &port_type);

      if (rc==L7_SUCCESS)
      {
        portExt->egress_type = port_type;
        portExt->Mask       |= PTIN_HWPORTEXT_MASK_EGRESS_TYPE;
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        rc = L7_SUCCESS;
      }
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "Result for L2LearnPortSet: %d",rc);
  }
  else if (portExt->operation==DAPI_CMD_SET)
  {
    /* MAC learning attributes */
    rc = hapi_ptin_l2learn_port_set(&dapiPort,
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_ENABLE)               ? portExt->macLearn_enable : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_ENABLE)   ? portExt->macLearn_stationMove_enable : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_PRIO)     ? portExt->macLearn_stationMove_prio : -1),
                                    ((portExt->Mask & PTIN_HWPORTEXT_MASK_MACLEARN_STATIONMOVE_SAMEPRIO) ? portExt->macLearn_stationMove_samePrio : -1) );
    if (rc == L7_SUCCESS)
    {
      /* Egress port type */
      if (portExt->Mask & PTIN_HWPORTEXT_MASK_EGRESS_TYPE)
      {
        /* TODO: Set to egress_type */
        rc = hapi_ptin_egress_port_type_set(&dapiPort, portExt->egress_type);
      }
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "Result for L2LearnPortSet: %d",rc);
  }

  return rc;
}


/**
 * Read counters from a physical interface
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinCountersRead(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  rc = hapi_ptin_counters_read((ptin_HWEthRFC2819_PortStatistics_t *)data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/**
 * Clear counters
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinCountersClear(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_HWEthRFC2819_PortStatistics_t *stat = (ptin_HWEthRFC2819_PortStatistics_t *) data;
  L7_uint port;

  port = stat->Port;

  if ( hapi_ptin_counters_clear(port) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error on hapi_ptin_counters_clear() on port# %u", port);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Get counters activity summary
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEth_PortsActivity_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinCountersActivityGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t rc;

  rc = hapi_ptin_counters_activity_get((ptin_HWEth_PortsActivity_t*)data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Uses vlan translation functionalities
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlanXlate_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanTranslate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_vlanXlate_t *xlate = (ptin_vlanXlate_t *) data;
  L7_RC_t rc = L7_SUCCESS;
  ptin_dapi_port_t dapiPort;
  ptin_hapi_xlate_t hapi_xlate;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}, Stage=%u, operation=%u, group=%u, oVlanId=%u, iVlanId=%u, newOVlanId=%u(%u), newIVlanId=%u(%u), rem_VLANs=%u",
            usp->unit, usp->slot, usp->port, xlate->stage, xlate->oper, xlate->portgroup,
            xlate->outerVlan, xlate->innerVlan,
            xlate->outerVlan_new,xlate->outerVlanAction,
            xlate->innerVlan_new,xlate->innerVlanAction,
            xlate->remove_VLANs);

  /* Validate interface */
  if ( ( xlate->portgroup==PTIN_XLATE_PORTGROUP_INTERFACE ) &&
       ( usp->unit<0 || usp->slot<0 || usp->port<0 ) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort,usp,dapi_g);

  /* Copy vlan and action information */
  hapi_xlate.outerVlanId      = xlate->outerVlan;
  hapi_xlate.innerVlanId      = xlate->innerVlan;
  hapi_xlate.newOuterVlanId   = xlate->outerVlan_new;
  hapi_xlate.newInnerVlanId   = xlate->innerVlan_new;
  hapi_xlate.outerVlanAction  = (xlate->remove_VLANs) ? PTIN_XLATE_ACTION_DELETE : xlate->outerVlanAction;
  hapi_xlate.innerVlanAction  = (xlate->remove_VLANs) ? PTIN_XLATE_ACTION_DELETE : xlate->innerVlanAction;

  hapi_xlate.outerPrio        = xlate->outerPrio;
  hapi_xlate.innerPrio        = xlate->innerPrio;
  hapi_xlate.newOuterPrio     = xlate->outerPrio_new;
  hapi_xlate.newInnerPrio     = xlate->innerPrio_new;
  hapi_xlate.outerPrioAction  = xlate->outerPrioAction;
  hapi_xlate.innerPrioAction  = xlate->innerPrioAction;

  switch ((L7_int) xlate->stage)
  {
  case PTIN_XLATE_STAGE_INGRESS:

    if ( xlate->oper == DAPI_CMD_GET )
    {
      rc = ptin_hapi_xlate_ingress_get(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_get(&dapiPort, xlate->outerVlan, xlate->innerVlan, &(xlate->outerVlan_new));
    }
    else if ( xlate->oper == DAPI_CMD_SET )
    {
      rc = ptin_hapi_xlate_ingress_add(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_add(&dapiPort, xlate->outerVlan, xlate->innerVlan, xlate->outerVlan_new);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      rc = ptin_hapi_xlate_ingress_delete(&dapiPort, &hapi_xlate);
      //rc = ptin_hapi_xlate_ingress_action_delete(&dapiPort, xlate->outerVlan, xlate->innerVlan);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR_ALL )
    {
      rc = ptin_hapi_xlate_ingress_delete_all();
      //rc = ptin_hapi_xlate_ingress_action_delete_all();
    }
    break;

  case PTIN_XLATE_STAGE_EGRESS:
    if ( xlate->oper == DAPI_CMD_GET )
    {
      rc = ptin_hapi_xlate_egress_get(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_get(xlate->portgroup, xlate->outerVlan, xlate->innerVlan, &(xlate->outerVlan_new));
    }
    else if ( xlate->oper == DAPI_CMD_SET )
    {
      rc = ptin_hapi_xlate_egress_add(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_add(xlate->portgroup, xlate->outerVlan, xlate->innerVlan, xlate->outerVlan_new);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      rc = ptin_hapi_xlate_egress_delete(xlate->portgroup, &hapi_xlate);
      //rc = ptin_hapi_xlate_egress_action_delete(xlate->portgroup, xlate->outerVlan, xlate->innerVlan);
    }
    else if ( xlate->oper == DAPI_CMD_CLEAR_ALL )
    {
      rc = ptin_hapi_xlate_egress_delete_all();
      //rc = ptin_hapi_xlate_egress_action_delete_all();
    }
    break;

  case PTIN_XLATE_STAGE_ALL:
    if ( xlate->oper == DAPI_CMD_CLEAR )
    {
      PT_LOG_TRACE(LOG_CTX_HAPI, "All translations will be removed");

      if ( (ptin_hapi_xlate_ingress_delete_all() != L7_SUCCESS) ||
           (ptin_hapi_xlate_egress_delete_all() != L7_SUCCESS) )
        rc = L7_FAILURE;
//    if ( (ptin_hapi_xlate_ingress_action_delete_all() != L7_SUCCESS) ||
//         (ptin_hapi_xlate_egress_action_delete_all() != L7_SUCCESS) )
//      rc = L7_FAILURE;
    }
    break;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define port groups for egress translation
 * 
 * @param usp 
 * @param cmd 
 * @param data: structure ptin_vlanXlate_classId_t
 * @param dapi_g 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanTranslatePortGroups(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_vlanXlate_classId_t *classSt = (ptin_vlanXlate_classId_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}, Oper=%d, classId=%d",
            usp->unit, usp->slot, usp->port, classSt->oper, classSt->class_id);

  /* Validate usp */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid usp");
    return L7_FAILURE;
  }

  switch ((L7_int) classSt->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_xlate_egress_portsGroup_set(classSt->class_id, &usp, 1, dapi_g);
    break;

  case DAPI_CMD_GET:
    rc = ptin_hapi_xlate_egress_portsGroup_get(&(classSt->class_id), usp, dapi_g);
    break;
  }
  
  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define general vlan settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  bcm_pbmp_t upbmp;
  ptin_vlan_mode_t *mode = (ptin_vlan_mode_t *) data;
  DAPI_USP_t cpuUsp;
  bcm_error_t rv;

  if (mode->vlanId<=1 || mode->vlanId>=4094)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Management cpu port is added when the vlan is created. Update the Hw 
  *  Vlan member mask
  */
  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error obtaining CPU interfaces");
    return L7_FAILURE;
  }

  BCM_PBMP_CLEAR(upbmp);
  if (mode->cpu_include)
  {
    /* Add CPU interface to vlan */
    rv = bcm_vlan_port_add(0, mode->vlanId, PBMP_CMIC(0), upbmp);
  }
  else
  {
    /* Remove CPU interface to vlan */
    rv = bcm_vlan_port_remove(0, mode->vlanId, PBMP_CMIC(0));
  }

  /* Any error? */
  if (L7_BCMX_OK(rv) != L7_TRUE)
    return L7_FAILURE;

  /* Add/remove all the local CPUs to the vlan */
  for (cpuUsp.port=0; cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
  {
    if (mode->cpu_include)
      BROAD_HW_VLAN_MEMBER_SET(&cpuUsp,mode->vlanId,dapi_g);
    else
      BROAD_HW_VLAN_MEMBER_CLEAR(&cpuUsp,mode->vlanId,dapi_g);
  }

  return L7_SUCCESS;
}

/**
 * Define vlan mode settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bridge_vlan_mode_t *mode = (ptin_bridge_vlan_mode_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI,"vlanId=%u, mask=0x%02X, fwdVlan=%u, outerTPID=0x%04X, mac_learn=%u, crossc=%u",
            mode->vlanId, mode->mask, mode->fwdVlanId, mode->outer_tpid, mode->learn_enable, mode->cross_connects_enable);

  /* Validate vlan */
  if (mode->vlanId==0 || mode->vlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid vlan (%d)", mode->vlanId);
    return L7_FAILURE;
  }

  /* If mask is empty there is nothing to be done */
  if (mode->mask==PTIN_BRIDGE_VLAN_MODE_MASK_NONE)
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"Mask is empty... nothing to be done");
    return L7_SUCCESS;
  }

  /* Forward vlan configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_FWDVLAN)
  {
    if (ptin_hapi_bridge_vlan_mode_fwdVlan_set(mode->vlanId, mode->fwdVlanId)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  /* Multicast configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_MC_GROUP)
  {    
    if (ptin_hapi_bridgeVlan_multicast_set(mode->vlanId, &mode->multicast_group, mode->multicast_flag)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  /* Outer TPID configuration */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_OTPID)
  {
    if (ptin_hapi_bridge_vlan_mode_outerTpId_set(mode->vlanId, mode->outer_tpid)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }
  /* MAC learning enable */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_LEARN_EN)
  {
    if (ptin_hapi_bridge_vlan_mode_macLearn_set(mode->vlanId, mode->learn_enable)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }
  /* Cross-connect enable */
  if (mode->mask & PTIN_BRIDGE_VLAN_MODE_MASK_CROSSCONN_EN)
  {
    if (ptin_hapi_bridge_vlan_mode_crossconnect_set(mode->vlanId, mode->cross_connects_enable, mode->double_tag)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished: rc=%d", rc);

  return rc;
}

#define PTIN_UPLINK_PROTECTION_MACMOVE  0

/**
 * Define vlan port settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeVlanPortControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_vlan_mode_t  *vlan_mode = (ptin_vlan_mode_t *) data;
  DAPI_PORT_t       *dapiPortPtr, *dapiPortPtr_prev;
  BROAD_PORT_t      *hapiPortPtr, *hapiPortPtr_prev;
  bcm_trunk_t       tgid_prev = -1, tgid  = -1;
  bcmx_lport_t      lport_prev= -1, lport = -1;
  L7_BOOL           port_prev_is_lag = L7_FALSE, port_is_lag = L7_FALSE;
  bcmx_l2_addr_t    bcmx_l2_addr;
  bcm_error_t rv;

  /* Prepare switch-over */
  memset(&bcmx_l2_addr, 0x00, sizeof(bcmx_l2_addr));

  /* Get the port info */
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  PT_LOG_TRACE(LOG_CTX_API, "usp_old={%d,%d,%d}, usp_new={%d,%d,%d}",
            vlan_mode->ddUsp.unit, vlan_mode->ddUsp.slot, vlan_mode->ddUsp.port,
            usp->unit, usp->slot, usp->port);

  /* Previous port */
  dapiPortPtr_prev = L7_NULLPTR;
  hapiPortPtr_prev = L7_NULLPTR;

  /* Previous port (Must be physical) */
  if (vlan_mode->ddUsp.unit == 1 &&     /* Unit 0 */
      vlan_mode->ddUsp.slot == 0 &&     /* Physical port */
      vlan_mode->ddUsp.port >= 0 && vlan_mode->ddUsp.port < PTIN_SYSTEM_N_PORTS)
  {
    dapiPortPtr_prev = DAPI_PORT_GET(&vlan_mode->ddUsp, dapi_g);
    hapiPortPtr_prev = HAPI_PORT_GET(&vlan_mode->ddUsp, dapi_g);

    /* Physical port to be removed */
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr_prev) == L7_TRUE)
    {
      bcmx_l2_addr.flags = 0; 
      bcmx_l2_addr.lport = hapiPortPtr_prev->bcmx_lport;
      bcmx_l2_addr.tgid  = -1;

      lport_prev = hapiPortPtr_prev->bcmx_lport;
    }
    /* Logical port to be removed */
    else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr_prev) == L7_TRUE)
    {
      if (hapiPortPtr_prev->hapiModeparm.lag.tgid == -1)
      {
        if (hapiPortPtr_prev->hapiModeparm.lag.lastValidTgid >= 0)
        {
          tgid_prev = hapiPortPtr_prev->hapiModeparm.lag.lastValidTgid;
        }
      }
      else
      {
        tgid_prev = hapiPortPtr_prev->hapiModeparm.lag.tgid;
      }

      /* Validate tgid value */
      if (tgid_prev >= 0)
      {
        bcmx_l2_addr.flags = BCM_L2_TRUNK_MEMBER;
        bcmx_l2_addr.lport = hapiPortPtr_prev->bcmx_lport;
        bcmx_l2_addr.tgid  = tgid_prev;

        /* We have a valid lag */
        port_prev_is_lag = L7_TRUE;
      }
      else
      {
        dapiPortPtr_prev = L7_NULLPTR;
        hapiPortPtr_prev = L7_NULLPTR;
      }
    }
    /* Others */
    else
    {
      dapiPortPtr_prev = L7_NULLPTR;
      hapiPortPtr_prev = L7_NULLPTR;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Previous port {%d,%d,%d}", vlan_mode->ddUsp.unit, vlan_mode->ddUsp.slot, vlan_mode->ddUsp.port);
  }

  /* Physical port to be added */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    lport = hapiPortPtr->bcmx_lport;
  }
  /* Logical port to be added */
  else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    if (hapiPortPtr->hapiModeparm.lag.tgid == -1)
    {
      if (hapiPortPtr->hapiModeparm.lag.lastValidTgid == -1)
      {
        tgid = hapiPortPtr->hapiModeparm.lag.lastValidTgid;
      }
    }
    else
    {
      tgid = hapiPortPtr->hapiModeparm.lag.tgid;
    }
    /* Validate tgid value */
    if (tgid < 0)
    {
      /* No valid LAG was provided */
      return L7_FAILURE;
    }

    port_is_lag = L7_TRUE; 
  }
  /* Others */
  else
  {
    /* No valid port provided */
    return L7_FAILURE;
  }

  /* Only use switching procedure for the SET command */
  if (vlan_mode->oper == DAPI_CMD_SET)
  {
    /* All vlans */
    if (vlan_mode->vlanId == 0 || vlan_mode->vlanId > 4095)
    {
      /* In case of switch-over, move MAC addresses */
      if (hapiPortPtr_prev != L7_NULLPTR)
      {
        /* Remove old port from vlan */
        hapiBroadAddRemovePortFromVlans(&vlan_mode->ddUsp, 0, dapi_g);

        #if (PTIN_UPLINK_PROTECTION_MACMOVE)
        /* Move MACs to newer port */
        if (port_is_lag)
        {
          rv = bcmx_l2_replace(BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_NEW_TRUNK | BCM_L2_REPLACE_NO_CALLBACKS, &bcmx_l2_addr, -1, tgid);
        }
        else
        {
          rv = bcmx_l2_replace(BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_NO_CALLBACKS, &bcmx_l2_addr, hapiPortPtr->bcmx_lport, -1);
        }

        if (rv != BCM_E_NONE) 
        {
          PT_LOG_ERR(LOG_CTX_HAPI,"Error applying bcm_l2_replace (rv = %d)\r\n", rv);
        }
        #endif
      }

      /* Add the new port to all vlans */
      hapiBroadAddRemovePortFromVlans(usp, 1, dapi_g);

      /* In case of switch-over, move MAC addresses */
      if (hapiPortPtr_prev != L7_NULLPTR)
      {
        #if (!PTIN_UPLINK_PROTECTION_MACMOVE)
        /* Flush MAC entries related to old port */
        if (port_prev_is_lag)
        {
          rv = bcmx_l2_addr_delete_by_trunk(tgid_prev, BCM_L2_DELETE_NO_CALLBACKS);
        }
        else
        {
          rv = bcmx_l2_addr_delete_by_port(lport_prev, BCM_L2_DELETE_NO_CALLBACKS);
        }
        if (rv != BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI,"Error flushing MAC entries from port {%d,%d,%d} (rv=%d)\r\n",
                  vlan_mode->ddUsp.unit, vlan_mode->ddUsp.slot, vlan_mode->ddUsp.port, rv);
        }
        #endif

        PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} switched to {%d,%d,%d}",
                  vlan_mode->ddUsp.unit, vlan_mode->ddUsp.slot, vlan_mode->ddUsp.port,
                  usp->unit, usp->slot, usp->port);
      }

      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} added to all VLANS (MACs flushed)", usp->unit, usp->slot, usp->port);
    }
    else
    {
      hapiBroadL2VlanAddPortToVlanHw(usp, vlan_mode->vlanId, L7_TRUE, dapi_g);
      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} added to VLAN %u", usp->unit, usp->slot, usp->port, vlan_mode->vlanId);
    }
  }
  else if (vlan_mode->oper == DAPI_CMD_CLEAR)
  {
    if (vlan_mode->vlanId == 0 || vlan_mode->vlanId > 4095)
    {
      /* Remove the port to all vlans */
      hapiBroadAddRemovePortFromVlans(usp, 0, dapi_g);
      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} removed from all VLANS", usp->unit, usp->slot, usp->port);
    }
    else
    {
      hapiBroadL2VlanRemovePortFromVlanHw(usp, vlan_mode->vlanId, dapi_g);
      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} removed from VLAN %u", usp->unit, usp->slot, usp->port, vlan_mode->vlanId);
    }
  }
  else if (vlan_mode->oper == DAPI_CMD_CLEAR_ALL)
  {
    if (vlan_mode->vlanId == 0 || vlan_mode->vlanId > 4095)
    {
      /* Remove the port to all vlans */
      hapiBroadAddRemovePortFromVlans(usp, 0, dapi_g);

      /* Flush MAc entries */
      if (port_is_lag)
      {
        (void) bcmx_l2_addr_delete_by_trunk(tgid, BCM_L2_DELETE_NO_CALLBACKS);
      }
      else
      {
        (void) bcmx_l2_addr_delete_by_port(lport, BCM_L2_DELETE_NO_CALLBACKS);
      }
      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} removed from all VLANS (MACs flushed)", usp->unit, usp->slot, usp->port);
    }
    else
    {
      hapiBroadL2VlanRemovePortFromVlanHw(usp, vlan_mode->vlanId, dapi_g);

      /* Flush MAc entries */
      if (port_is_lag)
      {
        (void) bcmx_l2_addr_delete_by_vlan_trunk(vlan_mode->vlanId, tgid, BCM_L2_DELETE_NO_CALLBACKS);
      }
      else
      {
        (void) bcmx_l2_addr_delete_by_vlan_port(vlan_mode->vlanId, lport, BCM_L2_DELETE_NO_CALLBACKS);
      }
      PT_LOG_TRACE(LOG_CTX_HAPI, "Port {%d,%d,%d} removed from VLAN %u (MACs flushed)", usp->unit, usp->slot, usp->port, vlan_mode->vlanId);
    }
  }

  return L7_SUCCESS;
}

/**
 * Define vlan mode settings related to Multicast groups
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeVlanMulticastSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_bridge_vlan_multicast_t *mode = (ptin_bridge_vlan_multicast_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (mode->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_bridgeVlan_multicast_set(mode->vlanId, &mode->multicast_group, mode->multicast_flag);
    break;
  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_bridgeVlan_multicast_reset(mode->vlanId, mode->multicast_group, mode->multicast_flag, mode->destroy_on_clear);
    break;
  case DAPI_CMD_CLEAR_ALL:
    rc = ptin_hapi_bridgeVlan_multicast_reset(mode->vlanId, mode->multicast_group, mode->multicast_flag, mode->destroy_on_clear);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Configure Multicast egress ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinMulticastEgressPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_bridge_vlan_multicast_t *mode = (ptin_bridge_vlan_multicast_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (mode->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_multicast_egress_port_add(&mode->multicast_group, mode->multicast_flag, mode->virtual_gport, &dapiPort);
    break;
  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_multicast_egress_port_remove(mode->multicast_group, mode->multicast_flag, mode->virtual_gport, &dapiPort);
    break;
  case DAPI_CMD_CLEAR_ALL:
    rc = ptin_hapi_multicast_egress_clean(mode->multicast_group, mode->destroy_on_clear);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Configure Virtual ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinVirtualPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_dapi_port_t dapiPort;
  ptin_vport_t *vport = (ptin_vport_t *) data;
  L7_RC_t rc = L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (vport->oper)
  {
  case DAPI_CMD_SET:
    if (vport->cmd == PTIN_VPORT_CMD_VP_OPER)
    {
      rc = ptin_hapi_vp_create(&dapiPort, 
                               vport->ext_ovid, vport->ext_ivid,
                               vport->int_ovid, vport->int_ivid,
                               &vport->multicast_group,
                               &vport->virtual_gport);

      if (rc == L7_SUCCESS)
      {
        rc = ptin_hapi_vport_maclimit_setmax(vport->virtual_gport, vport->macLearnMax);
      }
    }
    else if (vport->cmd == PTIN_VPORT_CMD_MAXMAC_SET)
    {
      rc = ptin_hapi_vport_maclimit_setmax(vport->virtual_gport, vport->macLearnMax); 
    }
    break;

  case DAPI_CMD_CLEAR:
  case DAPI_CMD_CLEAR_ALL:
    if (vport->cmd == PTIN_VPORT_CMD_VP_OPER)
    {
      rc = ptin_hapi_vp_remove(&dapiPort, vport->ext_ovid, vport->ext_ivid, vport->virtual_gport, vport->multicast_group);

      if (rc == L7_SUCCESS)
      {
        rc = ptin_hapi_vport_maclimit_reset(vport->virtual_gport);
      }
    }
    else if (vport->cmd == PTIN_VPORT_CMD_MAXMAC_SET)
    {
      rc = ptin_hapi_vport_maclimit_reset(vport->virtual_gport);
    }
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

/**
 * Set crossconnections
 * 
 * @param usp 
 * @param cmd 
 * @param data: ptin_bridge_crossconnect_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapiBroadPtinBridgeCrossconnect(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bridge_crossconnect_t *crossc = (ptin_bridge_crossconnect_t *) data;
  ptin_dapi_port_t dapiPort1, dapiPort2;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI,"SrcPort={%d,%d,%d}, DstPort={%d,%d,%d}, outerVlanId=%u, innerVlanId=%u",
            usp->unit, usp->slot, usp->port,
            crossc->dstUsp.unit, crossc->dstUsp.slot, crossc->dstUsp.port,
            crossc->outerVlanId, crossc->innerVlanId);

  /* Prepare dapiPortx structures */
  DAPIPORT_SET(&dapiPort1, usp, dapi_g);
  DAPIPORT_SET(&dapiPort2, &(crossc->dstUsp), dapi_g);

  /* Execute the correct operation */
  switch ((L7_int) crossc->oper)
  {
  case DAPI_CMD_SET:
    rc = ptin_hapi_bridge_crossconnect_add(crossc->outerVlanId, crossc->innerVlanId, &dapiPort1, &dapiPort2);
    break;

  case DAPI_CMD_CLEAR:
    rc = ptin_hapi_bridge_crossconnect_delete(crossc->outerVlanId, crossc->innerVlanId);
    break;

  case DAPI_CMD_CLEAR_ALL:
    rc  = ptin_hapi_bridge_crossconnect_delete_all();
    break;
  }

  /* Return the operation result */
  return rc;
}

/**
 * BW Policers implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinBwPolicer(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_bwPolicer_t *bwPolicer = (ptin_bwPolicer_t *) data;
  L7_RC_t status=L7_SUCCESS;

  if (usp != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}", usp->unit, usp->slot, usp->port); 
  }

  /* Adapt usp data */
  if (usp != L7_NULLPTR && !IS_SLOT_TYPE_PHYSICAL(usp, dapi_g) && !IS_SLOT_TYPE_LOGICAL_LAG(usp, dapi_g))
  {
    usp->unit = usp->slot = usp->port = -1;
  }

  if (usp != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}", usp->unit, usp->slot, usp->port); 
  }

  switch (bwPolicer->operation)  {
    case DAPI_CMD_GET:
      status=hapi_ptin_bwPolicer_get(usp, bwPolicer, dapi_g);
      break;

    case DAPI_CMD_SET:
      status=hapi_ptin_bwPolicer_set(usp, bwPolicer, dapi_g);
      break;

    case DAPI_CMD_CLEAR:
      status=hapi_ptin_bwPolicer_delete(usp, bwPolicer, dapi_g);
      break;
            
  case DAPI_CMD_CLEAR_ALL:
    status=hapi_ptin_bwPolicer_deleteAll(usp, bwPolicer, dapi_g);
    break;

  case PTIN_CMD_CREATE:
    status=hapi_ptin_bwPolicer_create(usp, bwPolicer, dapi_g);
    break;

  case PTIN_CMD_DESTROY:
    status=hapi_ptin_bwPolicer_destroy(usp, bwPolicer, dapi_g);
    break;

  default:
    status = L7_FAILURE;
  }

  return status;
}

/**
 * Counters implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinFpCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  ptin_evcStats_t *fpCounter = (ptin_evcStats_t *) data;
  L7_RC_t status=L7_SUCCESS;

  switch (fpCounter->operation)  {
    case DAPI_CMD_GET:
      status=hapi_ptin_fpCounters_get(usp, &fpCounter->counters, &fpCounter->profile, dapi_g);
      break;

    case DAPI_CMD_SET:
      status=hapi_ptin_fpCounters_set(usp, &fpCounter->profile, dapi_g);
      break;

    case DAPI_CMD_CLEAR:
      status=hapi_ptin_fpCounters_delete(usp, &fpCounter->profile, dapi_g);
      break;
            
  case DAPI_CMD_CLEAR_ALL:
    status=hapi_ptin_fpCounters_deleteAll(usp, &fpCounter->profile, dapi_g);
    break;

    default:
      status = L7_FAILURE;
  }

  return status;
}

/**
 * Traffic Rate limiters
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadStormControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_BOOL enable;
  ptin_dapi_port_t    dapiPort;
  ptin_stormControl_t *stormControl = (ptin_stormControl_t *) data;
  L7_RC_t status=L7_SUCCESS;

  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  switch (stormControl->operation)  {
    case DAPI_CMD_SET:
      enable = L7_ENABLE;
      break;

    case DAPI_CMD_CLEAR:
      enable = L7_DISABLE;
      break;

    default:
      return L7_FAILURE;
  }

  /* No stormcontrol using FP rules */
#if 0
  #if (!PTIN_BOARD_IS_LINECARD)
  status = hapi_ptin_stormControl_set(&dapiPort, enable, stormControl, PTIN_PORT_EGRESS_TYPE_PROMISCUOUS);
  #endif
#endif
  //#if (!PTIN_BOARD_IS_MATRIX)
  status = hapi_ptin_stormControl_set(&dapiPort, enable, stormControl, PTIN_PORT_EGRESS_TYPE_ISOLATED);
  //#endif

  return status;
}

/**
 * Enable PRBS generator/checker
 * 
 * @param usp : portInfo
 * @param enable : enable
 * @param dapi_g : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsEnable(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  L7_int        port;
  bcm_port_t    bcm_port;
  int           speed, autoneg, duplex, tries;
  bcm_port_if_t if_type;
  L7_uint16     preemphasys[4] = {0, 0, 0, 0};
  L7_RC_t       rc = L7_SUCCESS;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  port = usp->port;
  bcm_port = hapiSlotMapPtr[port].bcm_port;

  /* If intf type is KR4, special proceedings are necessary */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
  if (dapiCardPtr->wcPortMap[port].wcSpeedG == 40)
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
  if (PTIN_SYSTEM_10G_PORTS_MASK & (1ULL << port))
#else
  if (0)
#endif
  {
    /* Read current preemphasys value */
    rc = hapiBroadPTinPrbsPreemphasisGet(usp, preemphasys, 4);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error reading preemphasys from bcm_port %u", bcm_port);
    }

    /* Enable */
    if (enable)
    {
      /* If PRBS is to be enabled, disable AN */
      if (bcm_port_autoneg_set(0, bcm_port, L7_DISABLE) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
      }
      /* Full duplex */
      if (bcm_port_duplex_set(0, bcm_port, L7_ENABLE) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
      }
      /* Set speed again */
      if (bcm_port_speed_set(0, bcm_port, 40000) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
      }
      /* Set if again */
      if (bcm_port_interface_set(0, bcm_port, BCM_PORT_IF_KR4) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
      }

      /* Wait for KR4 establishment */
      tries = 0;
      do
      {
        osapiSleepMSec(50);
      } while (((tries++) < 10) &&
               (bcm_port_speed_get    (0, bcm_port, &speed  ) != BCM_E_NONE || speed   != 40000 ||
                bcm_port_autoneg_get  (0, bcm_port, &autoneg) != BCM_E_NONE || autoneg != L7_DISABLE ||
                bcm_port_duplex_get   (0, bcm_port, &duplex ) != BCM_E_NONE || duplex  != L7_TRUE ||
                bcm_port_interface_get(0, bcm_port, &if_type) != BCM_E_NONE || if_type != BCM_PORT_IF_KR4));

      PT_LOG_INFO(LOG_CTX_HAPI, "Needed %u tries to wait for KR4 reestablishment (bcm_port %u)", bcm_port);

      /* Force previous tap settings */
      if (rc == L7_SUCCESS)
      {
        PT_LOG_INFO(LOG_CTX_HAPI, "Going to force original preemphasys (1:0x%08x, 2:0x%08x, 3:0x%08x, 4:0x%08x) to bcm_port %u",
                 preemphasys[0], preemphasys[1], preemphasys[2], preemphasys[3], bcm_port);

        rc = hapiBroadPTinPrbsPreemphasisSet(usp, preemphasys, 4, L7_TRUE);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error forcing preemphasys to bcm_port %u", bcm_port);
        }
        else
        {
          PT_LOG_INFO(LOG_CTX_HAPI, "Success forcing preemphasys to bcm_port %u", bcm_port);
        }
      }
    }
  }

  /* PRBS generated by internal PHY */
  if (bcm_port_control_set(0, bcm_port, bcmPortControlPrbsMode, 0)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR applying bcmPortControlPrbsMode 0 to port %d, bcm_port=%d",enable, port, bcm_port);
    return L7_FAILURE;
  }
  /* PRBS polynomial 31 */
  if (bcm_port_control_set(0, bcm_port, bcmPortControlPrbsPolynomial, BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR applying bcmPortControlPrbsMode 0 to port %d, bcm_port=%d",enable, port, bcm_port);
    return L7_FAILURE;
  }
  /* For OLT1T1, SFI and XFI modes, invert PRBS sequence */
#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
  if (dapiCardPtr->wcPortMap[port].wcSpeedG == 10)
  {
    if (bcm_port_control_set(0, bcm_port, bcmPortControlPrbsTxInvertData, enable & 1)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "ERROR applying enable state %u to port %d, bcm_port=%d",enable, port, bcm_port);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "Success inverting PRBS sequence to port %d, bcm_port=%d", port, bcm_port);
  }
#endif
  /* PRBS enable */
  if (bcm_port_control_set(0, bcm_port, bcmPortControlPrbsTxEnable, enable & 1)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR applying enable state %u to port %d, bcm_port=%d",enable, port, bcm_port);
    return L7_FAILURE;
  }

  /* Restore KR4 mode */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)
  if (dapiCardPtr->wcPortMap[port].wcSpeedG == 40)
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
  if (PTIN_SYSTEM_10G_PORTS_MASK & (1ULL << port))
#else
  if (0)
#endif
  {
    if (!enable)
    {
      /* Restore tap settings */
      rc = hapiBroadPTinPrbsPreemphasisSet(usp, preemphasys, 4, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error restoring preemphasys to bcm_port %u", bcm_port);
      }
      else
      {
        PT_LOG_INFO(LOG_CTX_HAPI, "Success restoring preemphasys to bcm_port %u", bcm_port);
      }

      /* KR4 mode again */
      if (ptin_hapi_kr4_set(bcm_port) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error restoring KR4 mode at bcm_port %u", bcm_port);
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "PRBS TX/RX %s for port {%d,%d,%d}, bcm_port=%d",
            ((enable) ? "enabled" : "disabled"),
            port, bcm_port);

  return L7_SUCCESS;
}

/**
 * Read number of PRBS errors
 * 
 * @param usp : portInfo
 * @param rxErrors : number of errors (-1 if no lock)
 * @param dapi_g   : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsRxStatus(DAPI_USP_t *usp, L7_uint32 *rxErrors, DAPI_t *dapi_g)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  bcm_port_t  bcm_port;
  int         rxStatus;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  bcm_port = hapiSlotMapPtr[usp->port].bcm_port;

  if (bcm_port_control_get(0, bcm_port, bcmPortControlPrbsRxStatus, &rxStatus)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR reading rx status from port {%d,%d,%d}, bcm_port=%d",usp->unit,usp->slot,usp->port,bcm_port);
    return L7_FAILURE;
  }

  if (rxErrors!=L7_NULLPTR)
  {
    *rxErrors = (rxStatus==-1)  ? ((L7_uint32) -1) : rxStatus;
  }

  return L7_SUCCESS;
}

/**
 * Get preemphasys data
 * 
 * @param usp 
 * @param preemphasys 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t hapiBroadPTinPrbsPreemphasisGet(DAPI_USP_t *usp, L7_uint16 *preemphasys, L7_int number_of_lanes)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  bcm_port_t  bcm_port;
  L7_int linkscan;
  L7_int lane;
  L7_uint32 phy_data, saved_data;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  bcm_port = hapiSlotMapPtr[usp->port].bcm_port;

  /* Disable linkscan */
  if (bcm_linkscan_enable_get(0, &linkscan) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error reading linkscan mode");
    return L7_FAILURE;
  }
  if (bcm_linkscan_enable_set(0, 0) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error disablink linkscan to all ports");
    return L7_FAILURE;
  }

  /* Run all lanes */
  for (lane = 0; lane < number_of_lanes; lane++)
  {
    preemphasys[lane] = 0;

    /* Going to select lane */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0xffd0);
    if (rc != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }
    /* AER + lane */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1e, lane);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }
    /* Block 0x8060 */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0x8060);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      return L7_FAILURE;
    }
    /* Select Tap settings (0x8063:14) */
    rc = bcm_port_phy_get(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x13, &saved_data);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error reading register from port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x13, saved_data | (1<<14));
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      return L7_FAILURE;
    }
    /* Read preemphasys */
    rc = bcm_port_phy_get(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x10, &phy_data);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error reading register from port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }

    /* Restore 0x8063 register */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x13, saved_data);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      return L7_FAILURE;
    }

    /* Save preemphasys for this lane */
    preemphasys[lane] = phy_data & 0xffff;

    PT_LOG_INFO(LOG_CTX_HAPI, "Port %-2u (bcm_port %-2u) lane %u: preemphasys=0x%04x", usp->port, bcm_port, lane, preemphasys[lane]);
  }

  /* Reset lane */
  rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0xffd0);
  if (rc != BCM_E_NONE) 
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
    rc_global = rc;
  }
  else
  {
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1e, 0); 
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
    }
  }

  /* Restore linkscan mode */
  rc = bcm_linkscan_enable_set(0, linkscan);
  if (rc != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error restoring linkscan mode to all ports");
    rc_global = rc;
  }

  return rc_global;
}

/**
 * Set preemphasys data
 * 
 * @param usp 
 * @param preemphasys 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t hapiBroadPTinPrbsPreemphasisSet(DAPI_USP_t *usp, L7_uint16 *preemphasys, L7_int number_of_lanes, L7_BOOL force)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  bcm_port_t  bcm_port;
  L7_int linkscan;
  L7_int lane;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;

  /* Validate usp reference */
  if ( usp->unit != 1 ||
       usp->slot != 0 ||
       usp->port >= dapiCardPtr->numOfSlotMapEntries )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "ERROR: Invalid port reference {%d,%d,%d}",usp->unit,usp->slot,usp->port);
    return L7_FAILURE;
  }

  bcm_port = hapiSlotMapPtr[usp->port].bcm_port;

  /* Disable linkscan */
  if (bcm_linkscan_enable_get(0, &linkscan) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error reading linkscan mode");
    return L7_FAILURE;
  }
  if (bcm_linkscan_enable_set(0, 0) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error disablink linkscan to all ports");
    return L7_FAILURE;
  }

  /* Run all lanes */
  for (lane = 0; lane < number_of_lanes; lane++)
  {
    /* Set or clear bit 15 */
    preemphasys[lane] = (preemphasys[lane] & 0x7fff) | ((force & 1)<<15);

    /* Going to select lane */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0xffd0);
    if (rc != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }
    /* AER + lane */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1e, lane);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }
    /* Block 0x8060 */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0x82e0);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      return L7_FAILURE;
    }
    /* Set preemphasys */
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x12, preemphasys[lane]);
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
      continue;
    }

    PT_LOG_INFO(LOG_CTX_HAPI, "Port %-2u (bcm_port %-2u) lane %u: preemphasys=0x%04x", usp->port, bcm_port, lane, preemphasys[lane]);
  }

  /* Reset lane */
  rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1f, 0xffd0);
  if (rc != BCM_E_NONE) 
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
    rc_global = rc;
  }
  else
  {
    rc = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, 0x1e, 0); 
    if (rc != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error modifying register to port %u (bcm_port %u)", usp->port, bcm_port);
      rc_global = rc;
    }
  }

  /* Restore linkscan mode */
  rc = bcm_linkscan_enable_set(0, linkscan);
  if (rc != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error restoring linkscan mode to all ports");
    rc_global = rc;
  }

  return rc_global;
}

/**
 * Debug tool to dump current tap settings
 * 
 * @param port 
 */
void ptin_tapsettings_dump(L7_uint port)
{
  DAPI_USP_t usp;
  L7_uint   i;
  L7_uint16 preemphasys[4] = {0, 0, 0, 0};

  if (port >= ptin_sys_number_of_ports)
  {
    printf("Invalid port %u\r\n", port);
    return;
  }

  usp.unit = 1;
  usp.slot = 0;
  usp.port = port;

  if (hapiBroadPTinPrbsPreemphasisGet(&usp, preemphasys, 4) != L7_SUCCESS)
  {
    printf("Error reading current tap settings of port %u\r\n", port);
    return;
  }

  printf("Preemphasys for port %u (0x8060 with 0x8063:14=1):\r\n", port);
  for (i = 0; i < 4; i++)
  {
    printf(" Lane %u: 0x%08x (pre=%2u main=%2u post=%2u)\r\n", i, preemphasys[i],
           preemphasys[i] & 0xf, (preemphasys[i]>>4) & 0x3f, (preemphasys[i]>>10) & 0x1f);
  }

  fflush(stdout);
}

/**
 * Debug tool to set new tap settings
 * 
 * @param port 
 * @param data 
 * @param force 
 */
void ptin_tapsettings_set(L7_uint port, L7_uint16 pre, L7_uint16 main, L7_uint16 post, L7_uint force)
{
  DAPI_USP_t usp;
  L7_uint   i;
  L7_uint16 preemphasys[4] = {0, 0, 0, 0};

  if (port >= ptin_sys_number_of_ports)
  {
    printf("Invalid port %u\r\n", port);
    return;
  }

  printf("New preemphasys for port %u (0x82e2):\r\n", port);
  for (i = 0; i < 4; i++)
  {
    preemphasys[i] = (pre & 0xf) | ((main & 0x3f)<<4) | ((post & 0x1f)<<10) | ((force & 1)<<15);

    printf(" Lane %u: 0x%08x (pre=%2u main=%2u post=%2u force=%u)\r\n", i, preemphasys[i],
           preemphasys[i] & 0xf, (preemphasys[i]>>4) & 0x3f, (preemphasys[i]>>10) & 0x1f, (preemphasys[i]>>15) & 1);
  }

  usp.unit = 1;
  usp.slot = 0;
  usp.port = port;

  if (hapiBroadPTinPrbsPreemphasisSet(&usp, preemphasys, 4, force) != L7_SUCCESS)
  {
    printf("Error setting new tap settings of port %u\r\n", port);
    return;
  }

  printf("Done!\r\n");

  fflush(stdout);
}

/**
 * Get system resources
 * 
 * @param usp 
 * @param cmd 
 * @param data : (st_ptin_policy_resources)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinResourcesGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_policy_resources *resources = (st_ptin_policy_resources *) data;

  return ptin_hapi_policy_resources_get(resources);
}


/**
 * Add L3 Host IP
 * 
 * @param usp 
 * @param cmd 
 * @param data :
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinL3Manage(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  st_ptin_l3 *ptr = (st_ptin_l3 *) data;
  ptin_dapi_port_t dapiPort;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "usp={%d,%d,%d}",usp->unit, usp->slot, usp->port);

  /* Validate interface */
  if ( usp->unit<0 || usp->slot<0 || usp->port<0 )
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"USP not provided");
    return L7_FAILURE;
  }

  /* Prepare dapiPort structure */
  DAPIPORT_SET(&dapiPort, usp, dapi_g);

  if (ptr->oper == PTIN_L3_MANAGE_HOST)
  {
    switch (ptr->cmd) 
    {
      case DAPI_CMD_SET:
        rc = ptin_hapi_l3_host_add(&dapiPort, ptr);
        break;

      case DAPI_CMD_CLEAR:
        rc = ptin_hapi_l3_host_remove(&dapiPort, ptr);
        break;

      default:
        rc = L7_FAILURE;
        PT_LOG_ERR(LOG_CTX_HAPI,"Command not handled (%u)", ptr->cmd);
    }
  }
  else if (ptr->oper == PTIN_L3_MANAGE_ROUTE)
  {
    switch (ptr->cmd) 
    {
      case DAPI_CMD_SET:
        rc = ptin_hapi_l3_route_add(&dapiPort, ptr);
        break;

      case DAPI_CMD_CLEAR:
        rc = ptin_hapi_l3_route_remove(&dapiPort, ptr);
        break;

      default:
        rc = L7_FAILURE;
        PT_LOG_ERR(LOG_CTX_HAPI,"Command not handled (%u)", ptr->cmd);
    }
  }
  else
  {
    rc = L7_FAILURE;
    PT_LOG_ERR(LOG_CTX_HAPI,"Operation not implemented (%u)", ptr->oper);
  }

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error: rc=%u", rc);
  }

  return rc;
}

/*
L7_RC_t hapiBroadPtinStart(void)
{
  printf("hapiBroadPtinStart start\n");

  if (hapi_ptin_flow_init())  return L7_FAILURE;

  printf("hapiBroadPtinStart end\n");

  return L7_SUCCESS;
}
*/

/**
 * Read temperature sensors
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_temperature_monitor(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  ptin_dtl_temperature_monitor_t *temp_info = (ptin_dtl_temperature_monitor_t *) data;

  return ptin_hapi_temperature_monitor(temp_info);

}//broad_ptin_temperature_monitor





L7_RC_t hapiBroadPtinMEPControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g) {
    T_MEP_HDR         *p;

    p= ((hapi_mep_t *) data)->m;

    if (EMPTY_T_MEP(*p))    return hapiBroadPtinMEPDelete(usp, cmd, data, dapi_g);
    else                    return hapiBroadPtinMEPCreate(usp, cmd, data, dapi_g);
}//hapiBroadPtinMEPControl




L7_RC_t hapiBroadPtinMEPCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g) {
  DAPI_PORT_t       *dapiPortPtr;
  BROAD_PORT_t      *hapiPortPtr;
  T_MEP_HDR         *p;
  T_MEP_LM          *lm;
  //T_ME               *_p_me;

  //bcm_error_t rv;
  bcm_oam_group_info_t ginfo;
  bcm_oam_endpoint_info_t mep;
  //bcm_oam_loss_t loss;
  int r, alrdy;
  unsigned long i, empty;


  // Get the port info
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  p= ((hapi_mep_t *) data)->m;
  lm= ((hapi_mep_t *) data)->lm;

  //PT_LOG_TRACE(LOG_CTX_API, ,);

  //if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr_prev) == L7_TRUE)
  //else if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr_prev) == L7_TRUE)



  //Check if MAID's already in use...
  for (i=0, empty=-1, alrdy=0; i<N_MEPs; i++) {    //nMEGsMAIDs<=MEPs
      r=bcm_oam_group_get(0, i, &ginfo);
      if (BCM_E_NOT_FOUND==r) {
          if (empty>=N_MEPs) empty=i;
      }
      else
      if (BCM_E_EXISTS==r) {
          if (!memcmp(ginfo.name, &p->meg_id, sizeof(ginfo.name))) {
              ginfo.id=i;
              alrdy=1;
              break;
          }
      }
  }//for
  //... otherwise allocate it
  if (!alrdy) {
      if (empty>=N_MEPs) return L7_TABLE_IS_FULL;

      bcm_oam_group_info_t_init(&ginfo);
      ginfo.id=empty;   //1;
      memcpy(ginfo.name, &p->meg_id, sizeof(ginfo.name));   //memset(ginfo.name, 0, sizeof(ginfo.name));    //sprintf(ginfo.name, "");
      ginfo.flags= BCM_OAM_GROUP_WITH_ID; //| BCM_OAM_GROUP_REMOTE_DEFECT_TX;
      r=bcm_oam_group_create(0, &ginfo);
      if (BCM_E_NONE!=r) {
          PT_LOG_ERR(LOG_CTX_API, "bcm_oam_group_create()=%d\n\r", r);
          return L7_DEPENDENCY_NOT_MET;
      }
  }




  //MEP allocation
  bcm_oam_endpoint_info_t_init(&mep);
  mep.flags=
      //BCM_OAM_ENDPOINT_REPLACE | 
      //BCM_OAM_ENDPOINT_REMOTE |
      //BCM_OAM_ENDPOINT_INTERMEDIATE |

      //BCM_OAM_ENDPOINT_USE_QOS_MAP | BCM_OAM_ENDPOINT_PRI_TAG |

      BCM_OAM_ENDPOINT_CCM_COPYTOCPU |// BCM_OAM_ENDPOINT_CCM_RX |
      //BCM_OAM_ENDPOINT_CCM_DROP | BCM_OAM_ENDPOINT_CCM_COPYFIRSTTOCPU |
      //BCM_OAM_ENDPOINT_REMOTE_DEFECT_TX | BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE |
      //BCM_OAM_ENDPOINT_REMOTE_DEFECT_AUTO_UPDATE |

      //BCM_OAM_ENDPOINT_LOOPBACK | BCM_OAM_ENDPOINT_LINKTRACE |
      BCM_OAM_ENDPOINT_DELAY_MEASUREMENT | BCM_OAM_ENDPOINT_DM_COPYTOCPU |
      //BCM_OAM_ENDPOINT_DM_DROP |
      BCM_OAM_ENDPOINT_LB_COPYTOCPU |
      //BCM_OAM_ENDPOINT_LB_DROP |
      BCM_OAM_ENDPOINT_LT_COPYTOCPU |
      //BCM_OAM_ENDPOINT_LT_DROP |

      //BCM_OAM_ENDPOINT_PORT_STATE_TX | BCM_OAM_ENDPOINT_INTERFACE_STATE_TX |
      //BCM_OAM_ENDPOINT_PORT_STATE_UPDATE | BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE |
      //BCM_OAM_ENDPOINT_MATCH_INNER_VLAN | BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN |        // Selection of MEP based on S and C VLAN
      BCM_OAM_ENDPOINT_WITH_ID;

  //if (!invalid_T_MEP_LM(lm))
      mep.flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
 if (p->up1_down0) mep.flags |= BCM_OAM_ENDPOINT_UP_FACING;

  //mep.opcode_flags
  //mep.lm_flags
 
  //memcpy(&lm->endpoint_id, &mep.id, sizeof(lm->endpoint_id)); 

  mep.id=           1;//((hapi_mep_t *) data)->imep;  //1..N (creation function doesn't accept 0)
  mep.type=         bcmOAMEndpointTypeEthernet;
  mep.group=        ginfo.id;
  mep.name=         p->mep_id; //0;//mepid
  //mep.local_id=0;  // Used by remote endpoints only
  mep.level=        p->level; //0;
  mep.ccm_period=   0; //OAM_TMR_CODE_TO_ms[p->tmout];
  mep.vlan=         p->vid; //50;
  mep.inner_vlan=   0;  //...or set to 0 for one-tag
  mep.gport=        mep.tx_gport=       hapiPortPtr->bcmx_lport;//BCM_GPORT_LOCAL_SET(mep.tx_gport, 2);
  //mep.trunk_index= //The trunk port index for this
  //mep.intf_id=
  //mep.mpls_label=
  //mep.egress_label=
  {
   struct {
       bcm_mac_t v;
   } d={{0x01, 0x80, 0xC2, 0x00, 0x00, 0x30}};//, s={{0, 1, 2, 3, 4, 5}};

   d.v[5]|= p->level;
   memcpy(mep.dst_mac_address, &d, 6);
   {//SRC MAC ADDRESS
     L7_uint32 intIfNum;
     nimUSP_t  nim_usp;
         nim_usp.unit= usp->unit;
         nim_usp.slot= usp->slot;
         nim_usp.port= usp->port+1;
         if (L7_SUCCESS!=nimGetIntIfNumFromUSP(&nim_usp, &intIfNum)
             ||
             L7_SUCCESS!=nimGetIntfAddress(intIfNum, L7_SYSMAC_BIA, mep.src_mac_address)) {

             r=L7_REQUEST_DENIED;
             PT_LOG_ERR(LOG_CTX_HAPI, "couldn't get SMAC\n\r");
             goto _hapiBroadPtinMEPCreate_nokend;
         }//memcpy(mep.src_mac_address, &s, 6);
   }
  }
  //mep.pkt_pri=
  //mep.inner_pkt_pri=
  //mep.inner_tpid=
  //mep.outer_tpid=
  //mep.int_pri=
  //mep.cpu_qid=
  //uint8 pri_map[BCM_OAM_INTPRI_MAX];  // Priority mapping for LM counter table

  //mep.faults=// Fault flags
  //mep.persistent_faults=// Persistent fault flags
  //mep.clear_persistent_faults=// Persistent fault flags to clear on a
  //mep.ing_map=                        // Ingress QoS map profile
  //mep.egr_map=                        // Egress QoS map profile

  //mep.ms_pw_ttl
  //mep.port_state;                   // Endpoint port status
  //mep.interface_state;              // Endpoint interface status

  mep.lm_counter_base_id=mep.id;//0;             // Counter id assosiated to the mep
  //mep.loc_clear_threshold;          // Number of packets required to reset the Loss-of-Continuity status per end point
  //mep.timestamp_format;               // DM time stamp format - NTP/IEEE1588(PTP)


  //PT_LOG_TRACE(LOG_CTX_API, "Success -> %d \n\r", mep.id);
  r=bcm_oam_endpoint_create(0, &mep);
  if (BCM_E_NONE!=r) PT_LOG_ERR(LOG_CTX_HAPI, "bcm_oam_endpoint_create()=%d\n\r", r);

 /*
  bcm_oam_loss_t_init(&loss);
  loss.flags |=              BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_TX_ENABLE | BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_ALL_RX_COPY_TO_CPU;
  //loss.flags &=              ~(0UL | BCM_OAM_LOSS_COUNT_GREEN_AND_YELLOW);

  loss.loss_id=              1;//Oam Loss ID
  loss.id=                   1;
  //loss.rem_id=               2;
  loss.period=               1000;
  //loss.loss_threshold=
  //loss.loss_nearend;
  //loss.loss_farend;
  //loss.tx_nearend;
  //loss.rx_nearend;
  //loss.tx_farend;
  //loss.rx_farend;
  loss.pkt_pri_bitmap=0;
  loss.pkt_dp_bitmap=0;
  loss.pkt_pri=0;
  loss.int_pri=0;
  //loss.gport;
  //loss.rx_oam_packets;
  //loss.tx_oam_packets;
  bcm_oam_loss_add(0, &loss);

  PT_LOG_TRACE(LOG_CTX_API, " loss.rx_oam_packets %d ", loss.rx_oam_packets);
  PT_LOG_TRACE(LOG_CTX_API, " loss.tx_oam_packets %d ", loss.tx_oam_packets);
  PT_LOG_TRACE(LOG_CTX_API, " loss.rx_oam_packets %d ", loss.rx_oam_packets);
  PT_LOG_TRACE(LOG_CTX_API, " loss.rx_oam_packets %d ", loss.tx_oam_packets);
  PT_LOG_TRACE(LOG_CTX_API, " loss.tx_nearend %d ", loss.tx_nearend);
  PT_LOG_TRACE(LOG_CTX_API, " loss.rx_nearend %d ", loss.rx_nearend);
  PT_LOG_TRACE(LOG_CTX_API, " loss.tx_farend %d ", loss.tx_farend);
  PT_LOG_TRACE(LOG_CTX_API, " loss.rx_farend %d ", loss.rx_farend);
  
  */
  if (BCM_E_NONE==r) return L7_SUCCESS;

_hapiBroadPtinMEPCreate_nokend:
  if (!alrdy) {
      bcm_oam_group_destroy(0, ginfo.id);
      PT_LOG_TRACE(LOG_CTX_API, "ERROR\n\r", r);
  }

  return r;
}//hapiBroadPtinMEPCreate




L7_RC_t hapiBroadPtinMEPDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g) {
int r;
unsigned long i, group_id=-1;
bcm_oam_endpoint_info_t mep;

    r=bcm_oam_endpoint_get(0, 1+((hapi_mep_t *) data)->imep, &mep); //1..N
    if (BCM_E_NONE!=r) {PT_LOG_ERR(LOG_CTX_HAPI, "bcm_oam_endpoint_get()=%d\n\r", r); return L7_NOT_EXIST;}
    group_id=mep.group;

    r=bcm_oam_endpoint_destroy(0, 1+((hapi_mep_t *) data)->imep);   //1..N
    if (BCM_E_NONE!=r) {PT_LOG_ERR(LOG_CTX_HAPI, "bcm_oam_endpoint_destroy()=%d\n\r", r); return L7_FAILURE;}

    //Delete MEG/MA if unused by any other MEP
    for (i=1; i<=N_MEPs; i++) {                                      //1..N
        if (1+((hapi_mep_t *) data)->imep == i) continue;           //no sense processing the MEP we've just deleted
        if (BCM_E_NONE!=bcm_oam_endpoint_get(0, i, &mep)) continue;
        if (mep.group==group_id) return L7_SUCCESS;                 //at least one more MEP's using this MEG_ID/MAID
    }

    r=bcm_oam_group_destroy(0, group_id);   //1..N
    if (BCM_E_NONE!=r) PT_LOG_ERR(LOG_CTX_HAPI, "bcm_oam_endpoint_destroy()=%d\n\r", r);
    return r;
}//hapiBroadPtinMEPDelete


extern DAPI_t *dapi_g;

void hapitest(unsigned long intIfNum, unsigned short vid, unsigned char level, unsigned char prior) {
nimUSP_t  nim_usp;
DAPI_USP_t usp;
hapi_mep_t mep;
T_MEP_HDR  m;
T_MEP_LM   lm;

    nimGetUnitSlotPort(intIfNum, &nim_usp);
    usp.unit= nim_usp.unit;
    usp.slot= nim_usp.slot;
    usp.port= nim_usp.port-1;
    
    mep.imep=0;
    mep.m=&m;
    mep.lm=&lm;
    lm.CCMs0_LMMR1=1;
    memset(&m.meg_id, 0, 48);
    sprintf((char*)&m.meg_id, "%c%c%cbaby faceABCD", 1, 32, 13);
    m.vid=vid;
    m.mep_id=1;
    m.level=level;
    m.tmout=0;
    m.prt=10;//0;

    m.prior=prior;
    m.up1_down0=0;
    //u8  CoS, dummy_color;


    hapiBroadPtinMEPControl(&usp, 0, &mep, dapi_g);
}

void hapitest2(unsigned long intIfNum, unsigned short vid, unsigned char level, unsigned char prior) {
nimUSP_t  nim_usp;
DAPI_USP_t usp;
hapi_mep_t mep;
T_MEP_HDR m;
T_MEP_LM  lm;

    nimGetUnitSlotPort(intIfNum, &nim_usp);
    usp.unit= nim_usp.unit;
    usp.slot= nim_usp.slot;
    usp.port= nim_usp.port-1;
 
    mep.imep=0;
    mep.m=&m;
    mep.lm=&lm;
    lm.CCMs0_LMMR1=-1;
    sprintf((char*)&m.meg_id, "%c%c%cbaby faceABCD", 1, 32, 13);
    mep.imep=0;
    m.vid=vid;
    m.mep_id=-1;
    m.level=level;
    m.tmout=0;
    m.prt=10;//0;
        
    m.prior=prior;
    m.up1_down0=0;
    //u8  CoS, dummy_color;


    hapiBroadPtinMEPControl(&usp, 0, &mep, dapi_g);
}

/**
 * Send LMM
 * 
 * @param usp 
 * @param operation 
 * @param dataSize 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t broad_ptin_oam_sendlmm(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{
  #ifdef __Y1731_802_1ag_OAM_ETH__

  #if(PTIN_BOARD == PTIN_BOARD_TA48GE)

  bcm_mac_t mac_src;
  bcm_mac_t mac_dst;
  ptin_send_lmm_t entry;

  memcpy(&entry, (ptin_send_lmm_t *) data, sizeof(entry));

  bcm_oam_endpoint_info_t endpoint; 
  //bcm_port_t port;
  endpoint.id = 1;//; entry.endpoint; fixed number, only endpoint allowed
  int unit = 0;

  memcpy(mac_src, (u8*)entry.dMAC , sizeof(mac_src));
  memcpy(mac_dst, (u8*)entry.sMAC , sizeof(mac_dst));

  bcm_oam_endpoint_get(unit, endpoint.id, &endpoint);

  endpoint.flags = 0x400; // BCM_OAM_ENDPOINT_LOSS_MEASUREMENT
                        
  broadPtin_oam_tx(unit, endpoint.flags, endpoint.gport, &mac_dst, &mac_src, &endpoint);

  #endif // (PTIN_BOARD == PTIN_BOARD_TA48GE)

  #endif

  return L7_SUCCESS;
}
L7_RC_t broad_ptin_oam_check_lm_counters(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g)
{

  #ifdef __Y1731_802_1ag_OAM_ETH__

  #if(PTIN_BOARD == PTIN_BOARD_TA48GE)
    uint32 *ptr;
    ptr = (uint32*) data;

    PT_LOG_TRACE(LOG_CTX_HAPI, "%s: usp={%d,%d,%d} operation=%u dataSize=%u", __FUNCTION__, usp->unit, usp->slot, usp->port, operation, dataSize);
    //soc_mem_info_t *memp;
    int  i;
    soc_mem_t mem = 5872; // Fixed position in SOC mem 
    uint32    entry[SOC_MAX_MEM_WORDS];

    /* Rx
    int k = 0;
    int entry_dw = soc_mem_entry_words(0, mem);

    i = soc_mem_read(0, mem, 7, k, entry);
    */

    // Tx
    int k1 = 8192;


    // Read Memory from SoC
    i = soc_mem_read(0, mem, 7, k1, entry);

    *ptr = entry[0];  // Counter Value

    #endif // PTIN_BOARD == PTIN_BOARD_TA48GE

    #endif

    return L7_SUCCESS;
}


L7_RC_t broadPtin_oam_tx(L7_int unit, L7_int flags, bcm_gport_t gport_dst, bcm_mac_t *mac_dst, bcm_mac_t *mac_src, bcm_oam_endpoint_info_t *endpoint_info)
{

  #ifdef __Y1731_802_1ag_OAM_ETH__ 

  #if(PTIN_BOARD == PTIN_BOARD_TA48GE)

    bcm_pkt_t pkt;
    enet_hdr_t *ep = NULL;
    oam_hdr_t *op = NULL;
    int vp = 0;        
    int rv = 0;
    bcm_module_t module_id, dst_module_id;
    bcm_port_t port_id, dst_port_id;
    bcm_trunk_t trunk_id, dst_trunk_id;
    int local_id, dst_local_id;
    soc_higig_hdr_t *xgh = (soc_higig_hdr_t *)pkt._higig;

    sal_memset(&pkt, 0, sizeof(bcm_pkt_t));
    
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, endpoint_info->gport,
        &module_id, &port_id, &trunk_id, &local_id));

    if (BCM_GPORT_IS_MIM_PORT(endpoint_info->gport) ||
        BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        vp = 1;
    }

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) ||
         BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport_dst,
            &dst_module_id, &dst_port_id, &dst_trunk_id, &dst_local_id));
    }

    sal_memset(xgh, 0, sizeof(pkt._higig));
    soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
    soc_higig_field_set(unit, xgh, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig_field_set(unit, xgh, HG_cos, 0x7);
    
    if (vp) {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x2);
        soc_higig_field_set(unit, xgh, HG_fwd_type, 0x4);
        soc_higig_field_set(unit, xgh, HG_multipoint, 0x0);
        soc_higig_field_set(unit, xgh, HG_dst_type, 0);
            soc_higig_field_set(unit, xgh, HG_src_type, 0);
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            soc_higig_field_set(unit, xgh, HG_dst_vp, dst_local_id);
            /* coverity[copy_paste_error : FALSE] */
            soc_higig_field_set(unit, xgh, HG_src_vp, local_id);
        } else {
            soc_higig_field_set(unit, xgh, HG_dst_vp, local_id);
            soc_higig_field_set(unit, xgh, HG_src_vp, local_id);
        }
    } else {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x0);
        soc_higig_field_set(unit, xgh, HG_vlan_tag, endpoint_info->vlan);
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            soc_higig_field_set(unit, xgh, HG_dst_mod, dst_module_id);
            soc_higig_field_set(unit, xgh, HG_dst_port, dst_port_id);
            if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
                soc_higig_field_set(unit, xgh, HG_tgid, trunk_id);
                soc_higig_field_set(unit, xgh, HG_src_t, 1);
            } else {
                soc_higig_field_set(unit, xgh, HG_src_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, port_id);
            }
        } else {
            if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
                soc_higig_field_set(unit, xgh, HG_dst_mod, dst_module_id);
                soc_higig_field_set(unit, xgh, HG_dst_port, dst_port_id);
                soc_higig_field_set(unit, xgh, HG_src_mod, dst_module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, 0);
            } else {
                soc_higig_field_set(unit, xgh, HG_dst_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_dst_port, port_id);
                soc_higig_field_set(unit, xgh, HG_src_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, 0);
            }
        }
    }

    pkt.alloc_ptr = (uint8 *)soc_cm_salloc(unit, 128, "TX");        
    if (pkt.alloc_ptr == NULL) {           
        return (BCM_E_MEMORY);
    } else {        
        pkt._pkt_data.data = pkt.alloc_ptr;        
        pkt.pkt_data = &pkt._pkt_data;        
        pkt.blk_count = 1;        
        pkt._pkt_data.len = 128;    
    }        

    /* setup the packet */    
    pkt.flags &= ~BCM_TX_CRC_FLD;    
    pkt.flags |= BCM_TX_CRC_REGEN; 
    pkt.flags |= BCM_TX_HG_READY | BCM_TX_ETHER;
    
    sal_memset(pkt.pkt_data[0].data, 0, pkt.pkt_data[0].len);        
    ep = (enet_hdr_t *)(pkt.pkt_data[0].data);
    
    ENET_SET_MACADDR(ep->en_dhost, mac_dst);    
    ENET_SET_MACADDR(ep->en_shost, mac_src);
    
    ep->en_tag_tpid = bcm_htons(0x8100);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(5, 0, endpoint_info->vlan));
    ep->en_tag_len  = bcm_htons(0x8902);

    op = (oam_hdr_t *)(&ep->en_snap_dsap);
    op->mdl_ver = (endpoint_info->level << 5);
    op->flags = 0;
    if (flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
        op->opcode = 43;
        op->first_tlvoffset = 12;
    } else {
        op->opcode = 47;
        op->first_tlvoffset = 32;
    }

    if ((rv = bcm_tx(unit, &pkt, NULL)) != BCM_E_NONE) {        
    }

    soc_cm_sfree(unit, pkt.alloc_ptr);

    #endif   //PTIN_BOARD == PTIN_BOARD_TA48GE)

    #endif

    return L7_SUCCESS;
}















L7_RC_t broad_ptin_time_interface(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g) {
    if (BCM_E_NONE!=time_interface_enable(usp, data, dapi_g)) return L7_FAILURE;
    return L7_SUCCESS;
}//broad_ptin_time_interface




















L7_RC_t broad_ptin_ptp_fpga_entry(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g) {
ptin_dapi_port_t dapiPort;

    dapiPort.usp = usp;
    dapiPort.dapi_g = dapi_g;

    switch (operation) {
    case DAPI_CMD_SET:  return ptin_hapi_ptp_entry_add(&dapiPort, data);
    case DAPI_CMD_CLEAR:return ptin_hapi_ptp_entry_del(&dapiPort, data);
    default: break;
    }

    return L7_FAILURE;
}//broad_ptin_ptp_fpga_entry




L7_RC_t broad_ptin_oam_fpga_entry(DAPI_USP_t *usp, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data, DAPI_t *dapi_g) {
ptin_dapi_port_t dapiPort;

    dapiPort.usp = usp;
    dapiPort.dapi_g = dapi_g;

    switch (operation) {
    case DAPI_CMD_SET:  return ptin_hapi_oam_entry_add(&dapiPort, data);
    case DAPI_CMD_CLEAR:return ptin_hapi_oam_entry_del(&dapiPort, data);
    default: break;
    }

    return L7_FAILURE;
}//broad_ptin_oam_fpga_entry



















#ifdef __DEBUGGING_DEI_CFI_2_COLOR_2_DEI_CIF__
L7_RC_t hapit3(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g) {
bcm_port_t bcm_port;
//bcm_color_t bcm_color;
int i, r;

 r=bcm_switch_control_set (0, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);   printf("\tbcm_switch_control_set()=%d",r);

 for (bcm_port=1; bcm_port<=2; bcm_port++) {
     printf("\nbcm_port=%d\n", bcm_port);
     r=bcm_port_cfi_color_set(0, bcm_port, 0, bcmColorGreen);   printf("\tbcm_port_cfi_color_set()=%d",r);
     r=bcm_port_cfi_color_set(0, bcm_port, 1, bcmColorYellow);   printf("\tbcm_port_cfi_color_set()=%d",r);
     for (i=0; i<8; i++) {
         printf("\n\tpri=%d\n", i);
         r=bcm_port_vlan_priority_unmap_set(0, bcm_port, i, bcmColorGreen, i, 0);   printf("\tbcm_port_vlan_priority_unmap_set()=%d",r);
         r=bcm_port_vlan_priority_unmap_set(0, bcm_port, i, bcmColorYellow, i, 1);   printf("\tbcm_port_vlan_priority_unmap_set()=%d",r);
     }
     r=bcm_port_control_set(0, bcm_port, bcmPortControlEgressVlanPriUsesPktPri, 1); printf("\tbcm_port_control_set()=%d",r);
 }
 return L7_SUCCESS;
}
L7_RC_t hapit4(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g) {
bcm_port_t bcm_port;
int r;

 for (bcm_port=1; bcm_port<=2; bcm_port++) {
     r=bcm_port_control_set(0, bcm_port, bcmPortControlEgressVlanPriUsesPktPri, 0); printf("\tbcm_port_control_set()=%d",r);
 }
 return L7_SUCCESS;
}









#include <bcm/qos.h>
#undef BCM_IF_ERROR_RETURN
#define BCM_IF_ERROR_RETURN(op) {bcm_error_t r; r=op; printf("%s: f()=%d\n\r", __FUNCTION__,r); if (r) return r;}

/*
 * Function: create_entry()
 *
 * Create an IFP entry that matches on a specified VLAN ID. Attach a
 * color-aware policer with hard coded rates to the entry. Packets exceeding
 * PIR will be colored red and subsequently dropped. Yellow packets will
 * egress with the CFI bit set in the VLAN tag.
 */
bcm_error_t
create_entry(int unit, bcm_vlan_t vlanId, bcm_field_entry_t entry,
             bcm_field_group_t gid, int group_priority, bcm_policer_t * PolicerId)
{
    const int   vlanMask = 0x3ff;       /* VLAN ID is 12 bits */

    bcm_policer_config_t PolConfig;
    bcm_field_qset_t BcmFPQualSet;

    BCM_FIELD_QSET_INIT(BcmFPQualSet);
    BCM_FIELD_QSET_ADD(BcmFPQualSet, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(BcmFPQualSet, bcmFieldQualifyOuterVlan);

    BCM_IF_ERROR_RETURN(bcm_field_group_create_mode_id
                        (unit, BcmFPQualSet, group_priority, bcmFieldGroupModeDouble,
                         gid));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create_id(unit, gid, entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterVlanId(unit, entry, vlanId, vlanMask));

    /* Create a COLOR AWARE policer */
    bcm_policer_config_t_init(&PolConfig);
    PolConfig.mode = bcmPolicerModeTrTcm;
    PolConfig.ckbits_sec = 0; 
    PolConfig.ckbits_burst = 0;
    PolConfig.pkbits_sec = 10000;
    PolConfig.pkbits_burst = 10000;
    BCM_IF_ERROR_RETURN(bcm_policer_create(unit, &PolConfig, PolicerId));

    /* Attach at level = 0 */
    BCM_IF_ERROR_RETURN(bcm_field_entry_policer_attach(unit, entry, 0, *PolicerId));

    /* Drop Red */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionRpDrop, 0, 0));

    /* Green packets are colored green and egress normally */
    BCM_IF_ERROR_RETURN(bcm_field_action_add
                        (unit, entry, bcmFieldActionGpDropPrecedence,
                         BCM_FIELD_COLOR_GREEN, 0));

    /* Yellow packets are colored yellow */
    BCM_IF_ERROR_RETURN(bcm_field_action_add
                        (unit, entry, bcmFieldActionYpDropPrecedence,
                         BCM_FIELD_COLOR_YELLOW, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    return BCM_E_NONE;
}

/*
 * Function: color_counters()
 *
 * Attach stat counters to the specified entry to count either green/yellow
 * or yellow/red depending of value of "count_green" flag.
 */
bcm_error_t
color_counters(int unit, bcm_field_group_t group, bcm_field_entry_t entry,
               int count_green)
{
    const int   statCount = 2;
    /*const*/ bcm_field_stat_t stat_request1[2] = {
        bcmFieldStatGreenPackets,       /* Packet count of Green traffic. */
        bcmFieldStatYellowPackets       /* Packet count of Yellow traffic. */
    };
    /*const*/ bcm_field_stat_t stat_request2[2] = {
        bcmFieldStatYellowPackets,      /* Packet count of yellow traffic. */
        bcmFieldStatRedPackets  /* Packet count of red traffic. */
    };
    int         stat_id_1;
    int         stat_id_2;

    if (count_green) {
        /* Create an FP stat object, containing multiple counters */
        BCM_IF_ERROR_RETURN(bcm_field_stat_create
                            (unit, group, statCount, stat_request1, &stat_id_1));

        printf
          ("Stat ID = %d; fp stat get statid=%d type0=greenpackets type1=yellowpackets\n",
           stat_id_1, stat_id_1);

        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id_1));

    } else {
        BCM_IF_ERROR_RETURN(bcm_field_stat_create
                            (unit, group, statCount, stat_request2, &stat_id_2));

        printf
          ("Stat ID = %d; fp stat get statid=%d type0=yellowpackets type1=redpackets\n",
           stat_id_2, stat_id_2);

        BCM_IF_ERROR_RETURN(bcm_field_entry_stat_attach(unit, entry, stat_id_2));
    }

    /* Re-install the entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_reinstall(unit, entry));

    return BCM_E_NONE;
}

/*
 * Function: create_vlan()
 *
 * Create the specified VLAN, add front panel and CPU ports to the new
 * VLAN. Packets egressing on this VLAN will retain their VLAN tags.
 */
bcm_error_t
create_vlan(int unit, bcm_vlan_t vlan)
{
    bcm_port_config_t portConfig;

    bcm_pbmp_t  untagged;

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &portConfig));
    BCM_PBMP_CLEAR(untagged);   /* Never egress untagged for this VLAN */

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan));

    /* Add all front panel ports to new vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, portConfig.ge, untagged));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, portConfig.cpu, untagged));

    return BCM_E_NONE;
}

/*
 * Function: configure_qos_mapping()
 *
 * Set up ingress and egress QOS mapping. By default, ingressing packets
 * with the CFI bit from the VLAN tag set are marked RED. In this example,
 * we want to mark them as yellow. To accomplish this, it is necessary to
 * create a second QOS map. In the same example, we want to set the CFI bit
 * in the egressing packet if the packet is yellow. We need to create a second
 * egress QOS map to do this. All front panel ports will use the new QOS maps.
 */
bcm_error_t
configure_qos_mapping(int unit)
{
    bcm_port_config_t portConfig;
    bcm_port_t  port;
    bcm_qos_map_t l2_eg_map;
    bcm_qos_map_t l2_in_map;
    int         cfi;
    int         color;
    int         internal_priority;
    int         l2_eg_map_id;
    int         l2_in_map_id;
    int         pkt_pri;
    uint32      flags;

    /* Ingress mapping profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2;
    BCM_IF_ERROR_RETURN(bcm_qos_map_create(unit, flags, &l2_in_map_id));

    for (pkt_pri = 0; pkt_pri < 8; pkt_pri++) {
        for (cfi = 0; cfi <= 1; cfi++) {
            bcm_qos_map_t_init(&l2_in_map);
            /* In */
            l2_in_map.int_pri = pkt_pri;
            l2_in_map.pkt_cfi = cfi;

            /* Out: CFI maps to yellow */
            l2_in_map.pkt_pri = pkt_pri;
            l2_in_map.color = (cfi == 0) ? bcmColorGreen : bcmColorYellow;

            BCM_IF_ERROR_RETURN(bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id));
        }
    }

    /* Egress mapping profiles */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2;
    BCM_IF_ERROR_RETURN(bcm_qos_map_create(unit, flags, &l2_eg_map_id));

    for (internal_priority = 0; internal_priority < 16; internal_priority++) {
        pkt_pri = (internal_priority < 8) ? internal_priority : 7;
        for (color = bcmColorGreen; color <= bcmColorRed; color++) {
            bcm_qos_map_t_init(&l2_eg_map);
            /* In */
            l2_eg_map.color = color;
            l2_eg_map.int_pri = internal_priority;

            /* Out */
            l2_eg_map.pkt_pri = pkt_pri;
            /* If packet is not green, set the CFI bit. */
            l2_eg_map.pkt_cfi = (color == bcmColorGreen) ? 0 : 1;

            BCM_IF_ERROR_RETURN(bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_map_id));
        }
    }
	
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &portConfig));
    BCM_PBMP_ITER(portConfig.e, port) {
        BCM_IF_ERROR_RETURN(bcm_qos_port_map_set(unit, port, l2_in_map_id, l2_eg_map_id));
    }

    return BCM_E_NONE;
}

/*
 * Function: testcase()
 *
 * This is the main entry point for this example
 */
bcm_error_t
testcase(int unit)
{
    const bcm_vlan_t VID = 17;
    const int   group_priority = 10;
    const bcm_field_group_t gid = 10;
    const bcm_field_entry_t entry = 1;

    bcm_policer_t PolicerId;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set (unit, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI));
    BCM_IF_ERROR_RETURN(configure_qos_mapping(unit));
    BCM_IF_ERROR_RETURN(create_vlan(unit, VID));
    BCM_IF_ERROR_RETURN(create_entry(unit, VID, entry, gid, group_priority, &PolicerId));
    BCM_IF_ERROR_RETURN(color_counters(unit, gid, entry, 1));

    return BCM_E_NONE;
}

//if (BCM_FAILURE(testcase(0))) {
//    printf("Configuration Failed\n");
//}
#endif //__DEBUGGING_DEI_CFI_2_COLOR_2_DEI_CIF__

