#include "stdafx.h"
#include "MessageHandler.h"
#include "Protocol.h"
#ifdef WIN32
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib_tcpcomm2/sources/tcpcomm2/win64/Debug/tcpcomm2.lib")
#else
#pragma comment(lib, "../../../../lib_tcpcomm2/sources/tcpcomm2/win64/Release/tcpcomm2.lib")
#endif  //_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib_tcpcomm2/sources/tcpcomm2/win32/Debug/tcpcomm2.lib")
#else
#pragma comment(lib, "../../../../lib_tcpcomm2/sources/tcpcomm2/win32/Release/tcpcomm2.lib")
#endif  //_DEBUG
#endif  //_WIN64
#endif  //WIN32

namespace protcomm
{
	using namespace tcpcomm2;

	CMessageHandler::CMessageHandler()
	{
		m_pDataDelivery = NULL;
		m_pProtStack = NULL;
		m_bKeepaliveCreated = false;
	}

	CMessageHandler::~CMessageHandler()
	{
		for (list<TFragmentMessage>::iterator it = m_keepaliveFragList.begin(); it != m_keepaliveFragList.end(); ++it)
		{
			if (NULL != it->pData)
			{
				delete[] it->pData;
				it->pData = NULL;
			}
		}
	}

#define IF_DO(cond,act1) \
	if (cond)  \
	{		  \
	act1; \
	}
	size_t CMessageHandler::HandleCommRecvBuffer(size_t nCtxId, const char* pReq, size_t nReqLen)
	{
		//标识是否有msghead
		bool bWithMsgHead = m_pProtStack->IsRcvMsgHead();
		if (bWithMsgHead)
		{
			IF_DO(sizeof(PROTOCOL_FRAG_HEAD)+sizeof(PROTOCOL_MSG_HEAD) >= nReqLen, return 0);
			PROTOCOL_MSG_HEAD* pHeader = (PROTOCOL_MSG_HEAD*)pReq;

			PROTOCOL_FRAG_HEAD *pFragHead = (PROTOCOL_FRAG_HEAD*)(pReq + sizeof(PROTOCOL_MSG_HEAD));
			IF_DO(pFragHead->nSegSize > MAX_FRAG_DATA_SIZE, return -1);

			size_t nPkgLen = pFragHead->nSegSize + sizeof(PROTOCOL_FRAG_HEAD)+sizeof(PROTOCOL_MSG_HEAD);
			IF_DO(nPkgLen > nReqLen, return 0);
			return nPkgLen;
		}

		else
		{
			IF_DO(sizeof(PROTOCOL_FRAG_HEAD) >= nReqLen, return 0);
			PROTOCOL_FRAG_HEAD *pFragHead = (PROTOCOL_FRAG_HEAD*)(pReq);
			IF_DO(pFragHead->nSegSize > MAX_FRAG_DATA_SIZE, return -1);

			size_t nPkgLen = pFragHead->nSegSize + sizeof(PROTOCOL_FRAG_HEAD);
			IF_DO(nPkgLen > nReqLen, return 0);
			return nPkgLen;
		}
		return 0;
	}

