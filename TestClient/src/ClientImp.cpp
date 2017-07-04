
#include "EasyTimer.hpp"
#include "ClientImp.h"
#include "DataDelivery.h"
#include <string>
#include <windows.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <boost/timer.hpp>
#include <assert.h>
#include "ClientCfg.h"

std::string g_str = "uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123==]]uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123ufgfhguidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uufdghidxly123uuidxly123uuidxly123fsdfuuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuweridxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123ut24t5uidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uui@!2312dxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123==]]uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123ufgfhguidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uufdghidxly123uuidxly123uuidxly123fsdfuuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uufdghidxly123uuidxly123uuidxly123fsdfuuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuweridxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123ut24t5uidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uui@!2312dxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
	uuidxly123uuidxly123uuidxly123uuidxly123uuidxly123uu756idxly123uuidxly123uuidxly123uuidxly123uuidxly123uu\
";
boost::atomic_int nRcvCount(0);
boost::atomic_int nSndCount(0);
boost::timer g_SpendTime;
#include <boost/date_time.hpp>

typedef boost::posix_time::microsec_clock	ng_clock_t;
typedef boost::posix_time::ptime			ng_time_t;
typedef boost::posix_time::time_duration	ng_timespan_t;
ng_time_t g_RunTimeStart;
ng_time_t g_RunTimeEnd;
std::string g_strCopy;
ng_mutex_t	g_mutex;
ng_condition_t	g_cond;
class ClientCallBack :public CDataDelivery
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
		std::string strTemp(pReq, nReqLen);
		if (strTemp == g_strCopy)
        {
            ++nRcvCount;
			ClientInfo &Info = ClientCfg::GetInstance().m_client;
			int nTotal = Info.CirculNum * Info.vFuncId.size();
			if (nRcvCount == nTotal)
			{
				extern void ShowInfo();
				ShowInfo();
				g_RunTimeEnd = ng_clock_t::local_time();
				//cout << "Pack SpendTime seconds=" << g_SpendTime.elapsed() << endl;
				g_SpendTime.restart();
				nRcvCount = 0;
				nSndCount = 0;
				g_cond.notify_one();
			}
		}
		else
		{
			assert(false);
			std::cout << "Compare Different" << "strTemp len=" << strTemp << " " << nReqLen << std::endl;
            assert("回复的数据与发送的数据不一致" == 0);
		}
    };
    //交付业务分片数据，其中pMsgData必须调用ReleaseProtoBuff释放
    virtual void DeliverFragment(std::size_t nCtxId,
        const char* pMsgData,
        std::size_t nMsgLen,
        bool bWithMsgHead
        )
    {
        printf("client rcved fragment\n");
    };
    //建立连接的通知
    virtual void OnConnected(std::size_t nCtxId){};
    //断开连接的通知
    virtual void OnDisconnect(std::size_t nCtxId)
	{
		printf("client OnDisconnect\n");
	};
};

class TestClientLog :public CLogOutput
{
public:
    virtual void Debug(const char* pszString){};
    virtual void Info(const char* pszString) {};
    virtual void Warn(const char* pszString) {};
    virtual void Error(const char* pszString){};
    virtual void Fatal(const char* pszString){};
};


ClientImp::ClientImp()
{
}


ClientImp::~ClientImp()
{
    if (m_pCB)
    {
        delete m_pCB;
    }
}

