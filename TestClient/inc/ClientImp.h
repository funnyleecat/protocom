#pragma once
#include "IProtClient.h"
#include <vector>
#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>
using namespace protcomm;
typedef boost::mutex						ng_mutex_t;
typedef boost::condition_variable_any		ng_condition_t;
typedef boost::mutex::scoped_lock			ng_auto_lock;
class ClientCallBack;
class ClientImp
{
    struct TConnInfo
    {
        std::size_t dwCtx;
        int         nPackNum;
        TConnInfo() :dwCtx(0), nPackNum(0)
        {}
    };
public:
	ClientImp();
	~ClientImp();
    void Test();

    //∂‡¡¥Ω”≤‚ ‘
    void MultConnTest(); 
private:
    int GetPackNum();
private:
    IProtClient*        m_pClient;
    ClientCallBack*     m_pCB;
    std::vector<TConnInfo>  m_ConnVec;
};

