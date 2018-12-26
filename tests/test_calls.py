import sys
sys.path.insert(0, 'build/lib.linux-x86_64-2.7')
sys.path.insert(1, 'dll/x64release')
#
import unittest
from tdbpython import TDBClient
import tdbpython
import csv
import logging
import calendar
import datetime


class CallsTest(unittest.TestCase):
    def get_Date(self, year):
        date_list = []
        for i in range(1,13):
            m_lastday = calendar.monthrange(year,i)[1]
            for j in range(1,m_lastday+1):
                date = datetime.date(year,i,j)
                if date.weekday() in (5,6):
                    continue
                date_list.append(date)
        return date_list
        
    def get_futuredata(self, client, chWindCode, startdate, enddate):
        starttime = 0
        endtime = 30000000
        data = client.GetFutureAB(chWindCode, startdate, enddate,starttime, endtime, 0)
        return data
            
    def test_multicalls(self):
        client = TDBClient('', '', '', '')
        client.connect()
        codetable = client.GetCodeTable('dce')
        startdate = 20160704
        enddate = 20160704
        for date in self.get_Date(2016):
            sdate = date.strftime('%Y%m%d')
            idate = int(sdate)
            for code in codetable:
                chWindCode = code['chWindCode']
                print chWindCode, idate, idate
                startdate = idate
                enddate = idate
                starttime = 0
                endtime = 0
                #data = client.GetFutureAB(chWindCode, startdate, enddate,starttime, endtime, 0)
                data = self.get_futuredata(client, chWindCode, idate, idate)
                if data is not None:
                    print len(data)
                else:
                    print None