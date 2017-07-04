#pragma once
#include "Protocol.h"
#include <list>
#include <boost/atomic/atomic.hpp>

namespace SynTcpCli
{

	using namespace std;

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

	class CProtocolFragment
	{
	public:
		CProtocolFragment();
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
