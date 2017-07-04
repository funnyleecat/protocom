#pragma once

#include "IProtClient.h"
#include "ITcpClient.h"
#include "LogHandler.h"
#include "ProtStack.h"
#include "MessageHandler.h"

namespace protcomm
{
    class CMessageHandler;
    class CProtClient
        : public IProtClient
    {
        friend class IProtClient;
    public:
        //构造析构
        CProtClient();
        virtual ~CProtClient();

	public:
		void Initlize();

        //设置参数
		virtual size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData);
        //获得参数配置
		virtual size_t GetOpt(E_PROTOCOL_OPT eOpt)const;

		virtual bool Start();
		virtual void Stop();
        //建立链接
        virtual size_t Connect(const char* pszAddress, unsigned short nPort);
        //关闭连接
        virtual void Close(size_t nCtxId);

        //发送数据
        virtual bool SendPackageData(size_t nCtxId,
            int nFuncId,
            PROTO_PACK_TYPE ePackType,
            char* pData,
            std::size_t dwDataLen,
            const PROTOCOL_UUID& tUUID,
            std::size_t nCltPackId,
            int nUserData1,
            int nUserData2);
        virtual bool ForwardFragData(size_t nCtxId, char* pData, std::size_t dwDataLen, pfSentCallback_t pfbCB);
		
    private:
		CLogHandler*          m_pLogHandler;   //日志回调对象
		tcpcomm2::ITcpClient* m_pTcpClient;;   //服务端对象

		CProtStack*           m_pProtStack;    //协议栈处理对象
		CMessageHandler*      m_pMsgHandler;   //消息处理对象
    };
}

