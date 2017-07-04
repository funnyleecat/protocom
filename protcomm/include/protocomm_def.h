#pragma once

#include <cstddef>  //for size_t

namespace protcomm
{
    //Э��ջ������ѡ��
    enum E_PROTOCOL_OPT
    {
		OPT_PROTOCOL_LOGGER_HADNLER, //������־����,����
        OPT_DATA_DELIVERY,        //�ص��������,����Ϊ�ص������ַ������
        OPT_CIPHER,               //���������Ƿ���ܣ�ѡ��ֵΪ0��ʾ�����ܣ������ʾ���ܣ�Ĭ�ϼ���,�����Թ��ܻ�ûʵ�֣����Բ�����
        OPT_COMPRESS_LIMIT,       //���������Ƿ�ѹ����ѡ��ֵΪ0��ʾ��ѹ����������ڵ���ѡ��ֵ��ѹ����Ĭ��ֵ48*1024
        OPT_RCV_FULLPACK,         //��Ӧ����ҵ���ģʽ,ѡ��ֵΪ���ܺ�,Ĭ�Ϲ��ܺ�Ϊ0����OPT_RCV_FULLPACK����ҵ�����ʽ����
        OPT_RCV_FRAGMENT,         //��Ӧ��Ƭ����ģʽ��ѡ��ֵΪ���ܺ�,Ĭ�Ϲ��ܺ�Ϊ0����OPT_RCV_FRAGMENT��Ƭ������ʽ����
        OPT_RCV_MSGHEAD,          //��ʶ���յ��������Ƿ���PROTOCOL_MSG_HEAD��ͷ��Ĭ���ޣ���ѡ��ֵΪ0��ʾû��,1��ʾ��,inbound��Ҫ������
        OPT_SND_MSGHEAD,          //��ʶ�ӷ��͵������Ƿ���PROTOCOL_MSG_HEAD��ͷ��Ĭ���ޣ���ѡ��ֵΪ0��ʾû��,1��ʾ��,inbound��Ҫ������
		OPT_FRAG_HASH_SIZE,       //��Ƭ���hash���С(��ѡ,Ҫ��2^n,serverĬ��Ϊ512��clientĬ��Ϊ128)
		OPT_FRAG_TIMEOUT          //��Ƭ��ʱʱ��(��ѡ,Ĭ��Ϊ60s)
    };
	//ҵ���շ����ݵ�ģʽ
	enum E_TRANS_MODE
	{
		TRANS_FULLPACK,            //����ҵ�������
		TRANS_FRAGMENT,            //��Ƭҵ�������
	};

#ifdef __cplusplus
    extern "C" {
#endif
    typedef void(*pfSentCallback_t)(char*, size_t, bool);
#ifdef __cplusplus
    }
#endif
}
