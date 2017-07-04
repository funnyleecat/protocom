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
#pragma once

#include <list>
#include <boost/atomic/atomic.hpp>
#include "Protocol.h"
#include "LogHandler.h"
using namespace std;

namespace protcomm
{
	enum EMFragMode
	{
		emClientFragMode = 1, //�ͻ������
		emServerFragMode,     //��������
	};

	typedef struct tagFragmentMessage
	{
		size_t nDataLen; //��Ϣ�峤��
		char* pData;     //��Ϣ��
	}TFragmentMessage;

	class CProtocolFragment : public ILogHandler
	{
	public:
		CProtocolFragment(CLogHandler*& pLogHandler);
		~CProtocolFragment();

	public:
		bool SetFragMode(EMFragMode emFragMode);
		bool SetFragSize(std::size_t dwFragSize);
		bool SetMaxFragNum(std::size_t dwMaxFragNum);
		bool BuildFragment(const PROTOCOL_HEAD* pProtocolHead, list<TFragmentMessage> &fragList, int nCltPackId);
		bool BuildFragment(const PROTOCOL_HEAD* pProtocolHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, list<TFragmentMessage> &fragList, int nCltPackId);

	private:
		int GetPacketId();
		bool CreateFragment(PROTOCOL_FRAG_HEAD &tProtocolFragHead, char* pData, int nDataLen, list<TFragmentMessage> &fragList, const PROTOCOL_MSG_HEAD* pProtocolMsgHead = NULL);
		char* GetBuffer(std::size_t dwBufSize);

	private:
		EMFragMode m_emFragMode;
		std::size_t m_dwFragSize;
		std::size_t m_dwMaxFragNum;
		boost::atomic_int m_nPackId;
	};
}
