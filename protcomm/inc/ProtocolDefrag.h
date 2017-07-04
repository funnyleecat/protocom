/***********************************************************************
ģ����		: hqrtsrv
�ļ���		: ProtocolDefrag.h
����			: ouyi
��������		: 2016/12/20
��ע			: Э�����
-----------------------------------------------------------------------
�޸ļ�¼ :
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y  <���߻��޸�����>       <�޸�����>
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
		emClientDefragMode = 1, //�ͻ������
		emServerDefragMode      //��������
	};

	//���������ݰ�
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
		//�������ģʽ
		bool SetDefragMode(EMDefragMode emDefragMode, size_t dwFragHashSize, size_t dwFragTimeout);
		//�ͻ��˴���ְ�
		bool Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, TPackData &tPackData);
		//����˴���ְ�
		bool Defrag(const PROTOCOL_FRAG_HEAD* pProtocolFragHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, TPackData &tPackData);

	private:
		EMDefragMode m_emDefragMode;
		CPackSession<CCltPackKey>* m_pCltPackSession;
		CPackSession<CSrvPackKey>* m_pSrvPackSession;
	};
}
