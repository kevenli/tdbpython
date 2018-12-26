#include <Python.h>
#include "structmember.h"
#include "TDBAPIStruct.h"
#include "TDBAPI.h"
#include "string.h"
#include <stdio.h>


typedef struct {
    PyObject_HEAD
    PyObject *ip;
    PyObject *port;
    PyObject *username;
    PyObject *password;
    THANDLE m_thandle;
}TDBClient;

static void
TDBClient_dealloc(TDBClient* self)
{
    if(self->ip != NULL){
        Py_XDECREF(self->ip);
    }
    
    if(self->port != NULL){
        Py_XDECREF(self->port);
    }
    if(self->username != NULL){
        Py_XDECREF(self->username);
    }
    if(self->password != NULL){
        Py_XDECREF(self->password);
    }
    if(self->m_thandle != NULL)
    {
        TDB_Close(self->m_thandle);
        self->m_thandle = NULL;
    }
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
TDBClient_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    TDBClient *self;
    self = (TDBClient *)type->tp_alloc(type, 0);
    if(self != NULL){
        self->ip = PyString_FromString("");
        self->port = PyString_FromString("");
        self->username = PyString_FromString("");
        self->password = PyString_FromString("");
    }
    return (PyObject *)self;
}

static int
TDBClient_init(TDBClient *self, PyObject *args, PyObject *kwds)
{
    PyObject *ip=NULL, *port=NULL, *username=NULL, *password=NULL, *tmp=NULL;
    
    static char *kwlist[] = {"ip", "port", "username", "password", NULL};
    
    if (! PyArg_ParseTupleAndKeywords(args, kwds, "OOOO", kwlist,
                                      &ip, &port,
                                      &username, &password))
        return -1;
    if (ip){
        tmp = self->ip;
        Py_INCREF(ip);
        self->ip = ip;
        Py_XDECREF(tmp);
    }
    if (port){
        tmp = self->port;
        Py_INCREF(port);
        self->port = port;
        Py_XDECREF(tmp);
    }
    if (username){
        tmp = self->username;
        Py_INCREF(username);
        self->username = username;
        Py_XDECREF(tmp);
    }
    if (password){
        tmp = self->password;
        Py_INCREF(password);
        self->password = password;
        Py_XDECREF(tmp);
    }
    return 0;
}
static PyObject 
*TDBClient_connect(TDBClient *self)
{
    OPEN_SETTINGS settings;
    TDBDefine_ResLogin reslogin;
    THANDLE ret;
    
    strcpy(settings.szIP, PyString_AsString(self->ip));
    strcpy(settings.szPort, PyString_AsString(self->port));
    strcpy(settings.szUser, PyString_AsString(self->username));
    strcpy(settings.szPassword, PyString_AsString(self->password));
    
    ret = TDB_Open(&settings, &reslogin);
    if(ret == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed when connecting to server.");
        return NULL;
    }
    self->m_thandle = ret;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject
*TDBClient_GetCodeTable(TDBClient *self, PyObject *args)
{
    char* szMarket = NULL;
    int count = 0;
    TDBDefine_Code* pCodeTable = NULL;
    PyObject* tuple;
    PyObject* item;
    int ret;
    int i;
    
    PyArg_ParseTuple(args, "|s", &szMarket);
    ret = TDB_GetCodeTable(self->m_thandle, szMarket, &pCodeTable, &count);
    
    tuple = PyTuple_New(count);
    for (i=0;i<count;i++)
    {
        item = Py_BuildValue("{s:s,s:s,s:s,s:s,s:s,s:i}", 
            "chCode", pCodeTable[i].chCode,
            "chWindCode", pCodeTable[i].chWindCode,
            "chMarket", pCodeTable[i].chMarket,
            "chCNName", pCodeTable[i].chCNName,
            "chENName", pCodeTable[i].chENName,
            "nType", pCodeTable[i].nType);
        PyTuple_SetItem(tuple, i, item);
    }
    TDB_Free(pCodeTable);
    return tuple;
}

static PyObject*
TDBClient_GetTick(TDBClient *self, PyObject* args)
{
    char* chCode;
    TDBDefine_ReqTick reqtick ={"", 0, 0, 0, 0};
    TDBDefine_Tick* ptickdata=NULL;
    int count=0;
    int i;
    char buffer [50] = {0};
    PyObject* item;
    PyObject* tuple;
    TDB_ERROR ret;
    PyArg_ParseTuple(args, "sii|ii", &chCode,
        &reqtick.nBeginDate, &reqtick.nEndDate,
        &reqtick.nBeginTime, &reqtick.nEndTime);

    strncpy(reqtick.chCode, chCode, sizeof(reqtick.chCode));

    ret = TDB_GetTick(self->m_thandle, &reqtick, &ptickdata, &count);
    if (ret != TDB_SUCCESS && ret != TDB_NO_DATA)
    {
        snprintf(buffer, 49, "Failed when excuting query, error no %d", ret);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }

    tuple = PyTuple_New(count);
    
    for (i=0;i<count;i++)
    {
        item = Py_BuildValue("(s,s,i,i,i,L,L,i,i,c,c,L,L,i,i,i,i,i,i,i,i,i)",
            ptickdata[i].chWindCode,
            ptickdata[i].chCode,
            ptickdata[i].nDate,
            ptickdata[i].nTime,
            ptickdata[i].nPrice,
            ptickdata[i].iVolume,
            ptickdata[i].iTurover,
            ptickdata[i].nMatchItems,
            ptickdata[i].nInterest,
            ptickdata[i].chTradeFlag,
            ptickdata[i].chBSFlag,
            ptickdata[i].iAccVolume,
            ptickdata[i].iAccTurover,
            ptickdata[i].nHigh,
            ptickdata[i].nLow,
            ptickdata[i].nOpen,
            ptickdata[i].nPreClose,
            ptickdata[i].nIndex,
            ptickdata[i].nStocks,
            ptickdata[i].nUps,
            ptickdata[i].nDowns,
            ptickdata[i].nHoldLines
            );
        PyTuple_SetItem(tuple, i, item);
    }
    TDB_Free(ptickdata);
    return tuple;
}

static PyObject*
TDBClient_GetTickAB(TDBClient *self, PyObject* args)
{
    char* chCode;
    TDBDefine_ReqTick reqtick ={"", 0, 0, 0, 0};
    TDBDefine_TickAB* ptickdata=NULL;
    int count=0;
    int i;
    char buffer [50] = {0};
    PyObject* item;
    PyObject* tuple;
    TDB_ERROR ret;
    PyArg_ParseTuple(args, "sii|ii", &chCode,
        &reqtick.nBeginDate, &reqtick.nEndDate,
        &reqtick.nBeginTime, &reqtick.nEndTime);

    strncpy(reqtick.chCode, chCode, sizeof(reqtick.chCode));

    ret = TDB_GetTickAB(self->m_thandle, &reqtick, &ptickdata, &count);
    if (ret != TDB_SUCCESS && ret != TDB_NO_DATA)
    {
        snprintf(buffer, 49, "Failed when excuting query, error no %d", ret);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }

    tuple = PyTuple_New(count);

    for (i=0;i<count;i++)
    {
        item = Py_BuildValue("(s,s,i,i,i,L,L,i,i,c,c,L,L,i,i,i,i,(iiiiiiiiii),(iiiiiiiiii),(iiiiiiiiii),(iiiiiiiiii),i,i,L,L,i,i,i,i,i)",
            ptickdata[i].chWindCode,
            ptickdata[i].chCode,
            ptickdata[i].nDate,
            ptickdata[i].nTime,
            ptickdata[i].nPrice,
            ptickdata[i].iVolume,
            ptickdata[i].iTurover,
            ptickdata[i].nMatchItems,
            ptickdata[i].nInterest,
            ptickdata[i].chTradeFlag,
            ptickdata[i].chBSFlag,
            ptickdata[i].iAccVolume,
            ptickdata[i].iAccTurover,
            ptickdata[i].nHigh,
            ptickdata[i].nLow,
            ptickdata[i].nOpen,
            ptickdata[i].nPreClose,
            ptickdata[i].nAskPrice[0],
            ptickdata[i].nAskPrice[1],
            ptickdata[i].nAskPrice[2],
            ptickdata[i].nAskPrice[3],
            ptickdata[i].nAskPrice[4],
            ptickdata[i].nAskPrice[5],
            ptickdata[i].nAskPrice[6],
            ptickdata[i].nAskPrice[7],
            ptickdata[i].nAskPrice[8],
            ptickdata[i].nAskPrice[9],
            ptickdata[i].nAskVolume[0],
            ptickdata[i].nAskVolume[1],
            ptickdata[i].nAskVolume[2],
            ptickdata[i].nAskVolume[3],
            ptickdata[i].nAskVolume[4],
            ptickdata[i].nAskVolume[5],
            ptickdata[i].nAskVolume[6],
            ptickdata[i].nAskVolume[7],
            ptickdata[i].nAskVolume[8],
            ptickdata[i].nAskVolume[9],
            ptickdata[i].nBidPrice[0],
            ptickdata[i].nBidPrice[1],
            ptickdata[i].nBidPrice[2],
            ptickdata[i].nBidPrice[3],
            ptickdata[i].nBidPrice[4],
            ptickdata[i].nBidPrice[5],
            ptickdata[i].nBidPrice[6],
            ptickdata[i].nBidPrice[7],
            ptickdata[i].nBidPrice[8],
            ptickdata[i].nBidPrice[9],
            ptickdata[i].nBidVolume[0],
            ptickdata[i].nBidVolume[1],
            ptickdata[i].nBidVolume[2],
            ptickdata[i].nBidVolume[3],
            ptickdata[i].nBidVolume[4],
            ptickdata[i].nBidVolume[5],
            ptickdata[i].nBidVolume[6],
            ptickdata[i].nBidVolume[7],
            ptickdata[i].nBidVolume[8],
            ptickdata[i].nBidVolume[9],
            ptickdata[i].nAskAvPrice,
            ptickdata[i].nBidAvPrice,
            ptickdata[i].iTotalAskVolume,
            ptickdata[i].iTotalBidVolume,
            ptickdata[i].nIndex,
            ptickdata[i].nStocks,
            ptickdata[i].nUps,
            ptickdata[i].nDowns,
            ptickdata[i].nHoldLines
            );
        PyTuple_SetItem(tuple, i, item);
    }
    TDB_Free(ptickdata);
    return tuple;
}

static PyObject*
TDBClient_GetFuture(TDBClient *self, PyObject* args)
{
    char* chCode;
    TDBDefine_ReqFuture reqtick ={"",0, 0, 0, 0, 0};
    TDBDefine_Future* ptickdata=NULL;
    int count=0;
    int i;
    char buffer [50] = {0};
    PyObject* item;
    PyObject* tuple;
    TDB_ERROR ret;
    PyArg_ParseTuple(args, "sii|iii", &chCode, 
        &reqtick.nBeginDate, &reqtick.nEndDate, 
        &reqtick.nBeginTime, &reqtick.nEndTime,
        &reqtick.nAutoComplete);
    
    strncpy(reqtick.chCode, chCode, sizeof(reqtick.chCode));

    ret = TDB_GetFuture(self->m_thandle, &reqtick, &ptickdata, &count);
    if (ret != TDB_SUCCESS && ret != TDB_NO_DATA)
    {
        snprintf(buffer, 49, "Failed when excuting query, error no %d", ret);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }

    tuple = PyTuple_New(count);
    for (i=0;i<count;i++)
    {
        item = Py_BuildValue("(s,s,i,i,L,L,i,i,i,c,L,L,i,i,i,i,i,i,i)", 
            ptickdata[i].chWindCode,
            ptickdata[i].chCode,
            ptickdata[i].nDate,
            ptickdata[i].nTime,
            ptickdata[i].iVolume,
            ptickdata[i].iTurover,
            ptickdata[i].nSettle,
            ptickdata[i].nPosition,
            ptickdata[i].nCurDelta,
            ptickdata[i].chTradeFlag,
            ptickdata[i].iAccVolume,
            ptickdata[i].iAccTurover,
            ptickdata[i].nHigh,
            ptickdata[i].nLow,
            ptickdata[i].nOpen,
            ptickdata[i].nPrice,
            ptickdata[i].nPreClose,
            ptickdata[i].nPreSettle,
            ptickdata[i].nPrePosition
            );
        PyTuple_SetItem(tuple, i, item);
    }
    TDB_Free(ptickdata);
    return tuple;
}

static PyObject*
TDBClient_GetFutureAB(TDBClient *self, PyObject* args)
{
    char* chCode;
    TDBDefine_ReqFuture reqtick ={"",0, 0, 0, 0, 0};
    TDBDefine_FutureAB* ptickdata=NULL;
    int count=0;
    int i;
    char buffer [50] = {0};
    PyObject* item;
    PyObject* tuple;
    TDB_ERROR ret;
    PyArg_ParseTuple(args, "sii|iii", &chCode, 
        &reqtick.nBeginDate, &reqtick.nEndDate, 
        &reqtick.nBeginTime, &reqtick.nEndTime,
        &reqtick.nAutoComplete);
    
    strncpy(reqtick.chCode, chCode, sizeof(reqtick.chCode));

    ret = TDB_GetFutureAB(self->m_thandle, &reqtick, &ptickdata, &count);
    if (ret != TDB_SUCCESS && ret != TDB_NO_DATA)
    {
        snprintf(buffer, 49, "Failed when excuting query, error no %d", ret);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }

    tuple = PyTuple_New(count);
    for (i=0;i<count;i++)
    {
        item = Py_BuildValue("(s,s,i,i,L,L,i,i,i,c,L,L,i,i,i,i,i,I,i,I,i,i,i)", 
            ptickdata[i].chWindCode,
            ptickdata[i].chCode,
            ptickdata[i].nDate,
            ptickdata[i].nTime,
            ptickdata[i].iVolume,
            ptickdata[i].iTurover,
            ptickdata[i].nSettle,
            ptickdata[i].nPosition,
            ptickdata[i].nCurDelta,
            ptickdata[i].chTradeFlag,
            ptickdata[i].iAccVolume,
            ptickdata[i].iAccTurover,
            ptickdata[i].nHigh,
            ptickdata[i].nLow,
            ptickdata[i].nOpen,
            ptickdata[i].nPrice,
            ptickdata[i].nAskPrice[0],
            ptickdata[i].nAskVolume[0],
            ptickdata[i].nBidPrice[0],
            ptickdata[i].nBidVolume[0],
            ptickdata[i].nPreClose,
            ptickdata[i].nPreSettle,
            ptickdata[i].nPrePosition
            );
        PyTuple_SetItem(tuple, i, item);
    }
    TDB_Free(ptickdata);
    return tuple;
}

static PyObject*
TDBClient_GetKLine(TDBClient *self, PyObject* args)
{
    TDBDefine_ReqKLine reqK = {"", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    TDBDefine_KLine* pKLine = NULL;
    char* chcode;
    int nCount =0;
    int i;
    char buffer [50] = {0};
    PyObject* tuple;
    PyObject* item;
    TDB_ERROR ret;
    
    PyArg_ParseTuple(args, "siii", &chcode, &reqK.nBeginDate, &reqK.nEndDate, &reqK.nCycType);
    strncpy(reqK.chCode, chcode, sizeof(reqK.chCode));
    
    ret = TDB_GetKLine(self->m_thandle, &reqK, &pKLine, &nCount);

    if (ret != TDB_SUCCESS && ret != TDB_NO_DATA)
    {
        snprintf(buffer, 49, "Failed when excuting query, error no %d", ret);
        PyErr_SetString(PyExc_RuntimeError, buffer);
        return NULL;
    }
    
    tuple = PyTuple_New(nCount);
    for(i=0;i<nCount;i++)
    {
        item = Py_BuildValue("(s,s,i,i,i,i,i,i,L,L,i,i)", 
            pKLine[i].chWindCode,
            pKLine[i].chCode,
            pKLine[i].nDate,
            pKLine[i].nTime,
            pKLine[i].nOpen,
            pKLine[i].nHigh,
            pKLine[i].nLow,
            pKLine[i].nClose,
            pKLine[i].iVolume,
            pKLine[i].iTurover,
            pKLine[i].nMatchItems,
            pKLine[i].nInterest
            );
        PyTuple_SetItem(tuple, i, item);
        
    }
    TDB_Free(pKLine);
    return tuple;
    
}

static PyObject
*TDBClient_close(TDBClient *self)
{
    int ret=0;
    if(self->m_thandle != NULL)
    {
        ret = TDB_Close(self->m_thandle);
        self->m_thandle = NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMemberDef TDBClient_members[] = {
    {"ip", T_OBJECT_EX, offsetof(TDBClient, ip), 0,
     "ip"},
    {"port", T_OBJECT_EX, offsetof(TDBClient, port), 0,
     "port"},
    {"username", T_OBJECT_EX, offsetof(TDBClient, username), 0,
     "username"},
    {"password", T_OBJECT_EX, offsetof(TDBClient, password), 0,
     "password"},
    {NULL}  /* Sentinel */
};

static PyMethodDef TDBClient_methods[] = {
    {"connect", (PyCFunction)TDBClient_connect, METH_NOARGS,
     "Connect to tdb server"
    },
    {"close", (PyCFunction)TDBClient_close, METH_NOARGS,
     "Close the connection"
    },
    {"GetCodeTable", (PyCFunction)TDBClient_GetCodeTable, METH_VARARGS,
     "GetCodeTable"
    },
    {"GetTick", (PyCFunction)TDBClient_GetTick, METH_VARARGS,
     "TDBClient_GetTick"
    },
    {"GetTickAB", (PyCFunction)TDBClient_GetTickAB, METH_VARARGS,
     "TDBClient_GetTickAB"
    },
    {"GetFuture", (PyCFunction)TDBClient_GetFuture, METH_VARARGS,
     "TDBClient_GetFuture"
    },    
    {"GetFutureAB", (PyCFunction)TDBClient_GetFutureAB, METH_VARARGS,
     "TDBClient_GetFutureAB"
    },
    {"GetKLine", (PyCFunction)TDBClient_GetKLine, METH_VARARGS, 
    "TDBClient Get kline."},
    {NULL}  /* Sentinel */
};

static PyTypeObject TDBClientType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "core.TDBClient",             /*tp_name*/
    sizeof(TDBClient),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)TDBClient_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "TDBClient  objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    TDBClient_methods,             /* tp_methods */
    TDBClient_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)TDBClient_init,      /* tp_init */
    0,                         /* tp_alloc */
    TDBClient_new,                 /* tp_new */
};


static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initcore(void)
{
    PyObject *m;

    if (PyType_Ready(&TDBClientType) < 0)
        return;
    
    m = Py_InitModule3("core", module_methods,
                       "Example module that creates an extension type.");

    Py_INCREF(&TDBClientType);
    PyModule_AddObject(m, "TDBClient", (PyObject *)&TDBClientType);
}

