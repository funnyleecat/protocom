#pragma once
#include "protocomm_def.h"
#include <map>
#include <boost/thread.hpp>
using namespace std;
namespace protcomm
{
    /*=========================================================================
    ����������¼�ض����ܺŷ���ģʽ�ͽ���ģʽ��������ܺ�0
    �ӿ�  ��
    ��ע  ��
    ����  ��2016.10 xly
    =========================================================================*/
	class CFuncTransMode
    {
        typedef boost::shared_mutex					_RWMutex;
        typedef boost::shared_lock<_RWMutex>	    _Readlock;
        typedef boost::unique_lock<_RWMutex>	    _Writelock;
        struct tagSndRcvMode
        {
            E_TRANS_MODE eSndMode;  //����ģʽ
            E_TRANS_MODE eRcvMode;  //����ģʽ
        };
	public:
		CFuncTransMode();
		~CFuncTransMode();

        void SetRcvMode(int nFuncId, E_TRANS_MODE eMode);
        //void RemoveRcvMode(int nFuncId);
        E_TRANS_MODE GetRcvMode(int nFuncId);

        void SetSndMode(int nFuncId, E_TRANS_MODE eMode);
        //void RemoveSndMode(int nFuncId);
        E_TRANS_MODE GetSndMode(int nFuncId);
    private:
        void SetMode(map<int, E_TRANS_MODE>& tModeMap, int nFuncId, E_TRANS_MODE eMode);
        //void RemoveMode(map<int, E_TRANS_MODE>& tModeMap, int nFuncId);
        E_TRANS_MODE GetMode(const map<int, E_TRANS_MODE>& tModeMap, int nFuncId);
    private:
        map<int, E_TRANS_MODE> m_mapRcvMode;
        map<int, E_TRANS_MODE> m_mapSndMode;
		//_RWMutex m_CtxMutex;
	};
}


