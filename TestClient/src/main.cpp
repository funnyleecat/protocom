// TestProcomm.cpp : 定义控制台应用程序的入口点。
//
#ifdef NDEBUG
#ifdef _WIN64
#pragma comment( lib, "../../protcomm/Win64/Release/protcomm.lib")
#else
#pragma comment( lib, "../../protcomm/Win32/Release/protcomm.lib")
#endif
#else
#ifdef _WIN64
#pragma comment( lib, "../../protcomm/Win64/Debug/protcomm.lib")
#else
#pragma comment( lib, "../../protcomm/Win32/Debug/protcomm.lib")
#endif
#endif

#include "ClientImp.h"
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include "ClientCfg.h"
using namespace std;

int main()
{
	if (!ClientCfg::GetInstance().Initialize("cfg.xml"))
	{
		cout << "Init Xml Failed" << endl;
		return -1;
	}

    ClientImp client;
	client.MultConnTest();
	getchar();
	return 0;
}

