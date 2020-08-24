/*! \file ctsoc.c
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/init.h>

#include <shared/bsl.h>
#include <sal/core/alloc.h>
#include <sal/core/libc.h>
#include <appl/cputrans/atp.h>
#include <appl/cpudb/cpudb.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/rpc/rpc.h>
#include <bcm_int/rpc/rlink.h>
#include <bcm_int/common/rx.h>
#include <appl/cputrans/atptrans_socket.h>
#include <appl/cputrans/ct_tun.h>

#define BUFFERSIZE (13*1024)

struct cputranssoc_client_s {
    int         cu_buffersize;
    bcm_trans_ptr_t host_atp_transport;
    bcm_trans_ptr_t *original_atp_transport;
    cpudb_ref_t cpudb;
};

static struct cputranssoc_client_s cpu_transsoc_client;

static void
ukey2keystr(uint32 ukey, char *key_buff)
{
    sal_sprintf(key_buff, "00:00:%02X:%02X:%02X:%02X",
                (ukey >> 24) & 0xFF, (ukey >> 16) & 0xFF,
                (ukey >> 8) & 0xFF, ukey & 0xFF);
}

static void
ukey2key(const uint32 ukey, cpudb_key_t * const key)
{
    int         byte_shift;
    uint8      *key_ptr = &key->key[5];

    for (byte_shift = 0; byte_shift < sizeof(uint32); byte_shift++) {
        *key_ptr-- = (ukey >> (byte_shift * 8)) & 0xFF;
    }
    for (; byte_shift < sizeof(key->key); byte_shift++) {
        *key_ptr-- = 0;
    }
}

bcm_error_t
bcm_ctsoc_cpudb_create(const uint32 ukey)
{
    cpudb_key_t key;
    cpudb_entry_t *entry;

    if ((cpu_transsoc_client.cpudb = cpudb_create()) == NULL) {
        bsl_printf("ERROR: cpudb create failed\n");
        return BCM_E_MEMORY;
    }
    ukey2key(ukey, &key);
    CPUDB_KEY_SEARCH(cpu_transsoc_client.cpudb, key, entry);
    if (entry != NULL) {
        bsl_printf("WARNING key already in DB %d", ukey);
    }

    entry = cpudb_entry_create(cpu_transsoc_client.cpudb, key, TRUE);
    if (entry == NULL) {
        bsl_printf("Failed to create CPUDB entry for %u\n", ukey);
        return BCM_E_EXISTS;
    }
    memcpy(entry->base.mac, key.key, sizeof(bcm_mac_t));

    entry->flags = 0;
    entry->base.num_units = 0;
    entry->base.num_stk_ports = 0;

    CPUDB_KEY_SEARCH(cpu_transsoc_client.cpudb, key, entry);
    cpu_transsoc_client.cpudb->local_entry = entry;

    return BCM_E_NONE;
}

#define MARK printf("%s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__)

bcm_error_t
bcm_ctsoc_client_rpc_start(const int retry_ms, const int retries)
{
    const int   retry_us = retry_ms * 1000;

    atp_timeout_set(retry_us, retries);
    BCM_IF_ERROR_RETURN(bcm_rpc_start());
    BCM_IF_ERROR_RETURN(bcm_rlink_start());
    BCM_IF_ERROR_RETURN(ct_rx_tunnel_mode_default_set(BCM_CPU_TUNNEL_PACKET_RELIABLE));
    BCM_IF_ERROR_RETURN(ct_tx_tunnel_setup());

    return BCM_E_NONE;
}

bcm_error_t
bcm_ctsoc_client_server_start(void)
{
    BCM_IF_ERROR_RETURN(atptrans_socket_server_start());
    return BCM_E_NONE;
}

bcm_error_t
bcm_ctsoc_client_install(uint32 dest_ukey, bcm_ip_t dest_ip)
{
    cpudb_key_t local_key;
    cpudb_key_t dest_key;
    bcm_error_t rv;
    cpudb_entry_t *entry;

    CPUDB_KEY_COPY(local_key, cpu_transsoc_client.cpudb->local_entry->base.key);

    if (BCM_FAILURE(rv = atptrans_socket_local_cpu_key_set(local_key))) {
        bsl_printf("Failed to set local CPU key: [%s]", bcm_errmsg(rv));
    }


    /* Install socket for given dest CPU */
    ukey2key(dest_ukey, &dest_key);

    if (BCM_FAILURE(rv = atptrans_socket_install(dest_key, dest_ip, 0x0))) {
        bsl_printf("Cannot install socket interface in ATP transport for CPU key %d [%s]",
                dest_ukey, bcm_errmsg(rv));
        return rv;
    }

    /* Update DB if necessary */
    CPUDB_KEY_SEARCH(cpu_transsoc_client.cpudb, dest_key, entry);
    if (entry == NULL) {
        entry = cpudb_entry_create(cpu_transsoc_client.cpudb, dest_key, FALSE);
        if (entry == NULL) {
            bsl_printf("Failed to create CPUDB entry for %u\n", dest_ukey);
            return BCM_E_INTERNAL;
        }
        memcpy(entry->base.mac, dest_key.key, sizeof(bcm_mac_t));

        entry->flags = 0;
        entry->base.num_units = 0;
        entry->base.num_stk_ports = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ctsoc_setup
 * Purpose:
 *     Configure ATP and BCM transport.
 *
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_NONE - success
 *     BCM_E_XXX  - other errors
 */
bcm_error_t
bcm_ctsoc_setup()
{
    int         rv = BCM_E_NONE;
    bcm_trans_ptr_t *atp_transport;

    if (BCM_FAILURE(rv = atp_config_get(NULL, NULL, &atp_transport))) {
        bsl_printf("atp_config_get() FAILED: %d (%s)\n", rv, bcm_errmsg(rv));
    }
    if (BCM_SUCCESS(rv) && (atp_transport != &cpu_transsoc_client.host_atp_transport)) {

        /* Set up RX pool if needed */
        cpu_transsoc_client.cu_buffersize = BUFFERSIZE;
        if (!bcm_rx_pool_setup_done()) {
            if (BCM_FAILURE
                (rv = bcm_rx_pool_setup(-1, cpu_transsoc_client.cu_buffersize))) {
                bsl_printf("bcm_rx_pool_setup() FAILED: %d (%s)\n", rv, bcm_errmsg(rv));
            }
        } else {
            /* RX was probably set up by BCM RX, so use the BCM RX
             * packet size from the first unit, if available */
            bcm_rx_cfg_t rx_cfg;

            rv = bcm_rx_cfg_get(0, &rx_cfg);
            if (BCM_SUCCESS(rv) || rv == BCM_E_INIT) {
                /* E_INIT just means that the unit isn't running */
                cpu_transsoc_client.cu_buffersize = rx_cfg.pkt_size;
            } else {
                bsl_printf("bcm_rx_cfg_get() FAILED: %d (%s)\n", rv, bcm_errmsg(rv));
            }
        }
        /* ATP always adds CPUTRANS_HEADER_BYTES to its RX allocations */
        cpu_transsoc_client.cu_buffersize -= CPUTRANS_HEADER_BYTES;

        /* Save original transport */
        cpu_transsoc_client.original_atp_transport = atp_transport;

        /* Set up host transport */
        cpu_transsoc_client.host_atp_transport = *atp_transport;
        cpu_transsoc_client.host_atp_transport.tp_data_alloc = bcm_rx_pool_alloc;
        cpu_transsoc_client.host_atp_transport.tp_data_free = bcm_rx_pool_free;
        if (BCM_FAILURE
            (rv = atp_config_set(-1, -1, &cpu_transsoc_client.host_atp_transport))) {
            bsl_printf("atp_config_set() FAILED: %d (%s)\n", rv, bcm_errmsg(rv));
        }
        atp_db_update(cpu_transsoc_client.cpudb);
        rv = atp_start(ATP_F_LEARN_SLF,
                       ATP_UNITS_ALL,
                       BCM_RCO_F_ALL_COS);
    }

    return rv;
}

bcm_error_t
bcm_ctsoc_client_attach(const int local_unit, const int remote_unit, const uint32 ukey)
{
    char       *type = "client";
    char        subtype[24];

    ukey2keystr(ukey, subtype);

    return bcm_attach(local_unit, type, subtype, remote_unit);
}

void
bcm_ctsoc_mem_control(const int use_heap)
{
    rxp_memory_source_heap = use_heap;
    atp_ack_pkt_data_from_heap = use_heap;
}

bcm_error_t
bcm_ctsoc_socket_config_set(int local_port, int remote_port)
{
    int priority = -1; /* Do not default change priority. */
    return atptrans_socket_config_set(priority, local_port, remote_port);
}
