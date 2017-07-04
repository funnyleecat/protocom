#include "stdafx.h"
#include "ProtClient.h"

namespace protcomm
{
    IProtClient* IProtClient::GetInstance(int nIothread, int nWorkthread)
    {
        CProtClient* pClient = new CProtClient;
        pClient->m_pTcpClient = tcpcomm2::ITcpClient::GetInstance(nIothread, nWorkthread);
		pClient->Initlize();
        return pClient;
    }

    void IProtClient::Release(IProtClient* pInstance)
    {
        if (pInstance)
        {
            delete pInstance;
            pInstance = NULL;
        }
    }

    //构造析构
    CProtClient::CProtClient()
    {
		m_pLogHandler = NULL;
		m_pTcpClient = NULL;

		m_pProtStack = new CProtStack();
		assert(NULL != m_pProtStack);

		m_pMsgHandler = new CMessageHandler();
		assert(NULL != m_pMsgHandler);
		m_pMsgHandler->SetProtStack(m_pProtStack); //给CMessageHandler设置协议处理对象
    }

    CProtClient::~CProtClient()
    {
        if (NULL != m_pTcpClient)
        {
            m_pTcpClient->StopService();
            tcpcomm2::ITcpClient::Release(m_pTcpClient);
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

	void CProtClient::Initlize()
	{
		m_pMsgHandler->SetSendFunc(boost::bind(&tcpcomm2::ITcpClient::SendTcpMessage, m_pTcpClient, _1, _2));
		m_pTcpClient->SetSrvOption(/*ESrvOpt::*/OPT_SOCK_RCVBUF, MAX_FRAG_SIZE);
	}

    //设置参数
	size_t CProtClient::SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData)
    {
		switch (eOpt)
		{
		case OPT_PROTOCOL_LOGGER_HADNLER:
			assert(NULL != (CLogOutput*)dwData);
			m_pLogHandler = new CLogHandler((CLogOutput*)dwData);
			assert(NULL != m_pLogHandler);
			m_pTcpClient->SetSrvOption(/*ESrvOpt::*/OPT_LOGGER_HADNLER, (std::size_t)m_pLogHandler);
			m_pProtStack->SetOpt(OPT_PROTOCOL_LOGGER_HADNLER, (size_t)m_pLogHandler);
			break;
		case OPT_DATA_DELIVERY:
			assert(NULL != (CDataDelivery*)dwData);
			m_pMsgHandler->SetUserHandler((CDataDelivery*)dwData); //给CMessageHandler设置回调对象
			m_pTcpClient->SetSrvOption(OPT_MESSAGE_HADNLER, (size_t)m_pMsgHandler);
			break;
		default:
			m_pProtStack->SetOpt(eOpt, dwData);
			break;
		}

		return 0;
    }

    //获得参数配置
	size_t CProtClient::GetOpt(E_PROTOCOL_OPT eOpt) const
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

	bool CProtClient::Start()
	{
		m_pProtStack->Start();
		return m_pTcpClient->StartService();
	}

	void CProtClient::Stop()
	{
		m_pTcpClient->StopService();
	}

    //建立连接
    size_t CProtClient::Connect(const char* pszAddress, unsigned short nPort)
    {
		return m_pTcpClient->Connect(pszAddress, nPort);
    }

    //关闭连接
    void CProtClient::Close(size_t nCtxId)
    {
		m_pTcpClient->Close(nCtxId);
    }

    //发送数据
    bool CProtClient::SendPackageData(size_t nCtxId,
        int nFuncId,
        PROTO_PACK_TYPE ePackType,
        char* pData,
        std::size_t dwDataLen,
        const PROTOCOL_UUID& tUUID,
        std::size_t nCltPackId ,
        int nUserData1,
        int nUserData2)
    {
        //接收处理后的碎片
        list<TFragmentMessage> tFragList;
		bool bPrepare = m_pProtStack->SendPrepare(nFuncId, ePackType, pData, dwDataLen, 
			tUUID, nCltPackId, nUserData1, nUserData2, tFragList);
        bool bRet = true;
        if (bPrepare)
        {
            for (list<TFragmentMessage>::iterator it = tFragList.begin(); it != tFragList.end();++it)
            {
                if (bRet)
                {
                    TcpMessage tcpMsg;
                    tcpMsg.pData = it->pData;
                    tcpMsg.nDataLen = it->nDataLen;
                    tcpMsg.pCallback = RelaseFragBuf;
                    bRet = m_pTcpClient->SendTcpMessage(nCtxId, &tcpMsg);
                }
                else if (it->pData)
                {
                    delete[] it->pData;//如果有一个分片发送失败，后面的分片全部删除
                }
            }
        }

        return bRet;
    }

    bool CProtClient::ForwardFragData(size_t nCtxId, char* pData, std::size_t dwDataLen, pfSentCallback_t pfbCB)
    {
        TcpMessage tTcpMsg;
        tTcpMsg.pData = pData;
        tTcpMsg.nDataLen = dwDataLen;
        tTcpMsg.pCallback = pfbCB;
        return m_pTcpClient->SendTcpMessage(nCtxId, &tTcpMsg);
    }
}
