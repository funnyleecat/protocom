#include "stdafx.h"
#include "ProtServer.h"

#ifdef StartService
#undef StartService
#endif

namespace protcomm
{
    IProtServer* IProtServer::GetInstance(int nIothread, int nWorkthread, int nKeepalive)
    {
        CProtServer* pServer = new CProtServer;
        pServer->m_pTcpSrv = tcpcomm2::ITcpServer::GetInstance(nIothread, nWorkthread, nKeepalive);
		pServer->Initlize();
        return pServer;
    }

    void IProtServer::Release(IProtServer* pInstance)
    {
        if (pInstance)
        {
            if (dynamic_cast<CProtServer*>(pInstance))
            {
                delete pInstance;
                pInstance = NULL;
            }
        }
    }

    CProtServer::CProtServer()
    {
		m_pLogHandler = NULL;
		m_pTcpSrv = NULL;

		m_pProtStack = new CProtStack();
		assert(NULL != m_pProtStack);

		m_pMsgHandler = new CMessageHandler();
		assert(NULL != m_pMsgHandler);
		m_pMsgHandler->SetProtStack(m_pProtStack); //��CMessageHandler����Э�鴦�����
    }

	CProtServer::~CProtServer()
	{
		if (NULL != m_pTcpSrv)
		{
			tcpcomm2::ITcpServer::Release(m_pTcpSrv);
		}

		if (NULL != m_pMsgHandler)
		{
			delete m_pMsgHandler;
			m_pMsgHandler = NULL;
		}

		if (NULL != m_pProtStack)
		{
			delete m_pProtStack;
			m_pProtStack = NULL;
		}

		if (NULL != m_pLogHandler)
		{
			delete m_pLogHandler;
			m_pLogHandler = NULL;
		}
	}

	void CProtServer::Initlize()
	{
		m_pMsgHandler->SetSendFunc(boost::bind(&tcpcomm2::ITcpServer::SendTcpMessage, m_pTcpSrv, _1, _2));
		m_pTcpSrv->SetSrvOption(/*ESrvOpt::*/OPT_SOCK_RCVBUF, MAX_FRAG_SIZE * 2);
	}

    //���ò���
	size_t CProtServer::SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData)
    {
		switch (eOpt)
		{
		case OPT_PROTOCOL_LOGGER_HADNLER:
			assert(NULL != (CLogOutput*)dwData);
			m_pLogHandler = new CLogHandler((CLogOutput*)dwData);
			assert(NULL != m_pLogHandler);
			m_pTcpSrv->SetSrvOption(/*ESrvOpt::*/OPT_LOGGER_HADNLER, (std::size_t)m_pLogHandler);
			m_pProtStack->SetOpt(OPT_PROTOCOL_LOGGER_HADNLER, (size_t)m_pLogHandler);
			break;
		case OPT_DATA_DELIVERY:
			assert(NULL != (CDataDelivery*)dwData);
			m_pMsgHandler->SetUserHandler((CDataDelivery*)dwData); //��CMessageHandler���ûص�����
			m_pTcpSrv->SetSrvOption(OPT_MESSAGE_HADNLER, (size_t)m_pMsgHandler);
			break;
		default:
			m_pProtStack->SetOpt(eOpt, dwData);
			break;
		}

		return 0;

    }

    //��ò�������
	size_t CProtServer::GetOpt(E_PROTOCOL_OPT eOpt)const
    {
		size_t dwOpt = 0;
		switch (eOpt)
		{
		case OPT_PROTOCOL_LOGGER_HADNLER:
			dwOpt = (std::size_t)(m_pLogHandler->m_pOutput);
			break;
		default:
			dwOpt = m_pProtStack->GetOpt(eOpt);
			break;
		}

		return dwOpt;
    }

    //��������
    bool CProtServer::Start(unsigned short nPort)
    {
		m_pProtStack->Start();
		return m_pTcpSrv->StartService(nPort);
    }

    //ֹͣ����
    void CProtServer::Stop()
    {
		m_pTcpSrv->StopService();
    }

    //�ر�����
    void CProtServer::Close(size_t nCtxId)
    {
		m_pTcpSrv->Close(nCtxId);
    }

    //��������
    bool CProtServer::SendPackageData(size_t nCtxId,
        int nFuncId,
        PROTO_PACK_TYPE ePackType,
        char* pData,
        std::size_t dwDataLen,
        const PROTOCOL_UUID& tUUID,
        std::size_t nCltPackId,
        int nUserData1 ,
        int nUserData2 )
    {
        //���մ�������Ƭ
        list<TFragmentMessage> tFragList;
		bool bPrepare = m_pProtStack->SendPrepare(nFuncId, ePackType, pData, dwDataLen, 
            tUUID, nCltPackId, nUserData1, nUserData2, tFragList);
        bool bRet = true;
        if (bPrepare)
        {
            for (list<TFragmentMessage>::iterator it = tFragList.begin(); it != tFragList.end(); ++it)
            {
                if (bRet)
                {
                    TcpMessage tcpMsg;
                    tcpMsg.pData = it->pData;
                    tcpMsg.nDataLen = it->nDataLen;
                    tcpMsg.pCallback = RelaseFragBuf;
                    bRet = m_pTcpSrv->SendTcpMessage(nCtxId, &tcpMsg);
                }
                else if (it->pData)
                {
                    delete[] it->pData;//�����һ����Ƭ����ʧ�ܣ�����ķ�Ƭȫ��ɾ��
                }
            }
        }

        return bRet;
    }

    bool CProtServer::ForwardFragData(size_t nCtxId, char* pData, std::size_t dwDataLen, pfSentCallback_t pfbCB)
    {
        TcpMessage tTcpMsg;
        tTcpMsg.pData = pData;
        tTcpMsg.nDataLen = dwDataLen;
        tTcpMsg.pCallback = pfbCB;
        return m_pTcpSrv->SendTcpMessage(nCtxId, &tTcpMsg);
    }
}
