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
        //��������
        CProtClient();
        virtual ~CProtClient();

	public:
		void Initlize();

        //���ò���
		virtual size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData);
        //��ò�������
		virtual size_t GetOpt(E_PROTOCOL_OPT eOpt)const;

		virtual bool Start();
		virtual void Stop();
        //��������
        virtual size_t Connect(const char* pszAddress, unsigned short nPort);
        //�ر�����
        virtual void Close(size_t nCtxId);

        //��������
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
		CLogHandler*          m_pLogHandler;   //��־�ص�����
		tcpcomm2::ITcpClient* m_pTcpClient;;   //����˶���

		CProtStack*           m_pProtStack;    //Э��ջ�������
		CMessageHandler*      m_pMsgHandler;   //��Ϣ�������
    };
}

