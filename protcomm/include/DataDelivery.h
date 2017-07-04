#pragma once
#include "Protocol.h"
#include "protcomm_export.h"
#include <cstddef>
namespace protcomm
{
    /*=========================================================================
    类描述：数据交付的接口
    接口  ：
    备注  ：
    日期  ：2016.10 xly
    =========================================================================*/
    class CDataDelivery
    {
    public:
        virtual ~CDataDelivery(){};
        //交付完整的业务包
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
        //交付业务分片数据，其中pMsgData必须调用ReleaseProtoBuff释放
        virtual void DeliverFragment(std::size_t nCtxId,
            const char* pMsgData,
            std::size_t nMsgLen,
            bool bWithMsgHead
            )
        {
        };
        //建立连接的通知
        virtual void OnConnected(std::size_t nCtxId){};
        //断开连接的通知
        virtual void OnDisconnect(std::size_t nCtxId){};
    };
    /*=========================================================================
    类描述：日志输出对象
    接口  ：
    备注  ：
    日期  ：2016.10 xly
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

    //释放由Protocomm开辟的内存
    void PROTCOMM_API ReleaseProtoBuff(void* pProtoBuff);
}
