
#include "stdafx.h"
#include "ProtCfg.h"

namespace protcomm
{
	CProtCfg::CProtCfg()
	{
		m_dwCipher = 0;
		m_dwCompressLimit = 48 * 1024; //默认大于48k的数据都压缩
		m_dwRcvMsgHead = 0;
		m_dwSndMsgHead = 0;
		m_dwFragHashSize = 0;
		m_dwFragTimeout = 0;
	}

	CProtCfg::~CProtCfg()
	{
	}

	void CProtCfg::SetCipher(size_t dwCipher)
	{
		m_dwCipher = dwCipher;
	}

	size_t CProtCfg::GetCipher() const
	{
		return m_dwCipher;
	}

	void CProtCfg::SetCompressLimit(size_t dwCompressLimit)
	{
		m_dwCompressLimit = dwCompressLimit;
	}

	size_t CProtCfg::GetCompressLimit() const
	{
		return m_dwCompressLimit;
	}

	void CProtCfg::SetRcvMsgHead(size_t dwRcvMsgHead)
	{
		m_dwRcvMsgHead = dwRcvMsgHead;
	}

	size_t CProtCfg::GetRcvMsgHead() const
	{
		return m_dwRcvMsgHead;
	}

	void CProtCfg::SetSndMsgHead(size_t dwSndMsgHead)
	{
		m_dwSndMsgHead = dwSndMsgHead;
	}

	size_t CProtCfg::GetSndMsgHead() const
	{
		return m_dwSndMsgHead;
	}

	void CProtCfg::SetFragHashSize(size_t dwFragHashSize)
	{
		m_dwFragHashSize = dwFragHashSize;
	}

	size_t CProtCfg::GetFragHashSize() const
	{
		return m_dwFragHashSize;
	}

	void CProtCfg::SetFragTimeout(size_t dwFragTimeout)
	{
		m_dwFragTimeout = dwFragTimeout;
	}

	size_t CProtCfg::GetFragTimeout() const
	{
		return m_dwFragTimeout;
	}
}
