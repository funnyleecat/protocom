#pragma once
#include "Protocol.h"
#include "protcomm_export.h"
#include <cstddef>
namespace protcomm
{
    /*=========================================================================
    �����������ݽ����Ľӿ�
    �ӿ�  ��
    ��ע  ��
    ����  ��2016.10 xly
    =========================================================================*/
    class CDataDelivery
    {
    public:
        virtual ~CDataDelivery(){};
        //����������ҵ���
        virtual void DeliverPackage(std::size_t nCtxId,
            int nFuncId,
            PROTO_PACK_TYPE ePackType,
            const char* pReq,
            std::size_t nReqLen,
            const PROTOCOL_UUID& tUUID,
            std::size_t nCltPackId,
            int nUserData1,
            int nUserData2)
        {
        };
        //����ҵ���Ƭ���ݣ�����pMsgData�������ReleaseProtoBuff�ͷ�
        virtual void DeliverFragment(std::size_t nCtxId,
            const char* pMsgData,
            std::size_t nMsgLen,
            bool bWithMsgHead
            )
        {
        };
        //�������ӵ�֪ͨ
        virtual void OnConnected(std::size_t nCtxId){};
        //�Ͽ����ӵ�֪ͨ
        virtual void OnDisconnect(std::size_t nCtxId){};
    };
    /*=========================================================================
    ����������־�������
    �ӿ�  ��
    ��ע  ��
    ����  ��2016.10 xly
    =========================================================================*/
    class CLogOutput
    {
    public:
        virtual ~CLogOutput(){};
        virtual void Debug(const char* pszString){};
        virtual void Info(const char* pszString) {};
        virtual void Warn(const char* pszString) {};
        virtual void Error(const char* pszString){};
        virtual void Fatal(const char* pszString){};
    };

    //�ͷ���Protocomm���ٵ��ڴ�
    void PROTCOMM_API ReleaseProtoBuff(void* pProtoBuff);
}
