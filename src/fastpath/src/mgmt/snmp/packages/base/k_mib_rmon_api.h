/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_rmon_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_rmon.c
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*             
**********************************************************************/

#include "usmdb_mib_rmon_api.h"




L7_RC_t
snmpAlarmSampleTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbAlarmSampleTypeGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t 
snmpAlarmStartupAlarmGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbAlarmStartupAlarmGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t
snmpChannelAcceptTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbChannelAcceptTypeGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t
snmpChannelDataControlGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbChannelDataControlGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t 
snmpChannelEventStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbChannelEventStatusGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t
snmpBufferControlFullStatusGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbBufferControlFullStatusGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t
snmpBufferControlFullActionGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbBufferControlFullActionGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t 
snmpEventTypeGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbEventTypeGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}

L7_RC_t
snmpHostTopNRateBaseGet ( L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val )
{
    L7_int32 temp_val;
    L7_RC_t rc;

    rc = usmDbHostTopNRateBaseGet ( UnitIndex, Index, &temp_val );

    /* TODO: this value needs to be translated. */
    *val = temp_val;

    return rc;
}
