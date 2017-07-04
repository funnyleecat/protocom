#pragma once
#include "IProtServer.h"
using namespace protcomm;
class SrvCallBack;
class CServerImp
{
public:
	CServerImp();
	~CServerImp();
	void Test();
	void TestMsgHead();
private:
    SrvCallBack* m_pCB;
	IProtServer* m_pSever;
};

