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


#include "ServerImp.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
int main()
{
	CServerImp server;
	//server.Test();
	server.TestMsgHead();
	cout << "servet start!" << endl;
	getchar();
	return 0;
}
