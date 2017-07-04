#pragma once

#include <cstddef>  //for size_t

namespace protcomm
{
    //协议栈可设置选项
    enum E_PROTOCOL_OPT
    {
		OPT_PROTOCOL_LOGGER_HADNLER, //设置日志对象,必填
        OPT_DATA_DELIVERY,        //回调处理对象,数据为回调对象地址，必填
        OPT_CIPHER,               //发送数据是否加密，选项值为0表示不加密，否则表示加密，默认加密,该属性功能还没实现，可以不设置
        OPT_COMPRESS_LIMIT,       //发送数据是否压缩，选项值为0表示不压缩，否则大于等于选项值都压缩，默认值48*1024
        OPT_RCV_FULLPACK,         //响应完整业务包模式,选项值为功能号,默认功能号为0，以OPT_RCV_FULLPACK完整业务包形式接收
        OPT_RCV_FRAGMENT,         //响应分片数据模式，选项值为功能号,默认功能号为0，以OPT_RCV_FRAGMENT分片数据形式接收
        OPT_RCV_MSGHEAD,          //标识接收到的数据是否有PROTOCOL_MSG_HEAD包头（默认无），选项值为0表示没有,1表示有,inbound需要此设置
        OPT_SND_MSGHEAD,          //标识接发送的数据是否有PROTOCOL_MSG_HEAD包头（默认无），选项值为0表示没有,1表示有,inbound需要此设置
		OPT_FRAG_HASH_SIZE,       //分片组包hash表大小(可选,要求2^n,server默认为512，client默认为128)
		OPT_FRAG_TIMEOUT          //分片超时时间(可选,默认为60s)
    };
	//业务收发数据的模式
	enum E_TRANS_MODE
	{
		TRANS_FULLPACK,            //完整业务包处理
		TRANS_FRAGMENT,            //分片业务包数据
	};

#ifdef __cplusplus
    extern "C" {
#endif
    typedef void(*pfSentCallback_t)(char*, size_t, bool);
#ifdef __cplusplus
    }
#endif
}
