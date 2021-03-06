#pragma once

#include "protcomm_export.h"
#include "DataDelivery.h"
#include "protocomm_def.h"

namespace protcomm
{
    class PROTCOMM_API IProtServer
    {
    public:
        /*=========================================================================
        功能  ：获取一个服务户端实例
        参数  ：nIothread：io线程个数；
                nWorkthread：业务线程个数
                nKeepalive：客户端
        返回值：返回实例对象指针
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        static IProtServer* GetInstance(int nIothread, int nWorkthread, int nKeepalive = 30);
        /*=========================================================================
        功能  ：释放一个服务端实例
        参数  ：pInstance：实例对象指针
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        static void Release(IProtServer* pInstance);
        /*=========================================================================
        功能  ：设置协议栈和IO配置
        参数  ：eOpt：配置类型
        dwData：配置数据
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
		virtual size_t SetOpt(E_PROTOCOL_OPT eOpt, size_t dwData) = 0;
        /*=========================================================================
        功能  ：获得参数配置
        参数  ：eOpt：配置类型
        返回值：对应的配置数据
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
		virtual size_t GetOpt(E_PROTOCOL_OPT eOpt)const = 0;
        /*=========================================================================
        功能  ：启动服务器
        参数  ：nPort：服务器监听端口
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        virtual bool Start(unsigned short nPort) = 0;
        /*=========================================================================
        功能  ：停止服务器
        参数  ：
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        virtual void Stop() = 0;
        /*=========================================================================
        功能  ：关闭连接
        参数  ：链接ID
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        virtual void Close(size_t nCtxId) = 0;
        /*=========================================================================
        功能  ：发送数据
        参数  ：nCtxId：目标链接id，
                nFuncId：业务功能号
                ePackType：业务类型
                pData：发送数据起始地址
                dwDataLen：发送数据长度
                tUUID：32个字符的uuid，标识一个最终目标客户端，主动发送数据时填NULL即可，
                       回复数据时回填接收到的uuid。
                nUserData1，nUserData2：用户自定义数据
        回复数据时回填接收到的uuid。
        返回值：
        备注  ：
        日期  ：2016.10 xly
        =========================================================================*/
        virtual bool SendPackageData(size_t nCtxId,
            int nFuncId,
            PROTO_PACK_TYPE ePackType,
            char* pData,
            std::size_t dwDataLen,
            const PROTOCOL_UUID& tUUID,
            std::size_t nCltPackId = 0,
            int nUserData1 = 0,
            int nUserData2 = 0) = 0;
        /*=========================================================================
        功能  ：转发分片数据
        参数  ：nCtxId：目标链接id，
                pMsgData：发送数据起始地址，应该为一个完整的分片数据，包括包头
                dwMsgLen：发送数据长度
                pfbCB：回调函数，在发送结束后的回调，包括发送的缓冲区地址，数据长度，发送结果
        返回值：
        备注  ：直接转发到目的ctx，底层不做任何加工
        日期  ：2016.10 xly
        =========================================================================*/
        virtual bool ForwardFragData(size_t nCtxId,
            char* pData,
            std::size_t dwDataLen,
            pfSentCallback_t pfbCB) = 0;

        virtual ~IProtServer(){};
    protected:
        IProtServer(){};
        IProtServer(IProtServer& rhs);
        IProtServer& operator =(IProtServer& rhs);
    };
}

