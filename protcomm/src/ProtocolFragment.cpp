/***********************************************************************
ģ����		: hqrtsrv
�ļ���		: ProtocolFragment.h
����			: ouyi
��������		: 2016/12/20
��ע			: Э��ְ�
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
**************************************************************************/
#include "stdafx.h"
#include "ProtocolFragment.h"

namespace protcomm
{
	CProtocolFragment::CProtocolFragment(CLogHandler*& pLogHandler) :
		ILogHandler(pLogHandler),
		m_emFragMode(emServerFragMode),
		m_dwFragSize(0),
		m_dwMaxFragNum(0),
		m_nPackId(1)
	{
	}

	CProtocolFragment::~CProtocolFragment()
	{

	}

	bool CProtocolFragment::SetFragMode(EMFragMode emFragMode)
	{
		m_emFragMode = emFragMode;

		return true;
	}

	bool CProtocolFragment::SetFragSize(std::size_t dwFragSize)
	{
		m_dwFragSize = dwFragSize;

		return true;
	}

	bool CProtocolFragment::SetMaxFragNum(std::size_t dwMaxFragNum)
	{
		m_dwMaxFragNum = dwMaxFragNum;

		return true;
	}

	bool CProtocolFragment::BuildFragment(const PROTOCOL_HEAD* pProtocolHead, list<TFragmentMessage> &fragList, int nCltPackId)
	{
		assert(0 != m_dwFragSize && 0 != m_dwMaxFragNum);

		int nBufferLen = 0;
		char* pBuffer = NULL;

		int nDataLen = sizeof(PROTOCOL_HEAD) + pProtocolHead->nBodyLen;
		int nPackId = GetPacketId();

		//���÷ְ���ͷ����
		PROTOCOL_FRAG_HEAD tProtocolFragHead = { 0 };
		tProtocolFragHead.nFuncId = pProtocolHead->nFuncId;
		tProtocolFragHead.nPackSize = nDataLen;
		tProtocolFragHead.nCltPackId = (m_emFragMode == emClientFragMode ? nPackId : nCltPackId);
		tProtocolFragHead.nSrvPackId = (m_emFragMode == emClientFragMode ? 0 : nPackId);

		return CreateFragment(tProtocolFragHead, (char*)pProtocolHead, nDataLen, fragList);
	}

	bool CProtocolFragment::BuildFragment(const PROTOCOL_HEAD* pProtocolHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, list<TFragmentMessage> &fragList, int nCltPackId)
	{
		assert(0 != m_dwFragSize && 0 != m_dwMaxFragNum);

		int nBufferLen = 0;
		char* pBuffer = NULL;

		int nDataLen = sizeof(PROTOCOL_HEAD) + pProtocolHead->nBodyLen;
		int nPackId = GetPacketId();

		//���÷ְ���ͷ����
		PROTOCOL_FRAG_HEAD tProtocolFragHead = { 0 };
		tProtocolFragHead.nFuncId = pProtocolHead->nFuncId;
		tProtocolFragHead.nPackSize = nDataLen;
		tProtocolFragHead.nCltPackId = (m_emFragMode == emServerFragMode ? nCltPackId : nPackId);
		tProtocolFragHead.nSrvPackId = (m_emFragMode == emServerFragMode ? nPackId : 0);

		return CreateFragment(tProtocolFragHead, (char*)pProtocolHead, nDataLen, fragList, &tProtocolMsgHead);
	}

	int CProtocolFragment::GetPacketId()
	{
		return m_nPackId++;
	}

	bool CProtocolFragment::CreateFragment(PROTOCOL_FRAG_HEAD &tProtocolFragHead, char* pData, int nDataLen, list<TFragmentMessage> &fragList, const PROTOCOL_MSG_HEAD* pProtocolMsgHead /*= NULL*/)
	{
		int nMsgHeadLen = 0;
		if (NULL != pProtocolMsgHead)
		{
			nMsgHeadLen = sizeof(PROTOCOL_MSG_HEAD);
		}

		int nCurProcessDataLen = 0;
		int nMaxBufferLen = (int)(m_dwFragSize * m_dwMaxFragNum);
		char* pSrcCurPos = pData;
		char* pDstCurPos = NULL;
		do
		{
			nCurProcessDataLen = nDataLen;
			if (nCurProcessDataLen > nMaxBufferLen)
			{
				nCurProcessDataLen = nMaxBufferLen;
			}
			nDataLen -= nCurProcessDataLen;

			int nFragNum = (nCurProcessDataLen + (int)(m_dwFragSize) - 1) / (int)(m_dwFragSize);
			int nBufferLen = nFragNum * (nMsgHeadLen + sizeof(PROTOCOL_FRAG_HEAD)) + nCurProcessDataLen;
			char* pBuffer = GetBuffer(nBufferLen);
			pDstCurPos = pBuffer;

			for (int i = 0; i < nFragNum; i++)
			{
				if (NULL != pProtocolMsgHead)
				{
					memcpy(pDstCurPos, pProtocolMsgHead, nMsgHeadLen); //���msgx��ͷ
					pDstCurPos += nMsgHeadLen;
				}

				//���·ְ��İ�ͷ����
				tProtocolFragHead.nSegSize = nCurProcessDataLen > (int)(m_dwFragSize) ? (int)(m_dwFragSize) : nCurProcessDataLen;
				tProtocolFragHead.nOffset = (int)(pSrcCurPos - pData);
				memcpy(pDstCurPos, &tProtocolFragHead, sizeof(PROTOCOL_FRAG_HEAD)); //���ְ���ͷ
				pDstCurPos += sizeof(PROTOCOL_FRAG_HEAD);

				int nCopyLen = nCurProcessDataLen;
				if (nCurProcessDataLen > (int)(m_dwFragSize))
				{
					nCopyLen = (int)(m_dwFragSize);
				}
				memcpy(pDstCurPos, pSrcCurPos, nCopyLen); //���ְ���������
				pDstCurPos += nCopyLen;
				pSrcCurPos += nCopyLen;
				nCurProcessDataLen -= nCopyLen;
			}

			TFragmentMessage tFragmentMessage = { 0 };
			tFragmentMessage.nDataLen = nBufferLen;
			tFragmentMessage.pData = pBuffer;
			fragList.push_back(tFragmentMessage);
		} while (nDataLen > 0);

		return true;
	}

	char* CProtocolFragment::GetBuffer(std::size_t dwBufSize)
	{
		return new char[dwBufSize];
	}
}
