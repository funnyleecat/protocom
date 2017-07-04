#pragma once
#include "synClientItf.h"
#include <boost/asio.hpp>
#include <protocolFragment.h>
#include <boost/thread/mutex.hpp>
namespace SynTcpCli
{
	typedef boost::asio::io_service			tcp_service;
	typedef boost::shared_ptr<tcp_service>	tcp_service_ptr;

	typedef boost::asio::ip::tcp::socket	tcp_socket;
	typedef boost::shared_ptr<tcp_socket>	tcp_socket_ptr;

	typedef boost::asio::ip::tcp::endpoint	tcp_endpoint;
	typedef boost::shared_ptr<tcp_endpoint>	tcp_endpoint_ptr;

	typedef boost::mutex::scoped_lock Scopelock;

	class CSynConnection : public CSynConnectionItf
	{
	public:
		CSynConnection(const char *pSzHost, int nPort);
		~CSynConnection() throw();
		virtual bool Interaction(const char *pIn, 
								size_t inLen, 
								int nFuncId, 
								char *&pOut,
								size_t &outLen);
		virtual bool Connect();
		virtual void DisConnect();
		virtual bool Heartbeat();
	protected:
		bool ReadFullPack(char *&pProtcolHead, size_t& nPackSize);
		bool BuildFragment(const char* pSrcData, size_t nSrcDataLen, int nFuncId, PROTO_PACK_TYPE emPackType, std::list<TFragmentMessage>& fragList);
		void FreePack(char* out);
		bool DecodeFullPack(PROTOCOL_HEAD* pPackData, std::size_t dwDataLen, char*& pDecodeData, std::size_t& DecodeLen);
	protected:
		boost::shared_ptr<CProtocolFragment> m_pProtocolFragmentPtr;
		tcp_service_ptr m_CommService;
		tcp_socket_ptr	m_CommSocket;
		std::string m_strHost;
		int m_nPort;
		boost::mutex m_mutex;
	};

}

