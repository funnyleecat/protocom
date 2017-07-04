#include "stdafx.h"
#include "FuncIdTransMode.h"

namespace protcomm
{
	CFuncTransMode::CFuncTransMode()
	{
        m_mapRcvMode[0] = /*E_TRANS_MODE::*/TRANS_FULLPACK;
        m_mapSndMode[0] = /*E_TRANS_MODE::*/TRANS_FRAGMENT;
	}


	CFuncTransMode::~CFuncTransMode()
	{
	}


    void CFuncTransMode::SetRcvMode(int nFuncId, E_TRANS_MODE eMode)
    {
        return SetMode(m_mapRcvMode, nFuncId, eMode);
    }

    E_TRANS_MODE CFuncTransMode::GetRcvMode(int nFuncId)
    {
        return GetMode(m_mapRcvMode, nFuncId);
    }

    void CFuncTransMode::SetSndMode(int nFuncId, E_TRANS_MODE eMode)
    {
        return SetMode(m_mapSndMode, nFuncId, eMode);
    }

    E_TRANS_MODE CFuncTransMode::GetSndMode(int nFuncId)
    {
        return GetMode(m_mapSndMode, nFuncId);
    }
    void CFuncTransMode::SetMode(map<int, E_TRANS_MODE>& tModeMap, int nFuncId, E_TRANS_MODE eMode)
    {
        m_mapRcvMode[nFuncId] = eMode;
    }

    E_TRANS_MODE CFuncTransMode::GetMode(const map<int, E_TRANS_MODE>& tModeMap, int nFuncId)
    {
		map<int, E_TRANS_MODE>::const_iterator iteFuncid = tModeMap.find(nFuncId);
		if (iteFuncid != tModeMap.end())
        {
            return iteFuncid->second;
        }
        //认为一定能找到0，构造函数中一定添加0
        return tModeMap.find(0)->second;
    }
}
