/* 
 * $Id: bhh.h,v 1.2 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        policer.h
 * Purpose:     Defines common policer parameters.
 */

#ifndef   _SHR_POLICER_H_
#define   _SHR_POLICER_H_

/* database_handle macro definitions */
#define _SHR_POLICER_IS_INGRESS_POSITION (0)
#define _SHR_POLICER_IS_INGRESS_MASK (0x1)
#define _SHR_POLICER_IS_GLOBAL_POSITION (2)
#define _SHR_POLICER_IS_GLOBAL_MASK (0x1)
#define _SHR_POLICER_DATABASE_ID_POSITION (4)
#define _SHR_POLICER_DATABASE_ID_MASK (0x3)
#define _SHR_POLICER_CORE_ID_POSITION (8)
#define _SHR_POLICER_CORE_ID_MASK (0x3) /** support upto 4 cores */
#define _SHR_POLICER_DATABASE_HANDLE_MACRO_IND_POSITION (30)
#define _SHR_POLICER_DATABASE_HANDLE_MACRO_IND_MASK (0x1)

/* policer_id macro definitions */
#define _SHR_POLICER_ID_METER_INDEX_POSITION (0)
#define _SHR_POLICER_ID_METER_INDEX_MASK (0xFFFFF) /** 20 bits for meter index*/
#define _SHR_POLICER_ID_DATABASE_HANDLE_POSITION (20)
#define _SHR_POLICER_ID_DATABASE_HANDLE_MASK (0x1FF) /** 9 bits for  the handle */
#define _SHR_POLICER_ID_MACRO_IND_POSITION (30)
#define _SHR_POLICER_ID_MACRO_IND_MASK (0x1)

#define _SHR_POLICER_DATABASE_HANDLE_SET(_database_handle, _is_ingress, _is_global, _core_id, _database_id) \
    (_database_handle = ((_core_id & _SHR_POLICER_CORE_ID_MASK) << _SHR_POLICER_CORE_ID_POSITION) & \
                        ((_database_id & _SHR_POLICER_DATABASE_ID_MASK) << _SHR_POLICER_DATABASE_ID_POSITION) & \
                        ((_is_global & _SHR_POLICER_IS_GLOBAL_MASK) << _SHR_POLICER_IS_GLOBAL_POSITION) & \
                        ((_is_ingress & _SHR_POLICER_IS_INGRESS_MASK) << _SHR_POLICER_IS_INGRESS_POSITION) & \
                        ((0x1 & _SHR_POLICER_DATABASE_HANDLE_MACRO_IND_MASK) << _SHR_POLICER_DATABASE_HANDLE_MACRO_IND_POSITION))

#define _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(_database_handle) \
    ((_database_handle >> _SHR_POLICER_IS_INGRESS_POSITION) & _SHR_POLICER_IS_INGRESS_MASK) 

#define _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(_database_handle) \
    ((_database_handle >> _SHR_POLICER_IS_GLOBAL_POSITION) & _SHR_POLICER_IS_GLOBAL_MASK)

#define _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(_database_handle) \
    ((_database_handle >> _SHR_POLICER_CORE_ID_POSITION) & _SHR_POLICER_CORE_ID_MASK)

#define _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(_database_handle) \
    ((_database_handle >> _SHR_POLICER_DATABASE_ID_POSITION) & _SHR_POLICER_DATABASE_ID_MASK)



#define _SHR_POLICER_ID_SET(_policer_id, _database_handle, meter_index) \
    (_policer_id = ((meter_index & _SHR_POLICER_ID_METER_INDEX_MASK) << _SHR_POLICER_ID_METER_INDEX_POSITION) & \
                   ((_database_handle & _SHR_POLICER_ID_DATABASE_HANDLE_MASK) << _SHR_POLICER_ID_DATABASE_HANDLE_POSITION) & \
                   ((0x1 & _SHR_POLICER_ID_MACRO_IND_MASK) << _SHR_POLICER_ID_MACRO_IND_POSITION))

#define _SHR_POLICER_ID_METER_INDEX_GET(_policer_id) \
    ((_policer_id >> _SHR_POLICER_ID_METER_INDEX_POSITION) & _SHR_POLICER_ID_METER_INDEX_MASK)

#define _SHR_POLICER_ID_DATABASE_HANDLE_GET(_policer_id) \
    ((_policer_id >> _SHR_POLICER_ID_DATABASE_HANDLE_POSITION) & _SHR_POLICER_ID_DATABASE_HANDLE_MASK)

                   
#endif /* _SHR_POLICER_H_ */
