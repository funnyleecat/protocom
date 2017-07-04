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
    //回调函数释放内存
    void RelaseFragBuf(char* pBuf, size_t dwLen, bool bRet);
}

namespace protcomm
{
	//协议处理的数据包类型
	enum EmProtPackageType
	{
		emProtReqKeepalive, //心跳请求包
		emProtAckKeepalive, //心跳响应包
		emProtFullPackage,  //完整业务包
		emProtFragment,     //分片业务包
		emProtContinue      //继续处理(继续等待分片)
	};

	//分片数据包交付时需要的信息
	typedef struct tagFragmentDeliveryInfo
	{
		char* pMsgData;
		std::size_t nMsgLen;
		bool bWithMsgHead;
	}TFragmentDeliveryInfo;

	//完整业务数据包交付时需要的信息
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

		//发送用户数据前准备，压缩，加密，添加包头，通过参数返回切片结果
		bool SendPrepare(int nFuncId,
			PROTO_PACK_TYPE ePackType, char* pData, std::size_t dwDataLen, 
			const PROTOCOL_UUID& tUUID, std::size_t nCltPackId, int nUserData1, int nUserData2,
			std::list<TFragmentMessage> &fragList);

		//向用户转交完整包前准备，解压，解密，组包
		bool DeliverPackPrepare(const PROTOCOL_FRAG_HEAD* pFragData, const PROTOCOL_MSG_HEAD* pMsgHead,
			char*& pRcvData, std::size_t& dwRcvLen);

	private:
		//解压数据，带包头
		bool DecodeFullPack(const PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen);

	private:
		CLogHandler*          m_pLogHandler;   //日志
		CProtCfg*             m_pProtCfg;      //协议相关配置
		CProtocolFragment*    m_pBuildFrag;    //切片对象
		CProtocolDefrag*      m_pBuildPack;    //组包对象
	};
}
