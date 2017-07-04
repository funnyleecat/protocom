#pragma once
#include "ILoggerHandler.h"
#include "DataDelivery.h"
namespace protcomm
{
    class CLogHandler :tcpcomm2::ILoggerHandler
    {
    public:
        CLogHandler(CLogOutput* pLog) :m_pOutput(pLog)
        {

        }
        virtual void Debug(const char* pszString)
        {
            m_pOutput->Debug(pszString);
        }
        virtual void Info(const char* pszString)
        {
            m_pOutput->Info(pszString);
        }
        virtual void Warn(const char* pszString)
        {
            m_pOutput->Warn(pszString);
        }
        virtual void Error(const char* pszString)
        {
            m_pOutput->Error(pszString);
        }
        virtual void Fatal(const char* pszString)
        {
            m_pOutput->Fatal(pszString);
        }
    public:
        CLogOutput*  m_pOutput;
    };

	class ILogHandler
	{
	public:
		ILogHandler(CLogHandler*& pLogHandler) : m_pLogHandler(pLogHandler){};
		virtual ~ILogHandler(){};

	protected:
		CLogHandler*& m_pLogHandler;
		ILogHandler(ILogHandler& chs);
		ILogHandler& operator =(ILogHandler& chs);
	};
}
