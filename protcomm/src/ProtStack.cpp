
#include "stdafx.h"
#include "ProtStack.h"
#include "zip_buffer.h"

extern "C"
{
	//回调函数释放内存
	void RelaseFragBuf(char* pBuf, size_t dwLen, bool bRet)
	{
		delete[] pBuf;
	}
}

namespace protcomm
{
	//释放由Protocomm开辟的内存
	void ReleaseProtoBuff(void* pProtoBuff)
	{
		//不做检查，传错了就该挂
		delete (char*)pProtoBuff;
	}

	CProtStack::CProtStack()
	{
		m_pLogHandler = NULL;
		m_pProtCfg = new CProtCfg();
	}

	CProtStack::~CProtStack()
	{
		if (NULL != m_pProtCfg)
		{
			delete m_pProtCfg;
			m_pProtCfg = NULL;
		}

		if (NULL != m_pBuildFrag)
		{
			delete m_pBuildFrag;
			m_pBuildFrag = NULL;
		}

		if (NULL != m_pBuildPack)
		{
			delete m_pBuildPack;
			m_pBuildPack = NULL;
		}
	}

	//设置参数
	size_t CProtStack::SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData)
	{
		switch (eOpt)
		{
		case OPT_PROTOCOL_LOGGER_HADNLER:
			m_pLogHandler = (CLogHandler*)dwData;
			break;
		case OPT_CIPHER:
			m_pProtCfg->SetCipher(dwData);
			break;
		case OPT_COMPRESS_LIMIT:
			m_pProtCfg->SetCompressLimit(dwData);
			break;
		case OPT_RCV_MSGHEAD:
			m_pProtCfg->SetRcvMsgHead(dwData);
			break;
		case  OPT_SND_MSGHEAD:
			m_pProtCfg->SetSndMsgHead(dwData);
			break;
		case OPT_RCV_FULLPACK:
			m_pProtCfg->m_pTransMode.SetRcvMode((int)dwData, /*E_TRANS_MODE::*/TRANS_FULLPACK);
			break;
		case OPT_RCV_FRAGMENT:
			m_pProtCfg->m_pTransMode.SetRcvMode((int)dwData, /*E_TRANS_MODE::*/TRANS_FRAGMENT);
			break;
		case OPT_FRAG_HASH_SIZE:
			assert(dwData > 0 && ((dwData & (dwData - 1)) == 0)); //要求2^n
			m_pProtCfg->SetFragHashSize(dwData);
			break;
		case OPT_FRAG_TIMEOUT:
			assert(dwData > 0);
			m_pProtCfg->SetFragTimeout(dwData);
			break;
		default:
			assert(0 == "不好意思跑偏了");
			break;
		}

		return 0;
	}

	//获得参数配置
	size_t CProtStack::GetOpt(E_PROTOCOL_OPT eOpt) const
	{
		size_t dwOpt = 0;
		switch (eOpt)
		{
		case OPT_CIPHER:;
			dwOpt = m_pProtCfg->GetCipher();
			break;
		case OPT_COMPRESS_LIMIT:
			dwOpt = m_pProtCfg->GetCompressLimit();
			break;
		case OPT_RCV_MSGHEAD:
			dwOpt = m_pProtCfg->GetRcvMsgHead();
			break;
		case  OPT_SND_MSGHEAD:
			dwOpt = m_pProtCfg->GetSndMsgHead();
			break;
		case OPT_RCV_FULLPACK:
		case OPT_RCV_FRAGMENT:
			//todo
			break;
		case OPT_FRAG_HASH_SIZE:
			dwOpt = m_pProtCfg->GetFragHashSize();
			break;
		case OPT_FRAG_TIMEOUT:
			dwOpt = m_pProtCfg->GetFragTimeout();
			break;
		default:
			assert(0 == "不好意思跑偏了");
			break;
		}
		return dwOpt;
	}

	bool CProtStack::Start()
	{
		if (NULL == m_pLogHandler)
		{
			return false;
		}

		m_pBuildFrag = new CProtocolFragment(m_pLogHandler);
		m_pBuildFrag->SetFragSize(MAX_FRAG_DATA_SIZE);
		m_pBuildFrag->SetMaxFragNum(4);

		m_pBuildPack = new CProtocolDefrag(m_pLogHandler);

		//设置分片模式
		m_pBuildFrag->SetFragMode(m_pProtCfg->GetSndMsgHead() == 0 ? emClientFragMode : emServerFragMode);

		//设置组包模式
		size_t dwFragHashSize = m_pProtCfg->GetFragHashSize();
		size_t dwFragTiemout = m_pProtCfg->GetFragTimeout();
		EMDefragMode emDefragMode = (m_pProtCfg->GetRcvMsgHead() == 0 ? emClientDefragMode : emServerDefragMode);
		//未设置hash大小，服务端模式默认为512,客户端模式默认为128
		if (0 == dwFragHashSize)
		{
			dwFragHashSize = (emServerDefragMode == emDefragMode ? 512 : 128);
		}
		//未设置分片超时，设为默认的60s
		if (0 == dwFragTiemout)
		{
			dwFragTiemout = 60;
		} 
		m_pBuildPack->SetDefragMode(emDefragMode, dwFragHashSize, dwFragTiemout);

		return true;
	}

	EmProtPackageType CProtStack::HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen, char* pDeliveryInfo)
	{
		PROTOCOL_FRAG_HEAD* pFragHead = NULL;
		PROTOCOL_HEAD* pProtHead = NULL;

		bool bMsgHead = m_pProtCfg->GetRcvMsgHead() == 0 ? false : true;
		if (bMsgHead)
		{
			pFragHead = (PROTOCOL_FRAG_HEAD*)(pReq + sizeof(PROTOCOL_MSG_HEAD));
			pProtHead = (PROTOCOL_HEAD*)(pReq + sizeof(PROTOCOL_MSG_HEAD)+sizeof(PROTOCOL_FRAG_HEAD));
		}
		else
		{
			pFragHead = (PROTOCOL_FRAG_HEAD*)(pReq);
			pProtHead = (PROTOCOL_HEAD*)(pReq + sizeof(PROTOCOL_FRAG_HEAD));
		}

		//心跳请求业务包
		if (pFragHead->nOffset == 0 && pProtHead->chType == PROT_REQ_KEEPALIVE) 
		{
			return emProtReqKeepalive;
		}
		//心跳返回业务包
		if (pProtHead->chType == PROT_ACK_KEEPALIVE)
		{
			return emProtAckKeepalive;
		}

		EmProtPackageType emProtPackageType = emProtContinue;
		E_TRANS_MODE eRcvMode = m_pProtCfg->m_pTransMode.GetRcvMode(pFragHead->nFuncId);
		do
		{
			if (TRANS_FRAGMENT == eRcvMode)
			{
				size_t dwMsgFragLen = nReqLen;
				if (false == bMsgHead)
				{
					dwMsgFragLen = nReqLen + sizeof(PROTOCOL_MSG_HEAD); //如果接收的不带msghead，则预留msghead内存
				}
				char* pFragBuf = new char[dwMsgFragLen];
				char* pDataPos = pFragBuf;
				if (false == bMsgHead)
				{
					pDataPos += sizeof(PROTOCOL_MSG_HEAD); //如果接收数据不带msghead，拷贝的数据要偏移掉预留的内存
				}
				memcpy(pDataPos, pReq, nReqLen);

				TFragmentDeliveryInfo* pFragDeliveryInfo = (TFragmentDeliveryInfo*)pDeliveryInfo;
				pFragDeliveryInfo->pMsgData = pFragBuf;
				pFragDeliveryInfo->nMsgLen = dwMsgFragLen;
				pFragDeliveryInfo->bWithMsgHead = bMsgHead;

				emProtPackageType = emProtFragment;
				break;
			}
			else
			{
				char* pRcvData = NULL;
				std::size_t dwRcvLen = 0;
				if (bMsgHead)
				{
					PROTOCOL_MSG_HEAD* pMsgHead = (PROTOCOL_MSG_HEAD*)pReq;
					if (DeliverPackPrepare(pFragHead, pMsgHead, pRcvData, dwRcvLen))
					{
						TFullPackageDeliveryInfo* pFullDDeliveryInfo = (TFullPackageDeliveryInfo*)pDeliveryInfo;
						pFullDDeliveryInfo->tUUID = pMsgHead->tUUID;
						pFullDDeliveryInfo->nFuncId = pFragHead->nFuncId;
						pFullDDeliveryInfo->nCltPackId = pFragHead->nCltPackId;
						pFullDDeliveryInfo->pProtHead = (PROTOCOL_HEAD*)pRcvData;

						emProtPackageType = emProtFullPackage;
					}
				}
				else
				{
					if (DeliverPackPrepare(pFragHead, NULL, pRcvData, dwRcvLen))
					{
						PROTOCOL_UUID tUUID = { 0 };

						TFullPackageDeliveryInfo* pFullDDeliveryInfo = (TFullPackageDeliveryInfo*)pDeliveryInfo;
						pFullDDeliveryInfo->tUUID = tUUID;
						pFullDDeliveryInfo->nFuncId = pFragHead->nFuncId;
						pFullDDeliveryInfo->nCltPackId = pFragHead->nCltPackId;
						pFullDDeliveryInfo->pProtHead = (PROTOCOL_HEAD*)pRcvData;

						emProtPackageType = emProtFullPackage;
					}
				}
			}
		} while (false);

		return emProtPackageType;
	}

	bool CProtStack::IsRcvMsgHead()
	{
		if (NULL != m_pProtCfg)
		{
			return (m_pProtCfg->GetRcvMsgHead() == 0 ? false : true);
		}

		return false;
	}

	bool CProtStack::BuildFragment(const PROTOCOL_HEAD* pProtocolHead, list<TFragmentMessage> &fragList, int nCltPackId)
	{
		assert(NULL != m_pBuildFrag);
		return m_pBuildFrag->BuildFragment(pProtocolHead, fragList, nCltPackId);
	}

	bool CProtStack::BuildFragment(const PROTOCOL_HEAD* pProtocolHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, list<TFragmentMessage> &fragList, int nCltPackId)
	{
		assert(NULL != m_pBuildFrag);
		return m_pBuildFrag->BuildFragment(pProtocolHead, tProtocolMsgHead, fragList, nCltPackId);
	}

	//发送用户数据前准备，压缩，加密，添加包头，通过参数返回切片结果
	bool CProtStack::SendPrepare(int nFuncId,
		PROTO_PACK_TYPE ePackType, char* pData, std::size_t dwDataLen,
		const PROTOCOL_UUID& tUUID, std::size_t nCltPackId, int nUserData1, int nUserData2,
		std::list<TFragmentMessage> &fragList)
	{
		//是否压缩
		bool bCompress = false;
		if (m_pProtCfg->GetCompressLimit() > 0 && dwDataLen >= m_pProtCfg->GetCompressLimit())
		{
			bCompress = true;
		}
		//是否加密
		bool bCipher = m_pProtCfg->GetCipher() == 0 ? false : true;
		//发送分片是否携带MsgHead包头
		bool bWithMsgHead = m_pProtCfg->GetSndMsgHead() == 0 ? false : true;

		bool bRet = true;
		do
		{
			char* pBuffer = pData;
			std::size_t dwBufLen = dwDataLen;
			zip_buffer zipbuf;
			if (bCompress)
			{
				dwDataLen = zipbuf.zip((zip_char_t*)pData, (zip_size_t)dwDataLen);
				if (dwDataLen < 0)
				{
					bRet = false;
					break;
				}
				pBuffer = (char*)zipbuf.buffer();
			}

			if (bCipher)
			{
				//todo
			}

			//开辟新内存，包含业务包头，业务数据
			size_t deSendLen = sizeof(PROTOCOL_HEAD)+dwDataLen;
			char* pProtocolBuffer = new char[deSendLen];
			if (pProtocolBuffer == NULL)
			{
				bRet = false;
				break;
			}
			memset(pProtocolBuffer, 0, sizeof(PROTOCOL_HEAD));
			PROTOCOL_HEAD* pSendBuf = (PROTOCOL_HEAD*)pProtocolBuffer;
			pSendBuf->chVersion = PROTOCOL_VERSION;
			pSendBuf->chCompress = bCompress;
			pSendBuf->chCipher = bCipher;
			pSendBuf->nFuncId = nFuncId;
			pSendBuf->chType = ePackType;
			pSendBuf->nSrcLen = (int)dwBufLen;
			pSendBuf->nBodyLen = (int)dwDataLen;
			pSendBuf->nUsrData1 = nUserData1;
			pSendBuf->nUsrData2 = nUserData2;
			memcpy(pSendBuf->chBodyData, pBuffer, dwDataLen);
			//设置包头数据
			if (bWithMsgHead)
			{
				PROTOCOL_MSG_HEAD tMsgHead = { 0 };
				tMsgHead.tUUID = tUUID;
				bRet = m_pBuildFrag->BuildFragment(pSendBuf, tMsgHead, fragList, (int)nCltPackId);
			}
			else
			{
				bRet = m_pBuildFrag->BuildFragment(pSendBuf, fragList, (int)nCltPackId);
			}
			if (NULL != pProtocolBuffer)
			{
				delete[] pProtocolBuffer;
				pProtocolBuffer = NULL;
			}
		} while (0);

		return bRet;
	}

	//向用户转交完整包前准备，解压，解密，组包
	bool CProtStack::DeliverPackPrepare(const PROTOCOL_FRAG_HEAD* pFragData, const PROTOCOL_MSG_HEAD* pMsgHead,
		char*& pRcvData, std::size_t& dwRcvLen)
	{
		bool bRet = true;
		do
		{
			TPackData tPackData;
			tPackData.pData = NULL;
			tPackData.dwDataLen = 0;
			bool bMsgHead = /*m_dwRcvMsgHead*/m_pProtCfg->GetRcvMsgHead() == 0 ? false : true;
			if (bMsgHead && pMsgHead)
				bRet = m_pBuildPack->Defrag(pFragData, *pMsgHead, tPackData);
			else
				bRet = m_pBuildPack->Defrag(pFragData, tPackData);
			if (false == bRet)
			{
				break;
			}
			const PROTOCOL_HEAD* pHead = (const PROTOCOL_HEAD*)tPackData.pData;
			//包是否密
			bool bCipher = pHead->chCipher == 0 ? false : true;
			//包是否压缩
			bool bCompress = pHead->chCompress == 0 ? false : true;
			//如果加密了
			if (bCipher)
			{
				//解密
			}
			//如果压缩了
			pRcvData = tPackData.pData;
			dwRcvLen = tPackData.dwDataLen;
			if (bCompress)
			{
				//解压缩
				//释放组包内存
				if (!DecodeFullPack(pHead, tPackData.dwDataLen, pRcvData, dwRcvLen))
				{
					bRet = false;
				}
				if (tPackData.pData != NULL)
				{
					delete[] tPackData.pData;
					tPackData.dwDataLen = 0;
				}
			}
		} while (false);
		return bRet;
	}

	/*
	没有加密，没有压缩，return false
	如果压缩，解压
	如果加密，解密
	*/
	bool CProtStack::DecodeFullPack(const PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen)
	{
		PROTOCOL_HEAD* pHead = (PROTOCOL_HEAD*)pPackData;
		char* pNewBuff = 0;
		int nNewLen = 0;

		//包长度不匹配
		if ((pHead->nBodyLen) + sizeof(PROTOCOL_HEAD) != dwDataLen)
		{
			return false;
		}

		nNewLen = (pHead->nSrcLen) + sizeof(PROTOCOL_HEAD);
		pNewBuff = new char[nNewLen];

		//申请内存失败
		if (!pNewBuff)
		{
			return false;
		}

		//是否需要解压
		if (!pHead->chCompress)
		{
			//直接拷贝整个数据包
			memcpy(pNewBuff, pPackData, nNewLen);
			return true;
		}
		else
		{
			char* pBodyBuff = pNewBuff + sizeof(PROTOCOL_HEAD);
			int nBodyLen = pHead->nBodyLen;

			zip_buffer zipBuf;
			int nzip = zipBuf.unzip((const zip_char_t*)pHead->chBodyData, nBodyLen);
			if (nzip < 0)
			{
				return false;
			}

			//解压之后实际长度与包头长度不一致
			if (nzip != pHead->nSrcLen)
			{
				delete[]pNewBuff;
				pNewBuff = NULL;
				return false;
			}

			//填充协议头
			PROTOCOL_HEAD* pHeadBuff = (PROTOCOL_HEAD*)pNewBuff;
			memset(pHeadBuff, 0, sizeof(PROTOCOL_HEAD));
			memcpy(pHeadBuff, pHead, sizeof(PROTOCOL_HEAD));
			pHeadBuff->chCompress = 0;
			pHeadBuff->nSrcLen = nzip;
			pHeadBuff->nBodyLen = nzip;
			//填充包体
			memcpy(pHeadBuff->chBodyData, zipBuf.buffer(), zipBuf.length());

		}
		pDecodeData = pNewBuff;
		DecodeLen = nNewLen;
		return true;
	}
}
