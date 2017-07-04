#include "synClientItf.h"
#include "synConnection.h"
namespace SynTcpCli
{
	CSynConnectionItf::~CSynConnectionItf() throw()
	{

	}

	CSynConnectionItf* CSynConnectionItf::Create(const char *pSzHost, int nPort)
	{
		return new CSynConnection(pSzHost, nPort);
	}

	void CSynConnectionItf::Destory(CSynConnectionItf *&con)
	{
		delete con;
		con = NULL;
	}
}
