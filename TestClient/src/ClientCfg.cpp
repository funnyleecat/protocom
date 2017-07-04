#include "ClientCfg.h"
#include <StringFunc.hpp>
#include "../../../../lib/cross/rapidxml/rapidxml.hpp"
#include "../../../../lib/cross/rapidxml/rapidxml_utils.hpp"
#include "../../../../lib/cross/rapidxml/rapidxml_print.hpp"
using namespace rapidxml;

#ifdef WIN32
#   include <atlstr.h> 
#else
#   include <unistd.h>
#endif
//获取当前路径
bool GetCwd(string &strCwd)
{
	char szCwd[512] = { 0 };
#ifdef WIN32
	GetModuleFileNameA(NULL, szCwd, 512);
	char* pTmp = szCwd + (strlen(szCwd) - 1);

	while ((pTmp != szCwd) && (*pTmp != '\\'))
	{
		*pTmp = '\0';
		pTmp--;
	}
#else
	//获取当前目录绝对路径
	if (NULL == getcwd(szCwd, 512))
		return false;

	szCwd[strlen(szCwd)] = '/';
#endif
	strCwd = szCwd;
	return !strCwd.empty();
}
ClientCfg::ClientCfg()
{
}


ClientCfg::~ClientCfg()
{
}


bool ClientCfg::Initialize(const string& strFilePath)
{
	string sPath;
	if (!GetCwd(sPath))
		return false;
	sPath = sPath + strFilePath;
	bool bFlag = false;
	char buf[256] = { 0 };
	do
	{
		//解析xml
		try
		{
		file<> fdoc(sPath.c_str());
		xml_document<>   doc;

			doc.parse<0>(fdoc.data());
			xml_node<>* root = doc.first_node();
			//如果需要加配置，调用的位置从此处结束----嘎嘎嘎
			for (root; root; root = root->next_sibling())
			{
				string nodename = root->name();
				xml_node<>* node = root->first_node();
				if (nodename == "Client")
				{
					if (ReadClientInfo(root,m_client) != true)
						break;
				}
			}
		}
		catch (parse_error e)
		{
			break;;
		}
		bFlag = true;
	} while (0);
	return bFlag;
}
const char* GetfirstAttribute(xml_node<>* node, const char *pAttr)
{
	char *p = NULL;
	xml_attribute<char> *pAttr1 = node->first_attribute(pAttr);;
	if (pAttr1 == NULL)
		return "";
	else
		p = node->first_attribute(pAttr)->value();
	return p;
}
bool ClientCfg::ReadClientInfo(void* lp, ClientInfo& info)
{
	xml_node<>* node_chlid = (xml_node<>*)(lp);
	xml_node<>* node = node_chlid->first_node();
	for (node; node; node = node->next_sibling())
	{
		string nodename = node->name();
		if (nodename == "ip")
		{
			info.ip = node->value();
		}
		if (nodename == "port")
		{
			info.port = StringUtil::lexical_cast(node->value(), 0);
		}
		if (nodename == "ht_timeout")
		{
			info.ht_timeout = StringUtil::lexical_cast(node->value(), 0);
		}
		if (nodename == "io_threads")
		{
			info.io_threads = StringUtil::lexical_cast(node->value(), 10);
		}
		if (nodename == "work_threads")
		{
			info.work_threads = StringUtil::lexical_cast(node->value(), 10);
		}
		if (nodename == "FuncId")
		{
			xml_node<>* nodeNext = node->first_node();
			while (nodeNext != NULL)
			{
				FuncIdRange FunRan;
				FunRan.Min = atoi(GetfirstAttribute(nodeNext, "Min"));
				FunRan.Max = atoi(GetfirstAttribute(nodeNext, "Max"));
				info.vFuncId.push_back(FunRan);
				nodeNext = nodeNext->next_sibling();
			}
		}
		if (nodename == "CirculNum")
		{
			info.CirculNum = StringUtil::lexical_cast(node->value(), 10);
		}
		if (nodename == "StringNum")
		{
			nStringNum = StringUtil::lexical_cast(node->value(), 10);
		}
	}
	return true;
}