TimerService ts;
void ShowInfo()
{
//printf("已发送整包:%5d,已回复整包:%5d\n", nSndCount, nRcvCount);
// if (nRcvCount == nSndCount)
// {
//std::cout << "elapsed: " << g_SpendTime.elapsed() << std::endl;
// //ts.RemoveAllTimer();
// }

	printf("已成功发送整包:%5d,已回复整包:%5d\n",nSndCount, nRcvCount);

}
void ClientImp::MultConnTest()
{
    ts.AddTimer(boost::posix_time::seconds(3),boost::bind(&ShowInfo));
    ClientInfo &Info = ClientCfg::GetInstance().m_client;
    IProtClient* pClient = IProtClient::GetInstance(Info.io_threads, Info.work_threads);
    m_pCB = new ClientCallBack;
    TestClientLog* pLog = new TestClientLog;
    pClient->SetOpt(OPT_DATA_DELIVERY, (std::size_t)m_pCB);
	pClient->SetOpt(OPT_PROTOCOL_LOGGER_HADNLER, (std::size_t)pLog);
    //pClient->SetOpt(OPT_COMPRESS_LIMIT, 500);
    pClient->Start();
	string strCopy(g_str.c_str(), ClientCfg::GetInstance().nStringNum);
	g_strCopy = strCopy;
	g_strCopy += g_strCopy;
	g_strCopy += g_strCopy;
	g_strCopy += g_strCopy;
	g_strCopy += g_strCopy;
	g_strCopy += g_strCopy;
	g_strCopy += g_strCopy;
	std::cout << "send Len = " << strCopy.length() << std::endl;
    PROTOCOL_UUID uuid ={ 0 };
    std::string strUuid = "uuidtttt";
    memcpy(uuid.tUUID, strUuid.data(), strUuid.length());

    m_ConnVec.resize(1);
    int nFuncIdx = 0;
    std::size_t dwIndex = 0;

    while (true)
    {
        if (dwIndex >= m_ConnVec.size())
        {
            dwIndex = 0;
        }
        if (0 == m_ConnVec[dwIndex].dwCtx)
        {
            m_ConnVec[dwIndex].dwCtx = pClient->Connect(Info.ip.c_str(), Info.port);
        }
        if (0 == m_ConnVec[dwIndex].nPackNum)
        {
            m_ConnVec[dwIndex].nPackNum = GetPackNum();
        }
        m_ConnVec[dwIndex].nPackNum--;
		size_t t1 = m_ConnVec[dwIndex].dwCtx;
		size_t t2 = Info.vFuncId[dwIndex%Info.vFuncId.size()].Min;
		if (pClient->SendPackageData(m_ConnVec[dwIndex].dwCtx, Info.vFuncId[dwIndex%Info.vFuncId.size()].Min, PROT_REQ_BUSINESS,
			(char*)strCopy.c_str(), strCopy.length(), uuid, 1, 111, 222))
        {
            ++nSndCount;
        }
        if (nSndCount>=Info.CirculNum)
        {
			ng_auto_lock lock(g_mutex);
			g_cond.wait(lock);
			static int nCirCulNum = 1;
			printf("已循环次数:%5d\n", nCirCulNum);
			nCirCulNum++;
        }
        ++dwIndex;
    }
}

int ClientImp::GetPackNum()
{
    srand((int)time(0));
    return (rand() / 1000) + 1;//每个客户端发送1到150个包
}

//发送切片压缩，整包不压缩
void ClientImp::Test()
{
	ClientInfo &Info = ClientCfg::GetInstance().m_client;
    m_pClient = IProtClient::GetInstance(Info.io_threads, Info.work_threads);
    m_pCB = new ClientCallBack;
    TestClientLog* pLog = new TestClientLog;
    m_pClient->SetOpt(OPT_DATA_DELIVERY, (std::size_t)m_pCB);
	m_pClient->SetOpt(OPT_PROTOCOL_LOGGER_HADNLER, (std::size_t)pLog);
    //m_pClient->SetOpt(OPT_COMPRESS_LIMIT, 500);
    m_pClient->Start();

	g_RunTimeStart = ng_clock_t::local_time();

	size_t dwCtx = m_pClient->Connect(Info.ip.c_str(),Info.port );
	std::cout << "send Len = " << g_str.length() << std::endl;
	PROTOCOL_UUID uuid = { 0 };
    std::string strUuid = "uuid111";
    memcpy(uuid.tUUID, strUuid.data(),strUuid.length());
	//分片发送
	int n = Info.CirculNum;
	while (n--)
	{
		for (size_t i = 0; i < Info.vFuncId.size(); i++)
		//for (size_t i = 0; i < 1; i++)
		{
			bool bRet = m_pClient->SendPackageData(dwCtx, Info.vFuncId[i].Min, PROT_REQ_BUSINESS,
				(char*)g_str.c_str(), g_str.length(), uuid, 1, 111, 222);
		}
	}

}