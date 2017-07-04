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
		CFuncTransMode m_pTransMode;       //�շ�ģʽ����

	private:
		size_t         m_dwCipher;         //���������Ƿ���ܣ�dwDataΪ0��ʾ�����ܣ������ʾ���ܣ�Ĭ�ϼ���
		size_t         m_dwCompressLimit;  //���������Ƿ�ѹ����dwDataΪ0��ʾ��ѹ���������ʾѹ����Ĭ�ϴ����ض�ֵѹ��
		size_t         m_dwRcvMsgHead;     //���ܵ������Ƿ�Я��PROTOCOL_MSG_HEAD��ͷ
		size_t         m_dwSndMsgHead;     //���͵������Ƿ�Я��PROTOCOL_MSG_HEAD��ͷ
		size_t         m_dwFragHashSize;   //��Ƭ�����Ӧ��hash���С
		size_t         m_dwFragTimeout;    //��Ƭ��ʱʱ��(��λ��s)
	};
}