	void CMessageHandler::HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen)
	{
		//处理协议数据包
		char chDeliveryInfo[sizeof(TFullPackageDeliveryInfo) > sizeof(TFragmentDeliveryInfo) ? sizeof(TFullPackageDeliveryInfo) : sizeof(TFragmentDeliveryInfo)] = { 0 };
		EmProtPackageType emProtPackageType = m_pProtStack->HandleCommRequest(nCtxId, pReq, nReqLen, (char*)chDeliveryInfo);
		switch (emProtPackageType)
		{
		case emProtReqKeepalive:
			//回复心跳响应包
			if (!m_bKeepaliveCreated && CreateKeepalive())
			{
				m_bKeepaliveCreated = true;
			}
			SendKeepalive(nCtxId);
			break;
		case emProtFullPackage:
		{
			//交付完整数据包
			TFullPackageDeliveryInfo* pFullDeliveryInfo = (TFullPackageDeliveryInfo*)chDeliveryInfo;
			m_pDataDelivery->DeliverPackage(nCtxId, pFullDeliveryInfo->nFuncId,
				(PROTO_PACK_TYPE)(pFullDeliveryInfo->pProtHead->chType), pFullDeliveryInfo->pProtHead->chBodyData,
				pFullDeliveryInfo->pProtHead->nBodyLen,
				pFullDeliveryInfo->tUUID, pFullDeliveryInfo->nCltPackId,
				pFullDeliveryInfo->pProtHead->nUsrData1, pFullDeliveryInfo->pProtHead->nUsrData2);

			if (NULL != pFullDeliveryInfo->pProtHead)
			{
				delete[] (char*)(pFullDeliveryInfo->pProtHead);
				pFullDeliveryInfo->pProtHead = NULL;
			}
			
			break;
		}
		case emProtFragment:
		{
			//交付分片数据包
			TFragmentDeliveryInfo* pFragDeliveryInfo = (TFragmentDeliveryInfo*)chDeliveryInfo;
			m_pDataDelivery->DeliverFragment(nCtxId, pFragDeliveryInfo->pMsgData, pFragDeliveryInfo->nMsgLen, pFragDeliveryInfo->bWithMsgHead);

			break;
		}
		default:
			break;
		}
	}

	void CMessageHandler::HandleCommConnected(size_t nCtxId)
	{
		if (NULL != m_pDataDelivery)
		{
			m_pDataDelivery->OnConnected(nCtxId);
		}
	}

	void CMessageHandler::HandleCommDisconnect(size_t nCtxId)
	{
		if (NULL != m_pDataDelivery)
		{
			m_pDataDelivery->OnDisconnect(nCtxId);
		}
	}

	//设置协议处理对象
	void CMessageHandler::SetProtStack(CProtStack* pProtStack)
	{
		assert(NULL != pProtStack);
		m_pProtStack = pProtStack;
	}

	//设置用户处理回调对象
	void CMessageHandler::SetUserHandler(CDataDelivery* pUserHandler)
	{
		assert(NULL != pUserHandler);
		m_pDataDelivery = pUserHandler;
	}

	//设置发送数据的函数对象
	void CMessageHandler::SetSendFunc(boost::function<bool(std::size_t, const TcpMessage*)> SendFunc)
	{
		m_SendFunc = SendFunc;
	}

	bool CMessageHandler::CreateKeepalive()
	{
		bool bCreate = false;

		bool bMsgHead = m_pProtStack->IsRcvMsgHead();
		size_t deSendLen = sizeof(PROTOCOL_HEAD) + sizeof(PROTOCOL_KEEPALVIE);
		char* pProtocolBuffer = new char[deSendLen];
		if (pProtocolBuffer == NULL)
		{
			bCreate = false;
			return bCreate;
		}
		memset(pProtocolBuffer, 0, sizeof(PROTOCOL_HEAD));
		PROTOCOL_HEAD* pSendBuf = (PROTOCOL_HEAD*)pProtocolBuffer;

		pSendBuf->chVersion = PROTOCOL_VERSION;
		pSendBuf->chType = PROT_ACK_KEEPALIVE;
		pSendBuf->chCipher = 0;
		pSendBuf->chCompress = 0;
		pSendBuf->nFuncId = 0;
		pSendBuf->nSrcLen = sizeof(PROTOCOL_KEEPALVIE);
		pSendBuf->nBodyLen = sizeof(PROTOCOL_KEEPALVIE);
		pSendBuf->nUsrData1 = 0;
		pSendBuf->nUsrData2 = 0;
		if (bMsgHead)
		{
			PROTOCOL_MSG_HEAD tMsgHead = { 0 };
			bCreate = m_pProtStack->BuildFragment(pSendBuf, tMsgHead, m_keepaliveFragList, 0);
			PROTOCOL_FRAG_HEAD *pHead = (PROTOCOL_FRAG_HEAD *)m_keepaliveFragList.begin()->pData;
		}
		else
		{
			bCreate = m_pProtStack->BuildFragment(pSendBuf, m_keepaliveFragList, 0);
			PROTOCOL_FRAG_HEAD *pHead = (PROTOCOL_FRAG_HEAD *)m_keepaliveFragList.begin()->pData;
		}

		if (NULL != pProtocolBuffer)
		{
			delete[] pProtocolBuffer;
			pProtocolBuffer = NULL;
		}

		return bCreate;
	}

	void CMessageHandler::SendKeepalive(size_t nCtxId)
	{
		for (list<TFragmentMessage>::iterator it = m_keepaliveFragList.begin(); it != m_keepaliveFragList.end(); ++it)
		{
			TcpMessage tcpMsg;
			tcpMsg.pData = it->pData;
			tcpMsg.nDataLen = it->nDataLen;
			tcpMsg.pCallback = NULL;
			m_SendFunc(nCtxId, &tcpMsg);
		}
	}
}
