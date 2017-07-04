#pragma once

#include "stdafx.h"
#include <vector>
#include <map>
#include <boost/atomic/atomic.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include "Protocol.h"
#include "Utility.h"
#include "EasyTimer.hpp"

namespace protcomm
{
	typedef boost::lock_guard<boost::mutex> DO_LOCK;

	typedef struct tagTPackData TPackData;

	template<typename T> class CInnerPackSession;

	//服务端组包的key类型
	class CSrvPackKey
	{
	public:
		CSrvPackKey() : m_nCltPackId(0)
		{
			memset(m_tUUID.tUUID, 0, UUID_LENGTH);
		};

		CSrvPackKey(const PROTOCOL_UUID& uuid, int nPackId) : m_tUUID(uuid), m_nCltPackId(nPackId)
		{
		}

		bool operator< (const CSrvPackKey& other) const
		{
			int nRet = memcmp(m_tUUID.tUUID, other.m_tUUID.tUUID, sizeof(m_tUUID.tUUID));
			if (nRet < 0)
			{
				return true;
			}
			else if (nRet == 0 && m_nCltPackId < other.m_nCltPackId)
			{
				return true;
			}
			return false;
		}

		string GetKey() const
		{
			std::string strOrigUUID((char*)m_tUUID.tUUID, UUID_LENGTH);

			return Vsprintf_t("SrvPackKey, (UUID : nCltPackId): (%s : %d)", GetHexUUIDStr(strOrigUUID).c_str(), m_nCltPackId);
		}

	public:
		PROTOCOL_UUID   m_tUUID;
		int             m_nCltPackId;
	};

	//客户端组包的key类型
	class CCltPackKey
	{
	public:
		CCltPackKey() : m_nCltPackId(0), m_nSrvPackId(0)
		{
		}

		CCltPackKey(int nCltPack, int nSrvId) :m_nCltPackId(nCltPack), m_nSrvPackId(nSrvId)
		{
		}

		bool operator<(const CCltPackKey& other)const
		{
			if (m_nCltPackId < other.m_nCltPackId)
			{
				return true;
			}
			else if (m_nCltPackId == other.m_nCltPackId && m_nSrvPackId < other.m_nSrvPackId)
			{
				return true;
			}
			return false;
		}

		string GetKey() const
		{
			return Vsprintf_t("CltPackKey, (nCltPackId : nSrvPackId): (%d : %d)", m_nCltPackId, m_nSrvPackId);
		}

	public:
		int   m_nCltPackId;
		int   m_nSrvPackId;
	};

    //超时时钟
    class CTimeoutClock
    {
    public:
        CTimeoutClock() :m_dwSecPointer(0)
        {
            m_timerSrv.AddTimer(boost::posix_time::seconds(1),boost::bind(&CTimeoutClock::NextSec, this));
        }

        TimerService& GetTimerService()
        {
            return m_timerSrv;
        }

        unsigned long GetSec() const
        {
            return m_dwSecPointer;
        }

    private:
        void NextSec()
        {
            ++m_dwSecPointer;
        }

    private:
        TimerService    m_timerSrv;
        unsigned long   m_dwSecPointer;
    };

	//组包维护数据
	template<typename T>
	struct TFragCacheInfo
	{
		char* pPackBuf;  //组包缓冲区
		int   nPackLen;  //包长
		boost::atomic_int   nRcvLen; //已接收的长度
        unsigned long dwActvTime;    //最后活跃时间

	public:
		TFragCacheInfo()
		{
			pPackBuf = NULL;
			nRcvLen = 0;
			nPackLen = 0;
            dwActvTime = 0;
		}

		~TFragCacheInfo()
		{
			if (pPackBuf)
			{
				delete[] pPackBuf;
				pPackBuf = NULL;
			}
		}
	};

	template<typename T>
	class CInnerPackSession : public ILogHandler
	{
	public:
		typedef typename boost::shared_ptr< TFragCacheInfo<T> >      FRAGINFO_PTR;
	public:
		CInnerPackSession(CLogHandler*& pLogHandler, CTimeoutClock& Clock) :
			ILogHandler(pLogHandler),
			m_Clock(Clock)
        {

        }

