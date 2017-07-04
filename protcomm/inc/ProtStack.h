#pragma once

#include "Protocol.h"
#include "FuncIdTransMode.h"
#include "tcpcomm2_define.h"
#include "DataDelivery.h"
#include "LogHandler.h"
#include "ProtCfg.h"
#include "ProtocolFragment.h"
#include "ProtocolDefrag.h"

extern "C"
{
    //�ص������ͷ��ڴ�
    void RelaseFragBuf(char* pBuf, size_t dwLen, bool bRet);
}

namespace protcomm
{
	//Э�鴦������ݰ�����
	enum EmProtPackageType
	{
		emProtReqKeepalive, //���������
		emProtAckKeepalive, //������Ӧ��
		emProtFullPackage,  //����ҵ���
		emProtFragment,     //��Ƭҵ���
		emProtContinue      //��������(�����ȴ���Ƭ)
	};

	//��Ƭ���ݰ�����ʱ��Ҫ����Ϣ
	typedef struct tagFragmentDeliveryInfo
	{
		char* pMsgData;
		std::size_t nMsgLen;
		bool bWithMsgHead;
	}TFragmentDeliveryInfo;

	//����ҵ�����ݰ�����ʱ��Ҫ����Ϣ
	typedef struct tagFullPackageDeliveryInfo
	{
		PROTOCOL_UUID tUUID;
		int nFuncId;
		std::size_t nCltPackId;
		PROTOCOL_HEAD* pProtHead;
	}TFullPackageDeliveryInfo;

	class CProtStack
	{
	public:
		CProtStack();
		virtual ~CProtStack();

	public:
		EmProtPackageType HandleCommRequest(size_t nCtxId, const char* pReq, size_t nReqLen, char* pDeliveryInfo);

	public:
		size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData);
		size_t GetOpt(E_PROTOCOL_OPT eOpt) const;
		bool Start();
		bool IsRcvMsgHead();

		bool BuildFragment(const PROTOCOL_HEAD* pProtocolHead, list<TFragmentMessage> &fragList, int nCltPackId);
		bool BuildFragment(const PROTOCOL_HEAD* pProtocolHead, const PROTOCOL_MSG_HEAD &tProtocolMsgHead, list<TFragmentMessage> &fragList, int nCltPackId);

		//�����û�����ǰ׼����ѹ�������ܣ���Ӱ�ͷ��ͨ������������Ƭ���
		bool SendPrepare(int nFuncId,
			PROTO_PACK_TYPE ePackType, char* pData, std::size_t dwDataLen, 
			const PROTOCOL_UUID& tUUID, std::size_t nCltPackId, int nUserData1, int nUserData2,
			std::list<TFragmentMessage> &fragList);

		//���û�ת��������ǰ׼������ѹ�����ܣ����
		bool DeliverPackPrepare(const PROTOCOL_FRAG_HEAD* pFragData, const PROTOCOL_MSG_HEAD* pMsgHead,
			char*& pRcvData, std::size_t& dwRcvLen);

	private:
		//��ѹ���ݣ�����ͷ
		bool DecodeFullPack(const PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen);

	private:
		CLogHandler*          m_pLogHandler;   //��־
		CProtCfg*             m_pProtCfg;      //Э���������
		CProtocolFragment*    m_pBuildFrag;    //��Ƭ����
		CProtocolDefrag*      m_pBuildPack;    //�������
	};
}
