#pragma once
#include "IMessageHandler.h"
#include "ProtStack.h"
#include "boost/function.hpp"
#include <list>

struct tagTcpMessage;
typedef tagTcpMessage TcpMessage;
namespace protcomm
{
	class CProtStack;

    class CMessageHandler : public tcpcomm2::IMessageHandler
    {
    public:
		CMessageHandler();
        virtual ~CMessageHandler();

	public:
        size_t HandleCommRecvBuffer(size_t nCtxId, const char* pReq, size_t nReqLen);
        void HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen);
        void HandleCommConnected(size_t nCtxId);
        void HandleCommDisconnect(size_t nCtxId);

		//设置协议处理对象
		void SetProtStack(CProtStack* pProtStack);

        //设置用户处理回调对象
        void SetUserHandler(CDataDelivery* pUserHandler);

		//设置发送数据的函数对象
		void SetSendFunc(boost::function<bool(std::size_t, const TcpMessage*)> SendFunc);

	private:
		bool CreateKeepalive();            //创建固定心跳包
		void SendKeepalive(size_t nCtxId); //发送心跳包

    private:
		CDataDelivery* m_pDataDelivery; //向用户交付数据的回调对象
		CProtStack*    m_pProtStack;    //协议数据包处理对象

		boost::function<bool(std::size_t, const TcpMessage*)> m_SendFunc; //发送数据的函数对象
		bool m_bKeepaliveCreated;
		list<TFragmentMessage> m_keepaliveFragList; //心跳固定分片
    };
}
