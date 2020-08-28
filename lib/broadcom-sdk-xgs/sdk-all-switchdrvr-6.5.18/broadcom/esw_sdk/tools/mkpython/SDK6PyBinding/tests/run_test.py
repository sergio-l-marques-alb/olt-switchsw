##############################################################################
#
# Master test script.
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

import unittest, trace

# Import tests
import l2_sem
import l3_sem
import mc_sem
import qos_sem

class TestBinding(unittest.TestCase):
    def test_l2_sem_test_001(self):
        rv = l2_sem.l2_sem_test_001()
        self.assertEqual(rv, 0)

    def test_l3_sem_test_001(self):
        rv = l3_sem.l3_sem_test_001()
        self.assertEqual(rv, 0)

    def test_l3_sem_test_002(self):
        rv = l3_sem.l3_sem_test_002()
        self.assertEqual(rv, 0)

    def test_mc_sem_test_001(self):
        rv = mc_sem.mc_sem_test_001()
        self.assertEqual(rv, 0)

    def test_qos_sem_test_001(self):
        rv = qos_sem.qos_sem_test_001()
        self.assertEqual(rv, 0)

if __name__ == '__main__':
    unittest.main()
