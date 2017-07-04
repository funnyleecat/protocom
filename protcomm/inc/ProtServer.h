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

        //���ò���
		size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData);
        //��ò�������
		virtual size_t GetOpt(E_PROTOCOL_OPT eOpt)const;

        //��������
        bool Start(unsigned short nPort);
        //ֹͣ����
        void Stop();
        //�ر�����
        void Close(size_t nCtxId);

        //��������
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
		CLogHandler*          m_pLogHandler;   //��־�ص�����
		tcpcomm2::ITcpServer* m_pTcpSrv;       //����˶���

		CProtStack*           m_pProtStack;    //Э��ջ�������
		CMessageHandler*      m_pMsgHandler;   //��Ϣ�������
    };
}

