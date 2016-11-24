Please refer to the document BroadSAFE MicroHSM Software Reference for
information on the BroadSAFE uHSM driver.

The implementation here has been changed so that bsafe_open() takes a
StrataSwitch unit number and reset flag in lieue of a device name.

The application should only need to include bsafe_uhsm_lib.h and call
functions in bsafe_uhsm_lib.c.

	bsafe_uhsm_lib.c	<--- application driver source
	bsafe_uhsm_lib.h	<--- application driver header

The test functions bsafe_uhsm_lib_test.c have been included in the
current distribution.  To try them from the BCM diag command line, use
the bsafe command.

	BCM.0> bsafe 0		<--- open device
	BCM.0> bsafe 0x2	<--- initialize device key
	BCM.0> bsafe 0x4	<--- get public keys
	BCM.0> bsafe 0x10000	<--- test random number generator

Other functions have not been tested for SDK 5.2.0.
