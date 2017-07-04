#ifndef __TCP_PROTOCOL_V3_H_20161205__
#define __TCP_PROTOCOL_V3_H_20161205__

#pragma warning(disable:4996)
#pragma warning(disable:4200)


//һ�ֽڶ���
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

    //ҵ��Э��汾��
    const int PROTOCOL_VERSION = 0x3;
    //��ƬЭ��汾��
    const int SEG_PROTC_VER_1 = 0x1;

    enum PROTO_PACK_TYPE
    {
        PROT_REQ_KEEPALIVE = 0,  //�ͻ��˷�������
        PROT_ACK_KEEPALIVE = 1,  //�������Ӧ����
        PROT_REQ_BUSINESS = 2,   //�ͻ��˷�������
        PROT_ACK_BUSINESS = 3,   //�������Ӧ���� 
        PROT_PUSH_BUSINESS = 5,  //�������������
    };

    //�������ݶ���
    //�������ݰ�һ����Ƭ�������ĳ��ȣ��������ְ�ͷ�����ݵĳ����ܺ�,������buffer����󳤶�
    const int MAX_FRAG_SIZE = 1024 + 1024*64; 
    ////�������ݰ�һ����Ƭ�����ݵ���󳤶ȣ�Ԥ��1k����ͷ
    const int MAX_FRAG_DATA_SIZE = MAX_FRAG_SIZE - 1024;
    //uuid�ĳ���
    const int UUID_LENGTH   = 16;                        

    typedef struct tagProtocolHead
    {
        char    chVersion;      //Э��汾��
        char    chType;         //ҵ������ PROTO_PACK_TYPE
        char    chCipher;       //���ݰ��Ƿ���� ���(��ѹ���������)�����(�Ƚ��ܡ����ѹ)
        char    chCompress;     //���ݰ��Ƿ�ѹ�� 
        int     nFuncId;        //����ҵ��Ĺ��ܺ�
        int     nSrcLen;        //����ԭʼ���� - ���ܡ�ѹ��ǰ����
        int     nBodyLen;       //����ʵ�ʳ��� - ���ܡ�ѹ���󳤶�
        int     nUsrData1;      //�û��Զ�������1
        int     nUsrData2;      //�û��Զ�������2
        char    chBodyData[0];  //���ݶ�,��ϲ�����ϵ�
    }PROTOCOL_HEAD;

    //chType == REQ_KEEPALIVE
    //chType == ACK_KEEPALIVE
    typedef struct tagProtocolKeepalive
    {
        int nSentTime;
    }PROTOCOL_KEEPALVIE;

    //��Ƭ��ͷ
    typedef struct tagProtocolFragHead
    {
        int     nFuncId;        //���ܺš�
        int     nCltPackId;     //�ͻ���Ϊÿ��������һ��Ψһ��Id�������ԭ�����ء�
        int     nSrvPackId;     //�ͻ�����0�������Ϊÿ�����͵İ�����ΨһID
        int     nPackSize;      //��Ƭ�������Ĵ�С
        int     nSegSize;       //��ǰ��Ƭ��С
        int     nOffset;        //��ǰ��Ƭ���������е�ƫ����
        int     nReserve1;      //�����ֶ�
        char    chSegData[0];   //��������
    }PROTOCOL_FRAG_HEAD;

    //uuid�ṹ����
    typedef struct  tagProtocommUUID
    {
        unsigned char tUUID[UUID_LENGTH];     //û�н�����Ŷ
    }PROTOCOL_UUID;
    //��Ϣ�м����ͷ
    typedef struct tagProtocolMsgXHead
    {
        PROTOCOL_UUID   tUUID;          //��ʶΨһ�߼��ͻ��˵�uuid
        char            chTTL;          //������¼
    }PROTOCOL_MSG_HEAD;

#ifdef __cplusplus
}
#endif


#pragma pack()

#endif //__TCP_PROTOCOL_V3_H_20161205__

