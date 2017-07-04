#ifndef __TCP_PROTOCOL_V3_H_20161205__
#define __TCP_PROTOCOL_V3_H_20161205__

#pragma warning(disable:4996)
#pragma warning(disable:4200)


//一字节对齐
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

    //业务协议版本号
    const int PROTOCOL_VERSION = 0x3;
    //分片协议版本号
    const int SEG_PROTC_VER_1 = 0x1;

    enum PROTO_PACK_TYPE
    {
        PROT_REQ_KEEPALIVE = 0,  //客户端发送心跳
        PROT_ACK_KEEPALIVE = 1,  //服务端响应心跳
        PROT_REQ_BUSINESS = 2,   //客户端发送请求
        PROT_ACK_BUSINESS = 3,   //服务端响应请求 
        PROT_PUSH_BUSINESS = 5,  //服务端主动推送
    };

    //常量数据定义
    //定义数据包一个分片缓冲区的长度，包括各种包头和数据的长度总和,即接收buffer的最大长度
    const int MAX_FRAG_SIZE = 1024 + 1024*64; 
    ////定义数据包一个分片的数据的最大长度，预留1k给包头
    const int MAX_FRAG_DATA_SIZE = MAX_FRAG_SIZE - 1024;
    //uuid的长度
    const int UUID_LENGTH   = 16;                        

    typedef struct tagProtocolHead
    {
        char    chVersion;      //协议版本号
        char    chType;         //业务类型 PROTO_PACK_TYPE
        char    chCipher;       //数据包是否加密 封包(先压缩、后加密)、解包(先解密、后解压)
        char    chCompress;     //数据包是否压缩 
        int     nFuncId;        //请求业务的功能号
        int     nSrcLen;        //包体原始长度 - 加密、压缩前长度
        int     nBodyLen;       //包体实际长度 - 加密、压缩后长度
        int     nUsrData1;      //用户自定义数据1
        int     nUsrData2;      //用户自定义数据2
        char    chBodyData[0];  //数据段,不喜欢酱紫的
    }PROTOCOL_HEAD;

    //chType == REQ_KEEPALIVE
    //chType == ACK_KEEPALIVE
    typedef struct tagProtocolKeepalive
    {
        int nSentTime;
    }PROTOCOL_KEEPALVIE;

    //分片包头
    typedef struct tagProtocolFragHead
    {
        int     nFuncId;        //功能号。
        int     nCltPackId;     //客户端为每个包生成一个唯一的Id，服务端原样返回。
        int     nSrvPackId;     //客户端填0，服务端为每个发送的包生成唯一ID
        int     nPackSize;      //分片所属包的大小
        int     nSegSize;       //当前分片大小
        int     nOffset;        //当前分片在所属包中的偏移量
        int     nReserve1;      //保留字段
        char    chSegData[0];   //返回数据
    }PROTOCOL_FRAG_HEAD;

    //uuid结构定义
    typedef struct  tagProtocommUUID
    {
        unsigned char tUUID[UUID_LENGTH];     //没有结束符哦
    }PROTOCOL_UUID;
    //消息中间件包头
    typedef struct tagProtocolMsgXHead
    {
        PROTOCOL_UUID   tUUID;          //标识唯一逻辑客户端的uuid
        char            chTTL;          //跳数记录
    }PROTOCOL_MSG_HEAD;

#ifdef __cplusplus
}
#endif


#pragma pack()

#endif //__TCP_PROTOCOL_V3_H_20161205__

