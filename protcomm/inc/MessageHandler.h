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

		//����Э�鴦�����
		void SetProtStack(CProtStack* pProtStack);

        //�����û�����ص�����
        void SetUserHandler(CDataDelivery* pUserHandler);

		//���÷������ݵĺ�������
		void SetSendFunc(boost::function<bool(std::size_t, const TcpMessage*)> SendFunc);

	private:
		bool CreateKeepalive();            //�����̶�������
		void SendKeepalive(size_t nCtxId); //����������

    private:
		CDataDelivery* m_pDataDelivery; //���û��������ݵĻص�����
		CProtStack*    m_pProtStack;    //Э�����ݰ��������

		boost::function<bool(std::size_t, const TcpMessage*)> m_SendFunc; //�������ݵĺ�������
		bool m_bKeepaliveCreated;
		list<TFragmentMessage> m_keepaliveFragList; //�����̶���Ƭ
    };
}
