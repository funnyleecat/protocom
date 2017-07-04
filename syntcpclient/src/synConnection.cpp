#include "synConnection.h"
#include "zip_buffer.h"
namespace SynTcpCli
{
	CSynConnection::CSynConnection(const char *pSzHost, int nPort)
	:m_CommService(new tcp_service)
	, m_CommSocket(new tcp_socket(*m_CommService))
	, m_pProtocolFragmentPtr(new CProtocolFragment)
	, m_strHost(pSzHost)
	, m_nPort(nPort)
	{

	}

	CSynConnection::~CSynConnection() throw()
	{

	}

	bool CSynConnection::Connect()
	{
		try
		{
			tcp_endpoint ep(boost::asio::ip::address::from_string(m_strHost), m_nPort);
			m_CommSocket->connect(ep);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void CSynConnection::DisConnect()
	{
		try
		{
			m_CommSocket->close();
		}
		catch (...)
		{
		}
	}

	bool CSynConnection::Interaction(const char *pIn, size_t dLen, int nFuncId, char *&pOut, size_t &outLen)
	{
		try
		{
			Scopelock guard(m_mutex);
			std::list<TFragmentMessage> fragList;
			if (BuildFragment(pIn, dLen, nFuncId, PROT_REQ_BUSINESS, fragList))
			{
				// 发送分片
				for (list<TFragmentMessage>::iterator it = fragList.begin(); it != fragList.end(); ++it)
				{
					m_CommSocket->write_some(boost::asio::buffer(it->pData, it->nDataLen));
					FreePack(it->pData);
				}

				// 读取返回的数据包
				return ReadFullPack(pOut, outLen);
			}
			return false;
		}
		catch (...)
		{
			return false;
		}
	}

	bool CSynConnection::Heartbeat()
	{
		try{
			Scopelock guard(m_mutex);
			std::list<TFragmentMessage> fragList;
			static std::string emptyStr;
			if (BuildFragment(emptyStr.c_str(), 0, 0, PROT_REQ_KEEPALIVE, fragList))
			{
				// 发送分片
				for (list<TFragmentMessage>::iterator it = fragList.begin(); it != fragList.end(); ++it)
				{
					m_CommSocket->write_some(boost::asio::buffer(it->pData, it->nDataLen));
					FreePack(it->pData);
				}
			}
			return true;
		}
		catch (...)
		{
			return false;
		}		
	}

	void CSynConnection::FreePack(char* out)
	{
		if (NULL != out)
		{
			delete[] out;
		}
	}

	bool CSynConnection::BuildFragment(const char* pSrcData, size_t nSrcDataLen, int nFuncId, PROTO_PACK_TYPE emPackType, std::list<TFragmentMessage>& fragList)
	{
		//是否压缩
		bool bCompress = false;
		//是否加密
		bool bCipher = false;

		bool bRet = true;
		do
		{
			std::size_t dwDataLen = nSrcDataLen;
			char* pBuffer = (char*)pSrcData;
			std::size_t dwBufLen = nSrcDataLen;
			zip_buffer zipbuf;
			if (bCompress)
			{
				dwDataLen = zipbuf.zip((zip_char_t*)pSrcData, (zip_size_t)nSrcDataLen);
				if (dwDataLen < 0)
				{
					bRet = false;
					break;
				}
				pBuffer = (char*)zipbuf.buffer();
			}

			//开辟新内存，包含业务包头，业务数据
			size_t deSendLen = sizeof(PROTOCOL_HEAD)+dwDataLen;
			char* pProtocolBuffer = new char[deSendLen];
			if (pProtocolBuffer == NULL)
			{
				bRet = false;
				break;
			}
			memset(pProtocolBuffer, 0, sizeof(PROTOCOL_HEAD));
			PROTOCOL_HEAD* pSendBuf = (PROTOCOL_HEAD*)pProtocolBuffer;
			pSendBuf->chVersion = PROTOCOL_VERSION;
			pSendBuf->chCompress = bCompress;
			pSendBuf->chCipher = bCipher;
			pSendBuf->nFuncId = nFuncId;
			pSendBuf->chType = emPackType;
			pSendBuf->nSrcLen = (int)dwBufLen;
			pSendBuf->nBodyLen = (int)dwDataLen;
			pSendBuf->nUsrData1 = /*nUserData1*/0;
			pSendBuf->nUsrData2 = /*nUserData2*/0;
			memcpy(pSendBuf->chBodyData, pBuffer, dwDataLen);

			bRet = m_pProtocolFragmentPtr->BuildFragment(pSendBuf, fragList, 0);
			if (NULL != pProtocolBuffer)
			{
				delete[] pProtocolBuffer;
				pProtocolBuffer = NULL;
			}

		} while (0);

		return bRet;
	}

	bool CSynConnection::ReadFullPack(char*& pProtcolHead, size_t& nPackSize)
	{
		char* pSrcProtcolHead = NULL;
		size_t nSrcPackSize = 0;
		bool bRet = false;

		// 读取返回的分片数据包
		size_t nRcvLen = 0;
		do
		{
			// 读取分片包头
			char achFragHead[sizeof(PROTOCOL_FRAG_HEAD)] = { 0 };
			size_t nCurReadLen = 0;
			while (nCurReadLen != sizeof(PROTOCOL_FRAG_HEAD))
			{
				size_t nReadLen = m_CommSocket->read_some(boost::asio::buffer(achFragHead + nCurReadLen, sizeof(PROTOCOL_FRAG_HEAD)-nCurReadLen));
				nCurReadLen += nReadLen;
			}

			PROTOCOL_FRAG_HEAD* pFragHead = (PROTOCOL_FRAG_HEAD*)achFragHead;
			if (pFragHead->nSegSize > MAX_FRAG_DATA_SIZE)
			{
				//cout << "Invalid fragment data." << endl;
				return bRet;
			}
			if (NULL == pSrcProtcolHead)
			{
				nSrcPackSize = pFragHead->nPackSize;
				pSrcProtcolHead = new char[nSrcPackSize];
			}

			// 读取分片数据
			nCurReadLen = 0;
			while (nCurReadLen != pFragHead->nSegSize)
			{
				size_t nReadLen = m_CommSocket->read_some(boost::asio::buffer(pSrcProtcolHead + pFragHead->nOffset + nCurReadLen, pFragHead->nSegSize - nCurReadLen));
				nCurReadLen += nReadLen;
			}
			nRcvLen += pFragHead->nSegSize;

		} while (nRcvLen < nSrcPackSize);

		// 解压协议数据
		char* pDecodeProtcolHead = NULL;
		size_t nDecodeLen = 0;
		if (DecodeFullPack((PROTOCOL_HEAD*)pSrcProtcolHead, nSrcPackSize, pDecodeProtcolHead, nDecodeLen))
		{
			pProtcolHead = pDecodeProtcolHead;
			nPackSize = nDecodeLen;
			bRet = true;
		}

		if (NULL != pSrcProtcolHead)
		{
			delete[] pSrcProtcolHead;
			pSrcProtcolHead = NULL;
		}

		return bRet;
	}

	bool CSynConnection::DecodeFullPack(PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen)
	{
		PROTOCOL_HEAD* pHead = (PROTOCOL_HEAD*)pPackData;
		char* pNewBuff = NULL;
		int nNewLen = 0;

		//包长度不匹配
		if ((pHead->nBodyLen) + sizeof(PROTOCOL_HEAD) != dwDataLen)
		{
			return false;
		}

		nNewLen = (pHead->nSrcLen) + sizeof(PROTOCOL_HEAD);
		pNewBuff = new char[nNewLen];

		//申请内存失败
		if (!pNewBuff)
		{
			return false;
		}

		//是否需要解压
		if (!pHead->chCompress)
		{
			//直接拷贝整个数据包
			memcpy(pNewBuff, pPackData, nNewLen);
			pDecodeData = pNewBuff;
			DecodeLen = nNewLen;
			return true;
		}
		else
		{
			char* pBodyBuff = pNewBuff + sizeof(PROTOCOL_HEAD);
			int nBodyLen = pHead->nBodyLen;

			zip_buffer zipBuf;
			int nzip = zipBuf.unzip((const zip_char_t*)pHead->chBodyData, nBodyLen);
			if (nzip < 0)
			{
				return false;
			}

			//解压之后实际长度与包头长度不一致
			if (nzip != pHead->nSrcLen)
			{
				delete[] pNewBuff;
				pNewBuff = NULL;
				return false;
			}

			//填充协议头
			PROTOCOL_HEAD* pHeadBuff = (PROTOCOL_HEAD*)pNewBuff;
			memset(pHeadBuff, 0, sizeof(PROTOCOL_HEAD));
			memcpy(pHeadBuff, pHead, sizeof(PROTOCOL_HEAD));
			pHeadBuff->chCompress = 0;
			pHeadBuff->nSrcLen = nzip;
			pHeadBuff->nBodyLen = nzip;
			//填充包体
			memcpy(pHeadBuff->chBodyData, zipBuf.buffer(), zipBuf.length());

		}
		pDecodeData = pNewBuff;
		DecodeLen = nNewLen;
		return true;
	}

}
