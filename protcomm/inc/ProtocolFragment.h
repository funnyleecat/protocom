/***********************************************************************
模块名		: hqrtsrv
文件名		: ProtocolFragment.h
作者			: ouyi
创建日期		: 2016/12/20
备注			: 协议分包
-----------------------------------------------------------------------
修改记录 :
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y  <作者或修改者名>       <修改内容>
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
		emClientFragMode = 1, //客户端组包
		emServerFragMode,     //服务端组包
	};

	typedef struct tagFragmentMessage
	{
		size_t nDataLen; //消息体长度
		char* pData;     //消息体
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
