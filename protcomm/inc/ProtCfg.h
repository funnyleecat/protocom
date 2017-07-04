#pragma once

#include "FuncIdTransMode.h"

namespace protcomm
{
	class CProtCfg
	{
	public:
		CProtCfg();
		~CProtCfg();

	public:
		void SetCipher(size_t dwCipher);
		size_t GetCipher() const;

		void SetCompressLimit(size_t dwCompressLimit);
		size_t GetCompressLimit() const;

		void SetRcvMsgHead(size_t dwRcvMsgHead);
		size_t GetRcvMsgHead() const;

		void SetSndMsgHead(size_t dwSndMsgHead);
		size_t GetSndMsgHead() const;

		void SetFragHashSize(size_t dwFragHashSize);
		size_t GetFragHashSize() const;

		void SetFragTimeout(size_t dwFragTimeout);
		size_t GetFragTimeout() const;

	public:
		CFuncTransMode m_pTransMode;       //收发模式设置

	private:
		size_t         m_dwCipher;         //发送数据是否加密，dwData为0表示不加密，否则表示加密，默认加密
		size_t         m_dwCompressLimit;  //发送数据是否压缩，dwData为0表示不压缩，否则表示压缩，默认大于特定值压缩
		size_t         m_dwRcvMsgHead;     //接受的数据是否携带PROTOCOL_MSG_HEAD包头
		size_t         m_dwSndMsgHead;     //发送的数据是否携带PROTOCOL_MSG_HEAD包头
		size_t         m_dwFragHashSize;   //分片组包对应的hash表大小
		size_t         m_dwFragTimeout;    //分片超时时间(单位：s)
	};
}
