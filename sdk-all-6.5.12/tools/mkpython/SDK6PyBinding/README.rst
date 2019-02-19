Copyright: Broadcom Ltd.

=========================
BCM SDK6 Python Binding
=========================

Utility Name
--------------

SDK6 PYBINDING



Description
---------------

SDK6 pybinding is used to generate python wrappers for SDK6 C APIs

SWIG is used to generate python files and corresponding wrapper files.

Refer swig.org for more information on SWIG.

Application Support Layer:
ASL acts as a support layer for applications to use BCM APIs.
cpudb, cputrans and ctsoc modules are used to establish RPC connection with the device.
SAL is used for setting up BCM init parameters.

Sample python scripts are available in the tests directory.

:Version: 1.0



BCM API syntax
----------------

Refer NOTES section at the bottom of this document for python syntax of BCM APIs.


RPC Handshake
---------------

Refer sample/rpc_con.py for steps to establish RPC connection to the BCM device.

Run sample/rpc_con_with_dev_cntl.py to establish RPC connection from both host
and device.


Changelog:
-----------
## **[ 1.0 ] 09/14/2017**
### Added

* First release of SDK6 PYBINDING package


## **[ 1.1 ] 10/27/2017**

* Macro signature modified.
* Fix to make bcm_pbmp_t() visible.

NOTES:
---------

This section covers the basic approach to program BCM python API.

* Arrays:
    To create array for any type (scalar or otherwise), append the type
        with "_array(<num>)", num = number of elements

    ::

        c = int_array(10)
        mac = unsigned_char_array(6)
        pd_info_list = bcm_port_dest_info_t_array(100)


* Pointers:
    To create a pointer, append the type with "_ptr()".
    To assign value, use assign() and to read the value, use value()

    ::

        intp = int_ptr()
        intp.assign(10)
        print intp.value()


* Macros:
   Constant defines are directly mapped and be used as usual.

   ::

    pipes_max = BCM_PIPES_MAX

   For the functional macro, the arguments are wrapped.
   NOTE : For functional macros, the modified arguments due to macro expanstion
   is appended to the return value. Refer example below.

   ::

    gport, phy_port =  BCM_GPORT_LOCAL_SET(gport, phy_port)


* Typedefs:
    Most of the typedefs are resolved with typemaps.

    To create a pointer of a particular typedef, the basetype needs to be used.

   ::

    typedef int bcm_if_d;
    int bcm_l3_egress_create(int unit, uint32 flags, bcm_l3_egress_t *egr, bcm_if_t *if_id);)

    l3_egr_s = bcm_l3_egress_t()
    egr_id_p = int_ptr()
    l3_egr_s.mac_addr = nhmac.cast()
    l3_egr_s.vlan = egr_vid
    l3_egr_s.port = egr_port
    l3_egr_s.intf = 1
    l3_egr_s.flags = 0
    rv = bcm_l3_egress_create(unit, 0, l3_egr_s, egr_id_p)


   Most array typedefs have a python class that generates the array component.

   ::

    mac = bcm_mac_t()
    mac = uint8_array(8)


   The datatypes which are typedef-ed from standard datatypes can be used directly.
   For example, to create a gport, just assign the value directly.

   ::

    gport = 0


* Array to Pointer conversion:

   For the arrays that are created using _array() call, the pointer of this structure can be accessed through .cast() method.


   ::

        mac = bcm_mac_t()
        l2_s = bcm_l2_addr_t()
        l2_s.mac = mac.cast()


* Pointer to Array conversion:

   For converting pointer returned from BCM C API to array, XXX_array_frompointer() can be used.

   (Replace XXX with basetype of the variable)

   ::

    rv = bcm_l2_addr_get(unit, mac.cast(), vid, l2_r)
    mac_r = uint8_array_frompointer(l2_r.mac)



* Structures declaration:

   Structures are created as objects returned from python function calls.

   ::

        To create a structure of type cpudb_key_t, use
        cpudb_key = cpudb_key_t()


* Python function pointer:

   Refer below example to send python callable objects to function arguments.

   ::

    def l2_trav_cb(unit, info, user_data):
        print ("Unit : %d; vid : %d; port : %d" % (unit, info.vid, info.port))
    rv = bcm_l2_traverse(unit, l2_trav_cb, None)


   Internally, the wrapper is programmed to convert the understand the python object.

   There are 3 steps:

        Callback function register (Eg: bcm_py_bcm_l2_traverse_cb_register)

        Callback function dispatch (Eg: bcm_py_bcm_l2_traverse_cb_dispatch)

   The callback function would be unregistered after dispatch.


* C function pointers:

   To assign C function pointers, append "_cb" to the function name during assignment.

   ::

    atp_transport.tp_data_alloc = bcm_rx_pool_alloc_cb // bcm_rx_pool_alloc is a C function.
