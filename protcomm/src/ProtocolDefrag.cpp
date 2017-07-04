/***********************************************************************
模块名		: hqrtsrv
文件名		: ProtocolDefrag.h
作者			: ouyi
创建日期		: 2016/12/20
备注			: 协议组包
-----------------------------------------------------------------------
修改记录 :
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y  <作者或修改者名>       <修改内容>
**************************************************************************/
#include "stdafx.h"
#include "ProtocolDefrag.h"
#include "FragmentCompose.hpp"

namespace protcomm
{

	CProtocolDefrag::CProtocolDefrag(CLogHandler*& pLogHandler) :
		ILogHandler(pLogHandler),
		m_emDefragMode(emServerDefragMode),
		m_pCltPackSession(NULL),
		m_pSrvPackSession(NULL)
	{
	}

	CProtocolDefrag::~CProtocolDefrag()
	{
		if (NULL != m_pCltPackSession )
		{
			delete m_pCltPackSession;
			m_pCltPackSession = NULL;
		}

		if (NULL != m_pSrvPackSession)
		{
			delete m_pSrvPackSession;
			m_pSrvPackSession = NULL;
		}
	}

	bool CProtocolDefrag::SetDefragMode(EMDefragMode emDefragMode, size_t dwFragHashSize, size_t dwFragTimeout)
	{
		m_emDefragMode = emDefragMode;
		if (m_emDefragMode == emClientDefragMode)
		{
			m_pCltPackSession = new CPackSession<CCltPackKey>(m_pLogHandler, dwFragHashSize, dwFragTimeout);
			assert(NULL != m_pCltPackSession);
		}
		else if (m_emDefragMode == emServerDefragMode)
		{
			m_pSrvPackSession = new CPackSession<CSrvPackKey>(m_pLogHandler, dwFragHashSize, dwFragTimeout);
			assert(NULL != m_pSrvPackSession);
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CProtocolDefrag::Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, TPackData &tPackData)
	{
		assert(NULL != m_pCltPackSession);

		CCltPackKey tCltPackKey(pProtocolFragHead->nCltPackId, pProtocolFragHead->nSrvPackId);
		return m_pCltPackSession->ComposeFrag(tCltPackKey, pProtocolFragHead, tPackData);
	}

	bool CProtocolDefrag::Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, TPackData &tPackData)
	{
		assert(NULL != m_pSrvPackSession);

		CSrvPackKey tSrvPackKey(tProtocolMsgHead.tUUID, pProtocolFragHead->nCltPackId);
		return m_pSrvPackSession->ComposeFrag(tSrvPackKey, pProtocolFragHead, tPackData);
	}

}
