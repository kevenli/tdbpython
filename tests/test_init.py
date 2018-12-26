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

class InitTest(unittest.TestCase):
    def setUp(self):
        self.target = TDBClient('', '', '', '')
        self.target.connect()
        
    def tearDown(self):
        self.target.close()
        
    def test_GetCodeTable(self):
        client = self.target
        codetable = client.GetCodeTable()
        self.assertGreater(len(codetable), 0)

    def test_GetCodeTable2(self):
        client = self.target
        codetable = client.GetCodeTable('dce')
        #print codetable
        self.assertGreater(len(codetable), 0)
        for item in codetable:
            self.assertEqual(item['chMarket'], 'DCE')
            
    def test_GetFuture(self):
        client = self.target
        chWindCode = 'Y1701.DCE'
        chCode = 'y1701'
        
        startdate = 20160704
        enddate = 20160706
        data = client.GetFuture(chWindCode, startdate, enddate);
        
        for item in data:
            self.assertEqual(chWindCode, item[0])     # 0  wind代码，好像是转成tower了
            self.assertEqual(chCode, item[1])           # 1 交易所代码
            #self.assertGreaterEqual(item[2], startdate)     # date是actiondate，可能与早于交易日期
            self.assertLessEqual(item[2], enddate)      # 日期
            self.assertIsNotNone(item[3])                 # time 交易时间
            
            # typedef struct 
            # {
                # char chWindCode[32];               //万得代码(AG1312.SHF)
                # char chCode[32];                   //交易所代码(ag1312)
                # int nDate;                         //日期（自然日）格式：YYMMDD
                # int nTime;                         //时间，格式：HHMMSSmmm
                # __int64 iVolume;                   //成交量
                # __int64    iTurover;               //成交额(元)
                # int nSettle;                       //结算价((a double number + 0.00005) *10000)
                # int nPosition;                     //持仓量
                # int nCurDelta;                     //虚实度
                # char chTradeFlag;                  //成交标志（港股有值）
                # __int64 iAccVolume;                //当日累计成交量
                # __int64    iAccTurover;            //当日成交额(元)
                # int nHigh;                         //最高((a double number + 0.00005) *10000)
                # int nLow;                          //最低((a double number + 0.00005) *10000)
                # int nOpen;                         //开盘((a double number + 0.00005) *10000)
                # int nPrice;                        //成交价((a double number + 0.00005) *10000)
                # int nPreClose;                     //前收盘((a double number + 0.00005) *10000)
                # int nPreSettle;                    //昨结算((a double number + 0.00005) *10000)
                # int nPrePosition;                  //昨持仓
            # }TDBDefine_Future;
    
    def test_GetFuture_autocomplete(self):
        client = self.target
        chWindCode = 'Y1701.DCE'
        chCode = 'y1701'
        
        startdate = 20160706
        enddate = 20160706
        starttime = 90000000
        endtime = 150000000
        autocomplete = 1
        data = client.GetFuture(chWindCode, startdate, enddate, starttime, endtime, autocomplete);
        for item in data:
            self.assertEqual(chWindCode, item[0])     # 0  wind代码，好像是转成tower了
            self.assertEqual(chCode, item[1])           # 1 交易所代码
            self.assertGreaterEqual(item[2], startdate)     # date是actiondate，可能与早于交易日期
            self.assertLessEqual(item[2], enddate)      # 日期
            self.assertIsNotNone(item[3])                 # time 交易时间\
            
    def test_GetFutureAB(self):
        client = self.target
        chWindCode = 'Y1701.DCE'
        chCode = 'y1701'
        
        startdate = 20160706
        enddate = 20160706
        data = client.GetFutureAB(chWindCode, startdate, enddate);
        for item in data:
            self.assertEqual(chWindCode, item[0])     # 0  wind代码，好像是转成tower了
            self.assertEqual(chCode, item[1])           # 1 交易所代码
            #self.assertGreaterEqual(item[2], startdate)     # date是actiondate，可能与早于交易日期
            self.assertLessEqual(item[2], enddate)      # 日期
            self.assertIsNotNone(item[3])                 # time 交易时间
            # print item
            # typedef struct 
            # {
                # char chWindCode[32];                //万得代码(AG1312.SHF)
                # char chCode[32];                    //交易所代码(ag1312)
                # int nDate;                          //日期（自然日）格式：YYMMDD
                # int nTime;                          //时间（精确到毫秒，HHMMSSmmm）
                # __int64 iVolume;                    //成交量
                # __int64    iTurover;                //成交额(元)
                # int nSettle;                        //结算价((a double number + 0.00005) *10000)
                # int nPosition;                       //持仓量
                # int nCurDelta;                      //虚实度
                # char chTradeFlag;                   //成交标志
                # __int64 iAccVolume;                 //当日累计成交量
                # __int64    iAccTurover;             //当日成交额(元)
                # int nOpen;                          //开盘((a double number + 0.00005) *10000)
                # int nHigh;                          //最高((a double number + 0.00005) *10000)
                # int nLow;                           //最低((a double number + 0.00005) *10000)
                # int nPrice;                         //成交价((a double number + 0.00005) *10000)
                # int    nAskPrice;               //叫卖价((a double number + 0.00005) *10000)
                # unsigned int nAskVolume;        //叫卖量
                # int    nBidPrice;               //叫买价((a double number + 0.00005) *10000)
                # unsigned int nBidVolume;        //叫买量
                # int nPreClose;                     //前收盘((a double number + 0.00005) *10000)
                # int nPreSettle;                    //昨结算((a double number + 0.00005) *10000)
                # int nPrePosition;                  //昨持仓
            # }TDBDefine_FutureAB;
            
    def test_GetFutureAB2(self):
        client = self.target
        chWindCode = 'A.DCE'
        chCode = 'a'
        
        startdate = 20160104
        enddate = 20160104
        data = client.GetFutureAB(chWindCode, startdate, enddate);
        for item in data:
            self.assertEqual(chWindCode, item[0])     # 0  wind代码，好像是转成tower了
            self.assertEqual(chCode, item[1])           # 1 交易所代码
            self.assertLessEqual(item[2], enddate)      # 日期
            self.assertIsNotNone(item[3])                 # time 交易时间
                        
        
    def test_GetKLine(self):
        client = self.target
        chCode = 'y1701.DCE'
        data = client.GetKLine(chCode, 20160701, 20160706, tdbpython.CYC_MINUTE);
        for item in data:
            pass
            #print item
            # typedef struct 
            # {
                # char chWindCode[32];            //万得代码(AG1312.SHF)
                # char chCode[32];                //交易所代码(ag1312)
                # int nDate;                      //日期（自然日）格式：YYMMDD，例如20130101表示2013年1月1日，0表示今天
                # int nTime;                      //时间（HHMMSSmmm）例如94500000 表示 9点45分00秒000毫秒
                # int    nOpen;                   //开盘((a double number + 0.00005) *10000)
                # int    nHigh;                   //最高((a double number + 0.00005) *10000)
                # int    nLow;                    //最低((a double number + 0.00005) *10000)
                # int    nClose;                  //收盘((a double number + 0.00005) *10000)
                # __int64 iVolume;                //成交量
                # __int64    iTurover;            //成交额(元)
                # int    nMatchItems;             //成交笔数
                # int nInterest;                  //持仓量(期货)、IOPV(基金)、利息(债券)
            # }TDBDefine_KLine;
            

        

