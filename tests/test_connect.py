# -*-coding:utf8 -*-
import sys
if sys.platform.startswith('linux'):
    sys.path.insert(0, 'build/lib.linux-x86_64-2.7')
else:
    sys.path.insert(0, 'build/lib.win-amd64-2.7')
#
import unittest
from tdbpython import TDBClient
import tdbpython
import csv
import logging
import calendar
import datetime
import time

class ConnectTest(unittest.TestCase):
    def test_connect(self):
        client = TDBClient('', '', '', '')
        client.connect()
        #client.close()
        ret = client.close()
        self.assertEqual(ret, 0)
        
    def test_connect_fail(self):
        client = TDBClient('', '', '', '')
        try:
            client.connect()
            client.close()
            self.assertFail()
        except RuntimeError:
            pass
            
    def test_multi_clients(self):
        time.sleep(10)
        client = TDBClient('', '', '', '')
        client.connect()
        ret = client.close()
        self.assertEqual(ret, 0)
