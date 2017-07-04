#pragma once

#include "IProtServer.h"
#include "ITcpServer.h"
#include "LogHandler.h"
#include "ProtStack.h"
#include "MessageHandler.h"

namespace protcomm
{
    class CMessageHandler;
    class CProtServer
        : public IProtServer
    {
        friend  class IProtServer;
    public:
        CProtServer();
		virtual ~CProtServer();

	public:
		void Initlize();

        //设置参数
		size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData);
        //获得参数配置
		virtual size_t GetOpt(E_PROTOCOL_OPT eOpt)const;

        //启动服务
        bool Start(unsigned short nPort);
        //停止服务
        void Stop();
        //关闭连接
        void Close(size_t nCtxId);

        //发送数据
        virtual bool SendPackageData(size_t nCtxId,
            int nFuncId,
            PROTO_PACK_TYPE ePackType,
            char* pData,
            std::size_t dwDataLen,
            const PROTOCOL_UUID& tUUID,
            std::size_t nCltPackId,
            int nUserData1 ,
            int nUserData2 );
        virtual bool ForwardFragData(size_t nCtxId, char* pData, std::size_t dwDataLen, pfSentCallback_t pfbCB=NULL);

    private:		
		CLogHandler*          m_pLogHandler;   //日志回调对象
		tcpcomm2::ITcpServer* m_pTcpSrv;       //服务端对象

		CProtStack*           m_pProtStack;    //协议栈处理对象
		CMessageHandler*      m_pMsgHandler;   //消息处理对象
    };
}

