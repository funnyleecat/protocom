/******************************************************************************
ģ����  �� syntcpclient
�ļ���  �� synClientItf.h
����ļ���
�ļ�ʵ�ֹ��ܣ�ͬ��TCP���ӣ���װ����protocom�ı��ĸ�ʽ
����    ��lifeng
�汾    ��1.0.0.0
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
				���ܽ��ܣ�������һ�ν���
				��Σ�
					��1��pIn ���ͻ�����ָ��
					��2��inLen ���ͳ��ȣ��ֽ�Ϊ��λ
					��3��nFuncId ����
				���Σ�
					��4��pOut ����˷�������ָ��
					��5��outLen �������������ݵĳ���
				���أ�	
					true  ���ͳɹ�
					false ����ʧ��
			*/
			virtual bool Interaction(const char *pIn,
				size_t inLen,
				int nFuncId,
				char *&pOut,
				size_t &outLen) = 0;

			/*
				���ܽ��ܣ����ӷ���������
				���Σ�bool ����
					  true  ���ӳɹ�
					  false ����ʧ��
			*/
			virtual bool Connect() = 0;
			/*
				���ܽ��ܣ���������������
			*/
			virtual void DisConnect() = 0;
			
			/*
				���ܽ��ܣ��������������������
				���Σ�bool ����
					  true  ���ͳɹ�
					  false ����ʧ��	
			*/
			virtual bool Heartbeat() = 0;

			/*
				���ܽ��ܣ�����һ��ͬ��TCP����
				��Σ�
					��1��pSzHost ���������ַ���
					��2��nPort �˿ں�
				���أ�CSynConnectionItf ��ָ��
			*/
			static CSynConnectionItf* Create(const char *pSzHost, int nPort);

			/*
				���ܽ��ܣ�����һ��ͬ��TCP����
				��Σ�
				��1��con ������һ������
			*/
			static void Destory(CSynConnectionItf *&con);

	};
}
