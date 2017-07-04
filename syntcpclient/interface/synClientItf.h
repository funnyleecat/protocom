/******************************************************************************
模块名  ： syntcpclient
文件名  ： synClientItf.h
相关文件：
文件实现功能：同步TCP连接，封装符合protocom的报文格式
作者    ：lifeng
版本    ：1.0.0.0
******************************************************************************/
#pragma once
#include "export.h"
#include <cstddef>

namespace SynTcpCli
{
	class  SYNTCPCLI_API CSynConnectionItf
	{
		public:
			virtual ~CSynConnectionItf() throw();

			/*
				功能介绍：与服务端一次交互
				入参：
					（1）pIn 发送缓冲器指针
					（2）inLen 发送长度，字节为单位
					（3）nFuncId 发送
				出参：
					（4）pOut 服务端返回数据指针
					（5）outLen 服务器返回数据的长度
				返回：	
					true  发送成功
					false 发送失败
			*/
			virtual bool Interaction(const char *pIn,
				size_t inLen,
				int nFuncId,
				char *&pOut,
				size_t &outLen) = 0;

			/*
				功能介绍：连接服务器调用
				出参：bool 类型
					  true  连接成功
					  false 连接失败
			*/
			virtual bool Connect() = 0;
			/*
				功能介绍：断链服务器调用
			*/
			virtual void DisConnect() = 0;
			
			/*
				功能介绍：向服务器发送心跳报文
				出参：bool 类型
					  true  发送成功
					  false 发送失败	
			*/
			virtual bool Heartbeat() = 0;

			/*
				功能介绍：创建一个同步TCP连接
				入参：
					（1）pSzHost 主机名称字符串
					（2）nPort 端口号
				返回：CSynConnectionItf 的指针
			*/
			static CSynConnectionItf* Create(const char *pSzHost, int nPort);

			/*
				功能介绍：析构一个同步TCP连接
				入参：
				（1）con 析构掉一个连接
			*/
			static void Destory(CSynConnectionItf *&con);

	};
}
