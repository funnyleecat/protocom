#include "ServerImp.h"
#include "stdio.h"
class SrvCallBack :public CDataDelivery
{
public:
    //交付完整的业务包
    virtual void DeliverPackage(std::size_t nCtxId,
        int nFuncId,
        PROTO_PACK_TYPE ePackType,
        const char* pReq,
        std::size_t nReqLen,
        const PROTOCOL_UUID& tUUID,
        std::size_t nCltPackId,
        int nUserData1,
        int nUserData2)
    {
        printf("Full pack\n");


    };
    //交付业务分片数据，其中pMsgData必须调用ReleaseProtoBuff释放
    virtual void DeliverFragment(std::size_t nCtxId,
        const char* pMsgData,
        std::size_t nMsgLen,
        bool bWithMsgHead
        )
    {
        printf("Fragment pack\n");
    };
    //建立连接的通知
    virtual void OnConnected(std::size_t nCtxId){};
    //断开连接的通知
    virtual void OnDisconnect(std::size_t nCtxId){};
};

class TestServerLog :public CLogOutput
{
public:
    virtual void Debug(const char* pszString){};
    virtual void Info(const char* pszString) {};
    virtual void Warn(const char* pszString) {};
    virtual void Error(const char* pszString){};
    virtual void Fatal(const char* pszString){};
};

CServerImp::CServerImp()
{
}


CServerImp::~CServerImp()
{
}
//接收整包
//接收分片，设置分片大小
void CServerImp::Test()
{
	m_pSever = IProtServer::GetInstance(10, 10, 65535);
	m_pCB = new SrvCallBack;
	TestServerLog* pLog = new TestServerLog;
	m_pSever->SetOpt(OPT_DATA_DELIVERY, (std::size_t)m_pCB);
	m_pSever->SetOpt(OPT_LOGGER_HADNLER, (std::size_t)pLog);
	m_pSever->SetOpt(OPT_COMPRESS_LIMIT, 1024);
	m_pSever->SetOpt(OPT_RCV_FULLPACK, 123);
	m_pSever->SetOpt(OPT_RCV_FRAGMENT, 10);
	m_pSever->SetOpt(OPT_SOCK_RCVBUF, 200);
	bool bRet = m_pSever->Start(10010);
}

//接收分片，设置分片大小,带MsgHead
void CServerImp::TestMsgHead()
{
	m_pSever = IProtServer::GetInstance(10, 10, 65535);
	m_pCB = new SrvCallBack;
	TestServerLog* pLog = new TestServerLog;
	m_pSever->SetOpt(OPT_DATA_DELIVERY, (std::size_t)m_pCB);
	m_pSever->SetOpt(OPT_LOGGER_HADNLER, (std::size_t)pLog);
	m_pSever->SetOpt(OPT_COMPRESS_LIMIT, 1024);
	m_pSever->SetOpt(OPT_RCV_MSGHEAD, 1);
	m_pSever->SetOpt(OPT_RCV_FULLPACK, 123);
	m_pSever->SetOpt(OPT_RCV_FRAGMENT, 10);
	m_pSever->SetOpt(OPT_SOCK_RCVBUF, 200);
	bool bRet = m_pSever->Start(10010);
}