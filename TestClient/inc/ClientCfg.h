#pragma once
#include <Singleton.hpp>
#include <string>
#include <vector>
using namespace std;
typedef struct FCUNCIDRANGE
{
	int Min;
	int Max;
}FuncIdRange;
typedef struct TAGCLIENT
{
	std::string ip;
	int port;
	int ht_timeout;
	int io_threads;
	int work_threads;
	vector<FuncIdRange> vFuncId;
	int CirculNum;
}ClientInfo;
class ClientCfg :public CSingleton<ClientCfg>
{
public:
	ClientCfg();
	~ClientCfg();
	bool ClientCfg::Initialize(const string& strFilePath);
	ClientInfo m_client;
	int nStringNum;
private:
	bool ReadClientInfo(void* lp, ClientInfo& info);
};

