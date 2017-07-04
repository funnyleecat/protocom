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

	//����������key����
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

	//�ͻ��������key����
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

    //��ʱʱ��
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

	//���ά������
	template<typename T>
	struct TFragCacheInfo
	{
		char* pPackBuf;  //���������
		int   nPackLen;  //����
		boost::atomic_int   nRcvLen; //�ѽ��յĳ���
        unsigned long dwActvTime;    //����Ծʱ��

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

		//���һ����Ƭ���������true��ʾ�Ѿ�����һ��������tPackData�����򷵻�false
		bool ComposeFrag(const T& SessKey, const PROTOCOL_FRAG_HEAD* pFragData,TPackData& tPackData)
		{
			assert(pFragData != NULL);
			if (pFragData->nSegSize > MAX_FRAG_DATA_SIZE)
			{
				return false;
			}

			//���һ����Ƭ���������İ����Ǿͱ�������ʲô����
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
                if (NULL != FragPtr->pPackBuf)//���Ǳ���ļ��
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

        //��鳬ʱ
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
		//�����ڴ棬����ɶҲ���ɣ�ֻ��Ϊ�˽����ڴ�ر���
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
		//�������ڲ������ĸ���
		CPackSession(CLogHandler*& pLogHandler, std::size_t dwInTableNum = 128 /*Ҫ��2^n*/, std::size_t dwKeepSec = 30) :
			ILogHandler(pLogHandler),
			m_dwInTableNum(dwInTableNum),
			m_dwKeepSec(dwKeepSec)
		{
			m_vecInTable.reserve(m_dwInTableNum);
			for (std::size_t count = 0; count < dwInTableNum; ++count)
			{
				m_vecInTable.push_back(INNERSESS_PTR(new CInnerPackSession<T>(m_pLogHandler, m_Clock)));
			}
            //5����һ�Σ��Ǻ�
            m_Clock.GetTimerService().AddTimer(boost::posix_time::seconds(5),
                boost::bind(&CPackSession<T>::CheckTimeout, this));
		}

		//���һ����Ƭ���������true��ʾ�Ѿ�����һ��������tPackData�����򷵻�false��
		bool ComposeFrag(const T& SessKey, const PROTOCOL_FRAG_HEAD* pFragData, TPackData& tPackData)
		{
			std::size_t dwIndex = GetIndex(SessKey);
			return m_vecInTable[dwIndex]->ComposeFrag(SessKey, pFragData, tPackData);
		}

	private:
		//���key��vector�ж�Ӧ������λ��
		std::size_t GetIndex(const CSrvPackKey& SrvKey)
		{
			return SrvKey.m_nCltPackId & (m_dwInTableNum - 1);//��һ�ѣ�Ҫ���ĸ�����2��n�η���������������ȡ�����
		}

		std::size_t GetIndex(const CCltPackKey& CltKey)
		{
			return CltKey.m_nSrvPackId & (m_dwInTableNum - 1);
		}

        //��鳬ʱ
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
