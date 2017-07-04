#pragma once

#include <string>
using namespace std;

namespace protcomm
{
    string Vsprintf_t(const char* format, ...);
	string GetHexUUIDStr(string const &str);

#define OUTPUT_DEBUG_LOG(format, ...) if(NULL != m_pLogHandler){ \
	string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
	m_pLogHandler->Debug(tmp.c_str()); \
	}

#define OUTPUT_INFO_LOG(format, ...) if(NULL != m_pLogHandler){ \
	string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
	m_pLogHandler->Info(tmp.c_str()); \
	}

#define OUTPUT_WARN_LOG(format, ...) if(NULL != m_pLogHandler){ \
	string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
	m_pLogHandler->Warn(tmp.c_str()); \
	}

#define OUTPUT_ERROR_LOG(format, ...) if(NULL != m_pLogHandler){ \
	string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
	m_pLogHandler->Error(tmp.c_str()); \
	}

#define OUTPUT_FATAL_LOG(format, ...) if(NULL != m_pLogHandler){ \
	string tmp = Vsprintf_t(format, ## __VA_ARGS__); \
	m_pLogHandler->Fatal(tmp.c_str()); \
	}
}
