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
#pragma once

#include "Protocol.h"
#include "LogHandler.h"
#include <cstddef>
using namespace std;

namespace protcomm
{
	class CCltPackKey;
	class CSrvPackKey;
	template<typename T> class CPackSession;

	enum EMDefragMode
	{
		emClientDefragMode = 1, //客户端组包
		emServerDefragMode      //服务端组包
	};

	//完整的数据包
	typedef struct tagTPackData
	{
		char* pData;
		size_t dwDataLen;
	}TPackData;

	class CProtocolDefrag : public ILogHandler
	{
	public:
		CProtocolDefrag(CLogHandler*& pLogHandler);
		~CProtocolDefrag();

	public:
		//设置组包模式
		bool SetDefragMode(EMDefragMode emDefragMode, size_t dwFragHashSize, size_t dwFragTimeout);
		//客户端处理分包
		bool Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, TPackData &tPackData);
		//服务端处理分包
		bool Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, TPackData &tPackData);

	private:
		EMDefragMode m_emDefragMode;
		CPackSession<CCltPackKey>* m_pCltPackSession;
		CPackSession<CSrvPackKey>* m_pSrvPackSession;
	};
}
