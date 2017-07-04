
#include "stdafx.h"
#include "ProtStack.h"
#include "zip_buffer.h"

extern "C"
{
	//�ص������ͷ��ڴ�
	void RelaseFragBuf(char* pBuf, size_t dwLen, bool bRet)
	{
		delete[] pBuf;
	}
}

namespace protcomm
{
	//�ͷ���Protocomm���ٵ��ڴ�
	void ReleaseProtoBuff(void* pProtoBuff)
	{
		//������飬�����˾͸ù�
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

	//���ò���
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
			assert(dwData > 0 && ((dwData & (dwData - 1)) == 0)); //Ҫ��2^n
			m_pProtCfg->SetFragHashSize(dwData);
			break;
		case OPT_FRAG_TIMEOUT:
			assert(dwData > 0);
			m_pProtCfg->SetFragTimeout(dwData);
			break;
		default:
			assert(0 == "������˼��ƫ��");
			break;
		}

		return 0;
	}

	//��ò�������
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
			assert(0 == "������˼��ƫ��");
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

		//���÷�Ƭģʽ
		m_pBuildFrag->SetFragMode(m_pProtCfg->GetSndMsgHead() == 0 ? emClientFragMode : emServerFragMode);

		//�������ģʽ
		size_t dwFragHashSize = m_pProtCfg->GetFragHashSize();
		size_t dwFragTiemout = m_pProtCfg->GetFragTimeout();
		EMDefragMode emDefragMode = (m_pProtCfg->GetRcvMsgHead() == 0 ? emClientDefragMode : emServerDefragMode);
		//δ����hash��С�������ģʽĬ��Ϊ512,�ͻ���ģʽĬ��Ϊ128
		if (0 == dwFragHashSize)
		{
			dwFragHashSize = (emServerDefragMode == emDefragMode ? 512 : 128);
		}
		//δ���÷�Ƭ��ʱ����ΪĬ�ϵ�60s
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

		//��������ҵ���
		if (pFragHead->nOffset == 0 && pProtHead->chType == PROT_REQ_KEEPALIVE) 
		{
			return emProtReqKeepalive;
		}
		//��������ҵ���
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
					dwMsgFragLen = nReqLen + sizeof(PROTOCOL_MSG_HEAD); //������յĲ���msghead����Ԥ��msghead�ڴ�
				}
				char* pFragBuf = new char[dwMsgFragLen];
				char* pDataPos = pFragBuf;
				if (false == bMsgHead)
				{
					pDataPos += sizeof(PROTOCOL_MSG_HEAD); //����������ݲ���msghead������������Ҫƫ�Ƶ�Ԥ�����ڴ�
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

	//�����û�����ǰ׼����ѹ�������ܣ���Ӱ�ͷ��ͨ������������Ƭ���
	bool CProtStack::SendPrepare(int nFuncId,
		PROTO_PACK_TYPE ePackType, char* pData, std::size_t dwDataLen,
		const PROTOCOL_UUID& tUUID, std::size_t nCltPackId, int nUserData1, int nUserData2,
		std::list<TFragmentMessage> &fragList)
	{
		//�Ƿ�ѹ��
		bool bCompress = false;
		if (m_pProtCfg->GetCompressLimit() > 0 && dwDataLen >= m_pProtCfg->GetCompressLimit())
		{
			bCompress = true;
		}
		//�Ƿ����
		bool bCipher = m_pProtCfg->GetCipher() == 0 ? false : true;
		//���ͷ�Ƭ�Ƿ�Я��MsgHead��ͷ
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

			//�������ڴ棬����ҵ���ͷ��ҵ������
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
			//���ð�ͷ����
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

	//���û�ת��������ǰ׼������ѹ�����ܣ����
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
			//���Ƿ���
			bool bCipher = pHead->chCipher == 0 ? false : true;
			//���Ƿ�ѹ��
			bool bCompress = pHead->chCompress == 0 ? false : true;
			//���������
			if (bCipher)
			{
				//����
			}
			//���ѹ����
			pRcvData = tPackData.pData;
			dwRcvLen = tPackData.dwDataLen;
			if (bCompress)
			{
				//��ѹ��
				//�ͷ�����ڴ�
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
	û�м��ܣ�û��ѹ����return false
	���ѹ������ѹ
	������ܣ�����
	*/
	bool CProtStack::DecodeFullPack(const PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen)
	{
		PROTOCOL_HEAD* pHead = (PROTOCOL_HEAD*)pPackData;
		char* pNewBuff = 0;
		int nNewLen = 0;

		//�����Ȳ�ƥ��
		if ((pHead->nBodyLen) + sizeof(PROTOCOL_HEAD) != dwDataLen)
		{
			return false;
		}

		nNewLen = (pHead->nSrcLen) + sizeof(PROTOCOL_HEAD);
		pNewBuff = new char[nNewLen];

		//�����ڴ�ʧ��
		if (!pNewBuff)
		{
			return false;
		}

		//�Ƿ���Ҫ��ѹ
		if (!pHead->chCompress)
		{
			//ֱ�ӿ����������ݰ�
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

			//��ѹ֮��ʵ�ʳ������ͷ���Ȳ�һ��
			if (nzip != pHead->nSrcLen)
			{
				delete[]pNewBuff;
				pNewBuff = NULL;
				return false;
			}

			//���Э��ͷ
			PROTOCOL_HEAD* pHeadBuff = (PROTOCOL_HEAD*)pNewBuff;
			memset(pHeadBuff, 0, sizeof(PROTOCOL_HEAD));
			memcpy(pHeadBuff, pHead, sizeof(PROTOCOL_HEAD));
			pHeadBuff->chCompress = 0;
			pHeadBuff->nSrcLen = nzip;
			pHeadBuff->nBodyLen = nzip;
			//������
			memcpy(pHeadBuff->chBodyData, zipBuf.buffer(), zipBuf.length());

		}
		pDecodeData = pNewBuff;
		DecodeLen = nNewLen;
		return true;
	}
}
