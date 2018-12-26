/**
* @defgroup  MAINFRAME ϵͳ��Դ��
* @author 
* @version 1.0
* @date 2010-2013
* @warning ��Ȩ���У���¼�ؾ�
* @{
* Tips:
*/

#ifndef _TDBAPI_H_
#define _TDBAPI_H_

#include "TDBAPIStruct.h"

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(_WINDOWS)
#define __PLATFORM_WINDOWS__
#else
#define __PLATFORM_LINUX__
#endif

#ifdef __PLATFORM_WINDOWS__
    #ifdef TDB_API_EXPORT
        #define TDBAPI __declspec(dllexport)
    #else    
        #define TDBAPI __declspec(dllimport)
    #endif
#else
    #define TDBAPI
#endif

#ifdef __cplusplus
extern "C"
{
#endif


//��һ����TDB�����������ӣ�������ӳɹ����򷵻����Ӿ����ʧ�ܷ���NULL��
//loginRes�з����г�����̬�������ڣ�
TDBAPI THANDLE TDB_Open(const OPEN_SETTINGS* pSetting, TDBDefine_ResLogin* loginRes);

//ͨ����������ӣ���������TDB_Openһ��
TDBAPI THANDLE TDB_OpenProxy(const OPEN_SETTINGS* pSetting, TDB_PROXY_SETTING* pProxySetting, TDBDefine_ResLogin* loginRes);

//�����ر����ӣ�������Handle
TDBAPI int TDB_Close(THANDLE hTdb);

//��ȡ�����
//����ֵ��
//TDB_SUCCESS: �ɹ�
//TDB_INVALID_PARAMS: ��Ч�Ĳ�����hTdb��Ч��pCountΪ�գ�
//TDB_NETWORK_ERROR���������Ӵ���
//��szMarketΪ�գ��򷵻������г��Ĵ���
//��pCodeTableΪ�գ�pCount��Ϊ�գ��򷵻�
//*pCodeTable��ָ����ڴ���Ҫ�û���ʽ�ĵ��� TDB_Free �ͷ�
TDBAPI int TDB_GetCodeTable(THANDLE hTdb, const char* szMarket, TDBDefine_Code** pCodeTable, int* pCount);


//��ȡK��
//����ֵ��
//TDB_SUCCESS: �ɹ�
//TDB_INVALID_PARAMS: ��Ч�Ĳ���(hTdb��Ч��pReqΪ�գ�pCountΪ��)
//TDB_NETWORK_ERROR���������Ӵ���
//TDB_NO_DATA: û������
//TDB_NETWORK_TIMEOUT:��ʱ
//TDB_OUT_OF_MEMORY���ڴ�ľ�
//��pDataΪ�գ�pCount��Ϊ�գ��򷵻ؼ�¼����
//*pData ָ����ڴ����Ҫ�û���ʽ�ĵ���TDB_Free �ͷ�
TDBAPI int TDB_GetKLine(THANDLE hTdb, const TDBDefine_ReqKLine* pReq, TDBDefine_KLine** pData, int* pCount);


//��ȡ��������(�������̿�)����õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetTickAB(THANDLE hTdb, const TDBDefine_ReqTick* pReq, TDBDefine_TickAB** pData, int* pCount);

//��ȡ��������(���������̿�)����õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetTick(THANDLE hTdb, const TDBDefine_ReqTick* pReq, TDBDefine_Tick** pData, int* pCount);


//��ȡ�ڻ�����(���������̿�)����õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetFuture(THANDLE hTdb, const TDBDefine_ReqFuture* pReq, TDBDefine_Future** pData, int* pCount);

//��ȡ�ڻ����飬��õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetFutureAB(THANDLE hTdb, const TDBDefine_ReqFuture* pReq, TDBDefine_FutureAB** pData, int* pCount);

//��ȡ�������ϸ��Ϣ,������Ӷϵ�����δ���ӣ��򷵻�NULL�����ص�ָ����API�û������ͷ�֮
TDBAPI const TDBDefine_Code* TDB_GetCodeInfo(THANDLE hTdb, const char* szWindCode);

//��ȡ��ʳɽ����ݣ���õĽ����Ҫ�û��������� TDB_Free���ͷ�
TDBAPI int TDB_GetTransaction(THANDLE hTdb, const TDBDefine_ReqTransaction* pReq, TDBDefine_Transaction** pData, int* pCount);

//��ȡ���ί�����ݣ���õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetOrder(THANDLE hTdb, const TDBDefine_ReqOrder* pReq, TDBDefine_Order** pData, int* pCount);

//��ȡί�ж������ݣ���õĽ����Ҫ�û��������� TDB_Free ���ͷ�
TDBAPI int TDB_GetOrderQueue(THANDLE hTdb, const TDBDefine_ReqOrderQueue* pReq, TDBDefine_OrderQueue** pData, int* pCount);

//����ָ�깫ʽ
TDBAPI int TDB_AddFormula(THANDLE hTdb, const char* szName, const char* szContent, TDBDefine_AddFormulaRes* pRes);

//��ȡ�������ϵ�ָ�깫ʽ����õĽ����Ҫ�û��������� TDB_Free ���ͷţ����Ϊ�ջ���""�����ȡ���е�ָ�깫ʽ
TDBAPI int TDB_GetFormula(THANDLE hTdb, const char* szFormulaName, TDBDefine_FormulaItem** pFormula, int *pCount);

//����ָ�깫ʽ������ȡ�����pResult����Ϊ�գ�����CalcFormulaResult�ڲ��Ķ�̬�ڴ��ֶ���API���䡣
//��ȡ�����֮��Ӧ��������TDB_ReleaseCalcFormula�ͷ��ڴ�
TDBAPI int TDB_CalcFormula(THANDLE hTdb, const TDBDefine_ReqCalcFormula* pReqCalc, TDBDefine_CalcFormulaRes* pResult);

//�ͷ�TDB_CalcFormula���û�ȡ��TDBDefine_CalcFormulaResult����еĶ�̬�ڴ棬pResult���뱻 TDB_CalcFormula��ʼ����
TDBAPI void TDB_ReleaseCalcFormula(TDBDefine_CalcFormulaRes* pResult);


TDBAPI void TDB_Free(void* p);

//ɾ��ָ�깫ʽ
TDBAPI int TDB_DeleteFormula(THANDLE hTdb, const char* szFormulaName, TDBDefine_DelFormulaRes* pDelRes);

////�����ǰ���ڴ������ݣ�����ֹ֮
//TDBAPI void TDB_AbortRecv(THANDLE hTdb);


//���ָ�깫ʽ������ͷ��ڴ�
//TDBAPI int TDB_ReleaseEZFResult(THANDLE hTdb, TDBDefine_EZFormula_ResultData* pRetData);


#ifdef __cplusplus
}
#endif

#endif//_TDBAPI_H_