		//添加一个分片，如果返回true表示已经产生一个完整包tPackData，否则返回false
		bool ComposeFrag(const T& SessKey, const PROTOCOL_FRAG_HEAD* pFragData,TPackData& tPackData)
		{
			assert(pFragData != NULL);
			if (pFragData->nSegSize > MAX_FRAG_DATA_SIZE)
			{
				return false;
			}

			//如果一个分片就是完整的包，那就别整加锁什么鬼了
			if (pFragData->nSegSize >= pFragData->nPackSize)
			{
				assert(pFragData->nOffset == 0);
				tPackData.pData = GetBuffer(pFragData->nPackSize);
				tPackData.dwDataLen = pFragData->nPackSize;
				memcpy(tPackData.pData, pFragData->chSegData, pFragData->nPackSize);
				return true;
			}

			FRAGINFO_PTR FragPtr;
			{
				DO_LOCK lock(m_Mutex);

				pair<TABLE_IT, bool> ret;
				ret = m_PackTable.insert(pair<T, FRAGINFO_PTR>(SessKey, FRAGINFO_PTR(new TFragCacheInfo<T>)));
				FragPtr = ret.first->second;
				if (ret.second)
				{
					FragPtr->nPackLen = pFragData->nPackSize;
					FragPtr->pPackBuf = GetBuffer(FragPtr->nPackLen);
					FragPtr->dwActvTime = m_Clock.GetSec();
				}
			}
			assert(FragPtr->nPackLen == pFragData->nPackSize);
			assert(FragPtr->nRcvLen <= FragPtr->nPackLen);
			memcpy(FragPtr->pPackBuf + pFragData->nOffset, pFragData->chSegData, pFragData->nSegSize);
			FragPtr->nRcvLen += pFragData->nSegSize;

			if (FragPtr->nRcvLen >= FragPtr->nPackLen)
			{
				DO_LOCK lock(m_Mutex);
                if (NULL != FragPtr->pPackBuf)//不是必须的检查
                {
                    tPackData.pData = FragPtr->pPackBuf;
                    tPackData.dwDataLen = FragPtr->nRcvLen;
				    FragPtr->pPackBuf = NULL;
				    m_PackTable.erase(SessKey);
				    return true;
                }
			}
            FragPtr->dwActvTime = m_Clock.GetSec();
			return false;
		}

        //检查超时
        void CheckTimeout(unsigned long dwTimeout)
        {
			DO_LOCK lock(m_Mutex);

			typename std::map<T, FRAGINFO_PTR>::iterator it;
			for (it = m_PackTable.begin(); it != m_PackTable.end(); )
			{
				if (m_Clock.GetSec() - it->second->dwActvTime > dwTimeout)
				{
					OUTPUT_INFO_LOG("[protcomm][CInnerPackSession::CheckTimeout] Fragment timeout, %s, nPackLen: %d, nRcvLen: %d, dwTimeout: %d s.", 
						it->first.GetKey().c_str(), it->second->nPackLen, (int)it->second->nRcvLen, dwTimeout);
					m_PackTable.erase(it++);
				}
				else
				{
					it++;
				}
			}
        }

	private:
		//分配内存，现在啥也不干，只是为了将来内存池备用
		char* GetBuffer(std::size_t dwSize)
		{
			return new char[dwSize];
		}

	private:
		typedef typename std::map<T, FRAGINFO_PTR>	TABLE_T;
		typedef typename TABLE_T::iterator          TABLE_IT;
        CTimeoutClock&   m_Clock;
		TABLE_T          m_PackTable;
		boost::mutex     m_Mutex;
	};

	template<typename T>
	class CPackSession : public ILogHandler
	{
	public:
		//参数，内部缓存表的个数
		CPackSession(CLogHandler*& pLogHandler, std::size_t dwInTableNum = 128 /*要求2^n*/, std::size_t dwKeepSec = 30) :
			ILogHandler(pLogHandler),
			m_dwInTableNum(dwInTableNum),
			m_dwKeepSec(dwKeepSec)
		{
			m_vecInTable.reserve(m_dwInTableNum);
			for (std::size_t count = 0; count < dwInTableNum; ++count)
			{
				m_vecInTable.push_back(INNERSESS_PTR(new CInnerPackSession<T>(m_pLogHandler, m_Clock)));
			}
            //5秒检查一次，呵呵
            m_Clock.GetTimerService().AddTimer(boost::posix_time::seconds(5),
                boost::bind(&CPackSession<T>::CheckTimeout, this));
		}

		//添加一个分片，如果返回true表示已经产生一个完整包tPackData，否则返回false，
		bool ComposeFrag(const T& SessKey, const PROTOCOL_FRAG_HEAD* pFragData, TPackData& tPackData)
		{
			std::size_t dwIndex = GetIndex(SessKey);
			return m_vecInTable[dwIndex]->ComposeFrag(SessKey, pFragData, tPackData);
		}

	private:
		//获得key在vector中对应的索引位置
		std::size_t GetIndex(const CSrvPackKey& SrvKey)
		{
			return SrvKey.m_nCltPackId & (m_dwInTableNum - 1);//浪一把，要求表的个数是2的n次方，江湖传言这样取余更快
		}

		std::size_t GetIndex(const CCltPackKey& CltKey)
		{
			return CltKey.m_nSrvPackId & (m_dwInTableNum - 1);
		}

        //检查超时
        void CheckTimeout()
        {
            for (std::size_t dwIdx = 0; dwIdx < m_vecInTable.size();++dwIdx)
            {
                m_vecInTable[dwIdx]->CheckTimeout((unsigned long)m_dwKeepSec);
            }
        }

	private:
		typedef typename boost::shared_ptr<CInnerPackSession<T> >   INNERSESS_PTR;

		const std::size_t                       m_dwInTableNum;
		typename std::vector<INNERSESS_PTR >    m_vecInTable;
        CTimeoutClock                           m_Clock;
		const std::size_t m_dwKeepSec;
	};
}
